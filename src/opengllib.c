/* opengllib.c
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
 * $Id: opengllib.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "s2opengl.h"
#include "s2const.h"
#include "s2win.h"

#include "paulslib.h"
#include "bitmaplib.h"
#include "opengllib.h"

/*
	Clear the left and/or right buffer
	Call as ClearBuffer(1) for mono
           ClearBuffer(2) for stereo
           ClearBuffer(1) if in stereo and the card will clear both, eg: 4D51T
*/
void ClearBuffers(int which) {
  if (which == 2) {
    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawBuffer(GL_BACK_RIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  } else {
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}
void ClearAllBuffers(void) {
  glDrawBuffer(GL_FRONT_AND_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | 
	  GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
}

/*
   Write the current view to an image file
	Do the right thing for stereo, ie: two images
	The format corresponds to the Write_Bitmap() formats. 
	Use a negative format to get the image flipped vertically
	Honour the name if supplied, else do automatic naming
	Don't overwrite existing files if automatic naming is in effect.
	
*/
int WindowDump(char *name,int width,int height,int stereo,int format)
{
   FILE *fptr;
   static int counter = 0;
   char fname[32],ext[8];
   BITMAP4 *image = NULL;

   /* Allocate our buffer for the image */
   if ((image = Create_Bitmap(width,height)) == NULL) {
      fprintf(stderr,"WindowDump - Failed to allocate memory for image\n");
      return(FALSE);
   }

	glFinish();
   glPixelStorei(GL_PACK_ALIGNMENT,1);

   /* Open the file */
	switch (ABS(format)) {
   case 1: strcpy(ext,"tga"); break;
	case 11: strcpy(ext,"tga"); break;
	case 12: strcpy(ext,"tga"); break;
	case 13: strcpy(ext,"tga"); break;
   case 2: strcpy(ext,"ppm"); break;
   case 3: strcpy(ext,"rgb"); break;
   case 4: strcpy(ext,"raw"); break;
   case 5: strcpy(ext,"tif"); break;
   case 6: strcpy(ext,"eps"); break;
   case 7: strcpy(ext,"eps"); break;
	case 8: strcpy(ext,"raw"); break;
	case 9: strcpy(ext,"bmp"); break;
	}
	if (strlen(name) <= 0) {
		if (stereo)
			sprintf(fname,"L_%04d.%s",counter,ext);
		else
			sprintf(fname,"%04d.%s",counter,ext);
	} else {
      if (stereo)
         sprintf(fname,"L_%s.%s",name,ext);
      else
         sprintf(fname,"%s.%s",name,ext);
	}
	while (strlen(name) <= 0 && (fptr = fopen(fname,"rb")) != NULL) {
		counter++;
		fclose(fptr);
   	if (stereo)
      	sprintf(fname,"L_%04d.%s",counter,ext);
   	else
      	sprintf(fname,"%04d.%s",counter,ext);
	}
   if ((fptr = fopen(fname,"wb")) == NULL) {
      fprintf(stderr,"WindowDump - Failed to open file for window dump\n");
      return(FALSE);
   }

   /* Copy the image into our buffer */
   glReadBuffer(GL_BACK_LEFT);
   glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,image);

   /* Write the file */
	Write_Bitmap(fptr,image,width,height,format);
   fclose(fptr);

   if (stereo) {

		/* Open the file */
   	if (strlen(name) <= 0) {
   	   sprintf(fname,"R_%04d.%s",counter,ext);
   	} else {
   	   sprintf(fname,"R_%s.%s",name,ext);
   	}
   	while (strlen(name) <= 0 && (fptr = fopen(fname,"rb")) != NULL) {
   	   counter++;
   	   fclose(fptr);
   	   sprintf(fname,"R_%04d.%s",counter,ext);
   	}
      if ((fptr = fopen(fname,"wb")) == NULL) {
         fprintf(stderr,"WindowDump - Failed to open file for window dump\n");
         return(FALSE);
      }

      /* Copy the image into our buffer */
      glReadBuffer(GL_BACK_RIGHT);
      glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,image);

      /* Write the file */
      Write_Bitmap(fptr,image,width,height,format);
      fclose(fptr);
   }

   Destroy_Bitmap(image);
	counter++;
   return(TRUE);
}

/*
	Create a wireframe or solid box betwwen two diagonal vertices
	Assume it is all one colour and that has already been set up
	If type == 0 then draw a wireframe box
              1             solid
              2             wireframe with inset
	The faces are ordered CCW
              
        +-----+
       /|    /|       y
      / |   / |      ^
     /  |  /  |      |
   p/min+-----+      |
   /   / /   /       |
  +-----+pma/x       |
  |  /  |  /         +-------> x
  | /   | /         /
  |/    |/         /
  +-----+         / z
*/
void CreateBox(XYZ pmin,XYZ pmax,int type)
{
	XYZ dp;

	switch (type) {
	case 0:
		glBegin(GL_LINE_STRIP);
   	glVertex3f(pmin.x,pmin.y,pmin.z); 
   	glVertex3f(pmax.x,pmin.y,pmin.z);
		glVertex3f(pmax.x,pmin.y,pmax.z);
		glVertex3f(pmin.x,pmin.y,pmax.z);
		glVertex3f(pmin.x,pmin.y,pmin.z);
		glVertex3f(pmin.x,pmax.y,pmin.z);
   	glVertex3f(pmax.x,pmax.y,pmin.z);
   	glVertex3f(pmax.x,pmax.y,pmax.z);
   	glVertex3f(pmin.x,pmax.y,pmax.z);
   	glVertex3f(pmin.x,pmax.y,pmin.z);
   	glEnd();
		glBegin(GL_LINES);
		glVertex3f(pmax.x,pmin.y,pmin.z); glVertex3f(pmax.x,pmax.y,pmin.z);
   	glVertex3f(pmax.x,pmin.y,pmax.z); glVertex3f(pmax.x,pmax.y,pmax.z);
   	glVertex3f(pmin.x,pmin.y,pmax.z); glVertex3f(pmin.x,pmax.y,pmax.z);
   	glEnd();
		break;
   case 2:
		dp.x = (pmax.x - pmin.x) / 20;
      dp.y = (pmax.y - pmin.y) / 20;
      dp.z = (pmax.z - pmin.z) / 20;
      glBegin(GL_LINE_STRIP);
      glVertex3f(pmin.x-dp.x,pmin.y-dp.y,pmin.z-dp.z);
      glVertex3f(pmax.x+dp.x,pmin.y-dp.y,pmin.z-dp.z);
      glVertex3f(pmax.x+dp.x,pmin.y-dp.y,pmax.z+dp.z);
      glVertex3f(pmin.x-dp.x,pmin.y-dp.y,pmax.z+dp.z);
      glVertex3f(pmin.x-dp.x,pmin.y-dp.y,pmin.z-dp.z);
      glVertex3f(pmin.x-dp.x,pmax.y+dp.y,pmin.z-dp.z);
      glVertex3f(pmax.x+dp.x,pmax.y+dp.y,pmin.z-dp.z);
      glVertex3f(pmax.x+dp.x,pmax.y+dp.y,pmax.z+dp.z);
      glVertex3f(pmin.x-dp.x,pmax.y+dp.y,pmax.z+dp.z);
      glVertex3f(pmin.x-dp.x,pmax.y+dp.y,pmin.z-dp.z);
      glEnd();
      glBegin(GL_LINES);
      glVertex3f(pmax.x+dp.x,pmin.y-dp.y,pmin.z-dp.z); 
		glVertex3f(pmax.x+dp.x,pmax.y+dp.y,pmin.z-dp.z);
      glVertex3f(pmax.x+dp.x,pmin.y-dp.y,pmax.z+dp.z); 
		glVertex3f(pmax.x+dp.x,pmax.y+dp.y,pmax.z+dp.z);
      glVertex3f(pmin.x-dp.x,pmin.y-dp.y,pmax.z+dp.z); 
		glVertex3f(pmin.x-dp.x,pmax.y+dp.y,pmax.z+dp.z);
      glEnd();
		glBegin(GL_LINE_STRIP);       /* Right */
		glVertex3f(pmin.x-dp.x,pmin.y,pmin.z);
      glVertex3f(pmin.x-dp.x,pmin.y,pmax.z);
      glVertex3f(pmin.x-dp.x,pmax.y,pmax.z);
      glVertex3f(pmin.x-dp.x,pmax.y,pmin.z);
      glVertex3f(pmin.x-dp.x,pmin.y,pmin.z);
		glEnd();
      glBegin(GL_LINE_STRIP);       /* Left */
      glVertex3f(pmax.x+dp.x,pmin.y,pmin.z);
      glVertex3f(pmax.x+dp.x,pmin.y,pmax.z);
      glVertex3f(pmax.x+dp.x,pmax.y,pmax.z);
      glVertex3f(pmax.x+dp.x,pmax.y,pmin.z);
      glVertex3f(pmax.x+dp.x,pmin.y,pmin.z);
      glEnd();
      glBegin(GL_LINE_STRIP);       /* Top */
      glVertex3f(pmin.x,pmax.y+dp.y,pmin.z);
      glVertex3f(pmin.x,pmax.y+dp.y,pmax.z);
      glVertex3f(pmax.x,pmax.y+dp.y,pmax.z);
      glVertex3f(pmax.x,pmax.y+dp.y,pmin.z);
      glVertex3f(pmin.x,pmax.y+dp.y,pmin.z);
      glEnd();
      glBegin(GL_LINE_STRIP);       /* Bottom */
      glVertex3f(pmin.x,pmin.y-dp.y,pmin.z);
      glVertex3f(pmin.x,pmin.y-dp.y,pmax.z);
      glVertex3f(pmax.x,pmin.y-dp.y,pmax.z);
      glVertex3f(pmax.x,pmin.y-dp.y,pmin.z);
      glVertex3f(pmin.x,pmin.y-dp.y,pmin.z);
      glEnd();
      glBegin(GL_LINE_STRIP);       /* Front */
      glVertex3f(pmin.x,pmin.y,pmin.z-dp.z);
      glVertex3f(pmin.x,pmax.y,pmin.z-dp.z);
      glVertex3f(pmax.x,pmax.y,pmin.z-dp.z);
      glVertex3f(pmax.x,pmin.y,pmin.z-dp.z);
      glVertex3f(pmin.x,pmin.y,pmin.z-dp.z);
      glEnd();
      glBegin(GL_LINE_STRIP);       /* Back */
      glVertex3f(pmin.x,pmin.y,pmax.z+dp.z);
      glVertex3f(pmin.x,pmax.y,pmax.z+dp.z);
      glVertex3f(pmax.x,pmax.y,pmax.z+dp.z);
      glVertex3f(pmax.x,pmin.y,pmax.z+dp.z);
      glVertex3f(pmin.x,pmin.y,pmax.z+dp.z);
      glEnd();
      break;
	case 1:
      glBegin(GL_QUADS);
      glNormal3f(0.0,0.0,-1.0);
      glTexCoord2f(0.0,0.0); glVertex3f(pmin.x,pmin.y,pmin.z);
      glTexCoord2f(1.0,0.0); glVertex3f(pmin.x,pmax.y,pmin.z);
      glTexCoord2f(1.0,1.0); glVertex3f(pmax.x,pmax.y,pmin.z);
      glTexCoord2f(0.0,1.0); glVertex3f(pmax.x,pmin.y,pmin.z);
      glNormal3f(0.0,0.0,1.0);
      glTexCoord2f(0.0,0.0); glVertex3f(pmin.x,pmin.y,pmax.z);
      glTexCoord2f(1.0,0.0); glVertex3f(pmax.x,pmin.y,pmax.z);
      glTexCoord2f(1.0,1.0); glVertex3f(pmax.x,pmax.y,pmax.z);
      glTexCoord2f(0.0,1.0); glVertex3f(pmin.x,pmax.y,pmax.z);
      glNormal3f(-1.0,0.0,0.0);
      glTexCoord2f(0.0,0.0); glVertex3f(pmin.x,pmin.y,pmax.z);
      glTexCoord2f(1.0,0.0); glVertex3f(pmin.x,pmax.y,pmax.z);
      glTexCoord2f(1.0,1.0); glVertex3f(pmin.x,pmax.y,pmin.z);
      glTexCoord2f(0.0,1.0); glVertex3f(pmin.x,pmin.y,pmin.z);
      glNormal3f(1.0,0.0,0.0);
      glTexCoord2f(0.0,0.0); glVertex3f(pmax.x,pmin.y,pmax.z);
      glTexCoord2f(1.0,0.0); glVertex3f(pmax.x,pmin.y,pmin.z);
      glTexCoord2f(1.0,1.0); glVertex3f(pmax.x,pmax.y,pmin.z);
      glTexCoord2f(0.0,1.0); glVertex3f(pmax.x,pmax.y,pmax.z);
      glNormal3f(0.0,-1.0,0.0);
      glTexCoord2f(0.0,0.0); glVertex3f(pmin.x,pmin.y,pmax.z);
      glTexCoord2f(1.0,0.0); glVertex3f(pmin.x,pmin.y,pmin.z);
      glTexCoord2f(1.0,1.0); glVertex3f(pmax.x,pmin.y,pmin.z);
      glTexCoord2f(0.0,1.0); glVertex3f(pmax.x,pmin.y,pmax.z);
      glNormal3f(0.0,1.0,0.0);
      glTexCoord2f(0.0,0.0); glVertex3f(pmin.x,pmax.y,pmax.z);
      glTexCoord2f(1.0,0.0); glVertex3f(pmax.x,pmax.y,pmax.z);
      glTexCoord2f(1.0,1.0); glVertex3f(pmax.x,pmax.y,pmin.z);
      glTexCoord2f(0.0,1.0); glVertex3f(pmin.x,pmax.y,pmin.z);
      glEnd();
		break;
	}
}

/*
   Create a regular wireframe grid
	Split the x,y,z range into "resol" steps.
*/
void CreateRegularGrid(XYZ pmin,XYZ pmax,int resol)
{
   int i,j,k;
	double x,y,z,dx,dy,dz;

	dx = (pmax.x - pmin.x) / resol;
   dy = (pmax.y - pmin.y) / resol;
   dz = (pmax.z - pmin.z) / resol;

   glBegin(GL_LINES);
   for (k=0;k<=resol;k++) {
		z = pmin.z + dz * k;
      for (i=0;i<=resol;i++) {
			x = pmin.x + dx * i;
         glVertex3f(x,pmin.y,z);
         glVertex3f(x,pmax.y,z);
      }
   }
   for (k=0;k<=resol;k++) {
		z = pmin.z + dz * k;
      for (j=0;j<=resol;j++) {
			y = pmin.y + dy * j;
         glVertex3f(pmin.x,y,z);
         glVertex3f(pmax.x,y,z);
      }
   }
   for (i=0;i<=resol;i++) {
		x = pmin.x + dx * i;
      for (j=0;j<=resol;j++) {
			y = pmin.y + dy * j;
         glVertex3f(x,y,pmin.z);
         glVertex3f(x,y,pmax.z);
      }
   }
   glEnd();
}

/*
	Create a polygon from the vertices
	Handle degenerate case
	Calculate normals if not provided
	Include colours if provided per vertex
*/
void CreatePolygon(XYZ *p,XYZ *n,COLOUR *c,int np)
{
	int i;
	XYZ normal = {1,0,0};

	if (np < 1) {
		;
	} else if (np == 1) {
		glVertex3f(p[0].x,p[0].y,p[0].z);
	} else if (np == 2) {
      glVertex3f(p[0].x,p[0].y,p[0].z);
      glVertex3f(p[1].x,p[1].y,p[1].z);
	} else if (np == 3 || np == 4) {
		if (n == NULL)
			normal = CalcNormal(p[0],p[1],p[2]);
		for (i=0;i<np;i++) {
			if (n != NULL) 
				normal = n[i];
			glNormal3f(normal.x,normal.y,normal.z);
			if (c != NULL)
				glColor3f(c[i].r,c[i].g,c[i].b);
			switch (i) {
			case 0: glTexCoord2f(0.0,0.0); break;
			case 1: glTexCoord2f(1.0,0.0); break;
			case 2: glTexCoord2f(1.0,1.0); break;
			case 3: glTexCoord2f(0.0,1.0); break;
			}
      	glVertex3f(p[i].x,p[i].y,p[i].z);
		}
	} else {

	}
}

/*
   Create a simple sphere
   method is 0 for quads, 1 for triangles
   method "2" is a special mode to match freewrls method
   (quads look nicer in wireframe mode) 
*/
void CreateSimpleSphere(XYZ c,double r,int n,int method)
{
   int i,j,nn;
   double theta1,theta2,theta3;
   XYZ e,p;

   if (r < 0)
      r = -r;
   if (n < 0)
      n = -n;
   if (n < 4 || r <= 0) {
      glBegin(GL_POINTS);
      glVertex3f(c.x,c.y,c.z);
      glEnd();
      return;
   }

   /* Make n even */
   n /= 2;
   n *= 2;
   
   if (method == 2)
      nn = n;
   else
      nn = n/2;

   for (j=0;j<nn;j++) {
      if (method == 2) { 
         theta1 = j * PI / nn - PID2; 
         theta2 = (j + 1) * PI / nn - PID2;
      } else {
         theta1 = j * TWOPI / n - PID2; 
         theta2 = (j + 1) * TWOPI / n - PID2;
      }

      if (method == 1)
         glBegin(GL_TRIANGLE_STRIP);
      else
         glBegin(GL_QUAD_STRIP);

      for (i=0;i<=n;i++) {
         theta3 = i * TWOPI / n;

         e.x = cos(theta2) * cos(theta3);
         e.y = sin(theta2);
         e.z = cos(theta2) * sin(theta3);
         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;

         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
         glVertex3f(p.x,p.y,p.z);

         e.x = cos(theta1) * cos(theta3);
         e.y = sin(theta1);
         e.z = cos(theta1) * sin(theta3);
         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;

         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*j/(double)n);
         glVertex3f(p.x,p.y,p.z);
      }
      glEnd();
   }
}

