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
#include <stdio.h>
#include <vector>

#include "glh/glh_linear.h"
using namespace glh;

#include "glm.c"

////////////////////////////////////////////////////

static GLfloat brown[] = { 0.18, 0.12, 0.0, 1.0 };

static GLfloat blade_colors[4][4] = {
     { 50.0/255.0, 130.0/255.0, 50.0/255.0, 1.0 },
     { 70.0/255.0, 140.0/255.0, 70.0/255.0, 1.0 },
     { 60.0/255.0, 110.0/255.0, 40.0/255.0, 1.0 },
     { 110.0/255.0, 150.0/255.0, 0.0/255.0, 1.0 },
};

#define FLOOR_SIZE 20.0

static GLuint g_grass_dl;
static GLuint g_floor_dl;
static int g_num_grass_lists;

#define BASE_SCALE 1.0/150
#define SCALE_OFFSET 0.2/150
#define X_Y_ROT_OFFSET 20.0

#define TRIS_PER_BLADE 5

#define T(x) (model->triangles[(x)])

static std::vector<VBO *> g_VBOs;
static std::vector<std::vector<Point> >g_batch_vertices;
static std::vector<std::vector<Normal> >g_batch_normals;
static std::vector<std::vector<Color> >g_batch_colors;
static std::vector<std::vector<Index> >g_batch_indices;

////////////////////////////////////////////////////

void clearGrass()
{
     g_batch_vertices.clear();
     g_batch_normals.clear();
     g_batch_colors.clear();
     g_batch_indices.clear();

     if (g_grass_dl) {
	  glDeleteLists(g_grass_dl, g_num_grass_lists);
     }
     
     for (unsigned i = 0; i < g_VBOs.size(); i++) {
	  delete g_VBOs[i];
     }
     g_VBOs.clear();
}

