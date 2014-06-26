/* s2fishdome.c: S2PLOT fisheye and warped fisheye (dome) code
 *
 * Copyright 2006-2014 David G. Barnes, Paul Bourke, Christopher Fluke
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
 */

#include "s2opengl.h"
#include "s2types.h"
#include "s2win.h"

void DrawExtras(void);
void _s2debug(char *fn, char *messg, ...);
void CleanExit(void);

void DomeTextures(void);
int  DomeCreate(void);
void DrawDome(int,int);
void DomeSplitFace(DOMEFACE *,int *);
void DomeFlatten(XYZ *,double);
int  ReadMesh(char *);
void MakeDefaultMesh(void);
void CalcWarpData(void);
void EstimateWarp(double,double,double *,double *,double *);
void SetVertex(DOMEFACE);
int MirrorPosition(XYZ,double,XYZ,XYZ,XYZ *);

int ReadBinMesh(char *fname);
void s2Perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, 
		   GLdouble zFar);
void s2LookAt(float eyeX, float eyeY, float eyeZ, 
	      float lookAtX, float lookAtY, float lookAtZ, 
	      float upX, float upY, float upZ);
XYZ _s2priv_pmin(void);
XYZ _s2priv_pmax(void);

void MakeLighting(void);
void MakeMaterial(void);
void MakeGeometry(int, int);


/* All of these should probable become *local* variables and never
 * known about to S2PLOT.
 */
OPTIONS *_s2fd_options;

// Texture maps for the 4 walls
#define TEXTURESIZE (512)
BITMAP4 *texturetop = NULL,*texturebottom = NULL;
BITMAP4 *textureleft = NULL,*textureright = NULL;
GLuint walltextureid[4];

// Optional mapper
MESHNODE **mesh = NULL;
int meshtype=0,meshnx=0,meshny=0;

// Coordinate and texture coordinates for the 4 sides
int ndometop = 0, ndomebottom = 0;
int ndomeleft = 0, ndomeright = 0;
DOMEFACE *dometop = NULL,*domebottom = NULL;
DOMEFACE *domeleft = NULL,*domeright = NULL;

/*
	Reset the contents of the texture image buffers
	in preparation for them being copied into texture objects
*/
void DomeTextures(void) {
  BITMAP4 red = {255,0,0,255},blue = {0,0,255,255};
  BITMAP4 green = {0,255,0,255}, magenta = {255,0,255,255}; 
  
  if (texturetop != NULL)
    free(texturetop);
  texturetop = (BITMAP4 *)malloc(TEXTURESIZE*TEXTURESIZE*sizeof(BITMAP4));
  if (texturebottom != NULL)
    free(texturebottom);
  texturebottom = (BITMAP4 *)malloc(TEXTURESIZE*TEXTURESIZE*sizeof(BITMAP4));
  if (textureleft != NULL)
    free(textureleft);
  textureleft = (BITMAP4 *)malloc(TEXTURESIZE*TEXTURESIZE*sizeof(BITMAP4));
  if (textureright != NULL)
    free(textureright);
  textureright = (BITMAP4 *)malloc(TEXTURESIZE*TEXTURESIZE*sizeof(BITMAP4));
  if (texturetop == NULL || texturebottom == NULL || textureleft == NULL || textureright == NULL) { 
    fprintf(stderr,"Texture read failed\n");
    exit(-1);
  }
  
  /* Plain colours */
  Erase_Bitmap(texturetop,   TEXTURESIZE,TEXTURESIZE,blue);
  Erase_Bitmap(texturebottom,TEXTURESIZE,TEXTURESIZE,green);
  Erase_Bitmap(textureleft,  TEXTURESIZE,TEXTURESIZE,red);
  Erase_Bitmap(textureright, TEXTURESIZE,TEXTURESIZE,magenta);
  
}