/*
   Create a superellipse
   "method" is 0 for quads, 1 for triangles
      (quads look nicer in wireframe mode)/
	This is a "unit" ellipsoid (-1 -> 1) at the origin, 
		glTranslate() and glScale() as required.
*/
void CreateSuperEllipse(double power1,double power2,int n,int method)
{
   int i,j;
   double theta1,theta2,theta3;
   XYZ p,p1,p2,en;
	double delta;

	/* Shall we just draw a point? */
   if (n < 4) {
      glBegin(GL_POINTS);
      glVertex3f(0.0,0.0,0.0);
      glEnd();
      return;
   }

	/* Shall we just draw a plus */
	if (power1 > 10 && power2 > 10) {
		glBegin(GL_LINES);  
		glVertex3f(-1.0, 0.0, 0.0); 
		glVertex3f( 1.0, 0.0, 0.0); 
		glVertex3f( 0.0,-1.0, 0.0); 
		glVertex3f( 0.0, 1.0, 0.0); 
		glVertex3f( 0.0, 0.0,-1.0); 
		glVertex3f( 0.0, 0.0, 1.0); 
		glEnd();
      return;
	}

	delta = 0.01 * TWOPI / n;
   for (j=0;j<n/2;j++) {
     	theta1 = j * TWOPI / (double)n - PID2;
     	theta2 = (j + 1) * TWOPI / (double)n - PID2;

      if (method == 0)
         glBegin(GL_QUAD_STRIP);
      else
         glBegin(GL_TRIANGLE_STRIP);
      for (i=0;i<=n;i++) {
			if (i == 0 || i == n)
				theta3 = 0;
			else
         	theta3 = i * TWOPI / n;

			EvalSuperEllipse(theta2,theta3,power1,power2,&p);
			EvalSuperEllipse(theta2+delta,theta3,power1,power2,&p1);
			EvalSuperEllipse(theta2,theta3+delta,power1,power2,&p2);
			en = CalcNormal(p1,p,p2);
         glNormal3f(en.x,en.y,en.z);
         glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
         glVertex3f(p.x,p.y,p.z);

			EvalSuperEllipse(theta1,theta3,power1,power2,&p);
         EvalSuperEllipse(theta1+delta,theta3,power1,power2,&p1);
         EvalSuperEllipse(theta1,theta3+delta,power1,power2,&p2);
			en = CalcNormal(p1,p,p2);
         glNormal3f(en.x,en.y,en.z);
         glTexCoord2f(i/(double)n,2*j/(double)n);
         glVertex3f(p.x,p.y,p.z);
      }
      glEnd();
   }
}

void EvalSuperEllipse(double t1,double t2,double p1,double p2,XYZ *p)
{
	double tmp;
	double ct1,ct2,st1,st2;

	ct1 = cos(t1);
	ct2 = cos(t2);
	st1 = sin(t1);
	st2 = sin(t2);

	tmp  = SIGN(ct1) * pow(fabs(ct1),p1);
   p->x = tmp * SIGN(ct2) * pow(fabs(ct2),p2);
   p->y = SIGN(st1) * pow(fabs(st1),p1);
   p->z = tmp * SIGN(st2) * pow(fabs(st2),p2);
}

/*
	Create a sphere centered at c, with radius r, and precision n
	Draw a point for zero radius spheres
	Use CCW facet ordering
	"method" is 0 for quads, 1 for triangles 
		(quads look nicer in wireframe mode)
	Partial spheres can be created using theta1->theta2, phi1->phi2
	in radians 0<theta<2pi, -pi/2<phi<pi/2
*/
void CreateSphere(XYZ c,double r,int n,int method,
	double theta1,double theta2,double phi1,double phi2)
{
	int i,j;
	double t1,t2,t3;
	XYZ e,p;

	/* Handle special cases */
	if (r < 0)
		r = -r;
	if (n < 0)
		n = -n;
	if (n < 4 || r <= 0) {
		glBegin(GL_POINTS);
		glVertex3f(c.x,c.y,c.z);
		glEnd();
		return;
	}

	for (j=0;j<n/2;j++) {
		t1 = phi1 + j * (phi2 - phi1) / (n/2);
		t2 = phi1 + (j + 1) * (phi2 - phi1) / (n/2);

		if (method == 0)
			glBegin(GL_QUAD_STRIP);
		else
			glBegin(GL_TRIANGLE_STRIP);

      for (i=0;i<=n;i++) {
			t3 = theta1 + i * (theta2 - theta1) / n;

         e.x = cos(t1) * cos(t3);
         e.y = sin(t1);
         e.z = cos(t1) * sin(t3);
			p.x = c.x + r * e.x;
			p.y = c.y + r * e.y;
			p.z = c.z + r * e.z;
         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*j/(double)n);
         glVertex3f(p.x,p.y,p.z);

         e.x = cos(t2) * cos(t3);
         e.y = sin(t2);
         e.z = cos(t2) * sin(t3);
         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;

         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
         glVertex3f(p.x,p.y,p.z);

      }
		glEnd();
   }
}

