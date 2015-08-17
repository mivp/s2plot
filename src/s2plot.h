/* s2plot.h
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
 * $Id: s2plot.h 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#ifndef S2PLOT_H
#define S2PLOT_H

#include "s2types.h"
#if defined(S2OPENMP)
#include <omp.h>
#endif

#if defined(__cplusplus) && !defined(S2_CPPBUILD)
extern "C" {
#endif

/***********************************************************************
 *
 * OPENING, CLOSING AND SELECTING DEVICES
 *
 ***********************************************************************
 */

/* Open the s2plot device.  If fullscreen = 0, use windowed mode, else
 * make best effort at going fullscreen.  If stereo = 0, use mono view,
 * else use stereo view.  For stereo = 1, attempt active stereo mode,
 * or for stereo = 2, attempt passive stereo mode.  The commandline
 * arguments are needed for the creation of GLUT contexts.
 *
 * STATUS: functional, needs review
 */
int s2open(int fullscreen, int stereo, int argc, char **argv);

/* Open the s2plot device (device string version).  This is like the
 * PGPlot open function, which allows for setting the device via the
 * environment, via standard input when the program is run, or via
 * an explicit setting in the code.  Options are:
 *
 * /S2MONO   - mono device, windowed
 * /S2MONOF  - mono device, full screen if possible
 * /S2PASSV  - passive stereo device, windowed (not for projection)
 * /S2PASSVF - passive stereo device, full screen if possible
 * /S2ACTIV  - active stereo device, windowed
 * /S2ACTIVF - active stereo device, full screen if possible
 * /S2FISH   - fisheye projection, windowed
 * /S2FISHF  - fisheye projection, full screen if possible (not working?)
 *
 * If the device string is given as blank or empty, then the value of
 * the environment variable "S2PLOT_ENV" will be used, and should be
 * set to one of the above.  If this value is not found, then the
 * behaviour is as for "/?" described below.
 *
 * If the device string is given as "/?" then the user will be prompted
 * for their choice when the program is run.  The default choice will
 * be the value of S2PLOT_ENV if it contains a valid device.
 */
int s2opend(char *device, int argc, char **argv);

/* Open the s2plot device (device string version, ignoring command
 * line arguments).
 */
int s2opendo(char *device);

/* List the available s2plot devices on stdout */
void s2ldev(void);

/* Draw the scene.  Argument interactive should be non-zero.  If zero,
 * a warning will be issued.  Presence of this argument is historical.
 * This function never returns.  If you need to regain control after
 * displaying graphics, consider using s2disp.
 *
 * STATUS: functional, review recommended
 */
void s2show(int interactive);

/* Draw the scene, but return control when a timeout occurs or when
 * the user hits the 'TAB' key.  In some distributions, this
 * capability is not available - a warning will be issued and s2show
 * will be called implicitly.  If idelay is 0, the function returns
 * immediately the event buffer is clear; idelay > 0 means return
 * after this many seconds or when key is pressed; and idelay < 0
 * means no timeout, return when key pressed.  If argument
 * irestorecamera is > 0, then the camera will be returned to
 * its "home" position.  Otherwise, it is left in its current
 * position.
 */
void s2disp(int idelay, int irestorecamera);

/* Erase the geometry.  If called in the main program flow, this will
 * erase all geometry from the lists.  It is generally only used if
 * you are using the s2disp(...) function to regain control after
 * showing some geometry.  If called from a callback function
 * registered with s2scb(...), this will erase the dynamic geometry
 * list, however this is generally unnecessary as the dynamic geometry
 * is implicitly erased prior to the callback function being called.
 */
void s2eras();

/***********************************************************************
 *
 * WINDOWS AND VIEWPORTS
 *
 ***********************************************************************
 */

/* Set/query the part of 3d space that is used for plotting.  This is
 * the "viewport" where graphics are drawn.  The default values
 * are [-1,+1] for all coordinates.  A "fatal" error will occur if
 * x1 > x2 or y1 > y2 or z1 > z2.
 */
void s2svp(float x1, float x2, float y1, float y2, float z1, float z2);
void s2qvp(float *dx1, float *dx2, float *dy1, float *dy2,
	   float *dz1, float *dz2);

/* Set/query the world coordinate range for the device.  This configures
 * the mapping of the user's coordinates to the device coordinates.
 * x1, x2 define the range of the X-axis; y1, y2 the Y-axis, and
 * z1, z2 the Z-axis.  We use a standard right-handed coordinate
 * system with X negative to left, positive to right; Y negative
 * out of the screen, positive into the screen; and Z negative down,
 * positive up.  A "fatal" error will occur if x1 > x2, or y1 > y2,
 * or z1 > z2.  If you want to see the data "the other way around",
 * just rotate it once you're viewing it!
 *
 * STATUS: functional, needs review
 */
void s2swin(float x1, float x2, float y1, float y2, float z1, float z2);
void s2qwin(float *x1, float *x2, float *y1, float *y2,
	    float *z1, float *z2);

/* Convenience function which sets the plotting environment.  That is,
 * it sets the viewport to the default, sets the world coordinate
 * range as requested, and optionally labels the plot with a call to
 * s2box.  If "just" == 1, the scales of the x, y and z axes (in
 * something like world coordinates per inch) will be equal and the
 * plotting viewport may be a generic rectangular prism rather than a
 * cube (the case for just = 0).  Argument "axis" controls labelling
 * as follows:
 *
 * axis = -2: no box, axes or labels
 * axis = -1: draw box only
 * axis =  0: draw box and label it with coordinates
 * axis =  1: same as axis=0, but also draw the coordinate axes [NYI]
 * axis =  2: same as axis=1, but also draw grid lines at major intervals
 *
 * It is a requirement that xmin < xmax, ymin < ymax and zmin < zmax.
 * Failure to comply will result in a fatality.
 *
 * A call to s2sci can preceed the call to s2env so that the axis
 * labels are drawn in the desired colour.
 * 
 * Further options include requesting logarithmic labelling, not yet
 * implemented.
 */
void s2env(float xmin, float xmax, float ymin, float ymax,
	   float zmin, float zmax, int just, int axis);

/***********************************************************************
 *
 * PRIMITIVES
 *
 ***********************************************************************
 */

/* Draw a line, n vertices at (xpts, ypts, zpts). The line is
 * drawn in the current color and with the current line width.
 */
void s2line(int n, float *xpts, float *ypts, float *zpts);
void s2linea(int n, float *xpts, float *ypts, float *zpts, float alpha);

/* Draw a circle in the xy (yz, xz) plane at a given z coordinate.
 * Use nseg to control how many line segments are used to draw the
 * circle.  Low values of nseg (>= 3) can be used to draw n-sided
 * polygons!  Argument "asp" controls the aspect ratio and whether a
 * circle or ellipse is drawn.  asp<0 will produce a circle by
 * calculating the radius as the functions s2disk?? do.  asp=1.0 will
 * produce a circle when the x and y axes have the same device scale
 * and an ellipse otherwise; values < 1.0 will squash the ellipse
 * further in y (z, z); values > 1.0 stretch in the y (z, z)
 * direction.
 */
void s2circxy(float px, float py, float pz, float r, int nseg, float asp);
void s2circxz(float px, float py, float pz, float r, int nseg, float asp);
void s2circyz(float px, float py, float pz, float r, int nseg, float asp);

/* Draw a disk in the xy (xz, yz) plane, at a given z (y, x)
 * coordinate.  The disk has an inner and outer radius and the annulus
 * is filled.  Thus you can draw solid circles (r1 = 0, r2 > 0),
 * circle outlines (r1 > 0, r2 = r1+eps) and all things in betwixt.
 * The radius is given in world coordinates, and the actual radius
 * used is the quadratic mean of the radius converted to x (x, y) and
 * y (z, z) normalised device units.  Note that the disk is infinitely
 * thin and can vanish when viewed edge on.  If this is problem,
 * consider using s2circ to provide an outline for the disk/s.
 */
void s2diskxy(float px, float py, float pz, float r1, float r2);
void s2diskxz(float px, float py, float pz, float r1, float r2);
void s2diskyz(float px, float py, float pz, float r1, float r2);


/* Draw a rectangle in the xy (xz, yz) plane, at a given z (y, x)
 * coordinate.  The rectangle is a filled quadrangle.  The edge of the
 * rectangle is NOT drawn.  Use s2line if you need a border to your
 * rectangle/s.
 */
void s2rectxy(float xmin, float xmax, float ymin, float ymax, float z);
void s2rectxz(float xmin, float xmax, float zmin, float zmax, float y);
void s2rectyz(float ymin, float ymax, float zmin, float zmax, float x);

