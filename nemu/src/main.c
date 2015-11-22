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


int main(int argc, char *argv[]) {

	/* Initialize the monitor. */
	init_monitor(argc, argv);

	/* Test the implementation of the ``CPU_state'' structure. */
	reg_test();

	/* Initialize the virtual computer system. */
	restart();

	/* Receive commands from user. */
	ui_mainloop();

#ifdef DEBUG_CACHE_TIME_CALC
	printf("Total memory access time:%lld\n", memory_access_time);
#endif

	return 0;
}