/*
	Create an ellipsoid, same as sphere code but with scaling 
	along the three axes by cx,cy,cz
	(x*cx)^2 + (y*cy)^2 + (z*cz)^2 = r^2
	Use CCW facet ordering
*/
void CreateEllipsoid(XYZ c,double r,int n,
	double cx,double cy,double cz)
{
   int i,j;
   double theta1,theta2,theta3;
   XYZ e,p,normal;

   if (r < 0)
      r = -r;
   if (n < 0)
      n = -n;
   if (n < 1 || r <= 0 || cx <= 0 || cy <= 0 || cz <= 0) {
      glBegin(GL_POINTS);
      glVertex3f(c.x,c.y,c.z);
      glEnd();
      return;
   }

   for (j=0;j<n/2;j++) {
      theta1 = j * TWOPI / n - PID2;
      theta2 = (j + 1) * TWOPI / n - PID2;

      glBegin(GL_QUAD_STRIP);
      for (i=0;i<=n;i++) {
         theta3 = i * TWOPI / n;

         e.x = cos(theta1) * cos(theta3) * cx;
         e.y = sin(theta1) * cy;
         e.z = cos(theta1) * sin(theta3) * cz;
			normal = e;
			Normalise(&normal);
			p.x = c.x + r * e.x;
			p.y = c.y + r * e.y;
			p.z = c.z + r * e.z;

         glNormal3f(normal.x,normal.y,normal.z);
         glTexCoord2f(i/(double)n,2*j/(double)n);
         glVertex3f(p.x,p.y,p.z);

         e.x = cos(theta2) * cos(theta3) * cx;
         e.y = sin(theta2) * cy;
         e.z = cos(theta2) * sin(theta3) * cz;
         normal = e;
         Normalise(&normal);
         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;

         glNormal3f(normal.x,normal.y,normal.z);
         glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
         glVertex3f(p.x,p.y,p.z);

      }
      glEnd();
	}
}

/*
   Create a disk centered at c, with normal n,
	inner radius r0, and outer radius r1, and precision m
	theta1 and theta2 are the start and end angles in radians
*/
void CreateDisk(XYZ c,XYZ n,double r0,double r1,int m,
	double theta1,double theta2)
{
   int i;
	double theta;
	XYZ p,q,perp;
	
	/* 
		Create two perpendicular vectors perp and q 
		on the plane of the disk 
	*/
	Normalise(&n);
	perp = n;
	if (n.x == 0 && n.z == 0)
		perp.x += 1;
	else
		perp.y += 1;
	CROSSPROD(perp,n,q);
	CROSSPROD(n,q,perp);
	Normalise(&perp);
	Normalise(&q);

	glBegin(GL_QUAD_STRIP);
	for (i=0;i<=m;i++) {
      theta = theta1 + i * (theta2 - theta1) / m;
		p.x = c.x + r0 * (cos(theta) * perp.x + sin(theta) * q.x);
      p.y = c.y + r0 * (cos(theta) * perp.y + sin(theta) * q.y);
      p.z = c.z + r0 * (cos(theta) * perp.z + sin(theta) * q.z);
		glNormal3f(n.x,n.y,n.z);
		glTexCoord2f(i/(double)m,0.0);
		glVertex3f(p.x,p.y,p.z);
      p.x = c.x + r1 * (cos(theta) * perp.x + sin(theta) * q.x);
      p.y = c.y + r1 * (cos(theta) * perp.y + sin(theta) * q.y);
      p.z = c.z + r1 * (cos(theta) * perp.z + sin(theta) * q.z);
      glNormal3f(n.x,n.y,n.z);
		glTexCoord2f(i/(double)m,1.0);
		glVertex3f(p.x,p.y,p.z);
	}
	glEnd();
}

/*
   Create a cone/cylinder uncapped between end points p1, p2
   radius r1, r2, and precision m
	Create the cylinder between theta1 and theta2 in radians
*/
void CreateCone(XYZ p1,XYZ p2,double r1,double r2,int m,
	double theta1,double theta2)
{
   int i;
   double theta;
   XYZ n,p,q,perp;

	/* Normal pointing from p1 to p2 */
	n.x = p1.x - p2.x;
   n.y = p1.y - p2.y;
   n.z = p1.z - p2.z;
	Normalise(&n);

   /*
      Create two perpendicular vectors perp and q
      on the plane of the disk
   */
   perp = n;
   if (n.x == 0 && n.z == 0)
      perp.x += 1;
   else
      perp.y += 1;
   CROSSPROD(perp,n,q);
   CROSSPROD(n,q,perp);
   Normalise(&perp);
   Normalise(&q);

   glBegin(GL_QUAD_STRIP);
   for (i=0;i<=m;i++) {
      theta = theta1 + i * (theta2 - theta1) / m;
		n.x = cos(theta) * perp.x + sin(theta) * q.x;
      n.y = cos(theta) * perp.y + sin(theta) * q.y;
      n.z = cos(theta) * perp.z + sin(theta) * q.z;
		Normalise(&n);

      p.x = p1.x + r1 * n.x;
      p.y = p1.y + r1 * n.y;
      p.z = p1.z + r1 * n.z;
      glNormal3f(n.x,n.y,n.z);
      glTexCoord2f(i/(double)m,0.0);
      glVertex3f(p.x,p.y,p.z);

      p.x = p2.x + r2 * n.x;
      p.y = p2.y + r2 * n.y;
      p.z = p2.z + r2 * n.z;
      glNormal3f(n.x,n.y,n.z);
      glTexCoord2f(i/(double)m,1.0);
      glVertex3f(p.x,p.y,p.z);

   }
   glEnd();
}

/*
	Create a torus with the up vector aligned with the y axis
	Use OpenGL transformations to change it's orientation
	tmethod flips between the two possible tecture coordinate types
*/
void CreateTorus(XYZ c,double r0,double r1,int n,int method,
	double theta1,double theta2,double phi1, double phi2,int tmethod)
{
   int i,j;
   double t1,t2,phi;
   XYZ e,p;

   /* Handle special cases */
   if (r1 < 0)
      r1 = -r1;
   if (r0 < 0)
      r0 = -r0;
   if (n < 0)
      n = -n;
   if (n < 1 || r0+r1 <= 0) {
      glBegin(GL_POINTS);
      glVertex3f(c.x,c.y,c.z);
      glEnd();
      return;
   }

   for (j=0;j<n;j++) {
      t1 = j * (theta2 - theta1) / n;
      t2 = (j + 1) * (theta2 - theta1) / n;

      if (method == 0)
         glBegin(GL_QUAD_STRIP);
      else
         glBegin(GL_TRIANGLE_STRIP);

      for (i=0;i<=n;i++) {
         phi = phi1 + i * (phi2 - phi1) / n;

         e.x = cos(t1) * r1 * cos(phi);
         e.y = sin(phi) * r1;
         e.z = sin(t1) * r1 * cos(phi);
			Normalise(&e);
         p.x = c.x + cos(t1) * (r0 + r1 * cos(phi));
         p.y = c.y + sin(phi) * r1;
         p.z = c.z + sin(t1) * (r0 + r1 * cos(phi));
         glNormal3f(e.x,e.y,e.z);
			if (tmethod == 0)
         	glTexCoord2f(i/(double)n,j/(double)n);
			else
				glTexCoord2f(j/(double)n,i/(double)n);
         glVertex3f(p.x,p.y,p.z);

         e.x = cos(t2) * r1 * cos(phi);
         e.y = sin(phi) * r1;
         e.z = sin(t2) * r1 * cos(phi);
         Normalise(&e);
         p.x = cos(t2) * (r0 + r1 * cos(phi));
         p.y = sin(phi) * r1;
         p.z = sin(t2) * (r0 + r1 * cos(phi));
         glNormal3f(e.x,e.y,e.z);
			if (tmethod == 0)
         	glTexCoord2f(i/(double)n,(j+1)/(double)n);
			else
				glTexCoord2f((j+1)/(double)n,i/(double)n);
         glVertex3f(p.x,p.y,p.z);

      }
      glEnd();
   }

}

#define starsmag4 _s2x_starsmag4