/* Draw a wireframe cube with edges parallel to the main coordinate
 * axes.  Thickness and colour controlled by standard s2 functions.
 */
void s2wcube(float xmin, float xmax, float ymin, float ymax, 
	     float zmin, float zmax);

/* Draw a single point.  Symbol value as per s2pt description. */
void s2pt1(float x, float y, float z, int symbol);

/* Draw a set of points.  npts defines the length of the arrays.
 * Symbol value 1 produces a single pixel, independent of distance to
 * the point.  Symbols are drawn in the current color.  Markers taking
 * finite size are scaled by the current character height.  The
 * current linewidth does not affect markers.
 *
 * proposed symbols:
 * 0 = wireframe box
 * 1 = point
 * 2 = wireframe 3d cross
 * 4 = shaded sphere
 * 6 = shaded box
 */
void s2pt(int np, float *xpts, float *ypts, float *zpts, int symbol);

/* Draw a set of points with different markers.  The markers are selected
 * from the array "symbols" of length "ns".  If ns == n, then the i'th
 * point is drawn with the i'th symbol.  If ns < n, then for the first
 * ns points, the i'th symbol is used for the i'th point, thereafter the
 * first symbol is used for all remaining points.
 */
void s2pnts(int np, float *xpts, float *ypts, float *zpts, 
	    int *symbols, int ns);

/* Draw text in the xy (xz, yz) plane, at a fixed z (y, x) coordinate.
 * Text is drawn with its "bottom left" corner at the given
 * coordinate.  For text extending in the positive direction along an
 * axis, use the "s2text??f" versions and give the "flip?" (? = x, y,
 * z) argument as +1, give -1 for flipped text, or any other value for
 * scaling as you like it.
 */
void s2textxy(float x, float y, float z, char *text);
void s2textxz(float x, float y, float z, char *text);
void s2textyz(float x, float y, float z, char *text);
void s2textxyf(float x, float y, float z, float flipx,float flipy, char *text);
void s2textxzf(float x, float y, float z, float flipx,float flipz, char *text);
void s2textyzf(float x, float y, float z, float flipy,float flipz, char *text);

/* Find the bounding box of text drawn with the "s2text??" family of
 * functions.  For convenience, pad can be given > 0.0 to obtain
 * padding around the text of pad percent.
 */
void s2qtxtxy(float *x1, float *x2, float *y1, float *y2,
	      float x, float y, float z, char *text,
	      float pad);
void s2qtxtxz(float *x1, float *x2, float *z1, float *z2,
	      float x, float y, float z, char *text,
	      float pad);
void s2qtxtyz(float *y1, float *y2, float *z1, float *z2,
	      float x, float y, float z, char *text,
	      float pad);
void s2qtxtxyf(float *x1, float *x2, float *y1, float *y2,
	       float x, float y, float z, float flipx, float flipy,
	       char *text, float pad);
void s2qtxtxzf(float *x1, float *x2, float *z1, float *z2,
	       float x, float y, float z, float flipx, float flipz,
	       char *text, float pad);
void s2qtxtyzf(float *y1, float *y2, float *z1, float *z2,
	       float x, float y, float z, float flipy, float flipz,
	       char *text, float pad);

/* Draw an arrow from the point with world-coordinates (x1,y1,z1) to
 * (x2,y2,z2).  The size of the arrowhead at (x2,y2,z2) is controlled
 * by the current character size.
 */
void s2arro(float x1, float y1, float z1,
	    float x2, float y2, float z2);

/***********************************************************************
 *
 * DRAWING ATTRIBUTES
 *
 ***********************************************************************
 */

/* Set/query the pen color by index.  If it lies outside the defined range,
 * it will be set to the default color (white).
 */
void s2sci(int idx);
int s2qci(void);

/* Set/query color representation, ie. define a color associated with an
 * index.  Once defined this colour can be selected and used.  The
 * default colors (indices 0..15) can be redefined.
 */
void s2scr(int idx, float r, float g, float b);
void s2qcr(int idx, float *r, float *g, float *b);

/* Set/query the linewidth in pixels.  The default is 1.  Changing the line
 * width affects lines and vector symbols.  Unlike PGPLOT, text is not
 * affected.  Non-integer values are rounded.
 */
void s2slw(float width);
int s2qlw(void);

/* Set/query the line style.  Codes are identical to PGPLOT, viz.
 * 1 (full line), 2 (dashed), 3 (dot-dash-dot-dash), 4 (dotted),
 * 5 (dash-dot-dot-dot). The default is 1 (normal full line).
 */
void s2sls(int ls);
int s2qls(void);

/* Set/query the character height in "arbitrary" units.  The default character
 * size is 1.0, corresponding to a character about 15 pixels in size.
 * Changing the character height also changes the scale of tick marks
 * and symbols.
 */
void s2sch(float size);
float s2qch(void);

/* Enable/disable/toggle (1,0,-1) crosshair visibility */
void ss2txh(int enabledisable);
/* Query crosshair visibility */
int ss2qxh(void);

/* Enable/disable screen (2d) coordinates.  String "whichscreens" sets
 * the screen coordinates to draw in; can be any of 'c', 'l', or 'r',
 * or a combination of same.  Empty string returns drawing to 3D. */
void ss2tsc(char *whichscreens);

/* Set/query the style to be used for arrowheads drawn with s2arro.  Arrow
 * heads are only cones at this point.  The rendering mode (wireframe
 * or shaded) will affect the look.  Line-based arrowheads may be
 * added at a later stage.  Argument "fs" is the fill style (1 =
 * filled, 2 = outline [not yet implemented]); "angle" is the angle of
 * the arrow point in degrees, eg. 45.0 (the default) gives a cone of
 * semi-vertex angle 22.5 degrees; and "barb" is the fraction of the
 * cone that is cut away from the back, ie. 0.0 gives an open cone,
 * 1.0 gives a cone with a solid base (ie. you cannot see "into" the
 * cone from behind, and the default value of 0.3 gives a nice effect.
 * Values of "angle" are clipped to the range [5.0, 135.0].  Values of
 * "barb" are clipped to the range [0.05, 1.0].
 */
void s2sah(int fs, float angle, float barb);
void s2qah(int *fs, float *angle, float *barb);

/* Set/query whether clipping is being applied to geometry.  When
 * enabled, clipping means points not within the world coordinate
 * bounds as defined by the most recent call to s2swin, will not
 * be drawn.  The initial implementation applies to points only, 
 * future work may apply the clipping to lines and facets as well.
 * Enable/disable/toggle (1,0,-1) clipping. */
void s2twc(int enabledisable);
/* Query clipping state */
int s2qwc(void);

/***********************************************************************
 *
 * AXES, BOXES & LABELS
 *
 ***********************************************************************
 */

/* Draw a labelled box around the world space.  xtick, ytick and ztick
 * specify the major tick intervals on each axis (0.0 means let s2plot
 * figure out the "best" value/s); nxsub, nysub and nzsub indicate
 * how many subdivisions the major intervals should comprise and are
 * ignored when s2plot is determining major intervals itself.  If
 * nxsub, nysub or nzsub are 0, then they are automatically determined.
 * 
 * xopt is a character string, with any of the following:
 * B = draw front bottom (-Y,-Z) edge
 * C = draw front top (-Y,+Z) edge
 * D = draw back bottom (+Y,-Z) edge
 * E = draw back top (+Y,+Z) edge
 * T = draw major tick marks
 * S = draw minor tick marks [not yet implemented]
 * M = numeric labels in conventional location/s
 * N = numeric labels in non-conventional location/s
 * O = draw labels on an opaque panel so they are never seen in reverse
 *     - this is always done if both 'M' and 'N' options are requested
 * Q = make panel larger to fit axis titles drawn by s2lab function
 * G = draw grid lines at major tick intervals
 * L = written label is 10^(coordval), ie. logarithmic labels
 * 1 = force decimal labels (this is default: redundant option)
 * 2 = force exponential (scientific notation) labels

 * yopt is a character string as for xopt, but with following changes:
 * B = draw bottom left (-Z,-X) edge
 * C = draw bottom right (-Z,+X) edge
 * D = draw top left (+Z,-X) edge
 * E = draw top right (+Z,+X) edge
 *
 * zopt is a character string as for xopt, but with following changes:
 * B = draw left front (-X,-Y) edge
 * C = draw right front (-X,+Y) edge
 * D = draw left back (+X,-Y) edge
 * E = draw right back (+X,+Y) edge
 *
 * STATUS: largely complete!
*/
void s2box(char *xopt, float xtick, int nxsub,
	   char *yopt, float ytick, int nysub,
	   char *zopt, float ztick, int nzsub);

