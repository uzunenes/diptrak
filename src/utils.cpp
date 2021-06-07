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
mat_to_point(const cv::Mat& m)
{
	cv::Point2f p;

	p.x = 0;
	p.y = 0;

	if (m.empty())
	{
		return p;
	}

	p.x = m.at<float>(0);
	p.y = m.at<float>(1);

	return p;
}

cv::Mat
point_to_mat(const cv::Point2f& p)
{
	cv::Mat m(2, 1, CV_32F);

	m.at<float>(0) = p.x;
	m.at<float>(1) = p.y;

	return m;
}

cv::Point
get_center_bbox_cv(const cv::Rect& bbox_cv)
{
	cv::Point p;

	p.x = bbox_cv.x + (bbox_cv.width / 2);
	p.y = bbox_cv.y + (bbox_cv.height / 2);

	return p;
}

int
get_width_mat(cv::Mat& m)
{
	if (m.empty())
	{
		fprintf(stderr, "%s(): Mat is empty \n", __func__);
		return 0;
	}

	return m.cols;
}

int
get_height_mat(cv::Mat& m)
{
	if (m.empty())
	{
		fprintf(stderr, "%s(): Mat is empty \n", __func__);
		return 0;
	}

	return m.rows;
}

double
get_euclidean_distance_two_points(cv::Point2f p1, cv::Point2f p2)
{
	double distance;

	distance = sqrt(pow(p1.y - p2.y, 2) + pow((p1.x - p2.x), 2));

	return distance;
}
