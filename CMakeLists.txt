cmake_minimum_required(VERSION 3.10)

project(TestProject)

set(CMAKE_CXX_STANDARD 17)

# Add source files
set(SOURCES
    src/main.cpp
    src/glad.c
)

# Create executable
add_executable(TestProject ${SOURCES})

# Add project include directory for both platforms
include_directories(${PROJECT_SOURCE_DIR}/include)

# Define SDL2 version variable
# This variable specifies the version of SDL2 installed via Homebrew.
# Students can change this value if they have a different version of SDL2 installed.
# To override this value, pass -DSDL2_VERSION=<new_version> when running CMake.
# Example: cmake -DSDL2_VERSION=2.0.14 ..
set(SDL2_VERSION "2.30.7" CACHE STRING "SDL2 version installed via Homebrew")

if (APPLE)
    # macOS specific settings
    # Include SDL2 headers from the Homebrew installation path
    include_directories(/opt/homebrew/include/SDL2)
    
    # Link against the dynamic library instead of the framework
    target_link_libraries(TestProject /opt/homebrew/Cellar/sdl2/${SDL2_VERSION}/lib/libSDL2.dylib "-framework OpenGL")

    # Set rpath to find SDL2 dynamic library at runtime
    set_target_properties(TestProject PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "/opt/homebrew/Cellar/sdl2/${SDL2_VERSION}/lib"
    )
elseif(WIN32)
    # Windows specific settings
    include_directories(${PROJECT_SOURCE_DIR}/include)
    link_directories(${PROJECT_SOURCE_DIR}/lib)
    target_link_libraries(TestProject SDL2)
endif()