void makeGrass(unsigned &batch_vertex_count,
	       unsigned &batch_vertex_size,
	       unsigned &batch_data_size)
{
     //srand(0);

     float min_x = -FLOOR_SIZE;
     float max_x = FLOOR_SIZE;
     float min_z = -FLOOR_SIZE;
     float max_z = FLOOR_SIZE;

     // Load grass meshes

     GLMmodel* blades[4];
     blades[0] = glmReadOBJ("data/blade1_7.obj");
     blades[1] = glmReadOBJ("data/blade2_7.obj");
     blades[2] = glmReadOBJ("data/blade3_7.obj");
     blades[3] = glmReadOBJ("data/blade4_7.obj");

     for (int i=0; i < 4; i++) {
	  GLMmodel* model = blades[i];
	  glmUnitize(model);
	  glmFacetNormals(model);
	  glmSimpleVertexNormals(model);
     }

     // Reset global data
     clearGrass();
     batch_vertex_size = batch_data_size = 0;
     
     // Create batches

     if (method == DL) {
	  g_grass_dl = glGenLists(num_vertex_arrays);
	  g_num_grass_lists = num_vertex_arrays;
     }

     int num_blade_instances = num_total_tris / num_vertex_arrays / TRIS_PER_BLADE;

     for (int bi = 0; bi < num_vertex_arrays; bi++) {

	  int model_idx = bi % 4;
	  GLMmodel* model = blades[model_idx];

	  // Create a batch by cloning the blade

	  std::vector<Point> batch_vertices;
	  std::vector<Normal> batch_normals;
	  std::vector<Color> batch_colors;
	  std::vector<Index> batch_indices;

	  for (int i = 0; i < num_blade_instances; i++) {
	       
	       float scaleOffset = (float)rand()/RAND_MAX * SCALE_OFFSET;
	       float scale = BASE_SCALE + scaleOffset;
	       float xRotOffset = (float)rand()/RAND_MAX * X_Y_ROT_OFFSET - 90;
	       float yRotOffset = (float)rand()/RAND_MAX * X_Y_ROT_OFFSET;
	       float zRotOffset = (float)rand()/RAND_MAX * 360;
	       float x = ((float)rand()/RAND_MAX) * (max_x-min_x) + min_x;
	       float z = ((float)rand()/RAND_MAX) * (max_z-min_z) + min_z;

	       glPushMatrix();

	       glLoadIdentity();
	       glTranslatef(x, scale*70, z);
	       glRotatef(xRotOffset, 1, 0, 0);
	       glRotatef(yRotOffset, 0, 1, 0);
	       glRotatef(zRotOffset, 0, 0, 1);
	       glScalef(scale, scale, scale);

	       GLfloat M[16];
	       glGetFloatv(GL_MODELVIEW_MATRIX, M);
	       matrix4f mat(M);
	       mat.transpose();

	       unsigned init_num_vertices = batch_vertices.size();

	       // vertices
	       for (unsigned vi=1; vi <= model->numvertices; vi++) {
		    vec3f xv;
		    vec3f v(&model->vertices[3 * vi]);
		    mat.mult_matrix_vec(v, xv);
		    batch_vertices.push_back(Point(xv));
	       }

	       // vertex normals
	       for (unsigned ni=1; ni <= model->numvertices; ni++) {
		    vec3f xn;
		    vec3f n(&model->normals[3 * ni]);
		    // works because no non-uniform scaling
		    mat.mult_matrix_vec(n, xn);
		    xn.normalize();
		    batch_normals.push_back(Normal(xn));
	       }

	       // vertex colors
	       for (unsigned ci=1; ci <= model->numvertices; ci++) {
		    Color C(blade_colors[model_idx]);
		    batch_colors.push_back(C);
	       }

	       GLMgroup* group = model->groups;
	       while (group) {
		    for (unsigned ti = 0; ti < group->numtriangles; ti++) {
			 GLMtriangle* triangle = &T(group->triangles[ti]);
			 batch_indices.push_back(init_num_vertices + triangle->vindices[0] - 1);
			      batch_indices.push_back(init_num_vertices + triangle->vindices[1] - 1);
			      batch_indices.push_back(init_num_vertices + triangle->vindices[2] - 1);
		    }
		    group = group->next;
	       }
	       
	       glPopMatrix();
	  }

//	  printf("batched vertices: %d\n", batch_vertices.size());
//	  printf("batched indices: %d\n", batch_indices.size());

	  // 3. Load batch into OpenGL

	  if (method == DL) {

	       glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
	  
	       glNewList(g_grass_dl + bi, GL_COMPILE);
	  
	       if (!load_colors) {
		    // uniform color per batch
		    glColor4fv(blade_colors[model_idx]);
	       }

	       glVertexPointer(3, GL_FLOAT, 0, &batch_vertices[0]);
	       glEnableClientState(GL_VERTEX_ARRAY);

	       if (load_normals) {
		    glNormalPointer(NORMAL_GL_TYPE, NORMAL_GL_STRIDE, &batch_normals[0]);
		    glEnableClientState(GL_NORMAL_ARRAY);
	       }

	       if (load_colors) {
		    glColorPointer(COLOR_GL_SIZE, COLOR_GL_TYPE, 0, &batch_colors[0]);
		    glEnableClientState(GL_COLOR_ARRAY);
	       }

	       for (int ti=0; ti < num_texcoords; ti++) {
		    glClientActiveTextureARB(GL_TEXTURE0 + ti);
		    glTexCoordPointer(2, GL_FLOAT, sizeof(Point), &batch_vertices[0]);
		    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	       }

	       glDrawElements(GL_TRIANGLES, batch_indices.size(),
			      GL_UNSIGNED_INT, &batch_indices[0]);

	       glEndList();

	       glPopClientAttrib();

	  } else if (method == SEP_VBO) {

	       std::vector<Normal> *n = load_normals ? &batch_normals : NULL;
	       std::vector<Color> *c = load_colors ? &batch_colors : NULL;
	       GLenum mode = GL_TRIANGLES;
	       VBO* vbo = new VBO(mode, &batch_vertices, n, c, &batch_indices);
	       g_VBOs.push_back(vbo);

	  } else {
	       
	       g_batch_vertices.push_back(batch_vertices);
	       g_batch_normals.push_back(batch_normals);
	       g_batch_colors.push_back(batch_colors);
	       g_batch_indices.push_back(batch_indices);
	  }

	  if (batch_data_size == 0) {
	       batch_data_size += batch_vertices.size() * sizeof(batch_vertices[0]);

	       if (load_normals) 
		    batch_data_size += batch_normals.size() * sizeof(batch_normals[0]);

	       if (load_colors)
		    batch_data_size += batch_colors.size() * sizeof(batch_colors[0]);

	       batch_vertex_size = batch_data_size;

	       batch_data_size += batch_indices.size() * sizeof(Index);

	       batch_vertex_count = batch_vertices.size();
	  }
     }
     
     for (int i=0; i < 4; i++) {
	  glmDelete(blades[i]);
     }
}

