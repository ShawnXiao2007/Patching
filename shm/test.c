#include <sys/time.h>

int main()
{
	struct timeval tv;
	struct timezone tz;
	if(!gettimeofday(&tv,&tz))
	{
		printf("tv.time_t:%d",tv.tv_sec);
		printf("tv.suseconds_t:%d",tv.tv_usec);
	}
	return 1;
}
