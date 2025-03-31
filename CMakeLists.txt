cmake_minimum_required(VERSION 3.10)
project(ChargedParticles CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(/include)

# Get all .cpp files in the current directory
file(GLOB SOURCES "*.cpp")

add_executable(ChargedParticles ${SOURCES})

target_link_libraries(ChargedParticles glfw)
target_link_libraries(ChargedParticles GLEW)
target_link_libraries(ChargedParticles OpenGL::GL)