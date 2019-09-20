#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>

enum {
	TK_NOTYPE = 256, 

	/* TODO: Add more token types */

	TK_PLUS='+', TK_MINUS='-', TK_MULTIPLY='*', TK_DIVIDE='/',
	TK_LPAR='(', TK_RPAR=')',
	TK_DNUM='d', TK_EQ='e'
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_PLUS},     // plus
  {"\\-", TK_MINUS},     // minus
  {"\\*", TK_MULTIPLY}, // multiply
  {"\\/", TK_DIVIDE},    // divide
  {"\\(", TK_LPAR},     // left bracket
  {"\\)", TK_RPAR},     // right bracket
  {"[0-9]+", TK_DNUM},  // decimalism number
  {"==", TK_EQ}         // equal
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
} Token;

static Token tokens[32] __attribute__((used)) = {};
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
			case TK_PLUS:
				tokens[nr_token].type=rules[i].token_type;nr_token++;
				break;
			case TK_MINUS:
				tokens[nr_token].type=rules[i].token_type;nr_token++;
				break;
			case TK_MULTIPLY:
				tokens[nr_token].type=rules[i].token_type;nr_token++;
				break;
			case TK_DIVIDE:
				tokens[nr_token].type=rules[i].token_type;nr_token++;
				break;
			case TK_LPAR:
				tokens[nr_token].type=rules[i].token_type;nr_token++;
				break;
			case TK_RPAR:
				tokens[nr_token].type=rules[i].token_type;nr_token++;
				break;
			case TK_EQ:
				tokens[nr_token].type=rules[i].token_type;nr_token++;
				break;
			case TK_DNUM:
				for(int j=0;j<substr_len;j++){
					tokens[nr_token].str[j]= *(e+position+j-substr_len);
					printf("%d %c\n",j,tokens[nr_token].str[j]);
				}
				tokens[nr_token].type=rules[i].token_type;nr_token++;
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

int eval(int p,int q){
	if(p>q)assert(0);
	else if(p == q){
		int length = strlen(tokens[p].str);
		int sum = 0;
		if(tokens[p].type == 'd'){
			for(int i=0;i<length;i++){
				sum=sum*10+tokens[p].str[i]-48;
			}
		}
		return sum;
	}
	else if(check_parentheses(p,q) == true){
		return eval(p+1,q-1);
	}
	else{
		int op=-1;int i=q;int num=0;
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
		}
		
		int val1 = eval(p,op-1);
		int val2 = eval(op+1,q);

		switch(tokens[op].type){
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/':return val1/val2;
			default:assert(0);
		}
	}
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  uint32_t result = eval(0, nr_token);
  return result;
  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
