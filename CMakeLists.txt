cmake_minimum_required(VERSION 3.24)
project(egyptianLandmarks)

set(CMAKE_CXX_STANDARD 17)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(glad/)
add_executable(egyptianLandmarks main.cpp glad/include/glad/glad.h)

target_link_libraries(egyptianLandmarks glad glfw OpenGL::GL
        pthread X11 Xrandr Xi dl)