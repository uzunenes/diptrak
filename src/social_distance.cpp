#include "../include/social_distance.h"
#include "../include/tracks.h"
#include "../include/utils.h"

static void
delete_lost_sdict(std::vector<struct social_distance_detector>& sdist_list, int lost_time_thres_sec)
{
	int i;
	double now;

	now = what_time_is_it_now();
	for (i = 0; i < (int)sdist_list.size(); ++i)
	{
		if ((now - sdist_list[i].last_vis_sec) > lost_time_thres_sec) // delete sdict, lost!
		{
			sdist_list.erase(sdist_list.begin() + i);
		}
	}
}

static void
add_and_update_list(std::vector<struct social_distance_detector>& sdist_list, int tracks_id1, int tracks_id2)
{
	int i;
	struct social_distance_detector sdist;

	for (i = 0; i < (int)sdist_list.size(); ++i)
	{
		if ((sdist_list[i].track_id_nums[0] == tracks_id1 && sdist_list[i].track_id_nums[1] == tracks_id2) || (sdist_list[i].track_id_nums[0] == tracks_id2 && sdist_list[i].track_id_nums[1] == tracks_id1))
		{
			sdist_list[i].last_vis_sec = what_time_is_it_now(); // update time
			return;
		}
	}

	// add new sdist
	sdist.track_id_nums[0] = tracks_id1;
	sdist.track_id_nums[1] = tracks_id2;
	sdist.start_sec = what_time_is_it_now();
	sdist.last_vis_sec = what_time_is_it_now();
	sdist_list.push_back(sdist);
}

static void
check_new_sdict(const std::vector<struct tracks>& tracks, std::vector<struct social_distance_detector>& sdist_list, int dist_thres_pixel)
{
	int i, j;
	double distance;
	cv::Point center_p1, center_p2;

	for (i = 0; i < (int)tracks.size(); ++i)
	{
		center_p1 = get_center_bbox_cv(tracks[i].bbox_cv);

		for (j = i + 1; j < (int)tracks.size(); ++j)
		{
			center_p2 = get_center_bbox_cv(tracks[j].bbox_cv);

			distance = get_euclidean_distance_two_points(center_p1, center_p2);

			if (distance < dist_thres_pixel)
			{
				add_and_update_list(sdist_list, tracks[i].id, tracks[j].id);
			}
		}
	}
}

void
update_social_distance_detector(const std::vector<struct tracks>& tracks, std::vector<struct social_distance_detector>& sdist_list, int dist_thres_pixel, int lost_time_thres_sec)
{
	check_new_sdict(tracks, sdist_list, dist_thres_pixel);

	delete_lost_sdict(sdist_list, lost_time_thres_sec);
}

void
draw_social_distance_detector(const std::vector<struct tracks>& tracks, const std::vector<struct social_distance_detector>& sdist_list, cv::Mat& m, int social_distance_error_sec_thres)
{
	int i, j, tracks_id1, tracks_id2, cnt_found_flag;
	cv::Point center_point1, center_point2;
	cv::Scalar color;

	for (i = 0; i < (int)sdist_list.size(); ++i)
	{

		tracks_id1 = sdist_list[i].track_id_nums[0];
		tracks_id2 = sdist_list[i].track_id_nums[1];

		cnt_found_flag = -2;
		for (j = 0; j < (int)tracks.size(); ++j)
		{
			if (tracks[j].id == tracks_id1)
			{
				center_point1 = get_center_bbox_cv(tracks[j].bbox_cv);
				cnt_found_flag++;
			}
			else if (tracks[j].id == tracks_id2)
			{
				center_point2 = get_center_bbox_cv(tracks[j].bbox_cv);
				cnt_found_flag++;
			}
		}

		if (cnt_found_flag == 0)
		{
			color = cv::Scalar(0, 255, 0); // default green

			if ((sdist_list[i].last_vis_sec - sdist_list[i].start_sec) > social_distance_error_sec_thres)
			{
				color = cv::Scalar(0, 0, 255); // draw red
			}

			cv::line(m, center_point1, center_point2, color, 2);
		}
	}
}
