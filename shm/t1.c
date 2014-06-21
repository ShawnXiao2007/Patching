#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <sys/mman.h>   /* mprotect */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */
#include <signal.h>     /* Signal */
#include <fcntl.h>      /* File */
#include <sys/stat.h>   /* File */

#define SHMSZ 4096*1024*100    //400MB
#define TIMER 4000000    // 1000000us = 1s

void *shm;
pthread_t main_thread;
pid_t logger_pid;
int fd;
void *buf, *buf1, *buf_tmp;
int size, size1, size_tmp;

inline void switch_buf()
{
    //swap buf and buf1
    buf_tmp = buf;
    buf = buf1;
    buf1 = buf_tmp;
    //swap size and size1
    size_tmp = size;
    size = size1;
    size1 = size_tmp;
}

void inline write_to_log()
{
    if (write(fd, buf, size) != size) {
        perror("write");
        exit(1);
    }
    switch_buf();
}

void * timer_thread(void * arg)
{
    // wait for the program to start
    sem_t *s = (sem_t *)shm;
    sem_init(s, 1, 0);
    sem_wait(s);
    
    logger_pid = *(pid_t *)(shm + sizeof(sem_t));
    printf("logger_pid=%d\n", logger_pid);

    while(1)
    {
        usleep(TIMER);
        pthread_kill(main_thread, SIGUSR1);
        write_to_log();
    }
}

void * t1(void * arg)
{
	pid_t pid=getpid();
	printf("***This is child thread pid: %d\n",pid);
	int i=0;
	while(i<5)
	{
		i++;
		usleep(1000000);
		printf("*** \n");
	}
}

void clean_exit()
{
    close(fd);
    shmdt(shm);
}

void test()
{
    printf("SIGUSR1 received\n");
    kill(logger_pid, SIGUSR1);
}

void main1()
{
    int shmid;
    key_t key;
    size_t size1, size2;

    //open log file
    fd = open("./log.txt", O_CREAT | O_WRONLY, 0666);
    if (fd < 0) {
        perror("fopen");
    }

    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (sem_t *) -1) {
        perror("shmat");
        exit(1);
    }
    // shmdt() at exit
    atexit(clean_exit);

    // calculate base and boundary
    void *base1,*base2,*boundary1,*boundary2;
    base1 = shm + sizeof(sem_t) + sizeof(pid_t); // sem takes 16 size_t, pid_t takes 1 size_t
    base2 = shm + SHMSZ/2;
    boundary1 = base2 - 4096;
    boundary2 = shm + SHMSZ -4096;
    printf("base1: %p, boundary1: %p, size: %.2fMB\nbase2: %p, boundary2: %p, size: %.2fMB\n", base1, boundary1, (boundary1-base1)*1.0/1024/1024, base2, boundary2, (boundary2-base2)*1.0/1024/1024);

    //init buf and pos
    buf = base1;
    buf1 = base2;
    size = boundary1 - base1;
    size1 = boundary2 - base2;

    // create timer thread
    main_thread = pthread_self();
    pthread_t timer_thr;
    pthread_create(&timer_thr, NULL, timer_thread, NULL);

    //register SIGUSR1 handler
    struct sigaction sa;
    sa.sa_handler = test;
    sigaction(SIGUSR1, &sa, NULL);

    pthread_join(timer_thr, NULL);

    exit(0);
}

void main()
{
	pid_t pid;
	pthread_t tchild;
	while(1)
	{
		pid=getpid();
		printf("This is main process pid: %d\n",pid);
		pthread_create(&tchild,NULL,t1,NULL);
		usleep(10000000);
		pthread_join(tchild,NULL);
	}
}
