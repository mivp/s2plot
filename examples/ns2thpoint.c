/* ns2thpoint.c
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

int main(int argc, char *argv[])
{
   int N = 100;					/* Number of points */
   float x, y, z;				/* Positions */
   float r, g, b;				/* Colours */
   float size;					/* Thickness of point */
   int i;

   srand48((long)time(NULL));			/* Seed random numbers */

   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   for (i=0;i<N;i++) {				/* Random data positions */
      x = drand48()*2.0 - 1.0;
      y = drand48()*2.0 - 1.0;
      z = drand48()*2.0 - 1.0;
      r = drand48();
      g = drand48();
      b = drand48();
      size = drand48()*10.0;
      ns2thpoint(x,y,z, r,g,b, size);		/* Draw the thick point */
   }
   

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}

