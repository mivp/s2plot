/* s2chromacpts.c
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
#include <time.h>
#include "s2plot.h"

#define NPOINTS 500

void cb(double *time, int *keycount)
/* A callback for determining chromadepth colours based on distance */
{
   static int beenhere = 0;             /* Flag for initialising data */
   static int np = NPOINTS;             /* Number of points */
   static float xpts[NPOINTS], ypts[NPOINTS], zpts[NPOINTS];
   static float dist[NPOINTS];          /* Distance from camera to each point */
   static float size[NPOINTS];          /* Size to draw each point */
   XYZ pos, up, view;                   /* Camera parameters */
   float dmin = 9e30;                   /* Min and max distances */
   float dmax = -9e30;
   int i;

   if (!beenhere) {                     /* Initialise first time through */
      for (i=0;i<np;i++) {
         xpts[i] = drand48()*2.0 - 1.0;
         ypts[i] = drand48()*2.0 - 1.0;
         zpts[i] = drand48()*2.0 - 1.0;
       }
       beenhere = 1;
    }

    ss2qc(&pos, &up, &view, 1);     /* Where is the camera? */

    for (i=0; i< np; i++) {             /* Calculate distance to camera */
       dist[i] = sqrt((xpts[i] - pos.x) * (xpts[i] - pos.x) +
                   (ypts[i] - pos.y) * (ypts[i] - pos.y) +
                   (zpts[i] - pos.z) * (zpts[i] - pos.z));

       dmin = (dist[i] < dmin) ? dist[i] : dmin;
       dmax = (dist[i] > dmax) ? dist[i] : dmax;
    }

    for (i=0; i<np; i++) {              /* Scale sizes */
       size[i] = 8.0 - (dist[i] - dmin) / (dmax - dmin) * 6.0;
    }

    s2chromacpts(np, xpts, ypts, zpts, dist, size, dmin, dmax);
                /* Plot points using chromadepth information */
}

int main(int argc, char **argv)
{
   int nentries;                        /* Number of colour map entries */

   srand48((long)time(NULL));                   /* Seed random numbers */

   fprintf(stderr,"This demonstration appears in 3D when viewed with ");
   fprintf(stderr,"Chromadepth(TM) glasses\n");
   s2opend("/?",argc, argv);                    /* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);              /* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */

/* Load the chromadepth colour map, stored in directory S2PLOT_TEXPATH */
   nentries = ss2lcm("chromapal.txt", 100, 2000);
   s2scir(100, 100+nentries-1);                 /* Set colour range */

   cs2scb(&cb);

   s2show(1);

   return 1;
}
