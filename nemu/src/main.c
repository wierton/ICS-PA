void init_monitor(int, char *[]);
void reg_test();
void restart();
void ui_mainloop();

#ifdef DEBUG_CACHE_READ
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

#ifdef DEBUG_CACHE_READ
	printf("Total memory access time:%d\n", memory_access_time);
#endif

	return 0;
}
