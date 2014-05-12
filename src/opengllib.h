/* opengllib.h
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
 * $Id: opengllib.h 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#ifndef OPENGLLIB_H
#define OPENGLLIB_H

#if defined(__cplusplus) && !defined(S2_CPPBUILD)
extern "C" {
#endif

typedef struct {
	int id;
	char name[128];
} PANELITEM;
typedef struct {
	int id;
	int nitems;
	PANELITEM *items;
} PANEL;

int WindowDump(char *,int,int,int,int);
void ClearBuffers(int);
void ClearAllBuffers(void);
void CreateBox(XYZ,XYZ,int);
void CreateRegularGrid(XYZ,XYZ,int);
void CreatePolygon(XYZ *,XYZ *,COLOUR *,int);
void CreateSphere(XYZ,double,int,int,double,double,double,double);
void CreateSimpleSphere(XYZ,double,int,int);
void CreateSuperEllipse(double,double,int,int);
void EvalSuperEllipse(double,double,double,double,XYZ *);
void CreateEllipsoid(XYZ,double,int,double,double,double);
void CreateDisk(XYZ,XYZ,double,double,int,double,double);
void CreateCone(XYZ,XYZ,double,double,int,double,double);
void CreateTorus(XYZ,double,double,int,int,double,double,double,double,int);
void CreateStars(XYZ,double);
void CreateRandomStars(int);
BITMAP4 *ReadRawTexture(int,int,char *,COLOUR,int);
BITMAP4 *ReadPPMTexture(char *,COLOUR,int,int *,int *,int);
BITMAP4 *ReadRawTexture2(int,int,char *,int);
void DrawGLText(int,int,char *);
void CreateLabelVector(char *,XYZ,XYZ,XYZ,XYZ *,int *);
void Draw3DPlus(XYZ,double);
void DrawAxes(XYZ,XYZ,int,int);
void Create3DCursor(int,int,int,int,XYZ,XYZ,XYZ,double,double);

#if defined(__cplusplus) && !defined(S2_CPPBUILD)
} // extern "C" {
#endif

#endif /* OPENGLLIB_H */
