add_library(
	mem
	OBJECT
	${PLATFORM_MEM_PROT}/early_boot.S
)

target_include_directories(mem PUBLIC ${PLATFORM_MEM_PROT}/include)
