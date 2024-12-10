cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(PREFIX /home/marko/shit/riscv-gnu-toolchain-from-source/beyond/beyond/bin/riscv32-unknown-elf-)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -z max-page-size=4096")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -z common-page-size=4096")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdlib")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nodefaultlibs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv32imaf_zicsr_zifencei_sstc")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv32imafc_zicsr_zifencei_zca_sstc")
set(CMAKE_C_COMPILER ${PREFIX}gcc)

project(hypervisor LANGUAGES C ASM)

add_subdirectory(core)
#add_subdirectory(platform)
