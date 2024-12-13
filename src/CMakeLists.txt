cmake_minimum_required(VERSION 3.16.3)


###################
# platform config #
###################

set(PLATFORM_CMAKE "${CMAKE_SOURCE_DIR}/platform/$ENV{PLATFORM}/platform.cmake")
if(NOT EXISTS "${PLATFORM_CMAKE}")
message(FATAL_ERROR "Bad PLATFORM environment variable")
endif(NOT EXISTS "${PLATFORM_CMAKE}")

include(${PLATFORM_CMAKE})

foreach(VAR IN ITEMS PLATFORM_ARCH PLATFORM_MEM_PROT)
	if(NOT DEFINED ${VAR})
	    message(FATAL_ERROR "bad platform: ${VAR} is not set")
	endif(NOT DEFINED ${VAR})
endforeach()


#########################
# global compiler flags #
#########################

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ${PLATFORM_ARCH})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -z max-page-size=4096")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -z common-page-size=4096")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdlib")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nodefaultlibs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PLATFORM_MARCH}")
set(PREFIX ${PLATFORM_ARCH}-unknown-elf-)
set(CMAKE_C_COMPILER ${PREFIX}gcc)


###########
# project #
###########

project(hypervisor LANGUAGES C ASM)
set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(include)
include_directories(platform/$ENV{PLATFORM}/include)

add_subdirectory(mem/${PLATFORM_MEM_PROT})
add_subdirectory(driver)
add_subdirectory(lib)
add_subdirectory(core)
