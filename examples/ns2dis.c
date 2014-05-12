/* ns2dis.c
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

#define CELLS 30
int id2; 				/* ID for isosurface object - Global */

void colorfn(float *x, float *y, float *z, float *r, float *g, float *b) 
/* Isosurface colouring function */
{
   *r = 0.2 + 0.5 * (*x) / (float)(CELLS - 1);
   *g = 0.0 + 0.8 * (*y) / (float)(CELLS - 1);
   *b = 0.6 + 0.2 * (*z) / (float)(CELLS - 1);
}

void cb(double *t, int *kc)
{
   static int count = 0;			/* Count number of times in */
   static float level = 0.0;			/* Isosurface level to plot */
   int force = 1;				/* Force redraw of surface */

   if (count == 25) {				/* About once per second */
      count = 0;				/* Reset counter */
      level += 0.1;				/* Increase isosurface level */
      if (level > 1.0) level = 0.0;
      ns2sisl(id2, level);			/* Set the new level */
   } else {
      count++;					/* Keep counting */
   }
   ns2dis(id2, force);				/* Draw isosurface */
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
   int id1; 				/* ID for isosurface object */

   nx = CELLS; 				/* Grid dimensions */
   ny = CELLS;
   nz = CELLS;

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
   alpha      = 0.9;			/* Set the alpha channel */
   trans      = 'o';			/* Opaque isosurface */

/* Create the isosurface object */
   id1 = ns2cis(grid, nx, ny, nz, 0, nx-1, 0, ny-1, 0, nz-1,
		   tr, level, resolution, trans, alpha, 1., 1., 0.);
   ns2dis(id1, 0);			/* Draw it */

   level      = 0.7;			/* Set the isosurface level */
   resolution = 6;			/* Set the isosurface resolution */
   alpha      = 0.5;			/* Set the alpha channel */
   trans      = 's';			/* Opaque isosurface */

/* Create the isosurface object  - don't draw yet */
   id2 = ns2cisc(grid, nx, ny, nz, 0, nx-1, 0, ny-1, 0, nz-1,
		    tr, level, resolution, trans, alpha, colorfn);

   cs2scb(&cb);
   s2show(1);				/* Open the s2plot window */

   return 1;
}

