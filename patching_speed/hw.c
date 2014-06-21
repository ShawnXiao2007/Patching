#include <stdio.h>

int main()
{
	unsigned short test=123;
	printf("hello world: %p\n",&test);
	printf("test: %hu\n",test);
	while(1){
		unsigned short * p=NULL;
		scanf("%p",&p);
		if(p==0)
		{
			printf("...end\n");
			exit(0);
		}
		printf("p is: %p\n",p);
		printf("*p is: %hu\n",*p);
	}
}