/*
   Create a facet representation of the dome disk to a particular resolution
   Sets all the correct texture coordinates
   Deal with offset projection
   This is all done algorithmically rather than a final mathematical equation
   Dome disk is in the x-z plane at y=0
   The camera is lacated at (0,-1,0), see HandleDIsplay()
              | z
              |
              |
              |
              +--------- y
             /
            /
           / x
*/
int DomeCreate(void) {
  int i,j;
  double xmin = 1e32,zmin = 1e32,xmax = -1e32,zmax = -1e32;
  
  /* Top face - just a triangle */
  dometop = (DOMEFACE *)realloc(dometop,sizeof(DOMEFACE));
  dometop[0].p[0].x = -sqrt(2.0) / 2.0;
  dometop[0].p[0].y = 0;
  dometop[0].p[0].z = 0.5;
  dometop[0].u[0] = 0;
  dometop[0].v[0] = 1;
  dometop[0].p[1].x = 0;
  dometop[0].p[1].y = sqrt(2.0) / 2.0;
  dometop[0].p[1].z = 0.5;
  dometop[0].u[1] = 0;
  dometop[0].v[1] = 0;
  dometop[0].p[2].x = sqrt(2.0) / 2.0;
  dometop[0].p[2].y = 0;
  dometop[0].p[2].z = 0.5;
  dometop[0].u[2] = 1;
  dometop[0].v[2] = 0;
  ndometop = 1;
  
  /* Bottom face - just a triangle */
  domebottom = (DOMEFACE *)realloc(domebottom,sizeof(DOMEFACE));
  domebottom[0].p[0].x = -sqrt(2.0) / 2.0;
  domebottom[0].p[0].y = 0;
  domebottom[0].p[0].z = -0.5;
  domebottom[0].u[0] = 1;
  domebottom[0].v[0] = 0;
  domebottom[0].p[1].x = sqrt(2.0) / 2.0;
  domebottom[0].p[1].y = 0;
  domebottom[0].p[1].z = -0.5;
  domebottom[0].u[1] = 0;
  domebottom[0].v[1] = 1;
  domebottom[0].p[2].x = 0;
  domebottom[0].p[2].y = sqrt(2.0) / 2.0;
  domebottom[0].p[2].z = -0.5;
  domebottom[0].u[2] = 0;
  domebottom[0].v[2] = 0;
  ndomebottom = 1;
  
  /* Left face - two triangles */
  domeleft = (DOMEFACE *)realloc(domeleft,2*sizeof(DOMEFACE));
  domeleft[0].p[0].x = -sqrt(2.0) / 2.0;
  domeleft[0].p[0].y = 0;
  domeleft[0].p[0].z = -0.5;
  domeleft[0].u[0] = 0;
  domeleft[0].v[0] = 0;
  domeleft[0].p[1].x = 0;
  domeleft[0].p[1].y = sqrt(2.0) / 2.0;
  domeleft[0].p[1].z = -0.5;
  domeleft[0].u[1] = 1;
  domeleft[0].v[1] = 0;
  domeleft[0].p[2].x = -sqrt(2.0) / 2.0;
  domeleft[0].p[2].y = 0;
  domeleft[0].p[2].z = 0.5;
  domeleft[0].u[2] = 0;
  domeleft[0].v[2] = 1;
  domeleft[1].p[0].x = -sqrt(2.0) / 2.0;
  domeleft[1].p[0].y = 0;
  domeleft[1].p[0].z = 0.5;
  domeleft[1].u[0] = 0;
  domeleft[1].v[0] = 1;
  domeleft[1].p[1].x = 0;
  domeleft[1].p[1].y = sqrt(2.0) / 2.0;
  domeleft[1].p[1].z = -0.5;
  domeleft[1].u[1] = 1;
  domeleft[1].v[1] = 0;
  domeleft[1].p[2].x = 0;
  domeleft[1].p[2].y = sqrt(2.0) / 2.0;
  domeleft[1].p[2].z = 0.5;
  domeleft[1].u[2] = 1;
  domeleft[1].v[2] = 1;
  ndomeleft = 2;
  
  /* Right face - two triangles */
  domeright  = (DOMEFACE *)realloc(domeright,2*sizeof(DOMEFACE));
  domeright[0].p[0].x = 0;
  domeright[0].p[0].y = sqrt(2.0) / 2.0;
  domeright[0].p[0].z = -0.5;
  domeright[0].u[0] = 0;
  domeright[0].v[0] = 0;
  domeright[0].p[1].x = sqrt(2.0) / 2.0;
  domeright[0].p[1].y = 0;
  domeright[0].p[1].z = -0.5;
  domeright[0].u[1] = 1;
  domeright[0].v[1] = 0;
  domeright[0].p[2].x = sqrt(2.0) / 2.0;
  domeright[0].p[2].y = 0;
  domeright[0].p[2].z = 0.5;
  domeright[0].u[2] = 1;
  domeright[0].v[2] = 1;
  domeright[1].p[0].x = 0;
  domeright[1].p[0].y = sqrt(2.0) / 2.0;
  domeright[1].p[0].z = -0.5;
  domeright[1].u[0] = 0;
  domeright[1].v[0] = 0;
  domeright[1].p[1].x = sqrt(2.0) / 2.0;
  domeright[1].p[1].y = 0;
  domeright[1].p[1].z = 0.5;
  domeright[1].u[1] = 1;
  domeright[1].v[1] = 1;
  domeright[1].p[2].x = 0;
  domeright[1].p[2].y = sqrt(2.0) / 2.0;
  domeright[1].p[2].z = 0.5;
  domeright[1].u[2] = 0;
  domeright[1].v[2] = 1;
  ndomeright = 2;

  /* 
     Refine the faces to the desired resolution 
     Achieved by splitting each triangles into 4
     Preserve texture coordinates (of course)
  */
  for (i=0;i<5;i++) {
    dometop = (DOMEFACE *)realloc(dometop,(4*ndometop)*sizeof(DOMEFACE));
    DomeSplitFace(dometop,&ndometop);
    domebottom = (DOMEFACE *)realloc(domebottom,(4*ndomebottom)*sizeof(DOMEFACE));
    DomeSplitFace(domebottom,&ndomebottom);
    domeleft = (DOMEFACE *)realloc(domeleft,(4*ndomeleft)*sizeof(DOMEFACE));
    DomeSplitFace(domeleft,&ndomeleft);
    domeright = (DOMEFACE *)realloc(domeright,(4*ndomeright)*sizeof(DOMEFACE));
    DomeSplitFace(domeright,&ndomeright);
  }
  
  /* Turn into a hemisphere */
  for (j=0;j<3;j++) {
    for (i=0;i<ndometop;i++)
      Normalise(&(dometop[i].p[j]));
    for (i=0;i<ndomebottom;i++)
      Normalise(&(domebottom[i].p[j]));
    for (i=0;i<ndomeleft;i++)
      Normalise(&(domeleft[i].p[j]));
    for (i=0;i<ndomeright;i++)
      Normalise(&(domeright[i].p[j]));
  }
  
  
  /* Flatten onto xz plane */
  for (j=0;j<3;j++) {
    for (i=0;i<ndometop;i++) {
      DomeFlatten(&(dometop[i].p[j]),180.0);
      xmin = MIN(xmin,dometop[i].p[j].x);
      zmin = MIN(zmin,dometop[i].p[j].z);
      xmax = MAX(xmax,dometop[i].p[j].x);
      zmax = MAX(zmax,dometop[i].p[j].z);
    }
    for (i=0;i<ndomebottom;i++) {
      DomeFlatten(&(domebottom[i].p[j]),180.0);
      xmin = MIN(xmin,domebottom[i].p[j].x);
      zmin = MIN(zmin,domebottom[i].p[j].z);
      xmax = MAX(xmax,domebottom[i].p[j].x);
      zmax = MAX(zmax,domebottom[i].p[j].z);
    }
    for (i=0;i<ndomeleft;i++) {
      DomeFlatten(&(domeleft[i].p[j]),180.0);
      xmin = MIN(xmin,domeleft[i].p[j].x);
      zmin = MIN(zmin,domeleft[i].p[j].z);
      xmax = MAX(xmax,domeleft[i].p[j].x);
      zmax = MAX(zmax,domeleft[i].p[j].z);
    }  
    for (i=0;i<ndomeright;i++) {
      DomeFlatten(&(domeright[i].p[j]),180.0);
      xmin = MIN(xmin,domeright[i].p[j].x);
      zmin = MIN(zmin,domeright[i].p[j].z);
      xmax = MAX(xmax,domeright[i].p[j].x);
      zmax = MAX(zmax,domeright[i].p[j].z);
    }
  }
 
  // Fill in the warp map values
  CalcWarpData();
  
  return(TRUE);
}

