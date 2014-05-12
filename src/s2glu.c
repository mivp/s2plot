/* s2glu.h
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
 * $Id: s2glu.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#if defined(S2CLOSED)
#error WRONG VERSION OF s2glu.c BEING COMPILED!
#endif

#if defined(S2DARWIN)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

void s2Perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, 
		    GLdouble zFar) {
  gluPerspective(fovy, aspect, zNear, zFar);
}

void s2LookAt(float eyeX, float eyeY, float eyeZ, 
	      float lookAtX, float lookAtY, float lookAtZ, 
	      float upX, float upY, float upZ) {
  gluLookAt(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ,
	    upX, upY, upZ);
}

GLint s2Project(GLdouble objx, GLdouble objy, GLdouble objz,
		const GLdouble model[16], const GLdouble proj[16],
		const GLint viewport[4],
		GLdouble * winx, GLdouble * winy, GLdouble * winz) {
  return gluProject(objx, objy, objz, 
		    model, proj, viewport,
		    winx, winy, winz);
}

GLint s2UnProject(GLdouble winx, GLdouble winy, GLdouble winz,
		  const GLdouble model[16], const GLdouble proj[16],
		  const GLint viewport[4],
		  GLdouble * objx, GLdouble * objy, GLdouble * objz) {
  return gluUnProject(winx, winy, winz, 
		      model, proj, viewport,
		      objx, objy, objz);
}

