#include "kernel/types.h"
#include "user/user.h"

const int N = 280;

int p1[2];
int p2[2];

__attribute__((noreturn)) void findprime() {
  int prime;
  close(p1[1]);
  //第一个数一定是质数，如果没有数了那么就结束
  if(read(p1[0],&prime,sizeof(prime)))printf("prime %d\n",prime);
  else exit(0);

  int x;
  pipe(p2);
  int pid=fork();
  if(pid>0){
    close(p2[0]);
    while(read(p1[0],&x,sizeof(x))){
        if(x%prime>0){
            write(p2[1],&x,sizeof(x));
        }
    }
    close(p1[0]);
    close(p2[1]);
    wait(0);
    exit(0);
  }
  else{
    close(p1[0]);
    memcpy(p1,p2,sizeof(p1));
    findprime();
    exit(0);
  }
}

int main(int argc, char *argv[]) {
  pipe(p1);
  int pid=fork();
  if(pid>0){
    //初始进程，写入2~n
    close(p1[0]);
    for(int i=2;i<=N;i++)write(p1[1],&i,sizeof(i));
    close(p1[1]);
    wait(0);
    exit(0);
  }
  else {
    //往后传
    findprime();
    close(p1[0]);
    exit(0);
  }
}
