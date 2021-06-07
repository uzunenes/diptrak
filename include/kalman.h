#ifndef KALMAN_H
#define KALMAN_H

#include <opencv2/opencv.hpp>

cv::KalmanFilter
init_kalman_filter(cv::Point2f initial_point, int state_size = 4, int meas_size = 2, int contr_size = 0, int type = CV_32F);

cv::Point2f
predict(cv::KalmanFilter& kf);

void
correct(cv::KalmanFilter& kf, cv::Point2f p);

void
update_dT(cv::KalmanFilter& kf, double& last_tick);

#endif // KALMAN_H
