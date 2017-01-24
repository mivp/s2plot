/* s2funt.c
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

#define XLIMIT 10.0
#define YLIMIT 10.0
#define ZLIMIT 50.0

float fx(float *t)
{
  return XLIMIT * sin(*t);
}

float fy(float *t)
{
  return YLIMIT * *t / TWOPI * cos(*t);
}

float fz(float *t)
{
  return 20. + (ZLIMIT * *t / TWOPI + 4.*sin(5* *t)*sin(3* *t))*0.5;
}


int main(int argc, char *argv[])
{
   float x1 = -XLIMIT, x2 = XLIMIT;             /* x world coord range */
   float y1 = -YLIMIT, y2 = YLIMIT;             /* y world coord range */
   float z1 = -ZLIMIT/4.0, z2 = ZLIMIT;         /* z world coord range */

   s2opend("/?",argc,argv);                     /* Open the display */
   s2swin(x1,x2,y1,y2,z1,z2);                   /* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */
   s2sci(S2_PG_YELLOW);

   s2slw(3);                                    /* Set the line width */
   s2funt(&fx, &fy, &fz, 1000, -TWOPI, TWOPI);     /* Draw parametric line */
   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
