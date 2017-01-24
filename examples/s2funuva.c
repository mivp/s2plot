/* s2funuva.c
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
#include <math.h>
#include "s2plot.h"

#define LIMIT 0.7
#define pi 3.14159265359

/* amplitude, number of twists, major radius, minor radius */
#define AMP 0.4         /* Amplitude */
#define NTW 1.5         /* Number of twists */
#define R1 2.0          /* Major radius */
#define R2 0.5          /* Minor radius */


/* Parametric functions for surface */
float tkx(float *u, float *v) {
  return R2 * cos(*v) * cos(*u) + R1 * cos(*u) * (1. + AMP * cos(*u * NTW));
}
float tky(float *u, float *v) {
  return R2 * sin(*v) + AMP * sin(*u * NTW);
}
float tkz(float *u, float *v) {
  return R2 * cos(*v) * sin(*u) + R1 * sin(*u) * (1. + AMP * cos(*u * NTW));
}
float fcol(float *u, float *v) {
  return 0.5 + sin(0.5 * *u) * AMP * sin(*v);
}
float falpha(float *u, float *v) {
   if ((*v) < pi) return 0.0;
   else return 1.0;
}

int main(int argc, char *argv[])
{

   float x1 = -LIMIT, x2 = LIMIT;               /* x world coord range */
   float y1 = -LIMIT, y2 = LIMIT;               /* y world coord range */
   float z1 = -LIMIT, z2 = LIMIT;               /* z world coord range */
   char trans = 't';				/* Transparency type */

   s2opend("/?",argc, argv);                    /* Open the display */
   s2swin(x1, x2, y1, y2, z1, z2);              /* Set the window coordinates */

   s2icm("rainbow", 100, 500);                  /* Install colour map */
   s2scir(100, 500);                            /* Set colour range */
   
   s2funuva(tkx, tky, tkz, fcol, trans, falpha, 0., 4.*pi, 320, 0., 2.*pi, 90);
                                /* call the parametric surface function */

   s2show(1);                                   /* Open the s2plot window */

   return 1;

}
