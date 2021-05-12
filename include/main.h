#ifndef MAIN_H
#define MAIN_H

int
read_ini_file(const char* file_name, int* debug, int* show_frame, struct dnnetwork* dnnet, char* video_stream_source_name, char* video_stream_output_name);

void
add_exit_signals(void);

void
exit_signal(int signal);

#endif // MAIN_H