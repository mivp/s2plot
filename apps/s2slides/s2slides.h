/* s2slides.h
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
 * $Id: s2slides.h 5764 2012-09-25 04:16:14Z dbarnes $
 *
 */

#if !defined(S2SLIDES_H)
#define S2SLIDES_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "s2plot.h"
#include "s2glut.h"
#include <netdb.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "paulslib.h"
#include "bitmaplib.h"
#include "opengllib.h"

#define NOSTEREO     0
#define ACTIVESTEREO 1
#define DUALSTEREO   2

#define PPM 0
#define RAW 1
#define TGA 2
#define RLE 3
#define TIF 4

  typedef struct {
    char left[64];
    char right[64];
    int delay;
    int flip;
    int status; /* 0 = ready to read, 1 = bitmap loaded, 2 = texture created */
    int lw, lh, rw, rh; /* left and right image widths and heights */
    float laspect, raspect; /* left and right image aspect ratios */
    BITMAP4 *lbits, *rbits; /* left and right bitmaps */
    unsigned int ltex, rtex; /* left and right texture indices */
    char *command; /* command to execute, NULL if none */
  } IMAGELIST;
  
  /* a structure to hold an image, its bitmap, and ultimately textureid */
  typedef struct {
    int status; /* 0 = ready to read, 1 = bitmap loaded, 2 = texture created */
    int w, h; /* image width & height */
    float aspect; /* image aspect ratio (width / height) */
    size_t size; /* byte count of image bitmap size */
    BITMAP4 *bits; /* image bitmap */
    unsigned int texid; /* texture id */
  } IMAGETEX;
  
  typedef struct {
    int stereo;
    int fullscreen;
    int debug;
    int framedelay;                  // Time between frames 1/10 seconds
    int showhelp;	               
    int autoplay;                    // Run through slides automatically
    int exitsafe;                    // Require shift esc to end
    int exitatend;                   // Exit the show at the end
    int leftoffset,rightoffset;
    double xoffset,yoffset;
    double targetfps;
    int edit;
  } ANIMOPTIONS;
  
  void lGiveUsage(char *);
  int  ReadImage(char *,int);
  void ReadImagePair(void);
  void HandleMemory(int,int,int);
  void CreateDummy(int);
  
#if defined(__cplusplus)
} // extern "C" {
#endif

#endif
