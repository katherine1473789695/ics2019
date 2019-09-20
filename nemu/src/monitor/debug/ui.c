#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display(void);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static void cmd_err(int err_type,const char *command){
	switch(err_type){
		case 0:
			printf("Invalid arguments for command '%s'\n",command);
			break;
		case 1:
			printf("Lack arguments for command '%s'\n",command);
			break;
		default:
			printf("Unknown error\n");
			break;
	}
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
	char *sub_args = strtok(NULL, " ");
	if(sub_args == NULL)
		cpu_exec(1);
	else{
		int n = atoi(sub_args);
		if(n>0)
			cpu_exec(n);
		else
			cmd_err(0, "si");
	}
	return 0;
}

static int cmd_info(char *args){
	char *sub_args = strtok(NULL, " ");
	if(sub_args == NULL)cmd_err(1, "info");
	else{
		if(*sub_args == 'r')isa_reg_display();
		else if(*sub_args == 'w')
			;
		else cmd_err(0, "info");
		}
	return 0;
}


static int cmd_x(char *args){
	char *args1 = strtok(NULL, " ");
	char *args2 = strtok(NULL, " ");
	if(args1 == NULL)cmd_err(1, "x");
	else if(args2 == NULL)cmd_err(1, "x");
	else{
		int n = atoi(args1);
		if(n<1)cmd_err(0, "x");
		else{
			int addr;
	        sscanf(args2,"%x",&addr);
			for(int i=0;i<n;i++,addr+=4){
				uint32_t data;
				data = vaddr_read(addr,4);
				if((i & 0x3) == 0)printf("0x%08x: ",addr);
				printf("0x%08x\t", data);
				if((i & 0x3) == 0x3)printf("\n");
			}
			printf("\n");
		}
	}
	return 0;
}
static int cmd_p(char *args){
	char *args1 = strtok(NULL, " ");
	if(args1== NULL)cmd_err(1,"p");
	else{
		bool success;
		uint32_t result= expr(args1,&success);
		if(success)printf("0x%x(%d)\n",result,result);
		else printf("Invalid expr!\n");
	}
	return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute single instruction for default and execute n instructions if int n is given", cmd_si},
  { "info", "Show information about registers with argument 'r' and show information about watchpoint with argument 'w'", cmd_info},
  { "x", "Print N 4-Bytes from the start address calculated from EXPR", cmd_x},
  { "p", "Print the value of the expression", cmd_p},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
