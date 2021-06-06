#ifndef KALMAN_H
#define KALMAN_H

#include <opencv2/opencv.hpp>
#include <vector>

cv::KalmanFilter
init_kalman_filter(cv::Point2f initial_point, int state_size, int meas_size, int contr_size, int type);

cv::Point2f
predict(cv::KalmanFilter& kf);

void
correct(cv::KalmanFilter& kf, cv::Point2f p);

void
update_dT(cv::KalmanFilter& kf, double& last_tick);

#endif // KALMAN_H

