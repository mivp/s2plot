/* ss2qpr.c
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "s2plot.h"

void cb(double *t, int *kc)
/* Dynamic callback function */
{
   XYZ rot; 
   int wc = 1;				/* Use world coordinates */
   XYZ off = { 0.01, 0.01, 0.0 };	/* Small offset for text */

   int set;
   char string[255];
   s2sch(0.05);				/* Set the text height */
   if (*kc%2 == 0) {
      ss2qpr(&rot, wc);			/* Query the rotation point */ 
      sprintf(string,"Rotation about (%.2f, %.2f, %.2f)",rot.x,rot.y,rot.z);
      XYZ xyz = { 0.5, 0.5, 0.5 };
      ss2scf(xyz, wc);			/* Set the camera focus */
   } else {
      ss2qcf(&set, &rot, wc);		/* Query the camera focus */
      sprintf(string,"Focus at (%.2f, %.2f, %.2f)",rot.x,rot.y,rot.z);
   }
   ds2vtb(rot, off,string,1);		/* Write some dynamic billboard text */
   s2lab("","","","<space> to toggle mode");
					/* Display an instructional label */

}

int main(int argc, char *argv[])
{
   int i;				/* Loop variable */
   XYZ xyz;				/* Coordinate for plotting points */
   COLOUR col;				/* Colour of plotted points */

   s2opend("/S2MONO",argc,argv);	/* Query the s2plot device to use */
   s2swin(-1.,1.,-1.,1.,-1.,1.);	/* Set the world coordinates */
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);	/* Draw a coordinate box */

   srand48((long)time(NULL));		/* Seed the random number generator */

   for (i=0;i<1000;i++) {		/* Generate and plot random points */
      xyz.x = drand48()*2.0 - 1.0;	/*  with random colours */
      xyz.y = drand48()*2.0 - 1.0;
      xyz.z = drand48()*2.0 - 1.0;
      col.r = drand48();
      col.g = drand48();
      col.b = drand48();
      ns2vpoint(xyz, col);
   }
   ss2sas(1);				/* Start the geometry auto-spinning */
   cs2scb(cb);				/* Install a dynamic callback */
   s2show(1);				/* Display the geometry */
   return 0;
}
