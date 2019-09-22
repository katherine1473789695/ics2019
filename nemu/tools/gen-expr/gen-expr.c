#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
uint32_t choose(uint32_t n){
	return rand()%n;
}

static char buf[65000];
static inline void gen_rand_expr(int l,int r) {
  //buf[0] = '\0';
  if(l==r)buf[l]=choose(10)+48;
  else if(r==l+1){
	  /*switch(choose(2)){
		  case 0:
	          buf[l]=choose(9)+1+48;break;
		  default:
			  buf[l]=' ';break;
	  }*/
	  buf[l]=choose(9)+1+48;
	  buf[r]=choose(10)+48;
  }
  else{
	  int op=l+1+choose(r-l-1);
	  switch(choose(5)){
		  case 0:
			  gen_rand_expr(l,op-1);
			  buf[op]='+';
			  gen_rand_expr(op+1,r);
			  break;
		  case 1:
			  gen_rand_expr(l,op-1);
			  buf[op]='-';
			  gen_rand_expr(op+1,r);
			  break;
		  case 2:
              gen_rand_expr(l,op-1);
			  buf[op]='*';
			  gen_rand_expr(op+1,r);
			  break;
	      case 3:
	          gen_rand_expr(l,op-1);
			  buf[op]='/';
			  gen_rand_expr(op+1,r);
			  break;
		  /*case 4:
		      buf[l]=' ';
		      gen_rand_expr(l+1,r);
		      break;
		  case 5:
		      buf[r]=' ';
		      gen_rand_expr(l,r-1);
		      break;*/	  
		  default:
			  buf[l]='(';
			  buf[r]=')';
			  gen_rand_expr(l+1,r-1);
			  break;
	  }
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr(0,10);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
