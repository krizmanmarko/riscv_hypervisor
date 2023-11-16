void *
memset(void *dst, char c, int size)
{
	char *p = (char *)dst;
	for (int i = 0; i < size; i++) {
		p[i] = c;
	}
	return dst;
}