typedef struct {
	double x,y,z;
	double r,g,b;
} XYZRGB;
XYZRGB starsmag4[518] = {
  {3.19767,48.6191,87.3267,0.626327,0.626327,0.626327},
  {2.0531,85.851,51.2385,0.604034,0.604034,0.604034},
  {2.86488,-71.6271,69.7233,0.433121,0.433121,0.433121},
  {5.57087,26.1913,96.3482,0.544586,0.544586,0.544586},
  {8.36685,-15.3398,98.4616,0.467091,0.467091,0.467091},
  {2.47381,-97.5358,21.9235,0.547771,0.547771,0.547771},
  {8.46288,-67.3091,73.4701,0.591295,0.591295,0.591295},
  {8.2507,-69.063,71.8486,0.426752,0.426752,0.426752},
  {9.4644,80.7958,58.1589,0.456476,0.456476,0.456476},
  {14.6584,51.2955,84.5808,0.497877,0.497877,0.497877},
  {9.6953,83.4244,54.2804,0.60828,0.60828,0.60828},
  {17.9813,-30.8796,93.3977,0.62845,0.62845,0.62845},
  {11.3242,84.634,52.0465,0.47983,0.47983,0.47983},
  {11.9797,87.2212,47.4232,0.582803,0.582803,0.582803},
  {19.1827,62.2507,75.8741,0.434183,0.434183,0.434183},
  {19.4955,-72.7995,65.7279,0.493631,0.493631,0.493631},
  {29.0189,-17.6779,94.05,0.478769,0.478769,0.478769},
  {24.3533,58.2415,77.5554,0.626327,0.626327,0.626327},
  {16.7584,-82.1605,54.4867,0.428875,0.428875,0.428875},
  {35.4813,-14.2341,92.4038,0.462845,0.462845,0.462845},
  {18.1576,86.8071,46.2041,0.56051,0.56051,0.56051},
  {0.789823,99.9918,1.01264,0.630573,0.630573,0.630573},
  {27.362,-68.6051,67.4141,0.483015,0.483015,0.483015},
  {37.4798,26.4645,88.8532,0.460722,0.460722,0.460722},
  {25.3998,-75.5542,60.3855,0.42569,0.42569,0.42569},
  {27.407,75.0438,60.1439,0.46603,0.46603,0.46603},
  {22.3807,-84.0913,49.2723,0.796178,0.796178,0.796178},
  {42.1779,-27.4589,86.412,0.473461,0.473461,0.473461},
  {45.9807,-17.9403,86.9708,0.449045,0.449045,0.449045},
  {21.2311,89.6254,38.9426,0.4862,0.4862,0.4862},
  {41.1908,49.3621,76.5944,0.483015,0.483015,0.483015},
  {44.8329,35.5238,82.0248,0.564756,0.564756,0.564756},
  {30.0981,-78.379,54.3216,0.452229,0.452229,0.452229},
  {15.491,95.3303,25.9259,0.422505,0.422505,0.422505},
  {46.6564,-35.9635,80.8072,0.420382,0.420382,0.420382},
  {23.5828,-87.9397,41.3577,0.541401,0.541401,0.541401},
  {38.0482,67.3396,63.3854,0.605096,0.605096,0.605096},
  {48.3297,39.8149,77.9681,0.632696,0.632696,0.632696},
  {43.8821,57.3394,69.1849,0.526539,0.526539,0.526539},
  {34.916,-78.2741,51.5174,0.467091,0.467091,0.467091},
  {57.0462,-5.19438,81.968,0.522293,0.522293,0.522293},
  {65.2709,5.64459,75.5503,0.476645,0.476645,0.476645},
  {38.006,82.8017,41.224,0.44586,0.44586,0.44586},
  {60.0507,45.8038,65.5434,0.45966,0.45966,0.45966},
  {41.7032,79.6134,43.8469,0.42569,0.42569,0.42569},
  {44.3233,-76.4812,46.7554,0.420382,0.420382,0.420382},
  {68.7625,-15.4677,70.9399,0.432059,0.432059,0.432059},
  {53.5145,-64.6853,54.3325,0.501062,0.501062,0.501062},
  {71.2288,7.13185,69.8255,0.576433,0.576433,0.576433},
  {42.9249,80.3923,41.1647,0.53397,0.53397,0.53397},
  {56.3062,62.7152,53.8184,0.485138,0.485138,0.485138},
  {55.271,65.5473,51.4652,0.619958,0.619958,0.619958},
  {52.1578,70.5343,48.005,0.441614,0.441614,0.441614},
  {65.0234,-48.461,58.5105,0.434183,0.434183,0.434183},
  {71.021,-37.0684,59.8494,0.453291,0.453291,0.453291},
  {50.1553,76.4484,40.498,0.654989,0.654989,0.654989},
  {76.9717,15.6932,61.8795,0.462845,0.462845,0.462845},
  {77.4466,16.9053,60.9609,0.447983,0.447983,0.447983},
  {79.0181,-16.433,59.0432,0.449045,0.449045,0.449045},
  {55.5247,74.0658,37.832,0.525478,0.525478,0.525478},
  {70.15,53.418,47.1751,0.438429,0.438429,0.438429},
  {61.2597,67.6601,40.8577,0.444798,0.444798,0.444798},
  {81.5218,-16.9579,55.3771,0.469214,0.469214,0.469214},
  {75.8638,40.8543,50.7505,0.452229,0.452229,0.452229},
  {75.9219,41.2592,50.3342,0.434183,0.434183,0.434183},
  {76.4418,40.841,49.8865,0.54034,0.54034,0.54034},
  {35.3104,-90.4879,23.7723,0.436306,0.436306,0.436306},
  {76.8357,40.7587,49.3458,0.45966,0.45966,0.45966},
  {72.4251,52.8195,44.3249,0.542463,0.542463,0.542463},
  {22.7313,-96.2403,14.8695,0.501062,0.501062,0.501062},
  {65.9698,64.2925,38.9161,0.538217,0.538217,0.538217},
  {83.7857,-23.3592,49.3388,0.531847,0.531847,0.531847},
  {84.6974,21.6274,48.5655,0.476645,0.476645,0.476645},
  {86.8066,10.434,48.5361,0.429936,0.429936,0.429936},
  {66.1982,-67.2941,33.0046,0.435244,0.435244,0.435244},
  {41.3978,-88.68,20.5444,0.489384,0.489384,0.489384},
  {87.2437,26.9382,40.7782,0.457537,0.457537,0.457537},
  {74.9885,-55.6271,35.8099,0.467091,0.467091,0.467091},
  {86.9248,30.1413,39.1864,0.44586,0.44586,0.44586},
  {75.7266,-55.9438,33.7012,0.424628,0.424628,0.424628},
  {87.0397,32.8542,36.6701,0.470276,0.470276,0.470276},
  {88.5953,27.5003,37.3444,0.437367,0.437367,0.437367},
  {88.6497,27.347,37.3279,0.484076,0.484076,0.484076},
  {89.4973,28.4169,34.3906,0.754777,0.754777,0.754777},
  {93.2479,-5.84788,35.646,0.427813,0.427813,0.427813},
  {80.3278,-50.8474,31.016,0.43949,0.43949,0.43949},
  {53.3064,-81.9602,20.9988,0.497877,0.497877,0.497877},
  {90.7901,-24.7065,33.8638,0.434183,0.434183,0.434183},
  {94.6478,12.12,29.915,0.506369,0.506369,0.506369},
  {95.072,9.76697,29.4267,0.453291,0.453291,0.453291},
  {95.8261,4.25829,28.2706,0.450106,0.450106,0.450106},
  {80.5653,54.7068,22.7243,0.559448,0.559448,0.559448},
  {69.8473,69.2437,18.074,0.527601,0.527601,0.527601},
  {73.0221,65.7057,18.7223,0.446921,0.446921,0.446921},
  {73.1633,65.9142,17.3912,0.508493,0.508493,0.508493},
  {89.8678,-38.0604,21.7987,0.506369,0.506369,0.506369},
  {97.0386,-8.86577,22.47,0.548832,0.548832,0.548832},
  {94.0086,-27.9084,19.5831,0.493631,0.493631,0.493631},
  {68.2315,71.9316,13.0498,0.836518,0.836518,0.836518},
  {97.0363,-14.2658,19.505,0.832272,0.832272,0.832272},
  {97.5936,-11.9174,18.2612,0.462845,0.462845,0.462845},
  {98.7147,-4.18224,15.4245,0.488323,0.488323,0.488323},
  {98.2385,11.0597,15.0626,0.670913,0.670913,0.670913},
  {86.8442,47.8807,12.866,0.669851,0.669851,0.669851},
  {92.6115,-35.4445,12.9148,0.543524,0.543524,0.543524},
  {99.2536,-0.522142,12.1839,0.60828,0.60828,0.60828},
  {80.7997,-58.0285,10.2032,0.434183,0.434183,0.434183},
  {94.5279,-30.6065,11.3011,0.571125,0.571125,0.571125},
  {97.9217,17.2517,10.6644,0.469214,0.469214,0.469214},
  {98.8976,-10.2966,10.6418,0.550955,0.550955,0.550955},
  {99.44,-2.09763,10.358,0.664544,0.664544,0.664544},
  {92.8253,36.0689,9.0832,0.526539,0.526539,0.526539},
  {45.8852,-88.6928,5.30403,0.44586,0.44586,0.44586},
  {99.4678,-4.5363,9.2507,0.440552,0.440552,0.440552},
  {99.5905,-3.39014,8.38108,0.627389,0.627389,0.627389},
  {82.5049,-56.0266,7.34583,0.564756,0.564756,0.564756},
  {92.2101,-38.185,6.26066,0.462845,0.462845,0.462845},
  {96.516,-25.5816,5.49959,0.468153,0.468153,0.468153},
  {98.4386,-16.7968,5.26376,0.626327,0.626327,0.626327},
  {77.4976,63.1334,2.87896,0.423567,0.423567,0.423567},
  {62.7453,-77.7875,3.48466,0.436306,0.436306,0.436306},
  {93.3664,-35.6398,3.53714,0.440552,0.440552,0.440552},
  {81.0756,-58.4509,3.19964,0.5138,0.5138,0.5138},
  {99.1435,12.8916,2.08902,0.791932,0.791932,0.791932},
  {58.3756,81.1928,0.120564,0.450106,0.450106,0.450106},
  {96.9464,-24.4762,1.52084,0.451168,0.451168,0.451168},
  {70.7753,70.6459,0.145658,0.643312,0.643312,0.643312},
  {79.6397,60.4773,0.0967193,0.566879,0.566879,0.566879},
  {73.3544,-67.9637,0.273127,0.424628,0.424628,0.424628},
  {92.1889,38.2791,-5.99256,0.496815,0.496815,0.496815},
  {99.1922,-10.9296,-6.43838,0.422505,0.422505,0.422505},
  {86.2075,-50.0957,-7.66095,0.524416,0.524416,0.524416},
  {91.9157,38.2903,-9.23922,0.539278,0.539278,0.539278},
  {94.6632,-30.8284,-9.40693,0.63482,0.63482,0.63482},
  {83.0617,-55.1011,-8.03939,0.436306,0.436306,0.436306},
  {60.274,-79.5429,-6.3222,0.921444,0.921444,0.921444},
  {94.636,28.2328,-15.7143,0.640127,0.640127,0.640127},
  {93.1988,-32.9787,-15.0462,0.42569,0.42569,0.42569},
  {71.9142,-68.4498,-11.9574,0.508493,0.508493,0.508493},
  {88.8756,42.4691,-17.2482,0.528662,0.528662,0.528662},
  {95.5807,22.3175,-19.1379,0.488323,0.488323,0.488323},
  {93.9218,-28.763,-18.7454,1,1,1},
  {82.3447,-53.7426,-18.1957,0.424628,0.424628,0.424628},
  {80.7512,55.863,-18.9357,0.462845,0.462845,0.462845},
  {46.0014,-88.2467,-9.81778,0.497877,0.497877,0.497877},
  {61.9529,-77.2897,-13.7167,0.53397,0.53397,0.53397},
  {88.6908,-40.9667,-21.3467,0.434183,0.434183,0.434183},
  {84.6388,-48.4386,-22.1355,0.685775,0.685775,0.685775},
  {85.164,-46.8465,-23.5054,0.476645,0.476645,0.476645},
  {89.9851,35.1356,-25.8489,0.442675,0.442675,0.442675},
  {88.0355,-40.4078,-24.8389,0.524416,0.524416,0.524416},
  {85.6174,-44.4531,-26.3359,0.649682,0.649682,0.649682},
  {31.8918,-94.2635,-9.86429,0.443737,0.443737,0.443737},
  {84.5655,-45.0453,-28.6286,0.436306,0.436306,0.436306},
  {90.3504,28.4689,-32.0362,0.464968,0.464968,0.464968},
  {75.285,-60.3173,-26.3436,0.558386,0.558386,0.558386},
  {87.1206,37.4319,-31.7624,0.470276,0.470276,0.470276},
  {35.4407,-92.6904,-12.347,0.422505,0.422505,0.422505},
  {82.343,46.6357,-32.3224,0.442675,0.442675,0.442675},
  {81.3993,-48.9429,-31.285,0.584926,0.584926,0.584926},
  {91.8866,14.4174,-36.7287,0.537155,0.537155,0.537155},
  {67.3291,-68.5836,-27.624,0.5,0.5,0.5},
  {77.7765,52.827,-34.0606,0.539278,0.539278,0.539278},
  {77.7768,52.8265,-34.0607,0.63482,0.63482,0.63482},
  {90.382,9.10671,-41.811,0.804671,0.804671,0.804671},
  {89.1464,-16.5927,-42.1615,0.427813,0.427813,0.427813},
  {81.775,41.3074,-40.0818,0.46603,0.46603,0.46603},
  {79.1162,46.9874,-39.1511,0.723992,0.723992,0.723992},
  {78.6768,-48.4118,-38.2918,0.424628,0.424628,0.424628},
  {70.6659,-61.523,-34.9464,0.461783,0.461783,0.461783},
  {26.9919,-95.4272,-12.8485,0.42569,0.42569,0.42569},
  {80.6105,-42.0398,-41.6485,0.490446,0.490446,0.490446},
  {67.0302,-65.0454,-35.7219,0.449045,0.449045,0.449045},
  {52.5582,-79.8449,-29.3672,0.476645,0.476645,0.476645},
  {65.731,-64.2832,-39.3333,0.606157,0.606157,0.606157},
  {77.3849,-41.1581,-48.1413,0.546709,0.546709,0.546709},
  {57.2298,-73.5348,-36.2958,0.656051,0.656051,0.656051},
  {81.7164,15.9632,-55.386,0.471338,0.471338,0.471338},
  {41.2415,-86.1715,-29.5569,0.647558,0.647558,0.647558},
  {80.2869,-6.81265,-59.225,0.430998,0.430998,0.430998},
  {38.7691,87.2223,-29.8198,0.488323,0.488323,0.488323},
  {32.5478,-91.4515,-24.0263,0.444798,0.444798,0.444798},
  {62.4897,-57.7976,-52.4831,0.423567,0.423567,0.423567},
  {52.4656,-72.7161,-44.2689,0.437367,0.437367,0.437367},
  {46.1354,-79.7815,-38.8129,0.460722,0.460722,0.460722},
  {71.5275,31.1575,-62.5542,0.426752,0.426752,0.426752},
  {63.2589,-54.7364,-54.7926,0.454352,0.454352,0.454352},
  {74.2025,11.1797,-66.0985,0.4862,0.4862,0.4862},
  {43.486,-81.6222,-38.0367,0.636943,0.636943,0.636943},
  {51.982,-71.9845,-46.0012,0.429936,0.429936,0.429936},
  {71.7297,10.3583,-68.9025,0.514862,0.514862,0.514862},
  {47.4398,74.3631,-47.1125,0.511677,0.511677,0.511677},
  {35.4051,-87.1597,-33.9068,0.437367,0.437367,0.437367},
  {52.5798,66.6308,-52.8743,0.423567,0.423567,0.423567},
  {47.3165,73.3216,-48.8375,0.462845,0.462845,0.462845},
  {47.2557,-73.2516,-49.001,0.446921,0.446921,0.446921},
  {28.0095,-91.6336,-28.614,0.420382,0.420382,0.420382},
  {49.5265,-68.75,-53.1091,0.610403,0.610403,0.610403},
  {34.6682,-85.687,-38.1556,0.47983,0.47983,0.47983},
  {31.195,-88.5533,-34.4264,0.423567,0.423567,0.423567},
  {66.0887,4.03788,-74.9398,0.433121,0.433121,0.433121},
  {23.0605,-93.7993,-25.8825,0.666667,0.666667,0.666667},
  {51.7768,59.9059,-61.0774,0.43949,0.43949,0.43949},
  {33.3352,-85.9632,-38.7181,0.606157,0.606157,0.606157},
  {52.7505,56.4859,-63.4565,0.512739,0.512739,0.512739},
  {36.4522,-81.926,-44.2646,0.579618,0.579618,0.579618},
  {61.0049,-15.0547,-77.7931,0.63482,0.63482,0.63482},
  {27.3382,89.1497,-36.1242,0.455414,0.455414,0.455414},
  {37.1326,78.453,-49.6618,0.508493,0.508493,0.508493},
  {46.129,-64.9006,-60.4982,0.462845,0.462845,0.462845},
  {32.8942,-83.8998,-43.3451,0.512739,0.512739,0.512739},
  {57.3974,-1.99439,-81.863,0.429936,0.429936,0.429936},
  {56.0993,17.1795,-80.9799,0.471338,0.471338,0.471338},
  {50.5594,40.3133,-76.2796,0.528662,0.528662,0.528662},
  {25.6054,-88.7074,-38.4106,0.453291,0.453291,0.453291},
  {27.4542,85.7514,-43.5081,0.441614,0.441614,0.441614},
  {23.0938,-90.6838,-35.258,0.525478,0.525478,0.525478},
  {47.3719,43.848,-76.3758,0.433121,0.433121,0.433121},
  {29.6714,-81.4802,-49.8055,0.469214,0.469214,0.469214},
  {45.198,28.841,-84.4117,0.471338,0.471338,0.471338},
  {45.7867,20.7352,-86.4502,0.701699,0.701699,0.701699},
  {44.883,-21.3954,-86.7626,0.461783,0.461783,0.461783},
  {32.8818,-67.0711,-66.485,0.436306,0.436306,0.436306},
  {39.978,39.7424,-82.5972,0.47983,0.47983,0.47983},
  {31.7897,68.0906,-65.978,0.478769,0.478769,0.478769},
  {15.2681,-93.992,-30.5352,0.492569,0.492569,0.492569},
  {20.8259,-87.7416,-43.2168,0.484076,0.484076,0.484076},
  {39.7635,33.9422,-85.2455,0.567941,0.567941,0.567941},
  {39.7619,33.9404,-85.247,0.441614,0.441614,0.441614},
  {30.9613,66.2613,-68.1971,0.521231,0.521231,0.521231},
  {38.1312,-28.964,-87.7901,0.440552,0.440552,0.440552},
  {11.1462,-96.1414,-25.1514,0.420382,0.420382,0.420382},
  {20.3016,-85.4816,-47.7571,0.43949,0.43949,0.43949},
  {36.6433,16.1719,-91.6283,0.436306,0.436306,0.436306},
  {17.7637,-88.0356,-43.9795,0.492569,0.492569,0.492569},
  {23.5207,-74.5776,-62.3294,0.437367,0.437367,0.437367},
  {14.2561,-90.1791,-40.7983,0.552017,0.552017,0.552017},
  {20.4336,-75.9498,-61.7583,0.559448,0.559448,0.559448},
  {29.0273,-27.8884,-91.5404,0.514862,0.514862,0.514862},
  {23.7242,56.23,-79.2171,0.438429,0.438429,0.438429},
  {14.7997,-85.5846,-49.5605,0.443737,0.443737,0.443737},
  {13.8992,83.2752,-53.5915,0.593418,0.593418,0.593418},
  {11.5048,88.0898,-45.9112,0.654989,0.654989,0.654989},
  {15.5402,70.0892,-69.6133,0.525478,0.525478,0.525478},
  {11.4649,-85.6942,-50.2499,0.429936,0.429936,0.429936},
  {18.628,35.0593,-91.7815,0.573248,0.573248,0.573248},
  {19.1193,26.6052,-94.4808,0.490446,0.490446,0.490446},
  {15.0953,54.6017,-82.4063,0.475584,0.475584,0.475584},
  {17.0638,-25.5085,-95.1743,0.467091,0.467091,0.467091},
  {9.83465,-81.4025,-57.2443,0.432059,0.432059,0.432059},
  {15.4118,18.2736,-97.1007,0.426752,0.426752,0.426752},
  {4.39278,93.5636,-35.0223,0.437367,0.437367,0.437367},
  {9.98255,-52.7813,-84.3474,0.469214,0.469214,0.469214},
  {4.7856,-89.1163,-45.1153,0.512739,0.512739,0.512739},
  {4.08775,74.0564,-67.0742,0.451168,0.451168,0.451168},
  {2.47962,-91.8644,-39.4308,0.458599,0.458599,0.458599},
  {4.61817,25.1595,-96.673,0.617834,0.617834,0.617834},
  {4.05703,3.07953,-99.8702,0.461783,0.461783,0.461783},
  {1.59381,80.5874,-59.1873,0.585987,0.585987,0.585987},
  {-2.30833,-77.4089,-63.2656,0.569002,0.569002,0.569002},
  {-4.07671,-38.4613,-92.2177,0.526539,0.526539,0.526539},
  {-3.10285,-79.1955,-60.979,0.424628,0.424628,0.424628},
  {-3.4258,-85.4902,-51.7657,0.547771,0.547771,0.547771},
  {-3.66008,83.8979,-54.2931,0.493631,0.493631,0.493631},
  {-6.57101,-30.1404,-95.123,0.570064,0.570064,0.570064},
  {-8.67442,-1.16401,-99.6163,0.432059,0.432059,0.432059},
  {-4.59675,-86.9505,-49.1781,0.463907,0.463907,0.463907},
  {-5.2392,-89.1791,-44.9404,0.703822,0.703822,0.703822},
  {-5.24111,-89.1793,-44.9397,0.661359,0.661359,0.661359},
  {-7.80706,-76.8625,-63.4918,0.429936,0.429936,0.429936},
  {-12.4581,-28.4276,-95.0614,0.531847,0.531847,0.531847},
  {-7.36086,-83.9746,-53.7967,0.671975,0.671975,0.671975},
  {-4.33179,-95.1772,-30.3734,0.434183,0.434183,0.434183},
  {-13.7187,-39.7094,-90.7467,0.563694,0.563694,0.563694},
  {-5.02961,93.8423,-34.1809,0.434183,0.434183,0.434183},
  {-5.7531,-93.4426,-35.1481,0.559448,0.559448,0.559448},
  {-10.843,-74.9431,-65.3143,0.435244,0.435244,0.435244},
  {-11.8292,-75.4248,-64.5846,0.61465,0.61465,0.61465},
  {-18.0719,-2.52949,-98.3209,0.457537,0.457537,0.457537},
  {-18.0719,-2.52949,-98.3209,0.454352,0.454352,0.454352},
  {-7.47848,-92.7889,-36.528,0.521231,0.521231,0.521231},
  {-10.433,-86.3295,-49.3798,0.712314,0.712314,0.712314},
  {-13.0744,82.8644,-54.4293,0.657113,0.657113,0.657113},
  {-23.982,5.92628,-96.9007,0.4862,0.4862,0.4862},
  {-18.9901,62.003,-76.1249,0.537155,0.537155,0.537155},
  {-8.49389,-94.8594,-30.4885,0.460722,0.460722,0.460722},
  {-26.3094,19.0109,-94.5854,0.544586,0.544586,0.544586},
  {-31.0361,-39.3487,-86.5358,0.526539,0.526539,0.526539},
  {-27.614,-59.7796,-75.2586,0.553079,0.553079,0.553079},
  {-20.5759,81.8403,-53.6544,0.604034,0.604034,0.604034},
  {-20.5812,81.8367,-53.6579,0.42569,0.42569,0.42569},
  {-35.6354,-19.3573,-91.4079,0.740977,0.740977,0.740977},
  {-29.8932,-63.4832,-71.248,0.433121,0.433121,0.433121},
  {-40.15,-1.03991,-91.58,0.487261,0.487261,0.487261},
  {-25.1315,-80.3508,-53.9644,0.600849,0.600849,0.600849},
  {-34.3393,-66.5071,-66.3145,0.483015,0.483015,0.483015},
  {-29.48,75.8286,-58.146,0.647558,0.647558,0.647558},
  {-33.9484,-67.5254,-65.4815,0.522293,0.522293,0.522293},
  {-32.7659,-73.4776,-59.3922,0.57431,0.57431,0.57431},
  {-45.5261,31.5612,-83.2542,0.56051,0.56051,0.56051},
  {-36.6127,-67.0437,-64.5341,0.438429,0.438429,0.438429},
  {-35.1218,-70.4679,-61.6501,0.434183,0.434183,0.434183},
  {-25.4283,-86.9263,-42.3937,0.780255,0.780255,0.780255},
  {-46.8092,-44.9046,-76.1083,0.497877,0.497877,0.497877},
  {-42.2763,-59.2997,-68.5292,0.626327,0.626327,0.626327},
  {-22.3364,90.165,-37.0318,0.457537,0.457537,0.457537},
  {-52.6988,32.8578,-78.3786,0.849257,0.849257,0.849257},
  {-39.6542,-72.004,-56.9471,0.468153,0.468153,0.468153},
  {-53.0667,50.5603,-68.0263,0.464968,0.464968,0.464968},
  {-48.3312,61.9893,-61.8176,0.523355,0.523355,0.523355},
  {-46.5274,-67.1174,-57.7101,0.599788,0.599788,0.599788},
  {-31.259,-87.3222,-37.3859,0.846072,0.846072,0.846072},
  {-31.2593,-87.3225,-37.3851,0.703822,0.703822,0.703822},
  {-27.5433,-90.6125,-32.1051,0.506369,0.506369,0.506369},
  {-43.9536,-73.5959,-51.4949,0.600849,0.600849,0.600849},
  {-12.7077,-98.1776,-14.1307,0.438429,0.438429,0.438429},
  {-51.3104,-61.2819,-60.098,0.420382,0.420382,0.420382},
  {-64.9777,-9.85961,-75.3704,0.433121,0.433121,0.433121},
  {-58.7055,45.5143,-66.9486,0.558386,0.558386,0.558386},
  {-66.3069,3.30292,-74.7829,0.450106,0.450106,0.450106},
  {-65.1994,-27.6336,-70.6075,0.553079,0.553079,0.553079},
  {-18.5072,96.2006,-20.0728,0.624204,0.624204,0.624204},
  {-51.27,-68.3706,-51.9312,0.56051,0.56051,0.56051},
  {-52.2699,-67.0481,-52.6537,0.512739,0.512739,0.512739},
  {-54.3119,64.7994,-53.3971,0.473461,0.473461,0.473461},
  {-65.0631,-42.7073,-62.7923,0.495754,0.495754,0.495754},
  {-48.9983,-75.1699,-44.1435,0.434183,0.434183,0.434183},
  {-45.7022,-78.9075,-41.0477,0.483015,0.483015,0.483015},
  {-27.7408,-93.1561,-23.503,0.538217,0.538217,0.538217},
  {-62.9496,54.9238,-54.9612,0.476645,0.476645,0.476645},
  {-74.7448,-16.3034,-64.4004,0.567941,0.567941,0.567941},
  {-58.4131,-65.1403,-48.4216,0.503185,0.503185,0.503185},
  {-62.1749,-59.147,-51.3412,0.467091,0.467091,0.467091},
  {-54.9913,-70.3264,-45.0573,0.487261,0.487261,0.487261},
  {-23.9468,95.0157,-19.9644,0.521231,0.521231,0.521231},
  {-40.1967,85.6863,-32.2814,0.495754,0.495754,0.495754},
  {-68.81,48.6424,-53.8433,0.454352,0.454352,0.454352},
  {-60.7362,-65.8255,-44.4759,0.549894,0.549894,0.549894},
  {-78.1039,-25.5268,-56.9926,0.429936,0.429936,0.429936},
  {-79.2341,18.2879,-58.2023,0.441614,0.441614,0.441614},
  {-79.2337,18.2908,-58.202,0.441614,0.441614,0.441614},
  {-71.9642,44.9549,-52.917,0.60828,0.60828,0.60828},
  {-71.5734,-47.1551,-51.5136,0.464968,0.464968,0.464968},
  {-70.8059,-49.6637,-50.1999,0.456476,0.456476,0.456476},
  {-74.0489,44.3002,-50.5396,0.437367,0.437367,0.437367},
  {-82.448,11.1912,-55.4715,0.563694,0.563694,0.563694},
  {-80.4298,26.5925,-53.1403,0.455414,0.455414,0.455414},
  {-84.099,-5.98339,-53.7732,0.470276,0.470276,0.470276},
  {-70.4121,-55.3787,-44.4447,0.42569,0.42569,0.42569},
  {-84.2721,7.80724,-53.266,0.451168,0.451168,0.451168},
  {-38.253,-89.4393,-23.1802,0.542463,0.542463,0.542463},
  {-74.9867,-48.8075,-44.6634,0.433121,0.433121,0.433121},
  {-82.6322,26.9956,-49.4283,0.436306,0.436306,0.436306},
  {-77.5361,-44.0161,-45.285,0.538217,0.538217,0.538217},
  {-67.8935,-62.1106,-39.1503,0.483015,0.483015,0.483015},
  {-80.0068,-38.4644,-46.0369,0.598726,0.598726,0.598726},
  {-77.793,43.712,-45.1388,0.632696,0.632696,0.632696},
  {-82.5727,-33.8829,-45.0965,0.566879,0.566879,0.566879},
  {-82.3818,-35.2971,-44.3549,0.424628,0.424628,0.424628},
  {-39.7955,-89.6375,-19.5305,0.436306,0.436306,0.436306},
  {-89.3744,-6.44355,-44.3926,0.55414,0.55414,0.55414},
  {-90.0161,-8.1808,-42.7805,0.501062,0.501062,0.501062},
  {-81.9353,-43.1972,-37.6911,0.538217,0.538217,0.538217},
  {-62.5668,72.3128,-29.2618,0.432059,0.432059,0.432059},
  {-85.9454,32.8093,-39.2038,0.446921,0.446921,0.446921},
  {-17.9,-98.1283,-7.10067,0.432059,0.432059,0.432059},
  {-43.5701,87.8935,-19.4005,0.55414,0.55414,0.55414},
  {-82.6411,-44.5135,-34.482,0.7431,0.7431,0.7431},
  {-85.9996,36.6334,-35.5255,0.550955,0.550955,0.550955},
  {-92.4843,3.46185,-37.8771,0.43949,0.43949,0.43949},
  {-82.2483,-47.2799,-31.6202,0.545648,0.545648,0.545648},
  {-91.9516,-18.3389,-34.7647,0.573248,0.573248,0.573248},
  {-80.1958,52.4031,-28.68,0.546709,0.546709,0.546709},
  {-34.0717,-93.3754,-10.9614,0.641189,0.641189,0.641189},
  {-73.4385,62.8265,-25.683,0.470276,0.470276,0.470276},
  {-49.0465,-85.7539,-15.5149,0.44586,0.44586,0.44586},
  {-78.8103,-56.343,-24.7871,0.601911,0.601911,0.601911},
  {-75.2959,-61.6315,-23.067,0.518047,0.518047,0.518047},
  {-75.3734,-61.5902,-22.9236,0.46603,0.46603,0.46603},
  {-70.9013,-67.3805,-20.8058,0.460722,0.460722,0.460722},
  {-53.9393,-82.8943,-14.8017,0.512739,0.512739,0.512739},
  {-95.0377,16.2895,-26.5045,0.505308,0.505308,0.505308},
  {-82.8879,51.3936,-22.0974,0.428875,0.428875,0.428875},
  {-94.0101,-27.1016,-20.6788,0.587049,0.587049,0.587049},
  {-71.2682,-68.5045,-15.0987,0.491507,0.491507,0.491507},
  {-40.1054,91.1509,-9.11418,0.508493,0.508493,0.508493},
  {-94.9722,24.8526,-19.0427,0.475584,0.475584,0.475584},
  {-89.0078,42.0073,-17.692,0.511677,0.511677,0.511677},
  {-78.5283,59.9152,-15.6036,0.509554,0.509554,0.509554},
  {-89.3889,-42.2609,-14.9546,0.497877,0.497877,0.497877},
  {-55.95,-82.4423,-8.53657,0.542463,0.542463,0.542463},
  {-54.7418,-83.2704,-8.32942,0.490446,0.490446,0.490446},
  {-48.574,-87.1932,-6.15821,0.460722,0.460722,0.460722},
  {-78.9053,-60.5931,-10.1208,0.559448,0.559448,0.559448},
  {-63.9585,-76.4653,-7.89797,0.531847,0.531847,0.531847},
  {-79.2261,-60.3262,-9.16385,0.671975,0.671975,0.671975},
  {-60.6426,79.1238,-7.86713,0.548832,0.548832,0.548832},
  {-72.7802,-68.197,-7.22594,0.646497,0.646497,0.646497},
  {-97.0237,21.7462,-10.6537,0.624204,0.624204,0.624204},
  {-95.9495,-26.5533,-9.41357,0.469214,0.469214,0.469214},
  {-77.455,-62.9727,-5.92981,0.589172,0.589172,0.589172},
  {-42.6154,-90.4261,-2.65624,0.460722,0.460722,0.460722},
  {-69.1792,71.9417,-6.21516,0.441614,0.441614,0.441614},
  {-99.4234,7.96287,-7.18199,0.550955,0.550955,0.550955},
  {-76.3497,-64.4483,-4.13996,0.523355,0.523355,0.523355},
  {-88.3682,46.516,-5.22747,0.481953,0.481953,0.481953},
  {-99.7491,4.72324,-5.27418,0.446921,0.446921,0.446921},
  {-79.7399,-60.2419,-3.5309,0.504246,0.504246,0.504246},
  {-54.6282,83.7459,-1.543,0.446921,0.446921,0.446921},
  {-79.589,60.5302,-1.30123,0.435244,0.435244,0.435244},
  {-98.5477,-16.9756,-0.418532,0.490446,0.490446,0.490446},
  {-87.2473,48.8587,-0.850866,0.452229,0.452229,0.452229},
  {-62.2598,78.2487,-0.9219,0.60828,0.60828,0.60828},
  {-99.8687,5.11449,0.281065,0.423567,0.423567,0.423567},
  {-64.1293,-76.7072,1.85617,0.456476,0.456476,0.456476},
  {-86.2023,-50.6398,2.18515,0.527601,0.527601,0.527601},
  {-98.5594,16.6147,3.16193,0.449045,0.449045,0.449045},
  {-87.6147,48.118,2.88479,0.438429,0.438429,0.438429},
  {-93.1529,-35.9327,5.60093,0.435244,0.435244,0.435244},
  {-79.8764,-59.8488,6.1555,0.514862,0.514862,0.514862},
  {-86.3884,-49.7399,7.93608,0.558386,0.558386,0.558386},
  {-99.4406,-5.05736,9.27297,0.498938,0.498938,0.498938},
  {-82.0678,-56.4747,8.68783,0.64862,0.64862,0.64862},
  {-92.3722,37.0877,9.58579,0.437367,0.437367,0.437367},
  {-69.0248,-71.8956,8.16147,0.472399,0.472399,0.472399},
  {-89.6455,-42.9277,10.9952,0.546709,0.546709,0.546709},
  {-29.5577,95.4931,2.72334,0.46603,0.46603,0.46603},
  {-97.8012,-14.3392,15.1433,0.436306,0.436306,0.436306},
  {-76.9414,62.6381,12.5095,0.841826,0.841826,0.841826},
  {-87.3456,-45.3848,17.6344,0.508493,0.508493,0.508493},
  {-81.5344,54.9938,18.1056,0.478769,0.478769,0.478769},
  {-87.0573,-44.3019,21.4095,0.630573,0.630573,0.630573},
  {-90.3471,-36.0105,23.2518,0.472399,0.472399,0.472399},
  {-81.3929,54.0085,21.4074,0.501062,0.501062,0.501062},
  {-83.4912,-49.8189,23.3943,0.569002,0.569002,0.569002},
  {-89.2113,-37.0422,25.8691,0.444798,0.444798,0.444798},
  {-84.8123,-46.4387,25.5014,0.492569,0.492569,0.492569},
  {-93.1596,23.9607,27.3345,0.527601,0.527601,0.527601},
  {-95.5033,-8.51126,28.402,0.47983,0.47983,0.47983},
  {-89.052,-35.8753,27.9769,0.538217,0.538217,0.538217},
  {-36.1187,92.4956,11.8325,0.519108,0.519108,0.519108},
  {-56.3216,80.2491,19.6969,0.444798,0.444798,0.444798},
  {-89.2071,-30.648,33.2083,0.427813,0.427813,0.427813},
  {-70.8804,-65.0988,27.1682,0.423567,0.423567,0.423567},
  {-92.9842,5.43354,36.3925,0.488323,0.488323,0.488323},
  {-81.4974,46.8851,34.0584,0.518047,0.518047,0.518047},
  {-57.253,78.5098,23.6287,0.442675,0.442675,0.442675},
  {-87.9126,18.418,43.9562,0.556263,0.556263,0.556263},
  {-63.2769,70.872,31.1961,0.54034,0.54034,0.54034},
  {-84.594,31.787,42.8188,0.43949,0.43949,0.43949},
  {-87.4842,15.4164,45.9221,0.76327,0.76327,0.76327},
  {-88.184,1.75494,47.1222,0.430998,0.430998,0.430998},
  {-30.071,94.1281,15.3506,0.438429,0.438429,0.438429},
  {-25.4114,-95.5847,14.7589,0.424628,0.424628,0.424628},
  {-87.0597,11.1585,47.9176,0.451168,0.451168,0.451168},
  {-71.5185,57.4767,39.7685,0.432059,0.432059,0.432059},
  {-81.8936,33.3679,46.6908,0.476645,0.476645,0.476645},
  {-34.179,-91.4832,21.5084,0.467091,0.467091,0.467091},
  {-84.0232,-1.43354,54.2038,0.502123,0.502123,0.502123},
  {-57.2063,72.8268,37.7319,0.442675,0.442675,0.442675},
  {-55.8664,73.9801,37.4953,0.422505,0.422505,0.422505},
  {-80.4319,-21.7202,55.3077,0.46603,0.46603,0.46603},
  {-78.9587,-25.5132,55.8086,0.518047,0.518047,0.518047},
  {-44.1425,-83.6143,32.5591,0.639066,0.639066,0.639066},
  {-62.0856,64.6213,44.3786,0.611465,0.611465,0.611465},
  {-52.4185,-73.4813,43.0442,0.514862,0.514862,0.514862},
  {-74.7931,25.199,61.4085,0.45966,0.45966,0.45966},
  {-73.7667,27.416,61.6995,0.444798,0.444798,0.444798},
  {-30.3413,-91.4981,26.5989,0.481953,0.481953,0.481953},
  {-53.6183,71.0557,45.5649,0.712314,0.712314,0.712314},
  {-62.0622,55.8763,55.0102,0.583864,0.583864,0.583864},
  {-73.3892,-16.4976,65.8927,0.444798,0.444798,0.444798},
  {-35.4439,88.1624,31.1629,0.480892,0.480892,0.480892},
  {-37.8227,-85.2222,36.1473,0.457537,0.457537,0.457537},
  {-53.8829,65.8259,52.5698,0.426752,0.426752,0.426752},
  {-49.8193,69.3751,52.0109,0.450106,0.450106,0.450106},
  {-57.5525,50.3426,64.4463,0.505308,0.505308,0.505308},
  {-51.9787,61.6288,59.1617,0.450106,0.450106,0.450106},
  {-65.3888,9.1463,75.1042,0.428875,0.428875,0.428875},
  {-29.8134,88.7699,35.0864,0.585987,0.585987,0.585987},
  {-57.3389,-38.1254,72.5169,0.447983,0.447983,0.447983},
  {-60.0503,-9.70811,79.3707,0.536093,0.536093,0.536093},
  {-20.414,94.2995,26.2844,0.502123,0.502123,0.502123},
  {-12.4066,-97.5879,17.9634,0.44586,0.44586,0.44586},
  {-54.9179,-28.6729,78.4977,0.454352,0.454352,0.454352},
  {-55.0243,17.1499,81.7203,0.591295,0.591295,0.591295},
  {-52.6576,-27.7771,80.3468,0.54034,0.54034,0.54034},
  {-41.5486,-60.689,67.7536,0.525478,0.525478,0.525478},
  {-47.7982,-0.558018,87.8352,0.530786,0.530786,0.530786},
  {-31.9798,-73.0891,60.2933,0.660297,0.660297,0.660297},
  {-42.7721,42.8068,79.6124,0.44586,0.44586,0.44586},
  {-45.828,10.7961,88.2227,0.470276,0.470276,0.470276},
  {-24.1568,84.9903,46.8305,0.489384,0.489384,0.489384},
  {-21.2583,-86.8283,44.8211,0.541401,0.541401,0.541401},
  {-41.5934,-2.42092,90.9072,0.437367,0.437367,0.437367},
  {-27.9743,-68.8298,66.932,0.423567,0.423567,0.423567},
  {-20.2181,85.1867,48.3162,0.446921,0.446921,0.446921},
  {-24.1205,76.9204,59.173,0.444798,0.444798,0.444798},
  {-33.0034,18.7919,92.5075,0.484076,0.484076,0.484076},
  {-22.6267,-72.998,64.4928,0.622081,0.622081,0.622081},
  {-28.4875,50.3343,81.5777,0.532909,0.532909,0.532909},
  {-28.8825,39.9798,86.9909,0.42569,0.42569,0.42569},
  {-19.1699,-78.0615,59.4888,0.474522,0.474522,0.474522},
  {-27.3396,41.6307,86.7146,0.475584,0.475584,0.475584},
  {-12.1883,91.4964,38.469,0.471338,0.471338,0.471338},
  {-28.7272,-13.1906,94.8723,0.447983,0.447983,0.447983},
  {-27.0639,-27.263,92.327,0.497877,0.497877,0.497877},
  {-23.3584,-49.4279,83.7333,0.721868,0.721868,0.721868},
  {-18.5357,67.335,71.5712,0.460722,0.460722,0.460722},
  {-21.428,47.0747,85.5851,0.58811,0.58811,0.58811},
  {-23.034,26.2278,93.7099,0.580679,0.580679,0.580679},
  {-20.4027,-36.1177,90.9903,0.456476,0.456476,0.456476},
  {-15.1306,-71.0144,68.7606,0.430998,0.430998,0.430998},
  {-9.72202,-85.0222,51.7369,0.421444,0.421444,0.421444},
  {-18.5512,5.72543,98.0973,0.453291,0.453291,0.453291},
  {-15.1073,-34.3669,92.686,0.423567,0.423567,0.423567},
  {-6.73281,72.487,68.5587,0.43949,0.43949,0.43949},
  {-1.92753,97.6794,21.3312,0.504246,0.504246,0.504246}
};

