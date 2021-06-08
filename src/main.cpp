#include "../include/main.h"
#include "../include/Version.h"
#include "../include/dnnetwork.h"
#include "../include/image_opencv.h"
#include "../include/ini.h"
#include "../include/social_distance.h"
#include "../include/tracks.h"
#include "../include/utils.h"
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
	bool app_debug, app_show_frame;
	struct dnnetwork dnnet;
	char video_stream_source_name[2048];
	char video_stream_output_name[2048];
	cv::VideoCapture cap;
	cv::VideoWriter video_writer;
	cv::Mat frame;
	std::vector<cv::Rect> det_cv;
	std::vector<struct tracks> track_object_list;

	bool tracking_use_kalman_filter_is_enable = 0;
	int tracking_distance_thresh_pixel, tracking_invisible_for_too_long_thresh, tracking_age_thresh;
	float tracking_visibility_thres;

	std::vector<struct social_distance_detector> sdist_list;
	bool social_distance_is_enable = 0;
	int social_distance_thres_pixel, social_distance_lost_time_thres_sec, social_distance_error_thres_sec;

	fprintf(stdout, "%s(): dvmot started, version: [%s] .\n", __func__, Version);
	if (argc != 2)
	{
		fprintf(stderr, "%s(): Usage [./dvmot dvmot.ini] .\n", __func__);
		return -1;
	}

	add_exit_signals();

	if (read_ini_file(argv[1], &app_debug, &app_show_frame, &dnnet, &tracking_use_kalman_filter_is_enable, &tracking_distance_thresh_pixel, &tracking_invisible_for_too_long_thresh, &tracking_age_thresh, &tracking_visibility_thres, &social_distance_is_enable, &social_distance_thres_pixel, &social_distance_lost_time_thres_sec, &social_distance_error_thres_sec, video_stream_source_name, video_stream_output_name) != 0)
	{
		return -1;
	}

	if (open_video_stream(video_stream_source_name, cap) != 0)
	{
		return -1;
	}
	
	if (!cap.read(frame)) // get first frame
	{
		return -1;
	}

	if (create_video_writer(video_writer, video_stream_output_name, 'D', 'I', 'V', 'X', get_stream_fps(cap), get_width_mat(frame), get_height_mat(frame), 1) != 0)
	{
		return -1;
	}

	if (load_dnnetwork(&dnnet) != 0)
	{
		return -1;
	}

	while (1)
	{
		if (get_frame(cap, frame) != 0)
		{
			break;
		}

		det_cv = detect_objects(&dnnet, frame, app_debug);

		if (tracking_use_kalman_filter_is_enable)
		{
			predict_new_locations_of_tracks(track_object_list); // predict kalman, update tracks bbox
		}

		print_detection_cv(det_cv);

		update_tracks(track_object_list, det_cv, tracking_use_kalman_filter_is_enable, tracking_distance_thresh_pixel, tracking_invisible_for_too_long_thresh, tracking_age_thresh, tracking_visibility_thres);

		if (social_distance_is_enable)
		{
			update_social_distance_detector(track_object_list, sdist_list, social_distance_thres_pixel, social_distance_lost_time_thres_sec);
		}

		// draw_detections(det_cv, frame);

		draw_tracks(track_object_list, frame);

		if (social_distance_is_enable)
		{
			draw_social_distance_detector(track_object_list, sdist_list, frame, social_distance_error_thres_sec);
		}

		det_cv.clear();

		if (app_show_frame)
		{
			cv::imshow("dvmot", frame);
		}

		write_frame_in_video(video_writer, frame);

		if (cv::waitKey(10) == 27 || g_exit_signal) // pressed ESC or kill signal
		{
			break;
		}
	}

	release_video_writer(video_writer);
	close_network(&dnnet);
	fprintf(stdout, "%s(): bye.. \n", __func__);

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
read_ini_file(const char* file_name, bool* app_debug, bool* app_show_frame, struct dnnetwork* dnnet, bool* tracking_use_kalman_filter_is_enable, int* tracking_distance_thresh_pixel, int* tracking_invisible_for_too_long_thresh, int* tracking_age_thresh, float* tracking_visibility_thres, bool* social_distance_is_enable, int* social_distance_thres_pixel, int* social_distance_lost_time_thres_sec, int* social_distance_error_thres_sec, char* video_stream_source_name, char* video_stream_output_name)
{
	ini_t* config;

	config = ini_load(file_name);
	if (config == NULL)
	{
		fprintf(stderr, "%s(): Ini file read err: [%s] .\n", __func__, file_name);
		return -1;
	}

	*app_debug = atoi(ini_get(config, "app", "app_debug"));
	fprintf(stdout, "%s(): app_debug: [%d] .\n", __func__, *app_debug);

	*app_show_frame = atoi(ini_get(config, "app", "app_show_frame"));
	fprintf(stdout, "%s(): app_show_frame: [%d] .\n", __func__, *app_show_frame);

	dnnet->detect_thres = atoi(ini_get(config, "dnnetwork", "dnnetwork_detect_thres"));
	fprintf(stdout, "%s(): dnnetwork_detect_thres: [%d] .\n", __func__, dnnet->detect_thres);

	strcpy(dnnet->obj_names_file, ini_get(config, "dnnetwork", "dnnetwork_obj_names_file"));
	fprintf(stdout, "%s(): dnnetwork_obj_names_file: [%s] .\n", __func__, dnnet->obj_names_file);

	strcpy(dnnet->conf_file, ini_get(config, "dnnetwork", "dnnetwork_conf_file"));
	fprintf(stdout, "%s(): dnnetwork_conf_file: [%s] .\n", __func__, dnnet->conf_file);

	strcpy(dnnet->weights_file, ini_get(config, "dnnetwork", "dnnetwork_weights_file"));
	fprintf(stdout, "%s(): dnnetwork_weights_file: [%s] .\n", __func__, dnnet->weights_file);

	dnnet->gpu_id = atoi(ini_get(config, "dnnetwork", "dnnetwork_gpu_id"));
	fprintf(stdout, "%s(): dnnetwork_gpu_id: [%d] .\n", __func__, dnnet->gpu_id);

	*tracking_use_kalman_filter_is_enable = atoi(ini_get(config, "tracking", "tracking_use_kalman_filter_is_enable"));
	fprintf(stdout, "%s(): tracking_use_kalman_filter_is_enable: [%d] .\n", __func__, *tracking_use_kalman_filter_is_enable);

	*tracking_distance_thresh_pixel = atoi(ini_get(config, "tracking", "tracking_distance_thresh_pixel"));
	fprintf(stdout, "%s(): tracking_distance_thresh_pixel: [%d] .\n", __func__, *tracking_distance_thresh_pixel);

	*tracking_invisible_for_too_long_thresh = atoi(ini_get(config, "tracking", "tracking_invisible_for_too_long_thresh"));
	fprintf(stdout, "%s(): tracking_invisible_for_too_long_thresh: [%d] .\n", __func__, *tracking_invisible_for_too_long_thresh);

	*tracking_age_thresh = atoi(ini_get(config, "tracking", "tracking_age_thresh"));
	fprintf(stdout, "%s(): tracking_age_thresh: [%d] .\n", __func__, *tracking_age_thresh);

	*tracking_visibility_thres = atof(ini_get(config, "tracking", "tracking_visibility_thres"));
	fprintf(stdout, "%s(): tracking_visibility_thres: [%f] .\n", __func__, *tracking_visibility_thres);

	*social_distance_is_enable = atoi(ini_get(config, "social_distance", "social_distance_is_enable"));
	fprintf(stdout, "%s(): social_distance_is_enable: [%d] .\n", __func__, *social_distance_is_enable);

	*social_distance_thres_pixel = atoi(ini_get(config, "social_distance", "social_distance_thres_pixel"));
	fprintf(stdout, "%s(): social_distance_thres_pixel: [%d] .\n", __func__, *social_distance_thres_pixel);

	*social_distance_lost_time_thres_sec = atoi(ini_get(config, "social_distance", "social_distance_lost_time_thres_sec"));
	fprintf(stdout, "%s(): social_distance_lost_time_thres_sec: [%d] .\n", __func__, *social_distance_lost_time_thres_sec);

	*social_distance_error_thres_sec = atoi(ini_get(config, "social_distance", "social_distance_error_thres_sec"));
	fprintf(stdout, "%s(): social_distance_error_thres_sec: [%d] .\n", __func__, *social_distance_error_thres_sec);

	strcpy(video_stream_source_name, ini_get(config, "video_stream", "video_stream_source_name"));
	fprintf(stdout, "%s(): video_stream_source_name: [%s] .\n", __func__, video_stream_source_name);

	strcpy(video_stream_output_name, ini_get(config, "video_stream", "video_stream_output_name"));
	fprintf(stdout, "%s(): video_stream_output_name: [%s] .\n", __func__, video_stream_output_name);

	ini_free(config);

	return 0;
}
