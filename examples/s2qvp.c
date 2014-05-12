/* s2qvp.c
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
#include "s2plot.h"

int main(int argc, char *argv[])
{
   float dx1, dx2, dy1, dy2, dz1, dz2;		/* Viewport ranges */

   s2opend("/?",argc, argv);			/* Open the display */
   s2svp(-1.,1., -1.,1., -1.,1.);		/* First view port */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   s2qvp(&dx1,&dx2, &dy1,&dy2, &dz1,&dz2);	/* Query viewport coordinates */
   dx1 /= 2.0; dx2 /= 2.0;			/* Halve the coordintes */
   dy1 /= 2.0; dy2 /= 2.0;
   dz1 /= 2.0; dz2 /= 2.0;

   s2sci(S2_PG_RED);
   s2svp(dx1,dx2, dy1,dy2, dz1,dz2);		/* Create a half-size viewport */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2slw(3);
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   
   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
