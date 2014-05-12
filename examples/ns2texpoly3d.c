/* ns2texpoly3d.c
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

unsigned int texid;				/* 3D Texture ID */

void cb(double *t, int *kc)
{
   int nverts = 4;				/* Number of vertices in polygon */
   char trans = 't';				/* 3d texture transparency */
   float alpha = 0.7;				/* Alpha channel value */

   XYZ iP[4];					/* Array of polygon vertices */
   XYZ iTC[4];					/* Array of texture coordinates */
   static float texidx = 0.0;			/* Current z-texture coordinate */

   iP[0].x = iP[3].x = -1.0;			/* Define polygon vertices */
   iP[1].x = iP[2].x = +1.0;
   iP[0].y = iP[1].y = +1.0;
   iP[2].y = iP[3].y = -1.0;
   iP[0].z = iP[1].z = iP[2].z = iP[3].z = texidx*2.0 - 1.0;

   iTC[0].x = iTC[3].x = 0.0; 			/* Define texture coordinates */
   iTC[1].x = iTC[2].x = 1.0; 
   iTC[0].y = iTC[1].y = 1.0; 
   iTC[2].y = iTC[3].y = 0.0; 
   iTC[0].z = iTC[1].z = iTC[2].z = iTC[3].z = texidx;

   COLOUR col = { 1,1,1 };			/* Draw the polygon boundaries */
   ns2vline(iP[0],iP[1],col);
   ns2vline(iP[1],iP[2],col);
   ns2vline(iP[2],iP[3],col);
   ns2vline(iP[3],iP[0],col);

   ns2texpoly3d(iP, iTC, nverts, texid, trans, alpha);	/* Draw 3D texture */

   texidx += (1.0/320.0);			/* Choose next z-texture coordinate */
   if (texidx > 1.0) texidx = 0.0;
}

int main(int argc, char *argv[])
{
   int Nx = 32, Ny = 32, Nz = 32;		/* Texture dimensions */
   int w,h,d;					/* Returned dimensions */

   s2opend("/?",argc,argv);			/* Open the s2plot display device */
   s2swin(-1,1,-1,1,-1,1);			/* Set the world coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);
	
   texid = ss2c3dt(Nx, Ny, Nz);			/* Create a new 3d texture */
   unsigned char *bits = (unsigned char *)ss2g3dt(texid, &w, &h, &d);
						/* Get the texture data for modification */
   int i, j, k;
   long idx;
   float x,y,z;  
   for (i=0;i<Nx;i++) {
      x = -1.0 + i*(2.0/(float)(Nx-1));
      for (j=0;j<Ny;j++) {
         y = -1.0 + j*(2.0/(float)(Ny-1));
         for (k=0;k<Nz;k++) {
            z = -1.0 + k*(2.0/(float)(Nz-1));
            idx = ((i*Ny + j)*Nz + k)*4;
	    if ((x*x + y*y + z*z) < 1.0) {	/* 3D solid red sphere */
               bits[idx + 1 ] = 255;
               ns2point(x,y,z,1,1,1);		/* Plot point in sphere */
            } else { 
               bits[idx + 0 ] = 0;
            }

	    bits[idx + 1 ] = 0;			/* Blue channel */
	    bits[idx + 2 ] = 0;			/* Green channel */
	    bits[idx + 3 ] = 255;		/* Alpha channel */
         }     
      }     
   }     

   fprintf(stderr, "3d texture IS %d x %d x %d\n", w, h, d);
   ss2pt(texid);				/* Push back the texture */

   cs2scb(cb);					/* Install a dynamic callback */
   				
   s2show(1);					/* Display the geometry */

   return 0;
}
