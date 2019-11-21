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
  Elf_Ehdr elfheader;
  Elf_Phdr programheader;

  ramdisk_read(&elfheader,0,sizeof(Elf_Ehdr));
  uint16_t num = elfheader.e_phnum;
  uint32_t offset= elfheader.e_phoff;
  printf("%x\n",offset);
  printf("%x\n",elfheader.e_phentsize);
  printf("%x\n",num);
  printf("%x\n",get_ramdisk_size());
  while(num--){

    ramdisk_read(&programheader,elfheader.e_phoff+elfheader.e_phentsize,elfheader.e_phentsize);
    if(programheader.p_type==PT_LOAD){
      void* buf= NULL;
      ramdisk_read(&buf,programheader.p_offset,programheader.p_filesz);
    }
  }
  printf("%x",elfheader.e_phentsize);
  printf("%x",elfheader.e_phnum);
  printf("%x",elfheader.e_entry);
  return elfheader.e_entry;
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
