/* s2qah.c
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
   static int lkc = 0;				/* Count of key presses */
   int filled = 1;				/* Arrow fill style */

   if (lkc != *kc) {				/* Space bar pressed? */
      s2sah(filled, drand48()*130+5.0, drand48()*0.95+0.05);
						/* Choose random arrow format */
   }

   int fs;					/* Fill style */
   float angle, barb;				/* Arrow parameters */
   s2qah(&fs, &angle, &barb);			/* Query arrow paramters */
   
   char string[32];			
   sprintf(string,"Arrow (angle,barb) = (%.2f, %.2f)",angle,barb);	
						/* Write to string */
   s2textxy(0,0,0,string);			/* Display text */

   s2sch(4);					/* Control size of arrowhead */
   float x[2], y[2], z[2];			
   x[0] = -0.8; y[0] = -0.4; z[0] = 0.0;	/* Coordinates of line */
   x[1] = +0.8; y[1] = -0.4; z[1] = 0.0;
   s2arro(x[0],y[0],z[0], x[1],y[1],z[1]);	/* Draw the arrow */ 
   s2sch(1);					/* Reset size for text */

   lkc = *kc;					/* Update count */
}

int main(int argc, char *argv[])
{
   fprintf(stderr,"Press the <spacebar> to change arrow parameters");

   srand48((long)time(NULL));                   /* Seed random numbers */
   s2opend("/s2mono", argc, argv);             	/* Open in mono mode */
   s2swin(-1.,1., -1.,1., -1.,1.);              /* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */
   
   cs2scb(cb);					/* Install dynamic callback */
   s2show(1);                                   /* Open the s2plot window */

   return 1;
}
