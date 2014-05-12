/* s2sah.c
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

int main(int argc, char *argv[])
{
   float x1 = -0.3, x2 = +0.5;			/* Arrow coordinates */
   float y1 = +0.2, y2 = +0.1;
   float z1 = -0.4, z2 = +0.8;
   int fs = 1;					/* Arrow fill style */
   float angle = 30.0;				/* Arrow cone head angle */
   float barb = 0.7;				/* Arrow barb level */

   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   s2sci(S2_PG_YELLOW);
   s2sah(fs, angle, barb);			/* Set arrow head style */
   s2sch(5);					/* Set size of arrow head */
   s2arro(x1,y1,z1, x2,y2,z2);			/* Draw an arrow */
   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
