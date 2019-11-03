#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  //TODO();
  rtl_and(&s0,&id_dest->val,&id_src->val);
  //printf("%u %x %u %d",s0,id_dest->val,id_src->val,id_dest->width);
  rtl_update_ZFSF(&s0,id_dest->width);
  //printf("%u ",cpu.eflags.ZF);
  s0=0;
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);

  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtl_and(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  //donot realize the change of eflags
  rtl_update_ZFSF(&s0,id_dest->width);
  s1=0;
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);
  
  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
  rtl_xor(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->width);
  s1=0;
  rtl_set_OF(&s1);
  rtl_set_CF(&s1);

  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->width);
  s1=0;
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);

  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_sar(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU

  rtl_shl(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  //TODO();
  rtl_not(&s0,&id_dest->val);
  operand_write(id_dest,&s0);

  print_asm_template1(not);
}

make_EHelper(rol){
  s0=id_src->val;
  while(s0!=0){
    s1=(0x1<<(id_dest->width*8-1));
    s1= ((s1&id_dest->val)>>(8*id_dest->width-1))&0x1;
    rtl_shli(&id_dest->val,&id_dest->val,1);
    id_dest->val=id_dest->val|s1;
    s0--;
  }
  rtl_set_CF(&s1);
  if(id_src->val==1){
    s1=s1^((id_dest->val>>(id_dest->width*8-1))&0x1);
  }
  rtl_set_OF(&s1);
  operand_write(id_dest,&id_dest->val);

  print_asm_template2(rol);
}