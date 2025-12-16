#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"


/*
这个是生成随机字符串的代码，得根据这个来判定
char *
randstring(char *buf, int n)
{
  for(int i = 0; i < n-1; i++) {
    buf[i] = "./abcdef"[(rand() >> 7) % 8];
  }
  if(n > 0)
    buf[n-1] = '\0';
  return buf;
}

一开始字符集过于宽泛，并且没有判断结尾\0，所以一直找到 attacktest 这个串
*/

static int
ifchr(char x){
    //常见字符都认为合法
    if('a'<=x&&x<='f')return 1;
    if(x=='.'||x=='/')return 1;
    return 0;
}
static int
checksecret(char *s){
    for(int i=0;i<7;i++){
        if(!ifchr(s[i]))return 0;
    }
    if(s[7]!='\0')return 0;
    return 1;
}
int
main(int argc, char *argv[])
{
  // your code here.  you should write the secret to fd 2 using write
  // (e.g., write(2, secret, 8)
  //和 trace 不一样，这个不是实现系统调用，不应该也不能去修改 kernel里的东西。
  //前8字节是信息
  //多申请一些页，来确保命中率
  int n=32;
  char *base=sbrk(PGSIZE*n);
//   if(base==(char*)-1)exit(1);
  char *start=(char*)PGROUNDUP((uint64)base);
  char *end=base+PGSIZE*n;
  for(char *pg = start; pg + PGSIZE <= end; pg += PGSIZE){
    char *now=pg+32;
    if(checksecret(now)){
        // fprintf(1,"find:");write(1,now,8);//测试找到的secret
        write(2,now,8);
        exit(0);
    }
  }
  //没找到
  exit(1);
}
