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
foreach(SUB_DIR ${SUB_DIRS})
    if (IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${SUB_DIR})
        create_source_group(${SUB_DIR})
    endif()
endforeach()

file(GLOB_RECURSE PROJECT_SOURCES
    "*.h"
    "*.hpp"
    "*.cpp"
)

add_executable(${TARGET_NAME} ${PROJECT_SOURCES})

# MPI
# include_directories(${MPI_INCLUDE_PATH})
# MESSAGE(STATUS MPI_LIBRARIES=${MPI_LIBRARIES})
# target_link_libraries(${TARGET_NAME} ${MPI_LIBRARIES})

# TBB
if(TBB_FOUND)
	include_directories(${TBB_INCLUDE_DIRS})
	target_link_libraries(${TARGET_NAME} ${TBB_LIBRARIES})
endif()

