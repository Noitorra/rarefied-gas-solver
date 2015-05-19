cmake_minimum_required(VERSION 2.6)

project(RarefiedGasSolver)

# Initialize CXXFLAGS.
if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
# set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
# set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# Require MPI for this project:
# find_package(MPI REQUIRED)
# set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
# set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})

# Add out modules directory to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(TBB REQUIRED)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/")

include_directories(src)
add_subdirectory(src)
