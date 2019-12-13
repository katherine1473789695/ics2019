#include "nemu.h"
#include "isa/mmu.h"

static paddr_t page_translate(vaddr_t addr){
  CR3 cr3;
  PDE pde;
  PTE pte;
  cr3.val = cpu.cr3;
  printf("%x\n",cpu.cr3);
  pde.val = paddr_read(cr3.page_directory_base + PDE_INDEX(addr) * sizeof(PDE), sizeof(PDE));
  //printf("%x\n",pde.val);
  assert(pde.present == 1);
  pte.val = paddr_read(pde.page_frame + PTE_INDEX(addr) * sizeof(PTE), sizeof(PTE));
  assert(pte.present == 1);
  paddr_t paddr = (pte.page_frame<<12)|(addr & PAGE_MASK);
  return paddr;
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  CR0 cr0;
  cr0.val= cpu.cr0;
  if(cr0.paging){
    if((addr & PAGE_MASK) + len > PAGE_SIZE){
      assert(0);
    }else{
      paddr_t paddr = page_translate(addr);
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
        assert(0);
    }else{
      paddr_t paddr = page_translate(addr);
      paddr_write(paddr, data, len);
    }
  }else{
    paddr_write(addr,data,len);
  }
  
}

