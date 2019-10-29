#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  //TODO();
  cpu.esp = cpu.esp-4;
  vaddr_write(cpu.esp,*src1,4);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  //TODO();
  *dest = vaddr_read(cpu.esp,4);
  cpu.esp+=4;
}

static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  //TODO();
  //overflow when x and y have the opposite sign and res different from x
  t0 =  ((0x1<<(8*width-1))& (*src1))>>(8*width-1);  //x
  t1 =  ((0x1<<(8*width-1))& (*src2))>>(8*width-1);  //y
  //printf("%u %u   ",t0,t1);
  rtl_xor(&t1,&t0,&t1);
  if(t1==0) *dest=0;
  else{
    t0=t0&0x1; //the sign of x
    t1=((0x1<<(8*width-1))& (*res))>>(8*width-1);
    t1=t1&0x1; //the sign of result
    //printf("%u %u\n",t1,t0);
    rtl_xor(&t0,&t0,&t1);
    *dest = t0;
  }
}

static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  //TODO();
  *dest = (*res > *src1);
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  //TODO();
  t0 =  ((0x1<<(8*width-1))& (*src1))>>(8*width-1);
  t1 =  ((0x1<<(8*width-1))& (*src2))>>(8*width-1);
  rtl_xor(&t0,&t0,&t1);
  if(t0==1) *dest=0;
  else{
    t1=t1&0x1; //the sign of src2
    t0=((0x1<<(8*width-1))& (*res))>>(8*width-1);
    t0=t0&0x1; //the sign of result
    rtl_xor(&t0,&t0,&t1);
    *dest = t0;
  }
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  //TODO();
  *dest = !(*res >= *src1);
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    cpu.eflags.f=((*src) & 0x1); \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest=cpu.eflags.f; \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  //TODO();
  assert(width==1||width==2||width==4);
  t0 = (~(0xffffffff << (8*width-1)<<1));
  //printf("%u ",t0);
  //printf("%u",*result);
  t1 = ((*result & t0) == 0);
  rtl_set_ZF(&t1);
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  //TODO();
  assert(width==1||width==2||width==4);
  t0 = (*result) >> (8*width-1);
  t1 = t0&0x1;
  rtl_set_SF(&t1);
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
