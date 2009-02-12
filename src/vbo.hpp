/*
 Copyright (C) 2005 Louis Bavoil <bavoil@cs.utah.edu>                 

 This program is free software; you can redistribute it and/or        
 modify it under the terms of the GNU Library General Public License  
 as published by the Free Software Foundation; either version 2       
 of the License, or (at your option) any later version.               

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 GNU Library General Public License for more details.
*/

#include <GL/glew.h>
#include <vector>
#include <stdio.h>

#include "glh/glh_linear.h"
using namespace glh;

class Point_3f {
public:
     Point_3f() {}

     Point_3f(vec3f &v) {
	  m_v[0] = v[0];
	  m_v[1] = v[1];
	  m_v[2] = v[2];
     }
     Point_3f(GLfloat *v) {
	  m_v[0] = v[0];
	  m_v[1] = v[1];
	  m_v[2] = v[2];
     }

     GLfloat m_v[3];
};

class Vector_3b {
public:
     Vector_3b() {}

     Vector_3b(vec3f &v) {
	  m_v[0] = (GLbyte) (v[0] * 127);
	  m_v[1] = (GLbyte) (v[1] * 127);
	  m_v[2] = (GLbyte) (v[2] * 127);
     }
     Vector_3b(GLfloat *v) {
	  m_v[0] = (GLbyte) (v[0] * 127);
	  m_v[1] = (GLbyte) (v[1] * 127);
	  m_v[2] = (GLbyte) (v[2] * 127);
     }
  
     GLbyte m_v[3];
};

class Vector_4b {
public:
     Vector_4b() {}

     Vector_4b(vec3f &v) {
	  m_v[0] = (GLbyte) (v[0] * 127);
	  m_v[1] = (GLbyte) (v[1] * 127);
	  m_v[2] = (GLbyte) (v[2] * 127);
	  m_v[3] = 127;
     }
     Vector_4b(GLfloat *v) {
	  m_v[0] = (GLbyte) (v[0] * 127);
	  m_v[1] = (GLbyte) (v[1] * 127);
	  m_v[2] = (GLbyte) (v[2] * 127);
	  m_v[3] = 127;
     }
  
     GLbyte m_v[4];
};

class Vector_3ub {
public:
     Vector_3ub() {}

     Vector_3ub(vec3f &v) {
	  m_v[0] = (GLubyte) (v[0] * 255);
	  m_v[1] = (GLubyte) (v[1] * 255);
	  m_v[2] = (GLubyte) (v[2] * 255);
     }
     Vector_3ub(GLfloat *v) {
	  m_v[0] = (GLubyte) (v[0] * 255);
	  m_v[1] = (GLubyte) (v[1] * 255);
	  m_v[2] = (GLubyte) (v[2] * 255);
     }
     
     GLubyte m_v[3];
};

class Vector_4ub {
public:
     Vector_4ub() {}

     Vector_4ub(vec3f &v) {
	  m_v[0] = (GLubyte) (v[0] * 255);
	  m_v[1] = (GLubyte) (v[1] * 255);
	  m_v[2] = (GLubyte) (v[2] * 255);
	  m_v[3] = 255;
     }
     Vector_4ub(GLfloat *v) {
	  m_v[0] = (GLubyte) (v[0] * 255);
	  m_v[1] = (GLubyte) (v[1] * 255);
	  m_v[2] = (GLubyte) (v[2] * 255);
	  m_v[3] = 255;
     }
     
     GLubyte m_v[4];
};

class Vector_4f {
public:
     Vector_4f() {}

     Vector_4f(vec3f &v) {
	  m_v[0] = v[0];
	  m_v[1] = v[1];
	  m_v[2] = v[2];
	  m_v[3] = 1;
     }
     Vector_4f(GLfloat *v) {
	  m_v[0] = v[0];
	  m_v[1] = v[1];
	  m_v[2] = v[2];
	  m_v[3] = 1;
     }
     
     GLfloat m_v[4];
};

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define Point Point_3f

// Normals

#if 1
#define Normal Point_3f
#define NORMAL_GL_STRIDE 0
#define NORMAL_GL_TYPE GL_FLOAT
#endif

