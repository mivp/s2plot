/* ds2ahx.c
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

#define N 100                   /* Number of points */

XYZ xyz[N];                     /* Global variable: positions */
COLOUR col[N];                  /* Colour of the point */
int sel[N];                     /* Global varibale: selected data */

unsigned int itex, hitex;	/* Texture ids for de/selected textures */

void pick(unsigned int *id)
/* Callback when an item is clicked */
{
   sel[*id] = (sel[*id]+1)%2;                   /* Toggle the selection */
   cs2th(*id);                         		/* Update handle list */
}

void cb(double *t, int *kc)
{
   COLOUR normal = { 1.0, 1.0, 1.0 };
   COLOUR hilite = { 1.0, 1.0, 1.0 };           /* Highlight colour */
   float hsize = 0.02;                          /* Size of handle */
   int i;                                       /* Loop variable */

   for (i=0;i<N;i++) {
      ns2vthpoint(xyz[i], col[i], 3);           /* Draw thick point */
      ds2ahx(xyz[i], hsize, itex, hitex, normal, hilite, i, sel[i]);
                                          /* Draw handles with special textures */
   }
}

int main(int argc, char *argv[])
{
   int i, j;                                    /* Loop varibale */

   srand48((long)time(NULL));                   /* Seed random numbers */

   fprintf(stderr,"Shift-s to toggle handles\nShift-c to toggle crosshair\n");
   fprintf(stderr,"Right mouse select\n");

   s2opend("/?",argc, argv);                    /* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);              /* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);  /* Draw coordinate box */

   for (i=0;i<N;i++) {                          /* Set-up globals */
      xyz[i].x = drand48()*2.0 - 1.0;           /* Random position */
      xyz[i].y = drand48()*2.0 - 1.0;
      xyz[i].z = drand48()*2.0 - 1.0;
      col[i].r = drand48();                     /* Random colour */
      col[i].g = drand48();
      col[i].b = drand48();
      sel[i]   = 0;                             /* Not currently selected */
   }

   itex = ss2ct(16,16);				/* Create a new texture */
   hitex = ss2ct(16,16);			/* Create a new texture */
   int width, height;				/* Texture size */
   unsigned char *tex1 = ss2gt(itex, &width, &height);	/* Get texture memory */
   unsigned char *tex2 = ss2gt(hitex, &width, &height);	/* Get texture memory */

   int iid;					/* Index into texture arrays */
   for (j=0;j<height;j++) {			
      for (i=0;i<width;i++) {
         iid = (j*width + i)*4;			/* Stored as (r, g, b, alpha) */
         tex1[iid  ] = drand48()*255;		/* Randomish Red */
         tex1[iid+1] = 0;			/* Green */
         tex1[iid+2] = 0;			/* Blue */
         tex1[iid+3] = 100;			/* Alpha channel: 100/255 */

         tex2[iid  ] = 0;			/* Red */
         tex2[iid+1] = drand48()*255;		/* Randomish Green */
         tex2[iid+2] = 0;			/* Blue */
         tex2[iid+3] = 100;			/* Alpha channel: 100/255 */
      }
   }
   ss2pt(itex);					/* Push back textures for use */
   ss2pt(hitex);

   cs2thv(1);					/* Make sure handles are visible */

   cs2scb(&cb);                                 /* Install dynamic callback */
   cs2shcb(&pick);                              /* Install handle callback */

   s2show(1);                                   /* Open the s2plot window */

   return 1;
}
