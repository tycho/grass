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

#include "vbo.hpp"
#include "main.h"
#include <assert.h>

int VBO::total_buffer_size;

void VBO::update(GLenum mode,
		 std::vector<Point_3f> *v,
		 std::vector<Normal> *n,
		 std::vector<Color> *c,
		 std::vector<Index> *i)
{
     m_draw_mode = mode;

     // Setup VBOs

     m_vertices = new std::vector<Point_3f>(v->size());
     *m_vertices = *v;
     total_buffer_size += update_vbo(&m_position_vbo_id, v->size()*sizeof(Point_3f), &(*v)[0]);
     
     if (n != NULL && n->size() > 0) {
	  m_normals = new std::vector<Normal>(n->size());
	  *m_normals = *n;
	  total_buffer_size += update_vbo(&m_normal_vbo_id, n->size()*sizeof(Normal), &(*n)[0]);
     } else {
	  m_normals = NULL;
     }

     if (c != NULL && c->size() > 0) {
	  m_colors = new std::vector<Color>(c->size());
	  *m_colors = *c;
	  total_buffer_size += update_vbo(&m_color_vbo_id, c->size()*sizeof(Color), &(*c)[0]);
     } else {
	  m_colors = NULL;
     }

     // Setup EBO

     m_indices = new std::vector<Index>(i->size());
     *m_indices = *i;

     GLsizei ebo_size = m_indices->size() * sizeof(Index);
     const GLvoid *ebo_data = &(*m_indices)[0];
     total_buffer_size += update_ebo(&m_ebo_id, ebo_size, ebo_data);

     static bool init = false;
     if (!init) {
	  printf("VBO Size: %d KB (%d vertices)\n", v->size()*sizeof(Point_3f)/1024, v->size());
	  init = true;
     }

     //fprintf(stderr, "%d MB\n", total_buffer_size / (1024*1024));
}

void VBO::render()
{
     glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

     glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_position_vbo_id);
     glVertexPointer(3, GL_FLOAT, 0, NULL);
     glEnableClientState(GL_VERTEX_ARRAY);

     if (m_normals && m_normals->size() > 0) {
	  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_normal_vbo_id);
	  glNormalPointer(NORMAL_GL_TYPE, NORMAL_GL_STRIDE, NULL);
	  glEnableClientState(GL_NORMAL_ARRAY);
     } else {
	  glDisableClientState(GL_NORMAL_ARRAY);
     }
    
     if (m_colors && m_colors->size() > 0) {
	  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_color_vbo_id);
	  glColorPointer(COLOR_GL_SIZE, COLOR_GL_TYPE, 0, NULL);
	  glEnableClientState(GL_COLOR_ARRAY);
     } else {
	  glDisableClientState(GL_COLOR_ARRAY);
     }

     glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ebo_id);
     glDrawElements(m_draw_mode, m_indices->size(), GL_UNSIGNED_INT, NULL);

     glPopClientAttrib();
}
