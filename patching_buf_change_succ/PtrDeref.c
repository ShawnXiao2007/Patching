#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int testint=1989;

//using namespace std;
// Define the function to be called when ctrl-c (SIGINT) signal is sent to process

extern void* shmPtr_xgy=NULL;
extern unsigned short** basePtrAddr=NULL;

void
signal_callback_handler(int signum)
{
   //printf("shmPtr_xgy: %p\nbasePtrAddr: %p\n*basePtrAddr :%p\n",shmPtr_xgy,basePtrAddr,*basePtrAddr);
   //printf("Caught signal %d\n",signum);
   // Cleanup and close up stuff here
//   *basePtrAddr= *basePtrAddr==shmPtr_xgy ? shmPtr_xgy+268435456 : shmPtr_xgy;
	printf("--------Signal caught!----------\n");
   printf("shmPtr_xgy: %p\nbasePtrAddr: %p\n*basePtrAddr :%p\n",shmPtr_xgy,basePtrAddr,*basePtrAddr);
   *basePtrAddr= (*basePtrAddr==shmPtr_xgy)?(shmPtr_xgy+268435456):shmPtr_xgy;
   printf("shmPtr_xgy: %p\nbasePtrAddr: %p\n*basePtrAddr :%p\n",shmPtr_xgy,basePtrAddr,*basePtrAddr);
   if (shmPtr_xgy==*basePtrAddr)
   {
	printf("shmPtr_xgy==*basePtrAddr\n");
	*basePtrAddr=shmPtr_xgy+268435456;
        printf("shmPtr_xgy: %p\nbasePtrAddr: %p\n*basePtrAddr :%p\n",shmPtr_xgy,basePtrAddr,*basePtrAddr);
	printf("*****\n");
   }

   // Terminate program
}

void*
shm_sig(unsigned short** p)
{
	basePtrAddr=p;
	key_t key=5678;
	int shmSize=536870912;//12,12+268435456
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
	shmPtr_xgy=shmPtr_xgy+4;
	*pid=getpid();
	//printf("pid: %d\n*pid: %p\n",*pid,pid);
	//printf("shmPtr_xgy: %p\n",shmPtr_xgy);
	signal(SIGUSR1,signal_callback_handler);
	return shmPtr_xgy;
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
