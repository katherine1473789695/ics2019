#include "memory.h"
#include "proc.h"

static void *pf = NULL;
int _map(_AddressSpace *as, void *va, void *pa, int prot);
//extern PCB *current;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  void *va = NULL,*pa = NULL;
  void *begin =(void *) PGROUNDUP(current->max_brk);
  void* end = (void*)PGROUNDUP(brk);
  if(current->max_brk<=brk){
    for(va=begin;va<=end;va+=PGSIZE){
      pa = new_page(1);
      _map(&current->as,va,pa,0);
    }
    current->max_brk = (uint32_t )end;
  }

  return 0;
}

void init_mm() {
  //Log("here");
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  //panic("hi");
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
