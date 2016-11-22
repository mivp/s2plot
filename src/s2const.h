/* s2const.h
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
 * $Id: s2const.h 5810 2012-10-12 03:56:20Z dbarnes $
 *
 */

#ifndef S2CONST_H
#define S2CONST_H
/* Colours - based on PGPlot default settings */
#define S2_PG_BLACK 0
#define S2_PG_WHITE 1
#define S2_PG_RED   2
#define S2_PG_GREEN 3
#define S2_PG_BLUE  4
#define S2_PG_CYAN  5
#define S2_PG_MAGENTA 6
#define S2_PG_YELLOW 7
#define S2_PG_ORANGE 8
#define S2_PG_DKGRAY 14
#define S2_PG_DKGREY 14
#define S2_PG_LTGRAY 15
#define S2_PG_LTGREY 15

/* Rendering modes */
#define WIREFRAME      0
#define SHADE_FLAT     1
#define SHADE_DIFFUSE  2
#define SHADE_SPECULAR 3

/* Projections */
#define PERSPECTIVE 0
#define ORTHOGRAPHIC 1
#define FISHEYE 2

/* Stereo types */
#define NOSTEREO     0
#define ACTIVESTEREO 1
#define DUALSTEREO   2
#define ANAGLYPHSTEREO 3
#define TRIOSTEREO   4
#define INTERSTEREO  5
#define WDUALSTEREO  6
#define NULLSTEREO   -1

// Dome types */
#define NODOME      0
#define TRUNCBOTTOM 1
#define TRUNCTOP    2
#define VSPHERICAL  3
#define HSPHERICAL  4
#define WARPMAP     5
#define MIRROR1     6

// cursor types
#define S2_CURSOR_NONE 0
#define S2_CURSOR_CROSSHAIR 1
#define S2_SCREEN_WIDTH 0
#define S2_SCREEN_HEIGHT 1
#define S2_WINDOW_Y 2
#define S2_WINDOW_WIDTH 3
#define S2_WINDOW_HEIGHT 4


// these must map to GLUT values

// these must map to GLUT for s2glutwin to work! but could fix in future
// by writing s2swinGetModifiers() to translate
#define S2_ACTIVE_SHIFT 1
#define S2_ACTIVE_CTRL 2
#define S2_ACTIVE_ALT 4

// these must map to GLUT for s2glutwin to work!  but could fix in future
// by writing s2swinSpecial() ? to translate
#define S2_KEY_F1                     1
#define S2_KEY_F2                     2
#define S2_KEY_F3                     3
#define S2_KEY_F4                     4
#define S2_KEY_F5                     5 
#define S2_KEY_F6                     6
#define S2_KEY_F7                     7
#define S2_KEY_F8                     8
#define S2_KEY_F9                     9
#define S2_KEY_F10                    10
#define S2_KEY_F11                    11
#define S2_KEY_F12                    12
#define S2_KEY_LEFT                   100
#define S2_KEY_UP                     101
#define S2_KEY_RIGHT                  102
#define S2_KEY_DOWN                   103
#define S2_KEY_PAGE_UP                104
#define S2_KEY_PAGE_DOWN              105
#define S2_KEY_HOME                   106
#define S2_KEY_END                    107
#define S2_KEY_INSERT                 108

// and ditto for mouse buttons and states...
#define S2_LEFT_BUTTON                0
#define S2_MIDDLE_BUTTON              1
#define S2_RIGHT_BUTTON               2
#define S2_WHEEL_UP                   3
#define S2_WHEEL_DOWN                 4
#define S2_DOWN                       0
#define S2_UP                         1

// and ditto for visibility
#define S2_NOT_VISIBLE 0
#define S2_VISIBLE 1

// Warp map types
#define POLAR 1
#define RECTANGULAR 2

#define MAXVRMLLEN 32
#define MAXLABELLEN 256

#define SHUTTERGLASSES 0
#define ANAGLYPH_RB    1
#define ANAGLYPH_RG    2

#define INSPECT 0
#define FLY     1
#define WALK    2
#define OBJECT  3

#define NOCURSOR    0
#define ARROWCURSOR 1
#define CROSSCURSOR 2

#define HOME        1
#define FOCUS       2
#define FRONT       3
#define BACK        4
#define LEFT        5
#define RIGHT       6
#define TOP         7
#define BOTTOM      8
#define AXONOMETRIC 9

#define IMAGESPLIT 8

#define KEYBOARDCONTROL 0
#define MOUSECONTROL    1

#define MAXLIGHT 8


/* world coordinate system */
/* PLEASE NOTE: these are not yet used consistently throughout S2PLOT */
/* THEREFORE DO NOT CHANGE THEM AND EXPECT SENSIBLE RESULTS !!! */
#define _S2XAX 0
#define _S2YAX 1
#define _S2ZAX 2

