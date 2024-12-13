add_library(
	uart
	STATIC
	uart.S
)

target_include_directories(uart INTERFACE include)
