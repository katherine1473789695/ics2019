#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
	printf("eax\t0x%-8x\t%u\n",cpu.eax,cpu.eax);	
	printf("ecx\t0x%-8x\t%u\n",cpu.ecx,cpu.ecx);
	printf("edx\t0x%-8x\t%u\n",cpu.edx,cpu.edx);
	printf("ebx\t0x%-8x\t%u\n",cpu.ebx,cpu.ebx);
	printf("esp\t0x%-8x\t%u\n",cpu.esp,cpu.esp);
	printf("ebp\t0x%-8x\t%u\n",cpu.ebp,cpu.ebp);
	printf("esi\t0x%-8x\t%u\n",cpu.esi,cpu.esi);
	printf("edi\t0x%-8x\t%u\n",cpu.edi,cpu.edi);
	printf("pc\t0x%-8x\t%u\n",cpu.pc,cpu.pc);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
	int name_len = strlen(s);
	int width = 4;
	if(name_len == 3)width=4;
	else if(name_len ==2){
		if(s[1]=='h'||s[1]=='H'||s[1]=='l'||s[1]=='L')width=1;
		else width=2;
	}
	for(int i=R_EAX;i<=R_EDI;i++){
		if(strcmp(s,reg_name(i,width))==0){
			switch(width){
				case 4:return reg_l(i);
				case 2:return reg_w(i);
				case 1:return reg_b(i);
			}
		}
	}

	
    return 0;
}
