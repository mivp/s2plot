/* cs2srcb.c
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

/* cs2srcb.c: demo for cs2srcb function (set remote callback)
 *
 * to use:
 *
 * 1. build cs2srcb
 * 2. setenv S2PLOT_REMOTEPORT 3333 (or another number)
 * 3. run cs2srcb
 * 4. from another terminal:
 *    telnet localhost 3333
 *    S2 setcolor 0.9,0.1,0.5
 * 5. note changed billboard colour 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "s2plot.h"

unsigned int tid;

float _bb_r, _bb_g, _bb_b;

int remote_cb(char *data);
void cb(double *t, int *kc);

int main(int argc, char *argv[])
{
   int width = 16, height = 16;		/* Dimensions of texture */
   int i, j;				/* Loop variables */
   int idx;				/* Index into texture array */
   unsigned char *tex;				/* Array of texture values */

   srand48((long)time(NULL));			/* Seed random numbers */

   s2opend("/?",argc,argv);			/* Open the display */
   s2svp(-1.0,1.0, -1.0,1.0, -1.0,1.0);		/* Set the viewport coords */
   s2swin(-1.0,1.0, -1.0,1.0, -1.0,1.0);	/* Set the window coordinates */
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);	/* Draw a bounding box */

   tid = ss2ct(width, height);		/* Create a new texture */
   tex = ss2gt(tid, &width, &height);	/* Get the texture */

   for (j=0;j<height;j++) {
      for (i=0;i<width;i++) {
         idx = (j*width + i) * 4;		/* Stored as (r,g,b,alpha) */
         tex[idx  ] = 255;	                /* solid red */
         tex[idx+1] = 255;			/* Green */
         tex[idx+2] = 255;			/* Blue */
	 tex[idx+3] = 127 * drand48()+128;      /* Random transparency */
      }
   }
   ss2pt(tid);				/* Push texture for usage */

   _bb_r = 0.4;                         /* Set initial colour of billboard */
   _bb_g = 0.7;
   _bb_b = 0.9;

   cs2scb(&cb);				/* Install a callback */
   cs2srcb(&remote_cb);                 /* Install remote callback */

   s2show(1);				/* Open the s2plot window */

   return 1;
}

void cb(double *t, int *kc)
{
   float x = 0.0, y = 0.3, z = 0.4;			/* Location */
   float isize = 0.02;					/* Texture scale */
   float str_x = 0.0, str_y = 0.0, str_z = 0.0;		/* No stretch */
   float alpha = 0.9; 					/* Alpha channel */
   char trans = 't';					/* Transparency */
   
   ds2bb(x,y,z, str_x,str_y,str_z, isize, _bb_r, _bb_g, _bb_b, tid, alpha, trans);
					/* Draw the billboard */
}


int remote_cb(char *data) {
  // this remote callback will receive strings coming in on 
  // the S2PLOT_REMOTEPORT port.

  char *contype, *ctype, *cargs;

  contype = strtok(data, " ");
  
  // for this demo, we will only parse commands preceded by "S2"
  if (!contype || strcmp(contype, "S2")) {
    // return 0 to indicate we have not "consumed" this event
    fprintf(stderr, "remote_cb: no valid command prefix\n");
    return 0;
  }

  ctype = strtok(NULL, " ");
  if (!ctype) {
    fprintf(stderr, "remote_cb: no command present\n");
    return 0;
  }

  int consumed = 0;

  // the only command we accept is "setcolor" to set the 
  // billboard color
  if (!strcmp(ctype, "setcolor")) {
    cargs = strtok(NULL, " ");
    if (!cargs) {
      fprintf(stderr, "remote_cb: no argument for setcolor present\n");
      return 0;
    }
    float vr, vg, vb;
    sscanf(cargs, "%f,%f,%f", &vr, &vg, &vb);
    
    if ((vr >= 0.) && (vr <= 1.) && 
	(vg >= 0.) && (vg <= 1.) &&
	(vb >= 0.) && (vb <= 1.)) {
      _bb_r = vr;
      _bb_g = vg;
      _bb_b = vb;
    }
    
    consumed = 1;
  } else {
    fprintf(stderr, "remote_cb: unknown command %s\n", ctype);
  }

  return consumed;
}
