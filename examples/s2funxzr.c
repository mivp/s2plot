/* s2funxzr.c
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

#define XLIMIT 1.0
#define YLIMIT 1.0
#define ZLIMIT 1.0

float fxy(float *x, float *y)
{
   return ((*x)*(*x) + 0.25*(*y)*(*y));         /* Function to evaluate */
}

float fxz(float *x, float *z)
{
   return (0.1*(*x)*(*x) + 0.1*(*z)*(*z));      /* Function to evaluate */
}

float fyz(float *y, float *z)
{
   return (-0.3*(*y)*(*y) - 0.3*(*z)*(*z));     /* Function to evaluate */
}

int main(int argc, char *argv[])
{
   int nx = 64, ny = 64, nz = 64;
                                /* Resolution of grids to plot function on */
   int ctl = 0;                 /* User responsible for setting environ */

   float x1 = -XLIMIT*1.5, x2 = +XLIMIT*1.5;    /* World coordinates */
   float y1 = -YLIMIT*1.5, y2 = +YLIMIT*1.5;
   float z1 = -ZLIMIT*1.5, z2 = +ZLIMIT*1.5;
   float d1, d2;				/* Dummy variables */
   float rmin;                                  /* Minimum range value */
   float rmax;                                  /* Maximum range value */

   s2opend("/?",argc,argv);                     /* Open the display */
   s2swin(x1,x2,y1,y2,z1,z2);                   /* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */

   s2icm("rainbow",1000,2000);
   s2scir(1000,2000);

   d1 = 0.0; d2 = 0.0;                          /* Dummy variables */
   rmin = fxy(&d1,&d2);                         /* Minimum at centre */
   d1 = XLIMIT; d2 = YLIMIT;
   rmax = 2.0*fxy(&d1, &d2);            /* Maximum at edge */
   s2funxyr(fxy, nx, ny, -XLIMIT,XLIMIT, -YLIMIT,YLIMIT, ctl, rmin, rmax);
                                /* Functional surface in X-Y plane */

   d1 = XLIMIT; d2 = ZLIMIT;                    /* Dummy variables */
   rmax = fxz(&d1, &d2);                        /* Maximum at edge */
   rmin = -rmax;
   s2funxzr(fxz, nx, nz, -XLIMIT,XLIMIT, -ZLIMIT,ZLIMIT, ctl, rmin, rmax);
                                /* Functional surface in X-Z plane */

   d1 = 0.0; d2 = 0.0;                          /* Dummy variables */
   rmax = fyz(&d1, &d2);                        /* Maximum at centre */
   d1 = YLIMIT; d2 = ZLIMIT;                    /* Dummy variables */
   rmin = fyz(&d1, &d2);                        /* Minimum at edge */
   s2funyzr(fyz, ny, nz, -YLIMIT,YLIMIT, -ZLIMIT,ZLIMIT, ctl, rmin, rmax);
                                /* Functional surface in Y-Y plane */

   s2show(1);                   /* Open the s2plot window */

   return 1;
}

