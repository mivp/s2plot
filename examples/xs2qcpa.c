/* xs2qcpa.c
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
   int panel0 = 0;				/* Default ID of main panel */
   int panel1;					/* ID of newly created panel */
   int panel2;					/* ID of newly created panel */
   int pa;					/* Activity status of panel */

   srand48((long)time(NULL));			/* Seed random numbers */

   s2opend("/s2mono", argc, argv);      	/* Open in mono mode */

   xs2mp(panel0, 0.5, 1.0, 1.0, 1.0);		/* Move master to top */
   s2swin(-1.,1., -1.,1., -1.,1.);      	/* Set window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */
   
   panel1 = xs2ap(0.0, 0.0, 0.5, 0.5);		/* Create new panel */
   xs2cp(panel1);				/* Choose this panel */
   s2swin(-1.,1., -1.,1., -1.,1.);      	/* Set window coordinates */
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);  	/* Draw coordinate box */
   
   panel2 = xs2ap(0.5, 0.0, 1.0, 0.5);		/* Create new panel  */
   xs2cp(panel2);				/* Choose this panel */
   s2swin(-1.,1., -1.,1., -1.,1.);      	/* Set window coordinates */
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);  	/* Draw coordinate box */

   int i;					/* Loop variable */
   for (i=panel0;i<=panel2;i++) {
      if (drand48() > 0.7) xs2tp(i);		/* Randomly deactivate panel */
      xs2cp(i);					/* Chose a panel */
      pa = xs2qcpa();				/* Is this panel active? */
      if (pa) fprintf(stderr,"Panel %d is active\n",i);
      else fprintf(stderr,"Panel %d is inactive\n",i);
   }

   s2show(1);                                   /* Open the s2plot window */

   return 1;
}
