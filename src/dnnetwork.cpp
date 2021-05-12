#include "../include/dnnetwork.h"
#include "../include/image_opencv.h"
#include "../include/utils.h"

static image
mat_to_image(cv::Mat& mat)
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

	cuda_set_device(dnnet->gpu_id);

	clear = 0;
	dnnet->net = load_network(dnnet->conf_file, dnnet->weights_file, clear);
	if (dnnet->net == NULL)
	{
		fprintf(stderr, "%s(): Failed to load dnnetwork. \n", __func__);
		return -1;
	}

	fprintf(stdout, "%s(): Loaded net. w: [%d], h: [%d], inputs: [%d], outputs: [%d] \n", __func__, dnnet->net->w, dnnet->net->h, dnnet->net->inputs, dnnet->net->outputs);

	return 0;
}

int
close_network(struct dnnetwork* dnnet)
{
	free_network_ptr(dnnet->net);

	return 0;
}

static int
draw_detections(const detection* det, int det_cnt, const int* det_filtered_flag, cv::Mat& m)
{
	int i;
	detection* temp_det_ptr;

	for (i = 0; i < det_cnt; ++i)
	{
		if (det_filtered_flag[i] == -1)
		{
			continue;
		}

		temp_det_ptr = (detection*)(det + i);

		int kw = m.cols * temp_det_ptr->bbox.w;
		int kh = m.rows * temp_det_ptr->bbox.h;
		int kx = m.cols * temp_det_ptr->bbox.x;
		int ky = m.rows * temp_det_ptr->bbox.y;

		int left = (kx) - (kw / 2);
		int top = (ky) - (kh / 2);
		int right = (kx) + (kw / 2);
		int bottom = (ky) + (kh / 2);

		cv::rectangle(m, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(255, 100, 0), 2, 0);
	}

	return 0;
}

int
predict_image(const struct dnnetwork* dnnet, cv::Mat& m)
{
	double start_time = what_time_is_it_now();
	detection *det, *temp_det_ptr;
	int det_cnt, i, j;
	float max, thresh_normalized;
	int* det_filtered_flag;
	image im;

	im = mat_to_image(m);
	network_predict_image(dnnet->net, im);

	thresh_normalized = dnnet->detect_thres / 100.;
	det = get_network_boxes(dnnet->net, dnnet->net->w, dnnet->net->h, thresh_normalized, 0.5, 0, 1, &det_cnt, 0);

	do_nms_sort(det, det_cnt, det->classes, thresh_normalized);

	det_filtered_flag = (int*)malloc(det_cnt * sizeof(int));
	if (det_filtered_flag == NULL)
	{
		return -1;
	}

	for (i = 0; i < det_cnt; ++i)
	{
		max = 0;
		det_filtered_flag[i] = -1;

		for (j = 0; j < det->classes; ++j)
		{
			temp_det_ptr = det + i;

			if (temp_det_ptr->prob[j] < thresh_normalized)
			{
				temp_det_ptr->prob[j] = 0;
				continue;
			}

			if (max < temp_det_ptr->prob[j])
			{
				det_filtered_flag[i] = 1;
				max = temp_det_ptr->prob[j];
			}
		}
	}

	draw_detections(det, det_cnt, det_filtered_flag, m);

	free_detections(det, det_cnt);
	free_image(im);
	free(det_filtered_flag);

	fprintf(stdout, "%s(): Elapsed time: [%.1f] milisec, Detected object: [%d] \n", __func__, (what_time_is_it_now() - start_time) * 1000, det_cnt);

	return 0;
}