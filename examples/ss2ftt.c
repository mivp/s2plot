/* ss2ftt.c
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

unsigned int tid;

void cb(double *t, int *kc)
{
   XYZ xyz = {0.0, 0.3, 0.4};				/* Location */
   float isize = 0.02;					/* Texture scale */
   XYZ str = { 0.0, 0.0, 0.0};				/* No stretch */
   COLOUR col = { 1.0, 1.0, 0.0 };			/* Yellow */
   float alpha = 0.9; 					/* Alpha channel */
   char trans = 's';					/* Transparency */
   
   ds2vbb(xyz, str, isize, col, tid, alpha, trans); 	/* Draw the billboard */
}

int main(int argc, char *argv[])
{
   s2opend("/?",argc,argv);			/* Open the display */
   s2svp(-1.0,1.0, -1.0,1.0, -1.0,1.0);		/* Set the viewport coords */
   s2swin(-1.0,1.0, -1.0,1.0, -1.0,1.0);	/* Set the window coordinates */
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);	/* Draw a bounding box */

   char font[64];				/* Allocate font name string */

#if defined(S2DARWIN)
   sprintf(font,"/Library/Fonts/Arial Black");	/* Path to font */
			/* NOTE: This depends on your local system config */
#else
   sprintf(font,"/usr/X11R6/lib/X11/fonts/truetype/cmmi10.ttf");
#endif

   tid = ss2ftt(font,"s2plot",32,4);		/* Create the texture */

   cs2scb(&cb);					/* Install a callback */

   s2show(1);					/* Open the s2plot window */

   return 1;
}
