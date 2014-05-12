/* s2plot_fstubs.c
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
 * $Id: s2plot_fstubs.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 * Most of these are simple wrappers that add underscores and follow
 * FORTRAN argument conventions.  Strings have to be handled though,
 * and null terminators added etc.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "s2plot.h"

/* Error handling */
void _s2warn(char *fn, char *messg, ...);

/* Convert a FORTRAN string and length to a C string with null
 * termination.  It is the caller's responsibility to free the
 * returned pointer when it is finished with.
 */
char *_s2_f2cstr(char *text, int textlen) {
  int l = textlen + 1;
  char *retval = (char *)calloc(l, sizeof(char));
  if (!retval) {
    return NULL;
  }
  strncpy(retval, text, l-1);
  retval[l-1] = '\0';
  return retval;
}

/* Convert a C string with null termination to a FORTRAN string
 * and length. It is the caller's responsibility to free the
 * returned pointer when it is finished with.
 */
/* TODO - Test/correct this */
char *_s2_c2fstr(char *cstring, long int *textlen) {
  *textlen = strlen(cstring); /* Return value of strlen does NOT include null byte */
  char *retval = (char *)calloc(*textlen, sizeof(char));
  if (!retval) return NULL;
  strncpy(retval, cstring, *textlen);
  return retval;
}

/* Convert a FORTRAN 2d array into a float **-style array.  The
 * returned array can be freed with the partner function below.
 */
float **_s2_f2c2darr(float *dat, int nx, int ny) {
  int i, j;
  float **cdat = (float **)calloc(nx, sizeof(float *));
  for (i = 0; i < nx; i++) {
    cdat[i] = (float *)calloc(ny, sizeof(float));
    for (j = 0; j < ny; j++) {
      cdat[i][j] = dat[j * nx + i];
    }
  }
  return cdat;
}
void _s2_free2darr(float **cdat, int nx) {
  int i;
  for (i = 0; i < nx; i++) {
    free(cdat[i]);
  }
  free(cdat);
}

/* Convert a FORTRAN 3d array into a float ***-style array.  The
 * returned array can be freed with the partner function below.
 */
float ***_s2_f2c3darr(float *dat, int nx, int ny, int nz) {
  int i, j, k;
  float ***cdat = (float ***)calloc(nx, sizeof(float **));
  for (i = 0; i < nx; i++) {
    cdat[i] = (float **)calloc(ny, sizeof(float *));
    for (j = 0; j < ny; j++) {
      cdat[i][j] = (float *)calloc(nz, sizeof(float));
      for (k = 0; k < nz; k++) {
	cdat[i][j][k] = dat[k * nx * ny + j * ny + i];
      }
    }
  }
  return cdat;
}
void _s2_free3darr(float ***cdat, int nx, int ny) {
  int i, j;
  for (i = 0; i < nx; i++) {
    for (j = 0; j < ny; j++) {
      free(cdat[i][j]);
    }
    free(cdat[i]);
  }
  free(cdat);
}

/***********************************************************************
 *
 * OPENING, CLOSING AND SELECTING DEVICES
 *
 ***********************************************************************
 */

int s2open_(int *ifullscreen, int *istereo) {
  int iargc = 0;
  char *iargv[1];
  iargv[0] = (char *)calloc(strlen("s2plot.exe")+1, sizeof(char));
  strcpy(iargv[0], "s2plot.exe");
  int result = s2open(*ifullscreen, *istereo, iargc, iargv);
  free(iargv[0]);
  return result;
}
int s2opend_(char *device, long int devlen) {
  char *tdev = _s2_f2cstr(device, devlen);
  int result = s2opendo(tdev);
  free(tdev);
  return(result);
}
int s2opendo_(char *device, long int devlen) {
  char *tdev = _s2_f2cstr(device, devlen);
  int result = s2opendo(tdev);
  free(tdev);
  return(result);
}
void s2ldev_() {
  s2ldev();
}
void s2show_(int *interactive) {
  s2show(*interactive);
}
void s2disp_(int *idelay, int *irestorecamera) {
  s2disp(*idelay, *irestorecamera);
}
void s2eras_() {
  s2eras();
}
#if defined(S2THREADS)
void s2show_thr_(int *interactive) {
  s2show_thr(*interactive);
}
#endif

/***********************************************************************
 *
 * WINDOWS AND VIEWPORTS
 *
 ***********************************************************************
 */
void s2svp_(float *x1, float *x2, float *y1, float *y2, 
	    float *z1, float *z2) {
  s2svp(*x1, *x2, *y1, *y2, *z1, *z2);
}
void s2qvp_(float *dx1, float *dx2, float *dy1, float *dy2,
	    float *dz1, float *dz2) {
  s2qvp(dx1, dx2, dy1, dy2, dz1, dz2);
}
void s2swin_(float *x1, float *x2, float *y1, float *y2, 
	     float *z1, float *z2) {
  s2swin(*x1, *x2, *y1, *y2, *z1, *z2);
}
void s2qwin_(float *x1, float *x2, float *y1, float *y2,
	     float *z1, float *z2) {
  s2qwin(x1, x2, y1, y2, z1, z2);
}
void s2env_(float *xmin, float *xmax, float *ymin, float *ymax,
	    float *zmin, float *zmax, int *just, int *axis) {
  s2env(*xmin, *xmax, *ymin, *ymax, *zmin, *zmax, *just, *axis);
}

/***********************************************************************
 *
 * PRIMITIVES
 *
 ***********************************************************************
 */
void s2line_(int *n, float *xpts, float *ypts, float *zpts) {
  s2line(*n, xpts, ypts, zpts);
}
void s2circxy_(float *px, float *py, float *pz, float *r, int *nseg, 
	      float *asp) {
  s2circxy(*px, *py, *pz, *r, *nseg, *asp);
}
void s2circxz_(float *px, float *py, float *pz, float *r, int *nseg, 
	      float *asp) {
  s2circxz(*px, *py, *pz, *r, *nseg, *asp);
}
void s2circyz_(float *px, float *py, float *pz, float *r, int *nseg, 
	      float *asp) {
  s2circyz(*px, *py, *pz, *r, *nseg, *asp);
}
void s2diskxy_(float *px, float *py, float *pz, float *r1, float *r2) {
  s2diskxy(*px, *py, *pz, *r1, *r2);
}
void s2diskxz_(float *px, float *py, float *pz, float *r1, float *r2) {
  s2diskxz(*px, *py, *pz, *r1, *r2);
}
void s2diskyz_(float *px, float *py, float *pz, float *r1, float *r2) {
  s2diskyz(*px, *py, *pz, *r1, *r2);
}
void s2rectxy_(float *xmin, float *xmax, float *ymin, float *ymax, float *z) {
  s2rectxy(*xmin, *xmax, *ymin, *ymax, *z);
}
void s2rectxz_(float *xmin, float *xmax, float *zmin, float *zmax, float *y) {
  s2rectxz(*xmin, *xmax, *zmin, *zmax, *y);
}
void s2rectyz_(float *ymin, float *ymax, float *zmin, float *zmax, float *x) {
  s2rectyz(*ymin, *ymax, *zmin, *zmax, *x);
}

