/* ns2sisa.c
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
#include <time.h>
#include <math.h>
#include "s2plot.h"

/* Global variabl */
int id; 					/* ID for isosurface object */

void cb(double *t, int *kc)
{
   static int count = 0;			/* Count number of times in */
   static float alpha = 0.0;			/* Isosurface alpha channel */
   int force = 1;				/* Force redraw of surface */
   float dummy;					/* A random number */
   char trans;					/* Isosurface transparency */

   if (count == 12) {				/* About once per half second */
      count = 0;				/* Reset counter */
      alpha = drand48();		 	/* Random alpha channel value */
      dummy = drand48(); 			/* Random number */
      if (dummy > 0.6) trans = 't';		/* Select transparency type */
      else if (dummy > 0.3) trans = 's';
      else trans = 'o';
      ns2sisa(id, alpha, trans);		/* Set the new isosurface values */
   } else {
      count++;					/* Keep counting */
   }
   ns2dis(id, force);				/* Draw the isosurface */
}


int main(int argc, char *argv[]) 
{
   int i, j, k;				/* Loop variables */
   float x, y, z;			/* Dummy variables for grid values */
   int nx, ny, nz;			/* Number of cells in grid */
   float ***grid;			/* Grid data */
   float tr[12];			/* Transformation matrix */
   int resolution;			/* Resolution of isosurface */
   float level;				/* Isosurface level to plot */
   float alpha;				/* Alpha channel */
   char trans;				/* Drawing mode for isosurface */

   srand48((long)time(NULL));          /* Seed random numbers */

   nx = 30; 				/* Grid dimensions */
   ny = 30;
   nz = 30;

  /* Create transpose matrix mapping data indices to world coords */
   tr[0] = tr[4] = tr[8] = 0.0;				  /* Offsets */
   tr[1] = tr[6] = tr[11] = 1.0; 			  /* Increments */
   tr[2] = tr[3] = tr[5] = tr[7] = tr[9] = tr[10] = 0.;   /* Cross terms */

   s2opend("/?",argc, argv);                    /* Open the display */
   s2swin(0, nx-1, 0, ny-1, 0, nz-1);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */

   /* allocate and generate the data grid */
   grid = (float ***)malloc(nx * sizeof(float **));
   for (i = 0; i < nx; i++) {
      grid[i] = (float **)malloc(ny * sizeof(float *));
      x = (float)(i) / (float)(nx - 1);
      for (j = 0; j < ny; j++) {
         grid[i][j] = (float *)malloc(nz * sizeof(float));
         y = (float)(j) / (float)(ny - 1);
         for (k = 0; k < nz; k++) {
	    z = (float)(k) / (float)(nz - 1);
	    grid[i][j][k] = x*x*x + y*y - z*z*z*z;
         }
      }
   }
  
   level      = 0.2;			/* Set the isosurface level */
   resolution = 1;			/* Set the isosurface resolution */
   alpha      = 0.0;			/* Set the alpha channel */
   trans      = 'o';			/* Opaque isosurface */

/* Create the isosurface object */
   id = ns2cis(grid, nx, ny, nz, 0, nx-1, 0, ny-1, 0, nz-1,
		   tr, level, resolution, trans, alpha, 1., 1., 0.);

   s2scb(&cb);				/* Install a dynamic callback */
   s2show(1);				/* Open the s2plot window */

   return 1;
}