#if 0
#define Normal Vector_4b
#define NORMAL_GL_STRIDE 4
#define NORMAL_GL_TYPE GL_BYTE
#endif

#if 0
#define Normal Vector_3b
#define NORMAL_GL_STRIDE 0
#define NORMAL_GL_TYPE GL_BYTE
#endif

// Colors

#if 1
#define Color Vector_4ub
#define COLOR_GL_SIZE 4
#define COLOR_GL_TYPE GL_UNSIGNED_BYTE
#endif

#if 0
#define Color Vector_4f
#define COLOR_GL_SIZE 4
#define COLOR_GL_TYPE GL_FLOAT
#endif

#if 0
#define Color Vector_3ub
#define COLOR_GL_SIZE 3
#define COLOR_GL_TYPE GL_UNSIGNED_BYTE
#endif

#if 0
#define Color Vector_3f
#define COLOR_GL_SIZE 3
#define COLOR_GL_TYPE GL_FLOAT
#endif

#define Index unsigned int

/////////////////////////////////////

class VBO {
public:
     VBO(GLenum mode,
	 std::vector<Point_3f> *v,
	 std::vector<Normal> *n,
	 std::vector<Color> *c,
	 std::vector<Index> *i);
     
     ~VBO();

     void update(GLenum mode,
		 std::vector<Point_3f> *v,
		 std::vector<Normal> *n,
		 std::vector<Color> *c,
		 std::vector<Index> *i);
     
     void render();

private:
     static int total_buffer_size;

     GLuint m_position_vbo_id;
     GLuint m_normal_vbo_id;
     GLuint m_color_vbo_id;
     GLuint m_ebo_id;
     GLenum m_draw_mode;
     static bool m_interleave;

     std::vector<Point_3f> *m_vertices;
     std::vector<Normal> *m_normals;
     std::vector<Color> *m_colors;
     std::vector<Index> *m_indices;

     GLint free_vbo(GLuint vbo) {
	  GLint size = 0;
	  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
	  glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &size);
	  glDeleteBuffersARB(1, &vbo);
	  return size;
     }

     GLint free_ebo(GLuint ebo) {
	  GLint size = 0;
	  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ebo);
	  glGetBufferParameterivARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &size);
	  glDeleteBuffersARB(1, &ebo);
	  return size;
     }

     GLint update_vbo(GLuint *vbo, GLsizei vbo_size, const GLvoid *vbo_data) {
	  if (*vbo == 0) glGenBuffersARB(1, vbo);
	  else fprintf(stderr, "WARNING: updating static VBO\n");
	 
	  glBindBufferARB(GL_ARRAY_BUFFER_ARB, *vbo);
	  glBufferDataARB(GL_ARRAY_BUFFER_ARB, vbo_size, vbo_data, GL_STATIC_DRAW_ARB);
	  return vbo_size;
     }

     GLint update_ebo(GLuint *ebo, GLsizei ebo_size, const GLvoid *ebo_data) {
	  if (*ebo == 0) glGenBuffersARB(1, ebo);
	  else fprintf(stderr, "WARNING: updating static VBO\n");
	  
	  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, *ebo);
	  glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ebo_size, ebo_data, GL_STATIC_DRAW_ARB);
	  return ebo_size;
     }
};

inline
VBO::VBO(GLenum mode,
	 std::vector<Point_3f> *v,
	 std::vector<Normal> *n,
	 std::vector<Color> *c,
	 std::vector<Index> *i) 
{
     m_position_vbo_id = 0;
     m_normal_vbo_id = 0;
     m_color_vbo_id = 0;
     m_ebo_id = 0;
     update(mode, v, n, c, i);
}

inline
VBO::~VBO()
{
     total_buffer_size -= free_vbo(m_position_vbo_id);
     delete m_vertices;

     if (m_normal_vbo_id) {
	  total_buffer_size -= free_vbo(m_normal_vbo_id);
	  delete m_normals;
     }

     if (m_color_vbo_id) {
	  total_buffer_size -= free_vbo(m_color_vbo_id);
	  delete m_colors;
     }

     total_buffer_size -= free_ebo(m_ebo_id);
     delete m_indices;
}