void CreateStars(XYZ c,double radius) 
{
	int i;
	XYZ p;
	
	glBegin(GL_POINTS);
	for (i=0;i<518;i++) {
      glColor3f(starsmag4[i].r,starsmag4[i].g,starsmag4[i].b);
		p.x = c.x + radius * starsmag4[i].x / 100;
      p.y = c.y + radius * starsmag4[i].y / 100;
      p.z = c.z + radius * starsmag4[i].z / 100;
      glVertex3f(p.x,p.y,p.z);
   }
   glEnd();
}

/*
	Create a random star field, normally this would be put in a gllist
	Create 1 and 2 point stars of varying intensity
*/
void CreateRandomStars(int n)
{
   int i,j;
   double grey;
   XYZ p;

#ifndef WIN32
   srand(12345L);
#else
	srand48(12345L);
#endif
   for (j=1;j<=2;j++) {
      glPointSize(j);
      glBegin(GL_POINTS);
      for (i=0;i<n/2;i++) {
#ifndef WIN32
	p.x = rand() / (double)RAND_MAX - 0.5;
	p.y = rand() / (double)RAND_MAX - 0.5;
	p.z = rand() / (double)RAND_MAX - 0.5;
	grey = rand()/ (double)RAND_MAX / 2 + 0.5;
#else
			p.x = rand() / (double)RAND_MAX - 0.5;
         p.y = rand() / (double)RAND_MAX - 0.5;
         p.z = rand() / (double)RAND_MAX - 0.5;
			grey = (rand() / (double)RAND_MAX) / 2 + 0.5;
#endif
			if (Modulus(p) < 0.01)
				continue;
			Normalise(&p);
         glColor3f(grey,grey,grey);
         glVertex3f(1000*p.x,1000*p.y,1000*p.z);
      }
      glEnd();
   }
}

