// this section is interesting
// it is needed to get struct offset and size information into assembly files

// ## is used to concatenate (https://en.wikipedia.org/wiki/C_preprocessor#Token_concatenation)
// but then ##__VA_ARGS__ is something else...

#define DEFINE(sym, val) \
	asm volatile("\n-> " #sym " %0 " #val "\n" : : "i" (val))

#define OFFSETOF(s, field) \
	DEFINE(offsetof_##s##_##field, offsetof(s, field));
