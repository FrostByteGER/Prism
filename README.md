# Prism Engine
[![Build](https://github.com/FrostByteGER/Prism/workflows/Build/badge.svg)](https://github.com/FrostByteGER/Prism/actions?query=Build)

# Requirements
- Windows 10 1803 or later or Linux
  - Alternative: `curl` and `tar` for Windows
- Python 3.9 with `pip` or later 
  - Alternative: Standalone Conan C++ Package Manager 2.0 or later
- CMake 3.24.0 or later
- VulkanSDK 1.3.296.0 or later for debug builds. If you wish to compile release builds only, you can omit this

## Setup
- Launch the `Bootstrap.bat` or `Bootstrap.sh`


## Mesh Importing
Currently meshes have to be exported from blender with 
- Forward Axis: Z (The direction the mesh looks at) an
- Up Axis: -Y (The direction the mesh is standing on, or the direction of the invisible sky)

## Notes
Linux support is experimental and while it compiles, it hasn't been tested!
