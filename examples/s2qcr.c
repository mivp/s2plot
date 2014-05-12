/* s2qcr.c
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
   int i;					/* Loop variable */
   int N = 50;					/* Number of points */
   float x[50], y[50], z[50];			/* Coordinates of points */
   float r, g, b;				/* RGB colours */
   int symbol = 1;				/* Point symbol */
   int offset = 32;				/* Starting colour */

   srand48((long)time(NULL));			/* Seed random numbers */
   for (i=0;i<N;i++) {
      x[i] = drand48()*2.0 - 1.0;
      y[i] = drand48()*2.0 - 1.0;
      z[i] = drand48()*2.0 - 1.0;
   }

   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   s2slw(5);					/* Sets size of point */
   s2scir(offset, offset+N);			/* Set the colour index range */
   for (i=offset;i<(offset+N);i++) {
      r = drand48();				/* Choose random red value */
      g = r;					/* Green = Blue = Red */ 
      b = r;					/*  means grey-scale! */
      s2scr(i, r, g, b);			/* Set colour representation */
   }
   
   for (i=0;i<N;i++) {
      s2qcr(offset+i, &r, &g, &b);		/* Query colour representation */
      s2scr(offset+i, r, 1.0, g);		/* Make it greenish */
      s2sci(offset+i);				/* Set the colour */
      s2pt1(x[i],y[i],z[i],symbol);		/* Draw a single point */
   }
   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
