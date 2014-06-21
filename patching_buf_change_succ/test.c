//can a external bin access the external var in a original bin? if it can, then a library can access the global variable in the file and 

#include <stdio.h>
#include <sys/types.h>
#include "PtrDeref.h"

int main()
{
	int a=10;
	int b=10;
	a= a==b?1:0;
	printf("a: %d\n",a);
//printf("testint: %d\n",testint);
//    printf("pid_t: %d\n", sizeof(pid_t));
//    printf("uid_t: %d\n", sizeof(uid_t));
//    printf("gid_t: %d\n", sizeof(gid_t));
}

