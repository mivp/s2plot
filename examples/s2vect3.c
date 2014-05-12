/* s2vect3.c
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "s2plot.h"

#define NX 10					/* Dimensions of arrays */
#define NY 10
#define NZ 10
#define XLIMIT 1.0
#define YLIMIT 1.0
#define ZLIMIT 1.0

int main(int argc, char *argv[])
{
   float ***a, ***b, ***c;			/* Components of vectors */
   int i, j, k;					/* Loop variables */
   float x1 = -XLIMIT, x2 = +XLIMIT,		/* Range of plottable values */
         y1 = -YLIMIT, y2 = +YLIMIT,
         z1 = -ZLIMIT, z2 = +ZLIMIT;
   float minlength = +9E30;			/* Min and max vector lengths */
   float maxlength = -9E30;
   float tr[12];				/* Transformation matrix */
   float mindraw, scale;			/* See below */
   int colbylength, nc;				/* See below */
   float length;				/* Length of a vector */

   srand48((long)time(NULL));                   /* Seed random numbers */
   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(x1,x2, y1,y2, z1,z2);			/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */
   
   a = (float ***)calloc(NX, sizeof(float **));
   b = (float ***)calloc(NX, sizeof(float **));
   c = (float ***)calloc(NX, sizeof(float **));
   
   for (i=0;i < NX;i++) {
      a[i] = (float **)calloc(NY, sizeof(float **));
      b[i] = (float **)calloc(NY, sizeof(float **));
      c[i] = (float **)calloc(NY, sizeof(float **));
      for (j=0;j < NY;j++) {
         a[i][j] = (float *)calloc(NY, sizeof(float *));
         b[i][j] = (float *)calloc(NY, sizeof(float *));
         c[i][j] = (float *)calloc(NY, sizeof(float *));
         for (k=0;k < NZ;k++) {
             a[i][j][k] = drand48()*2.0 - 1.0;		/* X component */
             b[i][j][k] = drand48()*2.0 - 1.0;		/* Y component */
             c[i][j][k] = drand48()*2.0 - 1.0;		/* Z component */

             length = sqrt(a[i][j][k] * a[i][j][k] +
			   b[i][j][k] * b[i][j][k] +
			   c[i][j][k] * c[i][j][k]);
             maxlength = (length > maxlength) ? length : maxlength;
             minlength = (length < minlength) ? length : minlength;
         }
      }
   }
   
   tr[0] = x1; tr[1] = (x2-x1)/(float)(NX-1); tr[2] = 0.0; tr[3] = 0.0;
   tr[4] = y1; tr[5] = 0.0; tr[6] = (y2-y1)/(float)(NY-1); tr[7] = 0.0;
   tr[8] = z1; tr[9] = 0.0; tr[10]= 0.0; tr[11] = (z2-z1)/(float)(NZ-1); 

   s2icm("rainbow", 1000, 1500);		/* Install a colour map */
   s2scir(1000,1500);				/* Set the colour range */

   s2slw(2);					/* Set the line width */
   s2sch(2);					/* Set the size of arrowhead */
   s2sah(1, 30.0, 0.8);				/* Set the arrowhead type */
   scale = 0.2;					/* Size of vectors */
   nc = 0;					/* Put vector at coord point */
   colbylength = 1;				/* Use the colour map */
   mindraw = -1.0;				/* Draw them all! */

   s2vect3(a,b,c, NX,NY,NZ, 0,NX-1, 0,NY-1, 0,NZ-1, 
		scale, nc, tr, colbylength, mindraw, minlength, maxlength);

   s2show(1);
   return 1;
}
