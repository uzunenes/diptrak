#ifndef TRACKS_H
#define TRACKS_H

#include <opencv2/opencv.hpp>
#include <vector>

struct tracks
{
	int id;
	cv::Rect bbox_cv;
	std::string obj_name;
	cv::KalmanFilter kalman_filter;
	double last_tick_kf;
	int age;                         // the number of frames since the track was first detected.
	int total_visible_count;         // the total number of frames in which the track was detected (visible).
	int consecutive_invisible_count; // the number of consecutive frames for which the track was not detected (invisible).
};

void
update_tracks(std::vector<struct tracks>& tracks_objects, const std::vector<struct det_cv>& det_list, bool kalman_filter_is_enable, int distance_thresh_pixel, int invisible_for_too_long_thresh, int age_thresh, float visibility_thresh);

void
predict_new_locations_of_tracks(std::vector<struct tracks>& tracks_objects);

void
draw_tracks(const std::vector<struct tracks>& tracks_objects, cv::Mat& m);

#endif // TRACKS_H
