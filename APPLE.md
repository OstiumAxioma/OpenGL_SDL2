
# Building and Running TestProject on macOS

## Prerequisites

1. **Homebrew**: Ensure you have Homebrew installed. If not, you can install it by running:
   ```sh
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. **SDL2**: Install SDL2 via Homebrew:
   ```sh
   brew install sdl2
   ```

## Getting the SDL2 Version

To find the version of SDL2 installed via Homebrew, run:
```sh
brew info sdl2
```
Look for the version number in the output. It will be in the format `2.x.y`.

## Building TestProject

1. **Clone the Repository**: If you haven't already, clone the repository containing TestProject.
   ```sh
   git clone https://github.com/SaeedYazdanian/ComputerGraphics001.git
   cd ComputerGraphics001
   ```

2. **Set the SDL2 Version**: Open the 

CMakeLists.txt

 file and set the `SDL2_VERSION` variable to the version of SDL2 installed via Homebrew. For example:
   ```cmake
   set(SDL2_VERSION "2.x.y" CACHE STRING "SDL2 version installed via Homebrew")
   ```

3. **Create a Build Directory**: Create a build directory and navigate into it.
   ```sh
   mkdir build
   cd build
   ```

4. **Run CMake**: Configure the project with CMake, specifying the SDL2 version if different from the default.
   ```sh
   cmake -DSDL2_VERSION=2.x.y ..
   ```

5. **Build the Project**: Compile the project using `make`.
   ```sh
   make VERBOSE=1
   ```

## Running TestProject

After building the project, you can run the executable:
   ```sh
   ./TestProject
   ```

## Troubleshooting

If you encounter issues with the SDL2 library not being found at runtime, ensure that the `rpath` is correctly set in the 

CMakeLists.txt

 file. The `INSTALL_RPATH` should point to the directory containing the `libSDL2.dylib` file.

Example 

CMakeLists.txt

 snippet:
   ```cmake
   set_target_properties(TestProject PROPERTIES
       BUILD_WITH_INSTALL_RPATH TRUE
       INSTALL_RPATH "/usr/local/Cellar/sdl2/${SDL2_VERSION}/lib"
   )
   ```

If you need to override the SDL2 version, you can pass the version as an argument when running CMake:
   ```sh
   cmake -DSDL2_VERSION=2.x.y ..
   ```

Replace `2.x.y` with the actual version number of SDL2 installed on your system.

## Example Output

When you run TestProject, you should see a window with a red background. The application will continue running until you close the window.

---
