#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "alloc.h"


typedef struct memstr{
		//8바이트단위 메모리 할당 여부 배열
		int memarr[PAGESIZE/8];
		//얼만큼의 size씩 할당했는지 기록하는 배열
		int memoffset[PAGESIZE/8];

} memstr;


char *mem=NULL;
char buf[4096]={0x00,};
int flag=PROT_WRITE|PROT_READ;

memstr m;




int init_alloc(){

		if((mem=mmap(0,PAGESIZE,flag,MAP_PRIVATE|MAP_ANONYMOUS,-1,0))==MAP_FAILED){
				perror("mmap error\n");
				exit(1);
		}
		
		for(int i=0;i<PAGESIZE/8;i++){
				m.memarr[i]=0;
				m.memoffset[i]=0;
		}
		return 0;

}
int cleanup(){
		if(munmap(mem,PAGESIZE)==-1){
				perror("munmap error\n");
				exit(1);
		}
		return 0;

}
char *alloc(int a){
		if(a%8!=0|| a>PAGESIZE)
				return NULL;

		int count=a/8;
		for(int i=0;i<PAGESIZE;i++){
				//a칸 만큼 공간이 있는지 확인
				if(m.memarr[i]==0){
						count--;
						//여기서 시작 i 값 기록하도록해서 가독성 높임

				}else{
						count=a/8;
				}
				if(count==0){
						for(int j=0;j<a/8;j++){
								m.memarr[i-a/8+1+j]=1;
						}
						m.memoffset[i-a/8+1]=a/8;
						return mem+8*(i-a/8+1);
				}
				

		}
		return NULL;


}
void dealloc(char * a){
		int dsize=(a-mem)/8;
		for(int i=0;i<m.memoffset[dsize];i++){
				m.memarr[dsize+i]=0;
		}

		
}

/*
int main(){
		int count=0;

		init_alloc();
		for(int i=0;i<PAGESIZE/8;i++){
				if(m.memarr[i]==0)
						count++;
		}
		//testing
		char *str = alloc(512);
		char *str2=alloc(512);

		if(str == NULL|| str2==NULL)
		{
			printf("alloc failed\n");
			return 1;
		}

		strcpy(str, "Hello, world!");
		strcpy(str2,"Bye world!");
		printf("str: %s\nstr2:%s\n",str,str2);
		dealloc(str);

		printf("memarr: ");
		for(int i=0;i<PAGESIZE/8;i++){
				printf("%d",m.memarr[i]);
		}

		str=alloc(16);
		printf("memarr: ");
		for(int i=0;i<PAGESIZE/8;i++){
				printf("%d",m.memarr[i]);
		}


		cleanup();

}
*/

