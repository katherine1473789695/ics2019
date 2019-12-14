#include "proc.h"
#include <elf.h>
#include "fs.h"
#define DEFAULT_ENTRY ((void *)0x4000000)


#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd,void *buf,size_t len);
int fs_close(int fd);
size_t fs_offset(int fd);
size_t fs_lseek(int fd,size_t offset,int whence);
size_t fs_openoffset(int fd);
size_t fs_filesz(int fd);
//extern Finfo file_table[];

static uintptr_t loader(PCB *pcb, const char *filename) {
  //printf("enter loader\n");
  Elf_Ehdr elfheader;
  Elf_Phdr programheader;
  int fd = fs_open(filename,0,0);
  if(fd!=-1){
    //printf("before read\n");
    fs_read(fd,&elfheader,sizeof(Elf_Ehdr));
    //printf("%x\n",elfheader.e_entry);
    fs_lseek(fd,elfheader.e_phoff,SEEK_SET);
    for(uint16_t i=0;i<elfheader.e_phnum;i++){
      fs_read(fd,&programheader,sizeof(Elf_Phdr));
      size_t opset=fs_openoffset(fd);
      //ramdisk_read(&programheader,elfheader.e_phoff+i*elfheader.e_phentsize,sizeof(Elf_Phdr));
      if(programheader.p_type == PT_LOAD){
        fs_lseek(fd,programheader.p_offset,SEEK_SET);
        void *vaddr, *paddr;
        vaddr = (void*)programheader.p_vaddr;
        //printf("%x\n",vaddr);
        for(size_t i=0,sz = programheader.p_filesz;i<sz;i+=PGSIZE){
          size_t read_bytes = ((sz-i)>=PGSIZE) ? PGSIZE : (sz-i);
          //printf("%x\n",read_bytes);
          paddr = new_page(1);
          //printf("%x\n",paddr);
          _map(&pcb->as,vaddr,paddr,0);
          fs_read(fd,paddr,read_bytes);
          //memset((void*)paddr+programheader.p_filesz,0,(programheader.p_memsz-programheader.p_filesz));
        }
        //void *pa = new_page(1);
        //_map(&pcb->as,(void *)programheader.p_vaddr,pa,0);
        //fs_read(fd,pa,programheader.p_filesz);
        //uint8_t buf[programheader.p_filesz];
        //ramdisk_read(&buf,programheader.p_offset+fs_offset(fd),programheader.p_filesz);
        //memcpy((void*)programheader.p_vaddr,&buf,programheader.p_filesz);
        
      }
      fs_lseek(fd,opset,SEEK_SET);
    }
  }
  fs_close(fd);
  //TODO();
  //printf("going out loader\n");
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
  //Log("before loader");
  _protect(&pcb->as);
  uintptr_t entry = loader(pcb, filename);
  //Log("%x",entry);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
  //Log("the end of uload\n");
}
