/* geomviewer.h
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
 * $Id: geomviewer.h 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// this MUST be before the s2glut.h include as it sets a define 
// that is examined when glext.h is implicitly included by gl.h.
#if defined(S2LINUX)
#define GL_GLEXT_PROTOTYPES
#endif

#include "s2opengl.h"
#include "s2win.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/file.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/times.h>

#include "s2base.h"
#include "s2const.h"
#include "paulslib.h"
#include "bitmaplib.h"
#include "opengllib.h"



// RO access to pmin, pmax for device drivers
XYZ _s2priv_pmin(void);
XYZ _s2priv_pmax(void);

// internal error reporting
void _s2debug(char *fn, char *messg, ...);
void _s2warn(char *fn, char *messg, ...);
void _s2error(char *fn, char *messg, ...);
void _s2warnk(char key, char *messg, ...);

/* Stereo.c */
void HandleDisplay(void);

void CreateOpenGL(void);
void MakeGeometry(int, int, int);
void MakeLighting(void);
void MakeMaterial(void);
void HandleReshape(int,int);
void HandleKeyboard(unsigned char key,int x, int y);
float getFramerate();
void _s2_process_key(unsigned char key,int x, int y, int modifiers);
void HandleSpecialKeyboard(int key,int x, int y);
void RotateCamera(double,double,double,int);
void TranslateCamera(double,double,int);
void HandleMouse(int,int,int,int);
void HandleMouseMotion(int,int);
void HandlePassiveMotion(int,int);

void HandleSpaceballButton(int, int);
void HandleSpaceballMotion(int, int, int);
void HandleSpaceballRotate(int, int, int);


void HandleVisibility(int vis);
void HandleIdle(void);
int  ReadGeometry(char *);
void CleanGeometry(void);
#if defined(BUILDING_S2PLOT)
void SaveGeomFile(char *);
#else
void SaveGeomFile(void);
#endif
int  ReadOFF(char *);
void DeleteGeometry(void);
int  AutoPilot(int,char *);


void ReadViewFile(char *);
#if !defined(BUILDING_VIEWER)
void SaveViewFile(CAMERA, char *);
#else
void SaveViewFile(CAMERA);
#endif
void AppendCameraPosition(void);
void CreateAxes(void);
void DrawExtras(void);
void CreateProjection(int);
void AddLine2Database(XYZ,XYZ,COLOUR,COLOUR,double);
void AddFace2Database(XYZ *,int,COLOUR,double,XYZ);
void AddMarker2Database(int,double,XYZ,COLOUR);
void CleanExit(void);

// misc.c
void CreateASphere(XYZ,double,int,int,int);
void CreateAPlanet(XYZ,double,int,int,int,float,XYZ,float);
void GiveUsage(char *);
int  ReadVector(FILE *,XYZ *);
int  ReadColour(FILE *,COLOUR *);


