#include "proc.h"
#include <elf.h>
#include "fs.h"


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
//extern Finfo file_table[];

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elfheader;
  Elf_Phdr programheader;
  int fd = fs_open(filename,0,0);
  if(fd!=-1){
    fs_read(fd,&elfheader,sizeof(Elf_Ehdr));
    //printf("%x %x \n",sizeof(Elf_Ehdr),elfheader.e_phoff);
    //fs_read(fd,&programheader,elfheader.e_phoff-sizeof(Elf_Ehdr));
    fs_lseek(fd,elfheader.e_phoff,SEEK_SET);
    for(uint16_t i=0;i<elfheader.e_phnum;i++){
      fs_read(fd,&programheader,sizeof(Elf_Phdr));
      size_t opset=fs_openoffset(fd);
      //ramdisk_read(&programheader,elfheader.e_phoff+i*elfheader.e_phentsize,sizeof(Elf_Phdr));
      if(programheader.p_type == PT_LOAD){
        fs_lseek(fd,programheader.p_offset,SEEK_SET);
        fs_read(fd,(void *)programheader.p_vaddr,programheader.p_filesz);
        //uint8_t buf[programheader.p_filesz];
        //ramdisk_read(&buf,programheader.p_offset+fs_offset(fd),programheader.p_filesz);
        //memcpy((void*)programheader.p_vaddr,&buf,programheader.p_filesz);
        memset((void*)(programheader.p_vaddr+programheader.p_filesz),0,(programheader.p_memsz-programheader.p_filesz));
      }
      fs_lseek(fd,opset,SEEK_SET);
    }
  }
  fs_close(fd);
  //TODO();
  //Elf_Ehdr elfheader;
  //Elf_Phdr programheader;

  //ramdisk_read(&elfheader,0,sizeof(Elf_Ehdr));
  //for(uint16_t i=0;i<elfheader.e_phnum;i++){
    //ramdisk_read(&programheader,elfheader.e_phoff+i*elfheader.e_phentsize,sizeof(Elf_Phdr));
    //if(programheader.p_type == PT_LOAD){
      //uint8_t buf[programheader.p_filesz];
      //ramdisk_read(&buf,programheader.p_offset,programheader.p_filesz);
      //vaddr_write(programheader.p_vaddr,&buf,programheader.p_filesz);
      //memcpy((void*)programheader.p_vaddr,&buf,programheader.p_filesz);
      //memset((void*)(programheader.p_vaddr+programheader.p_filesz),0,(programheader.p_memsz-programheader.p_filesz));
    //}
  //}
  //ramdisk_read(&programheader,elfheader.e_phoff,sizeof(Elf_Phdr));
  
  //uint16_t num = elfheader.e_phnum;
  //uint32_t offset= elfheader.e_phoff;
  //uint16_t size=elfheader.e_phentsize;
  //uintptr_t  addr= elfheader.e_entry;
  //printf("%x\n",offset);
  //printf("%x\n",size);
  //printf("%x\n",num);
  //printf("%x\n",get_ramdisk_size());
  //while(num--){
    //ramdisk_read(&programheader,offset,size);
    //if(programheader.p_type==PT_LOAD){
      //uint32_t data;
      //ramdisk_read(&data,programheader.p_offset,programheader.p_filesz);
      //offset+=size;
    //}
  //}
  
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
