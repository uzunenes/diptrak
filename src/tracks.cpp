#include "../include/tracks.h"
#include "../include/hungarian.h"
#include "../include/image_opencv.h"
#include "../include/kalman.h"
#include "../include/utils.h"
#include <math.h>

// -- global variables ---
static int g_next_id = 0;
// ------------------------

typedef struct
{
	int track_idx;
	int det_idx;
} assignment_idx;

static void
create_new_tracks(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv, std::vector<int>& unassigned_detections, bool kalman_filter_is_enable)
{
	int i, index;

	for (i = 0; i < (int)unassigned_detections.size(); ++i)
	{
		struct tracks new_tracks;
		index = unassigned_detections[i];

		new_tracks.id = g_next_id;
		new_tracks.bbox_cv = det_cv[index];

		if (kalman_filter_is_enable)
		{
			new_tracks.kalman_filter = init_kalman_filter(get_center_bbox_cv(new_tracks.bbox_cv));
			new_tracks.last_tick_kf = 0;
		}

		new_tracks.age = 1;
		new_tracks.total_visible_count = 1;
		new_tracks.consecutive_invisible_count = 0;

		tracks_objects.push_back(new_tracks);

		++g_next_id;
	}
}

static void
delete_lost_tracks(std::vector<struct tracks>& tracks, int invisible_for_too_long_thresh, int age_thresh, float visibility_thresh)
{
	int i;
	float visibility;

	for (i = 0; i < (int)tracks.size(); ++i)
	{
		visibility = (float)tracks[i].total_visible_count / (float)tracks[i].age;

		if ((visibility < visibility_thresh && tracks[i].age < age_thresh) || tracks[i].consecutive_invisible_count > invisible_for_too_long_thresh)
		{
			tracks.erase(tracks.begin() + i);
		}
	}
}

static void
update_unassigned_tracks(std::vector<struct tracks>& tracks_objects, std::vector<int>& unassignments_tracks)
{
	int i, idx;

	for (i = 0; i < (int)unassignments_tracks.size(); ++i)
	{
		idx = unassignments_tracks[i];

		++tracks_objects[idx].age;
		++tracks_objects[idx].consecutive_invisible_count;
	}
}

static void
update_assigned_tracks(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv, std::vector<assignment_idx>& assignments_indexs, bool kalman_filter_is_enable)
{
	int i, track_idx, det_idx;

	for (i = 0; i < (int)assignments_indexs.size(); ++i)
	{
		track_idx = assignments_indexs[i].track_idx;
		det_idx = assignments_indexs[i].det_idx;

		cv::Rect bbox_cv_det = det_cv[det_idx];

		if (kalman_filter_is_enable)
		{
			cv::Point det_center_point = get_center_bbox_cv(det_cv[det_idx]);
			correct(tracks_objects[track_idx].kalman_filter, det_center_point);
		}

		tracks_objects[track_idx].bbox_cv = bbox_cv_det;

		++tracks_objects[track_idx].age;
		++tracks_objects[track_idx].total_visible_count;
		tracks_objects[track_idx].consecutive_invisible_count = 0;
	}
}

static void
detection_to_track_assignment(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv, std::vector<assignment_idx>& assignments_indexs, std::vector<int>& unassignments_tracks, std::vector<int>& unassigned_detections, int distance_th)
{
	int i, j;
	bool flag;
	double distance, cost;
	HungarianAlgorithm hung_algo;
	std::vector<int> assignment;
	std::vector<std::vector<double>> cost_matrix(det_cv.size(), std::vector<double>(tracks_objects.size(), distance_th));
	std::vector<std::vector<int>> cost_matrix_det_idx(det_cv.size(), std::vector<int>(tracks_objects.size(), distance_th));

	if (tracks_objects.size() == 0)
	{
		for (i = 0; i < (int)det_cv.size(); ++i)
		{
			unassigned_detections.push_back(i);
		}
		return;
	}

	for (i = 0; i < (int)det_cv.size(); ++i)
	{
		flag = 0;
		for (j = 0; j < (int)tracks_objects.size(); ++j)
		{
			cv::Point center_point1 = get_center_bbox_cv(tracks_objects[j].bbox_cv);
			cv::Point center_point2 = get_center_bbox_cv(det_cv[i]);

			distance = get_euclidean_distance_two_points(center_point1, center_point2);

			if (distance < distance_th)
			{
				flag = 1;
				cost_matrix[i][j] = distance;
			}
		}

		// new detections, store unassigned_detections
		if (flag == 0)
		{
			unassigned_detections.push_back(i);
		}
	}

	/*
	cost matrix
	------------------tracks_objects - col
	|det[i]
	|det[i+1]
	|
	new detection - row
	*/
	for (i = 0; i < (int)det_cv.size(); ++i)
	{
		for (j = 0; j < (int)tracks_objects.size(); ++j)
		{
			std::cout << cost_matrix[i][j] << " ";
		}
		std::cout << "\n";
	}

	cost = hung_algo.Solve(cost_matrix, assignment);
	fprintf(stdout, "%s(): Hung algo cost: [%.1f] \n", __func__, cost);

	for (i = 0; i < (int)cost_matrix.size(); ++i)
	{
		fprintf(stdout, "%d,%d \t", i, assignment[i]);

		if (assignment[i] != -1 && cost_matrix[i][assignment[i]] != distance_th) // assignments_indexs
		{
			assignment_idx temp;
			temp.track_idx = assignment[i];
			temp.det_idx = i;
			assignments_indexs.push_back(temp);
		}
	}
	fprintf(stdout, "\n");

	// unassignments_tracks
	for (i = 0; i < (int)tracks_objects.size(); ++i)
	{
		flag = 0;
		for (j = 0; j < (int)cost_matrix.size(); ++j)
		{
			if (assignment[j] == i)
			{
				flag = 1;
			}
		}

		if (flag == 0)
		{
			unassignments_tracks.push_back(i);
		}
	}
}

