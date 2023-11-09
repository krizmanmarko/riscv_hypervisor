
#include "lock.h"
#include "types.h"

extern uint64 hartid();
extern void printf(char *fmt, ...);

static struct lock lock_test_lk;
static int init_finished = 0;
static int hart0_finished = 0;
static int hart1_finished = 0;
static int hart2_finished = 0;
static int written_note = 0;

void
spinlock_test()
{
	if (hartid() == 0) {
		printf("No locking:\n");
		init_lock(&lock_test_lk, "test");
		init_finished = 1;
	}
	while (!init_finished);
	for (int i = 0; i < 400; i++) {
		char buf[2] = { 'A', '\0' };
		buf[0] += hartid();
		printf(buf);
	}

	if (hartid() == 0) {
		hart0_finished = 1;
	} else if (hartid() == 1) {
		hart1_finished = 1;
	} else if (hartid() == 2) {
		hart2_finished = 1;
	}
	while (!hart0_finished || !hart1_finished || !hart2_finished);
	acquire(&lock_test_lk);
	if (!written_note) {
		printf("\nUsing locks:\n");
		written_note = 1;
	}
	for (int i = 0; i < 400; i++) {
		char buf[2] = { 'A', '\0' };
		buf[0] += hartid();
		printf(buf);
	}
	release(&lock_test_lk);
}
TODO: this is an old file
