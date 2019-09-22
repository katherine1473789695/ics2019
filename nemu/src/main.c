#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>
#include "nemu.h"

int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char *e,bool *success);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  //int is_batch_mode = init_monitor(argc, argv);

  /* Receive commands from user. */
  //ui_mainloop(is_batch_mode);
    FILE * infile;
	char input[100];
	infile = fopen("~/ics2019/nemu/tools/gen-expr/input","r");
	while(fgets(input,100,infile)!=NULL){
		char *result=strtok(input," ");
		char *expression = strtok(NULL, " ");
		bool success=true;
		uint32_t count_result = expr(expression,&success);
		if(!success)printf("calculation failed!\n");
		else printf("%s\t%u\n",result,count_result);
	}
  return 0;
}
