/* ss2qfra.c
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

void cb(double *t, int *kc)
{
   ss2sas(1);					/* Autospin mode on */

   static int lkc = 0;                          /* Count of key presses */

   if (lkc != *kc) {                            /* Space bar pressed? */
     ss2sfra(drand48()*90-45.0);		/* Rotate the centre up */
   }

   float fra = ss2qfra();		/* Query fisheye rotation angle */
   s2sch(5);
   char string[32];
   sprintf(string,"Fisheye angle = %.2f",fra);  /* Write to string */
   s2textxy(0,0,0,string);                   	/* Display text */

   lkc = *kc;                                   /* Update count */
}


int main(int argc, char *argv[])
{

   fprintf(stderr,"Press <spacebar> to change fisheye rotation angle\n"); 
   srand48((long)time(NULL));                   /* Seed random numbers */
   s2opend("/S2FISH",argc, argv);		/* Open the display: fisheye */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */

   ss2tc(0);					/* Don't allow translation */

   cs2scb(cb);					/* Install dynamic callback */
   s2show(1);					/* Open the s2plot window */
   
   return 1;
}

