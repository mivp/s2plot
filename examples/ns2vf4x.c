/* ns2vf4x.c
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
   XYZ vertex[4];				/* 4 vertices */
   COLOUR col = { 1.0, 1.0, 0.3 };		/* Yellowish */
   float scale = 1.0;				/* Scale for texture */
   int texid;					/* ID for this texture */
   char *texture = "firetile2_pow2_rgb.tga";	
		/* Texture in directory pointed to by S2PLOT_TEXPATH */ 

   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   texid = ss2lt(texture);
   vertex[0].x = 0.0; vertex[0].y =  0.0; vertex[0].z = 0.0;
   vertex[1].x = 0.5; vertex[1].y =  0.0; vertex[1].z = 0.0;
   vertex[2].x = 0.5; vertex[2].y = -0.4; vertex[2].z = 1.0;
   vertex[3].x = 0.0; vertex[3].y = -0.4; vertex[3].z = 1.0;

   ns2vf4x(vertex, col, texid, scale, 'o'); 	/* Draw the polygon */

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}

