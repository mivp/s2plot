/* s2chromapts.c
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
#include <string.h>
#include "s2plot.h"

#define NPTS 500
#define D2R (M_PI / 180.)


int main(int argc, char **argv)
{

   XYZ pos  = {0., 0., -0.1};           /* Camera position */
   XYZ up   = {0., 1., 0.};             /* Camera up vector */
   XYZ view = {0., 0., 1.};             /* Camera view direction */
   int nentries;                        /* Number of entries in colour map */
   float a[NPTS], b[NPTS], dist[NPTS];  /* Coordinates for each point */
   float size[NPTS];                    /* Size to plot point */
   int i;                               /* Loop variable */

   fprintf(stderr,"This demonstration appears in 3D when viewed with ");
   fprintf(stderr,"Chromadepth(TM) glasses\n");
   s2opend("/S2FISH",argc, argv);        /* Open the display - fisheye */
   s2swin(-1.,1., -1.,1., -1.,1.);       /* Set the window coordinates */

   ss2sc(pos, up, view, 0);          	/* Set camera position */
   ss2tc(0);                        	/* prevent translation of the camera */

/* Load the chromadepth colour map, stored in directory S2PLOT_TEXPATH */
   nentries = ss2lcm("chromapal.txt", 100, 2000);
   s2scir(100, 100+nentries-1);          /* Set colour range */

   for (i=0;i<NPTS;i++) {               /* Create random positions */
      a[i] = (float)(rand()) / (float)RAND_MAX * 360.;
      b[i] = asinf(-1. + 2. * (float)(rand()) / (float)RAND_MAX) / M_PI * 180.0;
      dist[i] = sqrtf((float)(rand()) / (float)RAND_MAX * 100.);
      size[i] = 11. - dist[i] * 0.8;    /* Size based on distance */
   }

   s2chromapts(NPTS, a, b, dist, size, 1., 0., 10.);
                        /* Plot using chromadepth information */

   s2show(1);

   return 1;
}
