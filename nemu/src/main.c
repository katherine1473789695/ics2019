#include<stdio.h>
#include<string.h>
#include "nemu.h"

int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char *e,bool *success);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  /*int is_batch_mode = init_monitor(argc, argv);*/

  /*FILE *fp = fopen("/home/katherine/ics2019/nemu/tools/gen-expr/input","r");
  char str[100];
  int result;
  for(int i=0;i<100;i++){
	  fscanf(fp,"%u",&result);
	  fscanf(fp,"%s",str);
	  bool success = true;
	  printf("%u\n",expr(str,&success));
	  }
	  fclose(fp);
*/
  
    FILE * infile;
	char input[50];
	infile = fopen("/home/katherine/ics2019/nemu/tools/gen-expr/input","r");
	while(fgets( input,50,infile)!=NULL){
		char *result=strtok(input," ");
		char *expression = strtok(NULL, " ");
		expression[strlen(expression)-1]='\0';
		printf("%s\n",expression);
		bool success;
		uint32_t count_result = expr(expression,&success);
		if(!success)printf("calculation failed!\n");
		else printf("%s\t%u\n",result,count_result);
	}
	/* Receive commands from user. */
/*  ui_mainloop(is_batch_mode);*/

  return 0;
}
