#include "common.h"
_Context* do_syscall(_Context *c);
_Context* schedule(_Context *prev);

static _Context* do_event(_Event e, _Context* c) {
  //printf("%x\n",e.event);
  Log("switch event");
  switch (e.event) {
    case _EVENT_YIELD: return schedule(c);printf("after return\n");break;
    case _EVENT_SYSCALL: return do_syscall(c);break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
