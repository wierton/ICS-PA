#include "common.h"
#include <stdlib.h>
#include <time.h>

void init_monitor(int, char *[]);
void reg_test();
void restart();
void ui_mainloop();

/* define if necessary */
/* #define DEBUG_CACHE_TIME_CALC */

#ifdef DEBUG_CACHE_TIME_CALC
#include <stdio.h>
extern long long memory_access_time;
#endif

extern uint64_t local_eips, total_eips;

int main(int argc, char *argv[]) {
	/* generate seed */
	srand(time(NULL));

	/* Initialize the monitor. */
	init_monitor(argc, argv);

	/* Test the implementation of the ``CPU_state'' structure. */
	reg_test();

	/* Initialize the virtual computer system. */
	restart();

	/* init device */

	/* Receive commands from user. */
	ui_mainloop();

#ifdef DEBUG_CACHE_TIME_CALC
	printf("Total memory access time:%lld\n", memory_access_time);
#endif

	printf("local(%llu) in total (%llu)\n", local_eips, total_eips);

	return 0;
}
