/* ss2qnfp.c
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

void cb(double *t, int *kc)
{
   XYZ foc = { 0, 0, 0 };			/* Set the camera focus */
   ss2scf(foc, 1);

   if (*kc % 2 == 1)
      ss2snfe(0.5);				/* Change the near/far */
   else 					/*  expansion factor */
      ss2snfe(1.0);				/*  on <space> press */

   double near, far;				/* Distance to near/far clip planes */
   ss2qnfp(&near, &far);			/* Query these distances */
   char string[64];				/* Output as a string */
   sprintf(string,"Clipping plane distances: Near = %.2lf Far = %.2lf",near, far); 
   s2lab("","","",string);			/* Show the string as a screen label */
   
}

int main(int argc, char *argv[])
{
   srand48((long)time(NULL));                   /* Seed random numbers */
   s2opend("/s2dsana", argc, argv);		/* Open in anaglyph mode */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   int i, N = 500;				/* Loop variables */
   float bound = 2.0;				/* Bound bigger than window */ 
   XYZ xyz;					/* Point location */
   COLOUR rgb;					/* Point colour */
   for (i=0;i<N;i++) {
      xyz.x = 0.0;
      xyz.y = 0.0;
      xyz.z = drand48()*(2.0*bound) - bound;	/* Random z-value for point */
      rgb.r = drand48();
      rgb.g = drand48();
      rgb.b = drand48();
      ns2vthpoint(xyz, rgb, 3);			/* Draw the point */
   }
   cs2scb(cb);					/* Install a dynamic callback */

   XYZ pos = { 0.0, 0.0, +5.0 };		/* Move the camera closer */
   XYZ vd = { 0.0, 0, -1.0 };
   XYZ up = { 0.0, 1.0, 0 };
   ss2sc(pos, up, vd, 1);

   XYZ foc = { 0, 0, 0 };			/* Set the camera focus */
   ss2scf(foc, 1);

   s2textxy(-1,0.1,0,"Rotate camera and press <space>");	
   s2textxy(-0.5,-0.1,0,"to see effect of ss2snfe");

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
