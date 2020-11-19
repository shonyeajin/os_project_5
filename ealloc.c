#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "ealloc.h"

#define PAGEIDX PAGESIZE/MINALLOC
#define MEMIDX PAGESIZE/MINALLOC*4
/*
void printvsz(char *hint) {
  char buffer[256];
  sprintf(buffer, "echo -n %s && echo -n VSZ: && cat /proc/%d/stat | cut -d\" \" -f23", hint, getpid());
  system(buffer);
  //getchar();
}
*/
typedef struct memstr{
		//메모리 할당 여부 배열
		int memarr[MEMIDX];
		//할당된 배열의 시작 인덱스 칸에 어느정도의 인덱스만큼 저장되었는지 저장
		int memoffset[MEMIDX];
		//몇개의 페이지가 alloc되어 있는지 count하는 변수
		int pcount;
		//몇개의 청크가 alloc되어 있는지 count하는 변수
		int ccount;
		char *mem[4];
		int isfirst;

} memstr;
memstr m;

int flag=PROT_WRITE|PROT_READ;
int checkfunc(char *a){
		for(int i=0;i<16;i++){
				if(a==(m.mem[0]+256*i)){
				//		printf("check func index:%d\n",i);
						return 0;
				}
		}
		for(int i=0;i<16;i++){
				if(a==(m.mem[1]+256*i)){
				//		printf("check func index:%d\n",i);
						return 1;
				}
		}
		for(int i=0;i<16;i++){
				if(a==(m.mem[2]+256*i)){
				//		printf("check func index:%d\n",i);
						return 2;
				}
		}
		for(int i=0;i<16;i++){
				if(a==(m.mem[3]+256*i)){
				//		printf("check func index:%d\n",i);
						return 3;
				}
		}
		printf("checkfunc: not found\n");

}


void init_alloc(void){
		for(int i=0;i<MEMIDX;i++){
				m.memarr[i]=0;
				m.memoffset[i]=0;
		}
		m.pcount=0;
		m.ccount=0;
		for(int i=0;i<4;i++){
				m.mem[i]=NULL;
		}
		m.isfirst=1;
}

char *alloc(int a){
		int start=0;
		int count=a/MINALLOC;//이만큼의 인덱스 필요함
		//할당 요청 -> 페이지 부족, 할당하는 함수 작성
		//m.count==4이면 searching만 수행하면 됨.
		//할당하면 그만큼 ccount도 올려주기
		if(m.pcount<4){//m.count가 4가 아닐때 
				if(m.ccount%PAGEIDX==0||m.isfirst==1){//할당된 페이지가 모두 꽉찬상태
						if((m.mem[m.pcount]=mmap(0,PAGESIZE,flag,MAP_PRIVATE|MAP_ANONYMOUS,-1,0))==MAP_FAILED){
								perror("mmap error\n");
								exit(1);
						}
						m.pcount++;
						m.isfirst=0;
				}
				//alloc할 공간 searching
				for(int i=0;i<PAGEIDX*m.pcount;i++){
						if(m.memarr[i]==0){
								count--;
						}else{
								count=a/MINALLOC;
						}
						if(count==0){
								start=i-a/MINALLOC+1;
								for(int j=0;j<a/MINALLOC;j++){
										m.memarr[start+j]=1;
								}
								m.memoffset[start]=a/MINALLOC;
								m.ccount+=a/MINALLOC;
								
								//for return
								int ret=start/16;
								int retmod=start%16;
								//printf("return value:%d,%d,%d\nstart:%d\n",m.pcount,ret,retmod,start);
								return m.mem[ret]+retmod*MINALLOC;


								
						}
				}
				

				

		}else{//m.count가 4일 때 
				for(int i=0;i<MEMIDX;i++){
						if(m.memarr[i]==0){
								count--;
						}else{
								count=a/MINALLOC;
						}
						if(count==0){
								start=i-a/MINALLOC+1;
								for(int j=0;j<a/MINALLOC;j++){
										m.memarr[start+j]=1;
								}
								m.memoffset[start]=a/MINALLOC;
								//m.ccount+=a/MINALLOC;
								
								//for return
								int ret=start/16;
								int retmod=start%16;
								return m.mem[ret]+retmod*MINALLOC;

						}
				}



		}
	/*	printf("not allocated\n");
		printf("memarr:");
		for(int i=0;i<PAGESIZE/MINALLOC*4;i++){
				printf("%d",m.memarr[i]);
		}
		printf("\n");
*/

}

void dealloc(char *a){
		//어느페이지에 속하는지
		int ret=checkfunc(a);
		//printf("check func return num:%d\n",ret);
		for(int i=0;i<PAGESIZE/MINALLOC;i++){
				m.memarr[(PAGESIZE/MINALLOC)*ret+i]=0;
		}

/*
		int dsize=(a-m.mem[0])/MINALLOC;
		for(int i=0;i<m.memoffset[dsize];i++){
				m.memarr[dsize+i]=0;
		}
		//추가적으로 m.memoffset에 값 지워주기 근데 안지워도 상관없음
*/
}

void cleanup(void){
}
/*

int main(){
		init_alloc();
		//testing
		char *str=alloc(4096);
		char *str2=alloc(4096);
		char *str3=alloc(4096);
		char *str4=alloc(4096);

		strcpy(str,"hello world");
		strcpy(str2,"bye world");
		printf("str: %s\n",str);
		printf("str2: %s\n",str2);
		
		if(m.mem[0]==m.mem[1])
				printf("same\n");

		//printf("addr: %s\n",&str);


		dealloc(str2);



		printf("memarr:");
		for(int i=0;i<PAGESIZE/MINALLOC*4;i++){
				printf("%d",m.memarr[i]);
		}
		printf("\n");
}
*/
/*
int main()
{
  
  printf("\nInitializing memory manager\n\n");
  init_alloc();
  
  //Start tests

  printf("Test1: checking heap expansion; allocate 4 X 4KB chunks\n");

  printvsz("start test 1:");
  
  
  char *a[4];
  for(int i=0; i < 4; i++) {
    a[i] = alloc(4096);
  
  if(a[i]==NULL)
		  printf("\nhere\n");
  else
		  printf("addr: %s\n",a[i]);

    //write to chunk
    for(int j=0; j < 4096; j++)
      *(a[i]+j) = 'a';
    
    printvsz("should increase by 4KB:");
  }


  //read all content and verify;
  int mismatch=0;
  for(int i=0; i < 4; i++) {
    //read each chunk
    for(int j=0; j < 4096; j++)
      {
	char x = *(a[i]+j);
	if(x != 'a')
	  mismatch = 1;
      }
  }
    
  if(mismatch) {
    printf("ERROR: Chunk contents did not match\n");
    exit(1);
  }

  for(int i=0; i < 4; i++) {
    dealloc(a[i]);
  }

  printvsz("should not change:");
  printf("Test1: complete\n\n");

}
*/
