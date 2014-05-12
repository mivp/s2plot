/* s2anim.c
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
 * $Id: s2anim.c 5763 2012-09-25 04:14:25Z dbarnes $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "s2anim.h"
#include "s2plot.h"

#define XM1 100
#define XM2 800

Camera camera;

long countLines(FILE *fp)
/* Count the number of lines in the specified file */
{
   long N = 0;
   long int ptr;
   char string[1024];

   ptr = ftell(fp);		/* Current location in file */

   fgets(string,1024,fp);
   while (!feof(fp)) { fgets(string,1024,fp); N++; }
   fseek(fp, ptr, SEEK_SET);    /* Back to where file was */

   return N;
}


Slider initSlider(int id, float dmin, float dmax, float smin, float smax,
                float base, float width, int c1, int c2,
                char *cmap, char *label, int lower, int upper,
                int direct, int nstep, int toggle, int visible)
/* Initialise a slider data type */
{
   Slider slider;

   slider.id   = id;		/* ID for this slider */
   slider.dmin = dmin;		/* Data range minimum */
   slider.dmax = dmax;		/* Data range maximum */
   slider.smin = smin;		/* Screen position minimum */
   slider.smax = smax;		/* Screen position maximum */
   slider.base = base;		/* The other coordinate */
   slider.width = width;	/* Width of the slider */
   slider.c1 = c1;		/* Colour index of minimum */
   slider.c2 = c2;		/* Colour index of maximum */
   slider.level = slider.dmin;	/* Set the starting level */
   slider.last = ((slider.level-slider.dmin)/(slider.dmax-slider.dmin))*
                        (slider.smax-slider.smin) + slider.smin;
				/* Set the maximum slider level */

   slider.cmap  = (char *)calloc(CMAPLEN, sizeof(char));	
   sprintf(slider.cmap,"%s",cmap);	/* Colour map name */

   slider.label = (char *)calloc(strlen(label)+1, sizeof(char));
   sprintf(slider.label,"%s",label); 	/* Slider label */

   slider.toggle = toggle;	/* Whether slider has an activation toggle */
   slider.visible = visible;	/* Is the slider visible? */

   slider.lower = lower;	/* Only needed if more than one slider handle */
   slider.upper = upper;
   if ((lower < 0) && (upper < 0)) {
      slider.Nhand = 1;
   } else if (((lower <0) && (upper >= 0)) || ((lower >= 0) && (upper < 0))) {
      slider.Nhand = 2;
   } else {
      slider.Nhand = 3;
   }

   slider.direct = direct;	/* Axis direction: X or Y */
   slider.nstep  = nstep;	/* Number of discrete slider increments */
   slider.step   = (slider.smax-slider.smin)/(float)nstep;	
				/* Size of slider step */
   return slider;
}