#define _S2WORLD2DEVICE(wv, axis) (strlen(_s2_whichscreen) ? (wv) : (_s2devicemin[(axis)] + (_s2devicemax[(axis)]-_s2devicemin[(axis)]) / (_s2axismax[(axis)]-_s2axismin[(axis)]) * ((wv) - _s2axismin[(axis)])))

#define _S2WORLD2DEVICE_SO(wv, axis) (strlen(_s2_whichscreen) ? (wv) : ((_s2devicemax[(axis)]-_s2devicemin[(axis)]) / (_s2axismax[(axis)]-_s2axismin[(axis)]) * ((wv))))

#define _S2DEVICE2WORLD(dv, axis) (strlen(_s2_whichscreen) ? (dv) : (_s2axismin[(axis)] + (_s2axismax[(axis)]-_s2axismin[(axis)]) / (_s2devicemax[(axis)]-_s2devicemin[(axis)]) * ((dv) - _s2devicemin[(axis)])))

#define _S2DEVICE2WORLD_SO(dv, axis) (strlen(_s2_whichscreen) ? (dv) : ((_s2axismax[(axis)]-_s2axismin[(axis)]) / (_s2devicemax[(axis)]-_s2devicemin[(axis)]) * ((dv))))

#define _S2W3RADIUS(val) (strlen(_s2_whichscreen) ? (val) : (sqrt(0.333333 * (powf(_S2WORLD2DEVICE_SO(val, _S2XAX), 2.0) + powf(_S2WORLD2DEVICE_SO(val, _S2YAX), 2.0) + powf(_S2WORLD2DEVICE_SO(val, _S2ZAX), 2.0)))))

/* handy utility macros */

#define invbcopy(a,b,c) bcopy((b),(a),(c))

/* is a,b,c monotonic ? */
#define _S2MONOTONIC(a,b,c) ((((a)<(b))&&((b)<(c))) || (((a)>(b))&&((b)>(c))) ? 1 : 0)

#define S2_WITHIN(a, b, c) ((fabs((a)-(b)) < (c)) ? 1 : 0)

#define S2_COLOURWITHIN(a, y, c) ( (((a).r-(y).r)*((a).r-(y).r) + ((a).g-(y).g)*((a).g-(y).g) + ((a).b-(y).b)*((a).b-(y).b)) < c*c )

#define S2_XYZWITHIN(a, b, c) ( (((a).x-(b).x)*((a).x-(b).x) + ((a).y-(b).y)*((a).y-(b).y) + ((a).z-(b).z)*((a).z-(b).z)) < c*c )

#define S2INWORLD(p) (_S2MONOTONIC(_s2axismin[_S2XAX], (p).x, _s2axismax[_S2XAX]) && _S2MONOTONIC(_s2axismin[_S2YAX], (p).y, _s2axismax[_S2YAX]) && _S2MONOTONIC(_s2axismin[_S2ZAX], (p).z, _s2axismax[_S2ZAX]))


/* current pen color */
#define _S2PENRED (_s2_colormap[_s2_colidx].r)
#define _S2PENGRN (_s2_colormap[_s2_colidx].g)
#define _S2PENBLU (_s2_colormap[_s2_colidx].b)

/* separators for s2iden function */
#define _S2IDENPREFIX ""
#define _S2IDENMIDFIX "; "
#define _S2NULLIDSTR "<unknown>"

#define _S2MAXCOLORS 16384


/* character spacing in x direction at screen */
#define _S2XSP 18

/* fractional length of ticks for axis labels */
#define _S2TIKFRAC 0.04

#define _S2DEVCAP_CURSOR    0x01
#define _S2DEVCAP_CROSSHAIR 0x02
#define _S2DEVCAP_SELECTION 0x04
#define _S2DEVCAP_NOCOLOR   0x08

/* how many places to use in frame strings */
#define _S2FRAMESTRLEN 4
#define _S2GEOMEXT ".s2g"
#define _S2GEOMEXTLEN 4
#define _S2VIEWEXT ".s2v"
#define _S2VIEWEXTLEN 4
#define _S2DUMPEXT ".tga"
#define _S2DUMPEXTLEN 4

#define _S2ISOSURFACE _S2TRIANGLE_CACHE

#define ASCII_KEY_EVENT 1

#define S2PROMPT_LEN 80
#define S2BASEPROMPT_LEN 30

// this is needed to define stepping in PORT numbers for slaves
// exceeding this does not necessarily mean failure, but it may
// depending on the exact port and tree configuration you are using
// in your driver code
//#define S2MPIMAXNODES 32

#endif

