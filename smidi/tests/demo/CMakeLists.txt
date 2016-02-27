message(STATUS "Processing smidi demo...")

set(SMIDIDEMO_SOURCES "main.cpp")

add_executable(smidiDemo ${SMIDIDEMO_SOURCES})

target_link_libraries (smidiDemo LINK_PUBLIC smidi)

message(STATUS "Processing smidi demo done")

