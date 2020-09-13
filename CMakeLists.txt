cmake_minimum_required(VERSION 3.10)
project(coinop_emulator)

# "Pure" C++17 and no extensions.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default debug and optimize flags.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

include_directories(${CMAKE_SOURCE_DIR})

if(NOT EMSCRIPTEN)
    # GLFW
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/subrepos/glfw/)
    find_package(OpenGL REQUIRED)
    find_package(OpenAL REQUIRED)
endif()

# Dependency libraries.
add_subdirectory(z80)
add_subdirectory(mb88xx)
add_subdirectory(emulator)

# Galaga Machine engine
file(GLOB SOURCES_GALAGA ${CMAKE_CURRENT_SOURCE_DIR}/galaga/*.cpp)
list(APPEND SOURCES_GALAGA "${CMAKE_CURRENT_SOURCE_DIR}/subrepos/lodepng/lodepng.cpp")

add_library(galaga_lib ${SOURCES_GALAGA})
if(!EMSCRIPTEN)
    target_include_directories(galaga_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/subrepos/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR})
else()
    target_include_directories(galaga_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/subrepos/glm
        ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Set Galaga Machine libraries
set(GALAGA_MACHINE_LIBRARIES galaga_lib z80_lib mb88xx_lib emulator_lib)

if(EMSCRIPTEN)
    add_definitions(-DEMSCRIPTEN)

    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} --bind --memory-init-file 0 -s TOTAL_MEMORY=64MB")
    # Embed files at build-dir/data
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --embed-file \"${CMAKE_BINARY_DIR}/data@data\"")
    
    file(GLOB SOURCES_WASM_APP ${CMAKE_CURRENT_SOURCE_DIR}/glfwApp/*.cpp)

    add_executable(wasm_app ${SOURCES_WASM_APP})
    target_link_libraries(wasm_app 
        ${GALAGA_MACHINE_LIBRARIES}
    )
    target_include_directories(wasm_app PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/subrepos/glm/
        ${CMAKE_CURRENT_SOURCE_DIR}/subrepos/lodepng
    )

    # Use webgl2 and glfw3
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_WEBGL2=1 -s USE_GLFW=3")
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s BINARYEN_ASYNC_COMPILATION=1 -s WASM=1 -s BINARYEN=1 -s \"BINARYEN_METHOD='native-wasm'\" -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -o app_wasm2.html")
    # Enable js output (no html).
    #set(CMAKE_EXECUTABLE_SUFFIX "_wasm.js")
    set(CMAKE_EXECUTABLE_SUFFIX "_emsdk.html")
    add_definitions(-DUSE_WASM)

    # Copy the content of the sourcedir/WebApplication directory
	add_custom_command(TARGET wasm_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/WebApplication ${CMAKE_BINARY_DIR}/
        COMMENT "Copying files from ${CMAKE_SOURCE_DIR}/WebApplication/ to ${CMAKE_BINARY_DIR}/WebApplication")

    # Copy assets to build-dir/data/ directory
    add_custom_command(TARGET wasm_app PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
        COMMENT "Copying files from ${CMAKE_SOURCE_DIR}/data/ to ${CMAKE_BINARY_DIR}/data")

else()

    file(GLOB SOURCES_WASM_APP ${CMAKE_CURRENT_SOURCE_DIR}/glfwApp/*.cpp)    
    add_executable(glfw_app
        ${SOURCES_WASM_APP}
        glad/src/glad.c)
    target_include_directories(glfw_app PUBLIC
        ${OPENGL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/subrepos/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/subrepos/glm/
        ${CMAKE_CURRENT_SOURCE_DIR}/subrepos/lodepng
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${OPENAL_INCLUDE_DIR})
    target_link_libraries(glfw_app
        glfw
        ${GALAGA_MACHINE_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${OPENGL_gl_LIBRARY}
        ${OPENAL_LIBRARY})

    # Copy assets to build-dir/data/ directory
    add_custom_command(TARGET glfw_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
        COMMENT "Copying files from ${CMAKE_SOURCE_DIR}/data/ to ${CMAKE_BINARY_DIR}/data")

endif()