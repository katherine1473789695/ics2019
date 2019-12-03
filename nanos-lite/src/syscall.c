#include "common.h"
#include "syscall.h"
#include "proc.h"


int sys_yield();
void sys_exit(uintptr_t arg);
size_t sys_write(int fd,const void *buf,size_t count);
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd,void *buf,size_t len);
int fs_close(int fd);
size_t fs_write(int fd,const void *buf,size_t len);
size_t fs_lseek(int fd,size_t offset,int whence);
extern void naive_uload(PCB *pcb, const char *filename);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  uintptr_t result=-1;
  //printf("%x\n",c->GPR1);
  //printf("%d\n",a[0]);
  switch (a[0]) {
    case SYS_exit: naive_uload(NULL,"/bin/init");break;//sys_exit(a[1]);break;
    case SYS_yield: result=sys_yield();break;
    case SYS_open: result=fs_open((char*)a[1],a[2],a[3]);break;
    case SYS_read: result = fs_read(a[1],(void*)a[2],a[3]);break;
    case SYS_write: result = fs_write(a[1],(void*)a[2],a[3]);break;
    case SYS_close: result = fs_close(a[1]);break;
    case SYS_lseek: result = fs_lseek(a[1],a[2],a[3]);break;
    case SYS_brk: result=0;break;
    case SYS_execve: naive_uload(NULL,(void *)a[1]);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  c->GPRx=result;

  return NULL;
}

int sys_yield(){
  _yield();
  return 0;
}

void sys_exit(uintptr_t arg){
  _halt(arg);
}

//size_t sys_write(int fd,const void *buf,size_t count){
  //if(fd==1||fd==2){
    //for(int i=0;i<count;i++){
      //_putc(((char *)buf)[i]);
    //}
  //}
  //return count;
//}
