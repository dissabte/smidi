message(STATUS "Processing smidi demo...")

set(SMIDIDEMO_SOURCES "main.cpp")

# watcher
include(../../../external/watcher.cmake)

add_executable(smidiDemo ${SMIDIDEMO_SOURCES})

add_dependencies(smidiDemo smidi watcher)

target_link_libraries (smidiDemo LINK_PUBLIC smidi watcher)

message(STATUS "Processing smidi demo done")