/* Draw labels along the edges of the viewport.
 * Note that this routine expects s2box to
 * have been called with options to produce panels for drawing axis
 * text on.  If not, then the function will not fail, but the
 * resultant labels will be overlaid on back-to-front versions of
 * same.
 *
 * STATUS: review control and placement of labels.
 *
 */
void s2lab(char *xlab, char *ylab, char *zlab, char *title);

/* Set the custom help string.  This is shown on the 2nd press of the 
 * F1 key for S2PLOT programs, if it is set.
 */
void s2help(char *helpstr);


/***********************************************************************
 *
 * XY(Z) PLOTS
 *
 ***********************************************************************
 */

/* Draw error bars at the coordinates (xpts[i], ypts[i], zpts[i]),
 * i=0..(n-1).  Error bars are drawn in the direction indicated by
 * argument "dir" as described below.  One-sided error bar lengths are
 * given by "edelt", such that for error bars in eg. the x direction,
 * error bars are drawn to xpts[i]+edelt[i], to xpts[i]-edelt[i], or
 * to both.  Argument "termsize" gives the size of the terminals to
 * draw for each error bar; it is given in an integer increment of the
 * current linewidth.  Eg. if t=1, then end points are one pixel larger
 * than the line width used to draw the bars.
 *
 * dir: 1 for error bar in +x direction
 *      2      ...         +y   ...
 *      3      ...         +z   ...
 *      4      ...         -x   ...
 *      5      ...         -y   ...
 *      6      ...         -z   ...
 *      7      ...         +/-x ...
 *      8      ...         +/-y ...
 *      9      ...         +/-z ...
 *
 */
void s2errb(int dir, int n, float *xpts, float *ypts,
	    float *zpts, float *edelt, int termsize);

/* Draw a curve defined by parametric equations fx(t), fy(t) and fz(t).
 * N points are constructed, uniformly spaced from tmin to tmax.
 *
 * STATUS: functional, but some clipping might be desirable
 */
void s2funt(float(*fx)(float*), float(*fy)(float*), float(*fz)(float*),
	    int n, float tmin, float tmax);

/* Like s2funt, but an additional function (whose return value is clipped
 * to the range [0,1]) fc controls the color of the line, according to
 * the color index range currently set.  This function may be called
 * with fc NULL, in which case the behaviour is identical to s2funt.
 */
void s2funtc(float(*fx)(float*), float(*fy)(float*), float(*fz)(float*),
	     float(*fc)(float*), int n, float tmin, float tmax);


/* Draw the surface described by the provided function fxy (fxz, fyz).
 * The function is evaluated on a nx * ny (nx * nz, ny * nz) grid
 * whose world coordinates extend from (xmin,ymin) to (xmax,ymax).
 * The "ctl" argument has the following effect:
 *
 * ctl = 0: curve plotted in current window and viewport.  Caller is
 * responsible for having set the viewport and world coordinate system
 * suitably.
 *
 * ctl = 1: s2env is called automatically to fit the plot in the
 * current viewport.
 *
 * Beware that these functions consume memory to store all the function
 * evaluations prior to triangulating the surface.
 */
void s2funxy(float(*fxy)(float*,float*), int nx, int ny,
	     float xmin, float xmax, float ymin, float ymax,
	     int ctl);
void s2funxz(float(*fxz)(float*,float*), int nx, int nz,
	     float xmin, float xmax, float zmin, float zmax,
	     int ctl);
void s2funyz(float(*fyz)(float*,float*), int ny, int nz,
	     float ymin, float ymax, float zmin, float zmax,
	     int ctl);

/* Draw surface as per s2fun??, but with explicit settings for the
 * color range mapping.
 */
void s2funxyr(float(*fxy)(float*,float*), int nx, int ny,
	      float xmin, float xmax, float ymin, float ymax,
	      int ctl, float rmin, float rmax);
void s2funxzr(float(*fxz)(float*,float*), int nx, int nz,
	      float xmin, float xmax, float zmin, float zmax,
	      int ctl, float rmin, float rmax);
void s2funyzr(float(*fyz)(float*,float*), int ny, int nz,
	     float ymin, float ymax, float zmin, float zmax,
	      int ctl, float rmin, float rmax);

/* Plot the parametric function (generally a surface) defined by
 * { (fx(u,v), fy(u,v), fz(u,v) }, coloured by fcol(u,v) with
 * fcol required to fall in the range [0,1].  fcol is then mapped
 * to the current colormap index range (set with s2scir).  Range
 * and delta of u and v is required.
 */
void s2funuv(float(*fx)(float*,float*), float(*fy)(float*,float*),
	     float(*fz)(float*,float*), float(*fcol)(float*,float*),
	     float umin, float umax, int uDIV,
	     float vmin, float vmax, int vDIV);

/* plot the parametric function (generally a surface) defined by 
 *  { (fx(u,v), fy(u,v), fz(u,v) }, coloured by fcol(u, v) with 
 * fcol required to fall in the range [0,1], and mapping to the 
 * current colormap, and apply transparency to the surface with
 * falpha(u,v) defining the opacity in range [0,1].  For a constant
 * opacity, implement falpha(u,v) {return const_value;}.
 */
void s2funuva(float(*fx)(float*,float*), float(*fy)(float*,float*), 
	      float(*fz)(float*,float*), float(*fcol)(float*,float*), 
	      char trans, float(*falpha)(float*,float*),
	      float umin, float umax, int uDIV,
	      float vmin, float vmax, int vDIV);

/***********************************************************************
 *
 * IMAGES / SURFACES
 *
 ***********************************************************************
 */

/* draw a colour surface representation of the 2-dimensional array, data,
 * containing nx x ny values.  A sub-section only of the array is drawn,
 * viz. data[i1:i2][j1:j2].  Data values <= datamin are mapped to the
 * first color in the colormap (see s2scir), while values >= datamax
 * are mapped to the last entry in the colormap.  The mapping is linear
 * at this stage.  The final argument, tr, defines the transformation
 * of the data cell locations to world coordinates in the X-Y space,
 * and the transformation of data values to the Z ordinate, as follows:
 *
 * x = tr[0] + tr[1] * i + tr[2] * j
 * y = tr[3] + tr[4] * i + tr[5] * j
 * z = tr[6] + tr[7] * dataval
 *
 * todo: do we need to provide option to draw wireframe as well as
 * shaded surface?
 *
 * STATUS: functional, but could have enhancements
 */
void s2surp(float **data, int nx, int ny,
	    int i1, int i2, int j1, int j2,
	    float datamin, float datamax,
	    float *tr);

/* draw a colour surface representation of the 2-dimensional array, data,
 * containing nx x ny values.  A sub-section only of the array is drawn,
 * viz. data[i1:i2][j1:j2].  Data values <= datamin are mapped to the
 * first color in the colormap (see s2scmap), while values >= datamax
 * are mapped to the last entry in the colormap.  The mapping is linear
 * at this stage.  This function differs to the simpler "s2surp" in that
 * the tranformation array provides an arbitrary transform, allowing the
 * surface plot to be placed anywhere in the space oriented at any angle, etc.  * The transformation is as follows:
 *
 * x = tr[0] + tr[1] * i + tr[2] * j + tr[3] * dataval
 * y = tr[4] + tr[5] * i + tr[6] * j + tr[7] * dataval
 * z = tr[8] + tr[9] * i + tr[10]* j + tr[11]* dataval
 *
 * todo: do we need to provide option to draw wireframe as well as
 * shaded surface?
 *
 * STATUS: functional, but could have enhancements
 */
void s2surpa(float **data, int nx, int ny,
	     int i1, int i2, int j1, int j2,
	     float datamin, float datamax,
	     float *tr);

/* Set the range of color indices used for shading. */
void s2scir(int col1, int col2);

/* Query the color index range. */
void s2qcir(int *col1, int *col2);

/* Install various colormaps.  Give map name as a string, and index
 * range where you want the map installed.  Available maps are:
 *
 * rainbow
 * gray, grey
 * terrain, topo
 * iron, heated, hot
 * astro
 * alt, zebra
 * mgreen
 *
 * and may be preceeded by the exact string "inverse " to reverse the
 * map direction.
 *
 * STATUS: complete, needs tidy
 */
int s2icm(char *mapname, int idx1, int idx2);

/***********************************************************************
 *
 * ISOSURFACES
 *
 ***********************************************************************
 */

