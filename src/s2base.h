/* s2base.h
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
 * $Id: s2base.h 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#ifndef S2BASE_H
#define S2BASE_H

#ifndef XYZSTRUCT
typedef struct {
#if defined(S2TRIPLEFLOAT)
  float x,y,z;
#else
  double x,y,z;
#endif
} XYZ;
#define XYZSTRUCT
#endif

#ifndef COLOURSTRUCT
typedef struct {
#if defined(S2TRIPLEFLOAT)
  float r,g,b,a;
#else
  double r,g,b;
#endif
} COLOUR;
#define COLOURSTRUCT
#endif

typedef struct {
  XYZ vp;           	/* View position            */
  XYZ vd;           	/* View direction vector    */
  XYZ vu;           	/* View up direction        */
  XYZ pr;		/* Point to rotate about    */
  double focallength;   /* Focal Length along vd    */
  XYZ focus;            /* Focal point - Derived    */
  double aperture;  	/* Camera VERTICAL aperture */
  double eyesep;	/* Eye separation	    */
  double speed;         /* Speed in vd direction    */
  double fishrotate;    /* Rotate fisheye camera    */
} CAMERA;


#endif
