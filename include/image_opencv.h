#ifndef IMAGE_OPENCV_H
#define IMAGE_OPENCV_H

#include <opencv2/opencv.hpp>
#include <vector>

struct det_cv
{
	std::string name;
	cv::Rect bbox_cv;
	float pred_score;
};

int
get_frame(cv::VideoCapture& cap, cv::Mat& m);

int
open_video_stream(const char* file_name, cv::VideoCapture& cap);

int
create_video_writer(cv::VideoWriter& output_video_writer, const char* out_filename, char c1, char c2, char c3, char c4, int fps, int width, int height, int is_color);

int
write_frame_in_video(cv::VideoWriter& output_video_writer, const cv::Mat& mat);

int
release_video_writer(cv::VideoWriter& output_video_writer);

int
get_stream_fps(const cv::VideoCapture& cap);

void
draw_bbox_cv(const cv::Rect& bbox_cv, cv::Mat& m);

void
draw_detections(const std::vector<struct det_cv>& detection_list, cv::Mat& m);

void
print_detection_cv(const std::vector<struct det_cv>& det_cv);

#endif // IMAGE_OPENCV_H