/* create an isosurface rendering "object".  "grid"
 * is a 3d volume indexed by grid[0..(adim-1)][0..(bdim-1)][0..(cdim-1)].
 * The slice of data actually plotted is indexed by [a1..a2][b1..b2][c1..c2]
 * (as for s2vect3, for example).
 *
 * "tr" is the transformation matrix which maps indices into the grid
 * onto the x, y and z axes of the 3d world space. The standard (non-rotated,
 * non-skewed) transformation would have tr[2], tr[3], tr[5], tr[7], tr[9]
 * and tr[10] all zero.
 *
 * x = tr[0] + tr[1] * ia + tr[2] * ib + tr[3] * ic
 * y = tr[4] + tr[5] * ia + tr[6] * ib + tr[7] * ic
 * z = tr[8] + tr[9] * ia + tr[10]* ib + tr[11]* ic
 *
 * Provide tr = NULL to use the default, unit transformation.
 * 
 * "level" gives the level at which the isosurface should be drawn.
 * Skip every nth cell as given by resolution: if n = 1; no skipping occurs.
 *
 * Transparency of the surface is controlled by "trans" and "alpha".
 * "trans" can be 'o' for opaque, 't' for transparent "piling up", and
 * 's' for transparency that can occlude.  alpha is in [0,1] where 1 is
 * opaque.
 *
 * The colour of the isosurface is provided by red, green and blue in
 * the range [0,1].
 *
 * The isosurface should be drawn with a call to ns2dis.
 */
int ns2cis(float ***grid,
	   int adim, int bdim, int cdim,
	   int a1, int a2, int b1, int b2, int c1, int c2,
	   float *tr, float level, int resolution,
	   char trans, float alpha, float red, float green, float blue);

/* create an isosurface "object", but a callback function is provided
 * to return colours at given (x,y,z) where these are fractional indices
 * into the data volume (grid).  fcol is of form fcol(fx,fy,fz,r,g,b);
 *
 * The isosurface should be drawn with a call to ns2dis.
 */
int ns2cisc(float ***grid,
	    int adim, int bdim, int cdim,
	    int a1, int a2, int b1, int b2, int c1, int c2,
	    float *tr, float level, int resolution,
	    char trans, float alpha,
	    void(*fcol)(float*,float*,float*,float*,float*,float*));

/* draw an isosurface.  Set force to true to force the surface to be
 * recalculated, eg. you've changed the grid */
void ns2dis(int isid, int force);

/* change an isosurface level */
void ns2sisl(int isid, float level);

/* set isosurface alpha and transparency */
void ns2sisa(int isid, float alpha, char trans);

/* set isosurface colour */
void ns2sisc(int isid, float r, float g, float b);

/* Set/query general surface quality high (1) or low (0, default).
 * The former calculates weighted normals for all vertices, while the
 * latter calculates a single normal per facet of the surface.  The
 * former should be used for export to VRML. */
void ns2ssq(int hiq);
int ns2qsq(void);

/***********************************************************************
 *
 * VOLUME RENDERING
 *
 ***********************************************************************
 */

/* create a volume rendering "object".  "grid" is a 3d volume indexed by
 * grid[0..(adim-1)][0..(bdim-1)][0..(cdim-1)].
 * The slice of data actually plotted is indexed by [a1..a2][b1..b2][c1..c2].
 *
 * "tr" is the transformation matrix which maps indices into the grid
 * onto the x, y and z axes of the 3d world space. The standard (non-rotated,
 * non-skewed) transformation would have tr[2], tr[3], tr[5], tr[7], tr[9]
 * and tr[10] all zero.
 *
 * x = tr[0] + tr[1] * ia + tr[2] * ib + tr[3] * ic
 * y = tr[4] + tr[5] * ia + tr[6] * ib + tr[7] * ic
 * z = tr[8] + tr[9] * ia + tr[10]* ib + tr[11]* ic
 *
 * Supply NULL for tr to use a unit transformation matrix.
 *
 * "datamin" and "datamax" indicate the data values which are mapped
 * to "alphamin" and "alphamax", where alpha is transparency, and 0.0
 * is completely transparent (invisible) and 1.0 is opaque.  Ordinarily,
 * set datamin and datamax to bracket the "signal" region of your data,
 * and set alphamin to 0.0 and alphamax to something like 0.7 or so.
 * "trans" controls the sort of transparency: 'o' is opaque regardless
 * of alpha settings, 't' is transparent with brightening only while 's'
 * is transparent allowing absorption.
 *
 * Set datamin > datamax to request auto-scaling.
 *
 */
int ns2cvr(float ***grid,
	   int adim, int bdim, int cdim,
	   int a1, int a2, int b1, int b2, int c1, int c2,
	   float *tr, char trans,
	   float datamin, float datamax,
	   float alphamin, float alphamax);

/* vol rendering object, but with a function for alpha rather 
 * than a linear ramp.  The ialphafn is called with a float
 * (ptr) argument being the data value (which can be outside the
 * (datamin, datamax) range of course) and should return
 * a value between 0 (transparent) and 1 (opaque).  */
int ns2cvra(float ***grid,
	    int adim, int bdim, int cdim,
	    int a1, int a2, int b1, int b2, int c1, int c2,
	    float *tr, char trans,
	    float datamin, float datamax,
	    float(*ialphafn)(float*));

/* draw a volume rendering (dynamic-only). Set force to true to
 * make the textures reload, eg. you've changed the grid */
void ds2dvr(int vrid, int force);

/* change a volume rendering data and alpha range ("level").  After
 * changing, be sure to call ds2dvr with force=1.  No protection is
 * provided against datamin > datamax! */
void ns2svrl(int vrid, float datamin, float datamax,
	     float alphamin, float alphamax);

/* set/query alpha scaling mode for volume rendering, and normal
 * scaling mode for isosurfaces: 0 => none, 
 * 1 => scale by volume dimensions, 2 => scale by viewport dimensions
 *
 * NOTE: these are not particularly well recommended for volume rendering,
 * you will do better experimenting with manual settings via ns2sevas.
 */
void ns2svas(int mode);
int ns2qvas(void);

// explicit alpha scaling values; override alpha scaling mode
// if > 0.0
void ns2sevas(float sx, float sy, float sz);

/***********************************************************************
 *
 * S2 "NATIVE" ROUTINES - in general these functions do not use
 * any S2PLOT attributes at all, except drawing remains in world
 * coordinates (therefore using the coordinate system configuration).
 * However, some of them MAY modify S2PLOT attributes
 * implicitly; this will be corrected in future versions of S2PLOT.
 *
 ***********************************************************************
 */

/* Draw a sphere, with a given centre, radius and colour.
 */
void ns2sphere(float x, float y, float z,
	       float r,
	       float red, float green, float blue);
void ns2vsphere(XYZ P, float r, COLOUR col);

/* Draw a textured sphere, with given centre, radius, colour and
 * texture filename.
 */
void ns2spheret(float x, float y, float z,
		float r,
		float red, float green, float blue,
		char *texturefn);
void ns2vspheret(XYZ P, float r, COLOUR col, char *texturefn);

/* Draw a textured sphere, with given centre, radius, colour and
 * using a texture id as returned by s2loadtexture.
 */
void ns2spherex(float x, float y, float z, float r,
		 float red, float green, float blue,
		 unsigned int texid);
void ns2vspherex(XYZ P, float r, COLOUR col, unsigned int texid);

  /* Draw a planet, which is a textured sphere whose texture can be 
   * "slid" around the planet, and the planet then rotated about an
   * arbitrary axis.
   */
void ns2vplanett(XYZ iP, float ir, COLOUR icol, char *itexturefn,
		 float texture_phase, XYZ axis, float rotation);
void ns2vplanetx(XYZ iP, float ir, COLOUR icol, 
		 unsigned int itextureid,
		 float texture_phase, XYZ axis, float rotation);

/* Draw an annulus with given centre, inner and out radii, and
 * colour.
 */
void ns2disk(float x, float y, float z,
	     float nx, float ny, float nz,
	     float r1, float r2,
	     float red, float green, float blue);
void ns2vdisk(XYZ P, XYZ N, float r1, float r2, COLOUR col);

/* Draw an arc at (px,py,pz) with normal (nx,ny,nz) in current
 * pen (thickness,color).  Vector (sx,sy,sz) gives starting vector
 * from px and is rotated around the normal in steps (deg/(nseg-1)).
 * These two functions are "extensions", and their behaviour for
 * world coordinates which have unequal scales in x, y and/or z is
 * not particularly well defined.  For equal x,y,z world scales,
 * these arcs have major/minor axis ratio of 1.0.
 */