void s2wcube_(float *xmin, float *xmax, 
	      float *ymin, float *ymax,
	      float *zmin, float *zmax) {
  s2wcube(*xmin, *xmax, *ymin, *ymax, *zmin, *zmax);
}

void s2pt1_(float *x, float *y, float *z, int *symbol) {
  s2pt1(*x, *y, *z, *symbol);
}
void s2pt_(int *np, float *xpts, float *ypts, float *zpts, int *symbol) {
  s2pt(*np, xpts, ypts, zpts, *symbol);
}
void s2pnts_(int *np, float *xpts, float *ypts, float *zpts, int *symbols,
	     int *ns) {
  s2pnts(*np, xpts, ypts, zpts, symbols, *ns);
}
void s2textxy_(float *x, float *y, float *z, char *text, long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2textxy(*x, *y, *z, t1);
  free(t1);
}
void s2textxz_(float *x, float *y, float *z, char *text, long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2textxz(*x, *y, *z, t1);
  free(t1);
}
void s2textyz_(float *x, float *y, float *z, char *text, long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2textyz(*x, *y, *z, t1);
  free(t1);
}
void s2textxyf_(float *x, float *y, float *z, float *flipx, float *flipy,
		char *text, long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2textxyf(*x, *y, *z, *flipx, *flipy, t1);
  free(t1);
}
void s2textxzf_(float *x, float *y, float *z, float *flipx, float *flipz,
		char *text, long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2textxzf(*x, *y, *z, *flipx, *flipz, t1);
  free(t1);
}
void s2textyzf_(float *x, float *y, float *z, float *flipy, float *flipz,
		char *text, long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2textyzf(*x, *y, *z, *flipy, *flipz, t1);
  free(t1);
}
void s2qtxtxy_(float *x1, float *x2, float *y1, float *y2,
	       float *x, float *y, float *z, char *text, float *pad,
	       long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2qtxtxy(x1, x2, y1, y2, *x, *y, *z, t1, *pad);
  free(t1);
}
void s2qtxtxz_(float *x1, float *x2, float *z1, float *z2,
	       float *x, float *y, float *z, char *text, float *pad,
	       long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2qtxtxz(x1, x2, z1, z2, *x, *y, *z, t1, *pad);
  free(t1);
}
void s2qtxtyz_(float *y1, float *y2, float *z1, float *z2,
	       float *x, float *y, float *z, char *text, float *pad,
	       long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2qtxtyz(y1, y2, z1, z2, *x, *y, *z, t1, *pad);
  free(t1);
}
void s2qtxtxyf_(float *x1, float *x2, float *y1, float *y2,
		float *x, float *y, float *z, float *flipx, float *flipy,
		char *text, float *pad, long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2qtxtxyf(x1, x2, y1, y2, *x, *y, *z, *flipx, *flipy, t1, *pad);
  free(t1);
}
void s2qtxtxzf_(float *x1, float *x2, float *z1, float *z2,
		float *x, float *y, float *z, float *flipx, float *flipz,
		char *text, float *pad,	long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2qtxtxzf(x1, x2, z1, z2, *x, *y, *z, *flipx, *flipz, t1, *pad);
  free(t1);
}
void s2qtxtyzf_(float *y1, float *y2, float *z1, float *z2,
		float *x, float *y, float *z, float *flipy, float *flipz,
		char *text, float *pad,	long int textlen) {
  char *t1 = _s2_f2cstr(text, textlen);
  s2qtxtyzf(y1, y2, z1, z2, *x, *y, *z, *flipy, *flipz, t1, *pad);
  free(t1);
}
void s2arro_(float *x1, float *y1, float *z1,
	     float *x2, float *y2, float *z2) {
  s2arro(*x1, *y1, *z1, *x2, *y2, *z2);
}


/***********************************************************************
 *
 * ATTRIBUTES
 *
 ***********************************************************************
 */
void s2sci_(int *idx) {
  s2sci(*idx);
}
int s2qci_() {/*NEW*/
  return s2qci();
}
void s2scr_(int *idx, float *r, float *g, float *b) {
  s2scr(*idx, *r, *g, *b);
}
void s2qcr_(int *idx, float *r, float *g, float *b) {
  s2qcr(*idx, r, g, b);
}
void s2slw_(float *width) {
  s2slw(*width);
}
int s2qlw_() {/*NEW*/
  return s2qlw();
}
void s2sls_(float *width) {/*NEW*/
  s2sls(*width);
}
int s2qls_() {/*NEW*/
  return s2qls();
}
void s2sch_(float *size) {
  s2sch(*size);
}
int s2qch_() {/*NEW*/
  return s2qch();
}
void ss2txh_(int *enabledisable) {/*NEW*/
  ss2txh(*enabledisable);
}
int ss2qxh_() {/*NEW*/
  return ss2qxh();
}
void ss2tsc_(char *whichscreens, long int ws_length) {/*NEW*/
  char *ws = _s2_f2cstr(whichscreens, ws_length);
  ss2tsc(ws);
  free(ws);
}
void s2sah_(int *fs, float *angle, float *barb) {
  s2sah(*fs, *angle, *barb);
}
void s2qah_(int *fs, float *angle, float *barb) {
  s2qah(fs, angle, barb);
}

void s2twc_(int *enabledisable) {
  s2twc(*enabledisable);
}
int s2qwc_() {
  return s2qwc();
}

/***********************************************************************
 *
 * AXES, BOXES & LABELS
 *
 ***********************************************************************
 */
void s2box_(char *xopt, float *xtick, int *nxsub,
	    char *yopt, float *ytick, int *nysub,
	    char *zopt, float *ztick, int *nzsub,
	    long int xoptlen, long int yoptlen, long int zoptlen) {
  char *cxopt = _s2_f2cstr(xopt, xoptlen);
  char *cyopt = _s2_f2cstr(yopt, yoptlen);
  char *czopt = _s2_f2cstr(zopt, zoptlen);
  s2box(cxopt, *xtick, *nxsub, cyopt, *ytick, *nysub, czopt, *ztick, *nzsub);
  free(czopt);
  free(cyopt);
  free(cxopt);
}
void s2lab_(char *xlab, char *ylab, char *zlab, char *title,
	    long int xlablen, long int ylablen, long int zlablen,
	    long int titlen) {
  char *cxlab = _s2_f2cstr(xlab, xlablen);
  char *cylab = _s2_f2cstr(ylab, ylablen);
  char *czlab = _s2_f2cstr(zlab, zlablen);
  char *ctitl = _s2_f2cstr(title, titlen);
  s2lab(cxlab, cylab, czlab, ctitl);
  free(ctitl);
  free(czlab);
  free(cylab);
  free(cxlab);
}
void s2help_(char *helpstr, long int helplen) {
  char *hstr = _s2_f2cstr(helpstr, helplen);
  s2help(hstr);
  free(hstr);
}


