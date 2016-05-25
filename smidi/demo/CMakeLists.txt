message(STATUS "Processing smidi demo...")

set(SMIDIDEMO_SOURCES "main.cpp")

if (WATCHER_PATH)
	add_subdirectory("${WATCHER_PATH}" watcher)
	add_executable(smidiDemo ${SMIDIDEMO_SOURCES})
	add_dependencies(smidiDemo smidi watcher)
	target_link_libraries (smidiDemo LINK_PUBLIC smidi watcher)
else()
	message(STATUS "Please specify path to watcher library (WATCHER_PATH) to build this demo")
endif()

message(STATUS "Processing smidi demo done")