/*
	Flatten a dome to a planar object
	It is the planar object that is finally rendered in orthographic mode
*/
void DomeFlatten(XYZ *f,double aperture)
{
	double phi,r;

	r   = atan2(sqrt(f->x*f->x + f->z*f->z),f->y) / (DTOR * aperture / 2);
	phi = atan2(f->z,f->x);
	
	f->x = r * cos(phi);
	f->y = 0;
	f->z = r * sin(phi);
}

/*
	Refine a triangular mesh by bisecting each edge
	Forms 3 new triangles for each existing triangle on each iteration
	Could be made more efficient for drawing if the triangles were
	ordered in a fan or strip!
*/
void DomeSplitFace(DOMEFACE *f,int *n)
{
	int i;
	int n1,n2;

	n1 = *n;
	n2 = *n;

	for (i=0;i<n1;i++) {

		f[n2].p[0] = MidPoint(f[i].p[0],f[i].p[1]);
      f[n2].p[1] = f[i].p[1];
      f[n2].p[2] = MidPoint(f[i].p[1],f[i].p[2]);
      f[n2].u[0] = LinearInterpolate(f[i].u[0],f[i].u[1],0.5);
      f[n2].u[1] = f[i].u[1];
      f[n2].u[2] = LinearInterpolate(f[i].u[1],f[i].u[2],0.5);
      f[n2].v[0] = LinearInterpolate(f[i].v[0],f[i].v[1],0.5);
      f[n2].v[1] = f[i].v[1];
      f[n2].v[2] = LinearInterpolate(f[i].v[1],f[i].v[2],0.5);

      f[n2+1].p[0] = MidPoint(f[i].p[1],f[i].p[2]);
      f[n2+1].p[1] = f[i].p[2];
      f[n2+1].p[2] = MidPoint(f[i].p[2],f[i].p[0]);
      f[n2+1].u[0] = LinearInterpolate(f[i].u[1],f[i].u[2],0.5);
      f[n2+1].u[1] = f[i].u[2];
      f[n2+1].u[2] = LinearInterpolate(f[i].u[2],f[i].u[0],0.5);
      f[n2+1].v[0] = LinearInterpolate(f[i].v[1],f[i].v[2],0.5);
      f[n2+1].v[1] = f[i].v[2];
      f[n2+1].v[2] = LinearInterpolate(f[i].v[2],f[i].v[0],0.5);

      f[n2+2].p[0] = MidPoint(f[i].p[0],f[i].p[1]);
      f[n2+2].p[1] = MidPoint(f[i].p[1],f[i].p[2]);
      f[n2+2].p[2] = MidPoint(f[i].p[2],f[i].p[0]);
      f[n2+2].u[0] = LinearInterpolate(f[i].u[0],f[i].u[1],0.5);
      f[n2+2].u[1] = LinearInterpolate(f[i].u[1],f[i].u[2],0.5);
      f[n2+2].u[2] = LinearInterpolate(f[i].u[2],f[i].u[0],0.5);
      f[n2+2].v[0] = LinearInterpolate(f[i].v[0],f[i].v[1],0.5);
      f[n2+2].v[1] = LinearInterpolate(f[i].v[1],f[i].v[2],0.5);
      f[n2+2].v[2] = LinearInterpolate(f[i].v[2],f[i].v[0],0.5);

      //f[i].p[0] = f[i].p[0];
      f[i].p[1] = MidPoint(f[i].p[0],f[i].p[1]);
      f[i].p[2] = MidPoint(f[i].p[0],f[i].p[2]);
      //f[i].u[0] = f[i].u[0];
      f[i].u[1] = LinearInterpolate(f[i].u[0],f[i].u[1],0.5);
      f[i].u[2] = LinearInterpolate(f[i].u[0],f[i].u[2],0.5);
      //f[i].v[0] = f[i].v[0];
      f[i].v[1] = LinearInterpolate(f[i].v[0],f[i].v[1],0.5);
      f[i].v[2] = LinearInterpolate(f[i].v[0],f[i].v[2],0.5);

		n2 += 3;
	}

	*n = n2;
}

