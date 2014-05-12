/* ss2scaf.c
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
   XYZ pos;					/* Camera position */
   XYZ up;					/* Camera up vector */
   XYZ view;					/* Camera view vector */
   XYZ focus;
   int wc = 1;					/* Use world coordinates */
   int i, N = 20;				/* Loop variables */
   float x, y, z;				/* Temporary data */

   srand48((long)time(NULL));                   /* Seed random numbers */
   
   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);	/* Draw coordinate box */

   for (i=0;i<N;i++) {
      x = drand48()*2.0 - 1.0;			/* Random positions */
      y = drand48()*2.0 - 1.0;
      z = drand48()*2.0 - 1.0;
      s2sci(15*drand48()+1);			/* Random colour */
      s2pt1(x, y, z, 1);
   }

   pos.x  = 0.0; pos.y  = 0.0; pos.z  = 8.0;
   up.x   = 0.0; up.y   = 1.0; up.z   = 0.0;
   view.x = 0.0; view.y = 0.0; view.z = -1.0;
   focus.x = 0.0; focus.y = 0.0; focus.z = 1.0;
   ns2thpoint(focus.x, focus.y, focus.z, 1,1,1, 8);
   
   ss2scaf(pos, up, view, focus, wc);		/* Set new camera position */
   ss2sas(1);

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}