/***********************************************************************
 *
 * XY(Z) PLOTS
 *
 ***********************************************************************
 */
void s2errb_(int *dir, int *n, float *xpts, float *ypts, float *zpts,
	     float *edelt, int *termsize) {
  s2errb(*dir, *n, xpts, ypts, zpts, edelt, *termsize);
}
void s2funt_(void *fx, void *fy, void *fz, int *n, float *tmin, float *tmax) {
  float(*the_fx)(float*) = (float (*)(float *))fx;
  float(*the_fy)(float*) = (float (*)(float *))fy;
  float(*the_fz)(float*) = (float (*)(float *))fz;
  s2funt(the_fx, the_fy, the_fz, *n, *tmin, *tmax);
}
void s2funtc_(void *fx, void *fy, void *fz, void *fc, int *n, 
	      float *tmin, float *tmax) {
  float(*the_fx)(float*) = (float (*)(float *))fx;
  float(*the_fy)(float*) = (float (*)(float *))fy;
  float(*the_fz)(float*) = (float (*)(float *))fz;
  float(*the_fc)(float*) = (float (*)(float *))fc;
  s2funtc(the_fx, the_fy, the_fz, the_fc, *n, *tmin, *tmax);
}

void s2funxy_(void *fxy, int *nx, int *ny, float *xmin, float *xmax,
	      float *ymin, float *ymax, int *ctl) {
  float(*the_fxy)(float*,float*) = (float (*)(float *, float *))fxy;
  s2funxy(the_fxy, *nx, *ny, *xmin, *xmax, *ymin, *ymax, *ctl);
}
void s2funxz_(void *fxz, int *nx, int *nz, float *xmin, float *xmax,
	      float *zmin, float *zmax, int *ctl) {
  float(*the_fxz)(float*,float*) = (float (*)(float *, float *))fxz;
  s2funxz(the_fxz, *nx, *nz, *xmin, *xmax, *zmin, *zmax, *ctl);
}
void s2funyz_(void *fyz, int *ny, int *nz, float *ymin, float *ymax,
	      float *zmin, float *zmax, int *ctl) {
  float(*the_fyz)(float*,float*) = (float (*)(float *, float *))fyz;
  s2funyz(the_fyz, *ny, *nz, *ymin, *ymax, *zmin, *zmax, *ctl);
}

void s2funxyr_(void *fxy, int *nx, int *ny, float *xmin, float *xmax,
	       float *ymin, float *ymax, int *ctl, 
	       float *rmin, float *rmax) {
  float(*the_fxy)(float*,float*) = (float (*)(float *, float *))fxy;
  s2funxyr(the_fxy, *nx, *ny, *xmin, *xmax, *ymin, *ymax, *ctl, *rmin, *rmax);
}
void s2funxzr_(void *fxz, int *nx, int *nz, float *xmin, float *xmax,
	       float *zmin, float *zmax, int *ctl, 
	       float *rmin, float *rmax) {
  float(*the_fxz)(float*,float*) = (float (*)(float *, float *))fxz;
  s2funxzr(the_fxz, *nx, *nz, *xmin, *xmax, *zmin, *zmax, *ctl, *rmin, *rmax);
}
void s2funyzr_(void *fyz, int *ny, int *nz, float *ymin, float *ymax,
	       float *zmin, float *zmax, int *ctl, 
	       float *rmin, float *rmax) {
  float(*the_fyz)(float*,float*) = (float (*)(float *, float *))fyz;
  s2funyzr(the_fyz, *ny, *nz, *ymin, *ymax, *zmin, *zmax, *ctl, *rmin, *rmax);
}

void s2funuv_(void *fx, void *fy, void *fz, void *fcol,
	      float *umin, float *umax, int *uDIV,
	      float *vmin, float *vmax, int *vDIV) {
  float(*the_fx)(float*,float*) = (float (*)(float *, float *))fx;
  float(*the_fy)(float*,float*) = (float (*)(float *, float *))fy;
  float(*the_fz)(float*,float*) = (float (*)(float *, float *))fz;
  float(*the_fcol)(float*,float*) = (float (*)(float *, float *))fcol;
  s2funuv(the_fx, the_fy, the_fz, the_fcol, *umin, *umax, *uDIV,
	  *vmin, *vmax, *vDIV);
}

void s2funuva_(void *fx, void *fy, void *fz, void *fcol,
	       char *trans, void *falpha,
	      float *umin, float *umax, int *uDIV,
	      float *vmin, float *vmax, int *vDIV) {
  float(*the_fx)(float*,float*) = (float (*)(float *, float *))fx;
  float(*the_fy)(float*,float*) = (float (*)(float *, float *))fy;
  float(*the_fz)(float*,float*) = (float (*)(float *, float *))fz;
  float(*the_fcol)(float*,float*) = (float (*)(float *, float *))fcol;
  float(*the_falpha)(float*,float*) = (float (*)(float *, float *))falpha;
  s2funuva(the_fx, the_fy, the_fz, the_fcol, *trans, the_falpha,
	   *umin, *umax, *uDIV, *vmin, *vmax, *vDIV);
}


/***********************************************************************
 *
 * IMAGES / SURFACES
 *
 ***********************************************************************
 */

void s2surp_(float *data, int *nx, int *ny, int *i1, int *i2, 
	     int *j1, int *j2, float *datamin, float *datamax, 
	     float *tr) {
  float **cdata = _s2_f2c2darr(data, *nx, *ny);
  s2surp(cdata, *nx, *ny, *i1, *i2, *j1, *j2, *datamin, *datamax, tr);
  _s2_free2darr(cdata, *nx);
}
void s2surpa_(float *data, int *nx, int *ny, int *i1, int *i2, 
	      int *j1, int *j2, float *datamin, float *datamax, 
	      float *tr) {
  float **cdata = _s2_f2c2darr(data, *nx, *ny);
  s2surpa(cdata, *nx, *ny, *i1, *i2, *j1, *j2, *datamin, *datamax, tr);
  _s2_free2darr(cdata, *nx);
}
void s2scir_(int *col1, int *col2) {
  s2scir(*col1, *col2);
}
void s2qcir_(int *col1, int *col2) {
  s2qcir(col1, col2);
}
int s2icm_(char *mapname, int *idx1, int *idx2, long int textlen) {
  char *tmap = _s2_f2cstr(mapname, textlen);
  int retval = s2icm(tmap, *idx1, *idx2);
  free(tmap);
  return(retval);
}

