#ifndef KALMAN_H
#define KALMAN_H

#include <opencv2/opencv.hpp>
#include <vector>

cv::Point2f
predict(cv::KalmanFilter& kalman);

cv::KalmanFilter
init_kalman_filter(cv::Point initial_point);

cv::Mat
correct(cv::KalmanFilter& kalman, cv::Point p);

#endif // KALMAN_H

