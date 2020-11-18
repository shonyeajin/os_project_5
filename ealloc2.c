#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "ealloc.h"

//나중에 지우기
/*
void printvsz(char *hint) {
  char buffer[256];
  sprintf(buffer, "echo -n %s && echo -n VSZ: && cat /proc/%d/stat | cut -d\" \" -f23", hint, getpid());
  system(buffer);
 // getchar();
}
*/


typedef struct memstr{
		//256바이트단위 메모리 할당 여부 배열
		int memarr[PAGESIZE/MINALLOC*4];
		//얼만큼의 size씩 할당됐는지 기록하는 배열
		int memoffset[PAGESIZE/MINALLOC*4];
		//몇개의 페이지가 alloc되어 있는지 count하는 변수
		int count;
		char *mem[4];
		int mempage[PAGESIZE/MINALLOC*4];

} memstr;
memstr m;

int flag=PROT_WRITE|PROT_READ;


void init_alloc(void){
		for(int i=0;i<PAGESIZE/MINALLOC*4;i++){
				m.memarr[i]=0;
				m.memoffset[i]=0;
				m.mempage[i]=-1;
		}
		m.count=0;
		for(int i=0;i<4;i++){
				m.mem[i]=NULL;
		}
}

char *alloc(int a){
		if(m.count==0){
				if(a%MINALLOC!=0||a>PAGESIZE )
						return NULL;
				//처음엔 페이지 하나 할당
				int count=a/MINALLOC;
				if((m.mem[m.count]=mmap(0,PAGESIZE,flag,MAP_PRIVATE|MAP_ANONYMOUS,-1,0))==MAP_FAILED){
						perror("mmap error\n");
						exit(1);
				}
				for(int i=0; i<PAGESIZE;i++){
						if(m.memarr[i]==0){
								count--;
						}else{
								count=a/MINALLOC;
						}
						if(count==0){
								for(int j=0;j<a/MINALLOC;j++){
										m.memarr[i-a/MINALLOC+1+j]=1;
								}
								m.memoffset[i-a/MINALLOC+1]=a/MINALLOC;
								m.mempage[i-a/MINALLOC+1]=0;
								return m.mem[0]+MINALLOC*(i-a/MINALLOC+1);
						}
				}

	
		}else if(m.count<4){
				if(a%MINALLOC!=0||a>PAGESIZE*4 )
						return NULL;
				int count=a/MINALLOC;
				//이제부터는 memarr전체를 대상으로 채크하고 16배수로 넘어갈때마다 alloc
				
				for(int i=0; i<PAGESIZE/MINALLOC*4;i++){
					if((i%PAGESIZE==0)&&(i/PAGESIZE==m.count)){
							//새로운 페이지로 넘어갈때
						if((m.mem[m.count]=mmap(0,PAGESIZE,flag,MAP_PRIVATE|MAP_ANONYMOUS,-1,0))==MAP_FAILED){
								perror("mmap error\n");
								exit(1);
						}
						m.count++;

					}
					if(m.memarr[i]==0){
							count--;
					}else{
							count=a/MINALLOC;
					}
					if(count==0){
							for(int j=0;j<a/MINALLOC;j++){
									m.memarr[i-a/MINALLOC+1+j]=1;
							}
							m.memoffset[i-a/MINALLOC+1]=a/MINALLOC;
							//return (m.mem[0]+MINALLOC*(i-a/MINALLOC+1));
							//각자의 인덱스에 맞는 주소0~3에서 시작해야함.
							//m.mempage[i-a/MINALLOC+1]=i/(PAGESIZE/MINALLOC);
							//printf("page:%d\n",m.mempage[i-a/MINALLOC+1]);
							return(m.mem[i/(PAGESIZE/MINALLOC)]+MINALLOC*(i%(PAGESIZE/MINALLOC)-a/MINALLOC+1));
					}
				}

			


		}else{
				return NULL;
		}

}

void dealloc(char *a){
		//주소만 보고 어느 페이지에 속하는지 알수 없음...
		if(a==m.mem[0]){
				printf("num1\n");
				for(int i=0;i<PAGESIZE/MINALLOC;i++){
						m.memarr[(PAGESIZE/MINALLOC)*0+i]=0;
				}
		}else if(a==m.mem[1]){
				printf("num2\n");
				for(int i=0;i<PAGESIZE/MINALLOC;i++){
						m.memarr[(PAGESIZE/MINALLOC)*1+i]=0;
				}

		}else if(a==m.mem[2]){
				printf("num3\n");
				for(int i=0;i<PAGESIZE/MINALLOC;i++){
						m.memarr[(PAGESIZE/MINALLOC)*2+i]=0;
				}

		}else{
				printf("num4\n");
				for(int i=0;i<PAGESIZE/MINALLOC;i++){
						m.memarr[(PAGESIZE/MINALLOC)*3+i]=0;
				}

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
		printf("str: %s\n",str);
		printf("addr: %s",&str);


		dealloc(str2);



		printf("memarr:");
		for(int i=0;i<PAGESIZE/MINALLOC*4;i++){
				printf("%d",m.memarr[i]);
		}
		printf("\n");
}
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
