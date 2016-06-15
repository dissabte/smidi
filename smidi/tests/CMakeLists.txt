cmake_minimum_required(VERSION 3.0)

message(STATUS "Processing smidi unittests...")

set(SMIDI_TESTS smidi_tests)

include_directories(${UNITTEST_CPP_DIR})

file(GLOB SMIDI_TESTS_INCLUDE_FILES "*.h")
file(GLOB SMIDI_TESTS_SOURCE_FILES "*.cpp")

set(SMIDI_TESTS_SOURCES ${SMIDI_TESTS_INCLUDE_FILES} ${SMIDI_TESTS_SOURCE_FILES})

add_executable(${SMIDI_TESTS} ${SMIDI_TESTS_SOURCES})

target_link_libraries(${SMIDI_TESTS} UnitTest++ smidi)

message(STATUS "Processing smidi unittests done")
