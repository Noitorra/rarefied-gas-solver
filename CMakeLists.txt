cmake_minimum_required(VERSION 2.8.4)

project(RGS)

# Initialize CXXFLAGS.
cmake_policy(SET CMP0054 OLD)
message("COMPILER = ${CMAKE_CXX_COMPILER_ID}")
if(UNIX)
	if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	endif()
endif()
# set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
# set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# Require MPI for this project:
find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_CXX_LINK_FLAGS})

# Require Boost
find_package(Boost COMPONENTS system filesystem serialization REQUIRED)

set(output_dir "${CMAKE_CURRENT_SOURCE_DIR}/bin/")

# First for the generic no-config case (e.g. with mingw)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${output_dir} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${output_dir} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${output_dir} )

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${output_dir} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${output_dir} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${output_dir} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

include_directories(src)
add_subdirectory(src)