/*
	Draw the dome
	Create display lists the first time through

	Note: top, bottom, left and right faces are named -101,-102,-103,-104
	resp. (dbarnes, 20060925 for handle selection)
*/
#define displaylist _s2f_displaylist
void DrawDome(int usetexture,int makelists) {
  int i;
  static int first = TRUE;
  static int displaylist[4] = {-1,-1,-1,-1};

  if (makelists)
    first = TRUE;
  
  glDisable(GL_DEPTH);
  
  /* Top face */
  if (usetexture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,walltextureid[0]);
  }
  if (first) {
    if (displaylist[0] > 0)
      glDeleteLists(displaylist[0],1);
    displaylist[0] = glGenLists(1);
    glNewList(displaylist[0],GL_COMPILE);
    //fprintf(stderr, "loading name -101\n");
    //glLoadName(-101);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0,1.0,0.0);
    fprintf(stderr, "ndometop = %d, ndometop[0].wp = %f %f %f\n",
	    ndometop, dometop[0].wp[0].x, dometop[0].wp[0].y, dometop[0].wp[0].z);
    for (i=0;i<ndometop;i++) 
      SetVertex(dometop[i]);
    glEnd();
    //glLoadName(-1);
    glEndList();
  } else {
    glCallList(displaylist[0]);
  }
  
  /* Bottom face */
  if (usetexture) 
    glBindTexture(GL_TEXTURE_2D,walltextureid[1]);
  if (first) {
    if (displaylist[1] > 0)
      glDeleteLists(displaylist[1],1);
    displaylist[1] = glGenLists(1);
    glNewList(displaylist[1],GL_COMPILE);     
    //glLoadName(-102);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0,1.0,0.0);
    for (i=0;i<ndomebottom;i++) 
      SetVertex(domebottom[i]);
    glEnd();
    //glLoadName(-1);
    glEndList();
  } else {
    glCallList(displaylist[1]);
  }
  
  /* Left face */
  if (usetexture) 
    glBindTexture(GL_TEXTURE_2D,walltextureid[2]);
  if (first) {
    if (displaylist[2] > 0)
      glDeleteLists(displaylist[2],1);
    displaylist[2] = glGenLists(1);
    glNewList(displaylist[2],GL_COMPILE);
    //glLoadName(-103);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0,1.0,0.0);
    for (i=0;i<ndomeleft;i++) 
      SetVertex(domeleft[i]);
    glEnd();
    //glLoadName(-1);
    glEndList();
  } else {
    glCallList(displaylist[2]);
  }
  
  /* Right face */
  if (usetexture) 
    glBindTexture(GL_TEXTURE_2D,walltextureid[3]);
  if (first) {
    if (displaylist[3] > 0)
      glDeleteLists(displaylist[3],1);
    displaylist[3] = glGenLists(1);
    glNewList(displaylist[3],GL_COMPILE);
    //glLoadName(-104);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0,1.0,0.0);
    for (i=0;i<ndomeright;i++) 
      SetVertex(domeright[i]);
    glEnd();
    //glLoadName(-1);
    glEndList();
  } else {
    glCallList(displaylist[3]);
  }
  
  if (usetexture)
    glDisable(GL_TEXTURE_2D);
  first = FALSE;
  if (makelists)
    return;
}

