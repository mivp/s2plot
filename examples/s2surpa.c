/* s2surpa.c
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
#include "s2plot.h"

#define XLIMIT 10.0
#define YLIMIT 10.0
#define ZLIMIT 10.0

#define NX 64
#define NY 64

int main(int argc, char *argv[])
{

   float x1 = -XLIMIT, x2 = XLIMIT;             /* x world coord range */
   float y1 = -YLIMIT, y2 = YLIMIT;             /* y world coord range */
   float z1 = -ZLIMIT, z2 = ZLIMIT;             /* z world coord range */
   int i, j;                                    /* Loop variables */
   float **image;                               /* 2D array for image */
   float minz = 9e30;                           /* Minimum value */
   float maxz = -9e30;                          /* Maximum value */
   float x, y, dx, dy;                          /* Temporary variables */
   float tr[8];                                 /* Transformation matrix */
   float tr2[12];                               /* Transformation matrix */

   s2opend("/?",argc,argv);                     /* Open the display */
   s2swin(x1,x2,y1,y2,z1,z2);                   /* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */

   dx = (x2-x1)/(float)(NX-1);                  /* Spacing in X */
   dy = (y2-y1)/(float)(NY-1);                  /* Spacing in Y */

   image = (float **)calloc(NX, sizeof(float *));      /* Allocate memory */
   for (i=0;i<NX;i++) {
      image[i] = (float *)calloc(NY, sizeof(float));  /* Allocate memory */
      x = i*dx + x1;
      for (j=0;j<NY;j++) {
         y = j*dy + y1;
         image[i][j] = sqrt(x*x + y*y);			/* Data points */
         minz = (image[i][j] < minz) ? image[i][j] : minz;
         maxz = (image[i][j] > maxz) ? image[i][j] : maxz;
      }
   }

  /* set up a unity transformation matrix */
   tr[0] = x1; tr[1] = (x2-x1)/(float)(NX-1); tr[2] = 0.0;
   tr[3] = y1; tr[4] = 0.0; tr[5] = (y2-y1)/(float)(NY-1);
   tr[6] = z1; tr[7] = (z2-z1)/(maxz-minz);

   s2icm("rainbow", 1000, 1500);                /* Install colour map */
   s2scir(1000, 1500);                          /* Set colour range */

   /* plot surface */
   s2surp(image, NX, NY, 0, NX-1, 0, NY-1, minz, maxz, tr);

   /* plot the same data, but on the base-level z plane, using the
    * more general s2surpa function.  */

   tr2[0] = tr[0]; tr2[1] = tr[1]; tr2[2]  = tr[2]; tr2[3]  = 0.0;
   tr2[4] = tr[3]; tr2[5] = tr[4]; tr2[6]  = tr[5]; tr2[7]  = 0.0;
   tr2[8] = z1;    tr2[9] = 0.0;   tr2[10] = 0.0;   tr2[11] = 0.0;

   /* new colormap and plot */
   s2surpa(image, NX, NY, 0, NX-1, 0, NY-1, minz, maxz, tr2);

   s2show(1);					/* Show the S2PLOT window */

   return 1;

}
