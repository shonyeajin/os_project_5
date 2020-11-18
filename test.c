#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "ealloc.h"

int main(){
		char * arr[2];
		arr[0]=mmap(0,4096,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
		arr[1]=mmap(0,4096,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
		strcpy(arr[0],"hello world");
		strcpy(arr[1],"bye world");

		printf("%s,%s\n",arr[0],arr[1]);

		if(arr[0]==arr[1])
				printf("same");

}
