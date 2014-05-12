/* ss2sca.c
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
   int N = 100;					/* Number of points */
   static int flag = -1;			/* Flag for initialisation */
   static XYZ xyz[100];				/* Array of point positions */
   static COLOUR col[100];			/* Array of colours */
   int i;					/* Loop variable */

   if (flag < 0) {				/* First time through? */
      for (i=0;i<N;i++) {			
         xyz[i].x = drand48()*2.0 - 1.0; 	/* Random data positions */
         xyz[i].y = drand48()*2.0 - 1.0;
         xyz[i].z = drand48()*2.0 - 1.0;
         col[i].r = drand48(); 			/* Random data colours */
         col[i].g = drand48();
         col[i].b = drand48();
      }
      flag = 1;					/* Set the flag */
   }

   for (i=0;i<N;i++) {
      ns2vthpoint(xyz[i], col[i], 3);		/* Draw the point */
   }

   static int lkc = 1;				/* Last time <space> key pressed */
   if (lkc != *kc) 				/* Check for keyboard press */
      ss2sca(drand48()*70 + 20);		/* Set the camera aperture */
   lkc = *kc;					/* Update key press */

   int wc = ss2qca();				/* Query camera aperture */
   char text[64];				
   sprintf(text, "Aperture %d degrees\n",wc);	/* Prepare a text string */
   s2textxy(0.0,0.0,0.0, text);			/* Display text string */

   s2lab("","","","Press <space> to change camera aperture");
}

int main(int argc, char *argv[])
{

   srand48((long)time(NULL));			/* Seed random numbers */

   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   cs2scb(cb);					/* Install a dynamic callback */

   s2show(1);					/* Open the s2plot window */

   
   return 1;
}
