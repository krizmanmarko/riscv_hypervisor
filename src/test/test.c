
#include "lock.h"
#include "types.h"
#include "panic.h"

extern void printf(char *fmt, ...);
extern void uartputc(char c);
extern uint64 hartid();

// intended to run on 1 hart
static void
cpu_test()
{

}

// intended to run on 3 harts
static struct lock lock_test_lk;
static int init_finished = 0;
static int hart0_finished = 0;
static int hart1_finished = 0;
static int hart2_finished = 0;
static int written_note = 0;
static void
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

// intended to run on 1 hart
static void
printf_test()
{
	printf("TESTING hexadecimal output\n");
	printf("expected: address of printf\n", printf);
	printf("got     : %p\n", printf);
	printf("expected: -0000000000000001\n");
	printf("got     : %xld\n", 0xffffffffffffffffL);
	printf("expected: 7fffffff\n");
	printf("got     : %xd\n", 0xffffffff7fffffffL);
	printf("expected: -0001\n");
	printf("got     : %xhd\n", 0xffffffff7fffffffL);
	printf("expected: -01\n");
	printf("got     : %xhhd\n", 0xffffffff7fffffffL);

	printf("expected: ffffffff7fffffff\n");
	printf("got     : %xlu\n", 0xffffffff7fffffffL);
	printf("expected: 7fffffff\n");
	printf("got     : %xu\n", 0xffffffff7fffffffL);
	printf("expected: ffff\n");
	printf("got     : %xhu\n", 0xffffffff7fffffffL);
	printf("expected: ff\n");
	printf("got     : %xhhu\n", 0xffffffff7fffffffL);
	printf("\n");

	printf("TESTING binary output\n");
	printf("expected: -0000000000000001\n");
	printf("got     : %bhd\n", 0xffff);
	printf("expected: -00000001\n");
	printf("got     : %bhhd\n", 0xffff);
	printf("expected: 11110000\n");
	printf("got     : %bhhu\n", 0xfff0);
	printf("\n");

	printf("TESTING decimal output\n");
	printf("expected: 0 -1 1 1234 -1234\n");
	printf("got     : %d %d %d %d %d\n", 0, -1, 1, 1234, -1234);
	printf("expected: 0 0\n");
	printf("got     : %hhd %hd\n", 0x100, 0x10000);
	printf("\n");

	printf("TESTING string output\n");
	printf("expected: Hello world %%s\n");
	printf("got     : %s\n", "Hello world %s");
	printf("expected: (null)\n");
	printf("got     : %s\n", (char *)0);
}

int printf_test_finished = 0;
void
test(int testid)
{
	if (testid == 0)
		printf_test();
	if (testid == 1)
		panic("panic test");
	if (testid == 2)
		spinlock_test();
}
