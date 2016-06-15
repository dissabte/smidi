message(STATUS "Processing smidi demo...")

set(SMIDIDEMO_SOURCES "main.cpp")

if (HOTPLUG_FOUND)
	add_executable(smidiDemo ${SMIDIDEMO_SOURCES})
	add_dependencies(smidiDemo smidi hotplug)
	target_link_libraries (smidiDemo LINK_PUBLIC smidi hotplug)
else()
	message(WARNING "hotplug library is required to build smidi demo")
endif()

message(STATUS "Processing smidi demo done")

