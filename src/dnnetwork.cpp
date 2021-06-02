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

void
close_network(struct dnnetwork* dnnet)
{
	free_network_ptr(dnnet->net);
}


static std::vector<cv::Rect>
detections_to_opencv_rect(const detection* det, int det_cnt, const int* det_filtered_flag, cv::Mat& m)
{
	int i;
	detection* temp_det_ptr;
	std::vector<cv::Rect> det_cv;
    cv::Rect bbox_cv;

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

		int left_x = kx - (kw / 2);
		int top_y = ky - (kh / 2);

        if (left_x < 0)
            left_x = 0;
        else if ( left_x > m.cols  )
            left_x = m.cols;

        if ( top_y < 0 )
            top_y = 0;
        else if ( top_y > m.rows )
            top_y = m.rows;

        if ( kw < 0)
            kw = 0;
        else if( kw > m.cols )
            kw = m.cols;

        if ( kh < 0)
            kh = 0;
        else if( kw > m.rows )
            kw = m.rows;

        bbox_cv.x = left_x;
        bbox_cv.y = top_y;
        bbox_cv.width = kw;
        bbox_cv.height = kh;

		det_cv.push_back(bbox_cv);
	}

	return det_cv;
}

std::vector<cv::Rect>
detect_objects(const struct dnnetwork* dnnet, cv::Mat& m, int debug)
{
	detection *det, *temp_det_ptr;
	int det_cnt, i, j;
	float max, thresh_normalized;
	int* det_filtered_flag;
	image im;
	std::vector<cv::Rect> det_cv;
	double start_time;

	if(debug)
	{
		start_time = what_time_is_it_now();
	}
	else
	{
		start_time = 0;
	}

	im = mat_to_image(m);
	network_predict_image(dnnet->net, im);

	thresh_normalized = dnnet->detect_thres / 100.;
	det = get_network_boxes(dnnet->net, dnnet->net->w, dnnet->net->h, thresh_normalized, 0.5, 0, 1, &det_cnt, 0);

	do_nms_sort(det, det_cnt, det->classes, thresh_normalized);

	det_filtered_flag = (int*)malloc(det_cnt * sizeof(int));
	if (det_filtered_flag == NULL)
	{
		return det_cv;
	}

	for (i = 0; i < det_cnt; ++i)
	{
		max = 0;
		det_filtered_flag[i] = -1;

		for (j = 0; j < 1; ++j ) // !! only person id
        // for (j = 0; j < det->classes; ++j)
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

	det_cv = detections_to_opencv_rect(det, det_cnt, det_filtered_flag, m);

	free_detections(det, det_cnt);
	free_image(im);
	free(det_filtered_flag);

	if(debug)
	{
		fprintf(stdout, "%s(): Elapsed time: [%.1f] milisec, Detected object: [%d] \n", __func__, (what_time_is_it_now() - start_time) * 1000, (int)det_cv.size());
	}

	return det_cv;
}

