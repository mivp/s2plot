/* ns2svrl.c
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
#include <time.h>
#include "s2plot.h"

/* Global variables */
int vid;				/* ID for volume render object */
float tr[12];				/* Transformation matrix */
float ***volume;			/* The data cube */
int N = 5000;				/* Number of data points */
float *x, *y, *z;			/* Arrays for data values */

float gamin, gamax, gdmin, gdmax;

float ***initVolume(int nx, int ny, int nz)
/* Allocate memory and initialise a data cube */
{
   float ***volume;
   int i, j, k;

   volume = (float ***)malloc(nx * sizeof(float **));
   if (volume == NULL) {
      fprintf(stderr,"Failed to allocate %ld bytes\n",nx*sizeof(float **));
      exit(-1);
   }
   for (i=0;i<nx;i++) {
      volume[i] = (float **)malloc(ny * sizeof(float *));
      if (volume[i] == NULL) {
         fprintf(stderr,"Failed to allocate %ld bytes\n",nx*sizeof(float *));
         exit(-1);
      }
      for (j=0;j<ny;j++) {
         volume[i][j] = (float *)malloc(nz * sizeof(float));
         if (volume[i][j] == NULL) {
            fprintf(stderr,"Failed to allocate %ld bytes\n",nx*sizeof(float));
            exit(-1);
         }
         for (k=0;k<nz;k++) {
            volume[i][j][k] = 0.0;		/* Initialise */
         }
      }
   }

   return volume;
}

int mode = 0;		/* Parameter changing mode for callback */
void ncb(int *N)
{
   if (*N < 9) mode = *N;
}

void cb(double *t, int *kc)
/* A dynamic callback */
{
   static int lkc = 0;

   if (lkc != *kc) {
      switch(mode) {
         case 1 : gamin -= 0.1; 			/* Lower alpha min */
		  if (gamin < 0) gamin = 0.0;
 		  break;
         case 2 : gamin += 0.1; 			/* Raise alpha min */
		  if (gamin > gamax) gamin = gamax;
 		  break;
         case 3 : gamax -= 0.1; 			/* Lower alpha max */
		  if (gamax < gamin) gamax = gamin;
 		  break;
         case 4 : gamax += 0.1; 			/* Raise alpha max */
		  if (gamax > 1.0) gamax = 1.0;
 		  break;
         case 5 : gdmin -= 0.1; 			/* Lower data min */
		  if (gdmin < 0) gdmin = 0.0;
 		  break;
         case 6 : gdmin += 0.1; 			/* Raise data min */
		  if (gdmin > gdmax) gdmin = gdmax;
 		  break;
         case 7 : gdmax -= 0.1; 			/* Lower data max */
		  if (gdmax < gdmin) gdmax = gdmin;
 		  break;
         case 8 : gdmax += 0.1; 			/* Raise data max */
		  if (gdmax > 2.0) gdmax = 2.0;
 		  break;
         case 0 : gamin = 0.0; 				/* Reset */
		  gamax = 1.0;
		  gdmin = 0.0;
		  gdmax = 2.0;
 		  break;
      }
      ns2svrl(vid, gdmin, gdmax, gamin, gamax);
		/* Update the volume render data and alpha channel range */
   }

   ds2dvr(vid, 1);			/* Draw the volume render object */

   switch (mode) {			/* Display a label */
     case 0 : s2lab("","","","Reset - press space"); break;
     case 1 : s2lab("","","","Lower alpha min - press space"); break;
     case 2 : s2lab("","","","Raise alpha min - press space"); break;
     case 3 : s2lab("","","","Lower alpha max - press space"); break;
     case 4 : s2lab("","","","Raise alpha max - press space"); break;
     case 5 : s2lab("","","","Lower data min - press space"); break;
     case 6 : s2lab("","","","Raise data min - press space"); break;
     case 7 : s2lab("","","","Lower data max - press space"); break;
     case 8 : s2lab("","","","Raise data max - press space"); break;
   }

   lkc = *kc;
}


int main(int argc, char *argv[])
{
   int i; 					/* Loop variable */
   int nx = 32, ny = 32, nz = 32;		/* Dimensions of data cube */ 
   char trans;					/* Transparency mode */
   float x1 = -1.0, x2 = +1.0;			/* Data range along axes */
   float y1 = -1.0, y2 = +1.0;
   float z1 = -1.0, z2 = +1.0;
   int vi, vj, vk;				/* Temporary variables */
   float dx, dy, dz;				/* Temporary variables */
   COLOUR amb = {0.8, 0.8, 0.8};		/* Ambient light */

   srand48((long)time(NULL));			/* Seed random numbers */

   volume = initVolume(nx, ny, nz); 		/* Allocate memory */  

   for (i=0;i<12;i++) {				/* Set-up transfrom matrix */
      tr[i] = 0.0;
   }
   tr[ 0]  = x1; 				/* Mapping from data cube */
   tr[ 1]  = (x2-x1)/(float)(nx-1.0);		/* to physical coordinates */
   tr[ 4]  = y1; 
   tr[ 6]  = (y2-y1)/(float)(ny-1.0);
   tr[ 8]  = z1; 
   tr[11]  = (z2-z1)/(float)(nz-1.0);

   gdmin = 0.0;			/* Mininum data value in volume to plot */
   gdmax = 2.0;			/* Maximum data value in volume to plot */
   gamin = 0.0;			/* Minimum alpha channel value */
   gamax = 0.8;			/* Maximum alpha channel value */
   trans = 't';			/* Transparency type */

   dx = tr[1]*0.5;		/* Offsets for window - voxels are pixel */
   dy = tr[6]*0.5;		/*  centred */
   dz = tr[11]*0.5;

   s2opend("/S2MONO",argc,argv);			/* Open the display */
   s2swin(x1-dx,x2+dx, y1-dy,y2+dy, z1-dz,z2+dz);	/* Set window coords */
   s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);		/* Draw coord box */

   x = (float *)calloc(N, sizeof(float));
   y = (float *)calloc(N, sizeof(float));
   z = (float *)calloc(N, sizeof(float));

   for (i=0;i<N;i++) {			/* Create N random (x,y,z) values */
      vi = (int)(drand48()*(nx));
      vj = (int)(drand48()*(ny));
      vk = (int)(drand48()*(nz));
      x[i] = vi *  tr[1] + tr[0];
      y[i] = vj *  tr[6] + tr[4];
      z[i] = vk * tr[11] + tr[8];


      volume[vi][vj][vk]+=1.0-drand48()*drand48();	
					/* Give a value to volume */
   }
 
   s2scir(1000,2000);			/* Set colour range */
   s2icm("mgreen",1000,2000);		/* Install colour map */
   vid = ns2cvr(volume, nx, ny, nz, 0, nx-1, 0, ny-1, 0, nz-1, 
		tr, trans, gdmin, gdmax, gamin, gamax);
					/* Create the volume render object */

   cs2scb(cb);				/* Install a dynamic callback */
   cs2sncb(ncb);

   fprintf(stderr,"Press a number 1..8 to select mode\n");
   fprintf(stderr,"Press <spacebar> to change level\n");
   fprintf(stderr,"Press 0 and then <spacebar> to reset\n");

   ss2srm(SHADE_FLAT);			/* Set shading type to FLAT */
   ss2sl(amb, 0, NULL, NULL, 0);	/* Ambient lighting only */

   s2show(1);				/* Open the s2plot window */

   return 1;
}
