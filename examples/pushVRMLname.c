/* pushVRMLname.c
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
   fprintf(stderr,"Press <shift>-<w> to write VRML\n");
   fprintf(stderr,"Creates VRML file: test.wrl\n");

   srand48((long)time(NULL));           /* Seed random numbers */
   s2opend("/s2mono", argc, argv);      /* Open in mono mode */
   s2swin(-1.,1., -1.,1., -1.,1.);      /* Set the window coordinates */

   pushVRMLname("BOX");			/* Create a new VRML object name */
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);
					/* This will not appear in the VRML */

   int i, N = 100;			/* Loop variables */
   XYZ xyz;				/* Point location */
   COLOUR col;				/* Point colour */
 
   pushVRMLname("POINTS");		/* Create a new VRML object name */
   for (i=0;i<N;i++) {
      xyz.x = drand48()*2.0 - 1.0;	/* Random positions */
      xyz.y = drand48()*2.0 - 1.0;
      xyz.z = drand48()*2.0 - 1.0;
      col.r = drand48()*2.0 - 1.0;	/* Random colours */
      col.g = drand48()*2.0 - 1.0;
      col.b = drand48()*2.0 - 1.0;
      ns2vthpoint(xyz, col, 2);		/* Draw current point */
   }
   pushVRMLname("ANON");		/* Restore default VRML object name */
   
   s2show(1);                           /* Open the s2plot window */

   return 1;
}
