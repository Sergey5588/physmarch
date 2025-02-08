cmake_minimum_required(VERSION 3.5.0)
project(ray_marching VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)
include_directories(include)

add_executable(ray_marching 
    src/main.cpp 
    include/glad.h
    src/glad.c
    include/VBO.h
    src/VBO.cpp
    include/VAO.h
    src/VAO.cpp
    include/EBO.h
    src/EBO.cpp
    include/shaderClass.h
    src/shaderClass.cpp
)
target_link_libraries(ray_marching 
    glfw
    OpenGL::GL
)



add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/resources
        ${PROJECT_BINARY_DIR}/resources
        COMMENT "Copying resources into binary directory")

add_dependencies(ray_marching copy_resources)