/*
   Read a RAW texture file and return a pointer to the pixelmap
	Always return a texture unless the memory allocation fails.
	Return random noise if the file reading fails.
	The alpha value is set to 255 (opaque) unless the "trans" argument 
	is 't' then treat the colour "c" as fully tranparent, alpha = 0
*/
BITMAP4 *ReadRawTexture(int w,int h,char *fname,COLOUR c,int trans)
{
   int r,g,b;
   int i;
   double dr,dg,db,deltac = 1.0 / 255;
   FILE *fptr;
   BITMAP4 *ptr;

   /* Allocate memory for the texture */
   if ((ptr = (BITMAP4 *)malloc(w*h*sizeof(BITMAP4))) == NULL) {
      fprintf(stderr,"Failed to allocate memory for texture \"%s\"\n",fname);
      exit(-1);
   }

   /* Start off with a random texture, totally opaque */
   for (i=0;i<w*h;i++) {
      ptr[i].r = rand() % 255;
      ptr[i].g = rand() % 255;
      ptr[i].b = rand() % 255;
      ptr[i].a = 255;
   }

   /* Try to open the texture file */
   if ((fptr = fopen(fname,"rb")) == NULL) {
      fprintf(stderr,"Failed to open texture file \"%s\"\n",fname);
      return(ptr);
   }

   /* Actually read the texture */
   for (i=0;i<w*h;i++) {
      if ((r = fgetc(fptr)) != EOF &&
          (g = fgetc(fptr)) != EOF &&
          (b = fgetc(fptr)) != EOF) {
         ptr[i].r = r;
         ptr[i].g = g;
         ptr[i].b = b;

			/* Deal with transparency flag */
         if (trans == 't') {
            dr = r / 255.0 - c.r;
            dg = g / 255.0 - c.g;
            db = b / 255.0 - c.b;
            if (ABS(dr) < deltac && ABS(dg) < deltac && ABS(db) < deltac) 
               ptr[i].a = 0;
         }
      } else {
         /* Encountered short texture file */
         break;
      }
   }

   fclose(fptr);
   return(ptr);
}