void SetVertex(DOMEFACE face)
{
   int j;
   XYZ p;

	// If all intensities are negative, dont draw the triangle
	if (_s2fd_options->dometype == WARPMAP || _s2fd_options->dometype == MIRROR1) {
   	if (face.wi[0] < 0 || face.wi[1] < 0 || face.wi[2] < 0)
   	   return;
	}

   for (j=0;j<3;j++) {
      glTexCoord2f(face.u[j],face.v[j]);
      if (_s2fd_options->dometype == WARPMAP || _s2fd_options->dometype == MIRROR1) {
         if (_s2fd_options->showdomeintensity)
            glColor3f(face.wi[j],face.wi[j],face.wi[j]);
         p = face.wp[j];
         glVertex3f(p.x,p.y,p.z);
      } else {
         p = face.p[j];
         glVertex3f(p.x,p.y,p.z);
      }
   }
}

/*
   Create a dummy warping mesh, one that should have no effect
*/
void MakeDefaultMesh(void)
{
   int i,j;
   
   // Type and size
   meshtype = RECTANGULAR;
   meshnx = 1000;
   meshny = 1000;
   
   // Memory
   mesh = (MESHNODE **)malloc(meshnx*sizeof(MESHNODE *));
   for (i=0;i<meshnx;i++)
     mesh[i] = (MESHNODE *)malloc(meshny*sizeof(MESHNODE));
   
   // Contents
   for (j=0;j<meshny;j++) {
      for (i=0;i<meshnx;i++) {
         mesh[i][j].u = i / (double)(meshnx-1);
         mesh[i][j].v = j / (double)(meshny-1);
         mesh[i][j].x = 2 * i / (double)(meshnx-1) - 1;
         mesh[i][j].y = 2 * j / (double)(meshny-1) - 1;
         mesh[i][j].i = 1;
      }
   }
}

/*
   Create the additional warp mapping
   Fill in warp data in dometop, domebottom, domeleft, domeright
   Plane lies in the x-z plane
*/
void CalcWarpData(void) {      
  int i,j;
  double x,z,b;
  
  for (i=0;i<ndometop;i++) {
    for (j=0;j<3;j++) {
      EstimateWarp(dometop[i].p[j].x,dometop[i].p[j].z,&x,&z,&b);
      dometop[i].wp[j].x = x;
      dometop[i].wp[j].y = 0;
      dometop[i].wp[j].z = z;
      dometop[i].wi[j] = b;
    }
  }
  
  for (i=0;i<ndomebottom;i++) {
    for (j=0;j<3;j++) {
      EstimateWarp(domebottom[i].p[j].x,domebottom[i].p[j].z,&x,&z,&b);
      domebottom[i].wp[j].x = x;
      domebottom[i].wp[j].y = 0;
      domebottom[i].wp[j].z = z;
      domebottom[i].wi[j] = b;
    }
  }
  
  for (i=0;i<ndomeleft;i++) {
    for (j=0;j<3;j++) {
      EstimateWarp(domeleft[i].p[j].x,domeleft[i].p[j].z,&x,&z,&b);
      domeleft[i].wp[j].x = x;
      domeleft[i].wp[j].y = 0;
      domeleft[i].wp[j].z = z;
      domeleft[i].wi[j] = b;
    }
  }
  
  for (i=0;i<ndomeright;i++) {
    for (j=0;j<3;j++) {
      EstimateWarp(domeright[i].p[j].x,domeright[i].p[j].z,&x,&z,&b);
      domeright[i].wp[j].x = x;
      domeright[i].wp[j].y = 0;
      domeright[i].wp[j].z = z;
      domeright[i].wi[j] = b;
    }
  }
}