Bounds loadFrame(int Nframe, int Np, PlotMode lpm)
/* Read the Nframe numbered-frame depending on format type */
{
   FILE *fp;
   fp = fopen(framenames[Nframe], "r");
   if (fp == NULL) {
      fprintf(stderr,"Could not open file: %s [%d]\n",framenames[Nframe], Nframe);
      exit(1);
   }

   Ndata[Nframe] = Np;          /* Number of particles in this frame */
				/* All frames should have same value! */

   data[Nframe] = (XYZ *)calloc(Ndata[Nframe], sizeof(XYZ));
   if (data[Nframe] == NULL) {
      fprintf(stderr,"Failed to allocate %ld bytes for file: %s\n",
		(long)(sizeof(XYZ)*Ndata[Nframe]), framenames[Nframe]);
      exit(0);
   }
   rgb[Nframe]  = (COLOUR *)calloc(Ndata[Nframe], sizeof(COLOUR));
   if (rgb[Nframe] == NULL) {
      fprintf(stderr,"Failed to allocate %ld bytes for file: %s\n",
		(long)(sizeof(COLOUR)*Ndata[Nframe]), framenames[Nframe]);
      exit(0);
   }

   int i, j;                            /* Loop variable */
   XYZ min = { +1e9, +1e9, +1e9 };      /* Data range */
   XYZ max = { -1e9, -1e9, -1e9 };
   float dummy[3];                      /* Used to read in data */
   short int dshort[3];                 /* Format f4 only */

   unsigned char *tmp = NULL;           /* Used to do the endian swap */
   unsigned char tmp2[4];

   char string[256];                    /* A line from the data file */

   int dint;
   if (lpm.format == 2) {
      fread(&dint, sizeof(int), 1, fp); /* Starts with Np - but skip it */
   }
   for (i=0;i<Np;i++) {                 /* Loop over particles */
      rgb[Nframe][i].r = 1;             /* Set a default colour */
      rgb[Nframe][i].g = 1;
      rgb[Nframe][i].b = 1;

      switch (lpm.format) {
         case 0 : fread(&dummy, sizeof(float), 3, fp);   /* binary x,y,z */
                  if (lpm.endian) {
                     tmp = (unsigned char *)dummy;
                     for (j=0;j<3;j++) {
                        tmp2[0] = tmp[j*4+3];
                        tmp2[1] = tmp[j*4+2];
                        tmp2[2] = tmp[j*4+1];
                        tmp2[3] = tmp[j*4];
			float *_tmp_f = (float *)tmp2;
                        dummy[j] = *_tmp_f;
                     }
                  }
                  data[Nframe][i].x = dummy[0];
                  data[Nframe][i].y = dummy[1];
                  data[Nframe][i].z = dummy[2];
                  break;
         case 1 : fgets(string,128,fp);                  /* ASCII x,y,z */
                  sscanf(string,"%lf %lf %lf",
                        &data[Nframe][i].x, &data[Nframe][i].y, 
			&data[Nframe][i].z);
                  break;
         case 2 : fread(&dshort, sizeof(short int), 3, fp);     
							/* binary x,y,z */
                  if (lpm.endian) {
                     tmp = (unsigned char *)dshort;
                     for (j=0;j<3;j++) {
                        tmp2[0] = tmp[j*4+3];
                        tmp2[1] = tmp[j*4+2];
                        tmp2[2] = tmp[j*4+1];
                        tmp2[3] = tmp[j*4];
			float *_tmp_f = (float *)tmp2;
                        dummy[j] = *_tmp_f;
                     }
                  }
                  data[Nframe][i].x = dummy[0];
                  data[Nframe][i].y = dummy[1];
                  data[Nframe][i].z = dummy[2];
                  break;
         case 3 : fgets(string,128,fp);                 /* ASCII x,y,z,r,g,b */
                  sscanf(string,"%lf %lf %lf %lf %lf %lf",
                        &data[Nframe][i].x, &data[Nframe][i].y, 
			&data[Nframe][i].z, &rgb[Nframe][i].r, 
			&rgb[Nframe][i].g, &rgb[Nframe][i].b);
                  break;
         case 4 : sscanf(string,"%lf %lf %lf %df",      /* Ascii x,y,z,c */
                        &data[Nframe][i].x, &data[Nframe][i].y, 
			&data[Nframe][i].z, &dint);
                  rgb[Nframe][i].r = (float)dint;
                  rgb[Nframe][i].g = -1;
                  rgb[Nframe][i].b = -1;
                  break;
         case 5 : fread(&dummy, sizeof(float), 3, fp);  /* Binary x,y,z,r,g,b */
                  if (lpm.endian) {
                     tmp = (unsigned char *)dummy;
                     for (j=0;j<3;j++) {
                        tmp2[0] = tmp[j*4+3];
                        tmp2[1] = tmp[j*4+2];
                        tmp2[2] = tmp[j*4+1];
                        tmp2[3] = tmp[j*4];
			float *_tmp_f = (float *)tmp2;
                        dummy[j] = *_tmp_f;
                     }
                  }
                  data[Nframe][i].x = dummy[0];
                  data[Nframe][i].y = dummy[1];
                  data[Nframe][i].z = dummy[2];

                  fread(&dummy, sizeof(float), 3, fp);
                  if (lpm.endian) {
                     tmp = (unsigned char *)dummy;
                     for (j=0;j<3;j++) {
                        tmp2[0] = tmp[j*4+3];
                        tmp2[1] = tmp[j*4+2];
                        tmp2[2] = tmp[j*4+1];
                        tmp2[3] = tmp[j*4];
			float *_tmp_f = (float *)tmp2;
                        dummy[j] = *_tmp_f;
                     }
                  }
                  rgb[Nframe][i].r = dummy[0];
                  rgb[Nframe][i].g = dummy[1];
                  rgb[Nframe][i].b = dummy[2];
                  break;

         case 95 : fread(&dummy, sizeof(float), 3, fp);  /* Binary x,y,z,v where v is in [0,1] to be colourmapped */
                  if (lpm.endian) {
                     tmp = (unsigned char *)dummy;
                     for (j=0;j<3;j++) {
                        tmp2[0] = tmp[j*4+3];
                        tmp2[1] = tmp[j*4+2];
                        tmp2[2] = tmp[j*4+1];
                        tmp2[3] = tmp[j*4];
			float *_tmp_f = (float *)tmp2;
                        dummy[j] = *_tmp_f;
                     }
                  }
                  data[Nframe][i].x = dummy[0];
                  data[Nframe][i].y = dummy[1];
                  data[Nframe][i].z = dummy[2];

                  fread(&dummy, sizeof(float), 1, fp);
                  if (lpm.endian) {
                     tmp = (unsigned char *)dummy;
                     for (j=0;j<1;j++) {
                        tmp2[0] = tmp[j*4+3];
                        tmp2[1] = tmp[j*4+2];
                        tmp2[2] = tmp[j*4+1];
                        tmp2[3] = tmp[j*4];
			float *_tmp_f = (float *)tmp2;
                        dummy[j] = *_tmp_f;
                     }
                  }
		  int idx = XM1 + (int)(dummy[0] * (float)(XM2-XM1));
		  idx = idx < XM1 ? XM1 : idx > XM2 ? XM2 : idx;
		  float r, g, b;
		  s2qcr(idx, &r, &g, &b);
		  rgb[Nframe][i].r = r;
		  rgb[Nframe][i].g = g;
		  rgb[Nframe][i].b = b;
                  break;
      }


/* Determine maximum and minimum */
      if (data[Nframe][i].x < min.x) min.x = data[Nframe][i].x;
      if (data[Nframe][i].x > max.x) max.x = data[Nframe][i].x;
      if (data[Nframe][i].y < min.y) min.y = data[Nframe][i].y;
      if (data[Nframe][i].y > max.y) max.y = data[Nframe][i].y;
      if (data[Nframe][i].z < min.z) min.z = data[Nframe][i].z;
      if (data[Nframe][i].z > max.z) max.z = data[Nframe][i].z;
   }


/* Adjust the data bounds so that each axis side is the same length */
   float dx, dy, dz;
   float mx, my, mz;
   dx = (max.x - min.x)/2.0; mx = (max.x + min.x)/2.0;
   dy = (max.y - min.y)/2.0; my = (max.y + min.y)/2.0;
   dz = (max.z - min.z)/2.0; mz = (max.z + min.z)/2.0;
   if (dy > dx) dx = dy;
   if (dz > dx) dx = dz;
   min.x = mx - dx; max.x = mx + dx;
   min.y = my - dx; max.y = my + dx;
   min.z = mz - dx; max.z = mz + dx;

   Bounds bounds;
   bounds.min = min;
   bounds.max = max;
   fclose(fp);

   return bounds;
}

