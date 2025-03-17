# vk-pathtracer
[WIP] Interactive, physically-based rendering engine using the Vulkan API. Includes full camera control, a custom BVH builder, glTF 2.0 loader, and much more. This is a hobby project for my own learning and likely has many naive implementations.
![Ajax](./assets/screenshots/Ajax.png)

## Requirements
- Windows, Mac, or Linux
- Vulkan SDK v1.3+
- GCC/Clang with C++17
- CMake v3.20+

## Compilation
### Mac & Linux:

```bash
git clone https://github.com/tylertms/vk-pathtracer
cd vk-pathtracer
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
./build/bin/vk-pathtracer
```
### Windows:

Install Visual Studio, MinGW, MSYS, or another C++ toolchain.
```bash
git clone https://github.com/tylertms/vk-pathtracer
cd vk-pathtracer
cmake -B build -DCMAKE_BUILD_TYPE=Release
```
If a Makefile generator cannot be found, include the `-G` flag with your corresponding generator, i.e., `MinGW Makefiles` or `MSYS Makefiles`.
```bash
cmake --build build -j4
.\build\bin\vk-pathtracer.exe
```
## Features
- GPU-accelerated pathtracing algorithm
- Fully interactive scene (drag/zoom/pan)
- Control of camera FOV, focal distance, focal strength, exposure, etc.
- glTF 2.0 loader
- Position objects and adjust material properties from the sidebar
- Mix diffuse, specular, metallic, and dielectric BxDFs
- HDR environment mapping (equirectangular)
- ACES HDR tone mapping
- Partial texture support
- Save and load scenes using YAML files
- And more...

## Roadmap
- Hardware raytracing support (VK_KHR_ray_tracing_pipeline)
- Top level acceleration structure for scenes with many meshes
- Improved glTF texture support and object hierarchy
- BVH optimizations, construction through compute shader
  
## Issues
- Scene saving/loading is currently broken
- Creating new scenes is buggy and will sometimes crash the program
- Textures may not always be detected and may load incorrectly
Please report further issues using the Github Issue feature

## Gallery
![Dragon](./assets/screenshots/Dragon.png)
![Lucy](./assets/screenshots/Lucy.png)
![Spheres](./assets/screenshots/Spheres.png)


## References
[Coding Adventure: Ray Tracing - Sebastian Lague](https://www.youtube.com/watch?v=Qz0KTGYJtUk)
[Coding Adventure: Optimizing a Ray Tracer (by building a BVH) - Sebastian Lague](https://www.youtube.com/watch?v=C1H4zIiCOaI)