/*
   Lookup inverse warp map, goes from fisheye mesh coordinates (x,y),
   treat them as (u,v), lookup the warp map (x,y).
   Original (ux,vy) ranges from -1 to 1
   (tu,tv) ranges from 0 to 1
   (ix iy) ranges from 0 to meshnx and meshny
*/
void EstimateWarp(double u,double v,double *x,double *z,double *br) {
  int i,n,ix,iy;
  double tu,tv;
  double mu,longitude,latitude;
  XYZ p1,p,p0;
  
  double domeradius = 2.5;
  double mirrorradius = 0.3;
  XYZ mirrorpos = {2.4,0,0};
  XYZ projectorpos = {1.85,0,0};
  double aspectratio = 4/3.0;
  double throwdist = 1.5;
  XYZ frustum[4];
  
  if (_s2fd_options->dometype == MIRROR1) {
    
    // Location of projection plane, 1 unit in front of projector
    for (i=0;i<4;i++)
      frustum[i].x = projectorpos.x + 1;
    frustum[0].y = -0.5/throwdist;
    frustum[1].y = -0.5/throwdist;
    frustum[2].y = 0.5/throwdist;
    frustum[3].y = 0.5/throwdist;
    frustum[0].z = 0;
    frustum[1].z = 1.0/(throwdist*aspectratio);
    frustum[2].z = 1.0/(throwdist*aspectratio);
    frustum[3].z = 0;

    // longitude and latitude, latitude clampd at pi/2
    // latitude = 0 at north pole of dome, pi/2 at horizon of dome
    longitude = atan2(v,u); // -pi to pi
    latitude  = PID2 * sqrt(u*u + v*v);
    if (latitude > PID2)
      latitude = PID2;
    
    // p1 is the point on the dome, dome centered at the origin
    p1.x = domeradius * sin(latitude) * sin(longitude);
    p1.y = domeradius * sin(latitude) * cos(longitude);
    p1.z = domeradius * cos(latitude);
    
    // Calculate the reflection position on the dome
    if ((n = MirrorPosition(mirrorpos,mirrorradius,p1,projectorpos,&p0)) != 0) {
      fprintf(stderr,"Error %d, at (%g,%g) = (%g,%g)\n",n,u,v,latitude,longitude);
    }
    
    // Calculate the point "p" on the projection plane
    mu = (frustum[0].x - projectorpos.x) / (p0.x - projectorpos.x);
    p.x = frustum[0].x;
    p.y = projectorpos.y + mu * (p0.y - projectorpos.y);
    p.z = projectorpos.z + mu * (p0.z - projectorpos.z);
    
    // Set brigthness
    *br = 1;
    if (p1.x > 0)
      *br = 1 - p1.x / domeradius;
    if (*br < 0)
      *br = 0;
    if (p1.x >= mirrorpos.x)
      *br = -1;
    
    p.y *= throwdist;
    p.y *= 2;
    p.y *= aspectratio;
    p.z *= throwdist;
    p.z *= aspectratio;
    p.z *= 2;
    p.z -= 1;
    *x = p.y;
    *z = p.z;
    
  } else { // WARPMAP
    
    tu = (u + 1) / 2.0;
    if (tu < 0) tu = 0;
    if (tu > 1) tu = 1;
    
    tv = (v + 1) / 2.0;
    if (tv < 0) tv = 0;
    if (tv > 1) tv = 1;
    
    ix = (int)(tu * (meshnx - 1));
    iy = (int)(tv * (meshny - 1));
    
    *x  = mesh[ix][iy].x;
    *z  = mesh[ix][iy].y;
    *br = mesh[ix][iy].i;
  }
}

