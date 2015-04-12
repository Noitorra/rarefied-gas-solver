# RarefiedGasSolver, department project.
SET(TARGET_NAME ${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_SOURCES
    "*.h"
    "*.cpp"
)

add_executable(${TARGET_NAME} ${PROJECT_SOURCES})

# Boost
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	include_directories(${BOOST_INCLUDE_PATH})
	target_link_libraries(${TARGET_NAME}
	  ${Boost_FILESYSTEM_LIBRARY}
	  ${Boost_SYSTEM_LIBRARY}
	)
endif()

# MPI
# include_directories(${MPI_INCLUDE_PATH})
# MESSAGE(STATUS MPI_LIBRARIES=${MPI_LIBRARIES})
# target_link_libraries(${TARGET_NAME} ${MPI_LIBRARIES})


# TBB
if(TBB_FOUND)
	include_directories(${TBB_INCLUDE_DIRS})
	target_link_libraries(${TARGET_NAME} ${TBB_LIBRARIES})
endif()