void renderGrass()
{
     glCallList(g_floor_dl);

     if (method == DL) {

	  for (int i = 0; i < num_vertex_arrays; i++) {
	       glCallList(g_grass_dl + i);
	  }

     } else if (method == SEP_VBO) {

	  for (unsigned i = 0; i < g_VBOs.size(); i++) {
	       int model_idx = i % 4;
	       if (!load_colors) {
		    // uniform color per batch
		    glColor4fv(blade_colors[model_idx]);
	       }
	       g_VBOs[i]->render();
	  }
     }

     else if (method == VA) {

	  for (unsigned i = 0; i < g_batch_vertices.size(); i++) {
	       int model_idx = i % 4;
	       if (!load_colors) {
		    glColor4fv(blade_colors[model_idx]);
	       }

	       glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

	       glVertexPointer(3, GL_FLOAT, 0, &g_batch_vertices[i][0]);
	       glEnableClientState(GL_VERTEX_ARRAY);

	       if (load_normals && g_batch_normals[i].size() > 0) {
		    glNormalPointer(NORMAL_GL_TYPE, NORMAL_GL_STRIDE, &g_batch_normals[i][0]);
		    glEnableClientState(GL_NORMAL_ARRAY);
	       }
    
	       if (load_colors && g_batch_colors[i].size() > 0) {
		    glColorPointer(COLOR_GL_SIZE, COLOR_GL_TYPE, 0, &g_batch_colors[i][0]);
		    glEnableClientState(GL_COLOR_ARRAY);
	       }

	       for (int ti=0; ti < num_texcoords; ti++) {
		    glClientActiveTextureARB(GL_TEXTURE0 + ti);
		    glTexCoordPointer(2, GL_FLOAT, sizeof(Point), &g_batch_vertices[i][0]);
		    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	       }

	       GLenum mode = GL_TRIANGLES;
	       glDrawElements(mode, g_batch_indices[i].size(),
			      GL_UNSIGNED_INT, &g_batch_indices[i][0]);

	       glPopClientAttrib();
	  }
     }

     else if (method == IM) {

	  for (unsigned bi = 0; bi < g_batch_vertices.size(); bi++) {
	       int model_idx = bi % 4;
	       if (!load_colors) {
		    glColor4fv(blade_colors[model_idx]);
	       }
	       GLenum mode = GL_TRIANGLES;
	       glBegin(mode);
	       for (unsigned it = 0; it < g_batch_indices[bi].size(); it++) {
		    Index vi = g_batch_indices[bi][it];
#if 0
		    if (load_normals && g_batch_normals[bi].size() > 0) {
			 glNormal3fv(&g_batch_normals[bi][vi].m_v[0]);
		    }
		    if (load_colors && g_batch_colors[bi].size() > 0) {
			 glColor3ubv(&g_batch_colors[bi][vi].m_v[0]);
		    }
#endif
		    for (int ti=0; ti < num_texcoords; ti++) {
			 glClientActiveTextureARB(GL_TEXTURE0 + ti);
			 glTexCoord2fv(&g_batch_vertices[bi][vi].m_v[0]);
		    }
		    glVertex3fv(&g_batch_vertices[bi][vi].m_v[0]);
	       }
	       glEnd();
	  }
     }
}

void makeFloor()
{
     float half_side = FLOOR_SIZE;
     g_floor_dl = glGenLists(1);

     glNewList(g_floor_dl, GL_COMPILE);
     	
     glColor3fv(brown);

     glBegin(GL_TRIANGLE_FAN);
     glVertex3f(-half_side, 0, -half_side);
     glVertex3f( half_side, 0, -half_side);
     glVertex3f( half_side, 0,  half_side);
     glVertex3f(-half_side, 0,  half_side);
     glEnd();

     glEndList();
}
