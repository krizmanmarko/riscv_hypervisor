
#include "defs.h"
#include "dtb.h"
#include "lock.h"
#include "types.h"
#include "stdio.h"

// I do not want to rely on barriers for lock testing since barriers use locks
static struct lock lock_test_lk = LOCK_INITIALIZER;
static volatile int init_finished = 0;
static volatile int hart0_finished = 0;
static volatile int hart1_finished = 0;
static volatile int hart2_finished = 0;
static volatile int written_note = 0;

static uint64 hartid()
{
	uint64 id;
	__asm__ volatile("mv %0, tp" : "=r" (id));
	return id;
}

void
spinlock_test()
{
	if (hartid() == 0) {
		printf("\nSPINLOCK TEST (only runs on 3 cpus)\n");
		printf("[info] successful if As, Bs and Cs are seperated when using locks\n");
		printf("[info] first output is just sanity check and As, Bs and Cs should be mixed\n");
		printf("No locking:\n");
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

struct barrier b1 = BARRIER_INITIALIZER(DTB_NR_CPUS);

// basically spinlock test, but with barriers instead of global vars
void
barrier_test()
{
	if (hartid() == 0) {
		printf("\nBARRIER TEST (runs on DTB_NR_CPUS)\n");
		printf("[info] successful if all ABCs seperated from DEFs\n");
	}
	wait_barrier(&b1);
	for (int i = 0; i < 400; i++) {
		char buf[2] = { 'A', '\0' };
		buf[0] += hartid();
		printf(buf);
	}

	wait_barrier(&b1);
	if (hartid() == 0) {
		printf("\nBarrier:\n");
	}
	wait_barrier(&b1);
	for (int i = 0; i < 400; i++) {
		char buf[2] = { 'A' + DTB_NR_CPUS, '\0' };
		buf[0] += hartid();
		printf(buf);
	}
}
