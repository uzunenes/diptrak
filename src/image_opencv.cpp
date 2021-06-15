#include "../include/image_opencv.h"
#include "../include/utils.h"

int
get_frame(cv::VideoCapture& cap, cv::Mat& frame)
{
	if (!cap.read(frame))
	{
		fprintf(stderr, "%s(): Frame reading error. \n", __func__);
		return -1;
	}
	if (frame.empty())
	{
		fprintf(stdout, "%s(): Frame empty, end of the video file. \n", __func__);
		return -2;
	}

	return 0;
}

int
open_video_stream(const char* file_name, cv::VideoCapture& cap)
{
	char gst_pipeline[256];

	sprintf(gst_pipeline, "filesrc location=%s ! decodebin ! videoconvert ! appsink", file_name);
	fprintf(stdout, "%s(): Openining video file: [%s] , pipeline: [%s] .\n", __func__, file_name, gst_pipeline);

	try
	{
		cap.open(gst_pipeline, cv::CAP_GSTREAMER);
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
write_frame_in_video(cv::VideoWriter& output_video_writer, const cv::Mat& mat)
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
get_stream_fps(const cv::VideoCapture& cap)
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

void
draw_bbox_cv(const cv::Rect& bbox_cv, cv::Mat& m)
{
	cv::rectangle(m, bbox_cv, cv::Scalar(255, 100, 0), 2, 0);
}

void
draw_detections(const std::vector<struct det_cv>& detection_list, cv::Mat& m)
{
	int i;

	for (i = 0; i < (int)detection_list.size(); ++i)
	{
		draw_bbox_cv(detection_list[i].bbox_cv, m);
	}
}

void
print_detection_cv(const std::vector<struct det_cv>& detection_list)
{
	int i;

	for (i = 0; i < (int)detection_list.size(); ++i)
	{
		printf("%s(): index: [%d], x: [%d] y: [%d] width: [%d] height: [%d] , name: [%s], pred_score: [%.2f] \n", __func__, i, detection_list[i].bbox_cv.x, detection_list[i].bbox_cv.y, detection_list[i].bbox_cv.width, detection_list[i].bbox_cv.height, detection_list[i].name.c_str(), detection_list[i].pred_score);
	}
}
