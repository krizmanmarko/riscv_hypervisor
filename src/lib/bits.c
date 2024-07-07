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

// offset goes from right to left [7, 6, 5, 4, 3, 2, 1, 0]
// uint64[6:0] -> get_value(val, 0, 7);
uint64
get_value(uint64 val, int offset, int bits)
{
	uint64 mask = 0;
	if (offset < 0 || 63 < offset)
		return -1;
	if (bits < 0 || 63 < bits)
		return -1;

	mask = 0;
	for (int i = 0; i < bits; i++)
		mask = mask << 1 | 1;
	return val >> offset & mask;
}
