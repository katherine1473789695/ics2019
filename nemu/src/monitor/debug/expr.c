#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>
uint32_t isa_reg_str2val(const char *s,bool *success);

enum {
	TK_NOTYPE = 256, 

	/* TODO: Add more token types */

	TK_PLUS='+', TK_MINUS='-', TK_MULTIPLY='*', TK_DIVIDE='/',
	TK_LPAR='(', TK_RPAR=')',
	TK_REG='r', TK_HEX='h', TK_DNUM='d', 
	TK_EQ='e', TK_NEQ='n', TK_AND='a',
	TK_POINTER, TK_NEGTIVE
};

static struct rule {
  char *regex;
  int token_type;
  int priority;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE,0},    // spaces
  {"\\+", TK_PLUS,3},     // plus
  {"\\-", TK_MINUS,3},     // minus
  {"\\*", TK_MULTIPLY,4}, // multiply
  {"\\/", TK_DIVIDE,4},    // divide
  {"\\(", TK_LPAR,6},     // left bracket
  {"\\)", TK_RPAR,6},  // right bracket
  {"\\$[a-zA-Z]{3}", TK_REG,0}, //register
  {"0[xX][0-9a-fA-F]+", TK_HEX,0}, //hex number
  {"[0-9]+", TK_DNUM,0},  // decimalism number
  {"==", TK_EQ,2},         // equal
  {"!=", TK_NEQ,2},         //not equal
  {"&&", TK_AND,1}          //and
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
  int priority;
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
		
		if(substr_len>32)assert(0);
        switch (rules[i].token_type) {
			case TK_NOTYPE:break;
			case TK_PLUS:
			case TK_MINUS:
			case TK_MULTIPLY:
			case TK_DIVIDE:
			case TK_LPAR:
			case TK_RPAR:
			case TK_EQ:
			case TK_NEQ:
			case TK_AND:
				tokens[nr_token].type = rules[i].token_type;
				tokens[nr_token].priority = rules[i].priority;
				nr_token++;
				break;
			case TK_DNUM:
			case TK_HEX:
				for(int j=0;j<substr_len;j++){
					tokens[nr_token].str[j]= *(e+position+j-substr_len);
					//printf("%d %c\n",j,tokens[nr_token].str[j]);
				}
				tokens[nr_token].str[substr_len]='\0';
				tokens[nr_token].type = rules[i].token_type;
				tokens[nr_token].priority = rules[i].priority;
				nr_token++;
				break;
			case TK_REG:
				tokens[nr_token].type = rules[i].token_type;
				tokens[nr_token].priority = rules[i].priority;
				for(int j=0;j<substr_len-1;j++){
					tokens[nr_token].str[j]=*(e+position+1+j-substr_len);
					if(tokens[nr_token].str[j]<=90)tokens[nr_token].str[j]+=32;
				}
				tokens[nr_token].str[substr_len-1]='\0';
				nr_token++;
				break;
            default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

//  printf("nr_token = %d\n",nr_token);
  return true;
}

bool check_parentheses(int p,int q){
	if(!((tokens[p].type == '(')&&(tokens[q].type == ')')))return false;
	int k=0;
	for(int i=p+1;i<q;i++){
		if(tokens[i].type == '(')k++;
		if(tokens[i].type == ')')k--;
		if(k<0)return false;
	}
	if(k == 0)return true;
	else return false;
}

static int get_op(int l,int r){
	int op = l;
	int priority = 6;
	int par=0;
	for(int i=l;i<=r;i++){
		if(tokens[i].type == '(')par++;
		else if(tokens[i].type == ')')par--;
		//assert(par<0);
		if(par>0)continue;
		if((tokens[i].priority <= priority)&&(tokens[i].priority!=0)){
			priority = tokens[i].priority;
			op = i;
		}
	}
	return op;
}

static uint32_t eval(int p,int q,bool *success){
//	printf("%d,%d\n",p,q);
	if(p>q){
		*success = false;
		printf("wrong!\n");
		return -1;
	}
	else if(p == q){
		int length = strlen(tokens[p].str);
		int sum = 0;
		if(tokens[p].type == 'd'){
			for(int i=0;i<length;i++){
				sum=sum*10+tokens[p].str[i]-48;
			}
		}
		else if(tokens[p].type == 'h'){
			sscanf(tokens[p].str,"%x",&sum);
		}
		else if(tokens[p].type == 'r'){
			sum=isa_reg_str2val(tokens[p].str,success);
		}
		return sum;
	}
	else if(check_parentheses(p,q) == true){
		return eval(p+1,q-1,success);
	}
	else{
		/*int op=-1;int i=q;int num=0;
		for(;i>=p;i--){
			if(tokens[i].type=='+'||tokens[i].type=='-'){
				op=i;
				break;
			}else if(tokens[i].type==')'){
				num=1;i--;
				for(;i>=p;i--){
					if(tokens[i].type == ')')num++;
					if(tokens[i].type == '(')num--;
					if(num<0)assert(0);
					if(num==0)break;
				}
			}
		}
		if(op==-1){
			for(i=q;i>=p;i--){
				if(tokens[i].type == '*'||tokens[i].type == '/'){
					op=i;
					break;
				}else if(tokens[i].type == ')'){
					num=1;i--;
					for(;i>=p;i--){
						if(tokens[i].type == ')')num++;
						if(tokens[i].type == '(')num--;
						if(num<0)assert(0);
						if(num==0)break;
						}
				}
			}
		}*/
		int op = get_op(p,q);
		if(tokens[op].type == TK_POINTER){
			uint32_t addr = eval(op+1,q,success);
			return vaddr_read(addr,4);
		}
		else if(tokens[op].type == TK_NEGTIVE){
			return -eval(op+1,q,success);
		}
		
		int val1 = eval(p,op-1,success);
		int val2 = eval(op+1,q,success);

		switch(tokens[op].type){
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/'://return val1/val2;
				if(val2 == 0){
					*success = false;
					printf("Zero division error!\n");
					return -1;
				}else return val1/val2;
			case 'e':return val1 == val2;
			case 'n':return val1 != val2;
			case 'a':return val1 && val2;
			default:return -1;
		}
	}
}

uint32_t expr(char *e, bool *success) {
	//assert(make_token(e)==true);
	//printf("ok\n");
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  for(int i=0;i<nr_token;i++){
	  int t = tokens[i-1].type;
	  if((tokens[i].type == '*')&&(i==0||(t!='d' && t!='h' && t!='r' && t!=')'))){
		  tokens[i].type = TK_POINTER;
		  tokens[i].priority = 5;
	  }
	  else if((tokens[i].type == '-')&&(i==0||(t!='d' && t!='h' && t!='r' && t!=')'))){
		tokens[i].type = TK_NEGTIVE;
		tokens[i].priority = 5;
	  }	
  }

  *success = true;
  uint32_t result = eval(0, nr_token-1,success);
  return result;
  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
