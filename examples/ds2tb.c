/* ds2tb.c
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

float *x, *y, *z;			/* Global arrays - text positions */
int N;					/* Number of text labels */

void cb(double *t, int *kc)
/* A dynamic callback */
{
   int i;				/* Loop variable */
   float xoff = 0.01;			/* Small offsets - must be non-zero */
   float yoff = 0.01;			/*  for text to appear */
   
   s2sch(0.2);				/* Set the text height */
   for (i=0;i<N;i++) {			/* Draw the billboard labels */
     ds2tb(x[i], y[i], z[i], xoff, yoff, "S2PLOT", 0);
   }
}


int main(int argc, char *argv[])
{
   int i;					/* Loop variable */
   srand48((long)time(NULL));			/* Seed random numbers */

   s2opend("/S2MONO",argc,argv);		/* Open the display */
   s2swin(-1.5,1.5, -1.5,1.5, -1.5,1.5);
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);	/* Draw coord box */

   N = 20;					/* Number of labels to draw */
   x = (float *)calloc(N, sizeof(float));
   y = (float *)calloc(N, sizeof(float));
   z = (float *)calloc(N, sizeof(float));

   for (i=0;i<N;i++) {			/* Create N random (x,y,z) values */
      x[i] = drand48()*2.0 - 1.0;
      y[i] = drand48()*2.0 - 1.0;
      z[i] = drand48()*2.0 - 1.0;
   }
 
   cs2scb(cb);				/* Install a dynamic callback */

   s2show(1);				/* Open the s2plot window */

   return 1;
}
