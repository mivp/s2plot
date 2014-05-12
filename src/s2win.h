/* s2win.h
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
 * $Id: s2win.h 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#ifndef S2WIN_H
#define S2WIN_H

/* direct mappings with glut */
void s2winInit(int *argc, char **argv);
void s2winMainLoop(void);
void s2winInitWindowSize(int w, int h);
void s2winCreateWindow(char *title);
void s2winFullScreen(void);
void s2winReshapeFunc(void (*func)(int width, int height));
void s2winDisplayFunc(void (*func)(void));
void s2winVisibilityFunc(void (*func)(int state));
void s2winKeyboardFunc(void (*func)(unsigned char key, int x, int y));
void s2winIdleFunc(void (*func)(void));
void s2winSpecialFunc(void (*func)(int key, int x, int y));
void s2winMouseFunc(void (*func)(int button, int state, int x, int y));
void s2winMotionFunc(void (*func)(int x, int y));
void s2winPassiveMotionFunc(void (*func)(int x, int y));
void s2winSpaceballButtonFunc(void (*func)(int button, int state));
void s2winSpaceballMotionFunc(void (*func)(int x, int y, int z));
void s2winSpaceballRotateFunc(void (*func)(int x, int y, int z));
void s2winSetCursor(int cursor);
int s2winGet(int which);
void s2winSwapBuffers(void);
void s2winPostRedisplay(void);
int s2winGetModifiers(void);

/* non-direct mappings with glut */
void s2winInitDisplayMode(int stereotype, int aticard);
void s2winDestroyWindow(void);
void s2winBitmapCharacter(char *);


#endif
