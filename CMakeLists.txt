cmake_minimum_required(VERSION 2.8.4)

project(RGS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("COMPILER = ${CMAKE_CXX_COMPILER_ID}")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    add_compile_options(-std=c++14)
endif()

# Require MPI for this project:
find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_CXX_LINK_FLAGS})

# Require Boost
find_package(Boost COMPONENTS system filesystem serialization REQUIRED)

set(output_dir "${CMAKE_CURRENT_SOURCE_DIR}/bin/")

# First for the generic no-config case (e.g. with mingw)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${output_dir} )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${output_dir} )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${output_dir} )

# Second, for multi-config builds (e.g. msvc)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${output_dir} )
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${output_dir} )
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${output_dir} )
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

include_directories(src)
add_subdirectory(src)
