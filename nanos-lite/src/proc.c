#include "proc.h"

#define MAX_NR_PROC 4
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
void naive_uload(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}
static int fg_pcb = 1;
void set_pcb_id(int id){
    fg_pcb = id;
    Log("chosen=%d", id);
}

void init_proc() {
  //context_kload(&pcb[0],(void*)hello_fun);
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[3], "/bin/pal");

  switch_boot_pcb();
  //Log("the end of the proc");

  //Log("Initializing processes...");

  // load program here
  //naive_uload(NULL, "/bin/init");

}

_Context* schedule(_Context *prev) {
  //Log("enter schedule");
  current->cp = prev;
  //current = &pcb[1];
  //Log("current %x",current->cp);
  current = (current == &pcb[0] ? &pcb[fg_pcb] : &pcb[0]);
  return current->cp;
}
