/* ss2gpix.c
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
   unsigned int sw, sh;			/* Screen buffer size */
   int w, h;				/* Texture size: square texture */
   static unsigned char *screen = NULL;	/* Memory for RGB pixel map */
   static unsigned char *texture = NULL;/* Billboard texture to make */
   int i, j;				/* Loop variables */
   static int id = -1;			/* ID of texture: default value */
 
   COLOUR col = { 1, 1, 1 };		/* Billboard texture colour */
   XYZ pos = { 0, 0, 0 };		/* Billboard texture location */
   XYZ is  = { 0, 0, 0 };		/* Billboard stretch factor */
   int idx1, idx2;			/* Index into texture arrays */

   screen = ss2gpix(&sw, &sh); 		/* Capture the screen */
   
   if (sw < sh) { h = sw; w = sw; }	/* Choose a square region */
   else { w = sh; h = sh; }

   int dx = (sw - w)/2;			/* Find offset from screen centre */
   int dy = (sh - h)/2;

   id = ss2ct(w,h);			/* Create a new texture */

   texture = ss2gt(id, &w, &h);		/* Get memory for this texture */

   for (i=0;i<h;i++) {			/* Loop over height */
      for (j=0;j<w;j++) {		/* Loop over width */
         idx1 = (j*w + i)*4; 		/* Index to write into new texture */
         idx2 = ((j+dy)*sw + (i+dx))*3;	/* Index to copy from screen texture */
         texture[idx1  ] = screen[idx2  ];
         texture[idx1+1] = screen[idx2+1];
         texture[idx1+2] = screen[idx2+2];
      }
   }
   ss2pt(id);				/* Restore the texture for use */

   ss2tsc("c");				/* Change to screen coordinates */
   float xmin, xmax, ymin, ymax;	/* Boundaries of window */
   xmin = 0.01; xmax = 0.99;
   ymin = 0.01; ymax = 0.99;
   
   float ar = ss2qar();			/* Get screen aspect ratio */
   if (ar > 1.0) { 			/* Fiddle coordinates to get */
      xmin = 0.5-(0.49)/ar;		/*  square region in window centre */
      xmax = 0.5+(0.49)/ar;
   } else {
      ymin = 0.5-(0.49)*ar;
      ymax = 0.5+(0.49)*ar;
   }
   					/* Draw square with thick lines */
   ns2thline(xmin,ymin,0.01, xmax,ymin,0.01, 1,0,1, 3);
   ns2thline(xmin,ymax,0.01, xmax,ymax,0.01, 1,0,1, 3);
   ns2thline(xmin,ymin,0.01, xmin,ymax,0.01, 1,0,1, 3);
   ns2thline(xmax,ymin,0.01, xmax,ymax,0.01, 1,0,1, 3);
   ss2tsc("");				/* Return to world coordinates */

   ds2vbb(pos, is, 0.05, col, id, 1.0, 'o'); 	/* Draw billboard texture */

   if (screen != NULL) { 		/* Free screen texture memory */
      free(screen);   
      screen = NULL; 
   }
   if (texture != NULL) { 		/* Free billboard texture memory */
      free(texture); 
      texture = NULL; 
   }
  
}

int main(int argc, char *argv[])
{
   fprintf(stderr,"Use +/- to stare infinity in the face...\n"); 

   s2opend("/s2mono", argc, argv);             	/* Open in mono mode */
   s2swin(-1.,1., -1.,1., -1.,1.);              /* Set window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   cs2scb(&cb);					/* Install dynamic callback */

   s2show(1);					/* Open the s2plot window */

   return 1;
}
