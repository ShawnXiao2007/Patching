/*
 * shm-client - client program to demonstrate shared memory.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */
#include <fcntl.h>
#include <signal.h>

int main(int argc,char* argv[])
{
	int fd;
	fd=open(argv[1],O_CREAT | O_WRONLY, 0666);
	unsigned short buf[5];
        unsigned short buf2[5];
        unsigned short buf3[5];
	memset(buf,65535,10);
	write(fd,buf,10);
        memset(buf2,11111,10);
	write(fd,buf2,10);
        memset(buf3,22222,10);
	write(fd,buf3,10);
	close(fd);	
}
