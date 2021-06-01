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
    cv::Mat m(1, 2, CV_32F);

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

void
correct(cv::KalmanFilter& kalman, cv::Point p)
{
    cv::Mat m = point_to_mat(p);

    kalman.correct(m);
}


cv::KalmanFilter
init_kalman_filter()
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


    // declare an array of floats to feed into Kalman Filter Measurement Matrix, also known as Measurement Model
    float fltMeasurementMatrixValues[2][4] = { { 1, 0, 0, 0 },
                                               { 0, 1, 0, 0 } };
    // set Measurement Matrix
    kf.measurementMatrix = cv::Mat(2, 4, CV_32F, fltMeasurementMatrixValues);

    // default is 1, for smoothing try 0.0001
    cv::setIdentity(kf.processNoiseCov, cv::Scalar::all(0.0001));
    // default is 1, for smoothing try 10
    cv::setIdentity(kf.measurementNoiseCov, cv::Scalar::all(10));
    // default is 0, for smoothing try 0.1
    cv::setIdentity(kf.errorCovPost, cv::Scalar::all(0.1));

    return kf;
}