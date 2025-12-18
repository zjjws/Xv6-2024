#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  backtrace();
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


uint64
sys_sigalarm(void)
{
  int ticks;//用户传入 tick
  uint64 handler;//用户传入的处理函数地址
  struct proc *p = myproc();

  argint(0, &ticks);
  argaddr(1, &handler);

//   if(ticks == 0 && handler == 0){
  if(ticks==0||handler==0){//只要任何一个=0就应该认为是不需要 alarm 的。
    p->alarmticks = 0;
    p->alarmhandler = 0;
    p->alarm_elapsed = 0;
    p->alarm_inflight = 0;
    return 0;
  }

  p->alarmticks = ticks;
  p->alarmhandler = handler;
  p->alarm_elapsed = 0;//重计时
  p->alarm_inflight = 0;//标记要清零
  return 0;
}


uint64
sys_sigreturn(void)
{
//报警处理函数运行在用户态，其执行结束后必须显式调用 sigreturn。内核在处理该系统调用时，会恢复此前保存的陷阱帧，从而完整恢复被中断时的用户态寄存器状态，并允许后续报警再次触发。
//通过这种方式，进程在感知报警的同时，其正常执行流程不受破坏。
  struct proc *p = myproc();
  memmove(p->trapframe, &p->alarm_tf_backup, sizeof(struct trapframe));
  p->alarm_inflight = 0;
  return p->trapframe->a0;
}
