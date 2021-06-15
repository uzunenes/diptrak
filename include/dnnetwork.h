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
	std::vector<std::string> obj_names;
};

int
load_dnnetwork(struct dnnetwork* dnnet);

void
close_network(struct dnnetwork* dnnet);

std::vector<struct det_cv>
detect_objects(const struct dnnetwork* dnnet, const cv::Mat& m, int debug);

#endif // DNNETWORK_H