/*
   c  is the mirror position
   r is the mirror radius
   p1 is the position on the dome
   p2 is the projector position
*/ 
int MirrorPosition(XYZ c,double r,XYZ p1,XYZ p2,XYZ *p0)
{
   int i;
   int nres = 1000;
   double theta,thetamax,thetamin,phi;
   double ctheta,stheta;
   double l1,l2,val,valmin;
   XYZ p; 
   
   p0->x = 0;
   p0->y = 0;
   p0->z = 0;
   
   if (p1.z < 0)
      return(1);
      
   // Translate to origin
   p1.x -= c.x; 
   p2.x -= c.x;

   // Rotate about z axis so p1 lies in x-z plane
   phi = atan2(p1.y,p1.z);
   p1 = RotateX(p1,-phi);
   if (fabs(p1.y) > 0.00001)
      return(2);

   // Calculate the maximum possible value of theta
   // Mirror is at origin
   if (fabs(p2.x) <= r)
      return(3);
   thetamax = acos(r/fabs(p2.x));

   // Iterate to find the minimum light path length
   thetamin = thetamax;
   valmin = 1e32;
   for (i=0;i<=nres;i++) {
      theta = i * PID2 / (double)nres;
      if (theta >= thetamax)
         break;
      ctheta = cos(theta);
      stheta = sin(theta);
      l1 = (p2.x + r*ctheta)*(p2.x + r*ctheta) + (r*stheta)*(r*stheta);
      l2 = (p1.x + r*ctheta)*(p1.x + r*ctheta) + (p1.z - r*stheta)*(p1.z - r*stheta);
      val = sqrt(l1) + sqrt(l2);
      if (val < valmin) {
         valmin = val;
         thetamin = theta;
      }
   }
   //fprintf(stderr,"%d %g %g (%g,%g,%g) %g\n",i,thetamin,thetamax,p1.x,p1.y,p1.z,p2.x);
   if (thetamin < 0)
      return(4);

   // Calculate p0
   p.x = -r * cos(thetamin);
   p.y = 0;
   p.z = r * sin(thetamin);

   // Rotate back
   p = RotateX(p,phi);

   // Translate back
   p.x += c.x;

   *p0 = p;

   return(0);
}

void prep_s2fishdome(OPTIONS *ioptions) {
  _s2fd_options = ioptions;

  texturetop = NULL;
  texturebottom = NULL;
  textureleft = NULL;
  textureright = NULL;

  mesh = NULL;
  meshtype = 0;
  meshnx = meshny = 0;

  ndometop = ndomebottom = 0;
  ndomeleft = ndomeright = 0;
  dometop = domebottom = NULL;
  domeleft = domeright = NULL;

  if (_s2fd_options->dometype == WARPMAP) {
    // Possibly read the mesh file, for warped dome projection
    if (!ReadBinMesh(_s2fd_options->meshfile)) {
      if (_s2fd_options->debug)
	fprintf(stderr,"Creating default mesh\n");
      MakeDefaultMesh();
    }
  } else {
    //fprintf(stderr, "Making a default mesh of course\n");
    MakeDefaultMesh();
  }
  
  if (_s2fd_options->debug)
    fprintf(stderr,"Creating the dome\n");
  if (!DomeCreate()) {
    fprintf(stderr,"Failed to create dome geometry\n");
    CleanExit();
  }
  
  // Create the texture buffers and default images
  if (_s2fd_options->debug)
    fprintf(stderr,"Creating dome textures\n");
  DomeTextures();
  
  // Texture IDs for each face
  if (_s2fd_options->debug)
    fprintf(stderr,"Binding textures\n");
  glGenTextures(4,walltextureid);
  if (walltextureid[0] < 1 || walltextureid[1] < 1 || walltextureid[2] < 1 || walltextureid[3] < 1) {
    fprintf(stderr,"Received bad texture id <%d,%d,%d,%d>\n",
	    (int)walltextureid[0],(int)walltextureid[1],(int)walltextureid[2],(int)walltextureid[3]);
  }
  int i;
  for (i=0;i<4;i++) {
    if (_s2fd_options->debug)
      fprintf(stderr,"%d\n",i);
    glBindTexture(GL_TEXTURE_2D,walltextureid[i]);
#if defined(S2CYGWIN)
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
#else
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
#endif
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    switch (i) {
    case 0:
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,TEXTURESIZE,TEXTURESIZE,0,GL_RGBA,GL_UNSIGNED_BYTE,texturetop);
      break;
    case 1:
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,TEXTURESIZE,TEXTURESIZE,0,GL_RGBA,GL_UNSIGNED_BYTE,texturebottom);
      break;
    case 2:
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,TEXTURESIZE,TEXTURESIZE,0,GL_RGBA,GL_UNSIGNED_BYTE,textureleft);
      break;
    case 3:
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,TEXTURESIZE,TEXTURESIZE,0,GL_RGBA,GL_UNSIGNED_BYTE,textureright);
    }
  }

  _s2debug("(internal)", "/S2FISH*,/S2TRUNC* device support loaded");
}

void resize_s2fishdome(void) {
  // no special action for this device
}

int keybd_s2fishdome(char c) {
  int consumed = 0;
  // no special key for this device
  return consumed;
}

