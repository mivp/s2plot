/* cs2spcb.c
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

/* Global variable */
float r = 0, g = 0, b = 0;			/* Background colour */

void pcb(char *string)			
{
   int res;					/* How many numbers entered? */
   float rr, gg, bb;				/* Local variables */
   res = sscanf(string,"%f %f %f",&rr,&gg,&bb);	/* Read from prompt string */
   if (res == 3) { 
      r = rr;					/* Change background colour */
      g = gg;
      b = bb;
   }
}

void cb(double *t, int *kc)
{
   ss2sbc(r, g, b);				/* Set background colour */
   ss2sfc(1-r, 1-g, 1-b);			/* Choose sensible foreground */
   s2scr(32, 1-r, 1-g, 1-b);			/* Create a new colour index */
   s2sci(32);					/* Set colour to this index */
   s2textxy(-1,0,0,"Press ~ then enter R G B values e.g 1 1 1");
						/* Write some text */
}

int main(int argc, char *argv[])
{
   s2opend("/s2mono", argc, argv);             /* Open in mono mode */
   s2swin(-1.,1., -1.,1., -1.,1.);              /* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */
   
   cs2scb(cb);					/* Install dynamic callback */

   char string[32];				/* Allocate prompt string memory */
   cs2spcb(pcb, string);			/* Install a prompt callback */

   s2show(1);                                   /* Open the s2plot window */

   return 1;
}
