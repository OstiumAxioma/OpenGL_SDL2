cmake_minimum_required(VERSION 3.10)

project(TestProject)

set(CMAKE_CXX_STANDARD 17)

# Add SDL2 and GLAD directories
include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

# Add source files
set(SOURCES
    src/main.cpp
    src/glad.c
)

# Create executable
add_executable(TestProject ${SOURCES})

# Link SDL2 and GLAD
target_link_libraries(TestProject SDL2)
