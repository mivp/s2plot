/* s2winglut.c
 *
 * Copyright 2006-2012 David G. Barnes, Paul Bourke, Christopher Fluke
 *
 * This file is part of S2PLOT.
 *
 * S2PLOT is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * S2PLOT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with S2PLOT.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * We would appreciate it if research outcomes using S2PLOT would
 * provide the following acknowledgement:
 *
 * "Three-dimensional visualisation was conducted with the S2PLOT
 * progamming library"
 *
 * and a reference to
 *
 * D.G.Barnes, C.J.Fluke, P.D.Bourke & O.T.Parry, 2006, Publications
 * of the Astronomical Society of Australia, 23(2), 82-93.
 *
 * $Id: s2winglut.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#include "s2win.h"
#include "s2const.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(S2LINUX)
#include <GL/glut.h>
#elif defined(S2DARWIN)
#include <GLUT/glut.h>
#endif

#if defined(S2MPICH)
#include <mpi.h>
#endif

void s2winInit(int *argc, char **argv) {
  glutInit(argc, argv);
glutInitWindowPosition(0, 0);
}

void s2winMainLoop(void) {
  glutMainLoop();
}

void s2winInitWindowSize(int w, int h) {
  glutInitWindowSize(w, h);
}

void s2winCreateWindow(char *title) {
  glutCreateWindow(title);
}

void s2winFullScreen(void) {
  glutFullScreen();
}

void s2winReshapeFunc(void (*func)(int width, int height)) {
  glutReshapeFunc(func);
}

void s2winDisplayFunc(void (*func)(void)) {
  glutDisplayFunc(func);
}

void s2winVisibilityFunc(void (*func)(int state)) {
  glutVisibilityFunc(func);
}

void s2winKeyboardFunc(void (*func)(unsigned char key, 
				    int x, int y)) {
  glutKeyboardFunc(func);
}

void s2winIdleFunc(void (*func)(void)) {
  glutIdleFunc(func);
}

void s2winSpecialFunc(void (*func)(int key, int x, int y)) {
  glutSpecialFunc(func);
}

void s2winMouseFunc(void (*func)(int button, int state, 
				 int x, int y)) {
  glutMouseFunc(func);
}

void s2winMotionFunc(void (*func)(int x, int y)) {
  glutMotionFunc(func);
}

void s2winPassiveMotionFunc(void (*func)(int x, int y)) {
  glutPassiveMotionFunc(func);
}

void s2winSpaceballButtonFunc(void (*func)(int button, int state)) {
  glutSpaceballButtonFunc(func);
}
void s2winSpaceballMotionFunc(void (*func)(int x, int y, int z)) {
  glutSpaceballMotionFunc(func);
}
void s2winSpaceballRotateFunc(void (*func)(int x, int y, int z)) {
  glutSpaceballRotateFunc(func);
}

void s2winSetCursor(int cursor) {
  switch(cursor) {
  case S2_CURSOR_CROSSHAIR:
    glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    break;
  case S2_CURSOR_NONE:
    glutSetCursor(GLUT_CURSOR_NONE);
    break;
  default:
    // do nothing
    break;
  }
}

int s2winGet(int which) {
  switch(which) {
  case S2_SCREEN_WIDTH:
    return glutGet(GLUT_SCREEN_WIDTH);
    break;
  case S2_SCREEN_HEIGHT:
    return glutGet(GLUT_SCREEN_HEIGHT);
    break;
  case S2_WINDOW_Y:
    return glutGet(GLUT_WINDOW_Y);
    break;
  case S2_WINDOW_WIDTH:
    return glutGet(GLUT_WINDOW_WIDTH);
    break;
  case S2_WINDOW_HEIGHT:
    return glutGet(GLUT_WINDOW_HEIGHT);
    break;
  default:
    return 0;
    break;
  }
}

void s2winSwapBuffers(void) {
#if defined(S2MPICH)
  MPI_Barrier(MPI_COMM_WORLD);
#endif
  glutSwapBuffers();
}

void s2winPostRedisplay(void) {
  glutPostRedisplay();
}

int s2winGetModifiers(void) {
  // this ASSUMES GLUT_ACTIVE_* == S2_ACTIVE_* ... should fix with 
  // some mapping code in the future.
  return glutGetModifiers();
}

void s2winInitDisplayMode(int stereotype, int aticard) {

  if (stereotype == ACTIVESTEREO) {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | 
			GLUT_STEREO | GLUT_MULTISAMPLE); 
  } else if (stereotype == INTERSTEREO) {
#if defined(QUICKINTERSTEREO)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH |
			GLUT_STENCIL);
#else
    if (aticard) {
      // no multisample for ATI cards and INTERSTEREO
      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH |
			  GLUT_STENCIL);
    } else {
      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH |
			  GLUT_STENCIL | GLUT_MULTISAMPLE);
    }
#endif
  } else {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | 
			GLUT_MULTISAMPLE);
  }

}

void s2winDestroyWindow(void) {
  glutDestroyWindow(glutGetWindow());
}

void s2winBitmapCharacter(char *p) {
  glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *p);
}

