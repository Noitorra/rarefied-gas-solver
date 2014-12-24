# RarefiedGasSolver, department project.
SET(TARGET_NAME ${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)

# find all dirs in curdir
MACRO(find_subdirs result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

macro(find_all_files out_files in_dir in_mask)
	set(${out_files} "")
	foreach(in_msk ${in_mask})
		set(temp_file_mask "${in_dir}/${in_msk}")
		file(GLOB temp_files ${temp_file_mask})
		set(${out_files} "${${out_files}};${temp_files}")
	endforeach()
endmacro()


# harvest files:
# 1) harvest include dir
# 2) harvest src dir
# 3) ....
# 4) PROFIT!!!

macro(harvest_directory in_parent_dir in_child_dir)
	set(loc_current_dir "${in_parent_dir}/${in_child_dir}")
	message(STATUS "HARVESTING: " ${loc_current_dir})
	find_all_files(in_dir_source ${loc_current_dir} "*.h;*.hpp;*.cpp")
	STRING(REGEX REPLACE "/" "\\\\" in_child_dir_source_group "${in_child_dir}")
	source_group(${in_child_dir_source_group} FILES ${in_dir_source})
	set(${TARGET_NAME}_source "${${TARGET_NAME}_source};${in_dir_source}")
	message(STATUS "FOUND: " ${in_dir_source})
	find_subdirs(loc_subdirs ${loc_current_dir})
	foreach(loc_subdir ${loc_subdirs})
		harvest_directory(${in_parent_dir} "${in_child_dir}/${loc_subdir}")
	endforeach()
endmacro()

harvest_directory("${CMAKE_SOURCE_DIR}" "inc")
harvest_directory("${CMAKE_SOURCE_DIR}" "src")
#source_group("${subdir}" FILES ${subdir_source})

add_executable(${TARGET_NAME} ${${TARGET_NAME}_source})

# Boost
find_package(Boost COMPONENTS system filesystem REQUIRED)
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
# Add FindTBB directory to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/")
find_package(TBB REQUIRED)
if(TBB_FOUND)
	include_directories(${TBB_INCLUDE_DIRS})
	target_link_libraries(${TARGET_NAME} ${TBB_LIBRARIES})
endif()