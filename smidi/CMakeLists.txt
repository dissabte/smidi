cmake_minimum_required(VERSION 3.0)

set(LIBRARY_NAME smidi)

message(STATUS "Processing ${LIBRARY_NAME}...")

project(${LIBRARY_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SMIDI_INC_DIR "include")
set(SMIDI_SRC_DIR "src")

file(GLOB_RECURSE SMIDI_INCLUDE_FILES "${SMIDI_INC_DIR}/*.h")
file(GLOB_RECURSE SMIDI_SOURCE_FILES "${SMIDI_SRC_DIR}/*.cpp")
file(GLOB_RECURSE SMIDI_PRIVATE_INCLUDE_FILES "${SMIDI_SRC_DIR}/*.h")

set(SMIDI_SOURCES ${SMIDI_INCLUDE_FILES} ${SMIDI_PRIVATE_INCLUDE_FILES} ${SMIDI_SOURCE_FILES})

add_library(${LIBRARY_NAME} ${SMIDI_SOURCES})

target_include_directories(${LIBRARY_NAME} PUBLIC ${SMIDI_INC_DIR})

target_link_libraries(${LIBRARY_NAME} -lpthread)

# Unit tests
include(../external/unittest-cpp.cmake)
if(UNITTEST_CPP_FOUND)
	add_subdirectory("tests")
endif()

# Doxygen
set(SMIDI_DOCUMENTATION_DIR "docs")
include(${SMIDI_DOCUMENTATION_DIR}/doxygen.cmake)

message(STATUS "Processing ${LIBRARY_NAME} done")
