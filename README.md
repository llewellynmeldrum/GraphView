# GraphView
For simulating and viewing graph algorithms.

TODO:
- Implement graph rendering (just have some UI and a circle atm)
- Implement a few algorithms with playback controls of some kind
- bfs first,
- then dijkstras

# Usage
1. install dependencies
2. 
```bash
make run

```

# Dependencies
- GNU make
- GLFW (http://www.glfw.org):
```md
Linux:
  apt-get install libglfw-dev
Mac OS X:
  brew install glfw
MSYS2:
  pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
```
- OpenGL, used as part of the ImGui backend, and used directly for rendering of the graphs
- other 2 libraries are just embedded in /external

# Libraries used
- GLFW, for windowing and handling input
- ImGui: for UI. Also used to understand basics of GLFW.
- OpenGL, used as part of the ImGui backend, and used directly for rendering of the graphs
- glad: simplifies making certain OpenGL calls and allows for better usage of extensions


