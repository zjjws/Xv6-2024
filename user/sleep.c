#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    //sleep 后面只能有一个参数，并且得为数字(负数会被认为格式不正确)
  if(argc!=2 || ck_strint(argv[1])!=1){
    fprintf(2, "usage: sleep <time>\n");
    exit(1);
  }
  int time=atoi(argv[1]);
  sleep(time);
  exit(0);
}
