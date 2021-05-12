#include "../include/utils.h"
#include <cstddef>
#include <sys/time.h>

double
what_time_is_it_now(void)
{
	struct timeval time;
	if (gettimeofday(&time, NULL))
	{
		return 0;
	}
	return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
