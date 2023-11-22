cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0048 NEW)
project(opengles-workspace)
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glad)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")

add_library(opengles-workspace_lib STATIC 
    src/glfw_application.cpp
    src/main_loop.cpp
    src/renderer.cpp
    src/input.cpp
    src/game.cpp
    src/common.cpp
    third_party/glad/GL/src/gl.c
    )

include_directories(opengles-workspace_lib PUBLIC
    include
    third_party/glad/GL/include
    third_party/glfw/include
    third_party)

add_executable(opengles-workspace 
    main.cpp
)

target_link_libraries(opengles-workspace opengles-workspace_lib glfw ${GLFW_LIBRARIES})

configure_file(${CMAKE_SOURCE_DIR}/src/vertexshader.vertexshader ${CMAKE_BINARY_DIR}/vertexshader.vertexshader COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/fragmentshader.fragmentshader ${CMAKE_BINARY_DIR}/fragmentshader.fragmentshader COPYONLY)