void ns2arc(float px, float py, float pz,
	    float nx, float ny, float nz,
	    float sx, float sy, float sz,
	    float deg, int nseg);
void ns2varc(XYZ P, XYZ N, XYZ S, float deg, int nseg);

/* Draw an arc as above, but with specified major/minor axis ratio.
 * The major axis is given by the starting vector.
 */
void ns2erc(float px, float py, float pz,
	    float nx, float ny, float nz,
	    float sx, float sy, float sz,
	    float deg, int nseg, float axratio);
void ns2verc(XYZ P, XYZ N, XYZ S, float deg, int nseg, float axratio);

/* Draw text at a given position, with right and up vectors, a specific
 * colour, and a text string of course.
 */
void ns2text(float x, float y, float z,
	     float rx, float ry, float rz,
	     float ux, float uy, float uz,
	     float red, float green, float blue,
	     char *text);
void ns2vtext(XYZ P, XYZ R, XYZ U, COLOUR col, char *text);

/* Draw a point at the given position and colour.
 */
void ns2point(float x, float y, float z,
	      float red, float green, float blue);
void ns2vpoint(XYZ P, COLOUR col);
/* Draw multiple points in one colour */
void ns2vnpoint(XYZ *P, COLOUR col, int n);

/* Draw a thick point at given position, in colour and thickness in
 * pixels (not world coords).
 */
void ns2thpoint(float x, float y, float z,
		float red, float green, float blue,
		float size);
void ns2vthpoint(XYZ P, COLOUR col, float size);

/* Transparent thick dot */
void ns2vpa(XYZ P, COLOUR icol, float isize, char itrans, float ialpha);

/* Illumination */
void ns2i(float x, float y, float z,
	  float red, float green, float blue);
void ns2vi(XYZ P, COLOUR col);

/* Draw a line from one point to another in a specific colour.
 */
void ns2line(float x1, float y1, float z1,
	     float x2, float y2, float z2,
	     float red, float green, float blue);
void ns2vline(XYZ P1, XYZ P2, COLOUR col);

/* Draw a thick line from one point to another in a specific color.
 */
void ns2thline(float x1, float y1, float z1,
	       float x2, float y2, float z2,
	       float red, float green, float blue,
	       float width);
void ns2vthline(XYZ P1, XYZ P2, COLOUR col, float width);

/* Draw a wireframe cube (with axes parallel to the coordinate axes)
 * in a specific colour and thickness.*/
void ns2thwcube(float x1, float y1, float z1, 
		float x2, float y2, float z2,
		float red, float green, float blue,
		float width);
void ns2vthwcube(XYZ P1, XYZ P2, COLOUR col, float width);

/* Draw a coloured line, with colour blended between the two given
 * colours along the line.
 */
void ns2cline(float x1, float y1, float z1,
	      float x2, float y2, float z2,
	      float red1, float green1, float blue1,
	      float red2, float green2, float blue2);
void ns2vcline(XYZ P1, XYZ P2, COLOUR col1, COLOUR col2);
  /* Draw a thick coloured line, with colour blended between the 
   * two given colours along the line, and given width. */
void ns2thcline(float x1, float y1, float z1,
		float x2, float y2, float z2,
		float red1, float green1, float blue1,
		float red2, float green2, float blue2, 
		float width);
void ns2vthcline(XYZ P1, XYZ P2, COLOUR col1, COLOUR col2, float width);


/* 3-vertex facets of various specifications.
 * 1. single colour, auto-normals */
void ns2vf3(XYZ *P, COLOUR col);
/* Transparent 3-vertex facet, alpha in [0,1]; trans = 'o' (opaque),
 * 't' (addition blend - never gets dimmer), 's' (standard blend, can
 * get dimmer). Auto-normals. */
void ns2vf3a(XYZ *P, COLOUR col, char trans, float alpha);
/* 2. single colour, normals given */
void ns2vf3n(XYZ *P, XYZ *N, COLOUR col);
/* 3. coloured vertices, auto-normal */
void ns2vf3c(XYZ *P, COLOUR *col);
/* 4. the completely general 3-vertex facet: each vertex has a position,
 * a colour, and a normal.  (The simpler 3-vertex facet functions
 * call this function to do the work.) */
void ns2vf3nc(XYZ *P, XYZ *N, COLOUR *col);
/* Transparent 3-vertex facet with normals, alpha in [0,1]; trans =
 * 'o' (opaque), 't' (addition blend - never gets dimmer), 's'
 * (standard blend, can get dimmer). Auto-normals. */
void ns2vf3na(XYZ *P, XYZ *N, COLOUR col, char trans, float alpha);
/* and completely general + alpha per vertex */
void ns2vf3nca(XYZ *P, XYZ *N, COLOUR *col, char trans, float *alf);


/* 4-vertex facets of various specifications.  The vertices need not
 * be co-planar, but SHOULD be!
 * 1. single colour, auto-normals */
void ns2vf4(XYZ *P, COLOUR col);
/* 2. single colour, normals given */
void ns2vf4n(XYZ *P, XYZ *N, COLOUR col);
/* 3. coloured vertices, auto-normal */
void ns2vf4c(XYZ *P, COLOUR *col);
/* 4. the completely general 4-vertex facet: each vertex has a position,
 * a colour, and a normal.  (The simpler 4-vertex facet functions
 * call this function to do the work.) */
void ns2vf4nc(XYZ *P, XYZ *N, COLOUR *col);

/* textured 4-vertex facet */
/* scale in [0,1]; trans = 'o' (opaque), 't' (addition blend - never gets
 * dimmer), 's' (standard blend, can get dimmer) */
void ns2vf4t(XYZ *P, COLOUR col, char *texturefn, float scale,
	     char trans);

/* Draw a textured 4-vertex facet, using a texture id as returned by
 * s2loadtexture; scale should be in [0,1]; trans = 'o' or 't'. */
void ns2vf4x(XYZ *P, COLOUR col, unsigned int textureid, float scale,
	     char trans);
void ns2vf4xt(XYZ *iP, COLOUR icol, unsigned int itextureid, float iscale,
		char itrans, float ialpha);

/* draw solid cube with filled but transparent faces */
void ns2scube(float x1, float y1, float z1,
	      float x2, float y2, float z2,
	      float red, float green, float blue,
	      float alpha);
void ns2vscube(XYZ P1, XYZ P2, COLOUR col, float alpha);

/* Markers.  NOT the same as PGPLOT-type markers.
 * 0 = tetrahedron
 * 1 = 3-d plus
 * 2 = cube
 * 3 = octahedron
 */
void ns2m(int type, float size, float x, float y, float z,
	  float red, float green, float blue);
void ns2vm(int type, float size, XYZ P, COLOUR col);

/***********************************************************************
 *
 * DYNAMIC-ONLY GEOMETRY FUNCTIONS
 *
 ***********************************************************************
 */
/* draw a (dynamic) "billboard".  This is a textured facet (4 vertex)
 * that sits at a given location and always faces the camera.  By using
 * small, rotationally symmetric texture maps, the appearance of soft,
 * 3d objects can be produced at low frame-rate cost.  This function
 * should only be called from a dynamic callback: billboards cannot be
 * cached static geometry as they typically change with every refresh.
 *
 * alpha controls opacity (0.0 = transparent, 1.0 = opaque), and
 * trans ('o' = opaque, 't' = transparent, 's' = trans + absorption)
 */
void ds2bb(float x, float y, float z, float str_x, float str_y, float str_z,
	   float isize, float r, float g, float b, unsigned int itextid,
	   float alpha, char trans);
void ds2vbb(XYZ iP, XYZ iStretch,
	    float isize, COLOUR iC, unsigned int itexid,
	    float alpha, char trans);
/* draw a (dynamic) "billboard", with rotation ipangle (in degrees)
 * of the texture about the view direction */
void ds2vbbr(XYZ iP, XYZ iStretch, float isize, float ipangle,
	     COLOUR iC, unsigned int itexid,
	     float alpha, char trans);
/* billboard with aspect ratio: width:height instead of stretch, and
 * offset in screen coords (x,y) [offset.z ignored].
 */
void ds2vbbp(XYZ iP, XYZ offset, float aspect,
	     float isize, COLOUR iC, unsigned int itexid,
	     float alpha, char trans);
/* billboard with aspect ratio, offset in screen coords, and
 * rotation of the texture about the view direction */