/***********************************************************************
 *
 * ISOSURFACES
 *
 ***********************************************************************
 */

int ns2cis_(float *grid, int *adim, int *bdim, int *cdim,
	    int *a1, int *a2, int *b1, int *b2, int *c1, int *c2,
	    float *tr, float *level, int *resolution,
	    char *trans, float *alpha, float *red, float *green, float *blue) {
  int idret;
  float ***c3grid = _s2_f2c3darr(grid, *adim, *bdim, *cdim);
  idret = ns2cis(c3grid, *adim, *bdim, *cdim,
		 *a1, *a2, *b1, *b2, *c1, *c2, 
		 tr, *level, *resolution,
		 *trans, *alpha, *red, *green, *blue);
  // Cannot delete our c-version of the array because it hasn't been used yet!
  // This is problematic because the user might update their FORTRAN array and 
  // see no change herein.  An additional layer is needed in the FORTRAN stubs
  // to store the user-space grid and re-create the C version each actual
  // redraw.
  // _s2_free3darr(c3grid, *adim, *bdim);
  _s2warn("ns2cis", "FORTRAN issue: isosurface will not respond to changes in data grid.\n");
  return idret;
}
int ns2cisc_(float *grid, int *adim, int *bdim, int *cdim,
	     int *a1, int *a2, int *b1, int *b2, int *c1, int *c2,
	     float *tr, float *level, int *resolution,
	     char *trans, float *alpha, void *ffc) {
  void(*the_fcol)(float *, float *, float *, float *, float *, float *) =
    (void (*)(float *, float *, float *, float *, float *, float *))ffc;
  int idret;
  float ***c3grid = _s2_f2c3darr(grid, *adim, *bdim, *cdim);
  idret = ns2cisc(c3grid, *adim, *bdim, *cdim,
		  *a1, *a2, *b1, *b2, *c1, *c2, 
		  tr, *level, *resolution,
		  *trans, *alpha, the_fcol);
  // Cannot delete our c-version of the array (see ns2cis_ wrapper function)
  _s2warn("ns2cis", "FORTRAN issue: isosurface will not respond to changes in data grid.\n");
  return idret;
}

/* draw / modify isosurface object */
void ns2dis_(int *isid, int *force) {
  ns2dis(*isid, *force);
}
void ns2sisl_(int *isid, float *level) {
  ns2sisl(*isid, *level);
}
void ns2sisa_(int *isid, float *alpha, char *trans) {/*NEW*/
  ns2sisa(*isid, *alpha, *trans); /*Not sure if this is correct */
}
void ns2sisc_(int *isid, float *r, float *g, float *b) {/*NEW*/
  ns2sisc(*isid, *r, *g, *b);
}

/* set/query surface quality */
void ns2ssq_(int *hiq) {
  ns2ssq(*hiq);
}
int ns2qsq_(void) {
  return ns2qsq();
}

/***********************************************************************
 *
 * VOLUME RENDERING
 *
 ***********************************************************************
 */
int ns2cvr_(float *grid, int *adim, int *bdim, int *cdim,
	    int *a1, int *a2, int *b1, int *b2, int *c1, int *c2,
	    float *tr, char *trans, 
	    float *datamin, float *datamax,
	    float *alphamin, float *alphamax) {
  int idret;
  float ***c3grid = _s2_f2c3darr(grid, *adim, *bdim, *cdim);
  idret = ns2cvr(c3grid, *adim, *bdim, *cdim, 
		 *a1, *a2, *b1, *b2, *c1, *c2, 
		 tr, *trans,
		 *datamin, *datamax,
		 *alphamin, *alphamax);
  // Cannot delete our c-version of the array because it hasn't been used yet!
  // This is problematic because the user might update their FORTRAN array and 
  // see no change herein.  An additional layer is needed in the FORTRAN stubs
  // to store the user-space grid and re-create the C version each actual
  // redraw.
  // _s2_free3darr(c3grid, *adim, *bdim);
  _s2warn("ns2cvr", "FORTRAN issue: volume rendering will not respond to changes in data grid.\n");
  return idret;
}

int ns2cvra_(float *grid, int *adim, int *bdim, int *cdim,
	     int *a1, int *a2, int *b1, int *b2, int *c1, int *c2,
	     float *tr, char *trans, 
	     float *datamin, float *datamax,
	     void *falpha) {
  int idret;
  float ***c3grid = _s2_f2c3darr(grid, *adim, *bdim, *cdim);
  float(*the_falpha)(float*) = (float (*)(float *))falpha;
  idret = ns2cvra(c3grid, *adim, *bdim, *cdim, 
		  *a1, *a2, *b1, *b2, *c1, *c2, 
		  tr, *trans, *datamin, *datamax, the_falpha);
  // Cannot delete our c-version of the array because it hasn't been used yet!
  // This is problematic because the user might update their FORTRAN array and 
  // see no change herein.  An additional layer is needed in the FORTRAN stubs
  // to store the user-space grid and re-create the C version each actual
  // redraw.
  // _s2_free3darr(c3grid, *adim, *bdim);
  _s2warn("ns2cvr", "FORTRAN issue: volume rendering will not respond to changes in data grid.\n");
  return idret;
}

void ds2dvr_(int *vrid, int *force) {
  ds2dvr(*vrid, *force);
}

void ns2svrl_(int *vrid, float *datamin, float *datamax,
	      float *alphamin, float *alphamax) {
  ns2svrl(*vrid, *datamin, *datamax, *alphamin, *alphamax);
}

/* set/query alpha scaling */
void ns2svas_(int *mode) {
  ns2svas(*mode);
}
int ns2qvas_(void) {
  return ns2qvas();
}
void ns2sevas_(float *sx, float *sy, float *sz) {
  ns2sevas(*sx, *sy, *sz);
}

/***********************************************************************
 *
 * S2 "NATIVE" ROUTINES - "exact" matches to GEOM format - do not use
 * any S2PLOT attributes at all, except drawing remains in world
 * coordinates.
 *
 ***********************************************************************
 */

void ns2sphere_(float *x, float *y, float *z, float *r,
		float *red, float *green, float *blue) {
  ns2sphere(*x, *y, *z, *r, *red, *green, *blue);
}
void ns2vsphere_(XYZ *P, float *r, COLOUR *col) {
  ns2vsphere(*P, *r, *col);
}

