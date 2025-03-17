# vk-pathtracer
Interactive, physically-based rendering engine using the Vulkan API. 
![Ajax](./assets/screenshots/Ajax.png)

## Requirements
- Windows, Mac, or Linux
- Vulkan SDK v1.3+
- GCC/Clang with C++17
- CMake v3.20+

## Compile and Run
### Mac & Linux:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
./build/bin/vk-pathtracer
```
### Windows:

Install Visual Studio, MinGW, MSYS, or another C++ toolchain.
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```
If a Makefile generator cannot be found, include the `-G` flag with your corresponding generator, i.e., `MinGW Makefiles` or `MSYS Makefiles`.
```bash
cmake --build build -j4
.\build\bin\vk-pathtracer.exe
```
