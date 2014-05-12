/* s2anim.h
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
 * $Id: s2anim.h 5763 2012-09-25 04:14:25Z dbarnes $
 *
 */

#ifndef __SLIDER_H__
#define __SLIDER_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include "s2plot.h"


#define FNAME_LEN 256		/* Length of a filename */

#define CMAPLEN 32

typedef struct {
  int id;                      /* Handle id */
  float dmin, dmax;            /* Data range */
  float smin, smax;            /* Screen coordinate range */
  float level;                 /* Current value */
  int Nhand;                   /* Number of handles */
  float step;                  /* Step size */
  float width;                 /* Width */
  int direct;                  /* Direction: X/Y */
  float base;                  /* Other screen coordinate */
  char *cmap;                  /* Name of colour map */
  int c1, c2;                  /* Colour index range */
  char *label;
  float last;                  /* Last value of screen coordinates */
  int lower;
  int upper;
  int nstep;
  int toggle; 
  int visible;
} Slider;

#define BADARG -1		/* Error flags */
#define HELPQUIT -2

/* S2PLOT window types */
#define MONO 0			/* Monoscopic display */
#define PASSIVE 1		/* Passive = side-by-side stereo */
#define ACTIVE 2		/* Active = sequential stereo */
#define INTERLEAVE 3		/* Interleaved stereo */
#define WINDOW 0		/* Use a window */
#define FULLSCREEN 1		/* Plot fullscreen */

#define GRIDSPACE 10		/* Number of grid spaces to draw */
#define GRIDR	0.2		/* R colour for drawing grids */
#define GRIDG	0.2		/* G colour for drawing grids */
#define GRIDB	0.2		/* B colour for drawing grids */


#define _XX 0			/* A slider on the x-axis */
#define _YY 1			/* A slider on the y-axis */

typedef struct {
  long Np;			/* Number of particles */
  int format;			/* Input file format */
  int stereomode;		/* Type of stereo display to use */
  int screenmode;		/* Window or fullscreen? */
  int looping;			/* Circular looping */
  int symbol;			/* Draw as a symbol */
  float objsize;		/* Object size */
  int autospin;		/* Start in autospin mode */
  int box;			/* Draw the bounding box */
  int axes;			/* Draw axes */
  int grid;			/* Draw grid */
  int slider;			/* Is the slider visible? */
  int endian;			/* Swap on/off for different endian */
  int debug;			/* Show debug messages */
  
  char Xmap[128];               /* Colourmap for -f95 mode */
  int usebillboards;            /* whether to use billboards */
  int nblur;                    /* how many frames to show at once (trails) */
  
  /* THESE MODES ARE NOT CURRENTLY SUPPORTED */
  int fps;			/* Frames per second to aim for */
  int subsample;		/* Subsampling rate */
  int interp;			/* Interpolation factor */
  int ramp;			/* Colour ramp */
  int attribute;		/* Colour attribute */
  
  char basename[FNAME_LEN];	/* Base filename */
  char leftimage[FNAME_LEN];	/* Background left stereo image */
  char rightimage[FNAME_LEN];	/* Background right stereo image */
  char viewfile[FNAME_LEN];	/* Camera viewfile */
  char markerfile[FNAME_LEN];	/* Static geometry marker file */
  char flightpath[FNAME_LEN];	/* Flightpath file */
  char framefile[FNAME_LEN];	/* File containing frame names */
  
  int flag;			/* Error flag */
  
} PlotMode;			/* Command line arguments */

PlotMode pm;

typedef struct {
  XYZ min, max;		/* Minimum and maximum point position */
} Bounds;

typedef struct {
  XYZ *xyz;            	/* Array of marker positions */
  COLOUR *rgb;         	/* Array of marker colours */
  float *th;           	/* Array of marker thicknesses */
  Bounds bounds;       	/* World coordinate bounds for markers */
} Marker;			/* Static geometry markers */

typedef struct {
  XYZ pos, view, up;		/* Camera position, view direction, up vector */
  XYZ focus;			/* Camera rotation point */
  int wc;			/* Use world coordinates? */
} Camera;


/* Global variables */
Slider slider;			/* The global slider */
int Nload;			/* Number of frames currently loaded */
int Nmax;			/* Maximum number of frames to load */
int loadFlag; /* state of loading */
char **framenames;		/* Array of frame filenames */
struct timeval currTime;   /* last time the slider was updated */
struct timeval nextTime;   /* time of next slider update */

int texid;                      /* texture to use for billboards */

/* The main global data arrays */
int *Ndata;			/* Number of particles in this frame */
XYZ **data;			/* The point data */
COLOUR **rgb;			/* Colour for particles */

/* for interpolation */
XYZ *pdata;			/* The interpolated point data */
int interp;                     /* current interpolation */

/* Prototypes */
Slider initSlider(int id, float dmin, float dmax, float smin, float smax,
                float base, float width, int c1, int c2,
                char *cmap, char *label, int lower, int upper,
                int direct, int nstep, int toggle, int visible);
void drawSlider(Slider sl, float scale, int Ns);
void slider_cb(double *t, int *kc);
void ncb(int *Nkey);
Bounds loadFrame(int Nframe, int Np, PlotMode lpm);

void setNextUpdate(struct timeval * next, int fps);
int timeCompare(struct timeval t1, struct timeval t2);
void * frameLoaderThread (void * arg);
int kcb(unsigned char *key);
void CalcPoint(XYZ *pdata, XYZ* data, XYZ*ndata, float interp);

#endif