void ns2spheret_(float *x, float *y, float *z, float *r,
		float *red, float *green, float *blue, 
		char *texturefn, long int textlen) {
  char *ttex = _s2_f2cstr(texturefn, textlen);
  ns2spheret(*x, *y, *z, *r, *red, *green, *blue, ttex);
  free(ttex);
}
void ns2vspheret_(XYZ *P, float *r, COLOUR *col, char *texturefn,
		  long int textlen) {
  char *ttex = _s2_f2cstr(texturefn, textlen);
  ns2Vspheret(*P, *r, *col, ttex);
  free(ttex);
}

void ns2spherex_(float *x, float *y, float *z, float *r,
		   float *red, float *green, float *blue,
		   unsigned int *textureid) {
  ns2spherex(*x, *y, *z, *r, *red, *green, *blue, *textureid);
}

void ns2vspherex_(XYZ *P, float *r, COLOUR *col, unsigned int *textureid) {
  ns2vspherex(*P, *r, *col, *textureid);
}

void ns2vplanett_(XYZ *iP, float *ir, COLOUR *icol, char *itexturefn, 
		  float *texture_phase, XYZ *axis, float *rotation,
		  long int textlen) {
  char *ttex = _s2_f2cstr(itexturefn, textlen);
  ns2vplanett(*iP, *ir, *icol, ttex, *texture_phase, *axis, *rotation);
  free(ttex);
}
void ns2vplanetx_(XYZ *iP, float *ir, COLOUR *icol, 
		  unsigned int *textureid, 
		  float *texture_phase, XYZ *axis, float *rotation) {
  ns2vplanetx(*iP, *ir, *icol, *textureid, *texture_phase,
	      *axis, *rotation);
}

void ns2disk_(float *x, float *y, float *z, 
	      float *nx, float *ny, float *nz,
	      float *r1, float *r2,
	      float *red, float *green, float *blue) {
  ns2disk(*x, *y, *z, *nx, *ny, *nz, *r1, *r2, *red, *green, *blue);
}
void ns2vdisk_(XYZ *P, XYZ *N, float *r1, float *r2, COLOUR *col) {
  ns2vdisk(*P, *N, *r1, *r2, *col);
}
void ns2arc_(float *px, float *py, float *pz,
	     float *nx, float *ny, float *nz,
	     float *sx, float *sy, float *sz,
	     float *deg, int *nseg) {
  ns2arc(*px, *py, *pz, *nx, *ny, *nz, *sx, *sy, *sz, *deg, *nseg);
}
void ns2varc_(XYZ *P, XYZ *N, XYZ *S, float *deg, int *nseg) {
  ns2varc(*P, *N, *S, *deg, *nseg);
}

void ns2erc_(float *px, float *py, float *pz,
	     float *nx, float *ny, float *nz,
	     float *sx, float *sy, float *sz,
	     float *deg, int *nseg, float *axratio) {
  ns2erc(*px, *py, *pz, *nx, *ny, *nz, *sx, *sy, *sz, *deg, *nseg, *axratio);
}
void ns2verc_(XYZ *P, XYZ *N, XYZ *S, float *deg, int *nseg,
	      float *axratio) {
  ns2verc(*P, *N, *S, *deg, *nseg, *axratio);
}


void ns2text_(float *x, float *y, float *z,
	      float *rx, float *ry, float *rz,
	      float *ux, float *uy, float *uz,
	      float *red, float *green, float *blue,
	      char *text, long int textlen) {
  char *ttex = _s2_f2cstr(text, textlen);
  ns2text(*x, *y, *z, *rx, *ry, *rz, *ux, *uy, *uz, 
	  *red, *green, *blue, ttex);
  free(ttex);
}
void ns2vtext_(XYZ *P, XYZ *R, XYZ *U, COLOUR *col, char *text,
	       long int textlen) {
  char *ttex = _s2_f2cstr(text, textlen);
  ns2Vtext(*P, *R, *U, *col, ttex);
  free(ttex);
}

void ns2point_(float *x, float *y, float *z,
	       float *red, float *green, float *blue) {
  ns2point(*x, *y, *z, *red, *green, *blue);
}
void ns2vpoint_(XYZ *P, COLOUR *col) {
  ns2vpoint(*P, *col);
}
void ns2vnpoint_(XYZ *P, COLOUR *col, int *n) {
  ns2VNpoint(P, *col, *n);
}

void ns2thpoint_(float *x, float *y, float *z,
		float *red, float *green, float *blue, float *size) {
  ns2thpoint(*x, *y, *z, *red, *green, *blue, *size);
}
void ns2vthpoint_(XYZ *P, COLOUR *col, float *size) {
  ns2vthpoint(*P, *col, *size);
}

void ns2vpa_(XYZ *P, COLOUR *icol, float *isize, char *itrans, float *ialpha) {
  ns2vpa(*P, *icol, *isize, *itrans, *ialpha);
}

void ns2i_(float *x, float *y, float *z,
	  float *red, float *green, float *blue) {
  ns2i(*x, *y, *z, *red, *green, *blue);
}
void ns2vi_(XYZ *P, COLOUR *col) {
  ns2vi(*P, *col);
}

void ns2line_(float *x1, float *y1, float *z1, 
	      float *x2, float *y2, float *z2,
	      float *red, float *green, float *blue) {
  ns2line(*x1, *y1, *z1, *x2, *y2, *z2, *red, *green, *blue);
}
void ns2vline_(XYZ *P1, XYZ *P2, COLOUR *col) {
  ns2vline(*P1, *P2, *col);
}

void ns2thline_(float *x1, float *y1, float *z1, 
		float *x2, float *y2, float *z2,
		float *red, float *green, float *blue, float *width) {
  ns2thline(*x1, *y1, *z1, *x2, *y2, *z2, *red, *green, *blue, *width);
}
void ns2vthline_(XYZ *P1, XYZ *P2, COLOUR *col, float *width) {
  ns2vthline(*P1, *P2, *col, *width);
}

void ns2thwcube_(float *x1, float *y1, float *z1, 
		 float *x2, float *y2, float *z2,
		 float *red, float *green, float *blue,
		 float *width) {
  ns2thwcube(*x1, *y1, *z1, *x2, *y2, *z2, *red, *green, *blue, *width);
}
void ns2vthwcube_(XYZ *iP1, XYZ *iP2, COLOUR *col, float *width) {
  ns2vthwcube(*iP1, *iP2, *col, *width);
}

void ns2cline_(float *x1, float *y1, float *z1, 
	       float *x2, float *y2, float *z2,
	       float *red1, float *green1, float *blue1,
	       float *red2, float *green2, float *blue2) {
  ns2cline(*x1, *y1, *z1, *x2, *y2, *z2, *red1, *green1, *blue1,
	   *red2, *green2, *blue2);
}
void ns2vcline_(XYZ *P1, XYZ *P2, COLOUR *col1, COLOUR *col2) {
  ns2vcline(*P1, *P2, *col1, *col2);
}

