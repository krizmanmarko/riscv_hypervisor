add_library(
	printf
	STATIC
	printf.c
)

target_link_libraries(printf PRIVATE uart)
target_include_directories(printf PUBLIC include)
