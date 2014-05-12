/* s2errb.c
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

/* A modified version of sample program s2uc1.c */

int main(int argc, char *argv[]) 
{

  int i;
  float np = 100;       	/* point population size */
  float *xpts, *ypts, *zpts;    /* point positions */
  float *expts,*eypts,*ezpts;   /* error bar lengths */ 
  int symbol = 0;               /* -2 = infinitesimally small dot */
  float x1 = -100.0, x2 = +100.0;
  float y1 = -100.0, y2 = +100.0;
  float z1 = -100.0, z2 = +100.0;

  s2opend("/?",argc,argv);

  /* set world coordinate range */
  s2swin(x1,x2, y1,y2, z1,z2);

  /* generate a set of points extending over world coordinate space */
  xpts = (float *)calloc(np, sizeof(float));
  ypts = (float *)calloc(np, sizeof(float));
  zpts = (float *)calloc(np, sizeof(float));
  expts = (float*)calloc(np, sizeof(float));
  eypts = (float*)calloc(np, sizeof(float));
  ezpts = (float*)calloc(np, sizeof(float));


  for (i = 0; i < np; i++) {
    xpts[i] = x1 + (float)(rand()) / (float)RAND_MAX * (x2 - x1);
    ypts[i] = y1 + (float)(rand()) / (float)RAND_MAX * (y2 - y1);
    zpts[i] = z1 + (float)(rand()) / (float)RAND_MAX * (z2 - z1);
    expts[i]=(float)(rand()) / (float)RAND_MAX * (x2-x1)*0.05;
    eypts[i]=fabsf(ypts[i]) * 0.1;
    ezpts[i]= (z2-z1)*0.1;
  }

  /* plot red points */
  s2sci(S2_PG_RED);
  s2pt(np, xpts, ypts, zpts, symbol);
    
  /* plot various error bars */
  s2sci(S2_PG_WHITE);
  s2errb(4, np, xpts, ypts, zpts, expts, 1);
  s2sci(S2_PG_GREEN);
  s2errb(2, np, xpts, ypts, zpts, eypts, 2);
  s2sci(S2_PG_ORANGE);
  s2errb(9, np, xpts, ypts, zpts, ezpts, 3);

  /* draw and label a box around the points, in white */
  s2sci(S2_PG_WHITE);
  s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);

  s2show(1);

  return 0;

}
