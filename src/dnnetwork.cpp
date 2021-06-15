#include "../include/dnnetwork.h"
#include "../include/image_opencv.h"
#include "../include/utils.h"

struct filtered_detection_list_darknet
{
	int class_name_idx;
	float probability;
};

static image
mat_to_image(const cv::Mat& mat)
{
	int w = mat.cols;
	int h = mat.rows;
	int c = mat.channels();

	image im = make_image(w, h, c);

	unsigned char* data = (unsigned char*)mat.data;
	int step = mat.step;
	for (int y = 0; y < h; ++y)
	{
		for (int k = 0; k < c; ++k)
		{
			for (int x = 0; x < w; ++x)
			{
				im.data[k * w * h + y * w + x] = data[y * step + x * c + k] / 255.0f;
			}
		}
	}

	return im;
}

int
load_dnnetwork(struct dnnetwork* dnnet)
{
	int clear;
	std::ifstream obj_names_file;
	std::string name;

	cuda_set_device(dnnet->gpu_id);

	clear = 0;
	dnnet->net = load_network(dnnet->conf_file, dnnet->weights_file, clear);
	if (dnnet->net == NULL)
	{
		fprintf(stderr, "%s(): Failed to load dnnetwork. \n", __func__);
		return -1;
	}

	fprintf(stdout, "%s(): Loaded net. w: [%d], h: [%d], inputs: [%d], outputs: [%d] \n", __func__, dnnet->net->w, dnnet->net->h, dnnet->net->inputs, dnnet->net->outputs);

	// read obj names
	obj_names_file.open(dnnet->obj_names_file);
	if (!obj_names_file.is_open())
	{
		fprintf(stderr, "%s(): Failed to read obj names file: [%s] \n", __func__, dnnet->obj_names_file);
		return -2;
	}

	while (1)
	{
		if (!getline(obj_names_file, name))
		{
			break;
		}
		dnnet->obj_names.push_back(name);
	}

	fprintf(stdout, "%s(): Readed total: [%d] obj names \n", __func__, (int)dnnet->obj_names.size());

	return 0;
}

void
close_network(struct dnnetwork* dnnet)
{
	free_network_ptr(dnnet->net);
}

static std::vector<struct det_cv>
detections_to_det_cv(const struct dnnetwork* dnnet, const detection* det, int det_cnt, const struct filtered_detection_list_darknet* filtered_detection_list, const cv::Mat& m)
{
	int i;
	std::vector<struct det_cv> detection_list;

	for (i = 0; i < det_cnt; ++i)
	{
		if (filtered_detection_list[i].class_name_idx < 0)
		{
			continue;
		}

		detection* temp_det_ptr = (detection*)(det + i);

		int kw = m.cols * temp_det_ptr->bbox.w;
		int kh = m.rows * temp_det_ptr->bbox.h;
		int kx = m.cols * temp_det_ptr->bbox.x;
		int ky = m.rows * temp_det_ptr->bbox.y;

		int left_x = kx - (kw / 2);
		int top_y = ky - (kh / 2);

		if (left_x < 0)
			left_x = 0;
		else if (left_x > m.cols)
			left_x = m.cols;

		if (top_y < 0)
			top_y = 0;
		else if (top_y > m.rows)
			top_y = m.rows;

		if (kw < 0)
			kw = 0;
		else if (kw > m.cols)
			kw = m.cols;

		if (kh < 0)
			kh = 0;
		else if (kw > m.rows)
			kw = m.rows;

		struct det_cv temp_det_cv;
		cv::Rect bbox_cv;

		bbox_cv.x = left_x;
		bbox_cv.y = top_y;
		bbox_cv.width = kw;
		bbox_cv.height = kh;

		temp_det_cv.name = dnnet->obj_names[filtered_detection_list[i].class_name_idx];
		temp_det_cv.bbox_cv = bbox_cv;
		temp_det_cv.pred_score = filtered_detection_list[i].probability;

		detection_list.push_back(temp_det_cv);
	}

	return detection_list;
}

std::vector<struct det_cv>
detect_objects(const struct dnnetwork* dnnet, const cv::Mat& m, int debug)
{
	int det_cnt, i, j;
	double thresh_normalized, start_time = 0;
	detection* det;
	image im;
	std::vector<struct det_cv> detection_list;
	struct filtered_detection_list_darknet* filtered_detection_list;

	if (debug)
	{
		start_time = what_time_is_it_now();
	}

	im = mat_to_image(m);
	network_predict_image(dnnet->net, im);

	thresh_normalized = dnnet->detect_thres / 100.;
	det = get_network_boxes(dnnet->net, dnnet->net->w, dnnet->net->h, thresh_normalized, 0.5, 0, 1, &det_cnt, 0);

	do_nms_sort(det, det_cnt, det->classes, thresh_normalized);

	filtered_detection_list = (struct filtered_detection_list_darknet*)malloc(det_cnt * sizeof(struct filtered_detection_list_darknet));
	if (filtered_detection_list == NULL)
	{
		return detection_list;
	}

	for (i = 0; i < det_cnt; ++i)
	{
		float max = 0;
		filtered_detection_list[i].class_name_idx = -1;

		for (j = 0; j < det->classes; ++j)
		{
			detection* temp_det_ptr = det + i;

			if (temp_det_ptr->prob[j] < thresh_normalized)
			{
				continue;
			}

			if (max < temp_det_ptr->prob[j])
			{
				filtered_detection_list[i].class_name_idx = j;
				filtered_detection_list[i].probability = temp_det_ptr->prob[j];
				max = temp_det_ptr->prob[j];
			}
		}
	}

	detection_list = detections_to_det_cv(dnnet, det, det_cnt, filtered_detection_list, m);

	free_detections(det, det_cnt);
	free_image(im);
	free(filtered_detection_list);

	if (debug)
	{
		fprintf(stdout, "%s(): Elapsed time: [%.1f] milisec, Detected object: [%d] \n", __func__, (what_time_is_it_now() - start_time) * 1000, (int)detection_list.size());
	}

	return detection_list;
}
