#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/*  TODO: Implement the functionality of watchpoint */

WP* new_wp(char *s){
	if(s == NULL){
		printf("Lacking expression!\n");
		return NULL;
	}
	if(head == NULL){
		head=free_;
		free_ = free_->next;
		head->next = NULL;
		strcpy(head->expression,s);
		bool success;
		uint32_t result = expr(s,&success);
		if(!success){
			printf("Invalid Expression!\n");
			return NULL;
		}
		head->value = result;
		head->hit_time = 0;
		return head;
	}
	else{
		if(free_ == NULL){
			printf("NO Free watchpointer!\n");
			return NULL;
		}
		else{
			WP *p = free_;
			free_ = free_->next;
			p->next = head;
			head = p;
			strcpy(head->expression,s);
			bool success;
			uint32_t result = expr(s,&success);
			if(!success){
				printf("Invalid Expression!\n");
				return NULL;
			}
			head->value = result;
			head->hit_time = 0;
			return head;
		}
	}
}

void free_wp(WP *wp){
	if(wp == NULL){
		printf("Cannot free NULL pointer!\n");
		return;
	}
	else if(wp == head){
		head = wp->next;
		wp->next = free_;
		free_ = wp;
		return;
	}
	else{
		for(WP *p=head;p!=NULL;p=p->next){
			if(p->next == wp){
				p->next = wp->next;
				wp->next = free_;
				free_ = wp;
				return;
			}
		}
	}
}

void wp_delete(int num){
	int state = 0;
	for(WP *p=head;p!=NULL;p=p->next){
		if(p->NO == num){
			free_wp(p);
			printf("watchpoint %d deleted\n",num);
			state = 1;
			break;
		}
	}
	if(state == 0)printf("no such watchpoint!\n");
	return;
}
void wp_display(){
	if(head==NULL){
		printf("No watchpoints!\n");
		return;
	}
	printf("NUM\tVALUE\tWHAT\n");
	for(WP *p=head;p!=NULL;p=p->next){
		printf("%d\t%u\t%s\n",p->NO,p->value,p->expression);
		printf("\tBreakpoint already hit %d time(s)\n",p->hit_time); 
	}
	return;
}

bool check_watchpoint(){
	WP *a = head;
	bool flag = true,success;
	uint32_t result;
	if(a!=NULL){
		for(;a!=NULL;a=a->next){
			result = expr(a->expression,&success);
			if(result!=a->value){
				a->hit_time++;
				printf("Hardware watchpoint %d: %s\n",a->NO,a->expression);
				printf("Old value = %u\n",a->value);
				printf("New value = %u\n",result);
				a->value = result;
				flag = false;
			}
		}
	}
	return flag;
}

