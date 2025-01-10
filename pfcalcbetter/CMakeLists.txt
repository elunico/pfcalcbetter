cmake_minimum_required(VERSION 3.10)

# Project Name
project(MyCProject C)

# add glob for all .c files in the current directory
file(GLOB SOURCES "*.c")
list(REMOVE_ITEM SOURCES "/Users/thomaspovinelli/Developer/scratch/pfcalcbetter/pfcalcbetter/atest.c")

# Define the main executable
# add_executable(main *.c)

# add executable main using all the .c files from the prior glob
add_executable(main ${SOURCES})

add_executable(atest atest.c arena.c stack.c)

# Add include directories
include_directories(.)

option(DEBUG "debug mode" OFF)
if (DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    # add_compile_definitions(NOPRINT)
else() 
    add_compile_definitions(NDEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()

# Add optional compilation flags
option(LONG "Use long type" OFF)
if (LONG)
    add_compile_definitions(PF_NUM_LONG)
endif()

option(DOUBLE "Use double type" OFF)
if (DOUBLE)
    add_compile_definitions(PF_NUM_DOUBLE)
endif()

# compiler error if both long and double are set 
if (LONG AND DOUBLE)
    message(FATAL_ERROR "Cannot use both long and double types")
endif()

option(NOZDIV "Prevent zero division" OFF)
if (NOZDIV)
    add_compile_definitions(PFCALC_PREVENT_ZERO_DIV)
endif()
