#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool flag=true;
  if(cpu.eax!=ref_r->eax){
    printf("pc:0x%x cpu.eax:0x%x ref_r.eax:0x%x\n",cpu.pc,cpu.eax,ref_r->eax);
    flag=false;
  }
  if(cpu.ecx!=ref_r->ecx){
    printf("pc:0x%x cpu.ecx:0x%x ref_r.ecx:0x%x\n",cpu.pc,cpu.ecx,ref_r->ecx);
    flag=false;
  }
  if(cpu.edx!=ref_r->edx){
    printf("pc:0x%x cpu.edx:0x%x ref_r.edx:0x%x\n",cpu.pc,cpu.edx,ref_r->edx);
    flag=false;
  }
  if(cpu.ebx!=ref_r->ebx){
    printf("pc:0x%x cpu.ebx:0x%x ref_r.ebx:0x%x\n",cpu.pc,cpu.ebx,ref_r->ebx);
    flag=false;
  }
  if(cpu.esp!=ref_r->esp){
    printf("pc:0x%x cpu.esp:0x%x ref_r.esp:0x%x\n",cpu.pc,cpu.esp,ref_r->esp);
    flag=false;
  }
  if(cpu.ebp!=ref_r->ebp){
    printf("pc:0x%x cpu.ebp:0x%x ref_r.ebp:0x%x\n",cpu.pc,cpu.ebp,ref_r->ebp);
    flag=false;
  }
  if(cpu.esi!=ref_r->esi){
    printf("pc:0x%x cpu.esi:0x%x ref_r.esi:0x%x\n",cpu.pc,cpu.esi,ref_r->esi);
    flag=false;
  }
  if(cpu.edi!=ref_r->edi){
    printf("pc:0x%x cpu.edi:0x%x ref_r.edi:0x%x\n",cpu.pc,cpu.edi,ref_r->edi);
    flag=false;
  }
  if(cpu.pc!=ref_r->pc){
    printf("cpu.pc:0x%x ref_r.pc:0x%x\n",cpu.pc,ref_r->pc);
    flag=false;
  }
  return flag;
}

void isa_difftest_attach(void) {
}
