/* ss2tsc.c
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
   fprintf(stderr,"Try /s2mono and /s2passv modes to see differences\n");

   s2opend("/?", argc, argv);             /* Open in mono mode */
   s2swin(-1.,1., -1.,1., -1.,1.);        /* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */
   
   ss2tsc("r");				/* Right eye screen coordinates */
   s2textxy(0.1,0.1,0.1,"Right eye\n");

   ss2tsc("l");				/* Left eye screen coordinates */
   s2textxy(0.1,0.1,0.1,"Left eye\n");

   ss2tsc("c");				/* Main screen - non stereo mode */
   s2textxy(0.2,0.5,0.1,"Main screen\n");

   ss2tsc("clr");				
   s2textxy(0.2,0.7,0.1,"Everywhere!\n");

   ss2tsc("");				/* Leave screen coordinate mode */
   s2textxy(0.0,0.0,0,"World coordinates\n");
   
   s2show(1);                                   /* Open the s2plot window */

   return 1;
}
