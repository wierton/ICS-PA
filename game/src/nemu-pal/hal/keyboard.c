#include "hal.h"

#define NR_KEYS 18

enum {KEY_STATE_EMPTY, KEY_STATE_WAIT_RELEASE, KEY_STATE_RELEASE, KEY_STATE_PRESS};

/* Only the following keys are used in NEMU-PAL. */
static const int keycode_array[] = {
	K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE,
	K_RETURN, K_SPACE, K_PAGEUP, K_PAGEDOWN, K_r,
	K_a, K_d, K_e, K_w, K_q,
	K_s, K_f, K_p
};

/* K_UP,     K_DOWN,   K_LEFT,   K_RIGHT,  K_ESCAPE */
/* 0x48/0xc8 0xd0/0x50 0x4b/0xcb 0x4d/0xcd 0x1/0x81 */
/* K_RETURN, K_SPACE,  K_PAGEUP, K_PAGEDOWN, K_r */
/* 0x1c/0x9c 0x39/0xb9 0x49/0xc9 0x51/0xd1   0x13/0x93*/
/* K_a,      K_d,      K_e,      K_w,      K_q */
/* 0x1e/0x9e 0x20/0xa0 0x12/0x92 0x11/0x91 0x90/0x10*/
/* K_s,      K_f,      K_p */
/* 0x1f/0x9f 0x21/0xa1 0x19/0x99 */
static int key_state[NR_KEYS];

void
keyboard_event(void) {
	/* TODO: Fetch the scancode and update the key states. */

	int i,target_key;
	uint32_t scancode = in_byte(0x60);
	uint32_t updown = ((scancode >> 0x7) & 0x1);

	Log("scancode:0x%x\n", scancode&0x7f);

	for(i=0;i<NR_KEYS;i++)
	{
		if(key_state[i] == KEY_STATE_PRESS)
			key_state[i] = KEY_STATE_WAIT_RELEASE;
		if(key_state[i] == KEY_STATE_RELEASE)
			key_state[i] = KEY_STATE_EMPTY;
	}
	switch(scancode & 0x7f)
	{
		case 0x48:target_key = K_UP;break;
		case 0xd0:target_key = K_DOWN;break;
		case 0x4b:target_key = K_LEFT;break;
		case 0x4d:target_key = K_RIGHT;break;
		case 0x01:target_key = K_ESCAPE;break;
		case 0x1c:target_key = K_RETURN;break;
		case 0x39:target_key = K_SPACE;break;
		case 0x49:target_key = K_PAGEUP;break;
		case 0x51:target_key = K_PAGEDOWN;break;
		case 0x13:target_key = K_r;break;
		case 0x1e:target_key = K_a;break;
		case 0x20:target_key = K_d;break;
		case 0x12:target_key = K_e;break;
		case 0x11:target_key = K_w;break;
		case 0x10:target_key = K_q;break;
		case 0x1f:target_key = K_s;break;
		case 0x21:target_key = K_f;break;
		case 0x19:target_key = K_p;break;
		default:return;
	}
	if(updown)
		key_state[target_key] = KEY_STATE_RELEASE;
	else
		key_state[target_key] = KEY_STATE_PRESS;
}

static inline int
get_keycode(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return keycode_array[index];
}

static inline int
query_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return key_state[index];
}

static inline void
release_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_WAIT_RELEASE;
}

static inline void
clear_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_EMPTY;
}

bool 
process_keys(void (*key_press_callback)(int), void (*key_release_callback)(int)) {
	cli();
	/* TODO: Traverse the key states. Find a key just pressed or released.
	 * If a pressed key is found, call ``key_press_callback'' with the keycode.
	 * If a released key is found, call ``key_release_callback'' with the keycode.
	 * If any such key is found, the function return true.
	 * If no such key is found, the function return false.
	 * Remember to enable interrupts before returning from the function.
	 */

	int i;
	for(i=0;i<NR_KEYS;i++)
	{
		if(key_state[i] == KEY_STATE_PRESS)
		{
			key_press_callback(i);
			return true;
		}
		if(key_state[i] == KEY_STATE_RELEASE)
		{
			key_release_callback(i);
			return true;
		}
	}
	
	sti();
	return false;
}
