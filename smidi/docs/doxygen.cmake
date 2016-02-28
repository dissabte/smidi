cmake_minimum_required(VERSION 3.0)

message(STATUS "Processing Doxygen...")

find_package(Doxygen)
if(DOXYGEN_FOUND)
	set(DOXYGEN_INPUT ${CMAKE_SOURCE_DIR})
	set(README_MD "${CMAKE_SOURCE_DIR}/../README.md")
	configure_file(${CMAKE_SOURCE_DIR}/docs/doxygen.conf.in ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf)
	add_custom_target(smidi_docs COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf)
else(DOXYGEN_FOUND)
	message(STATUS "Warning: doxygen not found - reference manual will not be generated.")
endif(DOXYGEN_FOUND)

message(STATUS "Processing Doxygen done")


