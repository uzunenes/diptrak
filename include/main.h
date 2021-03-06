#ifndef MAIN_H
#define MAIN_H

int
read_ini_file(const char* file_name, bool* app_debug, bool* app_show_frame, struct dnnetwork* dnnet, bool* tracking_use_kalman_filter_is_enable, int* tracking_distance_thresh_pixel, int* tracking_invisible_for_too_long_thresh, int* tracking_age_thresh, float* tracking_visibility_thres, char* video_stream_source_name, char* video_stream_output_name);

void
add_exit_signals(void);

void
exit_signal(int signal);

#endif // MAIN_H