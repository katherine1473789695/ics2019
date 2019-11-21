#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
//static void* buf;

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  uintptr_t program_entry;
  uint32_t seg_off;
  uint16_t shentsize;
  uint16_t shnum;
  ramdisk_read(&program_entry,24,4);
  ramdisk_read(&seg_off,32,4);
  ramdisk_read(&shentsize,46,2);
  ramdisk_read(&shnum,48,2);
  printf("%x\n",program_entry);
  printf("%x\n",seg_off);
  printf("%x\n",shentsize);
  printf("%x\n",shnum);
  return program_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
