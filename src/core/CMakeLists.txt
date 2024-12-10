# make sure linkerscript is pre-processed (for #include directive)
add_custom_command(
	OUTPUT linkerscript.ld
	COMMAND ${CMAKE_C_COMPILER}
		-P
		-E ${CMAKE_CURRENT_SOURCE_DIR}/linkerscript.c
		-o linkerscript.ld
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/linkerscript.c
)

add_executable(hypervisor
	entry.S
	linkerscript.ld
)

target_link_options(
	hypervisor
	PRIVATE "-T${CMAKE_CURRENT_BINARY_DIR}/linkerscript.ld"
	PRIVATE "-nostartfiles"
)
