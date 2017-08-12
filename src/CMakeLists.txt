# helper function
function(create_source_group SUB_DIR)
    set(FULL_SUB_DIR ${PROJECT_SOURCE_DIR}/${SUB_DIR})
    file(GLOB SUB_DIR_SOURCES
            "${FULL_SUB_DIR}/*.h"
            "${FULL_SUB_DIR}/*.hpp"
            "${FULL_SUB_DIR}/*.cpp"
            )
    string(REPLACE "/" "\\" SOURCE_GROUP ${SUB_DIR})
    source_group(${SOURCE_GROUP} FILES ${SUB_DIR_SOURCES})
endfunction()

# RarefiedGasSolver, department project.
set(TARGET_NAME ${PROJECT_NAME})

# Create groups for visual studio 
create_source_group(src)
file(GLOB_RECURSE SUB_DIRS LIST_DIRECTORIES TRUE RELATIVE ${PROJECT_SOURCE_DIR} *)
foreach (SUB_DIR ${SUB_DIRS})
    if (IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${SUB_DIR})
        create_source_group(${SUB_DIR})
    endif ()
endforeach ()

file(GLOB_RECURSE PROJECT_SOURCES
        "*.h"
        "*.hpp"
        "*.cpp"
        )

add_executable(${TARGET_NAME} ${PROJECT_SOURCES})

# MPI
include_directories(${MPI_CXX_INCLUDE_PATH})
MESSAGE(STATUS MPI_CXX_LIBRARIES=${MPI_CXX_LIBRARIES})
target_link_libraries(${TARGET_NAME} ${MPI_CXX_LIBRARIES})

# Boost
if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	include_directories(${BOOST_INCLUDE_PATH})
	target_link_libraries(${TARGET_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_SERIALIZATION_LIBRARY})
endif ()

