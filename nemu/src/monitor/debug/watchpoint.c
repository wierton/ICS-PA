#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp()
{
	assert(free_ != NULL);

	WP *temp = free_;
	free_ = free_->next;
	temp->next = head;
	head = temp;
	return temp;
}

void free_wp(WP *wp)
{
	assert(wp != NULL && head != NULL);
	WP *p = head,*q = NULL;
	while(p != wp && p!=NULL)
	{
		assert(p != NULL);
		q = p;
		p = p->next;
	}
	if(p == NULL)
	{
		printf("No such watchpoint!\n");
		return;
	}
	if(q == NULL)
	{
		head = head->next;
		wp->next = free_;
		free_ = wp;
	}
	else
	{
		assert(p == wp && q->next == p);
		q->next = p->next;
		wp->next = free_;
		free_ = wp;
	}
}

WP *find_wp(int NO)
{
	if(NO > NR_WP || NO < 1)
		return NULL;
	else
		return &wp_list[NO-1];
}

void print_wp()
{
	WP *temp = head;
	if(head == NULL)
		printf("No watchpoint!\n");
	else
	{
		while(temp != NULL)
		{
			printf("%d:%s %d\n", temp->NO, temp->expr, temp->value);
			temp = temp->next;
		}
	}
}

bool check_wp()
{
	WP *temp = head;
	int value;
	bool is_valid, return_value = true;
	while(temp != NULL)
	{
		value = eval(temp->expr,&is_valid);
		assert(is_valid);
		printf("%d:%s %d -> %d\n",temp->NO, temp->expr, temp->value, value);
		return_value = false;
	}
	return return_value;
}
