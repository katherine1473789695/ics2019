#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  if(id_dest->width==1)rtl_sext(&s0,&id_dest->val,id_dest->width);
  rtl_push(&s0);
  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
  rtl_pop(&s0);
  operand_write(id_dest,&s0);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    //TODO();
    rtl_lr(&s0,0,2);
    s0>>=15;
    if(s0==1){
      s1=0xffff;
      rtl_sr(2,&s1,2);
    }else{
      s1=0x0000;
      rtl_sr(2,&s1,2);
    }
  }
  else {
    //TODO();
    rtl_lr(&s0,0,4);
    s0>>=31;
    if(s0==1){
      s1=0xffffffff;
      rtl_sr(2,&s1,4);
    }else{
      s1=0x00000000;
      rtl_sr(2,&s1,4);
    }
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
