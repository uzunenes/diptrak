#include "../include/tracks.h"
#include "../include/kalman.h"
#include "../include/hungarian.h"
#include "../include/image_opencv.h"
#include <math.h>


// -- global variables ---
static int g_next_id = 0;
// ------------------------

typedef struct
{
    int track_idx;
    int det_idx;
} assignment_idx;

cv::Point
get_center_bbox_cv(cv::Rect& bbox_cv)
{
    cv::Point p;

    p.x = bbox_cv.x + (bbox_cv.width / 2);
    p.y = bbox_cv.y + (bbox_cv.height / 2);

    return p;
}

static void
create_new_tracks(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv, 
                                                                        std::vector<int>& unassigned_detections)
{
    int i, index;

    for (i = 0; i < (int)unassigned_detections.size(); ++i)
    {
        struct tracks new_tracks;
        index = unassigned_detections[i];

        new_tracks.id = g_next_id;
        new_tracks.bbox_cv = det_cv[index];
        new_tracks.kalman_filter = init_kalman_filter(get_center_bbox_cv(new_tracks.bbox_cv));
        new_tracks.age = 1;
        new_tracks.total_visible_count = 1;
        new_tracks.consecutive_invisible_count = 0;

        tracks_objects.push_back(new_tracks);

        ++g_next_id;
    }
}

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
		visibility = (float)tracks[i].total_visible_count / (float)tracks[i].age;

		if ((visibility < visibility_threshold && tracks[i].age < age_threshold) || tracks[i].consecutive_invisible_count > invisible_for_too_long)
		{
			tracks.erase(tracks.begin() + i);
		}
	}
}

static void
update_unassigned_tracks(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv, std::vector<int>& unassignments_tracks)
{
    int i, idx;

    for( i = 0; i < (int)unassignments_tracks.size(); ++i )
    {
        idx = unassignments_tracks[i];

        ++tracks_objects[idx].age;
        ++tracks_objects[idx].consecutive_invisible_count;
    }
}

static void
update_assigned_tracks(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv, std::vector<assignment_idx>& assignments_indexs)
{
    int i, track_idx, det_idx;

    for( i = 0; i < (int)assignments_indexs.size(); ++i )
    {
        track_idx = assignments_indexs[i].track_idx;
        det_idx = assignments_indexs[i].det_idx;

        cv::Rect bbox_cv_det = det_cv[det_idx];
        cv::Point det_center_point = get_center_bbox_cv(det_cv[det_idx]);

//        correct(tracks_objects[track_idx].kalman_filter, det_center_point);

        tracks_objects[track_idx].bbox_cv = bbox_cv_det;

        ++tracks_objects[track_idx].age;
        ++tracks_objects[track_idx].total_visible_count;
        tracks_objects[track_idx].consecutive_invisible_count = 0;
    }
}

static void
detection_to_track_assignment(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv,
                                std::vector<assignment_idx>& assignments_indexs, std::vector<int>& unassignments_tracks, std::vector<int>& unassigned_detections)
{
    int i, j;
    double distance, cost;
    const int distance_th = 30;
    HungarianAlgorithm hung_algo;
    std::vector<int> assignment;
    std::vector<std::vector<double>> cost_matrix( det_cv.size(), std::vector<double> ( tracks_objects.size(), distance_th));
    std::vector<std::vector<int>> cost_matrix_det_idx( det_cv.size(), std::vector<int> ( tracks_objects.size(), distance_th));

    if( tracks_objects.size() == 0 )
    {
        for( i = 0; i < (int)det_cv.size(); ++i )
        {
            unassigned_detections.push_back(i);
        }
        return;
    }

    // unassigned_detections
    for( i = 0; i < (int)det_cv.size(); ++i )
    {
        bool flag = 0;
        for( j = 0; j < (int)tracks_objects.size(); ++j )
        {
            cv::Point center_point1 = get_center_bbox_cv(tracks_objects[j].bbox_cv);
            cv::Point center_point2 = get_center_bbox_cv(det_cv[i]);

            distance = sqrt( pow(center_point1.y - center_point2.y, 2) + pow((center_point1.x - center_point2.x), 2) );

//            std::cout << center_point1 << center_point2 << distance << "\n";

            if( distance < distance_th )
            {
                flag = 1;
                cost_matrix[i][j] = distance;
            }
        }

        if( flag == 0)
            unassigned_detections.push_back(i);
    }


    for(int k=0;k < (int)det_cv.size();++k )
    {
        for( int l=0; l < (int)tracks_objects.size() ;++l)
        {
            std::cout << cost_matrix[k][l] << " ";
        }
        std::cout << "\n";
    }

    cost = hung_algo.Solve(cost_matrix, assignment);
    fprintf(stdout, "%s(): Hung algo cost: [%.1f] \n", __func__, cost);
    
    for( i = 0; i < (int)cost_matrix.size(); ++i )
    {
        fprintf(stdout, "%d,%d \t", i, assignment[i]);

        if( assignment[i] != -1 && cost_matrix[i][assignment[i]] != distance_th )// assignments_indexs
        {
            assignment_idx temp;
            temp.track_idx = assignment[i];
            temp.det_idx = i;
            assignments_indexs.push_back(temp);
        }
        else if( assignment[i] != -1 && cost_matrix[i][assignment[i]] == distance_th ) // // unassignments_tracks
        {
            unassignments_tracks.push_back(assignment[i]);
        }
    }
    fprintf(stdout, "\n");
}

void
update_tracks(std::vector<struct tracks>& tracks_objects, std::vector<cv::Rect>& det_cv)
{
    std::vector<assignment_idx> assignments_indexs;
    std::vector<int> unassigned_detections, unassignments_tracks;

    detection_to_track_assignment(tracks_objects, det_cv, 
                                                    assignments_indexs, unassignments_tracks, unassigned_detections);

    update_assigned_tracks(tracks_objects, det_cv, 
                                            assignments_indexs);

    update_unassigned_tracks(tracks_objects, det_cv, 
                                                unassignments_tracks);

    delete_lost_tracks(tracks_objects);

    create_new_tracks(tracks_objects, det_cv,
                                        unassigned_detections);
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

        predicted_centroid = predict(tracks_objects[i].kalman_filter);

        predicted_centroid.x = predicted_centroid.x - ((float)bbox_cv.width / 2.0);
        if( predicted_centroid.x < 0)
            predicted_centroid.x = 0;

        predicted_centroid.y = predicted_centroid.y - ((float)bbox_cv.height / 2.0);
        if( predicted_centroid.y < 0)
            predicted_centroid.y = 0;
   
        bbox_cv.x = (int)round(predicted_centroid.x);
        bbox_cv.y = (int)round(predicted_centroid.y);
        printf("%s: %d %d %d %d \n", __func__, bbox_cv.x, bbox_cv.y, bbox_cv.width, bbox_cv.height);

        tracks_objects[i].bbox_cv = bbox_cv;
    }
}

void
draw_tracks(std::vector<struct tracks>& tracks_objects, cv::Mat& m)
{
    int i;
    char id[32];

    for( i = 0; i < (int)tracks_objects.size(); ++i )
    {
        draw_bbox_cv(tracks_objects[i].bbox_cv, m);

        // put id in center
        cv::Point center = get_center_bbox_cv(tracks_objects[i].bbox_cv);
        sprintf(id, "%d", tracks_objects[i].id);
        cv::putText(m, id, center, 
            cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
            1.5, // Scale. 2.0 = 2x bigger
            cv::Scalar(0, 0, 255), // BGR Color
            2, // Line Thickness (Optional)
            cv:: LINE_AA); // Anti-alias (Optional, see version note)
    }
}
