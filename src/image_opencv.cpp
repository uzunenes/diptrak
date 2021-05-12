#include "../include/image_opencv.h"

int
open_video_stream(const char* file_name, cv::VideoCapture& cap)
{
	fprintf(stdout, "%s(): Openining video file: [%s] .\n", __func__, file_name);

	try
	{
		cap.open(file_name);
	}
	catch (...)
	{
		fprintf(stderr, "%s(): Failed openin video file: [%s] .\n", __func__, file_name);
		return -1;
	}

	return 0;
}

int
create_video_writer(cv::VideoWriter& output_video_writer, const char* out_filename, char c1, char c2, char c3, char c4, int fps, int width, int height, int is_color)
{
	try
	{
		output_video_writer.open(out_filename, CV_FOURCC(c1, c2, c3, c4), fps, cv::Size(width, height), is_color);
	}
	catch (...)
	{
		fprintf(stderr, "%s(): OpenCV exception: create_video_writer: [%s] .\n", __func__, out_filename);
		return -1;
	}

	return 0;
}

int
write_frame_in_video(cv::VideoWriter& output_video_writer, cv::Mat& mat)
{
	try
	{
		output_video_writer.write(mat);
	}
	catch (...)
	{
		fprintf(stderr, "%s(): OpenCV exception: write_frame_cv. \n", __func__);
		return -1;
	}

	return 0;
}

int
release_video_writer(cv::VideoWriter& output_video_writer)
{
	try
	{
		fprintf(stdout, "%s(): closing...", __func__);
		output_video_writer.release();
		fprintf(stdout, "  closed. \n");
	}
	catch (...)
	{
		fprintf(stderr, "%s(): OpenCV exception: release_video_writer. \n", __func__);
		return -1;
	}

	return 0;
}

int
get_stream_fps(cv::VideoCapture& cap)
{
	int fps;

	fps = 25;
	try
	{

		fps = cap.get(cv::CAP_PROP_FPS);
	}
	catch (...)
	{
		fprintf(stderr, "%s():  Can't get FPS of source videofile. For output video FPS = [%d] by default. \n", __func__, fps);
	}

	return fps;
}

int
get_width_mat(cv::Mat& m)
{
	if (m.empty())
	{
		fprintf(stderr, "%s(): Mat is empty \n", __func__);
		return 0;
	}

	return m.cols;
}

int
get_height_mat(cv::Mat& m)
{
	if (m.empty())
	{
		fprintf(stderr, "%s(): Mat is empty \n", __func__);
		return 0;
	}

	return m.rows;
}