cmake_minimum_required(VERSION 3.0)

set(WATCHER_ROOT "${CMAKE_SOURCE_DIR}/../external/watcher/watcher")

if(EXISTS ${WATCHER_ROOT}/CMakeLists.txt)
	add_subdirectory(${WATCHER_ROOT} watcher)
else()
	message(FATAL_ERROR "Couldn't found watcher's cmake config. Please update watcher submodule.")
endif()
