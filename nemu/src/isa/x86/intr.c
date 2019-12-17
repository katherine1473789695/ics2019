#include "rtl/rtl.h"
#include "cpu/exec.h"
#include "isa/mmu.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
   rtl_push(&cpu.eflags_value);
   rtl_push(&cpu.cs);
   rtl_push(&ret_addr);
   cpu.eflags.IF = 0;
   vaddr_t idt_addr = cpu.idtr.base + sizeof(GateDesc)*NO;
   vaddr_t low_addr = vaddr_read(idt_addr,2) & 0xffff;
   vaddr_t high_addr = vaddr_read(idt_addr+4,4) & 0xffff0000;
   decinfo.jmp_pc = high_addr | low_addr;
   decinfo.is_jmp = 1;
   //printf("%x",decinfo.jmp_pc);
   rtl_j(decinfo.jmp_pc);

}

#define IRQ_TIMER 32

bool isa_query_intr(void) {
  if(cpu.INTR & cpu.eflags.IF){
    cpu.INTR = false;
    raise_intr(IRQ_TIMER,cpu.pc);
    return true;
  }

  return false;
}
