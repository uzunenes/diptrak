#include "../include/kalman.h"

static cv::Point2f
mat_to_point(cv::Mat& m)
{
    cv::Point2f p;

    p.x = 0;
    p.y = 0;

    if ( m.empty() )
    {
        return p;
    } 

    p.x = m.at<float>(0);
    p.y = m.at<float>(1);

    return p;
}

static cv::Mat
point_to_mat(cv::Point& p)
{
    cv::Mat m(2, 1, CV_32F);

    m.at<float>(0) = p.x;
    m.at<float>(1) = p.y;

    return m;
}

cv::Point2f
predict(cv::KalmanFilter& kalman)
{
    cv::Mat m;

    kalman.predict(m);

    return mat_to_point(m);
}

cv::Mat
correct(cv::KalmanFilter& kalman, cv::Point p)
{
    cv::Mat m = point_to_mat(p);

    cv::Mat estimated = kalman.correct(m);

    return estimated;
}

cv::KalmanFilter
init_kalman_filter(cv::Point initial_point)
{
    const int stateSize = 4;
    const int measSize = 2;
    const int contrSize = 0;
    const unsigned int type = CV_32F;

    cv::KalmanFilter kf(stateSize, measSize, contrSize, type);

    // declare an array of floats to feed into Kalman Filter Transition Matrix, also known as State Transition Model
    float fltTransitionMatrixValues[4][4] = { { 1, 0, 1, 0 },
                                              { 0, 1, 0, 1 },
                                              { 0, 0, 1, 0 },
                                              { 0, 0, 0, 1 } };

    // set Transition Matrix
    kf.transitionMatrix = cv::Mat(4, 4, CV_32F, fltTransitionMatrixValues);
    // set Measurement Matrix
    kf.statePre.at<float>(0) = (float)initial_point.x;;
    kf.statePre.at<float>(1) = (float)initial_point.y;
    kf.statePre.at<float>(2) = 0;
    kf.statePre.at<float>(3) = 0;

    cv::setIdentity(kf.measurementMatrix);
    cv::setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-4));
    cv::setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-1));
    cv::setIdentity(kf.errorCovPost, cv::Scalar::all(0.1));

    return kf;
}