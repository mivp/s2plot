/* ss2ct.c
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

int main(int argc, char *argv[])
{
   int texid;					/* ID for this texture */
   unsigned char *tex;					/* Array holding texture */
   int width, height;				/* Dimensions of texture */
   int i, j;					/* Loop variables */
   int idx;					/* Temporary index into array */
   XYZ xyz = { 0.0, 0.0, 0.0 };			/* Location of sphere */
   float r = 0.3;				/* Radius of sphere */
   COLOUR col = { 1.0, 1.0, 1.0 };		/* Colour of sphere */

   srand48((long)time(NULL));                   /* Seed random numbers */
   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   width = 128;					/* Width of new texture */
   height = 16;					/* Height of new texture */
   texid = ss2ct(width, height);		/* Create a new texture */
   tex = ss2gt(texid, &width, &height);		/* Store texture */

   for (j=0;j<height;j++) {			
      for (i=0;i<width;i++) {
         idx = (j*width + i)*4;			/* Stored as (r, g, b, alpha) */
         tex[idx  ] = 127*drand48()+128;	/* Randomish Red */
         tex[idx+1] = 0;			/* Green */
         tex[idx+2] = 0;			/* Blue */
						/* Do nothing to alpha */
      }
   }
   
   ss2pt(texid);				/* Restore texture for use */

   ns2vspherex(xyz, r, col, texid);		/* Draw textured sphere */

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
