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
//#include <unistd.h>
#define SHMSZ     16
void* shm=NULL;
unsigned short* buf1;
unsigned short* buf2;
int numofwords=268435456/sizeof(unsigned short);
int fd;
int debug_mode=1;

void* writer1()
{
	sem_t* semp1=shm+4;
	sem_t* semp2=shm+4+16;
	sem_t* semp3=shm+4+32;
	sem_t* semp4=shm+4+48;
	int s1,s2,s3,s4,s5;
	//sem_getvalue(semp2,&s2);
	//sem_getvalue(semp1,&s1);
	//printf("s2:%d\ts1:%d\n",s2,s1);
	while(1)
	{		
		//debug
		//printf("begin wait semp2.......\n");
		sem_wait(semp2);
		//debug
		//printf("write buf 1\n");
		//from teh start point of the buf1, find the end point of the buffer
		int M=numofwords-1;
		int L=0;
		int R=numofwords-1;
		while((!(*(buf1+M)==65535 && *(buf1+M-1)!=65535)) && M)
		{
			if(*(buf1+M)==65535)
			{
				R=M;
				M=(L+R)/2;
			}
			else
			{
				L=M;
				M=(L+R)/2;
			}
		}
		write(fd,buf1,M*2);
		sem_post(semp1);
		//debug
		//sem_getvalue(semp1,&s1);
		//sem_getvalue(semp2,&s2);
		//printf("semp1 posted. s1 is : %d . s2 is %d .\n",s1,s2);
		printf("buf1 to file\n");
	}	
}

void* writer2()
{
        sem_t* semp1=shm+4;
        sem_t* semp2=shm+4+16;
        sem_t* semp3=shm+4+32;
        sem_t* semp4=shm+4+48;
	int s1,s2,s3,s4,s5;
	//sem_getvalue(semp4,&s4);
	//sem_getvalue(semp3,&s3);
	//printf("s4:%d\ts3:%d\n",s4,s3);
        while(1)
        {
		//debug
		//printf("begin wait semp4.............\n");
                sem_wait(semp4);
		//debug
		//printf("write buf 2\n");
		//binary search for the end of log
                int M=numofwords-1;
                int L=0;
                int R=numofwords-1;
                while((!(*(buf2+M)==65535 && *(buf2+M-1)!=65535)) && M)
                {
                        if(*(buf2+M)==65535)
                        {
                                R=M;
                                M=(L+R)/2;
                        }
                        else
                        {
                                L=M;
                                M=(L+R)/2;
                        }
                }
                write(fd,buf2,M*2);
		//debug
                sem_post(semp3);
                //sem_getvalue(semp3,&s3);
                //sem_getvalue(semp4,&s4);
                //printf("semp3 posted. s3 is : %d . s4 is %d .\n",s3,s4);
		printf("buf2 to file\n");
        }
}

void* sig_sender(int* tinteval)
{
        struct timeval end;
	int* pid=(int*)(shm);
	while(1){   	
       		usleep(*((int*)tinteval));
		//debug
		//printf("signalsent\n");
     		//kill(*pid,SIGUSR1);
	        //printf("producer pid: %d\tconsumer pid: %d\n",*pid,getpid());
		kill(*pid,SIGUSR1);
		//gettimeofday(&end,NULL);
        	//printf("sigal sent at %d.%d\n",end.tv_sec,end.tv_usec);

	}
}

main(int argc, char* argv[])
{
    	fd = open(argv[1], O_CREAT | O_WRONLY, 0666);
    	if (fd < 0) {
    	    	perror("fopen");
    	}

    	int shmid;
    	key_t key;
    /*
     * We need to get the segment named
     * "5678", created by the server.
     */
    	key = 5678;

    /*
     * Locate the segment.
     */
    	while((shmid = shmget(key, SHMSZ, 0666)) < 0)
    	{
    //if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        	//perror("shmget");
     //   exit(1);
    	}

    /*
     * Now we attach the segment to our data space.
     */
    //void* shm;
    	if ((shm = shmat(shmid, NULL, 0)) == (void *) -1) {
        	//perror("shmat");
        	exit(1);
    	}
	buf1=shm+4+80;
	buf2=buf1+268435456/(sizeof(unsigned short*));
	pthread_t writer_thread_1;
	pthread_t writer_thread_2;
	pthread_t sig_thread;
	int tintvlp=atoi(argv[2]);
        sem_t* semp0=shm+4+64;
        sem_t* semp1=shm+4;
	sem_t* semp2=shm+4+16;
        sem_t* semp3=shm+4+32;
        //debug
        printf("wait semp0 ...\n");
        sem_wait(semp0);
	pthread_create(&sig_thread,NULL,sig_sender,&tintvlp);
	//debug
	printf("post semp2 ...\n");
	sem_post(semp2);
	pthread_create(&writer_thread_1,NULL,writer1,NULL);
	pthread_create(&writer_thread_2,NULL,writer2,NULL);

	//debug
	int s1;
	sem_getvalue(semp1,&s1);
	printf("s1:%d\n",s1);
	sem_wait(semp0);
	sem_wait(semp1);
	sem_wait(semp3);
	int cancel1=pthread_cancel(writer_thread_1);
	int cancel2=pthread_cancel(writer_thread_2);
	int cancelsig=pthread_cancel(sig_thread);
    	close(fd); 
	struct timeval end;
	gettimeofday(&end,NULL);
	printf("end at %d.%d\n",end.tv_sec,end.tv_usec);
}
