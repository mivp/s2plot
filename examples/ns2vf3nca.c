/* ns2vf3nca.c
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
   XYZ vertex[3];				/* 3 vertices */
   XYZ normal[3];				/* 3 vertices */
   float alpha[3];				/* 3 vertices */
   COLOUR col[3];				/* 3 vertices */
   char trans = 't';				/* Transparency type */
   int i;

   srand48((long)time(NULL));			/* Seed random numbers */
   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   for (i=0;i<3;i++) {
      vertex[i].x = drand48()*2.0 - 1.0;	/* Random corners to facet */
      vertex[i].y = drand48()*2.0 - 1.0;
      vertex[i].z = drand48()*2.0 - 1.0;
      normal[i].x = drand48()*2.0 - 1.0;	/* Random normal for facet */
      normal[i].y = drand48()*2.0 - 1.0;
      normal[i].z = drand48()*2.0 - 1.0;
      col[i].r = drand48();			/* Random colour for vertex */
      col[i].g = drand48();
      col[i].b = drand48();
   }
   alpha[0] = 0.0;				/* Set alpha channels for each */
   alpha[1] = 0.5;				/*   vertex */
   alpha[2] = 1.0;
   ns2vf3nca(vertex, normal, col, trans, alpha);
			/* Draw polygon with defined normals, vertex colours */
			/* and vertex transparencies */

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
