# lynx

This project is a lightweight rendering library built with Vulkan, designed to render simple primitives and geometric shapes. It primarily serves to draw and represent the dynamic objects of my 2D physics engine, [poly-physx](https://github.com/ismawno/poly-physx).

## Features

- Rendering of simple primitives using Vulkan

## Dependencies

- [glm](https://github.com/g-truc/glm)
- [cpp-kit](https://github.com/ismawno/cpp-kit)
- [yaml-cpp](https://github.com/ismawno/yaml-cpp) (optional)
- [spdlog](https://github.com/gabime/spdlog) (optional)
- [glfw](https://github.com/glfw/glfw)
- [imgui](https://github.com/ocornut/imgui)
- [implot](https://github.com/epezent/implot)

## Building and Usage

This project is intended to be used as a git submodule within another project (parent repo). A premake file is provided for building and linking lynx.

While these build instructions are minimal, this project is primarily for personal use. Although it has been built and tested on multiple machines (MacOS and Windows), it is not necessarily fully cross-platform or easy to build.

## License

lynx is licensed under the MIT License. See LICENSE for more details.
