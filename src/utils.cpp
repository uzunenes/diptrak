#include "../include/utils.h"
#include <cstddef>
#include <sys/time.h>

double
what_time_is_it_now(void)
{
	struct timeval time;
	if (gettimeofday(&time, NULL))
	{
		return 0;
	}
	return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

cv::Point2f
mat_to_point(cv::Mat& m)
{
    cv::Point2f p;

    p.x = 0;
    p.y = 0;

    if( m.empty() )
    {
        return p;
    }

    p.x = m.at<float>(0);
    p.y = m.at<float>(1);

    return p;
}

cv::Mat
point_to_mat(cv::Point2f& p)
{
    cv::Mat m(2, 1, CV_32F);

    m.at<float>(0) = p.x;
    m.at<float>(1) = p.y;

    return m;
}