void ds2vbbpr(XYZ iP, XYZ offset, float aspect, float isize,
	      float ipangle, COLOUR iC, unsigned int itexid,
	      float alpha, char trans);

/* draw text that faces the camera - use only from a dynamic callback.
 * offset is in character units along Right, Up and ViewDir vectors -
 * use this to displace labels in *screen* coordinates.  If scaletext
 * is true (non-zero) then text further from the camera will be made
 * larger, so that labels are equally legible at near and far distances.
 *
 * NOTE: this function uses the S2PLOT attributes for character height
 * (size) and colour.
 *
 * NOTE: z component of ioff is ignored.
 */
void ds2tb(float x, float y, float z, float x_off, float y_off,
	   char *text, int scaletext);
void ds2vtb(XYZ iP, XYZ ioff, char *text, int scaletext);

/* Protect / unprotect the dynamic geometry.  Typically use this by
 * setting ds2protect() at the end of your standard dynamic
 * callback, then then call ds2unprotect() if key presses / events
 * processed by other callbacks will result in a changed geometry
 * state.  Use with caution ... incorrect use can result in TMC
 * (total memory consumption).  Enquire protected state with
 * ds2isprotected().
 */
void ds2protect(void);
void ds2unprotect(void);
int ds2isprotected(void);

/***********************************************************************
 *
 * CALLBACK AND _S2HANDLE SYSTEM
 *
 ***********************************************************************
 */

/* Set/retrieve the dynamic geometry callback function; use NULL argument to
 * cancel callback */
void cs2scb(void *cbfn);
void *cs2qcb(void);

/* set/retrieve the dynamic callback function - this one takes a void data ptr
 * which gets handed off to the callback when it is invoked.
 */
void cs2scbx(void *icbfnx, void *data);
void *cs2qcbx(void);

/* Enable / disable / toggle animation state, ie. enable / disable / toggle
 * the dynamic geometry callback system. */
void cs2ecb();
void cs2dcb();
void cs2tcb();

/* Set/retrieve the user-installed keyboard callback function.  Use
 * NULL arg to disable the keyboard callback.  When registered, this
 * function gets to process normal keyboard (ascii) presses before
 * S2PLOT's internal handling.  It can consume the event (by returning
 * 1) and prevent the default S2PLOT handling.  A return value of 0
 * should be used when the key was not processed. The callback
 * function should have the form: int kbdcallback(unsigned char *key);
 * Shift-ESC cannot be over-ridden.
 */
void cs2skcb(void *cbfn);
void *cs2qkcb(void);

/* Set/retrieve the function to call when number-keys pressed.  Use NULL arg
 * to disable callback.  When registered, a callback overrides the
 * default s2plot handling of keys 1-7 (camera views). */
void cs2sncb(void *cbfn);
void *cs2qncb(void);

/* Register/retrieve function that will draw direct OpenGL graphics when
 * called.  This is a highly advanced capability of s2plot that is
 * open to many potential pitfalls.  Only experienced OpenGL
 * programmers should attempt to use it!  Use NULL arg to disable this
 * callback.  The coordinate system for drawing geometry in this function
 * is the viewport one.
 */
void cs2socb(void *oglfn);
void *cs2qocb(void);

/* Register/retrieve function that is called when remote control data comes in 
 * eg. from is2remote running on a wireless iPod touch.  This function
 * can consume the event (by returning 1) to prevent further (built-in)
 * handling by S2PLOT.
 */
void cs2srcb(void *remcb);
void *cs2qrcb();
void cs2srcb_sock(void *remcb);

/* Add a handle. */
void ds2ah(XYZ iP, float size, COLOUR icol, COLOUR ihilite,
	   unsigned int iid, int iselected);

/* Add a handle - with specified textures for unselected and selected  */
void ds2ahx(XYZ iP, float size, int itex, int ihitex,
	    COLOUR icol, COLOUR ihilite, 
	    unsigned int iid, int iselected);

/* Set/retrieve the handle callback function; use NULL argument to
 * cancel callback */
void cs2shcb(void *cbfn);
void *cs2qhcb(void);

/* Set the drag handle callback function; use NULL argument to
 * cancel callback */
void cs2sdhcb(void *cbfn);
void *cs2qdhcb(void);

/* set the callback function for the prompt functionality.  When
 * the users presses '~', a prompt is placed on screen and they
 * can type input.  When ENTER is pressed, the registered callback
 * is called with the command string as argument.  If ESC is pressed
 * instead, the prompt is dropped.
 */
void cs2spcb(void *pcbfn, void *data);
void *cs2qpcb(void);

/* set the text and position of the prompt.  xfrac and yfrac are
 * fractions of the viewport from bottom left.
 */
void cs2sptxy(char *prompt, float xfrac, float yfrac);

/* toggle the state of a named (dynamic) handle */
void cs2th(unsigned int iid);

/* are the handles currently visible? */
int cs2qhv();

/* enable/disable/toggle (1,0,-1) selection mode (ie. handle visibility) */
void cs2thv(int enabledisable);

/***********************************************************************
 *
 * PANELS
 *
 ***********************************************************************
 */

/* Add a new panel to the S2PLOT window.  The panel goes from (x1,y1)
 * to (x2,y2) where these are fractions of the window coordinates.
 * Individual panels can be activated and deactivated by providing the
 * panel id to the toggle function.
 *
 * Eventually each panel can have its own geometry...
 */
int xs2ap(float x1, float y1, float x2, float y2);

/* Toggle whether a particular panel is active.  Inactive panels
* are not drawn. */
void xs2tp(int panelid);

/* Select a particular panel for subsequent geometry calls... */
void xs2cp(int panelid);

/* Modify location of an existing panel */
void xs2mp(int panelid, float x1, float y1, float x2, float y2);

/* Link camera of panel with id "slaveid" to the camera of panel with
 * id "masterid".  This *cannot* be undone!
 */
void xs2lpc(int masterid, int slaveid);

/* Query whether a specified panel is active */
int xs2qpa(int panelid);
/* Query whether the currently selected panel is active */
int xs2qcpa(void);
/* Obtain the ID of the currently selected panel */
int xs2qsp(void);

/* set panel frame properties */
void xs2spp(COLOUR active, COLOUR inactive, float width);

/* Query screen (panel) width and height in PIXELS */
void ss2qsd(int *x, int *y);

/***********************************************************************
 *
 * ADVANCED TEXTURE / COLORMAP HANDLING
 *
 ***********************************************************************
 */

/* Load a texture for future (generally repeated) use */
unsigned int ss2lt(char *itexturefn);

/* get a pointer to an identified texture.  The return value of
 * the function is a (char *), which points to a chunk of memory
 * which is 4 * (width) *
 (height) bytes.  A 4-byte segment
 * of this memory contains r, g, b and a (alpha / transparency)
 * values [0,255].  If the texture is not found, NULL is returned.
 */
unsigned char *ss2gt(unsigned int textureID, int *width, int *height);

/* create a texture for the user to fill in as they see fit.  Typical
 * use is to call this function, then ss2gt and ss2pt to modify the
 * texture as desired.
 */
unsigned int ss2ct(int width, int height);

/* create a texture as per ss2ct, but texture is for "transient"
 *  use: this means the texture is much faster to create, but
 *  multi-resolution versions are not constructed/used. */
unsigned int ss2ctt(int width, int height);

/* reinstall a texture, eg. after modifying the map returned by 
 * s2gettexture.
 */
void ss2pt(unsigned int itextureID);

/* reinstall the texture, but for a "transient" texture: this routine
 * considerably faster, but multiresolution versions are not created.
 */
void ss2ptt(unsigned int itextureID);

/* load a colourmap into mem, starting at index startidx, read maximum
 * of maxn colours.  Return num read and stored.  Map file format is
 * per line:
 *
 * index red green blue
 *
 * with all integer vals.  Components are in range [0,255].
 *
 * index column is currently ignored.  After calling this function,
 * it is usual to call s2scir(startidx, startidx+retval-1) to
 * activate this colormap for subsequent use.
 */
int ss2lcm(char *imapfile, int startidx, int maxn);

/* create a texture with LaTeX commands.  The return value is the texture
 * handle (as used by eg. ns2vf4x etc).  The x:y aspect ratio of the
 * texture map is returned in "aspect".
 */
unsigned int ss2ltt(char *latexcmd, float *aspect);

#if defined(S2FREETYPE)
/* create a texture using the freetype font engine */
unsigned int ss2ftt(char *fontfilename, char *text, int fontsizepx,
		    int border);
#endif

/* delete a texture */
void ss2dt(unsigned int texid);

