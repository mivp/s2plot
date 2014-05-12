/* s2skypa.c
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

#define NX 32
#define NY 32

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
   float tr[12];                                /* Transformation matrix */
   int walls = 1;				/* Should walls be drawn? */
   int lcolor = S2_PG_LTGREY;			/* Colour index for left-hand wall */
   int fcolor = S2_PG_DKGREY;			/* Automatic colour */

   s2opend("/?",argc,argv);                     /* Open the display */
   
   dx = (x2-x1)/(float)(NX-1);                  /* Spacing in X */
   dy = (y2-y1)/(float)(NY-1);                  /* Spacing in Y */

   s2swin(x1-0.5*dx,x2+0.5*dx,y1-0.5*dy,y2+0.5*dy,z1,z2);                   
						/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */

   dx = (x2-x1)/(float)(NX-1);                  /* Spacing in X */
   dy = (y2-y1)/(float)(NY-1);                  /* Spacing in Y */

   image = (float **)calloc(NX, sizeof(float *));      /* Allocate memory */
   for (i=0;i<NX;i++) {
      image[i] = (float *)calloc(NY, sizeof(float));  /* Allocate memory */
      x = i*dx + x1;
      for (j=0;j<NY;j++) {
         y = j*dy + y1;
         image[i][j] = -sqrt(x*x + y*y);		/* Data points */
	 image[i][j] += rand() / (float)RAND_MAX;
         minz = (image[i][j] < minz) ? image[i][j] : minz;
         maxz = (image[i][j] > maxz) ? image[i][j] : maxz;
      }
   }

   s2icm("rainbow", 1000, 1500);                /* Install colour map */
   s2scir(1000, 1500);                          /* Set colour range */

   /* Set up the transformation mapping from grid to world coordinatee */

   tr[0] = x1; tr[1] = (x2-x1)/(float)(NX-1); tr[2] = 0.0; tr[3] = 0.0;
   tr[4] = y1; tr[5] = 0.0; tr[6] = (y2-y1)/(float)(NY-1); tr[7] = 0.0;
   tr[8] = z1; tr[9] = 0.0;   tr[10] = 0.0;   tr[11] = 0.0;

   /* Draw the data mapped to the z-plane */
   s2surpa(image, NX, NY, 0, NX-1, 0, NY-1, minz, maxz, tr);
   
   tr[8] = z1 - minz * (z2 - z1) / (maxz - minz);	/* Fix up z minimum */
   tr[11] = (z2 - z1) / (maxz - minz);			/* Fix up z range */

   /* with automatic colored building walls:  */
   s2skypa(image, NX, NY, 0, (NX-1)/2, 0, NY-1, minz, maxz, tr, walls, -1, -1);

   /* with grey walls: idx_left and idx_right specified */
   s2skypa(image, NX, NY, (NX-1)/2+1, NX-1, 0, NY-1, minz, maxz, tr, walls, 
		lcolor, fcolor);

   s2show(1);					/* Show the S2PLOT window */

   return 1;

}