void
update_tracks(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv, bool kalman_filter_is_enable, int distance_thresh_pixel, int invisible_for_too_long_thresh, int age_thresh, float visibility_thresh)
{
	std::vector<assignment_idx> assignments_indexs;
	std::vector<int> unassigned_detections, unassignments_tracks;

	detection_to_track_assignment(tracks_objects, det_cv, assignments_indexs, unassignments_tracks, unassigned_detections, distance_thresh_pixel);

	update_assigned_tracks(tracks_objects, det_cv, assignments_indexs, kalman_filter_is_enable);

	update_unassigned_tracks(tracks_objects, unassignments_tracks);

	delete_lost_tracks(tracks_objects, invisible_for_too_long_thresh, age_thresh, visibility_thresh);

	create_new_tracks(tracks_objects, det_cv, unassigned_detections, kalman_filter_is_enable);
}

void
predict_new_locations_of_tracks(std::vector<struct tracks>& tracks_objects)
{
	int i;
	cv::Rect bbox_cv;
	cv::Point2f predicted_centroid;

	for (i = 0; i < (int)tracks_objects.size(); ++i)
	{
		bbox_cv = tracks_objects[i].bbox_cv;

		update_dT(tracks_objects[i].kalman_filter, tracks_objects[i].last_tick_kf);
		predicted_centroid = predict(tracks_objects[i].kalman_filter);

		predicted_centroid.x = predicted_centroid.x - ((float)bbox_cv.width / 2.0);
		if (predicted_centroid.x < 0)
			predicted_centroid.x = 0;

		predicted_centroid.y = predicted_centroid.y - ((float)bbox_cv.height / 2.0);
		if (predicted_centroid.y < 0)
			predicted_centroid.y = 0;

		bbox_cv.x = (int)round(predicted_centroid.x);
		bbox_cv.y = (int)round(predicted_centroid.y);
		printf("%s: %d %d %d %d \n", __func__, bbox_cv.x, bbox_cv.y, bbox_cv.width, bbox_cv.height);

		tracks_objects[i].bbox_cv = bbox_cv;
	}
}

void
draw_tracks(const std::vector<struct tracks>& tracks_objects, cv::Mat& m)
{
	int i;
	const int offset_pixl = 20;
	char buff_text[32];

	for (i = 0; i < (int)tracks_objects.size(); ++i)
	{
		draw_bbox_cv(tracks_objects[i].bbox_cv, m);
		cv::Point center = get_center_bbox_cv(tracks_objects[i].bbox_cv);

		// put id
		cv::Point id_loc = cv::Point(center.x, center.y - 2 * offset_pixl);
		sprintf(buff_text, "id:%d", tracks_objects[i].id);
		cv::putText(m, buff_text, id_loc, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.1, cv::Scalar(0, 0, 255), 1.5, cv::LINE_AA);

		// put age
		cv::Point age_loc = cv::Point(center.x, center.y - offset_pixl);
		sprintf(buff_text, "age:%d", tracks_objects[i].age);
		cv::putText(m, buff_text, age_loc, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.1, cv::Scalar(0, 0, 255), 1.5, cv::LINE_AA);

		// put total visible count
		cv::Point t_vis_cnt = cv::Point(center.x, center.y);
		sprintf(buff_text, "tvt:%d", tracks_objects[i].total_visible_count);
		cv::putText(m, buff_text, t_vis_cnt, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.1, cv::Scalar(0, 0, 255), 1.5, cv::LINE_AA);

		cv::Point consecutive_invis_cnt = cv::Point(center.x, center.y + offset_pixl);
		sprintf(buff_text, "cit:%d", tracks_objects[i].consecutive_invisible_count);
		cv::putText(m, buff_text, consecutive_invis_cnt, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.1, cv::Scalar(0, 0, 255), 1.5, cv::LINE_AA);
	}
}