/***********************************************************************
 *
 * ENVIRONMENT / RENDERING ATTRIBUTES
 *
 ***********************************************************************
 */

/* Set/query sphere resolution.  Spheres are drawn with (res*res) flat
 * surfaces.  Larger spheres (or spheres that will be viewed
 * closer-up) require higher sphere resolutions.  Be warned that
 * rendering time takes a severe hit with resolutions much larger
 * than about 12.
 */
void ss2ssr(int res);
int ss2qsr(void);

/* Set/query the rendering mode.  Options are: WIREFRAME, SHADE_FLAT,
 * SHADE_DIFFUSE and SHADE_SPECULAR.
 */
void ss2srm(int mode);
int ss2qrm();

/* Set the entire lighting environment.  You can place a total of 8 lights
 * in the environment, at given positions and colours.  Set the ambient
 * light colour as you like, but if this is set to white light (1,1,1)
 * you will not see the effect of the position-specific lights */
void ss2sl(COLOUR ambient, int nlights, XYZ *lightpos,
		   COLOUR *lightcol, int worldcoords);

/* Set/query the background colour.  This call should almost always be followed
 * by calls to s2scr to set the 0th colour index to be the same as the
 * background, as well as a call to ss2sfc.  This call should only be made
 * *before* a call to s2swin.
 */
void ss2sbc(float r, float g, float b);
void ss2qbc(float *r, float *g, float *b);

/* Set/query the foreground color.  This is only used for debugging (d) and key
 * information (F1).  This should normally be followed by a call to
 * s2scr to set the 1st colour index to a similar colour.
 */
void ss2sfc(float r, float g, float b);
void ss2qfc(float *r, float *g, float *b);

/* Set/query the fisheye rotation angle (degrees).  This is only functional if
 * the projection is in use is a fisheye (or warp), and it has the effect
 * of rotating the projection "pole" away from the centre of the "screen",
 * towards the bottom of the screen, by rot degrees.
 */
void ss2sfra(float rot);
float ss2qfra(void);

/* Set/query the projection type of the device in use. Acceptable values are:
 * 0 = perspective
 * 1 = orthographic
 * 2 = fisheye
 */
void ss2spt(int projtype);
int ss2qpt();

/* Query the "screen" type, including stereo setting, fullscreen and
 * dome status/type.  See "s2const.h" for possible return values.
 */
void ss2qsa(int *stereo, int *fullscreen, int *dome);

/* Query the aspect ratio: returned value is ratio of width to height
 * ie. it is normally > 1 */
float ss2qar(void);

/* Set the expansion factor of the default near and far clipping plane
 * distance.  Values > 1 increase the separation of the near and far 
 * clipping planes, at the expense of stereo effect; values < 1 the
 * opposite.  Use with care! */
void ss2snfe(float expand);

/* get the actual near and far clip plane distances from the camera
 * in DEVICE coords */
void ss2qnfp(double *near, double *far);

/***********************************************************************
 *
 * ADVANCED CAMERA CONTROL
 *
 ***********************************************************************
 */

/* Set the camera position, up vector, view direction AND focus
 * (rotate-about) point, all in one shot.  If worldcoords > 0 then
 * caller has given world coords.  It is up to the caller to ensure a
 * self-consistent camera is given. */
void ss2scaf(XYZ position, XYZ up, XYZ vdir, XYZ focus, int worldcoords);

/* Set the camera position, up vector and view direction.  If
 * worldcoords > 0 then caller has given world coords, otherwise
 * they are viewport-relative coords. */
void ss2sc(XYZ position, XYZ up, XYZ vdir, int worldcoords);

/* Query the camera position, up vector and view direction.  If
 * worldcoords > 0 then return world coords, otherwise return
 * viewport-relative coords.  Please note that this function
 * returns camera position of LAST update.  Calling it
 * immediately after ss2sc without allowing a refresh or
 * redraw will not return identical arguments as given to
 * ss2sc.  The return value can be checked for this
 * possibility: if non-zero, then programmed changes to the
 * camera are still pending. */
int ss2qc(XYZ *position, XYZ *up, XYZ *vdir, int worldcoords);

/* Start or stop (1,0) the autospin (camera rotation) */
void ss2sas(int startstop);
/* Query autospin state */
int ss2qas(void);

/* Set/unset the camera mid/focus point.  If this is set, then rotation
 * will be about the provided point.  Unset to revert to "auto"
 * mid point.  If worldcoords > 0 then caller has given world
 * coordinates, otherwise they are viewport-relative coords. */
void ss2scf(XYZ position, int worldcoords);
void ss2ucf(void);

/* Set/query the camera aperture (in degrees). */
void ss2sca(float aperture);
float ss2qca(void);

/* Query the camera focus point, and whether it is explicitly set.
 * The return value of set indicates whether focus is "set" or
 * "unset" - see functions ss2scf and ss2ucf.  The position of the
 * camera focus point is returned in "position", in world
 * coordinates if the "worldcoords" argument is non-zero. */
void ss2qcf(int *set, XYZ *position, int worldcoords);

/* Query the point to rotate about.  Fortunately or unfortunately,
 * this is not guaranteed to be the same as the focus point (what
 * the camera is looking at); the meanings are mixed and need to be
 * disentagled.  For now, we allow querying of this point indepedent
 * of the focus point, as it is needed for some things like GPU-based
 * volume rendering. */
void ss2qpr(XYZ *position, int worldcoords);

/* Set/query the spin / interact speed.  Values between 0.01 and 20.0
 * are reasonably sensible. */
void ss2sss(float spd);
float ss2qss();

/* set/query the camera speed (ie. increment amount for camera
 * movements.  Values between 0.01 and 20.0 are reasonably sensible.
 */
void ss2scs(float spd);
float ss2qcs(void);

/* Enable/disable (1,0) camera translation */
void ss2tc(int enabledisable);

/* Set/query camera eye separation multiplier */
float ss2qess(void);
void ss2sess(float ieyesep);

/***********************************************************************
 *
 * IMAGE WRITING/FETCHING
 *
 ***********************************************************************
 */

/* Write the current frame image to a named TGA file.  This can be
 * used immediately after a call to s2disp to save a rendered image.
 * For effective use, s2disp must display the image long enough to
 * go beyond the S2PLOT "fade-in" time; the best way to do this is
 * to set the S2PLOT_FADETIME environment variable to 0.0, and then
 * call s2disp(0,1).   Do not include the ".tga" extension - this
 * is added for you. */
void ss2wtga(char *fname);

/* Fetch the current frame image to an RGB buffer.  The buffer is of
 * length height * width * 3 bytes, with each pixel a R,G,B triplet.
 * Pixels are returned in row order from the lowest to the highest
 * row, left to right in each row.  The first pixel is the bottom
 * left corner of the image.  THIS FUNCTION SHOULD ONLY BE USED
 * IN non-stereo MODES.   The caller of the function is responsible
 * for freeing the memory returned. */
unsigned char *ss2gpix(unsigned int *width, unsigned int *height);

/***********************************************************************
 *
 * MISCELLANEOUS PLOTS
 *
 ***********************************************************************
 */

/* Plot points on a sphere of given radius, at given longitude and
 * latitude (in degrees), coloured by the current colormap.  Index
 * into map is calculated linearly between dmin and dmax.  This
 * function is so called because with the right colormap, a
 * chromastereoscopic view will be produced for observing from the
 * origin of the coordinate system.  Size contains the desired
 * sizes of the points.  And a cartesian version follows.
 */
void s2chromapts(int n, float *ilong, float *lat, float *dist,
		 float *size, float radius, float dmin, float dmax);
void s2chromacpts(int n, float *ix, float *iy, float *iz,
		  float *dist, float *size, float dmin, float dmax);

