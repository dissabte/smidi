cmake_minimum_required(VERSION 3.0)

set(LIBRARY_NAME smidi)

message(STATUS "Processing ${LIBRARY_NAME}...")

project(${LIBRARY_NAME})

add_definitions(-DSMIDI_USE_ALSA)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SMIDI_INC_DIR "include")
set(SMIDI_SRC_DIR "src")

file(GLOB_RECURSE SMIDI_INCLUDE_FILES "${SMIDI_INC_DIR}/*.h")
file(GLOB_RECURSE SMIDI_SOURCE_FILES "${SMIDI_SRC_DIR}/*.cpp")
file(GLOB_RECURSE SMIDI_PRIVATE_INCLUDE_FILES "${SMIDI_SRC_DIR}/*.h")

# TODO: exclude those for osx and win

include_directories(${SMIDI_INC_DIR})

set(SMIDI_SOURCES ${SMIDI_INCLUDE_FILES} ${SMIDI_PRIVATE_INCLUDE_FILES} ${SMIDI_SOURCE_FILES})

if(LINUX)
	add_definitions(-Wall -Werror)
endif(LINUX)

# Unit tests
if(NOT UNITTEST_CPP_FOUND)
	include(../external/unittest-cpp.cmake)
endif()

add_library(${LIBRARY_NAME} ${SMIDI_SOURCES})

target_include_directories(${LIBRARY_NAME} PUBLIC ${SMIDI_INC_DIR})

# Unit tests
if(UNITTEST_CPP_FOUND)
	add_dependencies(${LIBRARY_NAME} UnitTest++)
	add_subdirectory("tests")
endif()

target_link_libraries(${LIBRARY_NAME} -lpthread -lasound)

# demo app
add_subdirectory("demo")

# Doxygen
set(SMIDI_DOCUMENTATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")
if (EXISTS "${SMIDI_DOCUMENTATION_DIR}/doxygen.cmake")
	include(${SMIDI_DOCUMENTATION_DIR}/doxygen.cmake)
endif()

message(STATUS "Processing ${LIBRARY_NAME} done")