void setCamera(Camera camera)
/* A viewfile was read - so better put the camera where we asked! */
{
   ss2sc(camera.pos, camera.up, camera.view, camera.wc);
   ss2scf(camera.focus, camera.wc);
}

void slider_cb(double *t, int *kc)
/* This callback determines what frame to show based on current */
/* slider position */
{
   float asp = ss2qar();		/* Get aspect ratio of window */

   XYZ han = { 0, 0, 0.1 };		/* Slider handle coordinates */
   long i;				/* Loop variable */
   //static int flag = -1;                     /* Set some values first time through */
   float loadbar;			/* Current frame loaded */
   int displayed_new = 1;                      /* true if we displayed a new frame this cb */

   if (Nmax > 1) {		/* Only draw slider if multiple frames */
     loadbar = ((Nload-slider.dmin)/(slider.dmax-slider.dmin))*
                        (slider.smax-slider.smin) + slider.smin;

     ss2txh(0);
     if (pm.slider) {			/* Is slider visible? <0> to toggle */
        ss2txh(1);
        COLOUR col = { 0.0, 0.0, 0.0 };
        ss2tsc("clr");			/* Draw in screen coordinates */
  
        drawSlider(slider, 0.01, Nmax);

         if (loadFlag < 0) {                                  /* Don't go beyond last frame loaded */
            if (slider.last > loadbar) slider.last = loadbar;
         } else {
            if (slider.last > slider.smax) slider.last = slider.smax;
         }
         if (slider.last < slider.smin) slider.last = slider.smin;
    
         if (slider.direct == _XX) {          /* Only version implemented here... */
            han.y = slider.base;
            han.x = slider.last;
            ds2ah(han, slider.width/2.0/asp, col, col, slider.id, 0);

            ns2thline(han.x,slider.base-slider.width/2.0,0.01,
                           han.x,slider.base+slider.width/2.0,0.01, 1.0, 0.0, 0.0,3);
    
            if (loadFlag < 0) {                        /* Draw line showing where loaded to */
                ns2thline(loadbar,slider.base-slider.width/2.0,0.01,
                           loadbar,slider.base-slider.width,0.01, 0.0, 1.0, 0.0,3);
            }
         }
         slider.level = (slider.last-slider.smin)/(slider.smax-slider.smin)*
                                       (slider.dmax-slider.dmin) + slider.dmin;
                                                       /* Update where the slider is */
      }
      ss2tsc("");
   }

   int ilev = (int) slider.level;                     /* Curent slider value */

   int minlev = ilev - pm.nblur;
   minlev = minlev < 0 ? 0 : minlev;

   XYZ zeroStretch;

   if (ilev < Nload) {

      // if interpolating and not on the final frame,
      // calculate the frame that is requried
      if ((pm.interp > 1) && (ilev < Nload))
      {
        for (i=0; i<Ndata[ilev];i++)
         CalcPoint(&(pdata[i]), &(data[ilev][i]), &(data[ilev+1][i]), 
                        ((float) interp / (float) pm.interp));
      } 
      // just have the pdata pointer point to the current frame
      else
      {
        pdata = data[ilev];
      }

      if (pm.symbol < 0) {
        /* Plot using (thick) dots */
        if (pm.usebillboards) {
         for (i=0;i<Ndata[ilev];i++) {
            zeroStretch.x = (pdata[i].x - data[minlev][i].x);
            zeroStretch.y = (pdata[i].y - data[minlev][i].y);
            zeroStretch.z = (pdata[i].z - data[minlev][i].z);
            ds2vbb(pdata[i], zeroStretch, pm.objsize * 0.03, 
                      rgb[ilev][i], texid, 0.5, 't');
         }
        } else {
         for (i=0;i<Ndata[ilev];i++) {
            ns2vthpoint(pdata[i], rgb[ilev][i], pm.objsize);
         }
        }
      } else {
        /* Plot using symbols */
         for (i=0;i<Ndata[ilev];i++) {
            ns2vm(pm.symbol, pm.objsize, pdata[i], rgb[ilev][i]);
         }
      }
   } else {
      ilev = Nload - 1;
      displayed_new = 0;
      for (i=0;i<Ndata[ilev];i++) {
         ns2vthpoint(pdata[i], rgb[ilev][i], pm.objsize);
      }
   }

   static int dir = +1;

   static int lkc = 0;
   if (*kc != lkc) { 
      if (pm.looping == 0) pm.looping = 1;
      else pm.looping = -pm.looping;
   }
   lkc = *kc;

   // only update the slider upon a frame increment
   gettimeofday(&currTime, NULL);
   if ((displayed_new) && (timeCompare(currTime, nextTime) > 0)) {
      setNextUpdate(&nextTime, pm.fps);      

      // if we are 'playing' and interpolating, advance 
      if ((pm.looping > 0) && (pm.interp > 1))
      {
         interp++;
         if (interp >= pm.interp)
          interp = 0;
      }

      // if we need to advance to the next frame
      if (interp == 0) 
      {
        switch (pm.looping) 
        {
         case 1 : 
            slider.level++;
            if (slider.level > slider.dmax) 
               slider.level = slider.dmin;
            slider.last = ((slider.level-slider.dmin)
                                     /(slider.dmax-slider.dmin))*
                                    (slider.smax-slider.smin) + slider.smin;
             break;

         case 2 : 
            if (dir > 0) 
            {
               slider.level++;
               if (slider.level > slider.dmax) 
               {
                  slider.level = slider.dmax; 
                   dir = -1; 
               }
            } else if (dir < 0) {
               slider.level--;
               if (slider.level < slider.dmin) 
               { 
                  slider.level = slider.dmin; 
                  dir = +1; 
               }
            }
            slider.last = ((slider.level-slider.dmin)
                                 /(slider.dmax-slider.dmin))*
                                  (slider.smax-slider.smin) + slider.smin;
            break;
        }
      }
   }
}