/* Draw a vector map of a 3D data array, with blanking.  Based on the
 * PGPlot function pgvect, we have args as follows:
 *
 * a, b and c are 3d arrays indexed by
 * [0..(adim-1)][0..(bdim-1)][0..(cdim-1)] holding the components of
 * the vectors in three orthogonal directions.
 *
 * The slice of data actually plotted is indexed by
 * [a1..a2][b1..b2][c1..c2].
 *
 * Vector lengths are scaled by the value "scale".  There is no "auto-set"
 * as per PGPlot's pgvect function.
 *
 * "nc" controls positioning of the vectors.  nc<0 places the head of the
 * vector on the coordinates; nc>0 places the vector base on the coords, and
 * nc == 0 centres the vector on the coords.
 *
 * "tr" is the transformation matrix which maps indexes into the arrays
 * onto the x, y and z axes of the 3d space.  NOTE: this transformation
 * IS NOT APPLIED to the vector components!!!  The standard (non-rotated,
 * non-skewed) transformation would have tr[2], tr[3], tr[5], tr[7], tr[9]
 * and tr[10] all zero.
 *
 * x = tr[0] + tr[1] * ia + tr[2] * ib + tr[3] * ic
 * y = tr[4] + tr[5] * ia + tr[6] * ib + tr[7] * ic
 * z = tr[8] + tr[9] * ia + tr[10]* ib + tr[11]* ic
 *
 * Vectors in whose length (sqrt(a[...]^2 + b[...]^2 + c[...]^2)) is less
 * than "minlength" are not drawn.
 *
 * If "colbylength" is > 0, the the vectors will be colored by mapping
 * those of length "minl" or smaller to the start of the current color
 * index range, and those of length "maxl" or greater to the end of the
 * current color index range.  This can look sweeet :)
 *
 * STATUS: functional, might like to revisit transformation application
 * as described above.
 */
void s2vect3(float ***a, float ***b, float ***c,
	     int adim, int bdim, int cdim, 
	     int a1, int a2, int b1, int b2, int c1, int c2,
	     float scale, int nc,
	     float *tr, float minlength,
	     int colbylength, float minl, float maxl);

/* "Skyscraper" plot with arbitrary rotation / skew / translation */
/* If walls = 1, then draw the building walls. */
void s2skypa(float **data, int nx, int ny,
	     int i1, int i2, int j1, int j2,
	     float datamin, float datamax,
	     float *tr, int walls, int idx_left, int idx_front);

/* "Impulse" plot with arbitrary rotation / skew / translation.
 * Draw "trunks" of points if "trunk" is true.  Use point types as
 * for s2pt.
 */
void s2impa(float **data, int nx, int ny,
	    int i1, int i2, int j1, int j2,
	    float datamin, float datamax,
	    float *tr, int trunk, int symbol);

/***********************************************************************
 *
 * MISCELLANEOUS ROUTINES
 *
 ***********************************************************************
 */

/* Write username, date, time and optional string at bottom of plot.
 */
void s2iden(char *textra);

/* Push the given name onto the VRML "name stack".  When VRML data
 * is written, it is written in "groups" with these names. */
void pushVRMLname(char *iname);

/***********************************************************************
 *
 * FUNCTIONS IN TESTING / DEVELOPMENT
 *
 * NOTE TO USERS: these functions are development functions, trying out
 *   new ideas, etc.  Your mileage will vary.
 *
 * NOTE TO DEVELOPERS: please implement FORTRAN stubs prior to moving
 *   functions out of this category.
 *
 ***********************************************************************
 */



#if defined(S2_3D_TEXTURES)
  /* textured polygon */
  void ns2texpoly3d(XYZ *iP, XYZ *iTC, float in, 
		  unsigned int texid, char itrans, float ialpha);

  /* create an empty 3d texture */
  unsigned int ss2c3dt(int width, int height, int depth);

  /* fetch pointer to 3d texture */
  unsigned char *ss2g3dt(unsigned int itextureID, int *width, int *height, int *depth);
#endif

  void ns2texmesh(int inverts, XYZ *iverts,
		int innorms, XYZ *inorms,
		int invtcs, XYZ *ivtcs,
		int infacets, int *ifacets, int *ifacets_tcs,
		unsigned int itexid,
		char itrans,
		  float ialpha);

  // "INTERNAL" 
  /* set debugging on or off */
  void zs2debug(int debug);

  /* set buffer swapping on or off */
  void zs2bufswap(int bufswap);

  /* set retain lists on or off */
  void zs2retainlists(int retlists);

/***********************************************************************
 *
 * LEGACY FUNCTION NAME SUPPORT
 *
 ***********************************************************************
 */

// S2 NATIVE ROUTINES
#define ns2Vsphere(...) ns2vsphere(__VA_ARGS__)
#define ns2Vspheret(...) ns2vspheret(__VA_ARGS__)
#define s2spheretID(...) ns2spherex(__VA_ARGS__)
#define s2VspheretID(...) ns2vspherex(__VA_ARGS__)
#define ns2Vdisk(...) ns2vdisk(__VA_ARGS__)
#define s2disk(...) ns2arc(__VA_ARGS__)
#define s2Vdisk(...) ns2varc(__VA_ARGS__)
#define ns2Vtext(...) ns2vtext(__VA_ARGS__)
#define ns2Vpoint(...) ns2vpoint(__VA_ARGS__)
#define ns2VNpoint(...) ns2vnpoint(__VA_ARGS__)
#define ns2Vthpoint(...) ns2vthpoint(__VA_ARGS__)
#define ns2Vi(...) ns2vi(__VA_ARGS__)
#define ns2Vline(...) ns2vline(__VA_ARGS__)
#define ns2Vthline(...) ns2vthline(__VA_ARGS__)
#define ns2Vcline(...) ns2vcline(__VA_ARGS__)
#define ns2Vf3(...) ns2vf3(__VA_ARGS__)
#define ns2Vf3n(...) ns2vf3n(__VA_ARGS__)
#define ns2Vf3c(...) ns2vf3c(__VA_ARGS__)
#define ns2Vf3nc(...) ns2vf3nc(__VA_ARGS__)
#define ns2Vf4(...) ns2vf4(__VA_ARGS__)
#define ns2Vf4n(...) ns2vf4n(__VA_ARGS__)
#define ns2Vf4c(...) ns2vf4c(__VA_ARGS__)
#define ns2Vf4nc(...) ns2vf4nc(__VA_ARGS__)
#define ns2Vf4t(...) ns2vf4t(__VA_ARGS__)
#define s2Vf4tID(...) ns2vf4x(__VA_ARGS__)
#define s2Vf4tIDt(...) ns2vf4xt(__VA_ARGS__)
#define ns2Vm(...) ns2vm(__VA_ARGS__)


// DYNAMIC GEOMETRY
#define s2dynbillboard(...) ds2vbb(__VA_ARGS__)

// CALLBACKS, _S2HANDLES
#define s2scb(...) cs2scb(__VA_ARGS__)
#define s2ecb(...) cs2ecb(__VA_ARGS__)
#define s2dcb(...) cs2dcb(__VA_ARGS__)
#define s2tcb(...) cs2tcb(__VA_ARGS__)
#define s2snumcb(...) cs2sncb(__VA_ARGS__)
#define s2soglcb(...) cs2socb(__VA_ARGS__)
#define s2handle(...) ds2ah(__VA_ARGS__)
#define cs2ah(...) ds2ah(__VA_ARGS__)
#define s2shcb(...) cs2shcb(__VA_ARGS__)
#define s2togglehandle(...) cs2th(__VA_ARGS__)
#define s2handlesvisible(...) cs2qhv(__VA_ARGS__)

// TEXTURES, COLORMAPS
#define s2loadtexture(...) ss2lt(__VA_ARGS__)
#define s2gettexture(...) ss2gt(__VA_ARGS__)
#define s2pushtexture(...) ss2pt(__VA_ARGS__)
#define s2loadmap(...) ss2lcm(__VA_ARGS__)
#define s2latexture(...) ss2ltt(__VA_ARGS__)
#define font2tex(...) ss2ftt(__VA_ARGS__)

// RENDERING, ENVIRONMENT
#define ns2ssphres(...) ss2ssr(__VA_ARGS__)
#define ns2srenmode(...) ss2srm(__VA_ARGS__)
#define ns2grenmode(...) ss2qrm(__VA_ARGS__)
#define s2setlighting(...) ss2sl(__VA_ARGS__)
#define s2sbc(...) ss2sbc(__VA_ARGS__)
#define ns2fishr(...) ss2sfra(__VA_ARGS__)
#define ns2ptype(...) ss2qpt(__VA_ARGS__)

// CAMERA
#define ns2setcam(...) ss2sc(__VA_ARGS__)
#define ns2getcam(...) ss2qc(__VA_ARGS__)
#define ns2rotcam(...) ss2sas(__VA_ARGS__)
#define ns2setfocus(...) ss2scf(__VA_ARGS__)
#define ns2unsetfocus(...) ss2ucf(__VA_ARGS__)
#define ns2sspinspd(...) ss2sss(__VA_ARGS__)
#define ns2gspinspd(...) ss2qss(__VA_ARGS__)
#define s2transcam(...) ss2tc(__VA_ARGS__)

#if defined(__cplusplus) && !defined(S2_CPPBUILD)
} /* extern "C" { */
#endif

#endif /* S2PLOT_H */
