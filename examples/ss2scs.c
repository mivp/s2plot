/* ss2scs.c
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
   static int lkc = 0;          	/* Count of key presses */
   static float spd = 1.0; 		/* Set the camera increment */

   if (lkc != *kc) {			/* Query whether <space> was pressed */
      spd += 2.0;			/* Update the camera increment */
   }
   ss2scs(spd);				/* Set the new camera increment */

   char string[32];			/* Write current increment to screen */
   sprintf(string,"%.2f",spd);
   s2textxy(0,0,0,string);		/* Display text */

   lkc = *kc;				/* Update key press count */
}


int main(int argc, char *argv[])
{

   srand48((long)time(NULL));                   /* Seed random numbers */
   s2opend("/s2mono",argc, argv);		/* Open the display: mono */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);	/* Draw coordinate box */

   s2lab("","","","Press <space> to change camera movement increment, then +/- to test zooming\n"); 

   cs2scb(cb);					/* Install dynamic callback */
   s2show(1);					/* Open the s2plot window */
   
   return 1;
}

