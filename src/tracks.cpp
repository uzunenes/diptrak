#include "../include/tracks.h"

static void
delete_lost_tracks(std::vector<struct tracks>& tracks)
{
    int i;
    const int invisible_for_too_long = 20;
    const int age_threshold = 8;
    const float visibility_threshold = 0.6;
    float visibility;

    for (i = 0; i < (int)tracks.size(); ++i)
    {
        visibility = tracks[i].total_visible_count / tracks[i].age;

        if ((visibility < visibility_threshold && tracks[i].age < age_threshold) || tracks[i].consecutive_invisible_count > invisible_for_too_long)
        {
            tracks.erase(tracks.begin() + i);
        }
    }
}

static void
detection_to_track_assignment(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv, std::vector<int>& assignments_indexs, std::vector<int>& unassignments_indexs, std::vector<int>& unassigned_detections)
{
}

static void
update_assigned_tracks(std::vector<struct tracks>& tracks_objects, std::vector<int>& assignments_indexs)
{
}

static void
update_unassigned_tracks(std::vector<struct tracks>& tracks_objects, std::vector<int>& unassignments_indexs)
{
}

static void
create_new_tracks(std::vector<struct tracks>& tracks_objects, std::vector<int>& unassigned_detections)
{
	int i;
	const int mesarumnet_noise;

kalman.measurementMatrix = np.array([[1, 0, 0, 0],
                                     [0, 1, 0, 0]], np.float32)

kalman.transitionMatrix = np.array([[1, 0, 1, 0],
                                    [0, 1, 0, 1],
                                    [0, 0, 1, 0],
                                    [0, 0, 0, 1]], np.float32)

kalman.processNoiseCov = np.array([[1, 0, 0, 0],
                                   [0, 1, 0, 0],
                                   [0, 0, 1, 0],
                                   [0, 0, 0, 1]], np.float32) * 0.03

	for(i = 0 ;i < (int)unassigned_detections.size(); ++i)
	{

	}

}

void
update_tracks(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv)
{
    std::vector<int> assignments_indexs, unassignments_indexs, unassigned_detections;

    detection_to_track_assignment(tracks_objects, det_cv, assignments_indexs, unassignments_indexs, unassigned_detections);

    update_assigned_tracks(tracks_objects, assignments_indexs);

    update_unassigned_tracks(tracks_objects, unassignments_indexs);

    delete_lost_tracks(tracks_objects);

    create_new_tracks(tracks_objects, unassigned_detections);
}

void
predict_new_locations_of_tracks(std::vector<struct tracks>& tracks_objects)
{
    int i;

    for (i = 0; i < (int)tracks_objects.size(); ++i)
    {

    }
}

void
draw_tracks(std::vector<struct tracks>& tracks_objects, cv::Mat& m)
{
}