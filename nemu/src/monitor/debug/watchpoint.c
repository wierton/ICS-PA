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
	if(free_ == NULL)
		assert(0);

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
