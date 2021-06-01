#ifndef IMAGE_OPENCV_H
#define IMAGE_OPENCV_H

#include <opencv2/opencv.hpp>
#include <vector>

struct det_cv
{
    char name[64];
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
write_frame_in_video(cv::VideoWriter& output_video_writer, cv::Mat& mat);

int
release_video_writer(cv::VideoWriter& output_video_writer);

int
get_stream_fps(cv::VideoCapture& cap);

int
get_width_mat(cv::Mat& m);

int
get_height_mat(cv::Mat& m);

void
draw_bbox_cv(cv::Rect& bbox_cv, cv::Mat& m);

void
draw_detections(std::vector<cv::Rect>& det_cv, cv::Mat& frame);

void
print_detection_cv(std::vector<cv::Rect>& det_cv);

#endif // IMAGE_OPENCV_H
