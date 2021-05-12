#ifndef DNNETWORK_H
#define DNNETWORK_H

#include <darknet.h>
#include <opencv2/opencv.hpp>

struct dnnetwork
{
	// ini files
	int detect_thres;
	char obj_names_file[2048];
	char conf_file[2048];
	char weights_file[2048];
	int gpu_id;

	network* net;
};

int
load_dnnetwork(struct dnnetwork* dnnet);

int
close_network(struct dnnetwork* dnnet);

int
predict_image(const struct dnnetwork* dnnet, cv::Mat& m);

#endif // DNNETWORK_H
