/* s2qtxtxzf.c
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
   float x = 0.0;
   float y = 0.5;
   float z = 0.2; 
   float flipx = -1.0;
   float flipz = -1.0;
   float x1,x2, z1,z2;				/* Bounding coordinates */
   char string[32] = "S2PLOT is great!";
   float pad = 0.5;				/* Padding around text */

   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   s2sci(S2_PG_YELLOW);
   s2slw(2);					/* Set line width */
   s2textxzf(x,y,z,flipx,flipz,string);		/* Write some flipped text in x-z plane */

   s2qtxtxzf(&x1,&x2, &z1,&z2, x,y,z, flipx, flipz, string, pad);
						/* Query bounding box for flipped text */

   s2sci(S2_PG_GREEN);	
   s2rectxz(x1,x2, z1,z2, y-0.01);		/* Draw bounding rectangle */
   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
