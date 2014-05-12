/* ss2wtga.c
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
   static int lkc = 0;			/* Has the spacebar been pressed? */
   static int mode = 0;			/* Should a frame be dumped */ 
   static int frame = 0;		/* Current output frame number */

   if (*kc != lkc) {			/* Has spacebar been pressed? */
      if (!mode) {			/* What is the current mode? */
         char string[64];		
 	 sprintf(string,"myframe_%d",frame);		/* Prepare filename */
	 fprintf(stderr,"Dumping frame: %s\n",string);	/* Output message */
         ss2wtga(string);		/* Write screen image to TGA file */
	 frame++;			/* Increment frame number */
      } 
      mode = 1;				/* Set mode - do not dump again */
   } else {				/* Write user message */
      s2textxy(-1,0,0,"Press <space> to dump TGA frame");
      mode = 0;				/* Make sure ready to dump frame */
   }

   lkc = *kc;				/* Update count of spacebar presses */
}

int main(int argc, char *argv[])
{
   srand48((long)time(NULL));           /* Seed random numbers */
   s2opend("/s2mono", argc, argv);      /* Open in mono mode */
   s2swin(-1.,1., -1.,1., -1.,1.);      /* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   int i, N = 200;			/* Loop variables */
   XYZ xyz;
   COLOUR col;
   
   for (i=0;i<N;i++) {
      xyz.x = drand48()*2.0 - 1.0;	/* Random position */
      xyz.y = drand48()*2.0 - 1.0;
      xyz.z = drand48()*2.0 - 1.0;
      col.r = drand48()*2.0 - 1.0;	/* Random colour */
      col.g = drand48()*2.0 - 1.0;
      col.b = drand48()*2.0 - 1.0;
      ns2vthpoint(xyz, col,3);		/* Draw the point */
   }

   cs2scb(&cb);				/* Install dynamic callback */
   s2show(1);				/* Open the s2plot window */

   return 1;
}