void ns2thcline_(float *x1, float *y1, float *z1,
		 float *x2, float *y2, float *z2,
		 float *red1, float *green1, float *blue1,
		 float *red2, float *green2, float *blue2, 
		 float *width) {
  ns2thcline(*x1, *y1, *z1, *x2, *y2, *z2, *red1, *green1, *blue1,
	     *red2, *green2, *blue2, *width);
}
void ns2vthcline_(XYZ *P1, XYZ *P2, COLOUR *col1, COLOUR *col2, float *width) {
  ns2vthcline(*P1, *P2, *col1, *col2, *width);
}

void ns2vf3_(XYZ *P, COLOUR *col) {
  ns2vf3(P, *col);
}
void ns2vf3a_(XYZ *P, COLOUR *col, char *trans, float *alpha) {/*NEW*/
  ns2vf3a(P, *col, *trans, *alpha);
}
void ns2vf3n_(XYZ *P, XYZ *N, COLOUR *col) {
  ns2vf3n(P, N, *col);
}
void ns2vf3c_(XYZ *P, COLOUR *col) {
  ns2vf3c(P, col);
}
void ns2vf3nc_(XYZ *P, XYZ *N, COLOUR *col) {
  ns2vf3nc(P, N, col);
}
void ns2vf3na_(XYZ *P, XYZ *N, COLOUR *col, char *trans, float *alpha) {
  ns2vf3na(P, N, *col, *trans, *alpha);
}

void ns2vf3nca_(XYZ *P, XYZ *N, COLOUR *col, char *trans, float *alpha) {
  ns2vf3nca(P, N, col, *trans, alpha);
}

void ns2vf4_(XYZ *P, COLOUR *col) {
  ns2vf4(P, *col);
}
void ns2vf4n_(XYZ *P, XYZ *N, COLOUR *col) {
  ns2vf4n(P, N, *col);
}
void ns2vf4c_(XYZ *P, COLOUR *col) {
  ns2vf4c(P, col);
}
void ns2vf4nc_(XYZ *P, XYZ *N, COLOUR *col) {
  ns2vf4nc(P, N, col);
}

void ns2vf4t_(XYZ *P, COLOUR *col, char *texturefn, float *scale,
	      char *trans, long int textlen) {
  char *ttex = _s2_f2cstr(texturefn, textlen);
  ns2Vf4t(P, *col, ttex, *scale, *trans);
  free(ttex);
}
void ns2vf4x_(XYZ *P, COLOUR *col, unsigned int *textureid, 
	       float *scale, char *trans) {
  ns2vf4x(P, *col, *textureid, *scale, *trans);
}

void ns2vf4xt_(XYZ *P, COLOUR *col, unsigned int *textureid,
		 float *scale, char *trans, float *ialpha) {
  ns2vf4xt(P, *col, *textureid, *scale, *trans, *ialpha);
}

void ns2scube_(float *x1, float *y1, float *z1,
	       float *x2, float *y2, float *z2,
	       float *red, float *green, float *blue,
	       float *alpha) {
  ns2scube(*x1, *y1, *z1, *x2, *y2, *z2, *red, *green, *blue, *alpha);
}
void ns2vscube_(XYZ *P1, XYZ *P2, COLOUR *col, float *alpha) {
  ns2vscube(*P1, *P2, *col, *alpha);
}


void ns2m_(int *type, float *size, float *x, float *y, float *z,
	   float *red, float *green, float *blue) {
  ns2m(*type, *size, *x, *y, *z, *red, *green, *blue);
}
void ns2vm_(int *type, float *size, XYZ *P, COLOUR *col) {
  ns2vm(*type, *size, *P, *col);
}


/***********************************************************************
 *
 * DYNAMIC-ONLY GEOMETRY FUNCTIONS
 *
 ***********************************************************************
 */

void ds2bb_(float *x, float *y, float *z,
	    float *str_x, float *str_y, float *str_z,
	    float *isize, float *r, float *g, float *b,
	    unsigned int *itexid, float *ialpha, char *itrans) {
  ds2bb(*x, *y, *z, *str_x, *str_y, *str_z, *isize, *r, *g, *b,
	*itexid, *ialpha, *itrans);
}
void ds2vbb_(XYZ *iP, XYZ *iS, float *isize, COLOUR *iC,
	     unsigned int *itexid, float *ialpha, char *itrans) {
  ds2vbb(*iP, *iS, *isize, *iC, *itexid, *ialpha, *itrans);
}
void ds2vbbr_(XYZ *iP, XYZ *iStretch, float *isize, float *ipangle,
	      COLOUR *iC, unsigned int *itexid,
	      float *ialpha, char *itrans) {
  ds2vbbr(*iP, *iStretch, *isize, *ipangle, *iC, *itexid, *ialpha, *itrans);
}
void ds2vbbp_(XYZ *iP, XYZ *offset, float *aspect,
	      float *isize, COLOUR *iC, unsigned int *itexid,
	      float *alpha, char *itrans) {
  ds2vbbp(*iP, *offset, *aspect, *isize, *iC, *itexid, *alpha, *itrans);
}
void ds2vbbpr_(XYZ *iP, XYZ *offset, float *aspect, float *isize,
	       float *ipangle, COLOUR *iC, unsigned int *itexid,
	       float *alpha, char *trans) {
  ds2vbbpr(*iP, *offset, *aspect, *isize, *ipangle, *iC, *itexid,
	   *alpha, *trans);
}

void ds2tb_(float *x, float *y, float *z, float *x_off, float *y_off,
	    char *text, int scaletext, long int textlen) {
  char *ttex = _s2_f2cstr(text, textlen);
  ds2tb(*x, *y, *z, *x_off, *y_off, ttex, scaletext);
  free(ttex);
}
void ds2vtb_(XYZ *iP, XYZ *ioff, char *text, int scaletext, long int textlen) {
  char *ttex = _s2_f2cstr(text, textlen);
  ds2vtb(*iP, *ioff, ttex, scaletext);
  free(ttex);  
}

void ds2protect_(void) {
  ds2protect();
}
void ds2unprotect_(void) {
  ds2unprotect();
}
int ds2isprotected_(void) {
  int result = ds2isprotected();
  return result;
}

/***********************************************************************
 *
 * CALLBACK AND _S2HANDLE SYSTEM
 *
 ***********************************************************************
 */

void cs2scb_(void *icbfn) {
  cs2scb(icbfn);
}
void *cs2qcb_(void) {
  return cs2qcb();
}

