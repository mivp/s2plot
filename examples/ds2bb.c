/* ds2bb.c
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

unsigned int tid;

void cb(double *t, int *kc)
{
   float x = 0.0, y = 0.3, z = 0.4;			/* Location */
   float isize = 0.02;					/* Texture scale */
   float str_x = 0.0, str_y = 0.0, str_z = 0.0;		/* No stretch */
   float r = 1.0, g = 1.0, b = 0.0;			/* Yellow */
   float alpha = 0.9; 					/* Alpha channel */
   char trans = 's';					/* Transparency */
   
   ds2bb(x,y,z, str_x,str_y,str_z, isize, r,g,b, tid, alpha, trans);
					/* Draw the billboard */
}

int main(int argc, char *argv[])
{
   int width = 16, height = 16;		/* Dimensions of texture */
   int i, j;				/* Loop variables */
   int idx;				/* Index into texture array */
   unsigned char *tex;				/* Array of texture values */

   srand48((long)time(NULL));			/* Seed random numbers */

   s2opend("/?",argc,argv);			/* Open the display */
   s2svp(-1.0,1.0, -1.0,1.0, -1.0,1.0);		/* Set the viewport coords */
   s2swin(-1.0,1.0, -1.0,1.0, -1.0,1.0);	/* Set the window coordinates */
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);	/* Draw a bounding box */

   tid = ss2ct(width, height);		/* Create a new texture */
   tex = ss2gt(tid, &width, &height);	/* Get the texture */

   for (j=0;j<height;j++) {
      for (i=0;i<width;i++) {
         idx = (j*width + i) * 4;		/* Stored as (r,g,b,alpha) */
         tex[idx  ] = 127*drand48()+128;	/* Randomish red */
         tex[idx+1] = 0;			/* Green */
         tex[idx+2] = 0;			/* Blue */
						/* Do nothing to alpha */
      }
   }
   ss2pt(tid);				/* Push texture for usage */

   cs2scb(&cb);				/* Install a callback */

   s2show(1);				/* Open the s2plot window */

   return 1;
}
