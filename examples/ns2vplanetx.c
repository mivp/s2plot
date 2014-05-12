/* ns2vplanetx.c
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

void cb(double *t, int *kc)
{
   float radius = 0.5;				/* Radius of planet */
   XYZ xyz = { -1.0, 0.0, 0.0 };		/* Location of planet */

   COLOUR colour = { 1.0, 1.0, 0.0 };		/* Yellow */
   static int tid = -1;				/* Texture id */
   if (tid < 0) {				/* First time through? */
      tid = ss2lt("firetile2_pow2_rgb.tga");	/* Load texture for re-use */
   }

   static float phase = 30.0;			/* Set the texture phase */
   XYZ axis = { 0.0, 0.0, 1.0 };		/* Set the rotation axis */
   static float rotation = 0.0;			/* Set the rotation value */

   ns2vplanetx(xyz, radius, colour, tid, phase, axis, rotation);
						/* Draw the "planet" */

   xyz.x = 0.0;					/* Shift centre */
   axis.x = 0.0; axis.y = 1.0; axis.z = 0.0;	/* New rotation axis */
   ns2vplanetx(xyz, radius, colour, tid, phase, axis, rotation);
						/* Draw the "planet" */

   xyz.x = 1.0;					/* Shift centre */
   axis.x = 1.0; axis.y = 0.0; axis.z = 0.0;	/* New rotation axis */
   ns2vplanetx(xyz, radius, colour, tid, phase, axis, rotation);
						/* Draw the "planet" */

   rotation += 1.0;				/* Update rotation in degrees */
   if (rotation > 360.0) rotation = 0.0;	/* Full circle? Reset */

   
}

int main(int argc, char *argv[])
{

   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */

   ss2ssr(32);					/* Set the sphere resolution */
   cs2scb(cb);					/* Install dynamic callback */

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}

