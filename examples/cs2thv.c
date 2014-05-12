/* cs2thv.c
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
#include "s2plot.h"

#define N 100                   /* Number of points */

XYZ xyz[N];                     /* Global variable: positions */
COLOUR col[N];                  /* Colour of the point */
int sel[N];                     /* Global varibale: selected data */

void cb(double *t, int *kc)
{
   COLOUR hilite = { 1.0, 1.0, 1.0 };           /* Highlight colour */
   float hsize = 0.02;                          /* Size of handle */
   int i;                                       /* Loop variable */
   static int count = 0;			/* Update each time called */ 

   if (count == 25) {				/* About once per second */
     cs2thv(-1);				/* Toggle handle visibility */
     count = 0;					/* Reset counter */
   } else {
     count++;					/* Keep counting */
   }
   for (i=0;i<N;i++) {
      ns2vthpoint(xyz[i], col[i], 3);                /* Draw thick point */
      ds2ah(xyz[i], hsize, col[i], hilite, i, sel[i]);
                                                    /* Draw handles */
   }
}

int main(int argc, char *argv[])
{
   int i;                                       /* Loop varibale */
   float x, y, z;                               /* Random data */

   srand48((long)time(NULL));                   /* Seed random numbers */

   s2opend("/?",argc, argv);                    /* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);              /* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */

   for (i=0;i<N;i++) {                          /* Set-up globals */
      xyz[i].x = drand48()*2.0 - 1.0;           /* Random position */
      xyz[i].y = drand48()*2.0 - 1.0;
      xyz[i].z = drand48()*2.0 - 1.0;
      col[i].r = drand48();                     /* Random colour */
      col[i].g = drand48();
      col[i].b = drand48();
      sel[i]   = 0;                             /* Not currently selected */
   }

   cs2scb(&cb);                                  /* Install dynamic callback */

   s2slw(2);                                    /* Set line width */
   for (i=0;i<N;i++) {          /* Non-selectable random data points */
      x = drand48()*2.0 - 1.0;
      y = drand48()*2.0 - 1.0;
      z = drand48()*2.0 - 1.0;
      s2sci(15*drand48() + 1);                  /* Random colour */
      s2pt1(x,y,z,1);
   }

   s2show(1);                                   /* Open the s2plot window */

   return 1;
}
