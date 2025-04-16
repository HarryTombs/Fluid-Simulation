cmake_minimum_required(VERSION 3.10.0)
project(SDL_Proj VERSION 0.1.0 LANGUAGES C CXX)

add_executable(SDL_Proj src/main.cpp)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/include)

target_link_libraries(SDL_Proj ${SDL2_LIBRARIES} )

# Find and link GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(SDL_Proj ${GLEW_LIBRARIES})

# Find and link GLUT
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
target_link_libraries(SDL_Proj ${GLUT_LIBRARIES})

find_package(OpenGL REQUIRED)
target_link_libraries(SDL_Proj ${OPENGL_LIBRARIES})