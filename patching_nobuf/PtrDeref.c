#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/mman.h>  /* Primitive System Data Types */

int testint=1989;

//using namespace std;
// Define the function to be called when ctrl-c (SIGINT) signal is sent to process

extern void* shmPtr_xgy=NULL;
extern unsigned short** basePtrAddr=NULL;

void
signal_callback_handler(int signum)
{
	//debug
	//printf("signal caught. dif= %d\n",(unsigned long)(*basePtrAddr)-(unsigned long)(shmPtr_xgy+268435456));
   //printf("shmPtr_xgy: %p\nbasePtrAddr: %p\n*basePtrAddr :%p\n",shmPtr_xgy,basePtrAddr,*basePtrAddr);
   //printf("Caught signal %d\n",signum);
   // Cleanup and close up stuff here
	
	*basePtrAddr= (unsigned long)(*basePtrAddr)<(unsigned long)(shmPtr_xgy+268435456) ? (shmPtr_xgy+268435456) : shmPtr_xgy;
	//debug
	int s1,s2,s3,s4,s5;
	//sem_getvalue(shmPtr_xgy-80,&s1);
	//printf("s1:%d\n",s1);
	if(*basePtrAddr==shmPtr_xgy)
	{
                //debug
               // sem_getvalue(shmPtr_xgy-80,&s3);
                //printf("s1 is %d\n",s1);
		//buf1 is ready to write and buf2 is ready to read
                sem_wait(shmPtr_xgy-64-16);//wait consumer finish reading buf1, so taht it can write buf1
		memset(*basePtrAddr,255,268435456);
		sem_post(shmPtr_xgy-16-16);//post buf2, so that consumer can read it
		//debug
		//printf("sem4 posted\n");	
		//sem_wait(shmPtr_xgy-64-16);//wait consumer finish reading buf1, so taht it can write buf1
	}
	else
	{
		//debug
		//sem_getvalue(shmPtr_xgy-48,&s3);
		//printf("s3 is %d\n",s3);
		//buf2 is ready to write and buf1 is ready to read; wait sem1
                sem_wait(shmPtr_xgy-32-16);//wait consumer finish reading buf2, so that it can write buf2
                memset(*basePtrAddr,255,268435456);
		sem_post(shmPtr_xgy-48-16);//post buf1, so that consumer can read it
		//debug
		//printf("sem2 posted\n");
		//sem_wait(shmPtr_xgy-32-16);//wait consumer finish reading buf2, so that it can write buf2
	}
   // Terminate program
}

void*
shm_sig(unsigned short** p)
{
	//printf("shm_sig\n");
	basePtrAddr=p;
	key_t key=5678;
	int shmSize=536870912+4+32+32+16;//12,12+268435456
	int shmFlag=(IPC_CREAT | 0666);
	int shmid=0;
	if((shmid=shmget(key,shmSize,shmFlag))<0){
		perror("shmget");
		exit(1);
	}
	shmPtr_xgy=shmat(shmid,NULL,0);
	if(shmPtr_xgy == (void *)-1){
		perror("shmat");
		exit(1);
	}
	int* pid=(int*)shmPtr_xgy;
	sem_t *semp1=(sem_t*)(shmPtr_xgy+4);//buf1 producer write
	sem_t *semp2=(sem_t*)(shmPtr_xgy+20);//buf1 producer read
	sem_init(semp1,1,1);
	sem_init(semp2,1,0);
        sem_t *semp3=(sem_t*)(shmPtr_xgy+4+32);//buf2 producer write
        sem_t *semp4=(sem_t*)(shmPtr_xgy+20+32);//buf2 producer read
        sem_init(semp3,1,1);
        sem_init(semp4,1,0);
	sem_t *semp0=(sem_t*)(shmPtr_xgy+4+64);//sem0 to kill the child threads, set to 1 when finish logging
	sem_init(semp0,1,0);
	//sem_wait(semp1);
        //memset(shmPtr_xgy,0,84);
	shmPtr_xgy=shmPtr_xgy+4+32+32+16;
        memset(shmPtr_xgy,255,268435456*2);
	unsigned long boundary=(unsigned long)shmPtr_xgy+268435456;
	boundary=4096*(boundary/4096-1);
   	/*if (mprotect((void *)boundary, 4096 - 1, PROT_NONE)) {
        	perror("mprotect");
        	exit(1);
    	}*/	
	*pid=getpid();
	sem_post(semp0);
	//debug
	//printf("sem0 posted ...\n");
	//printf("pid: %d\n*pid: %p\n",*pid,pid);
	//printf("shmPtr_xgy: %p\n",shmPtr_xgy);
	//signal(SIGUSR1,signal_callback_handler);
                //debug
		//int s1,s2,s3,s4;
                //sem_getvalue(shmPtr_xgy-48,&s3);
		//sem_getvalue(shmPtr_xgy-80,&s1);
                //printf("s1 is %d, s3 is %d\n",s1,s3);

        //printf("waiting for semp2 ...\n");
        //sem_wait(semp2);
        //printf("semp2 wait finished ...\n");

	return shmPtr_xgy;
}

void* stoplogger()
{
	sem_t *semp0=(sem_t*)(shmPtr_xgy-16);
	sem_post(semp0);
	if((unsigned long)(*basePtrAddr)<(unsigned long)(shmPtr_xgy+268435456))
	{
		sem_post((sem_t*)(shmPtr_xgy-64));
		//debug
		//printf("...semp2 posted. finish.\n");
	}
	else
	{
		sem_post((sem_t*)(shmPtr_xgy-32));
                //debug
                //printf("...semp4 posted. finish.\n");
	}
}
/*
int main1()
{
   // Register signal and signal handler
   printf("pid is: %d\n",getpid());
   signal(SIGUSR1, signal_callback_handler);

   while(1)
   {
      printf("Program processing stuff here.\n");
      sleep(1);
   }
   return EXIT_SUCCESS;
}
*/
/*
bool bufIsFull(long * semaphorePtr)
{
	if ((*semaphorePtr)<16381)
		return false;
	else
		return true;
}

void plus(long * semaphorePtr)
{
	(*semaphorePtr)++;
}

void setInitialSemaphore(long * semaphorePtr)
{
	*semaphorePtr=0;
}

long getSemaValue(long* semaphorePtr)
{
//	printf("get semaValue: semaphore value is: %d \n",*semaphorePtr);
	return *semaphorePtr;
}

void printSemaValue(long* semaphorePtr)
{
	printf("semaphore value is: %d \n",*semaphorePtr);
}
*/


int main()
{
}
