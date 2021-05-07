#include "../include/Version.h"
#include "../include/network.h"
#include "../include/config_file.h"
#include "../include/image_opencv.h"

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char** argv)
{
    fprintf(stdout, "%s(): dvmot started, version: [%s] .\n", __func__, Version);


	return 0;
}
