README

This program is designed to render terrain heightmaps using OpenGL. It provides various modes for rendering the terrain, including point cloud, wireframe, solid triangles, and smoothed triangles. The user can manipulate the terrain using mouse and keyboard inputs.

## Prerequisites
- OpenGL
- GLUT
- GLEW (only for Windows)

## Installation and Execution
1. Compile the program using a C++ compiler. Make sure to link OpenGL, GLUT, and GLEW libraries properly.
2. Run the executable with a heightmap image file as an argument.
    `make ./hw1 heightmap/SantaMonicaMountains-256.jpg`

## Controls
- Left mouse button or Middle mouse: Rotate the terrain.
- Shift + Left mouse button: Scale the terrain along the x-y axis.
- Shift + Middle mouse button: Scale the terrain along the z-axis.
- Ctrl + Right mouse button: Translate the terrain in the x y plane.
- Ctrl + Middle mouse button: Translate the terrain in the z plane.
- '1': Render terrain as a point cloud.
- '2': Render terrain as wireframe.
- '3': Render terrain as solid triangles.
- '4': Render terrain as smoothed triangles with optional scaling and exponentiation (see below).
- '+': Increase the scaling factor (only available in mode 4).
- '-': Decrease the scaling factor (only available in mode 4).
- '9': Increase the exponentiation factor (only available in mode 4).
- '0': Decrease the exponentiation factor (only available in mode 4).
- 'x': Save a screenshot.

## Modes
1. **Point Cloud**: Renders each vertex of the terrain as a point.
2. **Wireframe**: Renders the edges of each triangle of the terrain.
3. **Solid Triangles**: Renders the terrain as solid triangles.
4. **Smoothed Triangles**: Renders the terrain as smoothed triangles. Additionally, this mode allows scaling and exponentiation of the terrain.

## File Structure
- `openGLHeader.h`, `glutHeader.h`, `openGLMatrix.h`, `imageIO.h`: Header files for OpenGL and helper classes.
- `pipelineProgram.h`, `vbo.h`, `vao.h`: Header files for OpenGL helper classes.
- `outputImages/`: Directory to store screenshots.
- `vertexShader.glsl`, `fragmentShader.glsl`: Vertex and fragment shader source files.
- `main.cpp`: Main source code file.

## Credits
This program is developed for CSCI 420: Computer Graphics course homework.

## Extras
 - Render wireframe on top of solid triangles 
 - Custom fragment shader to render mountain terrain with color
 - Mode 3 uses triangle strip to generate the triangle mode
