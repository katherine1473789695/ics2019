#include "nemu.h"
#include "isa/mmu.h"

static paddr_t page_translate(vaddr_t addr){
  //printf("%x\n",addr);
  CR3 cr3;
  PDE pde;
  PTE pte;
  cr3.val = cpu.cr3;
  //printf("%x\n",cr3.page_directory_base);
  //printf("%x\n",addr);
  pde.val = paddr_read((cr3.page_directory_base<<12) + PDE_INDEX(addr) * sizeof(PDE), sizeof(PDE));
  //printf("%x\n",pde.val);
  //printf("%lx\n",sizeof(PDE));
  assert(pde.present == 1);
  pte.val = paddr_read((pde.page_frame<<12) + PTE_INDEX(addr) * sizeof(PTE), sizeof(PTE));
  assert(pte.present == 1);
  paddr_t paddr = (pte.page_frame<<12)|(addr & PAGE_MASK);
  //printf("the translated paddr is %x\n",paddr);
  return paddr;
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  CR0 cr0;
  cr0.val= cpu.cr0;
  if(cr0.paging){
    if((addr & PAGE_MASK) + len > PAGE_SIZE){
      //assert(0);
      int len1 = PAGE_SIZE-(addr & PAGE_MASK);
      int len2 = len-len1;
      paddr_t paddr1 = page_translate(addr);
      paddr_t paddr2 = page_translate(addr+len1);
      uint32_t data1 = paddr_read(paddr1,len1);
      uint32_t data2 = paddr_read(paddr2,len2);
      return (data2<<(len1<<3))|data1;
    }else{
      paddr_t paddr = page_translate(addr);
      //printf("%x\n",paddr);
      return paddr_read(paddr, len);
    }
  }else{
    return paddr_read(addr,len);
  }
  
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  CR0 cr0;
  cr0.val = cpu.cr0;
  if(cr0.paging){
      if((addr & PAGE_MASK) + len > PAGE_SIZE){
        //assert(0);
        int len1 = PAGE_SIZE - (addr & PAGE_MASK);
        int len2 = len - len1;
        paddr_t paddr1 = page_translate(addr);
        paddr_t paddr2 = page_translate(addr+len1);
        paddr_write(paddr1,data,len1);
        paddr_write(paddr2,(data>>(len1<<3)),len2);
    }else{
      paddr_t paddr = page_translate(addr);
      paddr_write(paddr, data, len);
    }
  }else{
    paddr_write(addr,data,len);
  }
  
}

