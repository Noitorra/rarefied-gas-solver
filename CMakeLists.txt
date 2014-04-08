cmake_minimum_required(VERSION 2.6)

project(RarefiedGasSolver)

# Initialize CXXFLAGS.
set(CMAKE_CXX_FLAGS                "-Wall -std=c++0x")
# set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
# set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else ()
    message(MESSAGE "Your C++ compiler does not support C++11.")
endif () 


# Require MPI for this project:
# find_package(MPI REQUIRED)
# set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
# set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")

include_directories(inc)

add_subdirectory(src)
