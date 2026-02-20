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
Debian-based Linux:
  apt-get install libglfw-dev
MacOS:
  brew install glfw
```
- GLM 
```md
MacOS:
  brew install glm 
```
- OpenGL, used as part of the ImGui backend, and used directly for rendering of the graphs
- other 2 libraries are just embedded in /external

# Libraries used
- GLFW, for windowing and handling input
- ImGui: for UI. Also used to understand basics of GLFW.
- OpenGL, used as part of the ImGui backend, and used directly for rendering of the graphs
- glad: simplifies making certain OpenGL calls and allows for better usage of extensions
- GLM: OpenGL graphics library


