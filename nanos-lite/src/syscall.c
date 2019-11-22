#include "common.h"
#include "syscall.h"


int sys_yield();
void sys_exit(uintptr_t arg);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  uintptr_t result=-1;
  //printf("%x\n",c->GPR1);
  printf("%d\n",a[0]);
  switch (a[0]) {
    case SYS_exit:sys_exit(a[1]);break;
    case SYS_yield: result=sys_yield();break;
    case SYS_write:
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
