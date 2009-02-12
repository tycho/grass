-----------------------------------------------------------
Grass Benchmark 1.0 (March 5, 2005)
Louis Bavoil <bavoil@cs.utah.edu>
-----------------------------------------------------------

This is an OpenGL benchmark that renders blades of grass.

The blades are grouped in batches. Each batch is rendered using
glBegin/glEnd, vertex arrays (one array per attribute), a display list
of vertex arrays, or separate vertex buffer objects (VBOs).

The total number of rendered triangles can be changed arbitrarily.

Vertices can be rendered with additional attributes: GL_FLOAT vertex
normals, and GL_UNSIGNED_BYTE vertex colors.

To benchmark the pre-transform-and-lighting cache, the number of
batches can be changed manually, or automatically. In the automatic
mode, the program attempts to estimate the optimal count of vertices
per batch, that is the size of the pre-T&L cache.

EXAMPLES:

To estimate the size of the pre-T&L cache using VBOs:
./grass_benchmark -pre-tl -pre-tl-jump 0.1 -method VBO

To compare display-lists of vertex arrays with VBOs:
./grass_benchmark -tris 1000000 -batches 200 -method DL
./grass_benchmark -tris 1000000 -batches 200 -method VBO
