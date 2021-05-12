#include "../include/main.h"
#include "../include/Version.h"
#include "../include/dnnetwork.h"
#include "../include/image_opencv.h"
#include "../include/ini.h"
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <string.h>

// -- global variables ---
static volatile int g_exit_signal = 0;
// ------------------------

int
main(int argc, char** argv)
{
	int debug, show_frame;
	struct dnnetwork dnnet;
	char video_stream_source_name[2048];
	char video_stream_output_name[2048];
	cv::VideoCapture cap;
	cv::VideoWriter video_writer;
	cv::Mat frame;
	char key;

	fprintf(stdout, "%s(): dvmot started, version: [%s] .\n", __func__, Version);
	if (argc != 2)
	{
		fprintf(stderr, "%s(): Usage [./dvmot dvmot.ini] .\n", __func__);
		return -1;
	}

	add_exit_signals();

	if (read_ini_file(argv[1], &debug, &show_frame, &dnnet, video_stream_source_name, video_stream_output_name) != 0)
	{
		return -1;
	}

	if (open_video_stream(video_stream_source_name, cap) != 0)
	{
		return -1;
	}
	// get first frame
	if (!cap.read(frame))
	{
		return -1;
	}

	if (create_video_writer(video_writer, video_stream_output_name, 'H', '2', '6', '4', get_stream_fps(cap), get_width_mat(frame), get_height_mat(frame), 1) != 0)
	{
		return -1;
	}

	if (load_dnnetwork(&dnnet) != 0)
	{
		return -1;
	}

	while (1)
	{
		if (!cap.read(frame))
		{
			fprintf(stderr, "%s(): Frame reading error. \n", __func__);
			break;
		}
		if (frame.empty())
		{
			fprintf(stdout, "%s(): Frame empty, end of the video file. \n", __func__);
			break;
		}

		predict_image(&dnnet, frame);

		write_frame_in_video(video_writer, frame);

		if (show_frame)
		{
			cv::imshow("dvmot", frame);
		}

		key = cv::waitKey(10);
		if (g_exit_signal || key == 27)
		{
			break;
		}
	}

	release_video_writer(video_writer);
	close_network(&dnnet);
	fprintf(stdout, "%s(): bye.. :) \n", __func__);

	return 0;
}

void
exit_signal(int signal)
{
	fprintf(stdout, "%s(): Exit signal: [%d] .\n", __func__, signal);
	g_exit_signal = signal;
}

void
add_exit_signals(void)
{
	(void)signal(SIGINT, exit_signal);
	(void)signal(SIGTERM, exit_signal);
	(void)signal(SIGHUP, exit_signal);
	(void)signal(SIGQUIT, exit_signal);
}

int
read_ini_file(const char* file_name, int* debug, int* show_frame, struct dnnetwork* dnnet, char* video_stream_source_name, char* video_stream_output_name)
{
	ini_t* config = NULL;

	config = ini_load(file_name);
	if (config == NULL)
	{
		fprintf(stderr, "%s(): Ini file read err: [%s] .\n", __func__, file_name);
		return -1;
	}

	*debug = atoi(ini_get(config, "app", "debug"));
	fprintf(stdout, "%s(): debug: [%d] .\n", __func__, *debug);

	*show_frame = atoi(ini_get(config, "app", "show_frame"));
	fprintf(stdout, "%s(): show_frame: [%d] .\n", __func__, *show_frame);

	dnnet->detect_thres = atoi(ini_get(config, "dnnetwork", "detect_thres"));
	fprintf(stdout, "%s(): detect_thres: [%d] .\n", __func__, dnnet->detect_thres);

	strcpy(dnnet->obj_names_file, ini_get(config, "dnnetwork", "obj_names_file"));
	fprintf(stdout, "%s(): obj_names_file: [%s] .\n", __func__, dnnet->obj_names_file);

	strcpy(dnnet->conf_file, ini_get(config, "dnnetwork", "conf_file"));
	fprintf(stdout, "%s(): conf_file: [%s] .\n", __func__, dnnet->conf_file);

	strcpy(dnnet->weights_file, ini_get(config, "dnnetwork", "weights_file"));
	fprintf(stdout, "%s(): weights_file: [%s] .\n", __func__, dnnet->weights_file);

	dnnet->gpu_id = atoi(ini_get(config, "dnnetwork", "gpu_id"));
	fprintf(stdout, "%s(): gpu_id: [%d] .\n", __func__, dnnet->gpu_id);

	strcpy(video_stream_source_name, ini_get(config, "video_stream", "source_name"));
	fprintf(stdout, "%s(): video_stream_source_name: [%s] .\n", __func__, video_stream_source_name);

	strcpy(video_stream_output_name, ini_get(config, "video_stream", "output_name"));
	fprintf(stdout, "%s(): video_stream_output_name: [%s] .\n", __func__, video_stream_output_name);

	ini_free(config);

	return 0;
}