#include "types.h"

int
count_set_bits(uint64 num)
{
	int count = 0;
	for (int i = 1; i != 0; i <<= 1)
		if (num & i)
			count++;
	return count;
}
