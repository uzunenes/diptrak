#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>

double
what_time_is_it_now(void);

char**
get_labels_custom(char *filename, int *size);

cv::Mat
point_to_mat(cv::Point2f& p);

cv::Point2f
mat_to_point(cv::Mat& m);

cv::Point
get_center_bbox_cv(cv::Rect& bbox_cv);

int
get_width_mat(cv::Mat& m);

int
get_height_mat(cv::Mat& m);


#endif // UTILS_H