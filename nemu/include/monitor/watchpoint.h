#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define WP_EXPR 64

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	char expr[WP_EXPR];
	int value;


} WP;

WP *new_wp();

void free_wp(WP *wp);

WP *find_wp(int NO);

void print_wp();

bool check_wp();

#endif