void cs2scbx_(void *icbfnx, void *data) {/*NEW*/
  cs2scbx(icbfnx, data);
}
void *cs2qcbx_(void) {
  return cs2qcbx();
}

void cs2ecb_(void) {
  cs2ecb();
}

void cs2dcb_(void) {
  cs2dcb();
}

void cs2tcb_(void) {
  cs2tcb();
}

void cs2skcb_(void *cbfn) {
  cs2skcb(cbfn);
}
void *cs2qkcb_(void) {
  return cs2qkcb();
}
void cs2sncb_(void *cbfn) {
  cs2sncb(cbfn);
}
void *cs2qncb_(void) {
  return cs2qncb();
}

/* [Not implemented] Register function to draw direct OpenGL graphics */
void cs2socb_(void *icbfn) {
  _s2warn("cs2socb", "function not available / useful from FORTRAN");
  return;
}
void *cs2qocb_(void) {
  _s2warn("cs2qocb", "function not available / useful from FORTRAN");
  return NULL;
}

void cs2srcb_(void *remcb) {
  cs2srcb(remcb);
}
void *cs2qrcb_(void) {
  return cs2qrcb();
}

void ds2ah_(XYZ *ip, float *isize, COLOUR *icol, COLOUR *ihilite, int *iid,
	       int *iselected) {
  ds2ah(*ip, *isize, *icol, *ihilite, *iid, *iselected);
}
void ds2ahx_(XYZ *ip, float *isize, int *itex, int *ihitex,/*NEW*/
             COLOUR *icol, COLOUR *ihilite,
             int *iid, int *iselected) {
  ds2ahx(*ip, *isize, *itex, *ihitex, *icol, *ihilite, *iid, *iselected);
}

void cs2shcb_(void *hcbfn) {
  cs2shcb(hcbfn);
}
void *cs2qhcb_(void) {
  return cs2qhcb();
}
void cs2sdhcb_(void *dhcbfn) {/*NEW*/
  cs2sdhcb(dhcbfn);
}
void *cs2qdhcb_(void) {
  return cs2qdhcb();
}
void cs2spcb_(void *pcbfn, void *data) {/*NEW*/
  cs2spcb(pcbfn, data);
}
void *cs2qpcb_(void) {
  return cs2qpcb();
}
void cs2sptxy_(char *prompt, float *xfrac, float *yfrac,
	       unsigned int promptlen) {
  char *pp = _s2_f2cstr(prompt, promptlen);
  cs2sptxy(pp, *xfrac, *yfrac);
  free(pp);
}

void cs2th_(int *iid) {
  cs2th(*iid);
}

int cs2qhv_() {
  int result = cs2qhv();
  return result;
}

void cs2thv_(int *enabledisable) {
  cs2thv(*enabledisable);
}

/***********************************************************************
 *
 * PANELS
 *
 ***********************************************************************
 */

int xs2ap_(float *x1, float *y1, float *x2, float *y2) {/*NEW*/
  return xs2ap(*x1, *y1, *x2, *y2);
}
void xs2tp_(int *panelid) {/*NEW*/
  xs2tp(*panelid);
}
void xs2cp_(int *panelid) {/*NEW*/
  xs2cp(*panelid);
}
void xs2mp_(int *panelid, float *x1, float *y1, float *x2, float *y2) {/*NEW*/
  xs2mp(*panelid, *x1, *y1, *x2, *y2);
}
void xs2lpc_(int *masterid, int *slaveid) {/*NEW*/
  xs2lpc(*masterid, *slaveid);
}
int xs2qpa_(int *panelid) {/*NEW*/
  return xs2qpa(*panelid);
}
int xs2qcpa_() {/*NEW*/
  return xs2qcpa();
}
int xs2qsp_() {/*NEW*/
  return xs2qsp();
}
void xs2spp_(COLOUR *active, COLOUR *inactive, float *width) {
  xs2spp(*active, *inactive, *width);
}
void ss2qsd_(int *x, int *y) {/*NEW*/
  ss2qsd(x, y);
}

/***********************************************************************
 *
 * ADVANCED TEXTURE / COLORMAP HANDLING
 *
 ***********************************************************************
 */

unsigned int ss2lt_(char *itexturefn, long int textlen) {
  char *ttex = _s2_f2cstr(itexturefn, textlen);
  unsigned int result = ss2lt(ttex);
  free(ttex);
  return result;
}

unsigned char *ss2gt_(unsigned int *iid, int *width, int *height) {
  _s2warn("ss2gt", "function not available from FORTRAN");
  return NULL;
}

void ss2pt_(unsigned int *iid) {
  _s2warn("ss2pt", "function not available from FORTRAN");
  return;
}
void ss2ptt_(unsigned int *iid) {
  _s2warn("ss2ptt", "function not available from FORTRAN");
  return;
}

int ss2lcm_(char *imapfile, int *startidx, int *maxn,
	       long int textlen) {
  char *ttex = _s2_f2cstr(imapfile, textlen);
  int result = ss2lcm(ttex, *startidx, *maxn);
  free(ttex);
  return result;
}
int s2loadmap_(char *imapfile, int *startidx, int *maxn,
	       long int textlen) {
  char *ttex = _s2_f2cstr(imapfile, textlen);
  int result = s2loadmap(ttex, *startidx, *maxn);
  free(ttex);
  return result;
}

unsigned int ss2ltt_(char *cmd, float *aspect, long int textlen) {
  char *ttex = _s2_f2cstr(cmd, textlen);
  unsigned int result = ss2ltt(ttex, aspect);
  free(ttex);
  return result;
}

#if defined(S2FREETYPE)
/* create a texture using the freetype font engine */
unsigned int ss2ftt_(char *fontfilename, char *text, int *fontsizepx,
		     int *border, long int fnlen, long int textlen) {
  char *tfont = _s2_f2cstr(fontfilename, fnlen);
  char *ttext = _s2_f2cstr(text, textlen);
  unsigned int result = ss2ftt(tfont, ttext, *fontsizepx, *border);
  free(ttext);
  free(tfont);
}
#endif


unsigned int ss2ct_(int *width, int *height) {/*NEW*/
  _s2warn("ss2ct", "function not available from FORTRAN");
  return 0;
}
unsigned int ss2ctt_(int *width, int *height) {/*NEW*/
  _s2warn("ss2ctt", "function not available from FORTRAN");
  return 0;
}
void ss2dt_(unsigned int *texid) {/*NEW*/
  ss2dt(*texid);
}

/***********************************************************************
 *
 * ENVIRONMENT / RENDERING ATTRIBUTES
 *
 ***********************************************************************
 */

void ss2ssr_(int *res) {
  ss2ssr(*res);
}
int ss2qsr_() {/*NEW*/
  return ss2qsr();
}

