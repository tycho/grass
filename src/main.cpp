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
#define GLUT_BUILDING_LIB
#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "main.h"
#include "math.h"
#include "scene.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <assert.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

int num_total_tris = 1000000;
int num_vertex_arrays = 4;
bool load_normals = false;
bool load_colors = false;
bool pre_tl_bench = false;
string method;
unsigned it_per_fps = 40;
float max_fps_rel_diff = 0.1;

int num_texcoords = 0;
static GLint max_texcoords;

static unsigned batch_vertex_count = 0;
static unsigned batch_data_size = 0;
static unsigned batch_vertex_size = 0;
static float frame_rate;

////////////////////////////////////////////////////
// Camera

float eye[3];
float lookat[3];
float up[3];

float eye0[] = { 0, 4, 0 };
float lookat0[] = { 0, 0, 5 };
float up0[] = { 0, 1, 0 };

GLdouble fovy = 60.0;
GLdouble aspect;
GLdouble zNear = 1.0;
GLdouble zFar = 10000.0;

////////////////////////////////////////////////////
// GUI

int window_width = 800;
int window_height = 600;
int main_window;

////////////////////////////////////////////////////
// GLUT Callbacks

void myGlutIdle(void)
{
     // make sure the main window is active
     if (glutGetWindow() != main_window)
	  glutSetWindow(main_window);

     // tell glut to call the display callback next iteration
     glutPostRedisplay();
}

void myGlutKeyboard(unsigned char key, int x, int y)
{
     switch(key)
     {
     case 27: 
     case 'q':
     case 'Q':
	  exit(0);
	  break;
     }

     glutPostRedisplay();
}

void myGlutReshape(int	x, int y)
{
     int viewport_tx, viewport_ty,viewport_tw, viewport_th;

     // update viewport
     viewport_tx = viewport_ty = 0;
     viewport_tw = x;
     viewport_th = y;
     glViewport(viewport_tx, viewport_ty, viewport_tw, viewport_th);

     // projection transform
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     aspect = (GLdouble)viewport_tw / viewport_th;
     gluPerspective(fovy, aspect, zNear, zFar);

     // return to the modelview stack
     glMatrixMode(GL_MODELVIEW);

     glutPostRedisplay();
}

static float sum_fps;
static unsigned n_fps;

static float fps0, fps1;
static int n0, n1, c1;

inline
bool sameFps(float fps0, float fps1)
{
     float r = fabs(fps1 - fps0)/fps0;
     //printf("%.2f\n", r);
     return r < max_fps_rel_diff;
}

void nextBench()
{
     if (n0 == 0) {
	  n0 = num_vertex_arrays;
	  fps0 = frame_rate;
	  num_vertex_arrays = num_total_tris/2000;
     }
     else if (n1 == 0) {
	  n1 = num_vertex_arrays;
	  fps1 = frame_rate;
	  assert(fps1 > fps0);
	  //assert(!sameFps(fps1, fps0));
	  num_vertex_arrays = (n0 + n1) / 2;
     } else if (n1 - n0 > 1) {
	  if (sameFps(frame_rate, fps0)) {
	       n0 = num_vertex_arrays;
	       fps0 = frame_rate;
	  } else {
	       c1 = batch_vertex_count;
	       n1 = num_vertex_arrays;
	       fps1 = frame_rate;
	  }
	  num_vertex_arrays = (n0 + n1) / 2;
     } else {
	  printf("\noptimal batch size:\n");
	  printf("vertex count:           %d\n", c1);
	  //printf("vertex data:            %d KB\n", batch_vertex_size/1024);
	  //printf("vertex+element size:    %d KB\n", batch_data_size/1024);
	  exit(0);
     }

     //printf("n0=%d %.2f; n1=%d %.2f\n", n0, fps0, n1, fps1);
     //printf("n=%d\n", num_vertex_arrays);

     makeGrass(batch_vertex_count, batch_vertex_size, batch_data_size);
}

// draw the scene
void myGlutDisplay()
{
#ifndef WIN32
     struct timeval tv1, tv2, diff;
     gettimeofday(&tv1, NULL);
#else
     DWORD tv1 = GetTickCount();
#endif

     glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     gluLookAt(eye[0], eye[1], eye[2], lookat[0], lookat[1], lookat[2], 0, 1, 0);

     renderGrass();

     glFinish();
     glutSwapBuffers();

#ifndef WIN32
     gettimeofday(&tv2, NULL);
     timersub(&tv2, &tv1, &diff);
     float Dt = (float)diff.tv_sec + (float)diff.tv_usec / (1000*1000);
     sum_fps += 1.0/Dt;
     n_fps++;
#else
     DWORD tv2 = GetTickCount();
     DWORD diff = tv2 - tv1;
     if (diff != 0) {
	  float Dt = (float) diff / 1000;
	  sum_fps += 1.0/Dt;
	  n_fps++;
     }
#endif

     if (n_fps == it_per_fps) {
	  frame_rate = sum_fps/n_fps;
	  sum_fps = 0;
	  n_fps = 0;
	  fprintf(stderr, "%d %.1f\n", num_vertex_arrays, frame_rate);
	  //fprintf(stderr, "%d %.2f\n", batch_vertex_count, frame_rate);
	  if (pre_tl_bench) {
	       nextBench();
	  } else {
	       exit(0);
	  }
     }
}

void initCamera()
{
     for (unsigned i=0; i<3; i++)
	  eye[i] = eye0[i];
     for (unsigned i=0; i<3; i++)
	  lookat[i] = lookat0[i];
     for (unsigned i=0; i<3; i++)
	  up[i] = up0[i];
}

