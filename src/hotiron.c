/* hotiron.c
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
 * $Id: hotiron.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

/*
 *  Creates texture array with saturation levels in the y (t) direction
 *  and hue and lightness changes in x (s) direction.
 *  Black band on both hue ends, for plotting data out of range.
 *			[0,black_border-1] and [texture_size-black_border,
 *                      texture_size-1] are black.  Otherwise, use cindex =
 *			black_border + h*(texture_size-2*black_border-1).
 *			Returns a malloc'ed array of texture_size*texture_size
 *			longs with successive texture_size blocks having
 *			different saturation values; each long has r,g,b,0
 *			stored as unsigned chars.
 *
 * The authors of this software are Eric Grosse and W. M. Coughran, Jr.
 * Copyright (c) 1991 by AT&T.
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 * We thank Cleve Moler for describing the "hot iron" scale to us.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/* color maps */
#define RAINBOW	1
#define GRAY	2
#define TERRAIN	3
#define IRON	4
#define ASTRO	5
#define ZEBRA	6

#define MGREEN  11
#define S2RED   12
#define S2GREEN 13
#define S2BLUE  14
#define S2YELLOW 15
#define S2CYAN 16
#define S2MAGENTA 17

void rainbow(double h, double s, double v, 
	     double *r, double *g, double *b);

#define lllong int32_t
lllong *
texture_gen(char *mapstr, int saturation_lvls, int texture_size,
	    int black_border) {
  unsigned char	r, g, b, *obuf;
  r = g = b = 0;
  int	invert = 0, map, ncolors = texture_size, tx, x, y, ihue;
  lllong	*texture;
  double	hue, oldhue, sat, oldsat, red, green, blue, h;
  
  if(!strncmp("inverse ",mapstr,8)){
    invert = 1;
    mapstr += 8;
  }
  if(!strcmp("rainbow",mapstr))
    map = RAINBOW;
  else if(!strcmp("gray",mapstr)||!strcmp("grey",mapstr))
    map = GRAY;
  else if(!strcmp("terrain",mapstr)||!strcmp("topo",mapstr))
    map = TERRAIN;
  else if(!strcmp("iron",mapstr)||!strcmp("heated",mapstr)||
	  !strncmp("hot",mapstr,3))
    map = IRON;
  else if(!strcmp("astro",mapstr))
    map = ASTRO; /* the astronomers made me do it */
  else if(!strncmp("alt",mapstr,3)||!strcmp("zebra",mapstr))
    map = ZEBRA;
  else if(!strcmp("mgreen",mapstr))
    map = MGREEN;
  else if(!strcmp("red",mapstr))
    map = S2RED;
  else if(!strcmp("green",mapstr))
    map = S2GREEN;
  else if (!strcmp("blue",mapstr))
    map = S2BLUE;
  else if (!strcmp("yellow",mapstr))
    map = S2YELLOW;
  else if (!strcmp("cyan", mapstr))
    map = S2CYAN;
  else if (!strcmp("magenta", mapstr))
    map = S2MAGENTA;
  else {
    fprintf(stderr, "Unknown colormap - installing grey map\n");
    map = GRAY;
  }
  
  texture = (lllong *)malloc(texture_size*saturation_lvls*sizeof(lllong));
  
  oldhue = -1;
  oldsat = -1;
  for (y = 0; y < saturation_lvls; y++) {
    obuf = (unsigned char *)(texture+y*texture_size);
    sat = y / (saturation_lvls-1.);
    if(sat>0.99) sat = 0.99;
    if( oldsat!=sat ){
      for (x = 0, tx = 0; x < texture_size; x++, tx += 4) {
	hue = (x-black_border) / (texture_size-2*black_border-1.);
	ihue = floor(ncolors*hue);
	hue = ((double)ihue)/(ncolors-1);
	if(hue>1.) hue = 1.;
	if(invert) hue = 1.-hue;
	if( x<black_border || x>=texture_size-black_border ){
	  r = 0;
	  g = 0;
	  b = 0;
	}else if( oldhue!=hue ){
	  switch(map){
	  case RAINBOW:
	    rainbow(1.-hue, sat, 1., &red, &green, &blue);
	    break;
	  case GRAY:
	    red = hue;
	    green = hue;
	    blue = hue;
	    break;
	  case TERRAIN:
	    h = 3*hue;
	    if(h<.25){
	      red = 0;
	      green = 0;
	      blue = 0.25+2*h;
	    }else if(h<2){
	      red = 0;
	      green = 0.25+(2-h);
	      blue = 0;
	    }else if(h<2.7){
	      red = .75;
	      green = .15;
	      blue = .0;
	    }else{
	      red = .9;
	      green = .9;
	      blue = .9;
	    }
	    break;
	  case IRON:
	    red = 3*(hue+.03);
	    green = 3*(hue-.333333);
	    blue = 3*(hue-.666667);
	    break;
	  case ASTRO:
	    red = hue;
	    green = hue;
	    blue = (hue+.2)/1.2;
	    break;
	  case ZEBRA:
	    red = (ihue+invert) % 2;
	    green = red;
	    blue = red;
	    break;
	  case MGREEN:
	    red = 3*(hue-.333333);
	    green = 3*(hue+0.03);
	    blue = 3*(hue-.66667);
	    break;
	  case S2RED:
	    red = hue;
	    green = 0.;
	    blue = 0.;
	    break;
	  case S2GREEN:
	    red = 0.;
	    green = hue;
	    blue = 0.;
	    break;
	  case S2BLUE:
	    red = 0.;
	    green = 0.;
	    blue = hue;
	    break;
	  case S2YELLOW:
	    red = hue;
	    green = hue;
	    blue = 0.;
	    break;
	  case S2CYAN:
	    red = 0.;
	    green = hue;
	    blue = hue;
	    break;
	  case S2MAGENTA:
	    red = hue;
	    green = 0.;
	    blue = hue;
	    break;
	  }
	  if(red>1.) red = 1.;
	  if(green>1.) green = 1.;
	  if(blue>1.) blue = 1.;
	  if(red<0.) red = 0.;
	  if(green<0.) green = 0.;
	  if(blue<0.) blue = 0.;
	  r = 255*red;
	  g = 255*green;
	  b = 255*blue;
	  oldhue = hue;
	  if(sat==1.)
	    printf("# %.2g %.2g %.2g\n",
		   r/255.,g/255.,b/255.);
	}
	obuf[tx+0] = r;
	obuf[tx+1] = g;
	obuf[tx+2] = b;
	obuf[tx+3] = 0;
      }
    }
    oldsat = sat;
  }
  return texture;
}