void dhcb(int *id, XYZ *p)
/* Drag a handle and update its position - works only along specified axis */
{
   if (slider.direct == _YY) {
      slider.last = p->y;
   } else if (slider.direct == _XX) {
      slider.last = p->x;
   }
}


void ncb(int *N)
/* Control the looping type and slider display */
{
    switch (*N) {
        case 0 : pm.slider = (pm.slider + 1)%2;          /* Toggle slider display */
                   break;
        case 1 : pm.looping = 1; break;                      /* Back to start looping */
        case 2 : pm.looping = 2; break;                      /* Back and forth looping */
    }
}

int kcb(unsigned char *key) 
{
   int consumed = 0;

   switch (*key) 
   {
    case ',':
    case '<':
      if (pm.fps > 1)
         pm.fps--;
      consumed = 1;
      break;

    case '.':
    case '>':
      pm.fps++;
      consumed = 1;
      break;
   }

   return consumed;

}

void drawSlider(Slider sl, float scale, int Ns)
/* Draw an onscreen slider */
{
   float asp = ss2qar();                                 /* Aspect ratio of window */
   float x, y;
   float dd = (sl.smax - sl.smin)/(Ns-1);
   float r1, g1, b1;
   float dc = (sl.c2-sl.c1)/(Ns-1);
   float ci;
   int i;
   XYZ P[4];
   COLOUR col = { 1.0, 1.0, 1.0 };

   s2scir(sl.c1, sl.c2);                                 /* Install colour map */
   s2icm(sl.cmap, sl.c1, sl.c2);

   if (sl.direct == _XX) {                                 /* X-axis slider */
      P[0].y = P[3].y = sl.base-sl.width/2.0;
      P[1].y = P[2].y = sl.base+sl.width/2.0;
      P[0].z = P[1].z = P[2].z = P[3].z = 0.02;

      for (i=0;i<(Ns-1);i++) {
         x = sl.smin + i*dd;
         ci = sl.c1 + dc*i;
         s2qcr((int)ci, &r1, &g1, &b1);
         col.r = r1; col.g = g1; col.b = b1;
         P[0].x = P[1].x = x;
         P[2].x = P[3].x = x+dd;             
         ns2vf4(P, col);
      }
   } else if (sl.direct == _YY) {                     /* Y-axis slider */
      P[0].x = P[3].x = sl.base-sl.width/2.0;
      P[1].x = P[2].x = sl.base+sl.width/2.0;
      P[0].z = P[1].z = P[2].z = P[3].z = 0.02;

      for (i=0;i<(Ns-1);i++) {
         y = sl.smin + i*dd;
         ci = sl.c1 + dc*i;
         s2qcr((int)ci, &r1, &g1, &b1);
         col.r = r1; col.g = g1; col.b = b1;
         P[0].y = P[1].y = y;
         P[2].y = P[3].y = y+dd;
         ns2vf4(P, col);
      }
   }

   col.r = col.g = col.b = 1.0;

/* Write the label */
   dd /= 10.0;
   if (sl.direct == _XX) {
      XYZ pos    = { sl.smax+dd, sl.base-sl.width/4.0, 0.1 };
      XYZ right = { scale, 0.0, 0.0 };
      XYZ norm   = { 0.0, scale*asp, 0.0 };
      ns2vtext(pos, right, norm, col, sl.label);
   } else if (sl.direct == _YY) {
      XYZ pos    = { sl.base+sl.width/4.0, sl.smax+dd, 0.1 };
      XYZ right = { 0.0, scale*asp, 0.0 };
      XYZ norm   = { -scale, 0.0, 0.0 };
      ns2vtext(pos, right, norm, col, sl.label);
   }

}

Slider createSlider(float dmin, float dmax, int c1, int c2, 
                                           char *cmap, char *label)
/* Coordinates for drawing slider */
{
   int cstep = c2-c1+1;         /* Colour index range */
   float width = 0.01;         /* On screen width of slider */
   float xlo = 0.05;          /* On screen coordinates for end points of slider */
   float xhi = 0.90;
   float ylo = 0.05 ;                                             

   return initSlider(0, dmin, dmax, xlo, xhi, ylo, width, c1, c2,
                   cmap,label,-1,-1,_XX, cstep, -1, 1);
}

char **readFrameNames(char *fname, int *Nf)
/* Read frames from a file - determines how many frames are present */ 
{
   FILE *fp;
   fp = fopen(fname,"r");
   if (fp == NULL) {
      fprintf(stderr,"Could not find framefile: %s\n",fname);
      exit(-1);
   }

   int lNf = countLines(fp);                     /* How many lines? */

   char **lnames   = (char **)calloc(lNf, sizeof(char *));
   if (lnames == NULL) { 
      fprintf(stderr,"Failed to allocate %ld bytes for frame names in readFrameNames\n",
                                           (long)(sizeof(char *)*lNf));
      exit(-1);
   }

   char string[FNAME_LEN];
   int i;
   for (i=0;i<lNf;i++) {
      fgets(string, FNAME_LEN, fp);
      lnames[i] = (char *)calloc(FNAME_LEN, sizeof(char));
      if (lnames[i] == NULL) { 
         fprintf(stderr,"Failed to allocate %ld bytes for frame name in readFrameNames\n",
                                                (long)(sizeof(char)*FNAME_LEN));
         exit(-1);
      }
    
      sscanf(string,"%s",lnames [i]);
   }
   fclose(fp);

   *Nf = lNf;                                             /* Send back number of rames read */
   return lnames;                                 /* Send back array of file names */
}