/*
	As per ReadRawTexture() except the intensity is used as the
	transparency (alpha value).
*/
BITMAP4 *ReadRawTexture2(int w,int h,char *fname,int trans)
{
   int r,g,b;
   int i;
   double intensity;
   FILE *fptr;
   BITMAP4 *ptr;

   /* Allocate memory for the texture */
   if ((ptr = (BITMAP4 *)malloc(w*h*sizeof(BITMAP4))) == NULL) {
      fprintf(stderr,"Failed to allocate memory for texture \"%s\"\n",fname);
      exit(-1);
   }

   /* Start off with a random texture, totally opaque */
   for (i=0;i<w*h;i++) {
      ptr[i].r = rand() % 255;
      ptr[i].g = rand() % 255;
      ptr[i].b = rand() % 255;
      ptr[i].a = 255;
   }

   /* Try to open the texture file */
   if ((fptr = fopen(fname,"rb")) == NULL) {
      fprintf(stderr,"Failed to open texture file \"%s\"\n",fname);
      return(ptr);
   }

   /* Actually read the texture */
   for (i=0;i<w*h;i++) {
      if ((r = fgetc(fptr)) != EOF &&
          (g = fgetc(fptr)) != EOF &&
          (b = fgetc(fptr)) != EOF) {
         ptr[i].r = r;
         ptr[i].g = g;
         ptr[i].b = b;

         /* Deal with transparency flag */
         if (trans == 't') {
				intensity = sqrt((double)r*r+g*g+b*b) / SQRT3;
            ptr[i].a = intensity;
         }
      } else {
         /* Encountered short texture file */
         break;
      }
   }

   fclose(fptr);
   return(ptr);
}

