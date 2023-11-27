cmake_minimum_required(VERSION 2.8...3.13)
project(test)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(TGUI CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Wget REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

set(CMAKE_VERBOSE_MAKEFILE ON)
file(GLOB SRCS main.cpp )
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} GLEW::GLEW tgui sfml-system sfml-network sfml-graphics sfml-window)