void showHelp(FILE *stream)
{
   fprintf(stream,"Required: \n");
   fprintf(stream,"            -np n         number of points to read from each file\n");
   fprintf(stream,"            -fn            read in different formats (currently n=0..5)\n");
   fprintf(stream,"            -Fr <name>            framefile\n");
   fprintf(stream,"Optional:\n");
   fprintf(stream,"            -f             full screen\n");
   fprintf(stream,"            -s             frame sequential stereo\n");
   fprintf(stream,"            -ss            dual screen stereo\n");
   fprintf(stream,"            -si            interleaved stereo\n");
   fprintf(stream,"            -bb            show bounding box (default: off)\n");
   fprintf(stream,"            -ax            show axes (default: off)\n");
   fprintf(stream,"            -gr            show grid (default: off)\n");
   fprintf(stream,"            -as            start in autospin mode (default: off)\n");
   fprintf(stream,"            -os n         object size (default: 1)\n");
   fprintf(stream,"            -B             use (soft) billboards\n");
   fprintf(stream,"            -l             circular looping (default: off)\n");
   fprintf(stream,"            -sl            show slider (default: off)\n");
   fprintf(stream,"            -m s          file with static point markers\n");
   fprintf(stream,"            -swap         big/small endian swap\n");
   fprintf(stream,"            -vf s         start with view file\n");
   fprintf(stream,"            -d             debug mode\n");    
   fprintf(stream,"            -h             this text\n");

   fprintf(stream,"            -Xm            colourmap (only for -f95)\n");

   fprintf(stream,"Press <space> to toggle animation\n");
   fprintf(stream,"Press <0> to toggle slider\n");
   fprintf(stream,"Press <1> to choose 'back to start' looping\n");
   fprintf(stream,"Press <2> to choose 'back and forth' looping\n");
}


PlotMode parse(int argc, char *argv[], PlotMode ss)
{
   int cnt = 1;                                 /* Current argument */

   while (cnt < argc) {                     /* Step through command line arguments */
 
      switch (argv[cnt][1]) {
         case 'n' : /* -np */
                         ss.Np = atol(argv[cnt+1]);            /* Number of points */
                         cnt+=2;
                         break;
         case 'F' : /* -Fr */
                         if (argv[cnt][2] == 'r') {                        /* -Fr */
                               sprintf(ss.framefile,"%s",argv[cnt+1]);            
                              cnt+=2;                                    /* Frame file name */
                         } 
                         break;
         case 'f' : /* -f?: -f_ or -fps or -f */
                         if (isdigit(argv[cnt][2])) {
                              ss.format = atoi(argv[cnt]+2);            /* format */
                               cnt++;
                         } else if (argv[cnt][2] == 'p') {             /* fps */
                              ss.fps = atoi(argv[cnt+1]);
                               cnt+=2;
                         } else { 
                              ss.screenmode = FULLSCREEN;             /* fullscreen */
                              cnt++;
                         }
                         break;
         case 'b' : /* -b?: -bb or -bg */
                         if (argv[cnt][2] == 'g') {                        /* background image */
                              sprintf(ss.leftimage,"%s",argv[cnt+1]);
                              sprintf(ss.rightimage,"%s",argv[cnt+2]);
                               cnt+=3;
                         } if (argv[cnt][2] == 'b') {            /* bounding box */
                              ss.box = 1;
                              cnt++;
                         }
                         break;
         case 'd' : /* -d?: -da or -dn or -d */
                         if (argv[cnt][2] == 'a') {
                              ss.symbol = atoi(argv[cnt+1]);            /* Symbol */
                              cnt+=2;
                         } else if (argv[cnt][2] == 'n') {
                              ss.subsample = atoi(argv[cnt+1]);/* Sub-sampling */
                              cnt++;
                         } else {                                                /* Debug mode */
                              ss.debug = 1;
                              cnt++;
                         }
                         break;
         case 'i' : /* -i: */
                         ss.interp = atoi(argv[cnt+1]);            /* Interpolation factor */
                         cnt+=2;
                         break;
         case 'o' : /* -os: */
                         ss.objsize = atof(argv[cnt+1]);            /* Object size */
                         cnt+=2;
                         break;
         case 'c' : /* -c */
                         ss.ramp   = atoi(argv[cnt+1]);            /* Colour ramp */
                         cnt+=2;
                         break;
         case 'C' : /* -C */
                         ss.attribute   = atoi(argv[cnt+1]);            /* Colour attribute */
                         cnt+=2;
                         break;
         case 'a' : /* -a?: -as or -ax */
                         if (argv[cnt][2] == 's') {                        /* Autospin */
                              ss.autospin = 1;
                              cnt++;
                         } else if (argv[cnt][2] == 'x') {
                              ss.axes = 1;                                    /* Show axes */
                              cnt++;
                              }            
                         break;
         case 'l' : /* -l */                                    
                         ss.looping = 1;                                    /* Circular looping */
                         cnt++;
                         break;
         case 'g' : /* -gr */
                         ss.grid = 1;                                    /* Show grid */
                         cnt++;
                         break;
         case 's' : /* -s?: -sw or -ss or -sl or -s */
                         if (argv[cnt][2] == 'w') {
                               ss.endian = 1;                                    /* Swap: endian */
                              cnt++;
                         } else if (argv[cnt][2] == 's') { 
                               ss.stereomode = PASSIVE;                        /* Passive stereo */
                              cnt++;
                               /* -ss */
                         } else if (argv[cnt][2] == 'l') {
                              ss.slider = 1;                                    /* Show slider */
                              cnt++;
                         } else if (argv[cnt][2] == 'i') {
                              ss.stereomode = INTERLEAVE;            /* Interleave stereo */
                              cnt++;
                            } else {
                               ss.stereomode = ACTIVE;                        /* Active stereo */
                              cnt++;
                               /* -s */
                         } 
                         break;
         case 'v' : /* -vf */
                         sprintf(ss.viewfile,"%s",argv[cnt+1]);
                           cnt+=2;                                                /* Viewfile */
                         break;
         case 'm' : /* -m */                                    
                         sprintf(ss.markerfile,"%s",argv[cnt+1]);
                           cnt+=2;                                                /* Static markers */
                         break;
         case 'p' : /* -p */
                         sprintf(ss.flightpath,"%s",argv[cnt+1]);
                           cnt+=2;                                                /* Flightpath file */
                         break;
         case 'h' : showHelp(stderr);                                    /* Help message */
                         ss.flag = HELPQUIT;
                         break;
                        
      case 'X': /* -Xm */
         sprintf(ss.Xmap,"%s",argv[cnt+1]);          
         cnt+=2;                                  /* colourmap name */
         break;

      case 'B': /* -B use billboards */
         ss.usebillboards = 1;
         cnt++;
         break;
                         
      case 'T': /* -T show trails */
         ss.nblur = atoi(argv[cnt+1]);
         cnt+=2;
         break;

         default   : fprintf(stderr,"Unknown argument: %s\n",argv[cnt]);
                         ss.flag = BADARG;
                         break;
        } 
        if (ss.flag < 0) return ss;
    }   

    if (ss.framefile[0] == '_') ss.flag = -1;         /* Fail if no frame file */
    if (ss.Np == 0) ss.flag = -1;                     /* Fail if num of points not given */

    return ss;
}


