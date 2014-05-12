/* xs2qsp.c
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
int pid = 0;			/* Which panel should be drawn? */ 
int panel0;			/* ID of the master panel */

void cb(double *t, int *kc)
{
   int myid = xs2qsp();		/* Query the panel id currently being drawn */

   if (myid == panel0) {	/* Is this the main panel? */
      static int count = 8;	/* Counter  */
      if (count < 0) {		/* Is it time to choose a new panel */
	 pid = random()%3; 	/* Choose a random new panel */
         count = 8;		/* Reset the counter */
      }
      count--;			/* Decrease counter */
   }

   if (myid == pid) {		/* If this panel is the selected panel... */
      s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */
      char string[32];
      sprintf(string,"I am panel %d",myid);	   /* Prepare a string */
      s2textxy(-1,0,0,string);			   /* Write the string */
   }
}

int main(int argc, char *argv[])
{
   int panel1;				/* ID of newly created panel */
   int panel2;				/* ID of newly created panel */

   s2opend("/s2mono", argc, argv);      /* Open in mono mode */
   s2swin(-1.,1., -1.,1., -1.,1.);      /* Set the window coordinates */

   panel0 = xs2qsp();			/* Get the ID of the master panel */
   xs2mp(panel0, 0.0, 0.5, 1.0, 1.0);	/* Move default panel to top left */
   cs2scb(&cb);				/* Install a dynamic callback */

   panel1 = xs2ap(0.0, 0.0, 0.5, 0.5);	/* Create panel in bottom left */
   xs2cp(panel1);			/* Choose this panel */
   s2swin(-1.,1., -1.,1., -1.,1.);      /* Set the window coordinates */
   cs2scb(&cb);				/* Install a dynamic callback */
   
   panel2 = xs2ap(0.5, 0.0, 1.0, 0.5);	/* Create panel in bottom right */
   xs2cp(panel2);			/* Choose this panel */
   s2swin(-1.,1., -1.,1., -1.,1.);      /* Set the window coordinates */
   cs2scb(&cb);				/* Install a dynamic callback */

   xs2cp(panel0);			/* Go back to main panel */

   s2sch(2);				/* Set the character height */

   s2show(1);                           /* Open the s2plot window */

   return 1;
}
