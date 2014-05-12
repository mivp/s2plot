/* ds2unprotect.c
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
#include <time.h>
#include "s2plot.h"

void ncb(int *N)
{
   int pstate = ds2isprotected();		/* What is current protection state? */

   if ((*N == 1) && (pstate == 0)) { 		/* Key 1 was pressed */
      ds2protect();				/* Protect dynamic geometry */
   }
   if ((*N == 2) && (pstate == 1)) { 		/* Key 2 was pressed */
      ds2unprotect(); 				/* Unprotect dynamic geometry */
   }
}

void cb(double *t, int *kc)
{
   static XYZ orbit; 				/* Position of point to draw */
   static float tt = 0;				/* Current position in orbit */
   orbit.x = cos(tt);				/* Update point position */
   orbit.y = sin(tt);
   tt += 0.05;

   COLOUR ocol = { 1, 1, 0 };			/* Point colour = yellow*/
   ns2vthpoint(orbit, ocol,  3);		/* Draw the point */

}

int main(int argc, char *argv[])
{
   srand48((long)time(NULL));			/* Seed random numbers */
   s2opend("/?", argc, argv);			/* Prompt for display type */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   cs2scb(cb);					/* Install dynamic geometry callback */
   cs2sncb(ncb);				/* Install number key press callback */

   char string[128];				/* Write instruction label */
   sprintf(string,"Press 1 to protect, press 2 to unprotect");
   s2lab("","","",string);

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