PlotMode initPlotMode()
/* Set some defaults - not all of these input modes are supported */
{
   PlotMode ss;
   ss.Np = 0;                                 /* No particles */
   ss.format = 0;                     /* Binary x,y,z format */
   ss.stereomode = MONO;         /* Monoscopic display */
   ss.screenmode = WINDOW;         /* Not fullscreen */
   ss.looping = 0;                     /* Do not loop */
   ss.subsample = 1;                     /* Subsampling factor */
   ss.interp = 1;                     /* Interperpolation factor */
   ss.symbol = -1;                     /* Symbol to plot points with */
   ss.objsize = 1.0;                     /* Size to plot points */
   ss.ramp = 1;                                 /* Colour ramp to use */
   ss.attribute = -1;                     /* Colour attribute to use */
   ss.autospin = 0;                     /* Should points autospin? */
   ss.box = 0;                                 /* Should a box be displayed? */
   ss.axes = 0;                                 /* Should the axes be displayed? */
   ss.grid = 0;                                 /* Should a grid be plotted? */
   ss.slider = 0;                     /* Do not show the slider */
   ss.fps = 25;                                 /* Set frames per second */
   ss.endian = 0;                     /* Use default endian for O/S */
   ss.debug = 0;                     /* No debugging messages */
   ss.flag = 0;                                 /* No special flag set */

   strcpy(ss.Xmap, "rainbow");   /* default colourmap is rainbow */
   ss.usebillboards = 0;         /* default: don't use billboards */
   ss.nblur = 0;                     /* no trails */

   sprintf(ss.basename,"_");         /* Base data name */
   sprintf(ss.leftimage,"_");         /* Background left image */
   sprintf(ss.rightimage,"_");         /* Background right image */
   sprintf(ss.viewfile,"_");         /* Camera view file */
   sprintf(ss.markerfile,"_");         /* Extra geometry markers */
   sprintf(ss.flightpath,"_");         /* Flightpath file */
   sprintf(ss.framefile,"_");    /* Important: file containing datafiles */

   return ss;
}


void plotMarkers(Marker m, int N)
/* Plot some additional markers - currently only points are supported */
{
   int i;
   for (i=0;i<N;i++) {
      ns2vthpoint(m.xyz[i], m.rgb[i], m.th[i]);
   }
}

Bounds   markerBounds(Marker m, int Nm)
/* Find the spanning range of marker geometry */ 
{
   int i;
   XYZ min = { +1e9, +1e9, +1e9 }, max = { -1e9, -1e9, -1e9 };
   for (i=0;i<Nm;i++) {
      if (m.xyz[i].x < min.x) min.x = m.xyz[i].x;
      if (m.xyz[i].y < min.y) min.y = m.xyz[i].y;
      if (m.xyz[i].z < min.z) min.z = m.xyz[i].z;
      if (m.xyz[i].x > max.x) max.x = m.xyz[i].x;
      if (m.xyz[i].y > max.y) max.y = m.xyz[i].y;
      if (m.xyz[i].z > max.z) max.z = m.xyz[i].z;
   }

/* Adjust bounds so that box is centered on midpoint of range */
/* but width in each axis direction is the same */
   float dx, dy, dz;
   float mx, my, mz;
   dx = (max.x - min.x)/2.0; mx = (max.x + min.x)/2.0;
   dy = (max.y - min.y)/2.0; my = (max.y + min.y)/2.0;
   dz = (max.z - min.z)/2.0; mz = (max.z + min.z)/2.0;
   if (dy > dx) dx = dy;
   if (dz > dx) dx = dz;
   min.x = mx - dx; max.x = mx + dx;
   min.y = my - dx; max.y = my + dx;
   min.z = mz - dx; max.z = mz + dx;

   Bounds bounds;
   bounds.min = min;
   bounds.max = max;

   return bounds;
}