/*
   Read a PPM texture file and return a pointer to the pixelmap
	and the size of the image.
   Always return a texture unless the memory allocation fails.
   Return random noise if the file reading fails.
	1. The transparency is related to te intensity of the pixel
	2. The alpha value is set to 255 (opaque) unless the "trans" argument
      is 't' then treat the colour "c" as fully tranparent, alpha = 0;
*/
BITMAP4 *ReadPPMTexture(char *fname,COLOUR c,int trans,int *w,int *h,int tmode)
{
   int r,g,b;
   int i;
   double intensity,dr,dg,db,deltac = 1.0 / 255;
   FILE *fptr;
	char aline[256];
   BITMAP4 *ptr;

   /* Allocate memory for the texture */
	*w = 64;
	*h = 64;
	if ((ptr = (BITMAP4 *)malloc((*w)*(*h)*sizeof(BITMAP4))) == NULL) {
      fprintf(stderr,"Failed to allocate memory for texture \"%s\"\n",fname);
      exit(-1);
   }

   /* Start off with a random texture, totally opaque */
   for (i=0;i<(*w)*(*h);i++) {
      ptr[i].r = rand() % 255;
      ptr[i].g = rand() % 255;
      ptr[i].b = rand() % 255;
      ptr[i].a = 255;
   }

   /* Try to open the texture file */
   if ((fptr = fopen(fname,"rb")) == NULL) {
      fprintf(stderr,"Failed to open texture file \"%s\"\n",fname);
      return(ptr);
   }

	/* Read the PPM header */
	for (i=0;i<3;i++) {
		if (!ReadLine(fptr,aline,250))
			break;
		if (aline[0] == '#')
			i--;
		if (i == 1)
			sscanf(aline,"%d %d",w,h);
	}

   /* Allocate memory for the texture */
	if ((ptr = (BITMAP4 *)realloc(ptr,(*w)*(*h)*sizeof(BITMAP4))) == NULL) {
      fprintf(stderr,"Failed to allocate memory for texture \"%s\"\n",fname);
      exit(-1);
   }

   /* Start off with a random texture, totally opaque */
   for (i=0;i<(*w)*(*h);i++) {
      ptr[i].r = rand() % 255;
      ptr[i].g = rand() % 255;
      ptr[i].b = rand() % 255;
      ptr[i].a = 255;
   }

   /* Actually read the texture */
   for (i=0;i<(*w)*(*h);i++) {
      if ((r = fgetc(fptr)) != EOF &&
          (g = fgetc(fptr)) != EOF &&
          (b = fgetc(fptr)) != EOF) {
         ptr[i].r = r;
         ptr[i].g = g;
         ptr[i].b = b;

         /* Deal with transparency flag */
         if (trans == 't') {
				if (tmode == 1) {
		         intensity = sqrt((double)r*r+g*g+b*b) / SQRT3;
            	ptr[i].a = intensity;
				} else {
            	dr = r / 255.0 - c.r;
            	dg = g / 255.0 - c.g;
            	db = b / 255.0 - c.b;
            	if (ABS(dr) < deltac && ABS(dg) < deltac && ABS(db) < deltac)
               	ptr[i].a = 0;
				}
         }
      } else {
         /* Encountered short texture file */
         break;
      }
   }

   fclose(fptr);
   return(ptr);
}

/*
	Draw some text on the screen in a particular colour
	Coordinates are 0 to screen width and 0 to screenheight
	This particular implementation uses a 8 by 13 nonproprotional font
	because that's what the GLUT implementation of s2win does.
*/
void DrawGLText(int x,int y,char *s)
{
#if defined(S2_NO_S2WIN)
  fprintf(stderr, "DrawGLText not implemented on this platform!\n");
#else
   int lines;
   char *p;

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(0,s2winGet(S2_WINDOW_WIDTH),0,s2winGet(S2_WINDOW_HEIGHT),-1,1); 
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   glRasterPos2i(x,y);
   for (p=s,lines=0;*p;p++) {
      if (*p == '\n' || *p == '\r') {
         lines++;
         glRasterPos2i(x,y-(lines*13));
      } else {
      	s2winBitmapCharacter(p);
		}
   }

   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
#endif
}


/*
	Draw a 3D plus sign
*/
void Draw3DPlus(XYZ p,double size)
{
   glPushMatrix();
   //glLoadName(-1);
   glTranslatef(p.x,p.y,p.z);
   glScalef(size,size,size);
   glBegin(GL_LINES);
   //glColor3f(1.0,1.0,1.0);
   glVertex3f(-1.0,0.0,0.0); /* x */
   glVertex3f(+1.0,0.0,0.0);
   glVertex3f(0.0,-1.0,0.0); /* y */
   glVertex3f(0.0,+1.0,0.0);
   glVertex3f(0.0,0.0,-1.0); /* z */
   glVertex3f(0.0,0.0,+1.0);
   glEnd();
   glPopMatrix();
}

/*
	Draw a generic set of axes
	p1 and p2 is the bounding box
	nticks is the number of ticks on each axis
	Optionally draw the planes
*/
void DrawAxes(XYZ p1,XYZ p2,int nticks,int planes)
{
   int i;
   double dx,dy,dz,dd;

   /* Choose the tick size */
   dx = p2.x - p1.x;
   dy = p2.y - p1.y;
   dz = p2.z - p1.z;
   dd = (dx < dy ? dx : dy);
   dd = (dd < dz ? dd : dz);
   dd /= 50;

   glBegin(GL_LINES);

   /* X axis */
   glColor3f(1.0,0.0,0.0);
   glVertex3f(p1.x,p1.y,p1.z);
   glVertex3f(p2.x,p1.y,p1.z);
   for (i=0;i<=nticks;i++) {
      glVertex3f(p1.x+i*dx/nticks,p1.y,   p1.z);
      glVertex3f(p1.x+i*dx/nticks,p1.y-dd,p1.z-dd);
   }

   /* Y axis */
   glColor3f(0.0,1.0,0.0);
   glVertex3f(p1.x,p1.y,p1.z);
   glVertex3f(p1.x,p2.y,p1.z);
   for (i=0;i<=nticks;i++) {
      glVertex3f(p1.x,   p1.y+i*dy/nticks,p1.z);
      glVertex3f(p1.x-dd,p1.y+i*dy/nticks,p1.z-dd);
   }

   /* Z axis */
   glColor3f(0.0,0.0,1.0);
   glVertex3f(p1.x,p1.y,p1.z);
   glVertex3f(p1.x,p1.y,p2.z);
   for (i=0;i<=nticks;i++) {
      glVertex3f(p1.x,   p1.y,   p1.z+i*dz/nticks);
      glVertex3f(p1.x-dd,p1.y-dd,p1.z+i*dz/nticks);
   }

	/* Optionally draw the axis planes */
	if (planes) {
		glColor3f(0.5,0.5,0.5);

		/* x-z */
		for (i=0;i<=nticks;i++) {
			glVertex3f(p1.x+i*dx/nticks,p1.y,p1.z);
			glVertex3f(p1.x+i*dx/nticks,p1.y,p2.z);
         glVertex3f(p1.x,p1.y,p1.z+i*dz/nticks);
         glVertex3f(p2.x,p1.y,p1.z+i*dz/nticks);
		}

		/* x-y */
      for (i=0;i<=nticks;i++) {
         glVertex3f(p1.x+i*dx/nticks,p1.y,p1.z);
         glVertex3f(p1.x+i*dx/nticks,p2.y,p1.z);
         glVertex3f(p1.x,p1.y+i*dy/nticks,p1.z);
         glVertex3f(p2.x,p1.y+i*dy/nticks,p1.z);
      }

		/* y-z */
      for (i=0;i<=nticks;i++) {
         glVertex3f(p1.x,p1.y+i*dy/nticks,p1.z);
         glVertex3f(p1.x,p1.y+i*dy/nticks,p2.z);
         glVertex3f(p1.x,p1.y,p1.z+i*dz/nticks);
         glVertex3f(p1.x,p2.y,p1.z+i*dz/nticks);
      }
	}

   glEnd();
}

/*
	Draw a 3D cursor 
	Based upon the camera position, view direction, and up vector
	It can be at a variable depth, most commonly the focal length
	This cursor should match the position of the 2D cursor when at focaldepth
*/
void Create3DCursor(int x,int y,int width,int height,
	XYZ vp,XYZ vd,XYZ vu,
	double aperture,double depth)
{
	XYZ right,p2,center;
	double thetah,thetav,tanthetah,tanthetav;
	double D,mu,denom,dx,dy,dd;

	/* Don't draw the cursor if it's near or off the window */
	if (x <= 1 || y <= 1 || x >= width-2 || y >= height-2)
		return;

	CROSSPROD(vd,vu,right);
	Normalise(&right);

	dx = 2 * (x - width/2) / (double)width;
	dy = 2 * (height/2 - y) / (double)height;

	thetav = aperture * DTOR;
	tanthetav = tan(thetav / 2);
	thetah = 2 * atan(width * tanthetav / height);
	tanthetah = tan(thetah / 2);

	/* A ray goes from vp to p2 */
	p2.x = vp.x + vd.x + dx * tanthetah * right.x + dy * tanthetav * vu.x;
	p2.y = vp.y + vd.y + dx * tanthetah * right.y + dy * tanthetav * vu.y;
	p2.z = vp.z + vd.z + dx * tanthetah * right.z + dy * tanthetav * vu.z;

	/* Find where this ray hits the plane at "depth" */
	D = -(vd.x * (vp.x + depth * vd.x) + 
		   vd.y * (vp.y + depth * vd.y) + 
		   vd.z * (vp.z + depth * vd.z));
   mu = vd.x * vp.x + vd.y * vp.y + vd.z * vp.z + D;
   denom = vd.x * (vp.x - p2.x) + vd.y * (vp.y - p2.y) + vd.z * (vp.z - p2.z);
	mu /= denom;

   center.x = vp.x + mu * (p2.x - vp.x);
   center.y = vp.y + mu * (p2.y - vp.y);
   center.z = vp.z + mu * (p2.z - vp.z);

	dd = depth / 10;

	glBegin(GL_LINES);
   glVertex3f(center.x-dd,center.y,center.z);
   glVertex3f(center.x+dd,center.y,center.z);
   glVertex3f(center.x,center.y-dd,center.z);
   glVertex3f(center.x,center.y+dd,center.z);
   glVertex3f(center.x,center.y,center.z-dd);
   glVertex3f(center.x,center.y,center.z+dd);
	glEnd();
}
