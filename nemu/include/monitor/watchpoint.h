#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expression[128];
  uint32_t value;
  int hit_time;
  

  /* TODO: Add more members if necessary */


} WP;

#endif
