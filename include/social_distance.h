#ifndef SOCIAL_DISTANCE_H
#define SOCIAL_DISTANCE_H

#include <opencv2/opencv.hpp>
#include <vector>

struct social_distance_detector
{
	int track_id_nums[2];
	double start_sec;
	double last_vis_sec;
};

void
update_social_distance_detector(const std::vector<struct tracks>& tracks, std::vector<struct social_distance_detector>& sdist_list, int dist_thres_pixel, int lost_time_thres_sec);

void
draw_social_distance_detector(const std::vector<struct tracks>& tracks, const std::vector<struct social_distance_detector>& sdist_list, cv::Mat& m, int social_distance_error_sec_thres);

#endif // SOCIAL_DISTANCE_H
