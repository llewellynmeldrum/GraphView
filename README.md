# GraphView
For simulating and viewing graph algorithms. Right now it sorta just renders graphs. But it looks cool:
<br>
<img width="600" alt="image" src="https://github.com/user-attachments/assets/b8ee95b3-175c-4579-bb69-4c4a7a5958da" />
<br>

## Features
- idk nice imgui gui
- Made with legibility/readability in mind
<img width="600" alt="image" src="https://github.com/user-attachments/assets/241c6b22-798c-4412-9f76-b1e844cd517e" />

TODO:
- ~~Implement graph rendering~~
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
- ImGui, for UI
- OpenGL, used as part of the ImGui backend, and used directly for drawing lines and circles
  - glad: simplifies making certain OpenGL calls and allows for better usage of extensions
  - GLM: OpenGL math library


# Credits 
- https://github.com/mhalber/Lines/
- (Holten, Danny; van Wijk, Jarke J. (2009), "A user study on visualizing directed edges in graphs")[https://web.archive.org/web/20111106004500/http://www.win.tue.nl/~dholten/papers/directed_edges_chi.pdf]