void ss2srm_(int *mode) {
  ss2srm(*mode);
}
int ss2qrm_() {
  return ss2qrm();
}

void ss2sl_(COLOUR *ambient, int *nlights, XYZ *lightpos,
	    COLOUR *lightcol, int *worldcoords) {
  ss2sl(*ambient, *nlights, lightpos, lightcol, *worldcoords);
}

void ss2sbc_(float *r, float *g, float *b) {
  ss2sbc(*r, *g, *b);
}
void ss2qbc_(float *r, float *g, float *b) {/*NEW*/
  ss2qbc(r, g, b);
}

void ss2sfc_(float *r, float *g, float *b) {/*NEW*/
  ss2sfc(*r, *g, *b);
}
void ss2qfc_(float *r, float *g, float *b) {/*NEW*/
  ss2qfc(r, g, b);
}

void ss2sfra_(float *rot) {
  ss2sfra(*rot);
}
float ss2qfra_() {/*NEW*/
  return ss2qfra();
}

void ss2spt_(int *projtype) {
  ss2spt(*projtype);
}
int ss2qpt_(void) {
  return ss2qpt();
}

void ss2qsa_(int *stereo, int *fullscreen, int *dome) {/*NEW*/
  ss2qsa(stereo, fullscreen, dome);
}
float ss2qar_() {/*NEW*/
  return ss2qar();
}
void ss2snfe_(float *expand) {/*NEW*/
  ss2snfe(*expand);
}
void ss2qnfp_(double *near, double *far) {
  ss2qnfp(near, far);
}

/***********************************************************************
 *
 * ADVANCED CAMERA CONTROL
 *
 ***********************************************************************
 */

void ss2sc_(XYZ *position, XYZ *up, XYZ *vdir, int *worldcoords) {
  ss2sc(*position, *up, *vdir, *worldcoords);
}

int ss2qc_(XYZ *position, XYZ *up, XYZ *vdir, int *worldcoords) {
  return ss2qc(position, up, vdir, *worldcoords);
}

void ss2sas_(int *startstop) {
  ss2sas(*startstop);
}

int ss2qas_() {/*NEW*/
  return ss2qas();
}

void ss2scf_(XYZ *position, int *worldcoords) {
  ss2scf(*position, *worldcoords);
}

void ss2ucf_(void) {
  ss2ucf();
}

void ss2sca_(float *aperture) {
  ss2sca(*aperture);
}
float ss2qca_() {
  return ss2qca();
}

void ss2qcf_(int *set, XYZ *position, int *worldcoords) {/*NEW*/
  ss2qcf(set, position, *worldcoords);
}

void ss2qpr_(XYZ *position, int *worldcoords) {
  ss2qpr(position, *worldcoords);
}

void ss2sss_(float *spd) {
  ss2sss(*spd);
}
float ss2qss_(void) {
  return ss2qss();
}

void ss2scaf_(XYZ *position, XYZ *up, XYZ *vdir, XYZ *focus, int *worldcoords) {
  ss2scaf(*position, *up, *vdir, *focus, *worldcoords);
}


void ss2scs_(float *spd) {
  ss2scs(*spd);
}
float ss2qcs_(void) {
  return ss2qcs();
}


void ss2tc_(int *enabledisable) {
  ss2tc(*enabledisable);
}

float ss2qess_() {/*NEW*/
  return ss2qess();
}
void ss2sess_(float *ieyesep) {/*NEW*/
  ss2sess(*ieyesep);
}

/***********************************************************************
 *
 * IMAGE WRITING/FETCHING
 *
 ***********************************************************************
 */

void ss2wtga_(char *fname, long int textlen) {/*NEW*/
  char *string = _s2_f2cstr(fname, textlen);
  ss2wtga(string);
  free(string);
}

unsigned char *ss2gpix_(unsigned int *width, unsigned int *height) {
  return ss2gpix(width, height);
}

/***********************************************************************
 *
 * MISCELLANEOUS PLOTS
 *
 ***********************************************************************
 */

void s2chromapts_(int *n, float *ilong, float *lat, float *dist,
		  float *size, float *radius, float *dmin, float *dmax) {
  s2chromapts(*n, ilong, lat, dist, size, *radius, *dmin, *dmax);
}
void s2chromacpts_(int *n, float *ix, float *iy, float *iz, float *dist,
		  float *size, float *dmin, float *dmax) {
  s2chromacpts(*n, ix, iy, iz, dist, size, *dmin, *dmax);
}

void s2vect3_(float *a, float *b, float *c, int *adim, int *bdim,
	      int *cdim, int *a1, int *a2, int *b1, int *b2,
	      int *c1, int *c2, float *scale, int *nc, float *tr,
	      float *minlength, int *colbylength, float *minl,
	      float *maxl) {
  float ***c3a = _s2_f2c3darr(a, *adim, *bdim, *cdim);
  float ***c3b = _s2_f2c3darr(b, *adim, *bdim, *cdim);
  float ***c3c = _s2_f2c3darr(c, *adim, *bdim, *cdim);
  s2vect3(c3a, c3b, c3c, *adim, *bdim, *cdim, *a1, *a2, *b1, *b2,
	  *c1, *c2, *scale, *nc, tr, *minlength, *colbylength,
	  *minl, *maxl);
  _s2_free3darr(c3c, *adim, *bdim);
  _s2_free3darr(c3b, *adim, *bdim);
  _s2_free3darr(c3a, *adim, *bdim);
}

void s2skypa_(float **data, int *nx, int *ny,
	      int *i1, int *i2, int *j1, int *j2,
	      float *datamin, float *datamax,
	      float *tr, int *walls, int *idx_left, int *idx_front) {
  s2skypa(data, *nx, *ny, *i1, *i2, *j1, *j2,
	  *datamin, *datamax, tr, *walls, *idx_left, *idx_front);
}

void s2impa_(float **data, int *nx, int *ny,
	     int *i1, int *i2, int *j1, int *j2,
	     float *datamin, float *datamax,
	     float *tr, int *trunk, int *symbol) {
  s2impa(data, *nx, *ny, *i1, *i2, *j1, *j2,
	 *datamin, *datamax, tr, *trunk, *symbol);
}

/***********************************************************************
 *
 * MISCELLANEOUS ROUTINES
 *
 ***********************************************************************
 */

void s2iden_(char *textra, long int textlen) {
  char *ttex = _s2_f2cstr(textra, textlen);
  s2iden(ttex);
  free(ttex);
}

void pushVRMLname_(char *iname, long int textlen) {/*NEW*/
  char *string = _s2_f2cstr(iname, textlen);
  pushVRMLname(string);
  free(string);
}

