#include "../include/Version.h"
#include <darknet.h>
#include <opencv2/opencv.hpp>

int
main(int argc, char** argv)
{
    fprintf(stdout, "%s(): dvmot started, version: [%s] .\n", __func__, Version);


	return 0;
}