Marker readMarkers(char *file, int *Nm)
{
   Marker m = { NULL, NULL, NULL };
   FILE *fp = fopen(file, "r");
   if (fp == NULL) {                                  /* Marker file does not exist */
      *Nm = 0;   
      return m;   
   }         
   *Nm = countLines(fp);                     /* How many markers to read? */
   rewind(fp);                                             /* Go back to start of file */

/* Allocate memory for marker data arrays */
   m.xyz = (XYZ *)calloc(*Nm, sizeof(XYZ));
   m.rgb = (COLOUR *)calloc(*Nm, sizeof(COLOUR));
   m.th   = (float *)calloc(*Nm, sizeof(float));

   char string[256];
   float x, y, z, r, g, b, th;                     /* Dummy variables */
   int i;
   for (i=0;i<*Nm;i++) { 
      fgets(string,255,fp);                      /* Read line from file */
      switch (string[0]) {
         case 'p' : sscanf(string,"p %f %f %f %f %f %f",&x,&y,&z,&r,&g,&b);
                         m.xyz[i].x = x; m.xyz[i].y = y; m.xyz[i].z = z;
                         m.rgb[i].r = r; m.rgb[i].g = g; m.rgb[i].b = b;
                         m.th[i] = 1;
                         break;
         case 'P' : sscanf(string,"p %f %f %f %f %f %f %f",&x,&y,&z,&r,&g,&b,&th);
                         m.xyz[i].x = x; m.xyz[i].y = y; m.xyz[i].z = z;
                         m.rgb[i].r = r; m.rgb[i].g = g; m.rgb[i].b = b;
                         m.th[i] = th;
                         break;
         default :   *Nm = 0;                        /* Other geometry type not supported */
                         break;
      }
   }
   m.bounds = markerBounds(m, *Nm);         /* Spanning range of marker geometry */

   fclose(fp);
   return m;
}


void showgrid(Bounds b)
{
   int Ng = GRIDSPACE;
   float dx = (b.max.x-b.min.x)/(float)(Ng);
   float dy = (b.max.y-b.min.y)/(float)(Ng);
   float dz = (b.max.z-b.min.z)/(float)(Ng);

   int i, j;
   float xx, yy, zz;

   for (i=0;i<=Ng;i++) {
      xx = b.min.x + i*dx;
      for (j=0;j<=Ng;j++) {
         yy = b.min.y + j*dy;
         ns2line(xx,yy,b.min.z, xx,yy,b.max.z, GRIDR, GRIDG, GRIDB);
      }
   }

   for (i=0;i<=Ng;i++) {
      zz = b.min.z + i*dz;
      for (j=0;j<=Ng;j++) {
         yy = b.min.y + j*dy;
         ns2line(b.min.x,yy,zz, b.max.x,yy,zz, GRIDR, GRIDG, GRIDB);
      }
   }

   for (i=0;i<=Ng;i++) {
      xx = b.min.x + i*dx;
      for (j=0;j<=Ng;j++) {
         zz = b.min.z + j*dz;
         ns2line(xx,b.min.z,zz, xx,b.max.z,zz, GRIDR, GRIDG, GRIDB);
      }
   
   }
}

Camera readCamera(char *viewfile)
{
   FILE *fp;
   Camera camera;
   camera.pos.x = camera.pos.y = camera.pos.z = 0.0;
   camera.view.x = camera.view.y = camera.view.z = 0.0;
   camera.up.x = camera.up.y = camera.up.z = 0.0;
   camera.focus.x = camera.focus.y = camera.focus.z = 0.0;
   camera.wc = -1;
   if ((fp = fopen(viewfile, "r")) == NULL) {         
/* Could not find camera file - just leave camera where it was */
      return camera;
   }

   char string[128];
   fgets(string, 128, fp);
   while (!feof(fp)) {
      if (strncmp("VP",string,2) == 0) {
         sscanf(string,"VP %lf %lf %lf",
                               &camera.pos.x, &camera.pos.y, &camera.pos.z);
      } else if (strncmp("VD",string,2) == 0) {
         sscanf(string,"VD %lf %lf %lf",
                               &camera.view.x, &camera.view.y, &camera.view.z);
      } else if (strncmp("VU",string,2) == 0) {
         sscanf(string,"VU %lf %lf %lf",
                               &camera.up.x, &camera.up.y, &camera.up.z);
      } else if (strncmp("PR",string,2) == 0) {
         sscanf(string,"PR %lf %lf %lf",
                               &camera.focus.x, &camera.focus.y, &camera.focus.z);
      }
      fgets(string, 128, fp);
   }
   camera.wc = 0;                     /* Viewport coordinates */

   fclose(fp);
   return camera;
}


void setNextUpdate(struct timeval * next, int fps)
{
   next->tv_usec += (1000000/fps);
   if (next->tv_usec >= 1000000)
   {
    next->tv_usec -= 1000000;
    next->tv_sec += 1;
   }
}

int timeCompare(struct timeval t1, struct timeval t2)
{
   long diff = (t1.tv_sec - t2.tv_sec) * 1000000;
   diff += (t1.tv_usec - t2.tv_usec);
   return diff;
}

/* 
 * load all the frames in the simulation
 */

void * frameLoaderThread (void * arg)
{

   struct timeval t ;

   while ((Nload < Nmax) && (loadFlag < 0)) 
   {
      loadFrame(Nload, pm.Np, pm);
      Nload++;

      /* sleep a little bit soas to not task the disk */
      t.tv_sec = 0;
      t.tv_usec = 1000;
      select (0, 0, 0, 0, &t) ;

   }
   Nload = Nmax;
   loadFlag = 1;

   return 0;
}

void CalcPoint(XYZ *pdata, XYZ* data, XYZ*ndata, float interp)
{

   if (interp == 0)
   {
    pdata->x = data->x;
    pdata->y = data->y;
    pdata->z = data->z;
   }
   else
   {
    pdata->x = data->x + interp * (ndata->x - data->x);
    pdata->y = data->y + interp * (ndata->y - data->y);
    pdata->z = data->z + interp * (ndata->z - data->z);
   }

}


