#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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

  if(argint(0, &pid) < 0)
    return -1;
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
sys_trace(void) {
  //获取trace系统调用的参数
  argint(0, &(myproc() ->trace_mask));
  return 0;
}

uint64
sys_sysinfo(void) {
  //两个函数一个获取进程的数量，一个获取空闲的内存数量，并将他们存放到结构体中
  //获取完之后，将结构体传回用户空间
  struct sysinfo now_info;
  struct proc *now = myproc();
  now_info.freemem = 0;//空闲内存初始化为0
  now_info.nproc = 0; //进程初始化为0
  getprocess(&now_info.nproc);//获取进程数量
  getfreemem(&now_info.freemem);//获取空闲内存数量

  //获取虚拟内存地址，不清楚第一个参数是干什么的？
  uint64 memaddr;
  if (argaddr(0, &memaddr) < 0) {
    return -1;
  }
  if (copyout(now->pagetable, memaddr, (char *)&now_info, sizeof(now_info)) < 0) {
    return -1;
  }
  return 0;
}