#include "../include/kalman.h"
#include "../include/utils.h"

cv::Point2f
predict(cv::KalmanFilter& kf)
{
	cv::Mat m;

	m = kf.predict();

	return mat_to_point(m);
}

void
correct(cv::KalmanFilter& kf, const cv::Point2f& p)
{
	cv::Mat m = point_to_mat(p);

	kf.correct(m);
}

void
update_dT(cv::KalmanFilter& kf, double& last_tick)
{
	double ticks, dT;

	ticks = (double)cv::getTickCount();

	dT = (ticks - last_tick) / cv::getTickFrequency(); // seconds

	if (last_tick == 0)
	{
		dT = 1;
	}

	last_tick = ticks; // update tick

	kf.transitionMatrix.at<float>(2) = dT;
	kf.transitionMatrix.at<float>(7) = dT;
}

cv::KalmanFilter
init_kalman_filter(const cv::Point2f& initial_point, int state_size, int meas_size, int contr_size, int type)
{
	const float process_noise_cov = 1e-2;
	const float measurement_noise_cov = 1e-1;
	const float error_cov_pre = 1;

	cv::KalmanFilter kf(state_size, meas_size, contr_size, type);

	// Transition State Matrix A
	// Note: set dT at each processing step!
	// [ 1 0 dT 0 ]
	// [ 0 1 0 dT]
	cv::setIdentity(kf.transitionMatrix);

	// Measure Matrix H
	// [ 1 0 0 0]
	// [ 0 1 0 0]
	kf.measurementMatrix = cv::Mat::zeros(meas_size, state_size, type);
	kf.measurementMatrix.at<float>(0) = 1.0f;
	kf.measurementMatrix.at<float>(5) = 1.0f;

	// Process Noise Covariance Matrix Q
	// [ Ex  0   0     0    ]
	// [ 0   Ey  0     0    ]
	// [ 0   0   Ev_x  0    ]
	// [ 0   0   0     Ev_y ]
	kf.processNoiseCov.at<float>(0) = process_noise_cov;
	kf.processNoiseCov.at<float>(5) = process_noise_cov;

	// Measures Noise Covariance Matrix R
	cv::setIdentity(kf.measurementNoiseCov, cv::Scalar(measurement_noise_cov));

	// >> Initialization
	kf.errorCovPre.at<float>(0) = error_cov_pre;
	kf.errorCovPre.at<float>(5) = error_cov_pre;

	cv::Mat state(state_size, 1, type);
	state.at<float>(0) = initial_point.x;
	state.at<float>(1) = initial_point.y;
	state.at<float>(2) = 0;
	state.at<float>(3) = 0;

	kf.statePost = state;
	// << Initialization

	return kf;
}
