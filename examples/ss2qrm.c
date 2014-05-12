/* ss2qrm.c
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

#define WIREFRAME		0
#define SHADE_FLAT		1
#define SHADE_DIFFUSE		2	
#define SHADE_SPECULAR 		3

void numcb(int *N)
/* Callback for pressing number key - this example does same as pressring 'R' */
{
   int mode = ss2qrm();				/* Current render mode */
   mode = (mode + 1) % 4;			/* Next render mode */

   switch(*N) {					
      case 1  : ss2srm(mode); break;		/* Set the render mode */
      default : return;
   }
   cs2tcb();					/* Turn the callback back on */
}

void cb(double *t, int *kc)
{
   float x, y, z;				/* Position */
   float r, g, b;				/* Colour */
   float radius = 0.3;				/* Radius */

   x = 0.0; y = 0.0; z = 0.0;			
   r = 1.0; g = 1.0; b = 1.0;			/* Yellow */
   ns2sphere(x,y,z, radius, r,g,b);		/* Draw the sphere */

   cs2tcb();		/* Temporarily turn-off callback */

}

int main(int argc, char *argv[])
{

   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   cs2scb(&cb);					/* Install callback */
   cs2sncb(&numcb);				/* Install number callback */

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
