/* ss2ucf.c
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

void numcb(int *N)
{
   if (*N != 1) return;
   ss2ucf();				/* Go back to rotating around centre */
}

int main(int argc, char *argv[])
{
   int i, N = 20;				/* Loop variables */
   float x, y, z;				/* Random data */
   XYZ focus;					/* Point to rotate about */
   int wc = 1;					/* Use world coordinates */

   srand48((long)time(NULL));			/* Seed random numbers */

   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   s2slw(3);					/* Set line width */
   for (i=0;i<N;i++) {
      x = drand48()*2.0 - 1.0;			/* Random (x,y,z) */
      y = drand48()*2.0 - 1.0;
      z = drand48()*2.0 - 1.0;
      s2sci(15*drand48() + 1);			/* Random colour */
      s2pt1(x,y,z,1);				/* Plot the point */
   }

   focus.x = drand48()*2.0 - 1.0;		/* Random rotation point */
   focus.y = drand48()*2.0 - 1.0;
   focus.z = drand48()*2.0 - 1.0;
   ss2scf(focus, wc);				/* Set the rotation point */

   s2sch(0.7);					/* Set text height */
   s2textxy(-1,0,0,"Rotate camera - then press 1 followed by +"); 

   cs2sncb(&numcb);				/* Install number callback */

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
