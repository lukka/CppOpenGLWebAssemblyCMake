# emulator library
file(GLOB SOURCES_EMULATOR ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(emulator_lib ${SOURCES_EMULATOR})
