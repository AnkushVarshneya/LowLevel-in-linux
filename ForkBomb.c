#include <stdio.h>
	#include <stdlib.h>

	int main()
	{
			int pid;
			int num;
			int i = 0;
			printf("Enter Number of forks:");
			scanf("%d", &num);

			while(i<num){
					printf("I am process  %d!\n",++i);
					sleep(1);
					if(!fork()) exit(0);
			  }
	}