#define VERSION "1.2 (November 3, 2005)"

void usage(char **argv)
{
     printf("%s -method (IM|VA|DL|VBO) [OPTIONS]\n", argv[0]);
     printf("\t %s = glBegin/glEnd\n", IM);
     printf("\t %s = vertex arrays\n", VA);
     printf("\t %s = display lists of vertex array\n", DL);
     printf("\t %s = separate VBOs\n", SEP_VBO);
     printf("\nOptions:\n");
     printf("-tris    \t total_num_tris  (default=%d)\n", num_total_tris);
     printf("-batches \t num_batches (default=%d)\n", num_vertex_arrays);
     printf("-it      \t num_iterations_per_fps (default=%d)\n", it_per_fps);
     printf("-normals \t load vertex normals (default=off)\n");
     printf("-colors \t load vertex colors (default=off)\n");
     printf("-texcoords \t number of texture coordinates (default=%d, max=%d)\n",
	    num_texcoords, max_texcoords);
     printf("-pre-tl \t find optimal vertex count of pre-T&L cache (default=off)\n");
     printf("-pre-tl-jump \t relative difference to find fps jumps (default=%.2f)\n",
	    max_fps_rel_diff );
     exit(0);
}

void error(const char *desc)
{
     fprintf(stdout, "Error: %s\n", desc);
     exit(1);
}

int main(int argc,	char* argv[])
{
     printf("-----------------------------------------------------------\n");
     printf("Grass Benchmark %s\n", VERSION);
     printf("Louis Bavoil <bavoil@cs.utah.edu>\n");
     printf("-----------------------------------------------------------\n");

     //
     // initialize GLUT
     //
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
     glutInitWindowSize(window_width, window_height);
     glutInitWindowPosition(100, 100);
     main_window = glutCreateWindow("Grass Benchmark");
     glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB, &max_texcoords);

     //
     // initialize GLEW
     //
     GLenum err = glewInit();
     if (GLEW_OK != err) {
	  error((char *)glewGetErrorString(err));
     }
     if (!GL_ARB_vertex_buffer_object) {
	  error("Error: VBO extension not supported\n");
     }

     //
     // parse the command line
     //
     for (int i = 1; i < argc; i++) {
	  if (argv[i][0] == '-') {
	       if (strcmp(argv[i], "-tris") == 0) {
		    num_total_tris = atoi(argv[++i]);
	       }
	       else if (strcmp(argv[i], "-batches") == 0) {
		    num_vertex_arrays = atoi(argv[++i]);
	       }
	       else if (strcmp(argv[i], "-normals") == 0) {
		    load_normals = true;
	       }
	       else if (strcmp(argv[i], "-colors") == 0) {
		    load_colors = true;
	       }
	       else if (strcmp(argv[i], "-texcoords") == 0) {
		    num_texcoords = atoi(argv[++i]);
	       }
	       else if (strcmp(argv[i], "-method") == 0) {
		    method = string(argv[++i]);
	       }
	       else if (strcmp(argv[i], "-it") == 0) {
		    it_per_fps = atoi(argv[++i]);
	       }
	       else if (strcmp(argv[i], "-pre-tl") == 0) {
		    pre_tl_bench = true;
	       }
	       else if (strcmp(argv[i], "-pre-tl-jump") == 0) {
		    max_fps_rel_diff = atof(argv[++i]);
	       }
	       else if (strcmp(argv[i], "-h") == 0) {
		    usage(argv);
	       }
	  }
     }

     if (method.length() == 0) {
	  usage(argv);
     }

     if (method == SEP_VBO && num_texcoords > 0) {
	  error("Error: texcoords not implemented for VBOs\n");
     }

     if (num_texcoords > max_texcoords) {
	  printf("GL_MAX_TEXTURE_COORDS = %d\n", max_texcoords);
	  error("Error: num texcoords > GL_MAX_TEXTURE_COORDS_ARB\n");
     }

     printf("# num grass triangles: %d\n", num_total_tris);
     printf("# rendering method: %s\n", 
	    method == IM ? "IM" :
	    method == VA ? "VA" :
	    method == DL ? "DL" :
	    method == SEP_VBO ? "VBO" :
	    "unknown");
     printf("# num batches: %d\n", num_vertex_arrays);
     printf("# iterations per fps: %d\n", it_per_fps);
     printf("# normals: %s\n", load_normals ? "on" : "off");
     printf("# colors: %s\n", load_colors ? "on" : "off");
     printf("# texcoords: %d\n", num_texcoords);
     printf("# pre-tl: %s\n", pre_tl_bench ? "on" : "off");
     if (pre_tl_bench) {
	  printf("# fps jump: %.1f%%\n", max_fps_rel_diff*100);
     }
     printf("\n");

     // initialize the camera with default view
     initCamera();

     //
     // set GLUT callbacks
     //
     glutDisplayFunc(myGlutDisplay);
     glutReshapeFunc(myGlutReshape);
     glutIdleFunc(myGlutIdle);
     glutKeyboardFunc(myGlutKeyboard);

     //
     // initialize OpenGL
     //
     glEnable(GL_DEPTH_TEST);
     glClearColor(0.0, 0.0, 0.0, 1.0);
     makeGrass(batch_vertex_count, batch_vertex_size, batch_data_size);
     makeFloor();

     // give control over to glut
     glutMainLoop();

     return 0;
}