int main(int argc, char *argv[])
{
   pm = initPlotMode();                                 /* Set some default values */
   pm = parse(argc, argv, pm);                     /* Read from command line */
   if (pm.flag < 0) {                                 /* Was there a problem? */
      if (pm.flag != HELPQUIT)                      /* Shown help already? Don't repeat */
         showHelp(stdout);                        /* Show help screen */
      exit(1);                                              /* Exit */
   }

   framenames = readFrameNames(pm.framefile, &Nmax);         /* Read filenames */
   data   = (XYZ **)calloc(Nmax+1, sizeof(XYZ *));         /* Allocate memory */
   rgb    = (COLOUR **)calloc(Nmax+1, sizeof(COLOUR *));
   Ndata = (int *)calloc(Nmax+1, sizeof(int));

   /* if interoplation is required, allocate memory for each frame */
   if (pm.interp > 1) 
      pdata = (XYZ *)calloc(pm.Np, sizeof(XYZ));
   else
      pdata = NULL;
   interp = 0;

   char string[32];                                 /* Try to determine type of display */
   if (pm.stereomode == PASSIVE) {
      switch (pm.screenmode) {
         case WINDOW     : sprintf(string,"/s2passv"); break;
         case FULLSCREEN : sprintf(string,"/s2passvf"); break;
      }
   } else if (pm.stereomode == ACTIVE) {
      switch (pm.screenmode) {
         case WINDOW     : sprintf(string,"/s2activ"); break;
         case FULLSCREEN : sprintf(string,"/s2activf"); break;
      }
   } else if (pm.stereomode == INTERLEAVE) {
      switch (pm.screenmode) {
         case WINDOW     : sprintf(string,"/s2inter"); break;
         case FULLSCREEN : sprintf(string,"/s2interf"); break;
      }
   } else {
      switch (pm.screenmode) {
         case WINDOW     : sprintf(string,"/s2mono"); break;
         case FULLSCREEN : sprintf(string,"/s2monof"); break;
      }
   }

   s2opend(string, argc, argv);                     /* Open the s2plot display */
   /* install colourmap */
   s2icm(pm.Xmap, XM1, XM2);

   texid = 0;
   if (pm.usebillboards) {
      // load a billboard texture
      texid = ss2lt("halo32.tga");
   }

   Bounds bounds = loadFrame(0, pm.Np, pm);         /* Bounds from 1st frame */


/* Read static geometry if it exists */
   int Nm = 0;
   Marker markers;

   markers.bounds.min.x = +1e9;
   markers.bounds.min.y = +1e9;
   markers.bounds.min.z = +1e9;
   markers.bounds.max.x = -1e9;
   markers.bounds.max.y = -1e9;
   markers.bounds.max.z = -1e9;
   markers.xyz = NULL;
   markers.rgb = NULL;
   markers.th   = NULL;

   if (strcmp(pm.markerfile,"_") != 0) {
      markers = readMarkers(pm.markerfile, &Nm);
      if (Nm > 0) {
         bounds = markers.bounds;
      }
   }
 
   camera.wc = -1;
   if (strcmp(pm.viewfile,"_") != 0) {
      camera = readCamera(pm.viewfile);
   }

   s2swin(bounds.min.x,bounds.max.x,bounds.min.y,bounds.max.y,
                   bounds.min.z,bounds.max.z);            /* Set world coordinates */


/* Create the frame slider */ 
   int c1 = 1000, c2 = 1000 + Nmax;
   int sliderID = 0;
   slider = createSlider(sliderID, Nmax-1, c1, c2, "grey", "frame");

   Nload = 1;                     /* First frame ready to load */

/* Callbacks */
   cs2scb(slider_cb);          /* Install a callback for the slider */
   cs2sncb(ncb);               /* Install a callback for number keys */
   cs2skcb(kcb);               /* Install a callback for keyboard */
   cs2sdhcb(dhcb);             /* Install dragging callback for the slider */
   cs2thv(1);                  /* Turn on handle visibility */

   if (pm.box) {		/* Draw bounding box? */
      if (pm.axes) {		/* Draw axes? */
	 s2sch(0.5);
         s2box("BCDETMNOPQ",0,0,"BCDETMNOPQ",0,0,"BCDETMNOPQ",0,0);            
	 s2lab("X-axis","Y-axis","Z-axis","");
	 s2sch(1.0);
      } else {
         s2box("BCDE",0,0,"BCDE",0,0,"BCDE",0,0);            
      }
   }
   
   if (pm.grid) showgrid(bounds);
   if (pm.autospin) ss2sas(1);		/* Start spinning? */

   if (Nm > 0) {
      plotMarkers(markers, Nm);
   }

   if (camera.wc >= 0) setCamera(camera);

   s2help("s2anim Key Actions\n"
	  "--------------------------------------------------------------\n"
	  "            space   start/stop animation\n"
	  "                0   show/hide slider\n"
	  "                1   select normal ('back to start') looping\n"
	  "                2   select 'back and forth' looping\n"
	  " shift-right-drag   change frame when slider is visible\n"
	  "--------------------------------------------------------------\n");

   /* start the frame loading thread */
   loadFlag = -1;
   pthread_t tid = 0;
   if (pthread_create (&tid, 0, frameLoaderThread, NULL) < 0) {
      perror ("Error creating new thread");
      return -1;
   } else {
      pthread_detach (tid);
   }

   gettimeofday(&currTime, NULL);
   gettimeofday(&nextTime, NULL);
   setNextUpdate(&nextTime, pm.fps);
   s2show(1);

   return 1;
}

