add_library(
	print
	STATIC
	print.c
)

target_link_libraries(print PRIVATE uart)
target_include_directories(print PUBLIC include)