void _s2_fadeinout(void);
void draw_s2fishdome(CAMERA cam) {
  
  // draw a fisheye / warped projection of the geometry
  double r, near, far;
  XYZ vp, vd, vr, vl;
  XYZ vright, vleft, vup;

  // Calculate various view vectors 
  vp = cam.vp;
  vd = cam.vd;
  vr = CrossProduct(vd,cam.vu);
  vl = CrossProduct(cam.vu,vd);
  vd = ArbitraryRotate(vd,cam.fishrotate,vr);
  vup = CrossProduct(vr,vd);
  vright = VectorAdd(vr,vd);
  vleft = VectorAdd(vl,vd);
  near = VectorLength(_s2priv_pmin(),_s2priv_pmax()) / 100;
  far  = MAX(cam.focallength,VectorLength(_s2priv_pmin(),
					     _s2priv_pmax())) * 20;
  
  // Left
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
  glViewport(0,0,TEXTURESIZE,TEXTURESIZE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  s2Perspective(90.0,1.0,near,far);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  s2LookAt(vp.x,vp.y,vp.z,vp.x+vleft.x,vp.y+vleft.y,vp.z+vleft.z,vup.x,vup.y,vup.z);
  MakeLighting();
  MakeMaterial();
  MakeGeometry(FALSE, FALSE);
  glBindTexture(GL_TEXTURE_2D,walltextureid[2]);
  glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,TEXTURESIZE,TEXTURESIZE);
    
  // Right
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
  glViewport(0,0,TEXTURESIZE,TEXTURESIZE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  s2Perspective(90.0,1.0,near,far);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  s2LookAt(vp.x,vp.y,vp.z,vp.x+vright.x,vp.y+vright.y,vp.z+vright.z,vup.x,vup.y,vup.z);
  MakeLighting();
  MakeMaterial();
  MakeGeometry(FALSE, FALSE);
  glBindTexture(GL_TEXTURE_2D,walltextureid[3]);
  glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,TEXTURESIZE,TEXTURESIZE);
  
  // Top 
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
  glViewport(0,0,TEXTURESIZE,TEXTURESIZE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  s2Perspective(90.0,1.0,near,far);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  s2LookAt(vp.x,vp.y,vp.z,vp.x+vup.x,vp.y+vup.y,vp.z+vup.z,-vright.x,-vright.y,-vright.z);
  MakeLighting();
  MakeMaterial();
  MakeGeometry(FALSE, FALSE);
  glBindTexture(GL_TEXTURE_2D,walltextureid[0]);
  glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,TEXTURESIZE,TEXTURESIZE);
  
  // Bottom 
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
  glViewport(0,0,TEXTURESIZE,TEXTURESIZE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  s2Perspective(90.0,1.0,near,far);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  s2LookAt(vp.x,vp.y,vp.z,vp.x-vup.x,vp.y-vup.y,vp.z-vup.z,-vleft.x,-vleft.y,-vleft.z);
  MakeLighting();
  MakeMaterial();
  MakeGeometry(FALSE, FALSE);
  glBindTexture(GL_TEXTURE_2D,walltextureid[1]);
  glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,TEXTURESIZE,TEXTURESIZE);
  
  // Remember the graphics state and return it at the end
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_DITHER);
  //glDisable(GL_FOG);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_LINE_STIPPLE);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_STENCIL_TEST);
  
  // Setup projections for the dome
  glDrawBuffer(GL_BACK);
  if (_s2fd_options->dometype == WARPMAP)
    glClearColor(0.0,0.0,0.0,0.0);
  else
    glClearColor(0.05,0.05,0.05,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0,0,_s2fd_options->screenwidth,_s2fd_options->screenheight);
  r = _s2fd_options->screenwidth / (double)_s2fd_options->screenheight;
  switch (_s2fd_options->dometype) {
  case TRUNCTOP:
  case TRUNCBOTTOM:
    glOrtho(-r*0.75,r*0.75,-0.75,0.75,0.1,10.0);
    break;
  case HSPHERICAL:
    glOrtho(-r*0.75,r*0.75,-0.75,0.75,0.1,10.0);
    break;
  case VSPHERICAL:
  case WARPMAP:
  default:
    glOrtho(-r,r,-1.0,1.0,0.1,10.0);
    break;
  }
    
  // Create camera projection for dome
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  switch (_s2fd_options->dometype) {
  case TRUNCBOTTOM:
    s2LookAt(0.0,-1.0,0.25,0.0,0.0,0.25,0.0,0.0,1.0);
    break;
  case TRUNCTOP:
    s2LookAt(0.0,-1.0,-0.25,0.0,0.0,-0.25,0.0,0.0,1.0);
    break;
  case HSPHERICAL:
  case VSPHERICAL:
  case WARPMAP:
  default:
    s2LookAt(0.0,-1.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0);
    break;
  }
    
  // Finally draw the dome geometry
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glColor3f(1.0,1.0,1.0);
  DrawDome(TRUE,FALSE);

  _s2_fadeinout();
  
  DrawExtras();
  glPopAttrib();
  
}

