#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  uintptr_t result=-1;
  //printf("%x\n",c->eax);

  switch (a[0]) {
    case SYS_yield: _yield();result=0;break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  c->GPRx=result;

  return NULL;
}
