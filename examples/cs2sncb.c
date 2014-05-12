/* cs2sncb.c
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

/* Spacebar: toggle between two different colours */
/* Press 1:  to turn off callback */
/* Press 2:  to turn on callback */
/* Press 3:  to select a different colour set to toggle between */
/* Press 4:  to select a different colour set to toggle between */
/* Press 5:  toggle callback - same as pressing 1 then 2 */

int colour = 1;                         /* Global variable: colour index for callback */

void ncb(int *N)
/* Callback for pressing number keys */
{
   if (*N > 5) return;                  /* Do nothing! */
   switch (*N) {
      case 1 : cs2dcb();                 /* Turn off callback */
               break;
      case 2 : cs2ecb();                 /* Turn on callback */
               break;
      case 3 : colour = 3;              /* Change the colour that alternates */
               break;
      case 4 : colour = 5;              /* Change the colour that alternates */
               break;
      case 5 : cs2tcb();                 /* Toggle callback on/off */
               break;

   }
}

void cb(double *t, int *kc)
/* A dynamic callback - jitter particle positions each time through */
{
   static int flag = 0;                 /* Flag on whether first time through */
   static float x[20], y[20], z[20];    /* Dynamic geometry */
   int i, N = 20;

   if (flag == 0) {                     /* First time only */
      for (i=0;i<N;i++) {
         x[i] = drand48()*2.0-1.0;      /* Random positions */
         y[i] = drand48()*2.0-1.0;
         z[i] = drand48()*2.0-1.0;
      }
      flag = 1;         	/* Set the flag - don't need this loop again */
   }

   for (i=0;i<N;i++) {
      x[i] += (drand48()*0.1-0.05); /* Apply a jitter each time in callback */
      y[i] += (drand48()*0.1-0.05);
      z[i] += (drand48()*0.1-0.05);
   }

   s2sci((*kc % 2) + colour);
         /* Set colour on number of times space is pressed - uses global */

   s2slw(3);                        /* Set the line width */
   s2pt(N, x, z, y, 1);             /* Draw the points */

}




int main(int argc, char *argv[])
{
   int i, N = 20;
   float x[20], y[20], z[20];			/* Static geometry */

   srand48((long)time(NULL));			/* Seed random numbers */
   for (i=0;i<N;i++) {
      x[i] = drand48()*4.0-2.0;			/* Random positions */
      y[i] = drand48()*4.0-2.0;
      z[i] = drand48()*4.0-2.0;
   }
  
   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-2.,2., -2.,2., -2.,2.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   cs2scb(&cb);                                  /* Install the callback */
   cs2sncb(&ncb);                              /* Install number callback */
 
   s2slw(4);					/* Set line width */
   s2sci(S2_PG_YELLOW);
   s2pt(N,x,y,z,1);				/* Draw static data */
   s2slw(1);

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
