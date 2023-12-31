#include "stdio.h"

void
printf_test()
{
	printf("\nPRINTF TEST\n");
	printf("[info] successful if expected value matches the one we got\n");
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
