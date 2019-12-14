#include <am.h>
#include <x86.h>

static _Context* (*user_handler)(_Event, _Context*) = NULL;

void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();
void __am_get_cur_as(_Context *c);
void __am_switch(_Context *c);

_Context* __am_irq_handle(_Context *c) {
  __am_get_cur_as(c);
  _Context *next = c;
  //_putc(c->edi);
  //_putc(c->eax);
  if (user_handler) {
    _Event ev = {0};
    switch (c->irq) {
      case 0x80:ev.event = _EVENT_SYSCALL;break;
      case 0x81:ev.event = _EVENT_YIELD;break;
      default: ev.event = _EVENT_ERROR; break;
    }
    _putc('t');
    next = user_handler(ev, c);
    if (next == NULL) {
      next = c;
    }
  }
  _putc('b');
  __am_switch(next);
  _putc('n');
  return next;
}

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  static GateDesc idt[NR_IRQ];  //256

  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
  }
  //STS_TG32=0xF
  //KSEL(desc)     (((desc) << 3) | DPL_KERN)
  //SEG_KCODE      1 

  // ----------------------- interrupts ----------------------------
  idt[32]   = GATE(STS_IG32, KSEL(SEG_KCODE), __am_irq0,   DPL_KERN);
  // ---------------------- system call ----------------------------
  idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vecsys, DPL_USER);
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  _Context *c = (_Context *)stack.end-1;
  //memset(c,0,sizeof(_Context));
  c->eip = (uintptr_t)entry;
  c->cs=0x8;

  return c;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
