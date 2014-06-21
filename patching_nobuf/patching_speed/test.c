//can a external bin access the external var in a original bin? if it can, then a library can access the global variable in the file and 

#include <stdio.h>
#include <sys/types.h>
#include "PtrDeref.h"
#include <semaphore.h>

int main()
{
unsigned short us=3;
unsigned short * p=&us;
printf("p:%p\n",p);
p++;
printf("p:%p\n",p);
p++;
printf("p:%p\n",p);

sem_t sem;
printf("size of sem: %d\n",sizeof(sem));
//printf("testint: %d\n",testint);
//    printf("pid_t: %d\n", sizeof(pid_t));
//    printf("uid_t: %d\n", sizeof(uid_t));
//    printf("gid_t: %d\n", sizeof(gid_t));
}

