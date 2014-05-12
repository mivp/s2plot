/* s2plot.c
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
 * $Id: s2plot.c 5828 2012-11-15 00:13:35Z dbarnes $
 *
 */

#include "s2globals.h"
#include "s2plot.h"
#include "s2plot_internal.h"
#include "s2privfn.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* GLOBAL DECLARATIONS AND DEFINITIONS
 * 
 * Generally specific to user-level functions; for draw-level global variables
 * see geomviewer.c.
 *
 */

/* Options - legacy from geomviewer */
OPTIONS options;

float _s2devicemin[3]; /* normalised device coord system lower bounds */
float _s2devicemax[3]; /* normalised device coord system upper bounds */
float _s2axismin[3]; /* world coord system axis minima */
float _s2axismax[3]; /* world coord system axis maxima */
float _s2axissgn[3]; /* axis sign - ie. +1 if max > min, -1 otherwise */
  
XYZ pmin={-1.0,-1.0,-1.0},pmax={1.0,1.0,1.0},pmid={0.0,0.0,0.0};
double rangemin=1e32,rangemax=-1e32;

/* Geometry Structure */
int nball    = 0; BALL    *ball    = NULL;
int nballt   = 0; BALLT   *ballt   = NULL;
int ndisk    = 0; DISK    *disk    = NULL;
int ncone    = 0; CONE    *cone    = NULL;
int ndot     = 0; DOT     *dot     = NULL;
int nline    = 0; LINE    *line    = NULL;
int nface3   = 0; FACE3   *face3   = NULL;
int nface4   = 0; FACE4   *face4   = NULL;
int nface4t  = 0; FACE4T  *face4t  = NULL;
int nlabel   = 0; LABEL   *label   = NULL;
#if defined(BUILDING_S2PLOT)
int nhandle  = 0; _S2HANDLE  *handle  = NULL;
int nbboard  = 0; _S2BBOARD  *bboard  = NULL;
int nbbset   = 0; _S2BBSET   *bbset   = NULL;
int nface3a  = 0; _S2FACE3A  *face3a  = NULL;
int ntrdot   = 0; TRDOT   *trdot   = NULL;
#if defined(S2_3D_TEXTURES)
int ntexpoly3d = 0; _S2TEXPOLY3D *texpoly3d = NULL;
#endif
int ntexmesh = 0; _S2TEXTUREDMESH *texmesh = NULL;
#endif

#if defined(BUILDING_S2PLOT)
/* s2 changes: add lists for static geometry */
int nball_s    = 0; BALL    *ball_s    = NULL;
int nballt_s   = 0; BALLT   *ballt_s   = NULL;
int ndisk_s    = 0; DISK    *disk_s    = NULL;
int ncone_s    = 0; CONE    *cone_s    = NULL;
int ndot_s     = 0; DOT     *dot_s     = NULL;
int nline_s    = 0; LINE    *line_s    = NULL;
int nface3_s   = 0; FACE3   *face3_s   = NULL;
int nface4_s   = 0; FACE4   *face4_s   = NULL;
int nface4t_s  = 0; FACE4T  *face4t_s  = NULL;
int nlabel_s   = 0; LABEL   *label_s   = NULL;
int nhandle_s  = 0; _S2HANDLE  *handle_s  = NULL;
int nbboard_s  = 0; _S2BBOARD  *bboard_s  = NULL;
int nbbset_s   = 0; _S2BBSET   *bbset_s   = NULL;
int nface3a_s  = 0; _S2FACE3A  *face3a_s  = NULL;
int ntrdot_s   = 0; TRDOT   *trdot_s   = NULL;
#if defined(S2_3D_TEXTURES)
int ntexpoly3d_s = 0; _S2TEXPOLY3D *texpoly3d_s = NULL;
#endif
int ntexmesh_s = 0; _S2TEXTUREDMESH *texmesh_s = NULL;

/* ... and for dynamic geometry */
int nball_d    = 0; BALL    *ball_d    = NULL;
int nballt_d   = 0; BALLT   *ballt_d   = NULL;
int ndisk_d    = 0; DISK    *disk_d    = NULL;
int ncone_d    = 0; CONE    *cone_d    = NULL;
int ndot_d     = 0; DOT     *dot_d     = NULL;
int nline_d    = 0; LINE    *line_d    = NULL;
int nface3_d   = 0; FACE3   *face3_d   = NULL;
int nface4_d   = 0; FACE4   *face4_d   = NULL;
int nface4t_d  = 0; FACE4T  *face4t_d  = NULL;
int nlabel_d   = 0; LABEL   *label_d   = NULL;
int nhandle_d  = 0; _S2HANDLE  *handle_d  = NULL;
int nbboard_d  = 0; _S2BBOARD  *bboard_d  = NULL;
int nbbset_d   = 0; _S2BBSET   *bbset_d   = NULL;
int nface3a_d  = 0; _S2FACE3A  *face3a_d  = NULL;
int ntrdot_d   = 0; TRDOT   *trdot_d   = NULL;
#if defined(S2_3D_TEXTURES)
int ntexpoly3d_d = 0; _S2TEXPOLY3D *texpoly3d_d = NULL;
#endif
int ntexmesh_d = 0; _S2TEXTUREDMESH *texmesh_d = NULL;

#endif

/* camera */
CAMERA camera;

/* lights */
int nlight = -1;
ALIGHT lights[MAXLIGHT];
float globalambient[4] = {0.3,0.3,0.3,1.0}; // wasGL
int defaultlights = 2;
#if defined(BUILDING_S2PLOT)
XYZ deflightpos[8] = {{2,2,2}, {0,0,0}, {0,2,2}, {2,0,0}, 
                      {0,0,2}, {2,2,0}, {2,0,2}, {0,2,0}};
#else
XYZ deflightpos[8] = {{1,1,1}, {0,0,0}, {0,1,1}, {1,0,0}, 
                      {0,0,1}, {1,1,0}, {1,0,1}, {0,1,0}};
#endif




/***********************************************************************
 *
 * _s2priv and other support functions
 *
 ***********************************************************************
 */

#include <stdarg.h>
#if !defined(S2CUSTOMWARNINGS)
void _s2debug(char *fn, char *messg, ...) {
  char *tstr;
  va_list ap;
  va_start(ap,messg);
  if (options.debug) {
    vasprintf(&tstr, messg, ap);
    fprintf(stderr, "S2PLOT DEBUG ... <%s>: %s\n", fn, tstr);
    free(tstr);
  }
}
void _s2warn(char *fn, char *messg, ...) {
  char *tstr;
  va_list ap;
  va_start(ap,messg);
  vasprintf(&tstr, messg, ap);
  fprintf(stderr, "S2PLOT WARNING ... <%s>: %s\n", fn, tstr);
  free(tstr);
}
void _s2error(char *fn, char *messg, ...) {
  char *tstr;
  va_list ap;
  va_start(ap,messg);
  vasprintf(&tstr, messg, ap);
  fprintf(stderr, "S2PLOT ERROR ... <%s>: %s\n", fn, tstr);
  free(tstr);
  //CleanExit();
  exit(-1);
}
void _s2warnk(char key, char *messg, ...) {
  char *tstr;
  va_list ap;
  va_start(ap, messg);
  vasprintf(&tstr, messg, ap);
  fprintf(stderr, "S2PLOT WARNING ... keypress '%c': %s\n", key, tstr);
  free(tstr);
}
#endif

int face3a_comp(const void *a, const void *b) {
  if (((_S2FACE3A *)a)->dist < ((_S2FACE3A *)b)->dist) {
    return -1;
  } else if (((_S2FACE3A *)a)->dist > ((_S2FACE3A *)b)->dist) {
    return +1;
  } else {
    return 0;
  }
}


/* copy S2PLOT world coordinate space bounds to stereo2 bounds */
void _s2priv_setBounds() {
  /* update s2 data */
  int i;
  float maxsc = -9e30;
  for (i = 0; i < 3; i++) {
    /* change: store sign AND world scaling of this axis.  The world
     * scaling is used to ensure characters etc. are square in
     * labelling, etc. 
     */

    /* for future: if viewports are allowed, which essentially means that
     * _s2devicemin and _s2devicemax can be set to be a non-square, then
     * this will need to be accounted for here.
     */
    //_s2axissgn[i] = (_s2axismax[i] - _s2axismin[i]);
    /* and here is the fix for non-square viewports  I hope */
    _s2axissgn[i] = (_s2axismax[i] - _s2axismin[i]) /
      (_s2devicemax[i] - _s2devicemin[i]);
    if (fabsf(_s2axissgn[i]) > maxsc) {
      maxsc = fabsf(_s2axissgn[i]);
    }
  }
  for (i = 0; i < 3; i++) {
    _s2axissgn[i] /= maxsc;
  }
}

#if defined(IGNORE_S2COMMON_C)
/*
	Add a line segment to the line database
*/
void AddLine2Database(XYZ p1,XYZ p2,COLOUR c1,COLOUR c2,double w) {
  if ((line = (LINE *)realloc(line,(nline+1)*sizeof(LINE))) == NULL) {
    _s2error("(internal)", "memory allocation for line failed");
  }
  w = ABS(w);
  line[nline].width = w;
  line[nline].p[0] = p1;
  line[nline].p[1] = p2;
  line[nline].colour[0] = c1;
  line[nline].colour[1] = c2;
#if defined(BUILDING_S2PLOT)
  strcpy(line[nline].whichscreen, _s2_whichscreen);
  strncpy(line[nline].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
  line[nline].VRMLname[MAXVRMLLEN-1] = '\0';
  line[nline].stipple_factor = 0;
  line[nline].stipple_pattern = 0;
#endif
  nline++;
}

/*
	Add a simple face to the database
	Scale and then translate it
*/
void AddFace2Database(XYZ *p,int n,COLOUR c,double scale,XYZ shift) {
  int i;
  
  if (n == 3) {
    if ((face3 = (FACE3 *)realloc(face3,(nface3+1)*sizeof(FACE3))) == NULL) {
      _s2error("(internal)", "memory allocation failed for face3");
    }
    for (i=0;i<3;i++) {
      face3[nface3].p[i].x = shift.x + scale*p[i].x;
      face3[nface3].p[i].y = shift.y + scale*p[i].y;
      face3[nface3].p[i].z = shift.z + scale*p[i].z;
      face3[nface3].n[i] = CalcNormal(p[0],p[1],p[2]);
      face3[nface3].colour[i] = c;
#if defined(BUILDING_S2PLOT)
      strcpy(face3[nface3].whichscreen, _s2_whichscreen);
      strncpy(face3[nface3].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
      face3[nface3].VRMLname[MAXVRMLLEN-1] = '\0';
#endif
    }
    nface3++;
  } else if (n == 4) {
    if ((face4 = (FACE4 *)realloc(face4,(nface4+1)*sizeof(FACE4))) == NULL) {
      _s2error("(internal)", "memory allocation failed for face4");
    }
    for (i=0;i<4;i++) {
      face4[nface4].p[i].x = shift.x + scale*p[i].x;
      face4[nface4].p[i].y = shift.y + scale*p[i].y;
      face4[nface4].p[i].z = shift.z + scale*p[i].z;
      face4[nface4].n[i] = CalcNormal(p[(i-1+4)%4],p[i],p[(i+1)%4]);
      face4[nface4].colour[i] = c;
#if defined(BUILDING_S2PLOT)
      strcpy(face4[nface4].whichscreen, _s2_whichscreen);
      strncpy(face4[nface4].VRMLname, _s2_VRMLnames[_s2_currVRMLidx],
	      MAXVRMLLEN);
      face4[nface4].VRMLname[MAXVRMLLEN-1] = '\0';
#endif
    }
    nface4++;
  }
}
#endif

/*
	Add a marker to the database
*/
void AddMarker2Database(int type,double size,XYZ p,COLOUR c)
{
	XYZ q[4];
	XYZ tetra[4] = {{ 1, 1, 1},  {-1,-1, 1},   {1,-1,-1},  {-1, 1,-1}};
	XYZ box[8]   = {{-1,-1, 1},  {1,-1, 1},    {1,-1,-1},  {-1,-1,-1},
					    {-1, 1, 1},  {1, 1, 1},    {1, 1,-1},  {-1, 1,-1}};
	XYZ octa[6]  = {{-1,0,1},  {1,0,1},  {1,0,-1},  {-1,0,-1},  
				       {0,-1.41421356,0},  {0,1.41421356,0}};

   switch (type) {
   case 0:
		q[0] = tetra[0]; q[1] = tetra[1]; q[2] = tetra[2]; 
		AddFace2Database(q,3,c,size/2,p);
      q[0] = tetra[1]; q[1] = tetra[3]; q[2] = tetra[2]; 
      AddFace2Database(q,3,c,size/2,p);
      q[0] = tetra[0]; q[1] = tetra[2]; q[2] = tetra[3]; 
      AddFace2Database(q,3,c,size/2,p);
      q[0] = tetra[0]; q[1] = tetra[3]; q[2] = tetra[1]; 
      AddFace2Database(q,3,c,size/2,p);
      break;
   case 1:
		q[0] = p; q[1] = p;
		q[0].x -= size/2; q[1].x += size/2;
      AddLine2Database(q[0],q[1],c,c,1.0);
      q[0] = p; q[1] = p; 
      q[0].y -= size/2; q[1].y += size/2;
      AddLine2Database(q[0],q[1],c,c,1.0);
      q[0] = p; q[1] = p;  
      q[0].z -= size/2; q[1].z += size/2;
      AddLine2Database(q[0],q[1],c,c,1.0);
      break;
   case 2:
      q[0] = box[0]; q[1] = box[1]; q[2] = box[2]; q[3] = box[3];    /* Bottom */
      AddFace2Database(q,4,c,size/2,p);
      q[0] = box[4]; q[1] = box[7]; q[2] = box[6]; q[3] = box[5];    /* Top */
      AddFace2Database(q,4,c,size/2,p);
      q[0] = box[0]; q[1] = box[4]; q[2] = box[5]; q[3] = box[1];    /* Front */
      AddFace2Database(q,4,c,size/2,p);
      q[0] = box[3]; q[1] = box[2]; q[2] = box[6]; q[3] = box[7];    /* Back */
      AddFace2Database(q,4,c,size/2,p);
      q[0] = box[1]; q[1] = box[5]; q[2] = box[6]; q[3] = box[2];    /* Left */
      AddFace2Database(q,4,c,size/2,p);
      q[0] = box[0]; q[1] = box[3]; q[2] = box[7]; q[3] = box[4];    /* Right */
      AddFace2Database(q,4,c,size/2,p);
      break;
	case 3:
      q[0] = octa[0]; q[1] = octa[5]; q[2] = octa[1];    /* Top half */
      AddFace2Database(q,3,c,size/2,p);
      q[0] = octa[1]; q[1] = octa[5]; q[2] = octa[2];
      AddFace2Database(q,3,c,size/2,p);
      q[0] = octa[2]; q[1] = octa[5]; q[2] = octa[3];
      AddFace2Database(q,3,c,size/2,p);
      q[0] = octa[3]; q[1] = octa[5]; q[2] = octa[0];
      AddFace2Database(q,3,c,size/2,p);
      q[0] = octa[0]; q[1] = octa[4]; q[2] = octa[1];    /* Bottom half */
      AddFace2Database(q,3,c,size/2,p);
      q[0] = octa[1]; q[1] = octa[4]; q[2] = octa[2];
      AddFace2Database(q,3,c,size/2,p);
      q[0] = octa[2]; q[1] = octa[4]; q[2] = octa[3];
      AddFace2Database(q,3,c,size/2,p);
      q[0] = octa[3]; q[1] = octa[4]; q[2] = octa[0];
      AddFace2Database(q,3,c,size/2,p);
		break;
   }
}

DISK *_s2priv_adddisks(int in) {
  DISK *disk_base;
  if (!disk) {
    disk = (DISK *)calloc(in, sizeof(DISK));
    disk_base = disk;
    ndisk = in;
  } else {
    disk = (DISK *)realloc(disk, (ndisk + in) * sizeof(DISK));
    disk_base = disk + ndisk;
    ndisk += in;
  }
  if (!disk) {
    ndisk = 0;
    return NULL;
  }
  return disk_base;
}

BALL *_s2priv_addballs(int in) {
  BALL *ball_base;
  if (!ball) {
    ball = (BALL *)calloc(in, sizeof(BALL));
    ball_base = ball;
    nball = in;
  } else {
    ball = (BALL *)realloc(ball, (nball + in) * sizeof(BALL));
    ball_base = ball + nball;
    nball += in;
  }
  if (!ball) {
    nball = 0;
    return NULL;
  }
  return ball_base;
}

LABEL *_s2priv_addlabels(int in) {
  LABEL *label_base;
  if (!label) {
    label = (LABEL *)calloc(in, sizeof(LABEL));
    label_base = label;
    nlabel = in;
  } else {
    label = (LABEL *)realloc(label, (nlabel + in) * sizeof(LABEL));
    label_base = label + nlabel;
    nlabel += in;
  }
  if (!label) {
    nlabel = 0;
    return NULL;
  }
  return label_base;
}

_S2HANDLE *_s2priv_addhandles(int in) {
  _S2HANDLE *handle_base;
  if (!handle) {
    handle = (_S2HANDLE *)calloc(in, sizeof(_S2HANDLE));
    handle_base = handle;
    nhandle = in;
  } else {
    handle = (_S2HANDLE *)realloc(handle, (nhandle + in) * sizeof(_S2HANDLE));
    handle_base = handle + nhandle;
    nhandle += in;
  }
  if (!handle) {
    nhandle = 0;
    return NULL;
  }
  return handle_base;
}

_S2BBOARD *_s2priv_addbboards(int in) {
  _S2BBOARD *bboard_base;
  if (!bboard) {
    bboard = (_S2BBOARD *)calloc(in, sizeof(_S2BBOARD));
    bboard_base = bboard;
    nbboard = in;
  } else {
    bboard = (_S2BBOARD *)realloc(bboard, (nbboard + in) * sizeof(_S2BBOARD));
    bboard_base = bboard + nbboard;
    nbboard += in;
  }
  if (!bboard) {
    nbboard = 0;
    return NULL;
  }
  return bboard_base;
}
  
_S2BBSET *_s2priv_addbbset(int in) {
  _S2BBSET *bbset_base;
  if (!bbset) {
    bbset = (_S2BBSET *)calloc(in, sizeof(_S2BBSET));
    bbset_base = bbset;
    nbbset = in;
  } else {
    bbset = (_S2BBSET *)realloc(bbset, (nbbset + in) * sizeof(_S2BBSET));
    bbset_base = bbset + nbbset;
    nbbset += in;
  }
  if (!bbset) {
    nbbset = 0;
    return NULL;
  }
  return bbset_base;
}

CONE *_s2priv_addcones(int in) {
  CONE *cone_base;
  if (!cone) {
    cone = (CONE *)calloc(in, sizeof(CONE));
    cone_base = cone;
    ncone = in;
  } else {
    cone = (CONE *)realloc(cone, (ncone + in) * sizeof(CONE));
    cone_base = cone + ncone;
    ncone += in;
  }
  if (!cone) {
    ncone = 0;
    return NULL;
  }
  return cone_base;
}

#if (1)
DOT *_s2priv_adddots(int in) {
  DOT *dot_base;
  if (!dot) {
    dot = (DOT *)calloc(in, sizeof(DOT));
    dot_base = dot;
    ndot = in;
  } else {
    dot = (DOT *)realloc(dot, (ndot + in) * sizeof(DOT));
    dot_base = dot + ndot;
    ndot += in;
  }
  if (!dot) {
    ndot = 0;
    return NULL;
  }
  return dot_base;
}
#endif

TRDOT *_s2priv_addtrdots(int in) {
  TRDOT *trdot_base;
  if (!trdot) {
    trdot = (TRDOT *)calloc(in, sizeof(TRDOT));
    trdot_base = trdot;
    ntrdot = in;
  } else {
    trdot = (TRDOT *)realloc(trdot, (ntrdot + in) * sizeof(TRDOT));
    trdot_base = trdot + ntrdot;
    ntrdot += in;
  }
  if (!trdot) {
    ntrdot = 0;
    return NULL;
  }
  return trdot_base;
}

#if (1)
LINE *_s2priv_addlines(int in) {
  LINE *line_base;
  if (!line) {
    line = (LINE *)malloc(in * sizeof(LINE));
    line_base = line;
    nline = in;
  } else {
    line = (LINE *)realloc(line, (nline + in) * sizeof(LINE));
    line_base = line + nline;
    nline += in;
  }
  if (!line) {
    nline = 0;
    return NULL;
  }
  return line_base;
}
#endif

FACE3 *_s2priv_addface3s(int in) {
  FACE3 *face3_base;
  if (!face3) {
    face3 = (FACE3 *)calloc(in, sizeof(FACE3));
    face3_base = face3;
    nface3 = in;
  } else {
    face3 = (FACE3 *)realloc(face3, (nface3 + in) * sizeof(FACE3));
    face3_base = face3 + nface3;
    nface3 += in;
  }
  if (!face3) {
    nface3 = 0;
    return NULL;
  }
  return face3_base;
}

_S2FACE3A *_s2priv_addface3as(int in) {
  _S2FACE3A *face3a_base;
  if (!face3a) {
    face3a = (_S2FACE3A *)calloc(in, sizeof(_S2FACE3A));
    face3a_base = face3a;
    nface3a = in;
  } else {
    face3a = (_S2FACE3A *)realloc(face3a, (nface3a + in) * sizeof(_S2FACE3A));
    face3a_base = face3a + nface3a;
    nface3a += in;
  }
  if (!face3a) {
    nface3a = 0;
    return NULL;
  }
  return face3a_base;
}

#if defined(S2_3D_TEXTURES)
_S2TEXPOLY3D *_s2priv_addtexpoly3ds(int in) {
  _S2TEXPOLY3D *texpoly3d_base;
  if (!texpoly3d) {
    texpoly3d = (_S2TEXPOLY3D *)calloc(in, sizeof(_S2TEXPOLY3D));
    texpoly3d_base = texpoly3d;
    ntexpoly3d = in;
  } else {
    texpoly3d = (_S2TEXPOLY3D *)realloc(texpoly3d, (ntexpoly3d + in) * sizeof(_S2TEXPOLY3D));
    texpoly3d_base = texpoly3d + ntexpoly3d;
    ntexpoly3d += in;
  }
  if (!texpoly3d) {
    ntexpoly3d = 0;
    return NULL;
  }
  int i;
  for (i = 0; i < in; i++) {
    texpoly3d_base[i].nverts = 0;
    texpoly3d_base[i].verts = NULL;
    texpoly3d_base[i].texcoords = NULL;
  }
  return texpoly3d_base;
}
#endif

_S2TEXTUREDMESH *_s2priv_addtexturedmesh(int in) {
  _S2TEXTUREDMESH *texmesh_base;
  if (!texmesh) {
    texmesh = (_S2TEXTUREDMESH *)calloc(in, sizeof(_S2TEXTUREDMESH));
    texmesh_base = texmesh;
    ntexmesh = in;
  } else {
    texmesh = (_S2TEXTUREDMESH *)realloc(texmesh, (ntexmesh + in) * 
					 sizeof(_S2TEXTUREDMESH));
    texmesh_base = texmesh + ntexmesh;
    ntexmesh += in;
  }
  if (!texmesh) {
    ntexmesh = 0;
    return NULL;
  }
  int i;
  for (i = 0; i < in; i++) {
    texmesh_base[i].nverts = 0;
    texmesh_base[i].verts = NULL;
    texmesh_base[i].nnorms = 0;
    texmesh_base[i].norms = NULL;
    texmesh_base[i].nvtcs = 0;
    texmesh_base[i].vtcs = NULL;
    texmesh_base[i].nfacets = 0;
    texmesh_base[i].facets = NULL;
    texmesh_base[i].facets_vtcs = NULL;
    texmesh_base[i].texid = 0;
    texmesh_base[i].trans = 'o';
    texmesh_base[i].alpha = 1.0;
  }
  return texmesh_base;
}

FACE4 *_s2priv_addface4s(int in) {
  FACE4 *face4_base;
  if (!face4) {
    face4 = (FACE4 *)calloc(in, sizeof(FACE4));
    face4_base = face4;
    nface4 = in;
  } else {
    face4 = (FACE4 *)realloc(face4, (nface4 + in) * sizeof(FACE4));
    face4_base = face4 + nface4;
    nface4 += in;
  }
  if (!face4) {
    nface4 = 0;
    return NULL;
  }
  return face4_base;
}

FACE4T *_s2priv_addface4ts(int in) {
  FACE4T *face4t_base;
  if (!face4t) {
    face4t = (FACE4T *)calloc(in, sizeof(FACE4T));
    face4t_base = face4t;
    nface4t = in;
  } else {
    face4t = (FACE4T *)realloc(face4t, (nface4t + in) * sizeof(FACE4T));
    face4t_base = face4t + nface4t;
    nface4t += in;
  }
  if (!face4t) {
    nface4t = 0;
    return NULL;
  }
  return face4t_base;
}

/* draw one quad in current color, not transparent */
void _s2priv_quad(float *xpts, float *ypts, float *zpts, float *normal) {
  if (!xpts || !ypts || !zpts || !normal) {
    return;
  }
  FACE4 *face4_base = _s2priv_addface4s(1);
  if (!face4_base) {
    _s2error("(internal)", "memory allocation failed for face4");
  }

  int i;
  for (i = 0; i < 4; i++) {
    /* version with scaling */
    face4_base[0].p[i].x = _S2WORLD2DEVICE(xpts[i], _S2XAX);
    face4_base[0].p[i].y = _S2WORLD2DEVICE(ypts[i], _S2YAX);
    face4_base[0].p[i].z = _S2WORLD2DEVICE(zpts[i], _S2ZAX);
    face4_base[0].n[i].x = _S2WORLD2DEVICE(normal[0], _S2XAX);
    face4_base[0].n[i].y = _S2WORLD2DEVICE(normal[1], _S2YAX);
    face4_base[0].n[i].z = _S2WORLD2DEVICE(normal[2], _S2ZAX);


    face4_base[0].colour[i].r = _S2PENRED;
    face4_base[0].colour[i].g = _S2PENGRN;
    face4_base[0].colour[i].b = _S2PENBLU;
    strcpy(face4_base[0].whichscreen, _s2_whichscreen);
    strncpy(face4_base[0].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], 
	    MAXVRMLLEN);
    face4_base[0].VRMLname[MAXVRMLLEN-1] = '\0';
  }
}

/* draw text in current color, at position, right and up vectors */
void _s2priv_text(float *labelp, float *labelr, float *labelu, char *text) {

  LABEL *label_base = _s2priv_addlabels(1);
  if (!label_base) {
    _s2error("(internal)", "memory allocation failed for text label");
  }
  

  label_base[0].p.x = _S2WORLD2DEVICE(labelp[0], _S2XAX);
  label_base[0].p.y = _S2WORLD2DEVICE(labelp[1], _S2YAX);
  label_base[0].p.z = _S2WORLD2DEVICE(labelp[2], _S2ZAX);

  /* bug fix?  _SO means scale only: these vectors should not be offset
   * from zero in world / device coords */
  label_base[0].right.x = _S2WORLD2DEVICE_SO(labelr[0], _S2XAX);
  label_base[0].right.y = _S2WORLD2DEVICE_SO(labelr[1], _S2YAX);
  label_base[0].right.z = _S2WORLD2DEVICE_SO(labelr[2], _S2ZAX);
  label_base[0].up.x = _S2WORLD2DEVICE_SO(labelu[0], _S2XAX);
  label_base[0].up.y = _S2WORLD2DEVICE_SO(labelu[1], _S2YAX);
  label_base[0].up.z = _S2WORLD2DEVICE_SO(labelu[2], _S2ZAX);

  label_base[0].colour.r = _S2PENRED;
  label_base[0].colour.g = _S2PENGRN;
  label_base[0].colour.b = _S2PENBLU;
  //strcpy(label_base[0].s, text);
  strncpy(label_base[0].s, text, MAXLABELLEN-1);
  label_base[0].s[MAXLABELLEN-1] = '\0';

  strcpy(label_base[0].whichscreen, _s2_whichscreen);
  strncpy(label_base[0].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  label_base[0].VRMLname[MAXVRMLLEN-1] = '\0';

}

void _s2_priv_textab(float ix, float iy, float iz, 
		     int axis1, int axis2, 
		     float flip1, float flip2, char *itext) {
  /* arrange things so we can call _s2priv_text, which needs world
   * 3-tuples giving position, right and up vectors.  itext is fine 
   * as it is.
   */
  float labelp[3], labelr[3], labelu[3];
  
  labelp[0] = ix;
  labelp[1] = iy;
  labelp[2] = iz;

  int axis3 = (3 - axis1 - axis2) % 3;

  /* ticklen is simply a fraction of the average axis length, scaled
   * by the current character height setting
   */
  float ticklen = _S2TIKFRAC * 0.33 * _s2_charsize *
    ((_s2axismax[0] - _s2axismin[0]) +
     (_s2axismax[1] - _s2axismin[1]) +
     (_s2axismax[2] - _s2axismin[2]));
  
  // right is along axis1 direction
  labelr[axis1] = _s2axissgn[axis1] * ticklen * flip1;
  labelr[axis2] = 0.0;
  labelr[axis3] = 0.0;

  // up is along y direction
  labelu[axis1] = 0.0;
  labelu[axis2] = _s2axissgn[axis2] * ticklen * flip2;
  labelu[axis3] = 0.0;

  _s2priv_text(labelp, labelr, labelu, itext);
}

void _s2_priv_qtxtab(float *a1, float *a2, float *b1, float *b2,
		     float ix, float iy, float iz, 
		     int axis1, int axis2,
		     float flip1, float flip2,
		     char *itext, float ipad) {
  /* DUPE from _s2_priv_textab */

  float labelp[3], labelr[3], labelu[3];
  
  labelp[0] = ix;
  labelp[1] = iy;
  labelp[2] = iz;

  int axis3 = (3 - axis1 - axis2) % 3;

  /* ticklen is simply a fraction of the average axis length, scaled
   * by the current character height setting
   */
  float ticklen = _S2TIKFRAC * 0.33 * _s2_charsize *
    ((_s2axismax[0] - _s2axismin[0]) +
     (_s2axismax[1] - _s2axismin[1]) +
     (_s2axismax[2] - _s2axismin[2]));
  
  // right is along axis1 direction
  labelr[axis1] = _s2axissgn[axis1] * ticklen * flip1;
  labelr[axis2] = 0.0;
  labelr[axis3] = 0.0;

  // up is along y direction
  labelu[axis1] = 0.0;
  labelu[axis2] = _s2axissgn[axis2] * ticklen * flip2;
  labelu[axis3] = 0.0;

  float ax1 = labelp[axis1];
  float ax2 = labelp[axis1] + (float)strlen(itext) * labelr[axis1] * 0.69;
  float ab1 = labelp[axis2] - labelu[axis2] * 0.30; /* offset for q, g, etc. */
  float ab2 = labelp[axis2] + labelu[axis2] * 0.86;

  /* now if you use ipad on all axes, the padding is usually too long
   * along the length of the text, as that's a further distance.  So we 
   * use "ipad" for the vertical padding, and scale it for the horizontal
   * padding.
   */
  if (a1) {
    *a1 = ax1 - (ax2 - ax1) * ipad * 0.5 / (float)strlen(itext);
  }
  if (a2) {
    *a2 = ax2 + (ax2 - ax1) * ipad * 0.5 / (float)strlen(itext);
  }
  if (b1) {
    *b1 = ab1 - (ab2 - ab1) * ipad * 0.5;
  } 
  if (b2) {
    *b2 = ab2 + (ab2 - ab1) * ipad * 0.5;
  }

  return;
}

/* find the smallest "round" number larger than x, a "round" number
 * being 1, 2 or 5 times a power of 10.  If x is negative, return
 * -round(abs(x)).  On return sub is a suitable number of
 * subdivisions; 2 or 5.
 *
 * Converted from a pgplot routine!  Thanks to TJP.
 */
float _s2priv_round(float x, int *nsub) {
  float nice[] = {0.0, 2.0, 5.0, 10.0};
  if (x == 0.0) {
    if (nsub) {
      *nsub = 2;
    }
    return 0.0;
  }
  float xx = fabsf(x);
  float xlog = log10f(xx);
  /*int ilog = floorf(xlog);*/
  int ilog = (int)(truncf(xlog));
  if (xlog < 0) {
    ilog--;
  }
  float pwr = powf(10.0, (float)ilog);
  float frac = xx / pwr;
  int i = 3;
  if (frac <= 5.0) {
    i = 2;
  }
  if (frac <= 2.0) {
    i = 1;
  }
  if (nsub) {
    *nsub = 5;
  }
  if (i == 1) {
    if (nsub) {
      *nsub = 2;
    }
  }
  return (x < 0 ? -fabsf(pwr*nice[i]) : fabsf(pwr*nice[i]));
}
  
/* draw a line by using a cylinder, or rather, a cone with non-converging
 * sides.  The thickness of the cylinder is auto-chosen.
 */
void _s2priv_coneline(float *xpts, float *ypts, float *zpts) {

  /* note: different ticklen calc actually works in device coords
   * NOT world coords 
   */
  float ticklen = _S2TIKFRAC * 0.02 *
    ((_s2devicemax[0] - _s2devicemin[0]) +
     (_s2devicemax[1] - _s2devicemin[1]) +
     (_s2devicemax[2] - _s2devicemin[2]));
  
  CONE *cone_base = _s2priv_addcones(1);
  if (!cone_base) {
    _s2error("(internal)", "memory allocation failed for cone-style line");
  }

  cone_base->p1.x = _S2WORLD2DEVICE(xpts[0], _S2XAX);
  cone_base->p1.y = _S2WORLD2DEVICE(ypts[0], _S2YAX);
  cone_base->p1.z = _S2WORLD2DEVICE(zpts[0], _S2ZAX);
  /* there has to be a better way to do the following - have scaled
   * radius according to X axis - should be dependent on all axes */
  cone_base->r1 = ticklen;
  cone_base->p2.x = _S2WORLD2DEVICE(xpts[1], _S2XAX);
  cone_base->p2.y = _S2WORLD2DEVICE(ypts[1], _S2YAX);
  cone_base->p2.z = _S2WORLD2DEVICE(zpts[1], _S2ZAX);
  cone_base->r2 = ticklen;
  cone_base->colour = _s2_colormap[_s2_colidx];
  strcpy(cone_base->whichscreen, _s2_whichscreen);
  strncpy(cone_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  cone_base->VRMLname[MAXVRMLLEN-1] = '\0';
  return;
}

int _s2priv_ns2cvr(float ***grid, 
		   int adim, int bdim, int cdim,
		   int a1, int a2, int b1, int b2, int c1, int c2,
		   float *tr, char trans, 
		   float datamin, float datamax,
		   float alphamin, float alphamax,
		   float (*ialphafn)(float*)) {
  
  int i, j, k;
  
  _s2_volumes = (_S2VRVOLUME *)realloc(_s2_volumes, (_s2_nvol + 1) * 
				      sizeof(_S2VRVOLUME));
  
  _S2VRVOLUME *it = _s2_volumes + _s2_nvol;
  it->grid = grid;
  it->adim = adim;
  it->bdim = bdim;
  it->cdim = cdim;
  it->a1 = a1;
  it->a2 = a2;
  it->b1 = b1;
  it->b2 = b2;
  it->c1 = c1;
  it->c2 = c2;
  if (tr) {
    bcopy(tr, it->tr, 12 * sizeof(float));
  } else {
    it->tr[2] = it->tr[3] = it->tr[5] = it->tr[7] = it->tr[9] = it->tr[10] = 0.;
    it->tr[0] = it->tr[4] = it->tr[8] = 0.;
    it->tr[1] = it->tr[6] = it->tr[11] = 1.0;
  }
  it->trans = trans;

  /* datamin, max ...*/
  if (datamax > datamin) {
    it->datamin = datamin;
    it->datamax = datamax;
  } else {
    it->datamin = 9e30;
    it->datamax = -9e30;
    for (i = 0; i < adim; i++) {
      for (j = 0; j < bdim; j++) {
	for (k = 0; k < cdim; k++) {
	  if (grid[i][j][k] > it->datamax) {
	    it->datamax = grid[i][j][k];
	  }
	  if (grid[i][j][k] < it->datamin) {
	    it->datamin = grid[i][j][k];
	  }
	}
      }
    }
  }

  it->alphamin = alphamin;
  it->alphamax = alphamax;
  it->alphafn = ialphafn;

  it->ntexts = 0;
  it->textureids = NULL;
  it->axis = 0;
  it->reverse = 0;

  _s2_nvol++;

  return _s2_nvol - 1;
}

/*-------------------------------------------------------------------------
   Given a grid cell and an isolevel, calculate the triangular
   facets requied to represent the isosurface through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
   0 will be returned if the grid cell is either totally above
   of totally below the isolevel.
*/
int Polygonise(GRIDCELL g,double iso,TRIANGLE *tri)
{
   int i,ntri = 0;
   int cubeindex;
   XYZ vertlist[12];
/*
   int edgeTable[256].  It corresponds to the 2^8 possible combinations of
   of the eight (n) vertices either existing inside or outside (2^n) of the
   surface.  A vertex is inside of a surface if the value at that vertex is
   less than that of the surface you are scanning for.  The table index is
   constructed bitwise with bit 0 corresponding to vertex 0, bit 1 to vert
   1.. bit 7 to vert 7.  The value in the table tells you which edges of
   the table are intersected by the surface.  Once again bit 0 corresponds
   to edge 0 and so on, up to edge 12.
   Constructing the table simply consisted of having a program run thru
   the 256 cases and setting the edge bit if the vertices at either end of
   the edge had different values (one is inside while the other is out).
   The purpose of the table is to speed up the scanning process.  Only the
   edges whose bit's are set contain vertices of the surface.
   Vertex 0 is on the bottom face, back edge, left side.
   The progression of vertices is clockwise around the bottom face
   and then clockwise around the top face of the cube.  Edge 0 goes from
   vertex 0 to vertex 1, Edge 1 is from 2->3 and so on around clockwise to
   vertex 0 again. Then Edge 4 to 7 make up the top face, 4->5, 5->6, 6->7
   and 7->4.  Edge 8 thru 11 are the vertical edges from vert 0->4, 1->5,
   2->6, and 3->7.
       4--------5     *---4----*
      /|       /|    /|       /|
     / |      / |   7 |      5 |
    /  |     /  |  /  8     /  9
   7--------6   | *----6---*   |
   |   |    |   | |   |    |   |
   |   0----|---1 |   *---0|---*
   |  /     |  /  11 /     10 /
   | /      | /   | 3      | 1
   |/       |/    |/       |/
   3--------2     *---2----*
*/

static int edgeTable[256]={
0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };

/*
   int triTable[256][16] also corresponds to the 256 possible combinations
   of vertices.
   The [16] dimension of the table is again the list of edges of the cube
   which are intersected by the surface.  This time however, the edges are
   enumerated in the order of the vertices making up the triangle mesh of
   the surface.  Each edge contains one vertex that is on the surface.
   Each triple of edges listed in the table contains the vertices of one
   triangle on the mesh.  The are 16 entries because it has been shown that
   there are at most 5 triangles in a cube and each "edge triple" list is
   terminated with the value -1.
   For example triTable[3] contains
   {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
   This corresponds to the case of a cube whose vertex 0 and 1 are inside
   of the surface and the rest of the verts are outside (00000001 bitwise
   OR'ed with 00000010 makes 00000011 == 3).  Therefore, this cube is
   intersected by the surface roughly in the form of a plane which cuts
   edges 8,9,1 and 3.  This quadrilateral can be constructed from two
   triangles: one which is made of the intersection vertices found on edges
   1,8, and 3; the other is formed from the vertices on edges 9,8, and 1.
   Remember, each intersected edge contains only one surface vertex.  The
   vertex triples are listed in counter clockwise order for proper facing.
*/
static int triTable[256][16] =
{{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

   /*
      Determine the index into the edge table which
      tells us which vertices are inside of the surface
   */
   cubeindex = 0;
   if (g.val[0] < iso) cubeindex |= 1;
   if (g.val[1] < iso) cubeindex |= 2;
   if (g.val[2] < iso) cubeindex |= 4;
   if (g.val[3] < iso) cubeindex |= 8;
   if (g.val[4] < iso) cubeindex |= 16;
   if (g.val[5] < iso) cubeindex |= 32;
   if (g.val[6] < iso) cubeindex |= 64;
   if (g.val[7] < iso) cubeindex |= 128;

   /* Cube is entirely in/out of the surface */
   if (edgeTable[cubeindex] == 0)
      return(0);

   /* Find the vertices where the surface intersects the cube */
   if (edgeTable[cubeindex] & 1)
      vertlist[0] = VertexInterp(iso,g.p[0],g.p[1],g.val[0],g.val[1]);
   if (edgeTable[cubeindex] & 2)
      vertlist[1] = VertexInterp(iso,g.p[1],g.p[2],g.val[1],g.val[2]);
   if (edgeTable[cubeindex] & 4)
      vertlist[2] = VertexInterp(iso,g.p[2],g.p[3],g.val[2],g.val[3]);
   if (edgeTable[cubeindex] & 8)
      vertlist[3] = VertexInterp(iso,g.p[3],g.p[0],g.val[3],g.val[0]);
   if (edgeTable[cubeindex] & 16)
      vertlist[4] = VertexInterp(iso,g.p[4],g.p[5],g.val[4],g.val[5]);
   if (edgeTable[cubeindex] & 32)
      vertlist[5] = VertexInterp(iso,g.p[5],g.p[6],g.val[5],g.val[6]);
   if (edgeTable[cubeindex] & 64)
      vertlist[6] = VertexInterp(iso,g.p[6],g.p[7],g.val[6],g.val[7]);
   if (edgeTable[cubeindex] & 128)
      vertlist[7] = VertexInterp(iso,g.p[7],g.p[4],g.val[7],g.val[4]);
   if (edgeTable[cubeindex] & 256)
      vertlist[8] = VertexInterp(iso,g.p[0],g.p[4],g.val[0],g.val[4]);
   if (edgeTable[cubeindex] & 512)
      vertlist[9] = VertexInterp(iso,g.p[1],g.p[5],g.val[1],g.val[5]);
   if (edgeTable[cubeindex] & 1024)
      vertlist[10] = VertexInterp(iso,g.p[2],g.p[6],g.val[2],g.val[6]);
   if (edgeTable[cubeindex] & 2048)
      vertlist[11] = VertexInterp(iso,g.p[3],g.p[7],g.val[3],g.val[7]);

   /* Create the triangles */
   for (i=0;triTable[cubeindex][i]!=-1;i+=3) {
      tri[ntri].p[0] = vertlist[triTable[cubeindex][i  ]];
      tri[ntri].p[1] = vertlist[triTable[cubeindex][i+1]];
      tri[ntri].p[2] = vertlist[triTable[cubeindex][i+2]];
      ntri++;
   }

   return(ntri);
}

void _s2priv_drawTriangleCache(_S2TRIANGLE_CACHE *cache) {
  int i;
  for (i = 0; i < cache->ntri; i++) {
    //ns2vf3a(cache->trivert + 3 * i, cache->col[i], cache->descr.trans,
    //	    cache->descr.alpha);
    ns2vf3na(cache->trivert + 3 * i, cache->normals + 3 * i,
	     cache->col[i], cache->descr.trans,
	     cache->descr.alpha);
  }
}

void _s2priv_addToTriangleCache(_S2TRIANGLE_CACHE *cache, XYZ *trivert, 
				float *tr, COLOUR col) {
//void _s2priv_addToTriangleCache(_S2TRIANGLE_CACHE *cache, XYZ *trivert,
//				XYZ cellnormal,
//				float *tr, COLOUR col) {
  int n = cache->ntri, i;
  cache->trivert = (XYZ *)realloc(cache->trivert, (n + 1) * 3 * sizeof(XYZ));
  //cache->normals = (XYZ *)realloc(cache->normals, (n + 1) * 3 * sizeof(XYZ));
  cache->col = (COLOUR *)realloc(cache->col, (n + 1) * sizeof(COLOUR));

  for (i = 0; i < 3; i++) {
    cache->trivert[n * 3 + i].x = 
      tr[0] + tr[1]*trivert[i].x + tr[2]*trivert[i].y + tr[3]*trivert[i].z;
    cache->trivert[n * 3 + i].y = 
      tr[4] + tr[5]*trivert[i].x + tr[6]*trivert[i].y + tr[7]*trivert[i].z;
    cache->trivert[n * 3 + i].z = 
      tr[8] + tr[9]*trivert[i].x + tr[10]*trivert[i].y + tr[11]*trivert[i].z;

  }
  cache->col[n] = col;
  cache->ntri++;
}

void _s2priv_calcNormalsForTriangleCache(_S2TRIANGLE_CACHE *cache) {

  if (cache->normals) {
    free(cache->normals);
  }
  cache->normals = (XYZ *)malloc(cache->ntri * 3 * sizeof(XYZ));

  if (_s2_fastsurfaces) {
    int i;
    for (i = 0; i < cache->ntri; i++) {
      cache->normals[3*i] = cache->normals[3*i+1] =
	cache->normals[3*i+2] = CalcNormal(cache->trivert[i*3],
					   cache->trivert[i*3+1],
					   cache->trivert[i*3+2]);
    }
    return;
  }


  // scaling for normals when axes are not cubic
  float sx, sy, sz;
  sx = sy = sz = 1.0;
  if (_s2_vralphascaling == 1) {
    // scale by volume dimensions
    sx = 1. / (float)(cache->descr.a2 - cache->descr.a1 + 1);
    sy = 1. / (float)(cache->descr.b2 - cache->descr.b1 + 1);
    sz = 1. / (float)(cache->descr.c2 - cache->descr.c1 + 1);
  } else if (_s2_vralphascaling == 2) {
    // scale by viewport dimensions
    sx = 1. / (_s2devicemax[_S2XAX] - _s2devicemin[_S2XAX]);
    sy = 1. / (_s2devicemax[_S2YAX] - _s2devicemin[_S2YAX]);
    sz = 1. / (_s2devicemax[_S2ZAX] - _s2devicemin[_S2ZAX]);
  }

  // store whether this vertex has had a normal assigned already
  // also store weights (area) of triangles...
  unsigned char *vdone = (unsigned char *)malloc(cache->ntri * 3); 
  float *pweight;
  pweight = (float *)malloc(cache->ntri * sizeof(float));
  XYZ vec1, vec2;
  int i,j,k,m;
  for (i = 0; i < cache->ntri; i++) {
    vdone[i*3] = vdone[i*3+1] = vdone[i*3+2] = 0;
    
    vec1 = VectorSub(cache->trivert[i*3], cache->trivert[i*3+1]);
    vec1.x *= sx;
    vec1.y *= sy;
    vec1.z *= sz;
    vec2 = VectorSub(cache->trivert[i*3], cache->trivert[i*3+2]);
    vec2.x *= sx;
    vec2.y *= sy;
    vec2.z *= sz;
    pweight[i] = 0.5 * Modulus(CrossProduct(vec1, vec2));    
  }

#define MAX_MATCHING_VERTS 30
  int matching_verts[MAX_MATCHING_VERTS];
  int nmatching_verts = 0;

  // loop over all triangles
  for (i = 0; i < cache->ntri; i++) {
    
    // loop over the vertices in this triangle
    for (j = 0; j < 3; j++) {

      // have we got a normal for this vertex already?
      if (vdone[i*3+j]) {
	continue;
      }

      // go through THIS and remaining triangles in list, find any with 
      // a vertex that matches this one, and create a weighted 
      // normal, weighted by inverse triangle area... (smaller
      // triangles are associated with higher curvature).  Then
      // apply this normal to the matching triangles and set
      // vdone appropriately.

      XYZ targ_vertex = cache->trivert[i*3+j];
      XYZ summed_normal = {0., 0., 0.};
      XYZ partial_normal;
      //XYZ vec1, vec2;
      float partial_weight;
      
      nmatching_verts = 0;

      for (k = i; k < cache->ntri && nmatching_verts < MAX_MATCHING_VERTS; k++) {
	for (m = 0; m < 3 && nmatching_verts < MAX_MATCHING_VERTS; m++) {
	  if (vdone[k*3+m]) {
	    continue;
	  }
	  if (S2_XYZWITHIN(targ_vertex, cache->trivert[k*3+m], 0.00001)) {
	    partial_normal = CalcNormal(cache->trivert[k*3], 
					cache->trivert[k*3+1],
					cache->trivert[k*3+2]);

	    matching_verts[nmatching_verts] = k*3+m;
	    nmatching_verts++;
#if (1)
	    static int showmesg = 0;
	    if (!showmesg) {
	      //fprintf(stderr, "EXAGGERATING NORMALS!!!\n");
	      showmesg = 1;
	    }
#endif
	    partial_normal.x *= sx;
	    partial_normal.y *= sy;
	    partial_normal.z *= sz;
	    partial_weight = pweight[k];

	    summed_normal.x += partial_normal.x * partial_weight;
	    summed_normal.y += partial_normal.y * partial_weight;
	    summed_normal.z += partial_normal.z * partial_weight;
	  }
	}
      }

      Normalise(&summed_normal);

      for (k = 0; k < nmatching_verts; k++) {
	cache->normals[matching_verts[k]] = summed_normal;
	vdone[matching_verts[k]] = 1;
      }
      /*
      for (k = i; k < cache->ntri; k++) {
	for (m = 0; m < 3; m++) {
	  if (vdone[k*3+m]) {
	    continue;
	  }
	  if (S2_XYZWITHIN(targ_vertex, cache->trivert[k*3+m], 0.001)) {
	    cache->normals[k*3+m] = summed_normal;
	    vdone[k*3+m] = 1;
	  }
	}
      }
      */
      
    }
  }

  free(pweight);
}
				     

void _s2priv_colrfn(float *ix, float *iy, float *iz, 
		    float *r, float *g, float *b) {
  if (r) {
    *r = _s2priv_colrfn_r;
  }
  if (g) {
    *g = _s2priv_colrfn_g;
  }
  if (b) {
    *b = _s2priv_colrfn_b;
  }
}


void _s2priv_generate_isosurface(int isid, int force) {
  _S2ISOSURFACE *it = _s2_isosurfs + isid;

  int state_changed = 0;
  if (memcmp(&(it->descr), &(it->cached_descr), 
	     sizeof(_S2TRIANGLE_CACHE_DESCR))) {
    // this should at least happen the first time around 
    state_changed = 1;
  }

  /* if state not changed AND not force, return leaving surface unchanged */
  if (!state_changed && !force) {
    return;
  }

  /* zap the current cache */
  it->ntri = 0;
  if (it->trivert) {
    free(it->trivert);
    it->trivert = NULL;
  }
  if (it->normals) {
    free(it->normals);
    it->normals = NULL;
  }
  if (it->col) {
    free(it->col);
    it->col = NULL;
  }

  /* copy the current descriptor to the cache descriptor */
  bcopy(&it->descr, &it->cached_descr, sizeof(_S2TRIANGLE_CACHE_DESCR));

  /* copy the color settings for this id to the global id */
  invbcopy(&_s2priv_colrfn_r, &it->descr.red, sizeof(float));
  invbcopy(&_s2priv_colrfn_g, &it->descr.green, sizeof(float));
  invbcopy(&_s2priv_colrfn_b, &it->descr.blue, sizeof(float));

  int i,j,k,n,l;
  float x2, y2, z2;
  GRIDCELL cell;
  TRIANGLE triangles[10];
  
  _s2debug("(internal)", "forming isosurface");
  
  x2 = y2 = z2 = 0.;
  
  COLOUR col;
  col.r = _s2priv_colrfn_r;
  col.g = _s2priv_colrfn_g;
  col.b = _s2priv_colrfn_b;
  float ixf, iyf, izf; /* fraction data indexes for colouring */
  float fr, fg, fb;

  int resolution = it->descr.resolution;
  float ***grid = it->grptr;

  // XXX pre-mult it->descr.tr by it->descr.local_tr ...
  float newtr[12];
  float *tr = it->descr.tr;
  float *ltr = it->descr.local_tr;
  newtr[0] = ltr[0] + ltr[1] * tr[0] + ltr[2] * tr[4] + ltr[3] * tr[8];
  newtr[1] =          ltr[1] * tr[1] + ltr[2] * tr[5] + ltr[3] * tr[9];
  newtr[2] =          ltr[1] * tr[2] + ltr[2] * tr[6] + ltr[3] * tr[10];
  newtr[3] =          ltr[1] * tr[3] + ltr[2] * tr[7] + ltr[3] * tr[11];
  
  newtr[4] = ltr[4] + ltr[5] * tr[0] + ltr[6] * tr[4] + ltr[7] * tr[8];
  newtr[5] =          ltr[5] * tr[1] + ltr[6] * tr[5] + ltr[7] * tr[9];
  newtr[6] =          ltr[5] * tr[2] + ltr[6] * tr[6] + ltr[7] * tr[10];
  newtr[7] =          ltr[5] * tr[3] + ltr[6] * tr[7] + ltr[7] * tr[11];
  
  newtr[8] = ltr[8] + ltr[9] * tr[0] + ltr[10]* tr[4] + ltr[11]* tr[8];
  newtr[9] =          ltr[9] * tr[1] + ltr[10]* tr[5] + ltr[11]* tr[9];
  newtr[10]=          ltr[9] * tr[2] + ltr[10]* tr[6] + ltr[11]* tr[10];
  newtr[11]=          ltr[9] * tr[3] + ltr[10]* tr[7] + ltr[11]* tr[11];

  for (i=it->descr.a1;i<=it->descr.a2-resolution;i+=resolution) {
    for (j=it->descr.b1;j<=it->descr.b2-resolution;j+=resolution) {
      for (k=it->descr.c1;k<=it->descr.c2-resolution;k+=resolution) {
	cell.p[0].x = i - x2;
	cell.p[0].y = j - y2;
	cell.p[0].z = k - z2;
	cell.val[0] = grid[i][j][k];
	cell.p[1].x = i + resolution - x2;
	cell.p[1].y = j     - y2;
	cell.p[1].z = k     - z2;
	cell.val[1] = grid[i+resolution][j][k];
	cell.p[2].x = i + resolution - x2;
	cell.p[2].y = j     - y2;
	cell.p[2].z = k + resolution - z2;
	cell.val[2] = grid[i+resolution][j][k+resolution];
	cell.p[3].x = i     - x2;
	cell.p[3].y = j     - y2;
	cell.p[3].z = k + resolution - z2;
	cell.val[3] = grid[i][j][k+resolution];
	cell.p[4].x = i     - x2;
	cell.p[4].y = j + resolution - y2;
	cell.p[4].z = k     - z2;
	cell.val[4] = grid[i][j+resolution][k];
	cell.p[5].x = i + resolution - x2;
	cell.p[5].y = j + resolution - y2;
	cell.p[5].z = k     - z2;
	cell.val[5] = grid[i+resolution][j+resolution][k];
	cell.p[6].x = i + resolution - x2;
	cell.p[6].y = j + resolution - y2;
	cell.p[6].z = k + resolution - z2;
	cell.val[6] = grid[i+resolution][j+resolution][k+resolution];
	cell.p[7].x = i     - x2;
	cell.p[7].y = j + resolution - y2;
	cell.p[7].z = k + resolution - z2;
	cell.val[7] = grid[i][j+resolution][k+resolution];
	
	n = Polygonise(cell,it->descr.level,triangles);

	for (l=0;l<n;l++) {
	  if (it->descr.fcol != _s2priv_colrfn) {
	    ixf = 0.3333333333333 * (triangles[l].p[0].x + triangles[l].p[1].x + triangles[l].p[2].x);
	    iyf = 0.3333333333333 * (triangles[l].p[0].y + triangles[l].p[1].y + triangles[l].p[2].y);
	    izf = 0.3333333333333 * (triangles[l].p[0].z + triangles[l].p[1].z + triangles[l].p[2].z);
	    it->descr.fcol(&ixf, &iyf, &izf, &fr, &fg, &fb);
	    col.r = fr;
	    col.g = fg;
	    col.b = fb;
	  }


	  //_s2priv_addToTriangleCache(it, triangles[l].p, it->descr.tr, col);
	  _s2priv_addToTriangleCache(it, triangles[l].p, newtr, col);
	}
      }
    }
  }
  _s2priv_calcNormalsForTriangleCache(it);
  return;
}

void _s2priv_bb(XYZ iP, XYZ iStretch, XYZ ioffset, 
		float iaspect, float size, float pangle, 
		COLOUR icol, unsigned int iid, float ialpha, 
		char itrans) {
  if (!_s2_dynamicEnabled) {
    _s2warn("_s2priv_bb", "called from non-dynamic mode");
    return;
  }
  _S2BBOARD *bboard_base = _s2priv_addbboards(1);
  if (!bboard_base) {
    _s2warn("_s2priv_bb", "could not allocate memory for billboard");
    return;
  }
  bboard_base[0].p.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  bboard_base[0].p.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  bboard_base[0].p.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  bboard_base[0].str = iStretch;
  bboard_base[0].offset = ioffset;
  bboard_base[0].aspect = iaspect;
  bboard_base[0].size = 
    sqrt(0.33333 * (powf(_S2WORLD2DEVICE_SO(size, _S2XAX),2.) +
		    powf(_S2WORLD2DEVICE_SO(size, _S2YAX),2.) +
		    powf(_S2WORLD2DEVICE_SO(size, _S2ZAX),2.)));
  bboard_base[0].pa = pangle * M_PI / 180.0; /* position angle in radians */
  bboard_base[0].col = icol;
  bboard_base[0].texid = iid;
  bboard_base[0].alpha = ialpha;
  bboard_base[0].trans = itrans;
  strcpy(bboard_base[0].whichscreen, _s2_whichscreen);
  strncpy(bboard_base[0].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  bboard_base[0].VRMLname[MAXVRMLLEN-1] = '\0';

  return;
}

// return the axis (1,2,3) and order (0,1) of drawing for the
// given volume rendering id.  Return axis = 0 means failure.
void _s2priv_choose_vr_axis(int vrid, int *axis, int *reverse) {

  *axis = 0;
  *reverse = 0;

  //int i, j, k;
  _S2VRVOLUME *it = _s2_volumes + vrid;

  // figure out which axis to draw
  double xdot, ydot, zdot;
  XYZ cp;
  cp.x = _S2WORLD2DEVICE_SO(it->tr[ 1], _S2XAX);
  cp.y = _S2WORLD2DEVICE_SO(it->tr[ 5], _S2YAX);
  cp.z = _S2WORLD2DEVICE_SO(it->tr[ 9], _S2ZAX);
  Normalise(&cp);
  xdot = DotProduct(camera.vd, cp);
  
  cp.x = _S2WORLD2DEVICE_SO(it->tr[ 2], _S2XAX);
  cp.y = _S2WORLD2DEVICE_SO(it->tr[ 6], _S2YAX);
  cp.z = _S2WORLD2DEVICE_SO(it->tr[10], _S2ZAX);
  Normalise(&cp);
  ydot = DotProduct(camera.vd, cp);
  
  cp.x = _S2WORLD2DEVICE_SO(it->tr[ 3], _S2XAX);
  cp.y = _S2WORLD2DEVICE_SO(it->tr[ 7], _S2YAX);
  cp.z = _S2WORLD2DEVICE_SO(it->tr[11], _S2ZAX);
  Normalise(&cp);
  zdot = DotProduct(camera.vd, cp);
  
  if (ABS(xdot) >= ABS(ydot) && ABS(xdot) >= ABS(zdot)) {
    *axis = 1;
    if (xdot > 0) {
      *reverse = 1;
    }
  }
  if (ABS(ydot) >= ABS(xdot) && ABS(ydot) >= ABS(zdot)) {
    *axis = 2;
    if (ydot > 0) {
      *reverse = 1;
    }
  }
  if (ABS(zdot) >= ABS(xdot) && ABS(zdot) >= ABS(ydot)) {
    *axis = 3;
    if (zdot > 0) {
      *reverse = 1;
    }
  }

  return;
}

// call with iaxis == 0 to request autoselect of axis
// otherwise axis = abs(axis) and reverse = (axis < 0)
void _s2priv_load_vr_textures(int vrid, int force, int iaxis) {

  int axis = 0;
  int reverse = 0;

  int i, j, k;
  _S2VRVOLUME *it = _s2_volumes + vrid;

  if (iaxis != 0) {
    axis = abs(iaxis);
    if (iaxis < 0) {
      reverse = 1;
    }
  } else {
#if (1)
    _s2priv_choose_vr_axis(vrid, &axis, &reverse);
#else
    // figure out which axis to draw
    double xdot, ydot, zdot;
    XYZ cp;
    cp.x = _S2WORLD2DEVICE_SO(it->tr[ 1], _S2XAX);
    cp.y = _S2WORLD2DEVICE_SO(it->tr[ 5], _S2YAX);
    cp.z = _S2WORLD2DEVICE_SO(it->tr[ 9], _S2ZAX);
    Normalise(&cp);
    xdot = DotProduct(camera.vd, cp);
    
    cp.x = _S2WORLD2DEVICE_SO(it->tr[ 2], _S2XAX);
    cp.y = _S2WORLD2DEVICE_SO(it->tr[ 6], _S2YAX);
    cp.z = _S2WORLD2DEVICE_SO(it->tr[10], _S2ZAX);
    Normalise(&cp);
    ydot = DotProduct(camera.vd, cp);
    
    cp.x = _S2WORLD2DEVICE_SO(it->tr[ 3], _S2XAX);
    cp.y = _S2WORLD2DEVICE_SO(it->tr[ 7], _S2YAX);
    cp.z = _S2WORLD2DEVICE_SO(it->tr[11], _S2ZAX);
    Normalise(&cp);
    zdot = DotProduct(camera.vd, cp);
    
    if (ABS(xdot) >= ABS(ydot) && ABS(xdot) >= ABS(zdot)) {
      axis = 1;
      if (xdot > 0) {
	reverse = 1;
      }
    }
    if (ABS(ydot) >= ABS(xdot) && ABS(ydot) >= ABS(zdot)) {
      axis = 2;
      if (ydot > 0) {
	reverse = 1;
      }
    }
    if (ABS(zdot) >= ABS(xdot) && ABS(zdot) >= ABS(ydot)) {
      axis = 3;
      if (zdot > 0) {
	reverse = 1;
      }
    }
#endif
  }
  
  if (axis == 0) {
    _s2warn("(internal)", "invalid camera axis for volume rendering");
    return;
  }

  if (axis == it->axis /* && reverse == it->reverse */ && !force) {
    //fprintf(stderr, "axis remains at %d\n", axis);
    it->reverse = reverse;
    return;
  }
  if (0 && options.debug) {
    fprintf(stderr,"New axis. Was %d, now %d\n",it->axis, axis);
  }
  it->axis = axis;
  it->reverse = reverse;


  // if we are here, then the axis has changed, so we need to zap
  // all the textures and recreate new ones.
  
  // delete textures
  for (i = 0; i < it->ntexts; i++) {
    ss2dt(it->textureids[i]);
  }
  
  // make new textures
  unsigned char *tptr = NULL;
  unsigned char *tbit = NULL;
  int nt, width, height, wq, hq;
  int tid;
  float sca = 1. / (it->datamax - it->datamin);
  float valsca;

  int idx1 = _s2_colr1;
  int idx2 = _s2_colr2;
  int idx;
  float r, g, b;

#define MAXOPACFRAC 0.7

  // alpha scaling for non-cubic volumes
  float sx, sy, sz, basess, xsca;
  sx = sy = sz = 1.0;
  if (_s2_vralphascaling == 1) {
    // scale by volume dimensions
    sx = it->a2 - it->a1 + 1;
    sy = it->b2 - it->b1 + 1;
    sz = it->c2 - it->c1 + 1;    
  } else if (_s2_vralphascaling == 2) {
    // scale by viewport dimensions
    sx = _s2devicemax[_S2XAX] - _s2devicemin[_S2XAX];
    sy = _s2devicemax[_S2YAX] - _s2devicemin[_S2YAX];
    sz = _s2devicemax[_S2ZAX] - _s2devicemin[_S2ZAX];
  }
  basess = sx;
  if (sy < basess) {
    basess = sy;
  }
  if (sz < basess) {
    basess = sz;
  }
  xsca = 1.0;

  switch (axis) {

  case 1:
    _s2debug("(internal)", "creating volume rendering textures for X-view");
    // textures are zy planes 
    nt = it->ntexts = it->a2 - it->a1 + 1;

    // alpha scaling for non-cubic volumes: 1.0 for x views
    if (_s2_evas_x > 0.0) {
      xsca = _s2_evas_x;
    } else {
      xsca = basess / sx;
    }
    //fprintf(stderr, "X-view xsca = %f; sca = %f\n", xsca, sca);
    //fprintf(stderr, "X-view amin = %f; amax = %f\n", it->alphamin, it->alphamax);

    //float minaf = 9e30, maxaf = -9e30;
    width = it->c2 - it->c1 + 1;
    height = it->b2 - it->b1 + 1;
    it->textureids = (unsigned int *)realloc(it->textureids, 
					     nt * sizeof(unsigned int));
    for (tid = 0; tid < nt; tid++) {
      it->textureids[tid] = ss2ct(width, height);
      tptr = ss2gt(it->textureids[tid], &wq, &hq);
      // (ignore wq, hq, as they should be idential to width, height)
      i = it->a1 + tid;
      for (k = 0; k < width; k++) {
	for (j = 0; j < height; j++) {
	  tbit = tptr + 4 * (k + width * j);
	  valsca = (it->grid[i][j + it->b1][k + it->c1] - it->datamin) * sca;
	  valsca = (valsca < 0.) ? 0. : ((valsca > 1.) ? 1. : valsca);
	  idx = idx1 + (int)(valsca * (float)(idx2 - idx1));
	  idx = (idx < idx1) ? idx1 : ((idx > idx2) ? idx2 : idx);
	  s2qcr(idx, &r, &g, &b);
	  //if (tid == 15) {
	  //  fprintf(stderr, "%5d %5d %7.3f %3d %7.4f %7.4f %7.4f\n", 
	  //	    idx1, idx2, valsca, idx, r, g, b);
	  //}
	  tbit[0] = r * 255.; // red
	  //if (tid == 15) {
	  //  fprintf(stderr, "%3d ", tbit[0]);
	  //}
	  tbit[1] = g * 255.; // green
	  tbit[2] = b * 255.; // blue
	  //tbit[3] = 255. * valsca * MAXOPACFRAC;
	  if (it->alphafn) {
	    tbit[3] = 255. * xsca * it->alphafn(&it->grid[i][j + it->b1][k + it->c1]);
	  } else {
	    // added db 20120131 to clip data < dmin
	    if (it->grid[i][j + it->b1][k + it->c1] - it->datamin < 0) {
	      tbit[3] = 0;
	    } else {
	      tbit[3] = 255. * xsca * (it->alphamin + valsca * (it->alphamax - it->alphamin));
	    }
	  }
	  //if (tbit[3] < minaf) {
	  //  minaf = tbit[3];
	  //}
	  //if (tbit[3] > maxaf) {
	  //  maxaf = tbit[3];
	  //}
	}
      }
      ss2pt(it->textureids[tid]);
      //fprintf(stderr, "minaf = %f, maxaf = %f\n", minaf, maxaf);
    }
    break;

  case 2:
    _s2debug("(internal)", "creating volume rendering textures for Y-view");
    // textures are zy planes 
    nt = it->ntexts = it->b2 - it->b1 + 1;

    // alpha scaling for non-cubic volumes: 1.0 for x views
    if (_s2_evas_y > 0.0) {
      xsca = _s2_evas_y;
    } else {
      xsca = basess / sy;
    }
    //fprintf(stderr, "Y-view xsca = %f\n", xsca);

    width = it->a2 - it->a1 + 1;
    height = it->c2 - it->c1 + 1;
    it->textureids = (unsigned int *)realloc(it->textureids, 
					     nt * sizeof(unsigned int));
    for (tid = 0; tid < nt; tid++) {
      it->textureids[tid] = ss2ct(width, height);
      tptr = ss2gt(it->textureids[tid], &wq, &hq);
      // (ignore wq, hq, as they should be idential to width, height)
      j = it->b1 + tid;
      for (i = 0; i < width; i++) {
	for (k = 0; k < height; k++) {
	  tbit = tptr + 4 * (i + width * k);
	  valsca = (it->grid[i + it->a1][j][k + it->c1] - it->datamin) * sca;
	  valsca = (valsca < 0.) ? 0. : ((valsca > 1.) ? 1. : valsca);
	  idx = idx1 + valsca * (idx2 - idx1);
	  idx = (idx < idx1) ? idx1 : ((idx > idx2) ? idx2 : idx);
	  s2qcr(idx, &r, &g, &b);
	  tbit[0] = r * 255.; // red
	  tbit[1] = g * 255.; // green
	  tbit[2] = b * 255.; // blue
	  //tbit[3] = 255. * valsca * MAXOPACFRAC;
	  if (it->alphafn) {
	    tbit[3] = 255. * xsca * it->alphafn(&it->grid[i + it->a1][j][k + it->c1]);
	  } else {
	    // added db 20120131 to clip data < dmin
	    if (it->grid[i + it->a1][j][k + it->c1] - it->datamin < 0) {
	      tbit[3] = 0;
	    } else {
	    tbit[3] = 255. * xsca * (it->alphamin + valsca * (it->alphamax - it->alphamin));
	    }
	  }
	}
      }
      ss2pt(it->textureids[tid]);
    }
    break;

  case 3:
    _s2debug("(internal)", "creating volume rendering textures for Z-view");
    // textures are xy planes 
    nt = it->ntexts = it->c2 - it->c1 + 1;

    // alpha scaling for non-cubic volumes: 1.0 for x views
    if (_s2_evas_z > 0.0) {
      xsca = _s2_evas_z;
    } else {
      xsca = basess / sz;
    }
    //fprintf(stderr, "Z-view xsca = %f\n", xsca);

    width = it->a2 - it->a1 + 1;
    height = it->b2 - it->b1 + 1;
    it->textureids = (unsigned int *)realloc(it->textureids, 
					     nt * sizeof(unsigned int));
    for (tid = 0; tid < nt; tid++) {
      it->textureids[tid] = ss2ct(width, height);
      tptr = ss2gt(it->textureids[tid], &wq, &hq);
      // (ignore wq, hq, as they should be idential to width, height)
      k = it->c1 + tid;
      for (i = 0; i < width; i++) {
	for (j = 0; j < height; j++) {
	  tbit = tptr + 4 * (i + width * j);
	  valsca = (it->grid[it->a1 + i][it->b1 + j][k] - it->datamin) * sca;
	  valsca = (valsca < 0.) ? 0. : ((valsca > 1.) ? 1. : valsca);
	  idx = idx1 + valsca * (idx2 - idx1);
	  idx = (idx < idx1) ? idx1 : ((idx > idx2) ? idx2 : idx);
	  s2qcr(idx, &r, &g, &b);
	  tbit[0] = r * 255.; // red
	  tbit[1] = g * 255.; // green
	  tbit[2] = b * 255.; // blue
	  //tbit[3] = 255. * valsca * MAXOPACFRAC;
	  if (it->alphafn) {
	    tbit[3] = 255. * xsca * it->alphafn(&it->grid[i + it->a1][j + it->b1][k]);
	  } else {
	    // added db 20120131 to clip data < dmin
	    if (it->grid[it->a1 + i][it->b1 + j][k] - it->datamin < 0) {
	      tbit[3] = 0;
	    } else {
	      tbit[3] = 255. * xsca * (it->alphamin + valsca * (it->alphamax - it->alphamin));
	    }
	  }
	  /*
	  if (i == 0  || j == 0 || i == width-1 || j == height-1) {
	    tbit[0] = 0;
	    tbit[1] = 255;
	    tbit[2] = 128;
	    tbit[3] = 255;
	  }
	  */
	}
      }
      ss2pt(it->textureids[tid]);
    }
    break;

  } // case (axis) 

}

/* Draw the surface described by the provided function "fab(a, b)".
 * The function is evaluated on a na X nb grid whose world coordinates
 * extend from (amin,bmin) to (amax,bmax).  The "ctl" argument has the
 * following effect:
 *
 * ctl = 0: surface is plotted in current window and viewport.  Caller
 * is responsible for having set the viewport and world coordinate
 * system suitably.
 *
 * ctl = 1: s2env is called automatically to fit the plot in the 
 * current viewport.
 *
 * The caller controls which axes a and b map to, and implicitly which
 * axis the data values map to, using the arguments "axisa" and
 * "axisb".  0=x, 1=y, and 2=z, so axisa=0 and axisb=1 will produce a 
 * "normal" z=f(x,y) plot.
 * 
 * If rmin > rmax, then mapping of function value to colormap is 
 * autoscaled, otherwise data value rmin -> color index 1, and 
 * rmax -> color index 2, linear inbetwixt.
 */
void _s2priv_funab(float(*ifab)(float*,float*), int ina, int inb, 
		   float iamin, float iamax, float ibmin, float ibmax, 
		   int iaxisa, int iaxisb, int ictl,
		   float irmin, float irmax) {
  if ((ina < 3) || (inb < 3) || (iamin > iamax) || (ibmin > ibmax)) {
    _s2warn("s2fun*", "invalid limits of insufficient grid size");
    return;
  }
  if ((iaxisa == iaxisb) || (iaxisa < 0) || (iaxisa > 2) ||
      (iaxisb < 0) || (iaxisb > 2)) {
    _s2warn("s2fun*", "invalid axis selection");
    return;
  }
  if (!ifab) {
    _s2warn("s2fun*", "invalid arguments [NULL function pointer]");
    return;
  }

  /* note iaxisa, iaxisb and axisc are in USER terms, that is, 0=x, 
   * 1=y, 2=z.  We will need to change them below according to the 
   * current values of _S2XAX, _S2YAX, _S2ZAX.
   */
  int axisc = (3 - iaxisa - iaxisb) % 3;

  float **data;
  int i, j;
  data = (float **)calloc(ina, sizeof(float **));
  if (!data) {
    _s2warn("s2fun*", "memory allocation failed");
    return;
  }
  float dmin = 9e30, dmax = -9e30;
  float a, b;
  // evaluate function and determine data range
    for (i = 0; i < ina; i++) {
    a = iamin + (float)i / (float)(ina - 1) * (iamax-iamin);
    data[i] = (float *)calloc(inb, sizeof(float *));
    if (!data[i]) {
      _s2warn("s2fun*", "memory allocation failed");
      return;
    }
    for (j = 0; j < inb; j++) {
      b = ibmin + (float)j / (float)(inb - 1) * (ibmax-ibmin);
      data[i][j] = (*ifab)(&a, &b);
      if (data[i][j] > dmax) {
	dmax = data[i][j];
      }
      if (data[i][j] < dmin) {
	dmin = data[i][j];
      }
    }
  }

  /* figure out correct mappings */
  /* and sort out axis limits if needed later */
  /* there would be a neater way to do this, but haven't bothered yet! */
  int axis, axis2, axis3;
  axis = axis2 = axis3 = -1;
  float txmin, txmax, tymin, tymax, tzmin, tzmax;
  txmin = tymin = tzmin = -1.0;
  txmax = tymax = tzmax = 1.0;
  switch (iaxisa) {
  case 0:
    axis = _S2XAX;
    txmin = iamin;
    txmax = iamax;
    break;
  case 1:
    axis = _S2YAX;
    tymin = iamin;
    tymax = iamax;
    break;
  case 2:
    axis = _S2ZAX;
    tzmin = iamin;
    tzmax = iamax;
    break;
  }
  switch (iaxisb) {
  case 0:
    axis2 = _S2XAX;
    txmin = ibmin;
    txmax = ibmax;
    break;
  case 1:
    axis2 = _S2YAX;
    tymin = ibmin;
    tymax = ibmax;
    break;
  case 2:
    axis2 = _S2ZAX;
    tzmin = ibmin;
    tzmax = ibmax;
    break;
  }
  switch(axisc) {
  case 0:
    axis3 = _S2XAX;
    txmin = dmin;
    txmax = dmax;
    break;
  case 1:
    axis3 = _S2YAX;
    tymin = dmin;
    tymax = dmax;
    break;
  case 2:
    axis3 = _S2ZAX;
    tzmin = dmin;
    tzmax = dmax;
    break;
  }

  if (irmin < irmax) {
    /* copy caller-supplied color limits to dmin, dmax */
    dmin = irmin;
    dmax = irmax;
  }
  
  /* now create transform that places "a" on axis, "b" on axis2,
   * and the function value on axis3.
   */
  float tr[12];
  tr[4*axis+0] = iamin; 
  tr[4*axis+1] = (iamax-iamin)/(float)(ina-1); 
  tr[4*axis+2] = 0.0;
  tr[4*axis+3] = 0.0;

  tr[4*axis2+0] = ibmin;
  tr[4*axis2+1] = 0.0; 
  tr[4*axis2+2] = (ibmax-ibmin)/(float)(inb-1);
  tr[4*axis2+3] = 0.0;

  tr[4*axis3+0] = 0.0;
  tr[4*axis3+1] = 0.0;
  tr[4*axis3+2] = 0.0;
  tr[4*axis3+3] = 1.0;

  if (ictl == 1) {
    /* arg order here is USER view, ie x, y then z. */
    s2env(txmin, txmax, tymin, tymax, tzmin, tzmax, 0, 0);
  }

  // and draw the surface 
  s2surpa(data, ina, inb, 0, ina-1, 0, inb-1, dmin, dmax, tr);

  /* free alloc'd memory */
  for (i = 0; i < ina; i++) {
    free(data[i]);
    data[i] = NULL;
  }
  free(data);

}


void _s2priv_s2funuv(float(*fx)(float*,float*), float(*fy)(float*,float*), 
		     float(*fz)(float*,float*), float(*fcol)(float*,float*), 
		     char trans, float(*ialphafn)(float*,float*),
		     float umin, float umax, int uDIV,
		     float vmin, float vmax, int vDIV) {
		     
  int i, j;
  float alpha, beta;
  float w, r, g, b;
  int cidx;

  int cmin, cmax;
  s2qcir(&cmin, &cmax);

  /* first pass: fill list of vertices and colours
   *
   * we choose to interpret uDIV and vDIV as number of "square
   * patches" in each direction.  To obtain normals at all vertices,
   * we need to extend the total range somewhat.  I suppose this is
   * dangerous for equations with poles, but it's worth a shot.
   * 
   * So, to get uDIV and vDIV square patches in each dir, we need
   * one additional vertex in each direction.
   *
   * PLUS two extended vertices in each direction.
   */
  int nvertx = uDIV + 3;
  int nverty = vDIV + 3;
  int nvert = nvertx * nverty;
  float uumin = umin - (umax - umin) / uDIV;
  float uumax = umax + (umax - umin) / uDIV;
  float vvmin = vmin - (vmax - vmin) / vDIV;
  float vvmax = vmax + (vmax - vmin) / vDIV;
  int vertn = 0;
  XYZ *vertices = (XYZ *)calloc(nvert, sizeof(XYZ));
  COLOUR *colours = (COLOUR *)calloc(nvert, sizeof(COLOUR));
  float *alphas = (float *)calloc(nvert, sizeof(float));
  if (!vertices || !colours) {
    _s2error("s2funuv", "failed to allocate vertex/colour memory");
  }
  for (i = 0; i < nvertx; i++) {
    alpha = uumin + ((float)i / (float)(nvertx-1)) * (uumax - uumin);
    for (j = 0; j < nverty; j++) {
      beta = vvmin + ((float)j / (float)(nverty-1)) * (vvmax - vvmin);
      vertices[vertn].x = fx(&alpha, &beta);
      vertices[vertn].y = fy(&alpha, &beta);
      vertices[vertn].z = fz(&alpha, &beta);
      w = fcol(&alpha, &beta);
      cidx = MAX(cmin,MIN(cmax,(int)(cmin+w*(float)(cmax-cmin))));
      s2qcr(cidx, &r, &g, &b);
      colours[vertn].r = r;
      colours[vertn].g = g;
      colours[vertn].b = b;
      if (ialphafn) {
	alphas[vertn] = ialphafn(&alpha, &beta);
      } else {
	alphas[vertn] = 1.0;
      }
      vertn++;
    }
  }
  if (vertn != nvert) {
    _s2error("s2funuv", "internal error");
  }

  /* second pass: create list of triangles with vertices from above
   * list, and store face normal in "c" (centroid).  Do this so we can
   * later fill in vertex normals in the "n" (normal).
   *
   * How many triangles are there?  Two for each vertex, except the
   * final row and column of vertices.
   */
  int ntrgl = 2 * (nvertx - 1) * (nverty - 1);
  TRIANGLE *trgls = (TRIANGLE *)calloc(ntrgl, sizeof(TRIANGLE));
  if (!trgls) {
    _s2error("s2funuv", "failed to allocate memory for triangle mesh");
  }
  int trgln = 0;
  for (i = 0; i < (nvertx - 1); i++) {
    for (j = 0; j < (nverty - 1); j++) {
      /* first triangle derived at this vertex */
      trgls[trgln].p[0] = vertices[i *     nverty + j];
      trgls[trgln].p[1] = vertices[i *     nverty + j + 1];
      trgls[trgln].p[2] = vertices[(i+1) * nverty + j];
      trgls[trgln].c = CalcNormal(trgls[trgln].p[0], trgls[trgln].p[1],
      			  trgls[trgln].p[2]);
      trgln++;

      /* second triangle derived at this vertex */
      trgls[trgln].p[0] = vertices[i *     nverty + j + 1];
      trgls[trgln].p[1] = vertices[(i+1) * nverty + j + 1];
      trgls[trgln].p[2] = vertices[(i+1) * nverty + j];
      trgls[trgln].c = CalcNormal(trgls[trgln].p[0], trgls[trgln].p[1],
      			  trgls[trgln].p[2]);
      trgln++;
    }
  }      
  if (trgln != ntrgl) {
    _s2error("s2funuv", "internal error");
  }

  /* third pass: calculate vertex normals via centroid of six 
   * adjacent face normals.
   *
   * There are as many vertex normals as vertices (ie. one more 
   * in each direction than square patches).
   *
   * However, we only need vertex normals for the inner grid, 
   * excluding the first and last rows and columns.  We'll 
   * allocate the memory the same way as for vertices but only
   * set the values of the relevant ones.
   */
  int nvnorm = nvert;
  XYZ *vnorms = (XYZ *)calloc(nvnorm, sizeof(XYZ));
  if (!vnorms) {
    _s2error("s2funuv", "failed to allocate memory for normals");
  }
  int vnormn = 0;
  XYZ norms[6];
  int rowstart; /* first triangle in this row */
  int prerowstart; /* first triangle in previous row (row "above") */
  for (i = 0; i < nvertx; i++) {

    /* note second line below is stupid for i = 0, but further down
     * we don't bother storing values for i = 0 anyway */
    rowstart = i * (nverty-1) * 2;
    prerowstart = (i - 1) * (nverty-1) * 2;

    for (j = 0; j < nverty; j++) {

      if ((i != 0) && (i != nvertx - 1) &&
	  (j != 0) && (j != nverty - 1)) {

	norms[0] = trgls[rowstart + j * 2 - 2].c;
	norms[1] = trgls[rowstart + j * 2 - 1].c;
	norms[2] = trgls[rowstart + j * 2    ].c;
	
	norms[3] = trgls[prerowstart + j * 2 - 1].c;
	norms[4] = trgls[prerowstart + j * 2    ].c;
	norms[5] = trgls[prerowstart + j * 2 + 1].c;
	
	vnorms[vnormn] = Centroid(norms, 6);
	Normalise(&(vnorms[vnormn]));
      }

      vnormn++;
    }
  }
  if (vnormn != nvnorm) {
    _s2error("s2funuv", "internal error");
  }

  /* fourth pass: add vertex normals to triangles */
  trgln = 0;
  for (i = 0; i < (nvertx - 1); i++) {
    for (j = 0; j < (nverty - 1); j++) {
      /* first triangle derived at this vertex */
      trgls[trgln].n[0] = vnorms[i     * nverty + j    ];
      trgls[trgln].n[1] = vnorms[i     * nverty + j + 1];
      trgls[trgln].n[2] = vnorms[(i+1) * nverty + j    ];
      trgln++;
      /* second triangle derived at this vertex */
      trgls[trgln].n[0] = vnorms[i *     nverty + j + 1];
      trgls[trgln].n[1] = vnorms[(i+1) * nverty + j + 1];
      trgls[trgln].n[2] = vnorms[(i+1) * nverty + j    ];
      trgln++;
    }
  }      
  
  /* fifth pass: draw the triangles, adding in colour information */
  trgln = 0;
  COLOUR col[3];
  col[0].r = 0.3; col[0].g = 0.5; col[0].b = 0.2;
  col[1].r = 0.3; col[1].g = 0.5; col[1].b = 0.2;
  col[2].r = 0.3; col[2].g = 0.5; col[2].b = 0.2;
  float alf[3];

  for (i = 0; i < nvertx - 1; i++) {
    for (j = 0; j < nverty - 1; j++) {
      /* first triangle derived at this vertex */
      col[0] = colours[i     * nverty + j    ];
      col[1] = colours[i     * nverty + j + 1];
      col[2] = colours[(i+1) * nverty + j    ];
      alf[0] = alphas[i     * nverty + j    ];
      alf[1] = alphas[i     * nverty + j + 1];
      alf[2] = alphas[(i+1) * nverty + j    ];
      if ((i > 0) && (i < nvertx - 2) &&
	  (j > 0) && (j < nverty - 2)) {
	if (ialphafn) {
	  ns2vf3nca(trgls[trgln].p, trgls[trgln].n, col, trans, alf);
	} else {
	  ns2Vf3nc(trgls[trgln].p, trgls[trgln].n, col);
	}
      }
      trgln++;

      /* second triangle derived at this vertex */
      col[0] = colours[i *     nverty + j + 1];
      col[1] = colours[(i+1) * nverty + j + 1];
      col[2] = colours[(i+1) * nverty + j    ];
      if ((i > 0) && (i < nvertx - 2) &&
	  (j > 0) && (j < nverty - 2)) {
	if (ialphafn) {
	  ns2vf3nca(trgls[trgln].p, trgls[trgln].n, col, trans, alf);
	} else {
	  ns2Vf3nc(trgls[trgln].p, trgls[trgln].n, col);
	}
      }
      trgln++;
    }
  }      
  
  free(vnorms);
  free(trgls);
  free(colours);
  free(vertices);
}

/* simple comparison functino for sorting vertices by x */
int XYZCompare(const void *v1, const void *v2) {
  XYZ *p1,*p2;
  p1 = (XYZ *)v1;
  p2 = (XYZ *)v2;
  if (p1->x < p2->x)
    return(-1);
  else if (p1->x > p2->x)
    return(1);
  else
    return(0);
}

/* create a red-X bitmap for texture use */
BITMAP4 *_s2priv_redXtexture(int w, int h) {
  BITMAP4 *ptr;
  int i, j, idx;
  if ((ptr = (BITMAP4 *)malloc(w * h * sizeof(BITMAP4))) == NULL) {
    _s2error("(internal)", "failed to allocate internal texture memory");
  }
  idx = 0;

  for (j = 0; j < h; j++) {

    // black bg, + blue + yellow borders
    for (i = 0; i < w; i++) {      
      ptr[idx].r = 0;
      ptr[idx].g = 0;
      ptr[idx].b = 0;
      ptr[idx].a = 160;
      //ptr[idx].r = 255;
      if ((i == 1) || (i == (w-1-1))) {
	ptr[idx].g = 255;
	ptr[idx].r = 255;
      }
      if ((j == 1) || (j == (h-1-1))) {
	ptr[idx].b = 255;
	ptr[idx].g = 255;
      }
      idx++;
    }

    // red x
    i = (float)j / (float)(h-1) * (float)(w-1);
    i = (i < 0) ? 0 : i;
    i = (i > w-1) ? w-1 : i;
    ptr[(j * w + i)].r = 255;
    ptr[(j * w + (w-1)-i)].r = 255;

  }

  return ptr;
}
/* 3d version */
BITMAP4 *_s2priv_redXtexture3d(int w, int h, int d) {
  BITMAP4 *ptr;
  int i, j, k, idx;
  if ((ptr = (BITMAP4 *)malloc(w * h * d * sizeof(BITMAP4))) == NULL) {
    _s2error("(internal)", "failed to allocated internal texture memory");
  }
  idx = 0;

  for (k = 0; k < d; k++) {
  
    for (j = 0; j < h; j++) {

      // black bg, + blue + yellow borders
      for (i = 0; i < w; i++) {      
	ptr[idx].r = ptr[idx].g = ptr[idx].b = 0;
	ptr[idx].r = 255;
	ptr[idx].a = 160;
	if ((i == 1) || (i == (w-1-1))) {
	  ptr[idx].g = 255;
	  ptr[idx].r = 255;
	}
	if ((j == 1) || (j == (h-1-1))) {
	  ptr[idx].b = 255;
	  ptr[idx].g = 255;
	}
	if ((k == 1) || (k == (d-1-1))) {
	  ptr[idx].b = 255;
	  ptr[idx].g = 255;
	}
	idx++;
      }
      
      // red x ???!?!?!?
      i = (float)j / (float)(h-1) * (float)(w-1);
      i = (i < 0) ? 0 : i;
      i = (i > w-1) ? w-1 : i;
      ptr[(j * w + i)].r = 255;
      ptr[(j * w + (w-1)-i)].r = 255;
      
    }
  }
  return ptr;
}

#if defined(IGNORE_S2COMMON_C)
/*
	Set the camera to the "home" position (mode == 1)
	Focus the camera to the center of the geometry (mode = 2)
	Move to a "axis aligned" position (mode 3..8)
*/
void CameraHome(int mode) {
  
  double size;
  
  // Stop any forward movement 
  camera.speed = 0;
  
  // Find the bounds of the geometry 
  CalcBounds();
  
  size = MAX((pmax.x-pmin.x),(pmax.y-pmin.y));
  size = MAX(size,(pmax.z-pmin.z));
#if defined(BUILDING_S2PLOT)
  /* addition dbarnes for S2PLOT: simply suggest the geometry is
   * larger than it really is! */
  size = sqrt(2.0) * size;
#endif
  
  // Set the camera attributes 
  camera.pr = pmid;
  switch (mode) {
  case HOME:
    camera.focallength = 1.75*size;
    camera.aperture = 45;
    camera.eyesep = size / 20.0;
    camera.vp = pmid;
    camera.vp.z += 1.75*size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  case FOCUS:
    break;
  case FRONT:
    camera.vp = pmid;
    camera.vp.z += 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  case BACK:
    camera.vp = pmid;
    camera.vp.z -= 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  case LEFT:
    camera.vp = pmid;
    camera.vp.x -= 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  case RIGHT:
    camera.vp = pmid;
    camera.vp.x += 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  case TOP:
    camera.vp = pmid;
    camera.vp.y += 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 0; camera.vu.z = 1;
    break;
  case BOTTOM:
    camera.vp = pmid;
    camera.vp.y -= 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 0; camera.vu.z = 1;
    break;
  case AXONOMETRIC:
    camera.vp = pmid;
    camera.vp.x -= size;
    camera.vp.y += size;
    camera.vp.z -= size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  }
  
  // Point the camera toward the mid point of the model 
  camera.vd.x = pmid.x - camera.vp.x;
  camera.vd.y = pmid.y - camera.vp.y;
  camera.vd.z = pmid.z - camera.vp.z;
  
  // Update the focal point 
  Normalise(&camera.vd);
  camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
  camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
  camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
  
  if (0 || options.debug) {
    fprintf(stderr,"Bounds = (%g,%g,%g) -> (%g,%g,%g)\n",
	    pmin.x,pmin.y,pmin.z,pmax.x,pmax.y,pmax.z);
    fprintf(stderr,"Midpoint = (%g,%g,%g)\n",pmid.x,pmid.y,pmid.z);
  }
}

/*
	Rotate (ix,iy) or roll (iz) the camera.
	Depends on the interaction mode.
	Correctly update all camera attributes
*/
void RotateCamera(double ix,double iy,double iz,int source) {
  fprintf(stdout, "HERE IN ROTATE CAMERA\n");
  XYZ vp,vu,vd;
  XYZ right;
  XYZ newvp,newr;
  double delta,radius;
  double dx,dy,dz;
  
  vu = camera.vu;
  Normalise(&vu);
  vp = camera.vp;
  vd = camera.vd;
  Normalise(&vd);
  CROSSPROD(vd,vu,right);
  Normalise(&right);
  
  fprintf(stderr, "Camera pos = %f, %f, %f\n", camera.vp.x, camera.vp.y, camera.vp.z);


  /* Determine the amount of change */
#if defined(BUILDING_S2PLOT)
  delta = ss2qcs() * DTOR;
#elif defined(BUILDING_VIEWER)
  delta = options.interactspeed * DTOR;
#endif
  if (source == MOUSECONTROL)
    delta /= 2;
  
  /* 
     Handle the roll 
     Just change the up vector
  */
  if (ABS(iz) > EPSILON) {
    camera.vu.x += iz * right.x * delta;
    camera.vu.y += iz * right.y * delta;
    camera.vu.z += iz * right.z * delta;
    Normalise(&camera.vu);
    return;
  }
  
  /*
    Fly mode
    Change the view direction vector along up or right vector
  */
  if (options.interaction == FLY) {
    camera.vd.x = vd.x + delta * ix * right.x + delta * iy * vu.x;
    camera.vd.y = vd.y + delta * ix * right.y + delta * iy * vu.y;
    camera.vd.z = vd.z + delta * ix * right.z + delta * iy * vu.z;
    Normalise(&camera.vd);
    
    /* Determine the new up vector */
    if (ABS(ix) > ABS(iy)) {
      CROSSPROD(camera.vd,camera.vu,right);
      Normalise(&camera.vu);
      CROSSPROD(right,camera.vd,camera.vu);
      Normalise(&camera.vu);
    } else if (ABS(ix) < ABS(iy)) {
      CROSSPROD(right,camera.vd,camera.vu);
      Normalise(&camera.vu);
    }
    
    camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
    camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
    camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
    
    return;
  }
  
  /*
    Walk mode
    ix changes the view direction
    iy changes the height along the up vector
  */
  if (options.interaction == WALK) {
    camera.vd.x = vd.x + delta * ix * right.x;
    camera.vd.y = vd.y + delta * ix * right.y;
    camera.vd.z = vd.z + delta * ix * right.z;
    Normalise(&camera.vd);
    
    camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
    camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
    camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
    
    return;
  }
  
  /* 
     Inspect mode 
     Handle special case when the camera coincident with the rotate point
  */
  dx = camera.vp.x - camera.pr.x;
  dy = camera.vp.y - camera.pr.y;
  dz = camera.vp.z - camera.pr.z;
  radius = sqrt(dx*dx + dy*dy + dz*dz);
  if (ABS(radius) > 0.0001) {
    
    /* Determine the new view point */
    delta *= radius;
    newvp.x = vp.x + delta * ix * right.x + delta * iy * vu.x - camera.pr.x; 
    newvp.y = vp.y + delta * ix * right.y + delta * iy * vu.y - camera.pr.y;
    newvp.z = vp.z + delta * ix * right.z + delta * iy * vu.z - camera.pr.z;
    Normalise(&newvp);
    camera.vp.x = camera.pr.x + radius * newvp.x;
    camera.vp.y = camera.pr.y + radius * newvp.y;
    camera.vp.z = camera.pr.z + radius * newvp.z;
    
    /* Determine the new right vector */
    newr.x = camera.vp.x + right.x - camera.pr.x;
    newr.y = camera.vp.y + right.y - camera.pr.y;
    newr.z = camera.vp.z + right.z - camera.pr.z;
    Normalise(&newr);
    newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
    newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
    newr.z = camera.pr.z + radius * newr.z - camera.vp.z;
    Normalise(&newr);
    
    camera.vd.x = camera.pr.x - camera.vp.x;
    camera.vd.y = camera.pr.y - camera.vp.y;
    camera.vd.z = camera.pr.z - camera.vp.z;
    Normalise(&camera.vd);
    
    /* Determine the new up vector */
    CROSSPROD(newr,camera.vd,camera.vu);
    Normalise(&camera.vu);
    
  } else {		/* Rotating about the camera position */
    
    // Calculate the new view direction 
    camera.vd.x += delta * ix * right.x + delta * iy * vu.x;
    camera.vd.y += delta * ix * right.y + delta * iy * vu.y;
    camera.vd.z += delta * ix * right.z + delta * iy * vu.z;
    Normalise(&camera.vd);
    
    // Determine the new up vector 
    CROSSPROD(right,camera.vd,camera.vu);
    Normalise(&camera.vu);
    
  }
  
  /* Update the focal point */
  Normalise(&camera.vd);
  camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
  camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
  camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;

}

/*
	Fly the camera forward or backward at a given speed
	In INSPECT mode we just get shifted in increments.
	In WALK and FLY mode it is continuous
*/
void FlyCamera(double speed)
{
#if defined(BUILDING_S2PLOT) 
  if (!_s2_transcam_enable) {
    return;
  }
#endif
	double delta;
	XYZ vp;
	
	/* Determine the amount to move */
	if (options.deltamove <= 0)
		delta = speed * VectorLength(pmin,pmax) / 2000;
	else
		delta = speed * options.deltamove;

#if defined(BUILDING_S2PLOT)
	// check we don't move through "the origin" = focus point
	XYZ newvp;
	newvp.x = camera.vp.x + delta * camera.vd.x;
	newvp.y = camera.vp.y + delta * camera.vd.y;
	newvp.z = camera.vp.z + delta * camera.vd.z;
	if (DotProduct(VectorSub(newvp, camera.pr), 
		       VectorSub(camera.vp, camera.pr)) <= 0) {
	  // don't allow this to happen
	  return;
	}
#endif

	/* Move the camera */
   vp.x = camera.vp.x + delta * camera.vd.x;
   vp.y = camera.vp.y + delta * camera.vd.y;
   vp.z = camera.vp.z + delta * camera.vd.z;

	/* Update the permanent settings */
	camera.vp = vp;
   if (options.projectiontype == PERSPECTIVE) {
      camera.pr.x += delta * camera.vd.x;
      camera.pr.y += delta * camera.vd.y;
      camera.pr.z += delta * camera.vd.z;
   }

	/* Update the focal distance */
   camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
   camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
   camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;

   if (options.interaction == INSPECT) {
     CameraHome(FOCUS);
   }
}

/*
	Calculate the model bounds
	Update pmin,pmax,pmid
	Optionally update rangemin,rangemax
*/
void CalcBounds(void) {
  int i;
  XYZ p;

#if defined(BUILDING_S2PLOT)
  pmin.x = _s2devicemin[_S2XAX];
  pmax.x = _s2devicemax[_S2XAX];
  pmin.y = _s2devicemin[_S2YAX];
  pmax.y = _s2devicemax[_S2YAX];
  pmin.z = _s2devicemin[_S2ZAX];
  pmax.z = _s2devicemax[_S2ZAX];
#else
  pmin.x = 1e32;  pmin.y = 1e32;  pmin.z = 1e32;
  pmax.x = -1e32; pmax.y = -1e32; pmax.z = -1e32;
#endif

  rangemin = 1e32;
  rangemax = -1e32;
  
  for (i=0;i<ndot;i++) {
#if defined(BUILDING_S2PLOT) 
    if (!strlen(dot[i].whichscreen))
#endif
      UpdateBounds(dot[i].p);
  }
  for (i=0;i<nball;i++) {
    p.x = ball[i].p.x + ball[i].r;
    p.y = ball[i].p.y + ball[i].r;
    p.z = ball[i].p.z + ball[i].r;
#if defined(BUILDING_S2PLOT)
    if (!strlen(ball[i].whichscreen))
#endif
      UpdateBounds(p);
    p.x = ball[i].p.x - ball[i].r;
    p.y = ball[i].p.y - ball[i].r;
    p.z = ball[i].p.z - ball[i].r;
#if defined(BUILDING_S2PLOT)
    if (!strlen(ball[i].whichscreen))
#endif
      UpdateBounds(p);
  }
  for (i=0;i<nballt;i++) {
    p.x = ballt[i].p.x + ballt[i].r;
    p.y = ballt[i].p.y + ballt[i].r;
    p.z = ballt[i].p.z + ballt[i].r;
#if defined(BUILDING_S2PLOT)
    if (!strlen(ballt[i].whichscreen))
#endif
      UpdateBounds(p);
    p.x = ballt[i].p.x - ballt[i].r;
    p.y = ballt[i].p.y - ballt[i].r;
    p.z = ballt[i].p.z - ballt[i].r;
#if defined(BUILDING_S2PLOT)
    if (!strlen(ballt[i].whichscreen))
#endif
      UpdateBounds(p);
  }
  for (i=0;i<ndisk;i++) {
    p.x = disk[i].p.x + disk[i].r2;
    p.y = disk[i].p.y + disk[i].r2;
    p.z = disk[i].p.z + disk[i].r2;
#if defined(BUILDING_S2PLOT)
    if (!strlen(disk[i].whichscreen))
#endif
      UpdateBounds(p);
    p.x = disk[i].p.x - disk[i].r2;
    p.y = disk[i].p.y - disk[i].r2;
    p.z = disk[i].p.z - disk[i].r2;
#if defined(BUILDING_S2PLOT)
    if (!strlen(disk[i].whichscreen))
#endif
      UpdateBounds(p);
  }
  for (i=0;i<ncone;i++) {
    p.x = cone[i].p1.x + cone[i].r1;
    p.y = cone[i].p1.y + cone[i].r1;
    p.z = cone[i].p1.z + cone[i].r1;
#if defined(BUILDING_S2PLOT)
    if (!strlen(cone[i].whichscreen))
#endif
      UpdateBounds(p);
    p.x = cone[i].p1.x - cone[i].r1;
    p.y = cone[i].p1.y - cone[i].r1;
    p.z = cone[i].p1.z - cone[i].r1;
#if defined(BUILDING_S2PLOT)
    if (!strlen(cone[i].whichscreen))
#endif
      UpdateBounds(p);
    
    p.x = cone[i].p2.x + cone[i].r2;
    p.y = cone[i].p2.y + cone[i].r2;
    p.z = cone[i].p2.z + cone[i].r2;
#if defined(BUILDING_S2PLOT)
    if (!strlen(cone[i].whichscreen))
#endif
      UpdateBounds(p);
    p.x = cone[i].p2.x - cone[i].r2;
    p.y = cone[i].p2.y - cone[i].r2;
    p.z = cone[i].p2.z - cone[i].r2;
#if defined(BUILDING_S2PLOT)
    if (!strlen(cone[i].whichscreen))
#endif
      UpdateBounds(p);
  }
  for (i=0;i<nline;i++) {
#if defined(BUILDING_S2PLOT)
    if (!strlen(line[i].whichscreen)) {
#endif
      UpdateBounds(line[i].p[0]);
      UpdateBounds(line[i].p[1]);
#if defined(BUILDING_S2PLOT)
    }
#endif
  }
  for (i=0;i<nface3;i++) {
#if defined(BUILDING_S2PLOT)
    if (!strlen(face3[i].whichscreen)) {
#endif
      UpdateBounds(face3[i].p[0]);
      UpdateBounds(face3[i].p[1]);
      UpdateBounds(face3[i].p[2]);
#if defined(BUILDING_S2PLOT)
    }
#endif
  }
#if defined(BUILDING_S2PLOT)
  for (i=0;i<nface3a;i++) {
    if (!strlen(face3a[i].whichscreen)) {
      UpdateBounds(face3a[i].p[0]);
      UpdateBounds(face3a[i].p[1]);
      UpdateBounds(face3a[i].p[2]);
    }
  }
#if defined(S2_3D_TEXTURES)
  for (i=0;i<ntexpoly3d;i++) {
    if (!strlen(texpoly3d[i].whichscreen)) {
      int j;
      for (j=0; j<texpoly3d[i].nverts;j++) {
	UpdateBounds(texpoly3d[i].verts[j]);
      }
    }
  }
#endif
  for (i = 0; i < ntexmesh; i++) {
    if (!strlen(texmesh[i].whichscreen)) {
      int j;
      for (j = 0; j < texmesh[i].nverts; j++) {
	UpdateBounds(texmesh[i].verts[j]);
      }
    }
  }
#endif
  for (i=0;i<nface4;i++) {
#if defined(BUILDING_S2PLOT)
    if (!strlen(face4[i].whichscreen)) {
#endif
      UpdateBounds(face4[i].p[0]);
      UpdateBounds(face4[i].p[1]);
      UpdateBounds(face4[i].p[2]);
      UpdateBounds(face4[i].p[3]);
#if defined(BUILDING_S2PLOT)
    }
#endif
  }
  for (i=0;i<nface4t;i++) {
#if defined(BUILDING_S2PLOT)
    if (!strlen(face4t[i].whichscreen)) {
#endif
      UpdateBounds(face4t[i].p[0]);
      UpdateBounds(face4t[i].p[1]);
      UpdateBounds(face4t[i].p[2]);
      UpdateBounds(face4t[i].p[3]);
#if defined(BUILDING_S2PLOT)
    }
#endif
  }

#if defined(BUILDING_S2PLOT)
  for (i = 0; i < ntrdot; i++) {
    if (!strlen(trdot[i].whichscreen)) {
      UpdateBounds(trdot[i].p);
    }
  }
#endif

  for (i=0;i<nlabel;i++)
#if defined(BUILDING_S2PLOT)
    if (!strlen(label[i].whichscreen)) 
#endif
      UpdateBounds(label[i].p);
#if defined(BUILDING_S2PLOT)
  for (i = 0; i < nlabel; i++) {
    if (!strlen(label[i].whichscreen)) {
      UpdateBounds(VectorAdd(VectorAdd(label[i].p, label[i].up), 
			     VectorMul(label[i].right, 
				       (double)strlen(label[i].s))));
    }
  }
#endif
  
  if (pmax.x <= pmin.x) { pmax.x = pmin.x; pmax.x += 0.01; pmin.x -= 0.01; }
  if (pmax.y <= pmin.y) { pmax.y = pmin.y; pmax.y += 0.01; pmin.y -= 0.01; }
  if (pmax.z <= pmin.z) { pmax.z = pmin.z; pmax.z += 0.01; pmin.z -= 0.01; }
  if (rangemin >= rangemax) { 
    rangemax = rangemin; 
    rangemin -= 0.01; 
    rangemax += 0.01; 
  }
  
  /* The center of the model */
#if defined(BUILDING_S2PLOT)
  if (!_s2_camexfocus) {
#endif
    pmid.x = (pmin.x + pmax.x) / 2;
    pmid.y = (pmin.y + pmax.y) / 2;
    pmid.z = (pmin.z + pmax.z) / 2;
#if defined(BUILDING_S2PLOT)
  } else {
    pmid = _s2_camfocus;
  }
#endif
  
  //fprintf(stderr, "in CalcBounds: pmin,pmax = (%f,%f,%f), (%f,%f,%f)\n", 
  //	  pmin.x, pmin.y, pmin.z, pmax.x, pmax.y, pmax.z);



  if (0 && options.debug) {
    fprintf(stderr,"Range: %g to %g\n",rangemin,rangemax);
  }
}

void UpdateBounds(XYZ p)
{
	double dist;

   pmin.x = MIN(pmin.x,p.x);
   pmin.y = MIN(pmin.y,p.y);
   pmin.z = MIN(pmin.z,p.z);
   pmax.x = MAX(pmax.x,p.x);
   pmax.y = MAX(pmax.y,p.y);
   pmax.z = MAX(pmax.z,p.z);
	
	dist = VectorLength(p,camera.vp);
	rangemin = MIN(rangemin,dist);
	rangemax = MAX(rangemax,dist);
}
#else
#include "s2common.c"
#endif

/* enable screen lists */
void _s2_startScreenGeometry(int erase) {
  _s2_screenEnabled = 1;
}

/* disable screen lists */
void _s2_endScreenGeometry() {
  _s2_screenEnabled = 0;
}


void _s2_clearGeometryList() {
  int i;
  if (nball) {
    free(ball);
    ball = NULL;
    nball = 0;
  }

  if (nballt) {	
    for (i = 0; i < nballt; i++) {
      /* only delete the texture if it is not called "<cached>" */
      if (strcmp(ballt[i].texturename, "<cached>")) {
	//glDeleteTextures(1,&(ballt[i].textureid));
		  _s2priv_dropTexture(ballt[i].textureid); //wasGL
	if (ballt[i].rgba != NULL) {
	  free(ballt[i].rgba);
	  ballt[i].rgba = NULL;
	}
      }
    }
    free(ballt);
    ballt = NULL;
    nballt = 0;
  }

  if (ndisk) {
    free(disk);
    disk = NULL;
    ndisk = 0;
  }

  if (ncone) {
    free(cone);
    cone = NULL;
    ncone = 0;
  }

  if (ndot) {
    free(dot);
    dot = NULL;
    ndot = 0;
  }

  if (nline) {
    free(line);
    line = NULL;
    nline = 0;
  }

  if (nface3) {
    free(face3);
    face3 = NULL;
    nface3 = 0;
  }

  if (nface3a) {
    free(face3a);
    face3a = NULL;
    nface3a = 0;
  }

#if defined(S2_3D_TEXTURES)
  if (ntexpoly3d) {
    for (i = 0; i < ntexpoly3d; i++) {
      if (texpoly3d[i].verts) {
	free(texpoly3d[i].verts);
      }
      if (texpoly3d[i].texcoords) {
	free(texpoly3d[i].texcoords);
      }
    }
    free(texpoly3d);
    texpoly3d = NULL;
    ntexpoly3d = 0;
  }
#endif

  if (ntexmesh) {
    for (i = 0; i < ntexmesh; i++) {
      if (texmesh[i].verts) {
	free(texmesh[i].verts);
      }
      if (texmesh[i].norms) {
	free(texmesh[i].norms);
      }
      if (texmesh[i].vtcs) {
	free(texmesh[i].vtcs);
      }
      if (texmesh[i].facets) {
	free(texmesh[i].vtcs);
      }
      if (texmesh[i].facets_vtcs) {
	free(texmesh[i].facets_vtcs);
      }
    }
    free(texmesh);
    texmesh = NULL;
    ntexmesh = 0;
  }

  if (nface4) {
    free(face4);
    face4 = NULL;
    nface4 = 0;
  }

  if (nface4t) {
    for (i = 0; i < nface4t; i++) {
      /* only delete the texture if it is not called "<cached>" */
      if (strcmp(face4t[i].texturename, "<cached>")) {
	//glDeleteTextures(1,&(face4t[i].textureid));
		  _s2priv_dropTexture(face4t[i].textureid); // wasGL
	if (face4t[i].rgba != NULL) {
	  free(face4t[i].rgba);
	  face4t[i].rgba = NULL;
	}
      }
    }
    free(face4t);
    face4t = NULL;
    nface4t = 0;
  }

  if (nlabel) {
    free(label);
    label = NULL;
    nlabel = 0;
  }

  if (nhandle) {
    free(handle);
    handle = NULL;
    nhandle = 0;
  }

  if (nbboard) {
    free(bboard);
    bboard = NULL;
    nbboard = 0;
  }

  if (nbbset) {
    free(bbset);
    bbset = NULL;
    nbbset = 0;
  }

  if (ntrdot) {
    free(trdot);
    trdot = NULL;
    ntrdot = 0;
  }

}

/* enable dynamic lists */
void _s2_startDynamicGeometry(int erase) {

  if (_s2_dynamicEnabled) {
    fprintf(stderr, "_s2_startDynamicGeometry INVALID * * * * * SHOULD NEVER HAPPEN! ARGH !!! * * * *\n");
  }


  /* copy current object counts and pointers to static list globals, 
   * and then copy dynamic list globals and counts to current.
   */

  nball_s = nball;
  ball_s = ball;
  nball = nball_d;
  ball = ball_d;
  
  nballt_s = nballt;
  ballt_s = ballt;
  nballt = nballt_d;
  ballt = ballt_d;

  ndisk_s = ndisk;
  disk_s = disk;
  ndisk = ndisk_d;
  disk = disk_d;

  ncone_s = ncone;
  cone_s = cone;
  ncone = ncone_d;
  cone = cone_d;

  ndot_s = ndot;
  dot_s = dot;
  ndot = ndot_d;
  dot = dot_d;
  
  nline_s = nline;
  line_s = line;
  nline = nline_d;
  line = line_d;
  
  nface3_s = nface3;
  face3_s = face3;
  nface3 = nface3_d;
  face3 = face3_d;

  nface3a_s = nface3a;
  face3a_s = face3a;
  nface3a = nface3a_d;
  face3a = face3a_d;

  nface4_s = nface4;
  face4_s = face4;
  nface4 = nface4_d;
  face4 = face4_d;

  nface4t_s = nface4t;
  face4t_s = face4t;
  nface4t = nface4t_d;
  face4t = face4t_d;

  nlabel_s = nlabel;
  label_s = label;
  nlabel = nlabel_d;
  label = label_d;

  nhandle_s = nhandle;
  handle_s = handle;
  nhandle = nhandle_d;
  handle = handle_d;
  
  nbboard_s = nbboard;
  bboard_s = bboard;
  nbboard = nbboard_d;
  bboard = bboard_d;
  
  nbbset_s = nbbset;
  bbset_s = bbset;
  nbbset = nbbset_d;
  bbset = bbset_d;
  
  ntrdot_s = ntrdot;
  trdot_s = trdot;
  ntrdot = ntrdot_d;
  trdot = trdot_d;  
  
#if defined(S2_3D_TEXTURES)
  ntexpoly3d_s = ntexpoly3d;
  texpoly3d_s = texpoly3d;
  ntexpoly3d = ntexpoly3d_d;
  texpoly3d = texpoly3d_d;
#endif

  ntexmesh_s = ntexmesh;
  texmesh_s = texmesh;
  ntexmesh = ntexmesh_d;
  texmesh = texmesh_d;

  if (erase) {
    _s2_clearGeometryList();
  }

  _s2_dynamicEnabled = 1;
}

/* enable static lists */
void _s2_endDynamicGeometry() {

  if (!_s2_dynamicEnabled) {
    fprintf(stderr, "_s2_endDynamicGeometry INVALID * * * * * SHOULD NEVER HAPPEN! ARGH !!! * * * *\n");
  }

  nball_d = nball;
  ball_d = ball;
  nball = nball_s;
  ball = ball_s;

  nballt_d = nballt;
  ballt_d = ballt;
  nballt = nballt_s;
  ballt = ballt_s;

  ndisk_d = ndisk;
  disk_d = disk;
  ndisk = ndisk_s;
  disk = disk_s;

  ncone_d = ncone;
  cone_d = cone;
  ncone = ncone_s;
  cone = cone_s;
  
  ndot_d = ndot;
  dot_d = dot;
  ndot = ndot_s;
  dot = dot_s;
  
  nline_d = nline;
  line_d = line;
  nline = nline_s;
  line = line_s;
  
  nface3_d = nface3;
  face3_d = face3;
  nface3 = nface3_s;
  face3 = face3_s;

  nface3a_d = nface3a;
  face3a_d = face3a;
  nface3a = nface3a_s;
  face3a = face3a_s;

  nface4_d = nface4;
  face4_d = face4;
  nface4 = nface4_s;
  face4 = face4_s;
  
  nface4t_d = nface4t;
  face4t_d = face4t;
  nface4t = nface4t_s;
  face4t = face4t_s;
  
  nlabel_d = nlabel;
  label_d = label;
  nlabel = nlabel_s;
  label = label_s;
  
  nhandle_d = nhandle;
  handle_d = handle;
  nhandle = nhandle_s;
  handle = handle_s;
  
  nbboard_d = nbboard;
  bboard_d = bboard;
  nbboard = nbboard_s;
  bboard = bboard_s;

  nbbset_d = nbbset;
  bbset_d = bbset;
  nbbset = nbbset_s;
  bbset = bbset_s;

  ntrdot_d = ntrdot;
  trdot_d = trdot;
  ntrdot = ntrdot_s;
  trdot = trdot_s;
  
#if defined(S2_3D_TEXTURES)
  ntexpoly3d_d = ntexpoly3d;
  texpoly3d_d = texpoly3d;
  ntexpoly3d = ntexpoly3d_s;
  texpoly3d = texpoly3d_s;
#endif

  ntexmesh_d = ntexmesh;
  texmesh_d = texmesh;
  ntexmesh = ntexmesh_s;
  texmesh = texmesh_s;

  _s2_dynamicEnabled = 0;
}

void _s2priv_pushGlobalsToPanel(int panelid) {
  // internal function: no need to check panelid ... !!! ???
  S2PLOT_PANEL *it = _s2_panels + panelid;
  bcopy(_s2devicemin, it->devicemin, 3 * sizeof(float));
  bcopy(_s2devicemax, it->devicemax, 3 * sizeof(float));
  bcopy(_s2axismin, it->axismin, 3 * sizeof(float));
  bcopy(_s2axismax, it->axismax, 3 * sizeof(float));
  bcopy(_s2axissgn, it->axissgn, 3 * sizeof(float));

  it->clipping = _s2_clipping;

  //bcopy(&camera, &(it->camera), sizeof(CAMERA));
  bcopy(&camera, (it->camera), sizeof(CAMERA));

  bcopy(&pmin, &(it->pmin), sizeof(XYZ));
  bcopy(&pmax, &(it->pmax), sizeof(XYZ));
  bcopy(&pmid, &(it->pmid), sizeof(XYZ));
  bcopy(&rangemin, &(it->rangemin), sizeof(rangemin));
  bcopy(&rangemax, &(it->rangemax), sizeof(rangemax));

  //bcopy(&(options.autospin), &(it->autospin), sizeof(XYZ));
  bcopy(&(options.autospin), (it->autospin), sizeof(XYZ));
  bcopy(&(options.camerahome), &(it->camerahome), sizeof(CAMERA));

  // callbacks:
  it->callback = _s2_callback;
  it->callbackx = _s2_callbackx;
  it->callbackx_data = _s2_callbackx_data;
  it->userkeys = _s2_user_keys;
  it->numcb = _s2_numcb;
  it->oglcb = _s2_oglcb;
  it->remcb = _s2_remcb;
  it->handlecallback = _s2_handlecallback;
  it->draghandlecb = _s2_draghandle_callback;
  it->promptcb = _s2_promptcbx;
  it->promptcb_data = _s2_promptcbx_data;
  strcpy(it->baseprompt, _s2baseprompt);
  it->prompt_x = _s2prompt_x;
  it->prompt_y = _s2prompt_y;

  // drag state
  bcopy(_s2_dragmodel, it->dragmodel, 16 * sizeof(double)); // wasGL
  bcopy(_s2_dragproj, it->dragproj, 16 * sizeof(double)); // wasGL
  bcopy(_s2_dragview, it->dragview, 4 * sizeof(int)); // wasGL

  // GL_listindex is not done - it is handled by direct access

  /* "current" geometry */
  bcopy(&nball, &(it->nball), sizeof(int));
  bcopy(&nballt, &(it->nballt), sizeof(int));
  bcopy(&ndisk, &(it->ndisk), sizeof(int));
  bcopy(&ncone, &(it->ncone), sizeof(int));
  bcopy(&ndot, &(it->ndot), sizeof(int));
  bcopy(&nline, &(it->nline), sizeof(int));
  bcopy(&nface3, &(it->nface3), sizeof(int));
  bcopy(&nface4, &(it->nface4), sizeof(int));
  bcopy(&nface4t, &(it->nface4t), sizeof(int));
  bcopy(&nlabel, &(it->nlabel), sizeof(int));
  bcopy(&nhandle, &(it->nhandle), sizeof(int));
  bcopy(&nbboard, &(it->nbboard), sizeof(int));
  bcopy(&nbbset, &(it->nbbset), sizeof(int));
  bcopy(&nface3a, &(it->nface3a), sizeof(int));
  bcopy(&ntrdot, &(it->ntrdot), sizeof(int));
#if defined(S2_3D_TEXTURES)
  bcopy(&ntexpoly3d, &(it->ntexpoly3d), sizeof(int));
#endif
  bcopy(&ntexmesh, &(it->ntexmesh), sizeof(int));

  bcopy(&ball, &(it->ball), sizeof(BALL *));
  bcopy(&ballt, &(it->ballt), sizeof(BALLT *));
  bcopy(&disk, &(it->disk), sizeof(DISK *));
  bcopy(&cone, &(it->cone), sizeof(CONE *));
  bcopy(&dot, &(it->dot), sizeof(DOT *));
  bcopy(&line, &(it->line), sizeof(LINE *));
  bcopy(&face3, &(it->face3), sizeof(FACE3 *));
  bcopy(&face4, &(it->face4), sizeof(FACE4 *));
  bcopy(&face4t, &(it->face4t), sizeof(FACE4T *));
  bcopy(&label, &(it->label), sizeof(LABEL *));
  bcopy(&handle, &(it->handle), sizeof(_S2HANDLE *));
  bcopy(&bboard, &(it->bboard), sizeof(_S2BBOARD *));
  bcopy(&bbset, &(it->bbset), sizeof(_S2BBSET *));
  bcopy(&face3a, &(it->face3a), sizeof(_S2FACE3A *));
  bcopy(&trdot, &(it->trdot), sizeof(TRDOT *));
#if defined(S2_3D_TEXTURES)
  bcopy(&texpoly3d, &(it->texpoly3d), sizeof(_S2TEXPOLY3D *));
#endif
  bcopy(&texmesh, &(it->texmesh), sizeof(_S2TEXTUREDMESH *));

  /* "static" geometry */
  bcopy(&nball_s, &(it->nball_s), sizeof(int));
  bcopy(&nballt_s, &(it->nballt_s), sizeof(int));
  bcopy(&ndisk_s, &(it->ndisk_s), sizeof(int));
  bcopy(&ncone_s, &(it->ncone_s), sizeof(int));
  bcopy(&ndot_s, &(it->ndot_s), sizeof(int));
  bcopy(&nline_s, &(it->nline_s), sizeof(int));
  bcopy(&nface3_s, &(it->nface3_s), sizeof(int));
  bcopy(&nface4_s, &(it->nface4_s), sizeof(int));
  bcopy(&nface4t_s, &(it->nface4t_s), sizeof(int));
  bcopy(&nlabel_s, &(it->nlabel_s), sizeof(int));
  bcopy(&nhandle_s, &(it->nhandle_s), sizeof(int));
  bcopy(&nbboard_s, &(it->nbboard_s), sizeof(int));
  bcopy(&nbbset_s, &(it->nbbset_s), sizeof(int));
  bcopy(&nface3a_s, &(it->nface3a_s), sizeof(int));
  bcopy(&ntrdot_s, &(it->ntrdot_s), sizeof(int));
#if defined(S2_3D_TEXTURES)
  bcopy(&ntexpoly3d_s, &(it->ntexpoly3d_s), sizeof(int));
#endif
  bcopy(&ntexmesh_s, &(it->ntexmesh_s), sizeof(int));

  bcopy(&ball_s, &(it->ball_s), sizeof(BALL *));
  bcopy(&ballt_s, &(it->ballt_s), sizeof(BALLT *));
  bcopy(&disk_s, &(it->disk_s), sizeof(DISK *));
  bcopy(&cone_s, &(it->cone_s), sizeof(CONE *));
  bcopy(&dot_s, &(it->dot_s), sizeof(DOT *));
  bcopy(&line_s, &(it->line_s), sizeof(LINE *));
  bcopy(&face3_s, &(it->face3_s), sizeof(FACE3 *));
  bcopy(&face4_s, &(it->face4_s), sizeof(FACE4 *));
  bcopy(&face4t_s, &(it->face4t_s), sizeof(FACE4T *));
  bcopy(&label_s, &(it->label_s), sizeof(LABEL *));
  bcopy(&handle_s, &(it->handle_s), sizeof(_S2HANDLE *));
  bcopy(&bboard_s, &(it->bboard_s), sizeof(_S2BBOARD *));
  bcopy(&bbset_s, &(it->bbset_s), sizeof(_S2BBSET *));
  bcopy(&face3a_s, &(it->face3a_s), sizeof(_S2FACE3A *));
  bcopy(&trdot_s, &(it->trdot_s), sizeof(TRDOT *));
#if defined(S2_3D_TEXTURES)
  bcopy(&texpoly3d_s, &(it->texpoly3d_s), sizeof(_S2TEXPOLY3D *));
#endif
  bcopy(&texmesh_s, &(it->texmesh_s), sizeof(_S2TEXTUREDMESH *));

  /* "dynamic" geometry */
  bcopy(&nball_d, &(it->nball_d), sizeof(int));
  bcopy(&nballt_d, &(it->nballt_d), sizeof(int));
  bcopy(&ndisk_d, &(it->ndisk_d), sizeof(int));
  bcopy(&ncone_d, &(it->ncone_d), sizeof(int));
  bcopy(&ndot_d, &(it->ndot_d), sizeof(int));
  bcopy(&nline_d, &(it->nline_d), sizeof(int));
  bcopy(&nface3_d, &(it->nface3_d), sizeof(int));
  bcopy(&nface4_d, &(it->nface4_d), sizeof(int));
  bcopy(&nface4t_d, &(it->nface4t_d), sizeof(int));
  bcopy(&nlabel_d, &(it->nlabel_d), sizeof(int));
  bcopy(&nhandle_d, &(it->nhandle_d), sizeof(int));
  bcopy(&nbboard_d, &(it->nbboard_d), sizeof(int));
  bcopy(&nbbset_d, &(it->nbbset_d), sizeof(int));
  bcopy(&nface3a_d, &(it->nface3a_d), sizeof(int));
  bcopy(&ntrdot_d, &(it->ntrdot_d), sizeof(int));
#if defined(S2_3D_TEXTURES)
  bcopy(&ntexpoly3d_d, &(it->ntexpoly3d_d), sizeof(int));
#endif
  bcopy(&ntexmesh_d, &(it->ntexmesh_d), sizeof(int));

  bcopy(&ball_d, &(it->ball_d), sizeof(BALL *));
  bcopy(&ballt_d, &(it->ballt_d), sizeof(BALLT *));
  bcopy(&disk_d, &(it->disk_d), sizeof(DISK *));
  bcopy(&cone_d, &(it->cone_d), sizeof(CONE *));
  bcopy(&dot_d, &(it->dot_d), sizeof(DOT *));
  bcopy(&line_d, &(it->line_d), sizeof(LINE *));
  bcopy(&face3_d, &(it->face3_d), sizeof(FACE3 *));
  bcopy(&face4_d, &(it->face4_d), sizeof(FACE4 *));
  bcopy(&face4t_d, &(it->face4t_d), sizeof(FACE4T *));
  bcopy(&label_d, &(it->label_d), sizeof(LABEL *));
  bcopy(&handle_d, &(it->handle_d), sizeof(_S2HANDLE *));
  bcopy(&bboard_d, &(it->bboard_d), sizeof(_S2BBOARD *));
  bcopy(&bbset_d, &(it->bbset_d), sizeof(_S2BBSET *));
  bcopy(&face3a_d, &(it->face3a_d), sizeof(_S2FACE3A *));
  bcopy(&trdot_d, &(it->trdot_d), sizeof(TRDOT *));
#if defined(S2_3D_TEXTURES)
  bcopy(&texpoly3d_d, &(it->texpoly3d_d), sizeof(_S2TEXPOLY3D *));
#endif
  bcopy(&texmesh_d, &(it->texmesh_d), sizeof(_S2TEXTUREDMESH *));

}

void _s2priv_pushPanelToGlobals(int panelid) {
  // internal function: no need to check panelid ... !!! ???
  S2PLOT_PANEL *it = _s2_panels + panelid;
  invbcopy(_s2devicemin, it->devicemin, 3 * sizeof(float));
  invbcopy(_s2devicemax, it->devicemax, 3 * sizeof(float));
  invbcopy(_s2axismin, it->axismin, 3 * sizeof(float));
  invbcopy(_s2axismax, it->axismax, 3 * sizeof(float));
  invbcopy(_s2axissgn, it->axissgn, 3 * sizeof(float));

  _s2_clipping = it->clipping;

  //invbcopy(&camera, &(it->camera), sizeof(CAMERA));
  invbcopy(&camera, (it->camera), sizeof(CAMERA));

  invbcopy(&pmin, &(it->pmin), sizeof(XYZ));
  invbcopy(&pmax, &(it->pmax), sizeof(XYZ));
  invbcopy(&pmid, &(it->pmid), sizeof(XYZ));
  invbcopy(&rangemin, &(it->rangemin), sizeof(rangemin));
  invbcopy(&rangemax, &(it->rangemax), sizeof(rangemax));

  //invbcopy(&(options.autospin), &(it->autospin), sizeof(XYZ));
  invbcopy(&(options.autospin), (it->autospin), sizeof(XYZ));
  invbcopy(&(options.camerahome), &(it->camerahome), sizeof(CAMERA));

  // callbacks:
  _s2_callback = it->callback;
  _s2_callbackx = it->callbackx;
  _s2_callbackx_data = it->callbackx_data;
  _s2_user_keys = it->userkeys;
  _s2_numcb = it->numcb;
  _s2_oglcb = it->oglcb;
  _s2_remcb = it->remcb;
  _s2_handlecallback = it->handlecallback;
  _s2_draghandle_callback = it->draghandlecb;
  _s2_promptcbx = it->promptcb;
  _s2_promptcbx_data = it->promptcb_data;
  strcpy(_s2baseprompt, it->baseprompt);
  _s2prompt_x = it->prompt_x;
  _s2prompt_y = it->prompt_y;


  // drag state
  invbcopy(_s2_dragmodel, it->dragmodel, 16 * sizeof(double)); // wasGL
  invbcopy(_s2_dragproj, it->dragproj, 16 * sizeof(double)); // wasGL
  invbcopy(_s2_dragview, it->dragview, 4 * sizeof(int)); // wasGL

  /* "current" geometry */
  invbcopy(&nball, &(it->nball), sizeof(int));
  invbcopy(&nballt, &(it->nballt), sizeof(int));
  invbcopy(&ndisk, &(it->ndisk), sizeof(int));
  invbcopy(&ncone, &(it->ncone), sizeof(int));
  invbcopy(&ndot, &(it->ndot), sizeof(int));
  invbcopy(&nline, &(it->nline), sizeof(int));
  invbcopy(&nface3, &(it->nface3), sizeof(int));
  invbcopy(&nface4, &(it->nface4), sizeof(int));
  invbcopy(&nface4t, &(it->nface4t), sizeof(int));
  invbcopy(&nlabel, &(it->nlabel), sizeof(int));
  invbcopy(&nhandle, &(it->nhandle), sizeof(int));
  invbcopy(&nbboard, &(it->nbboard), sizeof(int));
  invbcopy(&nbbset, &(it->nbbset), sizeof(int));
  invbcopy(&nface3a, &(it->nface3a), sizeof(int));
  invbcopy(&ntrdot, &(it->ntrdot), sizeof(int));
#if defined(S2_3D_TEXTURES)
  invbcopy(&ntexpoly3d, &(it->ntexpoly3d), sizeof(int));
#endif
  invbcopy(&ntexmesh, &(it->ntexmesh), sizeof(int));

  invbcopy(&ball, &(it->ball), sizeof(BALL *));
  invbcopy(&ballt, &(it->ballt), sizeof(BALLT *));
  invbcopy(&disk, &(it->disk), sizeof(DISK *));
  invbcopy(&cone, &(it->cone), sizeof(CONE *));
  invbcopy(&dot, &(it->dot), sizeof(DOT *));
  invbcopy(&line, &(it->line), sizeof(LINE *));
  invbcopy(&face3, &(it->face3), sizeof(FACE3 *));
  invbcopy(&face4, &(it->face4), sizeof(FACE4 *));
  invbcopy(&face4t, &(it->face4t), sizeof(FACE4T *));
  invbcopy(&label, &(it->label), sizeof(LABEL *));
  invbcopy(&handle, &(it->handle), sizeof(_S2HANDLE *));
  invbcopy(&bboard, &(it->bboard), sizeof(_S2BBOARD *));
  invbcopy(&bbset, &(it->bbset), sizeof(_S2BBSET *));
  invbcopy(&face3a, &(it->face3a), sizeof(_S2FACE3A *));
  invbcopy(&trdot, &(it->trdot), sizeof(TRDOT *));
#if defined(S2_3D_TEXTURES)
  invbcopy(&texpoly3d, &(it->texpoly3d), sizeof(_S2TEXPOLY3D *));
#endif
  invbcopy(&texmesh, &(it->texmesh), sizeof(_S2TEXTUREDMESH *));

  /* "static" geometry */
  invbcopy(&nball_s, &(it->nball_s), sizeof(int));
  invbcopy(&nballt_s, &(it->nballt_s), sizeof(int));
  invbcopy(&ndisk_s, &(it->ndisk_s), sizeof(int));
  invbcopy(&ncone_s, &(it->ncone_s), sizeof(int));
  invbcopy(&ndot_s, &(it->ndot_s), sizeof(int));
  invbcopy(&nline_s, &(it->nline_s), sizeof(int));
  invbcopy(&nface3_s, &(it->nface3_s), sizeof(int));
  invbcopy(&nface4_s, &(it->nface4_s), sizeof(int));
  invbcopy(&nface4t_s, &(it->nface4t_s), sizeof(int));
  invbcopy(&nlabel_s, &(it->nlabel_s), sizeof(int));
  invbcopy(&nhandle_s, &(it->nhandle_s), sizeof(int));
  invbcopy(&nbboard_s, &(it->nbboard_s), sizeof(int));
  invbcopy(&nbbset_s, &(it->nbbset_s), sizeof(int));
  invbcopy(&nface3a_s, &(it->nface3a_s), sizeof(int));
  invbcopy(&ntrdot_s, &(it->ntrdot_s), sizeof(int));
#if defined(S2_3D_TEXTURES)
  invbcopy(&ntexpoly3d_s, &(it->ntexpoly3d_s), sizeof(int));
#endif
  invbcopy(&ntexmesh_s, &(it->ntexmesh_s), sizeof(int));

  invbcopy(&ball_s, &(it->ball_s), sizeof(BALL *));
  invbcopy(&ballt_s, &(it->ballt_s), sizeof(BALLT *));
  invbcopy(&disk_s, &(it->disk_s), sizeof(DISK *));
  invbcopy(&cone_s, &(it->cone_s), sizeof(CONE *));
  invbcopy(&dot_s, &(it->dot_s), sizeof(DOT *));
  invbcopy(&line_s, &(it->line_s), sizeof(LINE *));
  invbcopy(&face3_s, &(it->face3_s), sizeof(FACE3 *));
  invbcopy(&face4_s, &(it->face4_s), sizeof(FACE4 *));
  invbcopy(&face4t_s, &(it->face4t_s), sizeof(FACE4T *));
  invbcopy(&label_s, &(it->label_s), sizeof(LABEL *));
  invbcopy(&handle_s, &(it->handle_s), sizeof(_S2HANDLE *));
  invbcopy(&bboard_s, &(it->bboard_s), sizeof(_S2BBOARD *));
  invbcopy(&bbset_s, &(it->bbset_s), sizeof(_S2BBSET *));
  invbcopy(&face3a_s, &(it->face3a_s), sizeof(_S2FACE3A *));
  invbcopy(&trdot_s, &(it->trdot_s), sizeof(TRDOT *));
#if defined(S2_3D_TEXTURES)
  invbcopy(&texpoly3d_s, &(it->texpoly3d_s), sizeof(_S2TEXPOLY3D *));
#endif
  invbcopy(&texmesh_s, &(it->texmesh_s), sizeof(_S2TEXTUREDMESH *));

  /* "dynamic" geometry */
  invbcopy(&nball_d, &(it->nball_d), sizeof(int));
  invbcopy(&nballt_d, &(it->nballt_d), sizeof(int));
  invbcopy(&ndisk_d, &(it->ndisk_d), sizeof(int));
  invbcopy(&ncone_d, &(it->ncone_d), sizeof(int));
  invbcopy(&ndot_d, &(it->ndot_d), sizeof(int));
  invbcopy(&nline_d, &(it->nline_d), sizeof(int));
  invbcopy(&nface3_d, &(it->nface3_d), sizeof(int));
  invbcopy(&nface4_d, &(it->nface4_d), sizeof(int));
  invbcopy(&nface4t_d, &(it->nface4t_d), sizeof(int));
  invbcopy(&nlabel_d, &(it->nlabel_d), sizeof(int));
  invbcopy(&nhandle_d, &(it->nhandle_d), sizeof(int));
  invbcopy(&nbboard_d, &(it->nbboard_d), sizeof(int));
  invbcopy(&nbbset_d, &(it->nbbset_d), sizeof(int));
  invbcopy(&nface3a_d, &(it->nface3a_d), sizeof(int));
  invbcopy(&ntrdot_d, &(it->ntrdot_d), sizeof(int));
#if defined(S2_3D_TEXTURES)
  invbcopy(&ntexpoly3d_d, &(it->ntexpoly3d_d), sizeof(int));
#endif
  invbcopy(&ntexmesh_d, &(it->ntexmesh_d), sizeof(int));

  invbcopy(&ball_d, &(it->ball_d), sizeof(BALL *));
  invbcopy(&ballt_d, &(it->ballt_d), sizeof(BALLT *));
  invbcopy(&disk_d, &(it->disk_d), sizeof(DISK *));
  invbcopy(&cone_d, &(it->cone_d), sizeof(CONE *));
  invbcopy(&dot_d, &(it->dot_d), sizeof(DOT *));
  invbcopy(&line_d, &(it->line_d), sizeof(LINE *));
  invbcopy(&face3_d, &(it->face3_d), sizeof(FACE3 *));
  invbcopy(&face4_d, &(it->face4_d), sizeof(FACE4 *));
  invbcopy(&face4t_d, &(it->face4t_d), sizeof(FACE4T *));
  invbcopy(&label_d, &(it->label_d), sizeof(LABEL *));
  invbcopy(&handle_d, &(it->handle_d), sizeof(_S2HANDLE *));
  invbcopy(&bboard_d, &(it->bboard_d), sizeof(_S2BBOARD *));
  invbcopy(&bbset_d, &(it->bbset_d), sizeof(_S2BBSET *));
  invbcopy(&face3a_d, &(it->face3a_d), sizeof(_S2FACE3A *));
  invbcopy(&trdot_d, &(it->trdot_d), sizeof(TRDOT *));
#if defined(S2_3D_TEXTURES)
  invbcopy(&texpoly3d_d, &(it->texpoly3d_d), sizeof(_S2TEXPOLY3D *));
#endif
  invbcopy(&texmesh_d, &(it->texmesh_d), sizeof(_S2TEXTUREDMESH *));

}  

/*
 Write our own OpenGL text.
 The simplex font was derived from the hershey font set
 The array contains the vectors for characters 23 to 126 inclusive
 The first element for each character is the number of vectors
 The second element for each character is the width (proportional fonts)
 The remaining numbers are coordinates, (0,0) is bottom left.
 -1,-1 indicates a pen rise
 The characters are at most 25 units high.
 "list" needs to be at least 220 times the string length
 The text is created along "right", in the "right"-"up" plane.
 The size is determined by the length of the "right" and "up" vectors.
 */
void CreateLabelVector(char *s,XYZ p,XYZ right,XYZ up,XYZ *list,int *nlist)
{
	int c,j;
	unsigned int i;
	XYZ p1,p2;
	double x,y;
	
#include "simplex.h"
	
	(*nlist) = 0;
	
	for (i=0;i<strlen(s);i++) {
		c = s[i];
		if (c < 32 || c > 126)
			continue;
		c -= 32;
		
		for (j=1;j<simplex[c][0];j++) {
			x = simplex[c][2*(j-1)+2] / 25.0;
			y = simplex[c][2*(j-1)+3] / 25.0;
			p1.x = p.x + x * right.x + y * up.x;
			p1.y = p.y + x * right.y + y * up.y;
			p1.z = p.z + x * right.z + y * up.z;
			
			x = simplex[c][2*j+2] / 25.0;
			y = simplex[c][2*j+3] / 25.0;
			p2.x = p.x + x * right.x + y * up.x;
			p2.y = p.y + x * right.y + y * up.y;
			p2.z = p.z + x * right.z + y * up.z;
			
			if ((simplex[c][2*(j-1)+2] == -1 && simplex[c][2*(j-1)+3] == -1) ||
				(simplex[c][2*j+2]     == -1 && simplex[c][2*j+3]     == -1)) 
				continue;
			list[*nlist]     = p1;
			list[(*nlist)+1] = p2;
			(*nlist) += 2;
		}
		
		p.x += right.x * simplex[c][1] / 25.0;
		p.y += right.y * simplex[c][1] / 25.0;
		p.z += right.z * simplex[c][1] / 25.0;
	}
}


/***********************************************************************
 *
 * WINDOWS AND VIEWPORTS
 *
 ***********************************************************************
 */

/* set up the 3d viewport */
void s2svp(float ix1, float ix2, float iy1, float iy2, float iz1, float iz2) {
  if ((ix1 < ix2) && (iy1 < iy2) && (iz1 < iz2)) {
    _s2devicemin[_S2XAX] = ix1;
    _s2devicemax[_S2XAX] = ix2;
    _s2devicemin[_S2YAX] = iy1;
    _s2devicemax[_S2YAX] = iy2;
    _s2devicemin[_S2ZAX] = iz1;
    _s2devicemax[_S2ZAX] = iz2;
    _s2priv_setBounds();
  } else {
    _s2error("s2svp", "invalid limits in function call");
  }
}
/* query the viewport settings */
void s2qvp(float *odx1, float *odx2, float *ody1, float *ody2,
	   float *odz1, float *odz2) {
  if (odx1) {
    *odx1 = _s2devicemin[_S2XAX];
  }
  if (odx2) {
    *odx2 = _s2devicemax[_S2XAX];
  }
  if (ody1) {
    *ody1 = _s2devicemin[_S2YAX];
  }
  if (ody2) {
    *ody2 = _s2devicemax[_S2YAX];
  }
  if (odz1) {
    *odz1 = _s2devicemin[_S2ZAX];
  }
  if (odz2) {
    *odz2 = _s2devicemax[_S2ZAX];
  }
}
/* set the world coordinate range for the current viewport */
void s2swin(float ix1, float ix2, float iy1, float iy2, float iz1, float iz2) {
  if ((ix1 < ix2) && (iy1 < iy2) && (iz1 < iz2)) {
    _s2axismin[_S2XAX] = ix1;
    _s2axismax[_S2XAX] = ix2;
    _s2axismin[_S2YAX] = iy1;
    _s2axismax[_S2YAX] = iy2;
    _s2axismin[_S2ZAX] = iz1;
    _s2axismax[_S2ZAX] = iz2;
    _s2priv_setBounds();
  } else {
    _s2error("s2swin", "invalid limits in function call");
  }
}
/* query the world coordinate range */
void s2qwin(float *ox1, float *ox2, float *oy1, float *oy2,
	    float *oz1, float *oz2) {
  if (ox1) {
    *ox1 = _s2axismin[_S2XAX];
  }
  if (ox2) {
    *ox2 = _s2axismax[_S2XAX];
  }
  if (oy1) {
    *oy1 = _s2axismin[_S2YAX];
  }
  if (oy2) {
    *oy2 = _s2axismax[_S2YAX];
  }
  if (oz1) {
    *oz1 = _s2axismin[_S2ZAX];
  }
  if (oz2) {
    *oz2 = _s2axismax[_S2ZAX];
  }
}

/* convenience function to set the viewport, world coordinate
 * range, and optionally draw and label a box around it.
 */
void s2env(float ixmin, float ixmax, float iymin, float iymax, 
	   float izmin, float izmax, int just, int axis) {

  if ((ixmin > ixmax) || (iymin > iymax) || (izmin > izmax)) {
    _s2error("s2env", "invalid limits in function call");
  }
  
  if (just == 1) {
    float sca = MAX(MAX(ixmax-ixmin, iymax-iymin), izmax-izmin);
    s2svp(ixmin/sca, ixmax/sca, iymin/sca, iymax/sca, izmin/sca, izmax/sca);
  } else {
    if (just) {
      _s2warn("s2env", "unsupported 'just' argument in function call");
    }
    s2svp(-1.,1.,-1.,1.,-1.,1.);
  } 
  s2swin(ixmin,ixmax,iymin,iymax,izmin,izmax);
  if (axis < -1) {
    /* no labels at all */
    return;
  }

  float tick = 0.;
  float sub = 0;
  char opt[] = "BCDE.........";
  int optidx = 4;
  if (axis >= 0) {
    opt[optidx++] = 'T';
    opt[optidx++] = 'M';
    opt[optidx++] = 'N';
    opt[optidx++] = 'Q';
  }
  if (axis >= 1) {
    // draw coordinate axes - NYI
  }
  if (axis >= 2) {
    opt[optidx++] = 'G';
  }
  
  opt[optidx++] = '\0';
  /* options in place - call the s2box routine */
  s2box(opt, tick, sub, opt, tick, sub, opt, tick, sub);
  return;
}

/***********************************************************************
 *
 * ATTRIBUTES ETC.
 *
 ***********************************************************************
 */

/* set the color index for drawing */
void s2sci(int idx) {
  if ((idx < 0) || (idx >= _s2_cmapsize)) {
    idx = S2_PG_WHITE;
  }
  _s2_colidx = idx;
}

/* set a color representation */
void s2scr(int idx, float ir, float ig, float ib) {
  if ((idx < 0) || (idx > _S2MAXCOLORS)) {
    _s2warn("s2scr", "invalid color index supplied");
    return;
  }
  if (idx >= _s2_cmapsize) {
    /* idx is beyond current map size, increase ... */
    _s2_colormap = (COLOUR *)realloc(_s2_colormap, (idx+1)*sizeof(COLOUR));
    if (!_s2_colormap) {
      _s2error("s2scr", "failed to increase colormap size");
    } else {
      _s2_cmapsize = idx+1;
    }
  }
  
  _s2_colormap[idx].r = ir;
  _s2_colormap[idx].b = ib;
  _s2_colormap[idx].g = ig;
}

/* query color map values */
void s2qcr(int idx, float *oir, float *og, float *ob) {
  if ((idx < 0) || (idx > _s2_cmapsize)) {
    _s2warn("s2qcr", "invalid color index supplied");
    if (oir) {
      *oir = 0.;
    }
    if (og) {
      *og = 0.;
    }
    if (ob) {
      *ob = 0.;
    }
    return;
  }
  if (oir) {
    *oir = _s2_colormap[idx].r;
  }
  if (og) {
    *og = _s2_colormap[idx].g;
  }
  if (ob) {
    *ob = _s2_colormap[idx].b;
  }
  return;
}

/* set line width */
void s2slw(float width) { _s2_linewidth = fabsf(width); }

/* set line style */
void s2sls(int ls) { _s2_linestyle = ls; }

/* set character height */
void s2sch(float size) { _s2_charsize = fabsf(size); }

/* set arrowhead style */
void s2sah(int fs, float angle, float barb) {
  _s2_arrow_fs = MAX(1, MIN(2, fs));
  _s2_arrow_angle = MAX(5.0, MIN(135.0, angle));
  _s2_arrow_barb = MAX(0.05, MIN(1.0, barb));
}



/***********************************************************************
 *
 * PRIMITIVES
 *
 ***********************************************************************
 */

/* draw a point */
void s2pt1(float ix, float iy, float iz, int isymbol) {
  s2pt(1, &ix, &iy, &iz, isymbol);
}

/* draw a set of points with the same symbol */
void s2pt(int inp, float *ixpts, float *iypts, float *izpts, int isymbol) {
  // sanity check
  if (!ixpts || !iypts || !izpts) {
    _s2warn("s2pt", "invalid function argument(s) [NULL pointer(s)]");
    return;
  }
  /* code created to add these points to the (global) list of dots,
   * balls, lines, etc. */
  int i;
  if (inp > 0) {

    float ticklen = _S2TIKFRAC * _s2_charsize * 0.7;
    
    switch(isymbol) {

      /* a wireframe box */
    case 0: {
      XYZ p, q[2];
      int i;
      float del = 0.7 * ticklen;
      COLOUR thecol = _s2_colormap[_s2_colidx];
      for (i = 0; i < inp; i++) {
	p.x = ixpts[i];
	p.y = iypts[i];
	p.z = izpts[i];
	if (_s2_clipping && !S2INWORLD(p)) {
	  continue;
	}
	p.x = _S2WORLD2DEVICE(ixpts[i], _S2XAX);
	p.y = _S2WORLD2DEVICE(iypts[i], _S2YAX);
	p.z = _S2WORLD2DEVICE(izpts[i], _S2ZAX);
	q[0] = q[1] = p;
	
	// surround of negative-most x plane
	q[0].x = p.x - del;
	q[1].x = p.x - del;

	q[0].y = p.y - del;
	q[0].z = p.z - del;
	q[1].y = p.y + del;
	q[1].z = p.z - del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	q[0].y = p.y - del;
	q[0].z = p.z + del;
	q[1].y = p.y + del;
	q[1].z = p.z + del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	q[0].y = p.y - del;
	q[0].z = p.z - del;
	q[1].y = p.y - del;
	q[1].z = p.z + del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	q[0].y = p.y + del;
	q[0].z = p.z - del;
	q[1].y = p.y + del;
	q[1].z = p.z + del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	// surround of positive-most x plane
	q[0].x = p.x + del;
	q[1].x = p.x + del;

	q[0].y = p.y - del;
	q[0].z = p.z - del;
	q[1].y = p.y + del;
	q[1].z = p.z - del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	q[0].y = p.y - del;
	q[0].z = p.z + del;
	q[1].y = p.y + del;
	q[1].z = p.z + del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	q[0].y = p.y - del;
	q[0].z = p.z - del;
	q[1].y = p.y - del;
	q[1].z = p.z + del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	q[0].y = p.y + del;
	q[0].z = p.z - del;
	q[1].y = p.y + del;
	q[1].z = p.z + del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	// joins
	q[0].x = p.x - del;
	q[1].x = p.x + del;

	q[0].y = p.y - del;
	q[0].z = p.z - del;
	q[1].y = p.y - del;
	q[1].z = p.z - del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	q[0].y = p.y + del;
	q[0].z = p.z - del;
	q[1].y = p.y + del;
	q[1].z = p.z - del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	q[0].y = p.y - del;
	q[0].z = p.z + del;
	q[1].y = p.y - del;
	q[1].z = p.z + del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

	q[0].y = p.y + del;
	q[0].z = p.z + del;
	q[1].y = p.y + del;
	q[1].z = p.z + del;
	AddLine2Database(q[0], q[1], thecol, thecol, 1.0);

      }
      break;
    }


      /* a 3d cross or a shaded box */
    case 2: 
    case 6: {
      int tsym = 1;
      float sca = 1.0;
      if (isymbol == 6) {
	tsym = 2;
	sca = 0.7;
      }
      XYZ p;
      for (i = 0; i < inp; i++) {
	p.x = ixpts[i];
	p.y = iypts[i];
	p.z = izpts[i];
	if (_s2_clipping && !S2INWORLD(p)) {
	  continue;
	}
	p.x = _S2WORLD2DEVICE(ixpts[i], _S2XAX);
	p.y = _S2WORLD2DEVICE(iypts[i], _S2YAX);
	p.z = _S2WORLD2DEVICE(izpts[i], _S2ZAX);
	AddMarker2Database(tsym, ticklen*2.0*sca, p, _s2_colormap[_s2_colidx]);
      }
      break;
    }


      /* a shaded sphere */
    case 4: {
      XYZ p;
      for (i = 0; i < inp; i++) {
	p.x = ixpts[i];
	p.y = iypts[i];
	p.z = izpts[i];
	if (_s2_clipping && !S2INWORLD(p)) {
	  continue;
	}
	
	BALL *ball_base = _s2priv_addballs(1);

	ball_base[0].p.x = _S2WORLD2DEVICE(ixpts[i], _S2XAX);
	ball_base[0].p.y = _S2WORLD2DEVICE(iypts[i], _S2YAX);
	ball_base[0].p.z = _S2WORLD2DEVICE(izpts[i], _S2ZAX);
	ball_base[0].r = ticklen;
	ball_base[0].colour.r = _S2PENRED;
	ball_base[0].colour.g = _S2PENGRN;
	ball_base[0].colour.b = _S2PENBLU;
	strcpy(ball_base[0].whichscreen, _s2_whichscreen);
	strncpy(ball_base[0].VRMLname, _s2_VRMLnames[_s2_currVRMLidx],
		MAXVRMLLEN);
	ball_base[0].VRMLname[MAXVRMLLEN-1] = '\0';
      }
      break;
    }

      /* a simple dot */
      //case -2:
      //case 1: {
    default: {
      if ((isymbol != -2) && (isymbol != 1)) {
	_s2warn("s2pt", "unknown symbol, reverting to points");
      }

      XYZ p;
      for (i = 0; i < inp; i++) {
	
	p.x = ixpts[i];
	p.y = iypts[i];
	p.z = izpts[i];
	if (_s2_clipping && !S2INWORLD(p)) {
	  continue;
	}

	DOT *dot_base = _s2priv_adddots(1);
	if (!dot_base) {
	  _s2error("s2pt", "failed to allocate memory for dots");
	}

	dot_base[0].colour.r = _S2PENRED;
	dot_base[0].colour.g = _S2PENGRN;
	dot_base[0].colour.b = _S2PENBLU;
	dot_base[0].p.x = _S2WORLD2DEVICE(ixpts[i], _S2XAX);
	dot_base[0].p.y = _S2WORLD2DEVICE(iypts[i], _S2YAX);
	dot_base[0].p.z = _S2WORLD2DEVICE(izpts[i], _S2ZAX);
	dot_base[0].size = (float)_s2_linewidth;
	strcpy(dot_base[0].whichscreen, _s2_whichscreen);
	strncpy(dot_base[0].VRMLname, _s2_VRMLnames[_s2_currVRMLidx],
		MAXVRMLLEN);
	dot_base[0].VRMLname[MAXVRMLLEN-1] = '\0';
      }
      break;
    }
      
    } /* switch */
  }
}


/* current line width and style */
float _s2_linewidth;
int _s2_linestyle;
int _s2_conelines;

/* draw a polyline */
void s2line(int in, float *ixpts, float *iypts, float *izpts) {
  s2linea(in, ixpts, iypts, izpts, 1.0);
}
void s2linea(int in, float *ixpts, float *iypts, float *izpts, float ialpha) {
  // sanity check
  if (!ixpts || !iypts || !izpts) {
    _s2warn("s2line", "invalid function argument(s) [NULL pointer(s)]");
    return;
  }
  /* code created to add these line segments to the (global) list */
  int i;
  in--;
  if (_s2_conelines && 1) {
    for (i = 0; i < in; i++) {
      _s2priv_coneline(ixpts+i, iypts+i, izpts+i);
    }
  } else {
    if (in > 0) {
      LINE *line_base = _s2priv_addlines(in);
      if (!line_base) {
	_s2error("s2line", "failed to allocate memory for line(s)");
      }      
      for (i = 0; i < in; i++) {
	line_base[i].colour[0].r = _S2PENRED;
	line_base[i].colour[0].g = _S2PENGRN;
	line_base[i].colour[0].b = _S2PENBLU;
	line_base[i].colour[1].r = _S2PENRED;
	line_base[i].colour[1].g = _S2PENGRN;
	line_base[i].colour[1].b = _S2PENBLU;
	line_base[i].p[0].x = _S2WORLD2DEVICE(ixpts[i], _S2XAX);
	line_base[i].p[0].y = _S2WORLD2DEVICE(iypts[i], _S2YAX);
	line_base[i].p[0].z = _S2WORLD2DEVICE(izpts[i], _S2ZAX);
	line_base[i].p[1].x = _S2WORLD2DEVICE(ixpts[i+1], _S2XAX);
	line_base[i].p[1].y = _S2WORLD2DEVICE(iypts[i+1], _S2YAX);
	line_base[i].p[1].z = _S2WORLD2DEVICE(izpts[i+1], _S2ZAX);
	line_base[i].width = _s2_linewidth;
	strcpy(line_base[i].whichscreen, _s2_whichscreen);
	strncpy(line_base[i].VRMLname, _s2_VRMLnames[_s2_currVRMLidx],
		MAXVRMLLEN);
	line_base[i].VRMLname[MAXVRMLLEN-1] = '\0';
	switch (_s2_linestyle) {
	case 2:
	  line_base[i].stipple_factor = 3;
	  line_base[i].stipple_pattern = 0x0F0F;
	  break;
	case 3:
	  line_base[i].stipple_factor = 3;
	  line_base[i].stipple_pattern = 0x2727;
	  break;
	case 4:
	  line_base[i].stipple_factor = 3;
	  line_base[i].stipple_pattern = 0x2222;
	  break;
	case 5:
	  line_base[i].stipple_factor = 3;
	  line_base[i].stipple_pattern = 0x2227;
	  break;
	default:
	case 1:
	  line_base[i].stipple_factor = 0;
	  line_base[i].stipple_pattern = 0;
	  break;
	}
	line_base[i].alpha = ialpha;
      }
    }
  }
}

/* draw a disk/annulus */
void s2diskxy(float ipx, float ipy, float ipz, float ir1, float ir2) {
  DISK *disk_base = _s2priv_adddisks(1);
  if (!disk_base) {
    _s2error("s2diskxy", "failed to allocate memory for disk");
  }
  disk_base->p.x = _S2WORLD2DEVICE(ipx, _S2XAX);
  disk_base->p.y = _S2WORLD2DEVICE(ipy, _S2YAX);
  disk_base->p.z = _S2WORLD2DEVICE(ipz, _S2ZAX);
  disk_base->r1 = sqrt(0.5 * (powf(_S2WORLD2DEVICE_SO(ir1, _S2XAX), 2.0) +
			      powf(_S2WORLD2DEVICE_SO(ir1, _S2YAX), 2.0)));
  disk_base->r2 = sqrt(0.5 * (powf(_S2WORLD2DEVICE_SO(ir2, _S2XAX), 2.0) +
			      powf(_S2WORLD2DEVICE_SO(ir2, _S2YAX), 2.0)));
  disk_base->colour = _s2_colormap[_s2_colidx];
  disk_base->n.x = 0.0;
  disk_base->n.y = 0.0;
  disk_base->n.z = 1.0;
  strcpy(disk_base->whichscreen, _s2_whichscreen);
  return;
}
void s2diskxz(float ipx, float ipy, float ipz, float ir1, float ir2) {
  DISK *disk_base = _s2priv_adddisks(1);
  if (!disk_base) {
    _s2error("s2diskxz", "failed to allocate memory for disk");
  }
  disk_base->p.x = _S2WORLD2DEVICE(ipx, _S2XAX);
  disk_base->p.y = _S2WORLD2DEVICE(ipy, _S2YAX);
  disk_base->p.z = _S2WORLD2DEVICE(ipz, _S2ZAX);
  disk_base->r1 = sqrt(0.5 * (powf(_S2WORLD2DEVICE_SO(ir1, _S2XAX), 2.0) +
			      powf(_S2WORLD2DEVICE_SO(ir1, _S2ZAX), 2.0)));
  disk_base->r2 = sqrt(0.5 * (powf(_S2WORLD2DEVICE_SO(ir2, _S2XAX), 2.0) +
			      powf(_S2WORLD2DEVICE_SO(ir2, _S2ZAX), 2.0)));
  disk_base->colour = _s2_colormap[_s2_colidx];
  disk_base->n.x = 0.0;
  disk_base->n.y = 1.0;
  disk_base->n.z = 0.0;
  strcpy(disk_base->whichscreen, _s2_whichscreen);
  return;
}
void s2diskyz(float ipx, float ipy, float ipz, float ir1, float ir2) {
  DISK *disk_base = _s2priv_adddisks(1);
  if (!disk_base) {
    _s2error("s2diskyz", "failed to allocate memory for disk");
  }
  disk_base->p.x = _S2WORLD2DEVICE(ipx, _S2XAX);
  disk_base->p.y = _S2WORLD2DEVICE(ipy, _S2YAX);
  disk_base->p.z = _S2WORLD2DEVICE(ipz, _S2ZAX);
  disk_base->r1 = sqrt(0.5 * (powf(_S2WORLD2DEVICE_SO(ir1, _S2YAX), 2.0) +
			      powf(_S2WORLD2DEVICE_SO(ir1, _S2ZAX), 2.0)));
  disk_base->r2 = sqrt(0.5 * (powf(_S2WORLD2DEVICE_SO(ir2, _S2YAX), 2.0) +
			      powf(_S2WORLD2DEVICE_SO(ir2, _S2ZAX), 2.0)));
  disk_base->colour = _s2_colormap[_s2_colidx];
  disk_base->n.x = 1.0;
  disk_base->n.y = 0.0;
  disk_base->n.z = 0.0;
  strcpy(disk_base->whichscreen, _s2_whichscreen);
  return;
}

/* draw text */
void s2textxy(float ix, float iy, float iz, char *itext) {
  _s2_priv_textab(ix, iy, iz, _S2XAX, _S2YAX, 1., 1., itext);
}
void s2textxz(float ix, float iy, float iz, char *itext) {
  _s2_priv_textab(ix, iy, iz, _S2XAX, _S2ZAX, 1., 1., itext);
}
void s2textyz(float ix, float iy, float iz, char *itext) {
  _s2_priv_textab(ix, iy, iz, _S2YAX, _S2ZAX, 1., 1., itext);
}
void s2textxyf(float ix, float iy, float iz, 
	       float flipx, float flipy, char *itext) {
  _s2_priv_textab(ix, iy, iz, _S2XAX, _S2YAX, flipx, flipy, itext);
}
void s2textxzf(float ix, float iy, float iz, 
	       float flipx, float flipz, char *itext) {
  _s2_priv_textab(ix, iy, iz, _S2XAX, _S2ZAX, flipx, flipz, itext);
}
void s2textyzf(float ix, float iy, float iz, 
	       float flipy, float flipz, char *itext) {
  _s2_priv_textab(ix, iy, iz, _S2YAX, _S2ZAX, flipy, flipz, itext);
}

/* query bounding box of text */
void s2qtxtxy(float *x1, float *x2, float *y1, float *y2, 
	      float ix, float iy, float iz,
	      char *itext, float ipad) {
  _s2_priv_qtxtab(x1, x2, y1, y2, ix, iy, iz, _S2XAX, _S2YAX, 1., 1., 
		  itext, ipad);
}
void s2qtxtxz(float *x1, float *x2, float *z1, float *z2, 
	      float ix, float iy, float iz,
	      char *itext, float ipad) {
  _s2_priv_qtxtab(x1, x2, z1, z2, ix, iy, iz, _S2XAX, _S2ZAX, 1., 1., 
		  itext, ipad);
}
void s2qtxtyz(float *y1, float *y2, float *z1, float *z2, 
	      float ix, float iy, float iz,
	      char *itext, float ipad) {
  _s2_priv_qtxtab(y1, y2, z1, z2, ix, iy, iz, _S2YAX, _S2ZAX, 1., 1., 
		  itext, ipad);
}
void s2qtxtxyf(float *x1, float *x2, float *y1, float *y2, 
	       float ix, float iy, float iz, float flipx, float flipy,
	       char *itext, float ipad) {
  _s2_priv_qtxtab(x1, x2, y1, y2, ix, iy, iz, _S2XAX, _S2YAX, flipx, flipy, 
		  itext, ipad);
}
void s2qtxtxzf(float *x1, float *x2, float *z1, float *z2, 
	       float ix, float iy, float iz, float flipx, float flipz,
	       char *itext, float ipad) {
  _s2_priv_qtxtab(x1, x2, z1, z2, ix, iy, iz, _S2XAX, _S2ZAX, flipx, flipz, 
		  itext, ipad);
}
void s2qtxtyzf(float *y1, float *y2, float *z1, float *z2, 
	       float ix, float iy, float iz, float flipy, float flipz,
	       char *itext, float ipad) {
  _s2_priv_qtxtab(y1, y2, z1, z2, ix, iy, iz, _S2YAX, _S2ZAX, flipy, flipz, 
		  itext, ipad);
}

void s2arro(float ix1, float iy1, float iz1, 
	    float ix2, float iy2, float iz2) {
  if (_s2_arrow_fs != 1) {
    _s2warn("s2arro", "unsupported arrow fill style");
    return;
  }
  
  float xpts[2], ypts[2], zpts[2];
  xpts[0] = ix1;
  xpts[1] = ix2;
  ypts[0] = iy1;
  ypts[1] = iy2;
  zpts[0] = iz1;
  zpts[1] = iz2;
  /* 1. draw a line from 1 to 2 */
  s2line(2, xpts, ypts, zpts);
  /* 2. draw a cone with tip at point 2, and base somewhere along the 
   * line, back towards point 1 by an amount prop to character size, 
   * and base radius prop. to character size.
   */
  /* need to work in device coords here to make sure arrow head is 
   * "square" and indep. of line length */ 
  int i;
  for (i = 0; i < 2; i++) {
    xpts[i] = _S2WORLD2DEVICE(xpts[i], _S2XAX);
    ypts[i] = _S2WORLD2DEVICE(ypts[i], _S2YAX);
    zpts[i] = _S2WORLD2DEVICE(zpts[i], _S2ZAX);
  }
  float linelength = sqrt((xpts[1]-xpts[0]) * (xpts[1]-xpts[0]) +
			  (ypts[1]-ypts[0]) * (ypts[1]-ypts[0]) +
			  (zpts[1]-zpts[0]) * (zpts[1]-zpts[0]));
  float ticklen = _S2TIKFRAC * _s2_charsize * 0.7;
  /* note: arrow head may be longer than line joining 1 - 2 !!! */
  /* allocate space for TWO cones: inside and outside */  
  CONE *cone_base = _s2priv_addcones(2);
  if (!cone_base) {
    _s2error("s2arro", "failed to allocate memory for arrow heads");
  }

  /* outside cone */
  cone_base->p1.x = xpts[1];
  cone_base->p1.y = ypts[1];
  cone_base->p1.z = zpts[1];
  cone_base->r1 = 0.0; /* pointy end of arrow! */
  cone_base->p2.x = xpts[1] + (xpts[0] - xpts[1]) * ticklen / linelength;
  cone_base->p2.y = ypts[1] + (ypts[0] - ypts[1]) * ticklen / linelength;
  cone_base->p2.z = zpts[1] + (zpts[0] - zpts[1]) * ticklen / linelength;
  cone_base->r2 = ticklen * tan(0.5 * _s2_arrow_angle / 180.0 * PI);
  cone_base->colour = _s2_colormap[_s2_colidx];
  strcpy(cone_base->whichscreen, _s2_whichscreen);
  strncpy(cone_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  cone_base->VRMLname[MAXVRMLLEN-1] = '\0';

  /* inside cone */
  cone_base++;
  /* its vertex is moved from p1 towards p2 by fractional amount 
   * _s2_arrow_barb.
   */
  cone_base->p1.x = (cone_base-1)->p1.x + 
    ((cone_base-1)->p2.x - (cone_base-1)->p1.x) * _s2_arrow_barb;
  cone_base->p1.y = (cone_base-1)->p1.y + 
    ((cone_base-1)->p2.y - (cone_base-1)->p1.y) * _s2_arrow_barb;
  cone_base->p1.z = (cone_base-1)->p1.z + 
    ((cone_base-1)->p2.z - (cone_base-1)->p1.z) * _s2_arrow_barb;
  cone_base->r1 = 0.0;
  cone_base->p2 = (cone_base-1)->p2;
  cone_base->r2 = (cone_base-1)->r2;
  cone_base->colour = _s2_colormap[_s2_colidx];
  strcpy(cone_base->whichscreen, _s2_whichscreen);
  strncpy(cone_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  cone_base->VRMLname[MAXVRMLLEN-1] = '\0';
  return;
}



/***********************************************************************
 *
 * AXES, BOXES & LABELS
 *
 ***********************************************************************
 */

/* draw and label box around viewport */
void s2box(char *ixopt, float ixtick, int inxsub,
	   char *iyopt, float iytick, int inysub,
	   char *izopt, float iztick, int inzsub) {

  if (strlen(_s2_whichscreen)) {
    _s2warn("s2box", "function does not support screen coordinates");
    return;
  }

  char ctlchar[] = {'B', '\0'};
  char ctl2char[] = {'M', '\0'};

  float aline[3][2]; /* first index is axis, second index is start,end pt */
  float bline[3][2];

  /* ticklen is simply a fraction of the average axis length, scaled
   * by the current character height setting
   */
  float ticklen = _S2TIKFRAC * 0.33 * _s2_charsize *
    ((_s2axismax[0] - _s2axismin[0]) +
     (_s2axismax[1] - _s2axismin[1]) +
     (_s2axismax[2] - _s2axismin[2]));


  float labelp[3], labelr[3], labelu[3]; /* label position, right and up */
  char labelt[20]; /* label text */

  /* draw a labelled box around the world space.  Ooh, fun fun fun! */

  int axis, axis2, axis3;
  char *opt = NULL;
  float tick = 0.;
  int sub = 0;
  for (axis = 0; axis < 3; axis++) {
    if (axis == _S2XAX) {
      opt = ixopt;
      tick = ixtick;
      sub = inxsub;
    } else if (axis == _S2YAX) {
      opt = iyopt;
      tick = iytick;
      sub = inysub;
    } else if (axis == _S2ZAX) {
      opt = izopt;
      tick = iztick;
      sub = inzsub;
    }
    axis2 = (axis + 1) % 3;
    axis3 = (axis + 2) % 3;

    // store these opts for later use eg. by s2lab
    strcpy(_s2_opt_store[axis], opt);

    int sign2, sign3; /* direction to draw tick marks */
    sign2 = sign3 = 0;

    float major = 0;
    int nsubdiv = 0;

    /* first: calculate major interval and subdivisions in case they 
     * are needed */
    if (strstr(opt, "L")) {
      /* logarithmic requested: major = 1.0, minor = 0.1 */
      major = 1.0;
      nsubdiv = 9;
    } else if (tick <= 0.0) {
      /* "borrow" from PGPLOT's choice of xtick */
      major = MAX(0.05, MIN(7.0 * _S2XSP / options.screenwidth, 0.20)) *
	(_s2axismax[axis] - _s2axismin[axis]);
      major = _s2priv_round(major, &nsubdiv);
    } else {
      /* caller supplied major division */
      major = tick;
      nsubdiv = sub;
    }
    
    int norsign = 0;
    /* second: draw and mark frame edges if wanted */    
    aline[axis][0] = _s2axismin[axis];
    aline[axis][1] = _s2axismax[axis];
    for (ctlchar[0] = 'B'; ctlchar[0] < 'F'; ctlchar[0]++) {
      switch(ctlchar[0]) {
      case 'B':
	aline[axis2][0] = aline[axis2][1] = _s2axismin[axis2];
	aline[axis3][0] = aline[axis3][1] = _s2axismin[axis3];
	sign2 = sign3 = +1;
	norsign = +1;
	break;
      case 'C':
	aline[axis2][0] = aline[axis2][1] = _s2axismin[axis2];
	aline[axis3][0] = aline[axis3][1] = _s2axismax[axis3];
	sign2 = +1;
	sign3 = -1;
	norsign = -1;
	break;
      case 'D':
	aline[axis2][0] = aline[axis2][1] = _s2axismax[axis2];
	aline[axis3][0] = aline[axis3][1] = _s2axismin[axis3];
	sign2 = -1;
	sign3 = +1;
	norsign = -1;
	break;
      case 'E':
	aline[axis2][0] = aline[axis2][1] = _s2axismax[axis2];
	aline[axis3][0] = aline[axis3][1] = _s2axismax[axis3];
	sign2 = sign3 = -1;
	norsign = +1;
	break;
      } /* switch */

      if ((axis == 1) || (axis == 2)) {
	norsign *= -1;
      }

      if (strstr(opt, ctlchar)) {
	_s2_conelines = 1;
	s2line(2, aline[0], aline[1], aline[2]);
	_s2_conelines = 0;
      } else {
	continue;
      }

      float normal[3];
      float trapheight;
      //trapheight = _s2_charsize + 1.0;
      trapheight = 2.0;
      if (strstr(opt, "Q")) {
	trapheight += 1.2;
      }
      bline[axis][0] = aline[axis][0] - 
	trapheight * _s2axissgn[axis] * ticklen;
      bline[axis][1] = aline[axis][1] + 
	trapheight * _s2axissgn[axis] * ticklen;
      bline[axis2][0] = bline[axis2][1] = aline[axis2][0] - 
	trapheight * sign2 * _s2axissgn[axis2] * ticklen;
      bline[axis3][0] = bline[axis3][1] = 
	aline[axis3][0] - trapheight * sign3 * _s2axissgn[axis3] * ticklen;
      
      /* vertices are now in aline and bline - copy to lists */
      /* add repeat of first point (ignored by quads, but useful 
       * for drawing surround */
      float xpts[5], ypts[5], zpts[5];
      int i;
      for (i = 0; i < 2; i++) {
	xpts[i] = aline[0][i];
	xpts[i+2] = bline[0][1-i];
	ypts[i] = aline[1][i];
	ypts[i+2] = bline[1][1-i];
	zpts[i] = aline[2][i];
	zpts[i+2] = bline[2][1-i];
      }
      xpts[4] = xpts[0];
      ypts[4] = ypts[0];
      zpts[4] = zpts[0];
      
      normal[axis] = 0.0;
      normal[axis2] = + trapheight * sign2 * (_s2axissgn[axis2]) * ticklen;
      normal[axis3] = - trapheight * sign3 * (_s2axissgn[axis3]) * ticklen;

      /* both labels are wanted, or user has explicitly asked for 
       * panels to lay the text on. */
      if ((strstr(opt, "M") && strstr(opt, "N")) || 
	  strstr(opt, "O")) {
	int tcidx = _s2_colidx;
	s2sci(S2_PG_DKGRAY);
	_s2priv_quad(xpts, ypts, zpts, normal);
	s2sci(tcidx);
	_s2_conelines = 1;
	s2line(5, xpts, ypts, zpts);
	_s2_conelines = 0;
      }

      /* ok - now work along this axis marking ticks and labels if desired */

      /* 1. find starting point */
      int j = 0;
      float tik = (truncf(_s2axismin[axis] / major) + j) * major;
      /* get to within axis bounds */
      while (tik < _s2axismin[axis]) {
	j++;
	tik = (truncf(_s2axismin[axis] / major) + j) * major;
      }
      /* now back off to just outside axis bounds so that we can draw
       * the "leftmost" subticks
       */
      j--;
      tik = (truncf(_s2axismin[axis] / major) + j) * major;

      
      while (tik <= _s2axismax[axis]) {
	
	/* draw major tick marks */
	if ((tik > _s2axismin[axis]) && strstr(opt, "T")) {
	  bline[axis][0] = bline[axis][1] = tik;
	  
	  /* tick marks along axis-axis2 plane */
	  bline[axis2][0] = aline[axis2][0];
	  bline[axis2][1] = aline[axis2][0] + 
	    (float)sign2 * _s2axissgn[axis2] * ticklen;
	  bline[axis3][0] = aline[axis3][0];
	  bline[axis3][1] = aline[axis3][1];
	  s2line(2, bline[0], bline[1], bline[2]);
	  
	  /* tick marks along axis-axis3 plane */
	  bline[axis2][0] = aline[axis2][0];
	  bline[axis2][1] = aline[axis2][1];
	  bline[axis3][0] = aline[axis3][0];
	  bline[axis3][1] = aline[axis3][1] + 
	    (float)sign3 * _s2axissgn[axis3] * ticklen;
	  s2line(2, bline[0], bline[1], bline[2]);
	}

	/* draw major grid lines */
	if ((tik > _s2axismin[axis]) && strstr(opt, "G")) {

	  /* preserve current color and generate a dimmer version */
	  int _pres_colidx = _s2_colidx;
	  COLOUR _pres_col = _s2_colormap[_s2_colidx];
	  s2scr(_pres_colidx,_pres_col.r*0.5,_pres_col.g*0.5,_pres_col.b*0.5);
	  
	  /* grid lines along axis-axis2 plane */
	  bline[axis][0] = bline[axis][1] = tik;
	  bline[axis2][0] = aline[axis2][0];
	  bline[axis2][1] = aline[axis2][0] +
	    (float)sign2 * (_s2axismax[axis2] - _s2axismin[axis2]);
	  bline[axis3][0] = aline[axis3][0];
	  bline[axis3][1] = aline[axis3][1];
	  s2line(2, bline[0], bline[1], bline[2]);

	  /* grid lines along axis-axis3 plane */
	  bline[axis2][0] = aline[axis2][0];
	  bline[axis2][1] = aline[axis2][1];
	  bline[axis3][0] = aline[axis3][0];
	  bline[axis3][1] = aline[axis3][1] + 
	    (float)sign3 * (_s2axismax[axis3] - _s2axismin[axis3]);
	  s2line(2, bline[0], bline[1], bline[2]);

	  s2scr(_pres_colidx,_pres_col.r,_pres_col.g,_pres_col.b);
	}

	/* draw minor tick marks */
	if (strstr(opt, "S")) {
	  /* half-strength colour */
	  int _pres_colidx = _s2_colidx;
	  COLOUR _pres_col = _s2_colormap[_s2_colidx];
	  s2scr(_pres_colidx,_pres_col.r*0.5,_pres_col.g*0.5,_pres_col.b*0.5);

	  int ill;
	  float stik;
	  for (ill = 1; ill < nsubdiv; ill++) {
	    if (strstr(opt, "L")) {
	      /* logarithmic */
	      /* subticks are at tik+log(2), tik+log(3), ..., tik+log(9) */
	      stik = tik + log10f((float)(ill+1));
	    } else {
	      /* linear */
	      /* subticks are at tik + major/nsubdiv, tik + 2.major/nsubdiv,
	       * ..., tik + (nsubdiv-1).major/nsubdiv
	       */
	      stik = tik + ((float)ill) * major / (float)nsubdiv;
	    }

	    if (!_S2MONOTONIC(_s2axismin[axis], stik, _s2axismax[axis])) {
	      continue;
	    }

	  bline[axis][0] = bline[axis][1] = stik;
	  
	  /* tick marks along axis-axis2 plane */
	  bline[axis2][0] = aline[axis2][0];
	  bline[axis2][1] = aline[axis2][0] + 
	    (float)sign2 * _s2axissgn[axis2] * ticklen * 0.66;
	  bline[axis3][0] = aline[axis3][0];
	  bline[axis3][1] = aline[axis3][1];
	  s2line(2, bline[0], bline[1], bline[2]);
	  
	  /* tick marks along axis-axis3 plane */
	  bline[axis2][0] = aline[axis2][0];
	  bline[axis2][1] = aline[axis2][1];
	  bline[axis3][0] = aline[axis3][0];
	  bline[axis3][1] = aline[axis3][1] + 
	    (float)sign3 * _s2axissgn[axis3] * ticklen * 0.66;
	  s2line(2, bline[0], bline[1], bline[2]);
	    
	  }
	  s2scr(_pres_colidx,_pres_col.r,_pres_col.g,_pres_col.b);
	}


	/* note: this should depend on an option in "i?opt" */
	/* label !!! one only, 135-degrees from the tick vectors */
	/* up */
	labelu[axis] = 0.0;
	/* *** NOTE NOTE NOTE -> ticklen already has charsize in it *** */
	/* this should be fixed? */
	/* div fix tried for now */
	labelu[axis2] = _s2_charsize/_s2_charsize * (float)sign2 * 
	  _s2axissgn[axis2] * ticklen;
	labelu[axis3] = _s2_charsize/_s2_charsize * (float)sign3 * 
	  _s2axissgn[axis3] * ticklen;
	
	/* draw up to two labels, one either side of our trapezoid */
	// int norsign = +1;
	for (ctl2char[0] = 'M'; ctl2char[0] < 'O'; ctl2char[0]++) {

	  /* text */
	  if (strstr(opt, "L")) {
	    /* logarithmic: default to decimal */
	    if (strstr(opt, "2")) {
	      sprintf(labelt, "%.0E", powf(10.0, tik));
	    } else {
	      sprintf(labelt, "%+.*f", (int)(MAX(0.0, -tik)), powf(10.0, tik));
	    }
	  } else {
	    /* linear: default to decimal */
	    if (strstr(opt, "2")) {
	      sprintf(labelt, "%.2E", tik);
	    } else {
	      if (_S2MONOTONIC(-1e-5, tik, +1e-5)) {
		/* essentially zero: don't take log.  If user has smaller
		 * data they should darned well use exponential notn. */
		sprintf(labelt, "0.00");
	      } else {
		sprintf(labelt, "%.*f", 
			(int)(MAX(0.0, -log10f(fabsf(tik))+3.0)), tik);
	      }
	    }
	  }
	  
	  if (strstr(opt, ctl2char)) {
	    
	    /* right */
	    /* charsize already implicit in labelu vector */
	    labelr[axis] = labelu[axis2]*normal[axis3]*(float)norsign - 
	      labelu[axis3]*normal[axis2]*(float)norsign;
	    labelr[axis2] = labelr[axis3] = 0.0;
	    if (labelr[axis] > 0) {
	      labelr[axis] = ticklen * fabsf(_s2axissgn[axis]);
	    } else {
	      labelr[axis] = -ticklen * fabsf(_s2axissgn[axis]);
	    }
	    /* position */
	    labelp[axis] = tik - (0.5 * (float)strlen(labelt)) * labelr[axis];
	    if (!(_S2MONOTONIC(_s2axismin[axis],labelp[axis],_s2axismax[axis]) &&
		  _S2MONOTONIC(_s2axismin[axis],labelp[axis]+6.0*labelr[axis],
			    _s2axismax[axis]))) {
	      continue;
	    }
	    
	    /* offset of labels depends on character size, via labelu */
	    labelp[axis2] = aline[axis2][0] - 1.4 * labelu[axis2] +
	      0.04 * normal[axis2] * (float)norsign; // place above/below quad
	    labelp[axis3] = aline[axis3][0] - 1.4 * labelu[axis3] +
	      0.04 * normal[axis3] * (float)norsign; // place above/below quad
	    
	    _s2priv_text(labelp, labelr, labelu, labelt);
	  }
	  norsign *= -1; /* second time around normal is flipped */
	}
	j++;
	tik = (truncf(_s2axismin[axis] / major) + j) * major;
      }
    }
  }
}

/* draw x, y, z and plot titles */
void s2lab(char *ixlab, char *iylab, char *izlab, char *ititle) {
  if (strlen(_s2_whichscreen)) {
    _s2warn("s2lab", "function does not support screen coordinates");
    return;
  }
  /* a fair bit of code borrowed from s2box here is used to work out 
   * precisely where to put the labels.
   */
  float aline[3][2]; /* first index is axis, second index is start,end pt */
  float bline[3][2];
  char ctlchar[] = {'B', '\0'};
  char ctl2char[] = {'M', '\0'};
  
  /* ticklen is simply a fraction of the average axis length, scaled
   * by the current character height setting
   */
  float ticklen = _S2TIKFRAC * 0.33 * _s2_charsize *
    ((_s2axismax[0] - _s2axismin[0]) +
     (_s2axismax[1] - _s2axismin[1]) +
     (_s2axismax[2] - _s2axismin[2]));
  
  
  float labelp[3], labelr[3], labelu[3]; /* label position, right and up */

  int axis, axis2, axis3;
  char *text = NULL;
  for (axis = 0; axis < 3; axis++) {
    if (axis == _S2XAX) {
      text = ixlab;
    } else if (axis == _S2YAX) {
      text = iylab;
    } else if (axis == _S2ZAX) {
      text = izlab;
    }
    axis2 = (axis + 1) % 3;
    axis3 = (axis + 2) % 3;
    
    int sign2, sign3; /* direction to draw tick marks */
    sign2 = sign3 = 0;

    int norsign = 0;
    /* second: draw and mark frame edges if wanted */    
    aline[axis][0] = _s2axismin[axis];
    aline[axis][1] = _s2axismax[axis];
    for (ctlchar[0] = 'B'; ctlchar[0] < 'F'; ctlchar[0]++) {
      switch(ctlchar[0]) {
      case 'B':
	aline[axis2][0] = aline[axis2][1] = _s2axismin[axis2];
	aline[axis3][0] = aline[axis3][1] = _s2axismin[axis3];
	sign2 = sign3 = +1;
	norsign = +1;
	break;
      case 'C':
	aline[axis2][0] = aline[axis2][1] = _s2axismin[axis2];
	aline[axis3][0] = aline[axis3][1] = _s2axismax[axis3];
	sign2 = +1;
	sign3 = -1;
	norsign = -1;
	break;
      case 'D':
	aline[axis2][0] = aline[axis2][1] = _s2axismax[axis2];
	aline[axis3][0] = aline[axis3][1] = _s2axismin[axis3];
	sign2 = -1;
	sign3 = +1;
	norsign = -1;
	break;
      case 'E':
	aline[axis2][0] = aline[axis2][1] = _s2axismax[axis2];
	aline[axis3][0] = aline[axis3][1] = _s2axismax[axis3];
	sign2 = sign3 = -1;
	norsign = +1;
	break;
      } /* switch */

      if ((axis == 1) || (axis == 2)) {
	norsign *= -1;
      }

      if (!strstr(_s2_opt_store[axis], ctlchar)) {
	continue;
      }

      float normal[3];
      float trapheight;
      trapheight = _s2_charsize + 1.0;
      bline[axis][0] = aline[axis][0] - 
	trapheight * _s2axissgn[axis] * ticklen;
      bline[axis][1] = aline[axis][1] + 
	trapheight * _s2axissgn[axis] * ticklen;
      bline[axis2][0] = bline[axis2][1] = aline[axis2][0] - 
	trapheight * sign2 * _s2axissgn[axis2] * ticklen;
      bline[axis3][0] = bline[axis3][1] = 
	aline[axis3][0] - trapheight * sign3 * _s2axissgn[axis3] * ticklen;
      
      /* vertices are now in aline and bline - copy to lists */
      /* add repeat of first point (ignored by quads, but useful 
       * for drawing surround */
      float xpts[5], ypts[5], zpts[5];
      int i;
      for (i = 0; i < 2; i++) {
	xpts[i] = aline[0][i];
	xpts[i+2] = bline[0][1-i];
	ypts[i] = aline[1][i];
	ypts[i+2] = bline[1][1-i];
	zpts[i] = aline[2][i];
	zpts[i+2] = bline[2][1-i];
      }
      xpts[4] = xpts[0];
      ypts[4] = ypts[0];
      zpts[4] = zpts[0];
      
      normal[axis] = 0.0;
      normal[axis2] = + trapheight * sign2 * (_s2axissgn[axis2]) * ticklen;
      normal[axis3] = - trapheight * sign3 * (_s2axissgn[axis3]) * ticklen;

      /* label goes at centre of axis */
      /* 1. find starting point */
      float tik = (_s2axismax[axis] + _s2axismin[axis]) * 0.5;

      /* note: this should depend on an option in "i?opt" */
      /* label !!! one only, 135-degrees from the tick vectors */
      /* up */
      labelu[axis] = 0.0;
      /* *** NOTE NOTE NOTE -> ticklen already has charsize in it *** */
      /* this should be fixed? */
      /* div fix tried for now */
      labelu[axis2] = _s2_charsize/_s2_charsize * (float)sign2 * 
	_s2axissgn[axis2] * ticklen;
      labelu[axis3] = _s2_charsize/_s2_charsize * (float)sign3 * 
	_s2axissgn[axis3] * ticklen;
      
      /* draw up to two labels, one either side of our trapezoid */
      // int norsign = +1;
      for (ctl2char[0] = 'M'; ctl2char[0] < 'O'; ctl2char[0]++) {
	//if (strstr(opt, ctl2char)) {
	  
	/* right */
	/* charsize already implicit in labelu vector */
	labelr[axis] = labelu[axis2]*normal[axis3]*(float)norsign - 
	  labelu[axis3]*normal[axis2]*(float)norsign;
	labelr[axis2] = labelr[axis3] = 0.0;
	if (labelr[axis] > 0) {
	  labelr[axis] = ticklen * fabsf(_s2axissgn[axis]);
	} else {
	  labelr[axis] = -ticklen * fabsf(_s2axissgn[axis]);
	}
	/* position */
	labelp[axis] = tik - (0.5 * (float)strlen(text)) * labelr[axis];
	if (!(_S2MONOTONIC(_s2axismin[axis],labelp[axis],_s2axismax[axis]) &&
	      _S2MONOTONIC(_s2axismin[axis],labelp[axis]+6.0*labelr[axis],
			_s2axismax[axis]))) {
	  continue;
	}
	
	/* offset of labels depends on character size, via labelu */
	labelp[axis2] = aline[axis2][0] - 2.55 * labelu[axis2] +
	  0.04 * normal[axis2] * (float)norsign; // place above/below quad
	labelp[axis3] = aline[axis3][0] - 2.55 * labelu[axis3] +
	  0.04 * normal[axis3] * (float)norsign; // place above/below quad
	
	/* write the text */
	_s2priv_text(labelp, labelr, labelu, text);

	norsign *= -1; /* second time around normal is flipped */
      }
    }
  }

  if (!ititle || !strlen(ititle)) {
    return;
  }
  if (_s2_title) {
    free(_s2_title);
  }
  _s2_title = (char *)calloc(strlen(ititle) + 1, sizeof(char));
  // should check alloc worked 
  strcpy(_s2_title, ititle);
}

/***********************************************************************
 *
 * XY(Z) PLOTS
 *
 ***********************************************************************
 */
/* draw error bars */
void s2errb(int idir, int inp, float *ixpts, float *iypts,
	    float *izpts, float *iedelt, int itermsize) {
  // sanity check/s
  if ((idir < 1) || (idir > 9)) {
    _s2warn("s2errb", "invalid direction for error bars");
    return;
  }
  if (inp < 1) {
    return;
  }
  if (!ixpts || !iypts || !izpts || !iedelt) {
    _s2warn("s2errb", "invalid function arguments [NULL pointer(s)]");
    return;
  }

  int i, j;
  int axis;
  
  /* figure out which bars to draw and on which axes */
  float draw[2];
  draw[0] = ((idir < 4) || (idir > 6)) ? 1. : -99.;
  draw[1] = (idir > 3) ? -1. : -99.;
  axis = ((idir-1) % 3);

  float aline[3][2];
  for (i = 0; i < inp; i++) {
    aline[_S2XAX][0] = aline[_S2XAX][1] = ixpts[i];
    aline[_S2YAX][0] = aline[_S2YAX][1] = iypts[i];
    aline[_S2ZAX][0] = aline[_S2ZAX][1] = izpts[i];

    /* loop over two possible directions for e/bar */    
    for (j = 0; j < 2; j++) {
      if (draw[j] < -9.) {
	continue;
      }
      aline[axis][1] = aline[axis][0] + draw[j] * iedelt[i];
      s2line(2, aline[0], aline[1], aline[2]);
      if (itermsize > 0) {
	s2slw(_s2_linewidth + itermsize);
	s2pt1(aline[0][1], aline[1][1], aline[2][1], 1);
	s2slw(_s2_linewidth - itermsize);
      }
    }
  }  
}

/* draw parametric function curve */
void s2funt(float(*fx)(float*), float(*fy)(float*), float(*fz)(float*),
	    int in, float itmin, float itmax) {
  // sanity check
  if (!fx || !fy || !fz) {
    _s2warn("s2funt", "invalid function arguments [NULL pointer(s)]");
    return;
  }

  s2funtc(fx, fy, fz, NULL, in, itmin, itmax);
}

/* draw coloured parametric function curve */
void s2funtc(float(*fx)(float*), float(*fy)(float*), float(*fz)(float*),
	     float(*fc)(float*), int in, float itmin, float itmax) {

  // sanity check
  if (!fx || !fy || !fz) {
    _s2warn("s2funtc", "invalid function arguments [NULL pointer(s)]");
    return;
  }

  float xpts[2], ypts[2], zpts[2], col[2];
  int i;
  float t = itmin;
  xpts[1] = (*fx)(&t);
  ypts[1] = (*fy)(&t);
  zpts[1] = (*fz)(&t);
  if (fc) {
    col[1] = MAX(0.0, MIN(1.0, fc(&t)));
  } else {
    col[1] = 0.0;
  }
  for (i = 1; i < in; i++) {
    xpts[0] = xpts[1];
    ypts[0] = ypts[1];
    zpts[0] = zpts[1];
    col[0] = col[1];
    t = itmin + (float)i / (float)(in - 1) * (itmax - itmin);
    xpts[1] = (*fx)(&t);
    ypts[1] = (*fy)(&t);
    zpts[1] = (*fz)(&t);
    if (fc) {
      col[1] = MAX(0.0, MIN(1.0, fc(&t)));
      s2sci(MAX(_s2_colr1, MIN(_s2_colr2, _s2_colr1 + 
			       ((col[0]+col[1])*0.5) * 
			       (float)(_s2_colr2-_s2_colr1))));
    }
    s2line(2, xpts, ypts, zpts);
  }
}

/* draw functionally-defined surfaces */
void s2funxy(float(*fxy)(float*,float*), int nx, int ny,
	     float xmin, float xmax, float ymin, float ymax,
	     int ctl) {
  // give dmax < dmin, meaning funab will auto-scale colors 
  _s2priv_funab(fxy, nx, ny, xmin, xmax, ymin, ymax, _S2XAX, _S2YAX, ctl,
		0.0, -1.0);
}
void s2funxz(float(*fxz)(float*,float*), int nx, int nz,
	     float xmin, float xmax, float zmin, float zmax,
	     int ctl) {
  // give dmax < dmin, meaning funab will auto-scale colors 
  _s2priv_funab(fxz, nx, nz, xmin, xmax, zmin, zmax, _S2XAX, _S2ZAX, ctl,
		0.0, -1.0);
}
void s2funyz(float(*fyz)(float*,float*), int ny, int nz,
	     float ymin, float ymax, float zmin, float zmax,
	     int ctl) {
  // give dmax < dmin, meaning funab will auto-scale colors 
  _s2priv_funab(fyz, ny, nz, ymin, ymax, zmin, zmax, _S2YAX, _S2ZAX, ctl,
		0.0, -1.0);
}

/* functionally defined surfaces with set color map scaling */
void s2funxyr(float(*fxy)(float*,float*), int nx, int ny,
	      float xmin, float xmax, float ymin, float ymax,
	      int ctl, float rmin, float rmax) {
  _s2priv_funab(fxy, nx, ny, xmin, xmax, ymin, ymax, _S2XAX, _S2YAX, ctl,
		rmin, rmax);
}
void s2funxzr(float(*fxz)(float*,float*), int nx, int nz,
	      float xmin, float xmax, float zmin, float zmax,
	      int ctl, float rmin, float rmax) {
  _s2priv_funab(fxz, nx, nz, xmin, xmax, zmin, zmax, _S2XAX, _S2ZAX, ctl,
		rmin, rmax);
}
void s2funyzr(float(*fyz)(float*,float*), int ny, int nz,
	      float ymin, float ymax, float zmin, float zmax,
	      int ctl, float rmin, float rmax) {
  _s2priv_funab(fyz, ny, nz, ymin, ymax, zmin, zmax, _S2YAX, _S2ZAX, ctl,
		rmin, rmax);
}

/* plot the parametric function (generally a surface) defined by 
 *  { (fx(u,v), fy(u,v), fz(u,v) }, coloured by fcol(u, v) with 
 * fcol required to fall in the range [0,1], and mapping to the 
 * current colormap.
 */
void s2funuv(float(*fx)(float*,float*), float(*fy)(float*,float*), 
	     float(*fz)(float*,float*), float(*fcol)(float*,float*), 
	     float umin, float umax, int uDIV,
	     float vmin, float vmax, int vDIV) {
  _s2priv_s2funuv(fx, fy, fz, fcol, 'z', NULL, umin, umax, uDIV, vmin, vmax, vDIV);
}

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
	      float vmin, float vmax, int vDIV) {
  _s2priv_s2funuv(fx, fy, fz, fcol, trans, falpha, umin, umax, uDIV, vmin, vmax, vDIV);
}


/***********************************************************************
 *
 * IMAGES / SURFACES / TEXTURES
 *
 ***********************************************************************
 */

/* surface plot */
void s2surp(float **data, int nx, int ny, 
	    int i1, int i2, int j1, int j2,
	    float datamin, float datamax, 
	    float *tr) {
  
  if ((i2 <= i1) || (i1 < 0) || (i1 >= nx) || (i2 < 0) || (i2 >= nx) ||
      (j2 <= j1) || (j1 < 0) || (j1 >= ny) || (j2 < 0) || (j2 >= ny) ||
      (datamax <= datamin)) {
    _s2warn("s2surp", "invalid data dimensions or slice");
    return;
  }

  if (_s2_cmapsize < 1) {
    _s2warn("s2surp", "colormap too small or not set");
    return;
  }

  if (!data || !tr) {
    _s2warn("s2surp", "invalid function arguments [NULL pointer(s)]");
    return;
  }

  int i, j, k;

  int nvert = (i2 - i1 + 1) * (j2 - j1 + 1);
  XYZ *pxyz = (XYZ *)calloc(nvert + 3, sizeof(XYZ));
  if (!pxyz) {
    _s2error("s2surp", "failed to allocate vertex memory");
  }
  k = 0;
  for (i = i1; i <= i2; i++) {
    for (j = j1; j <= j2; j++) {
      pxyz[k].x = tr[0] + tr[1] * (float)i + tr[2] * (float)j;
      pxyz[k].y = tr[3] + tr[4] * (float)i + tr[5] * (float)j;
      pxyz[k].z = tr[6] + tr[7] * data[i][j];
      k++;
    }
  }

  ITRIANGLE *trgls = (ITRIANGLE *)calloc(3 * nvert, sizeof(ITRIANGLE));
  if (!trgls) {
    _s2error("s2surp", "failed to allocate triangle memory");
  }
  int ntrgl;
  if (Triangulate(nvert, pxyz, trgls, &ntrgl)) {
    _s2error("s2surp", "failed triangulating surface");
  }

  XYZ shift;
  shift.x = shift.y = shift.z = 0.0;
  double scale = 1.0;
  XYZ tri3d[3];
  float rawz;
  int colidx;
  for (i = 0; i < ntrgl; i++) {
    tri3d[0].x = _S2WORLD2DEVICE(pxyz[trgls[i].p1].x, _S2XAX);
    tri3d[0].y = _S2WORLD2DEVICE(pxyz[trgls[i].p1].y, _S2YAX);
    tri3d[0].z = _S2WORLD2DEVICE(pxyz[trgls[i].p1].z, _S2ZAX);
    tri3d[1].x = _S2WORLD2DEVICE(pxyz[trgls[i].p2].x, _S2XAX);
    tri3d[1].y = _S2WORLD2DEVICE(pxyz[trgls[i].p2].y, _S2YAX);
    tri3d[1].z = _S2WORLD2DEVICE(pxyz[trgls[i].p2].z, _S2ZAX);
    tri3d[2].x = _S2WORLD2DEVICE(pxyz[trgls[i].p3].x, _S2XAX);
    tri3d[2].y = _S2WORLD2DEVICE(pxyz[trgls[i].p3].y, _S2YAX);
    tri3d[2].z = _S2WORLD2DEVICE(pxyz[trgls[i].p3].z, _S2ZAX);
    /* calculate colour ... */
    rawz = ((pxyz[trgls[i].p1].z + pxyz[trgls[i].p2].z + 
	     pxyz[trgls[i].p3].z) / 3.0 - tr[6]) / tr[7];

    colidx = MAX(_s2_colr1, MIN(_s2_colr2, _s2_colr1 + (rawz - datamin) / 
				(datamax - datamin) * 
				(float)(_s2_colr2 - _s2_colr1 + 1)));
    AddFace2Database(tri3d, 3, _s2_colormap[colidx], scale, shift);
  }
  free(pxyz);
  free(trgls);
}

/* surface plot with arbitrary rotation / skew / translation */
void s2surpa(float **data, int nx, int ny, 
	     int i1, int i2, int j1, int j2,
	     float datamin, float datamax, 
	     float *tr) {
  
  if ((i2 <= i1) || (i1 < 0) || (i1 >= nx) || (i2 < 0) || (i2 >= nx) ||
      (j2 <= j1) || (j1 < 0) || (j1 >= ny) || (j2 < 0) || (j2 >= ny) ||
      (datamax <= datamin)) {
    _s2warn("s2surpa", "invalid data dimensions or slice");
    return;
  }

  if (_s2_cmapsize < 1) {
    _s2warn("s2surpa", "colormap too small or not set");
    return;
  }

  if (!data || !tr) {
    _s2warn("s2surpa", "invalid function arguments [NULL pointer(s)]");
    return;
  }
  
  int i, j, k;
  int nvert = (i2 - i1 + 1) * (j2 - j1 + 1);
  XYZ *pxyz = (XYZ *)calloc(nvert + 3, sizeof(XYZ));
  /* store datavalues here for choosing colour */
  float *dvals = (float *)calloc(nvert + 3, sizeof(float));
  if (!pxyz) {
    _s2error("s2surpa", "failed to allocate vertex memory");
  }
  k = 0;
  for (i = i1; i <= i2; i++) {
    for (j = j1; j <= j2; j++) {
      /* do a simple transformation now, then undo and get it right
       * after triangulation of the surface */
      pxyz[k].x = (float)i;
      pxyz[k].y = (float)j;
      pxyz[k].z = data[i][j];
      k++;
    }
  }
  qsort(pxyz, k, sizeof(XYZ), XYZCompare);


  ITRIANGLE *trgls = (ITRIANGLE *)calloc(3 * nvert, sizeof(ITRIANGLE));
  if (!trgls) {
    _s2error("s2surpa", "failed to allocate triangle memory");
  }
  int ntrgl;
  if (Triangulate(nvert, pxyz, trgls, &ntrgl)) {
    _s2error("s2surpa", "failed to triangulate surface");
  }

  /* apply the full transformation to the vertices */
  float ttx, tty, ttz;
  for (i = 0; i < k; i++) {
    ttx = pxyz[i].x;
    tty = pxyz[i].y;
    ttz = pxyz[i].z;
    pxyz[i].x = tr[0] + tr[1] * ttx + tr[2] * tty + tr[3] * ttz;
    pxyz[i].y = tr[4] + tr[5] * ttx + tr[6] * tty + tr[7] * ttz;
    pxyz[i].z = tr[8] + tr[9] * ttx + tr[10]* tty + tr[11]* ttz;
    dvals[i] = ttz;
  }

  XYZ shift;
  shift.x = shift.y = shift.z = 0.0;
  double scale = 1.0;
  XYZ tri3d[3];
  float rawz;
  int colidx;
  for (i = 0; i < ntrgl; i++) {
    tri3d[0].x = _S2WORLD2DEVICE(pxyz[trgls[i].p1].x, _S2XAX);
    tri3d[0].y = _S2WORLD2DEVICE(pxyz[trgls[i].p1].y, _S2YAX);
    tri3d[0].z = _S2WORLD2DEVICE(pxyz[trgls[i].p1].z, _S2ZAX);
    tri3d[1].x = _S2WORLD2DEVICE(pxyz[trgls[i].p2].x, _S2XAX);
    tri3d[1].y = _S2WORLD2DEVICE(pxyz[trgls[i].p2].y, _S2YAX);
    tri3d[1].z = _S2WORLD2DEVICE(pxyz[trgls[i].p2].z, _S2ZAX);
    tri3d[2].x = _S2WORLD2DEVICE(pxyz[trgls[i].p3].x, _S2XAX);
    tri3d[2].y = _S2WORLD2DEVICE(pxyz[trgls[i].p3].y, _S2YAX);
    tri3d[2].z = _S2WORLD2DEVICE(pxyz[trgls[i].p3].z, _S2ZAX);
    /* calculate colour ... */
    rawz = (dvals[trgls[i].p1] + dvals[trgls[i].p2] + dvals[trgls[i].p3]) * 
      0.333333;

    colidx = MAX(_s2_colr1, MIN(_s2_colr2, _s2_colr1 + (rawz - datamin) / 
				(datamax - datamin) * 
				(float)(_s2_colr2 - _s2_colr1 + 1)));
    AddFace2Database(tri3d, 3, _s2_colormap[colidx], scale, shift);
  }

  /* free alloc'ed memory */
  free(trgls);
  free(dvals);
  free(pxyz);

}

/* set index range for colourful shading */
void s2scir(int icol1, int icol2) {
  _s2_colr1 = icol1;
  _s2_colr2 = icol2;
}

/* query color index range */
void s2qcir(int *ocol1, int *ocol2) {
  if (ocol1) {
    *ocol1 = _s2_colr1;
  }
  if (ocol2) {
    *ocol2 = _s2_colr2;
  }
}

/* install various colormaps */
int s2icm(char *imapname, int iidx1, int iidx2) {
  int32_t *colormap;
  int ncolors;
  int i;
  float r, g, b;
  unsigned char *obuf;

  ncolors = iidx2 - iidx1 + 1;
  colormap = texture_gen(imapname, 16, ncolors, 0);
  for (i = 0; i < ncolors; i++) {
    obuf = (unsigned char *)(colormap + 12 * ncolors + i);
    r = obuf[0] / 255.0;
    g = obuf[1] / 255.0;
    b = obuf[2] / 255.0;
    s2scr(iidx1 + i, r, g, b);
  }
  free(colormap);
  return(ncolors);

}

/* create an empty texture */
unsigned int ss2ct(int width, int height) {
  BITMAP4 *bitmap = _s2priv_redXtexture(width, height);
  // use mipmaps
  return _s2priv_setupTexture(width, height, bitmap, 1);
}
#if defined(S2_3D_TEXTURES)
/* 3d texture version */
unsigned int ss2c3dt(int width, int height, int depth) {
  BITMAP4 *bitmap = _s2priv_redXtexture3d(width, height, depth);
  // use mipmaps
  return _s2priv_setupTexture3d(width, height, depth, bitmap, 1);
}
#endif
/* delete a texture by id */
void ss2dt(unsigned int texid) {
  _s2priv_dropTexture(texid);
}
/* fetch pointer to a texture */
unsigned char *ss2gt(unsigned int itextureID, int *width, int *height) {
  int i;
  for (i = 0; i < _s2_ctext_count; i++) {
    if (_s2_ctext[i].id == itextureID) {
      if (width) {
	*width = _s2_ctext[i].width;
      }
      if (height) {
	*height = _s2_ctext[i].height;
      }
      return (unsigned char *)(_s2_ctext[i].bitmap);
    }
  }
  return NULL;
}
#if defined(S2_3D_TEXTURES)
/* 3d texture version */
unsigned char *ss2g3dt(unsigned int itextureID, int *width, int *height, int *depth) {
  int i;
  for (i = 0; i < _s2_ctext_count; i++) {
    if (_s2_ctext[i].id == itextureID) {
      if (width) {
	*width = _s2_ctext[i].width;
      }
      if (height) {
	*height = _s2_ctext[i].height;
      }
      if (depth) {
	*depth = _s2_ctext[i].depth;
      }
      return (unsigned char *)(_s2_ctext[i].bitmap);
    }
  }
  return NULL;
}
#endif




/***********************************************************************
 *
 * "NATIVE" S2 FUNCTIONS
 *
 ***********************************************************************
 */

/* draw sphere */
void ns2sphere(float ix, float iy, float iz, 
	       float ir,
	       float ired, float igreen, float iblue) {
  XYZ P = {ix, iy, iz};
#if defined(S2TRIPLEFLOAT)
  COLOUR C = {ired, igreen, iblue, 1.};
#else
  COLOUR C = {ired, igreen, iblue};
#endif
  ns2vsphere(P, ir, C);
  return;
}
void ns2vsphere(XYZ iP, float ir, COLOUR icol) {
  BALL *ball_base = _s2priv_addballs(1);
  if (!ball_base) {
    _s2error("ns2*sphere", "could not allocate memory for sphere");
  }
  ball_base->p.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  ball_base->p.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  ball_base->p.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  ball_base->r = _S2W3RADIUS(ir);
  ball_base->colour = icol;
  strcpy(ball_base->whichscreen, _s2_whichscreen);
  strncpy(ball_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  ball_base->VRMLname[MAXVRMLLEN-1] = '\0';
  return;
}

/* draw textured sphere */
void ns2spheret(float ix, float iy, float iz,
		float ir,
		float ired, float igreen, float iblue,
		char *texturefn) {
  XYZ P = {ix, iy, iz};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {ired, igreen, iblue, 1.};
#else
	COLOUR C = {ired, igreen, iblue};
#endif
  ns2vspheret(P, ir, C, texturefn);
  return;
}
void ns2vspheret(XYZ iP, float ir, COLOUR icol, char *itexturefn) {
  float texture_phase = 0.;
  XYZ axis = {0.,0.,0.};
  float rotation = 0.;
  ns2vplanett(iP, ir, icol, itexturefn, texture_phase, axis, rotation);
}
void ns2vplanett(XYZ iP, float ir, COLOUR icol, char *itexturefn,
		 float texture_phase, XYZ axis, float rotation) {
  
  if ((ballt = (BALLT *)realloc(ballt, (nballt + 1)*sizeof(BALLT))) == NULL) {
    _s2error("ns2*spheret/ns2*planett", 
	     "failed to allocate memory for textured ball");
  }
  ballt[nballt].p.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  ballt[nballt].p.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  ballt[nballt].p.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  ballt[nballt].r = _S2W3RADIUS(ir);
  ballt[nballt].colour = icol;
  strcpy(ballt[nballt].whichscreen, _s2_whichscreen);
  strcpy(ballt[nballt].texturename, itexturefn);
  
  /* Check to see if the texture already exists */
  int gotitalready = FALSE;
  int i;
  for (i=0;i<nballt;i++) {
    if (strcmp(ballt[i].texturename,itexturefn) == 0) {
      gotitalready = TRUE;
      break;
    }
  }
  if (gotitalready) {
    ballt[nballt].rgba = NULL;
    ballt[nballt].textureid = ballt[i].textureid;
  } else {
    ballt[nballt].rgba = NULL;
    ballt[nballt].textureid = s2loadtexture(itexturefn);
  }

  ballt[nballt].texture_phase = texture_phase;
  ballt[nballt].axis.x = _S2WORLD2DEVICE_SO(axis.x, _S2XAX);
  ballt[nballt].axis.y = _S2WORLD2DEVICE_SO(axis.y, _S2YAX);
  ballt[nballt].axis.z = _S2WORLD2DEVICE_SO(axis.z, _S2ZAX);
  Normalise(&ballt[nballt].axis);
  ballt[nballt].rotation = rotation;

  nballt++;

  return;
}

/* draw textured sphere using texture ID cached by S2PLOT */
void ns2spherex(float ix, float iy, float iz, float ir,
		float ired, float igreen, float iblue,
		unsigned int itexid) {
  XYZ P = {ix, iy, iz};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {ired, igreen, iblue, 1.};
#else
	COLOUR C = {ired, igreen, iblue};
#endif
  ns2vspherex(P, ir, C, itexid);
  return;
}
void ns2vspherex(XYZ iP, float ir, COLOUR icol, 
		  unsigned int itextureid) {
  float texture_phase = 0.;
  XYZ axis = {0.,0.,0.};
  float rotation = 0.;
  ns2vplanetx(iP, ir, icol, itextureid, texture_phase, axis, rotation);
}
void ns2vplanetx(XYZ iP, float ir, COLOUR icol, 
		 unsigned int itextureid,
		 float texture_phase, XYZ axis, float rotation) {
  
  if ((ballt = (BALLT *)realloc(ballt, (nballt + 1)*sizeof(BALLT))) == NULL) {
    _s2error("ns2*spherex/ns2*planetx", 
	     "failed to allocate memory for textured ball");
  }
  ballt[nballt].p.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  ballt[nballt].p.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  ballt[nballt].p.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  ballt[nballt].r = _S2W3RADIUS(ir);
  ballt[nballt].colour = icol;
  strcpy(ballt[nballt].whichscreen, _s2_whichscreen);
  strcpy(ballt[nballt].texturename, "<cached>");
  
  ballt[nballt].rgba = NULL;
  ballt[nballt].textureid = (unsigned int)itextureid; // wasGL

  ballt[nballt].texture_phase = texture_phase;
  ballt[nballt].axis.x = _S2WORLD2DEVICE_SO(axis.x, _S2XAX);
  ballt[nballt].axis.y = _S2WORLD2DEVICE_SO(axis.y, _S2YAX);
  ballt[nballt].axis.z = _S2WORLD2DEVICE_SO(axis.z, _S2ZAX);
  Normalise(&ballt[nballt].axis);
  ballt[nballt].rotation = rotation;

  nballt++;
  return;
}

/* draw a disk with centre and normal */
void ns2disk(float ix, float iy, float iz, 
	     float inx, float iny, float inz,
	     float ir1, float ir2, 
	     float ired, float igreen, float iblue) {
  XYZ P = {ix, iy, iz};
  XYZ N = {inx, iny, inz};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {ired, igreen, iblue, 1.};
#else
	COLOUR C = {ired, igreen, iblue};
#endif
  ns2vdisk(P, N, ir1, ir2, C);
  return;
}
void ns2vdisk(XYZ iP, XYZ iN, float ir1, float ir2, COLOUR icol) {
  DISK *disk_base = _s2priv_adddisks(1);
  if (!disk_base) {
    _s2error("ns2*disk", "failed to allocate memory for disk");
  }
  disk_base->p.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  disk_base->p.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  disk_base->p.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  disk_base->r1 = _S2W3RADIUS(ir1);
  disk_base->r2 = _S2W3RADIUS(ir2);
  disk_base->colour = icol;
  disk_base->n.x = iN.x;
  disk_base->n.y = iN.y;
  disk_base->n.z = iN.z;
  strcpy(disk_base->whichscreen, _s2_whichscreen);
  return;
}

/* draw an arbitrarily oriented disk/circle via vector rotation */
void ns2arc(float ipx, float ipy, float ipz, 
	    float inx, float iny, float inz,
	    float isx, float isy, float isz,
	    float ideg, int inseg) {
  ns2erc(ipx, ipy, ipz, inx, iny, inz, isx, isy, isz,
	 ideg, inseg, 1.0);
}
void ns2varc(XYZ iP, XYZ iN, XYZ iS, float ideg, int inseg) {
  ns2verc(iP, iN, iS, ideg, inseg, 1.0);
}
void ns2erc(float ipx, float ipy, float ipz, 
	    float inx, float iny, float inz,
	    float isx, float isy, float isz,
	    float ideg, int inseg, float axratio) {
  XYZ P = {ipx, ipy, ipz};
  XYZ N = {inx, iny, inz};
  XYZ S = {isx, isy, isz};
  ns2verc(P, N, S, ideg, inseg, axratio);
}
void ns2verc(XYZ iP, XYZ iN, XYZ iS, float ideg, int inseg,
	     float axratio) {
  if (inseg < 3) {
    _s2warn("ns2*rc", "cannot draw an arc with < 2 segments");
    return;
  }
  inseg++;

  // allocate space for points along the arc
  float *xpts, *ypts, *zpts;
  xpts = (float *)calloc(inseg, sizeof(float));
  ypts = (float *)calloc(inseg, sizeof(float));
  zpts = (float *)calloc(inseg, sizeof(float));

  // calculate the offset vector which we use to modify the circle
  // traced out by vector rotation to an ellipse
  XYZ shat = iS;
  Normalise(&shat);
  XYZ nhat = iN;
  Normalise(&nhat);
  XYZ kvec = CrossProduct(nhat, shat);
  SetVectorLength(&kvec, (1. - 1. / axratio) * 
		  sqrt(DotProduct(iS,iS)*DotProduct(iS,iS) -
		       DotProduct(iS,nhat)*DotProduct(iS,nhat)));

  float theta;
  int i;
  XYZ drawpt;
  float sinth;
  for (i = 0; i < inseg; i++) {
    theta = ideg * DTOR * (float)i / (float)(inseg - 1);
    sinth = sin(theta);
    drawpt = ArbitraryRotate(iS, theta, iN);
    xpts[i] = iP.x + drawpt.x - kvec.x * sinth;
    ypts[i] = iP.y + drawpt.y - kvec.y * sinth;
    zpts[i] = iP.z + drawpt.z - kvec.z * sinth;
  }
  s2line(inseg, xpts, ypts, zpts);
  free(xpts);
  free(ypts);
  free(zpts);
  return;
}

/* draw text */
void ns2text(float ix, float iy, float iz, 
	     float irx, float iry, float irz,
	     float iux, float iuy, float iuz,
	     float ired, float igreen, float iblue,
	     char *itext) {
  XYZ P = {ix, iy, iz};
  XYZ R = {irx, iry, irz};
  XYZ U = {iux, iuy, iuz};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {ired, igreen, iblue, 1.};
#else
	COLOUR C = {ired, igreen, iblue};
#endif
  ns2vtext(P, R, U, C, itext);
  return;
}
void ns2vtext(XYZ iP, XYZ iR, XYZ iU, COLOUR icol, char *itext) {
  LABEL *label_base = _s2priv_addlabels(1);
  if (!label_base) {
    _s2error("ns2*text", "failed to allocate memory for text label");
  }
  label_base[0].p.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  label_base[0].p.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  label_base[0].p.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  label_base[0].right.x = _S2WORLD2DEVICE_SO(iR.x, _S2XAX);
  label_base[0].right.y = _S2WORLD2DEVICE_SO(iR.y, _S2YAX);
  label_base[0].right.z = _S2WORLD2DEVICE_SO(iR.z, _S2ZAX);
  label_base[0].up.x = _S2WORLD2DEVICE_SO(iU.x, _S2XAX);
  label_base[0].up.y = _S2WORLD2DEVICE_SO(iU.y, _S2YAX);
  label_base[0].up.z = _S2WORLD2DEVICE_SO(iU.z, _S2ZAX);
  label_base[0].colour = icol;
  strncpy(label_base[0].s, itext, MAXLABELLEN);
  label_base[0].s[MAXLABELLEN-1] = '\0';
  strcpy(label_base[0].whichscreen, _s2_whichscreen);
  strncpy(label_base[0].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  label_base[0].VRMLname[MAXVRMLLEN-1] = '\0';
  return;
}

/* draw a point (or points) */
void ns2point(float ix, float iy, float iz,
	      float ired, float igreen, float iblue) {
  ns2thpoint(ix, iy, iz, ired, igreen, iblue, 1.);
  return;
}
void ns2vpoint(XYZ iP, COLOUR icol) {
  ns2vthpoint(iP, icol, 1.);
  return;
}
void ns2vnpoint(XYZ *iP, COLOUR icol, int in) {
  int i;
  for (i = 0; i < in; i++) {
    ns2vpoint(iP[i], icol);
  }
  return;
}
void ns2thpoint(float ix, float iy, float iz,
		float ired, float igreen, float iblue,
		float isize) {
  XYZ P = {ix, iy, iz};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {ired, igreen, iblue, 1.};
#else
	COLOUR C = {ired, igreen, iblue};
#endif
  ns2vthpoint(P, C, isize);
  return;
}
void ns2vthpoint(XYZ iP, COLOUR icol, float isize) {
  if (_s2_clipping && !S2INWORLD(iP)) {
    return;
  }

  DOT *dot_base = _s2priv_adddots(1);
  if (!dot_base) {
    _s2error("ns2*point", "failed to allocate memory for dot");
  }
  dot_base->p.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  dot_base->p.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  dot_base->p.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  dot_base->colour = icol;
  dot_base->size = isize;
  strcpy(dot_base->whichscreen, _s2_whichscreen);
  strncpy(dot_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  dot_base->VRMLname[MAXVRMLLEN-1] = '\0';
  return;
}

void ns2vpa(XYZ iP, COLOUR icol, float isize, char itrans, float ialpha) {
  if (_s2_clipping && !S2INWORLD(iP)) {
    return;
  }
  TRDOT *trdot_base = _s2priv_addtrdots(1);
  if (!trdot_base) {
    _s2error("ns2vpa", "failed to allocate memory for transparent dot");
  }
  trdot_base->p.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  trdot_base->p.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  trdot_base->p.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  trdot_base->colour = icol;
  trdot_base->size = isize;
  trdot_base->trans = itrans;
  trdot_base->alpha = ialpha;
  strcpy(trdot_base->whichscreen, _s2_whichscreen);
  return;
}

/* illumination */
void ns2i(float ix, float iy, float iz,
	  float ired, float igreen, float iblue) {
  XYZ P = {ix, iy, iz};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {ired, igreen, iblue, 1.};
#else
	COLOUR C = {ired, igreen, iblue};
#endif
  ns2vi(P, C);
  return;
}
void ns2vi(XYZ iP, COLOUR icol) {
  if (nlight < 0) {
    nlight = 0;
  }
  //fprintf(stderr, "in ns2vi: nlight = %d\n", nlight);
  XYZ oP;
  oP.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  oP.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  oP.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  if (nlight < MAXLIGHT) {
    lights[nlight].p[0] = oP.x;
    lights[nlight].p[1] = oP.y;
    lights[nlight].p[2] = oP.z;
    lights[nlight].p[3] = 1; /* positional light  flag */
    lights[nlight].c[0] = icol.r;
    lights[nlight].c[1] = icol.g;
    lights[nlight].c[2] = icol.b;
    lights[nlight].c[3] = 1;
    nlight++;
  }
  return;
}

/* simple line */
void ns2line(float ix1, float iy1, float iz1, 
	     float ix2, float iy2, float iz2,
	     float ired, float igreen, float iblue) {
  ns2thline(ix1, iy1, iz1, ix2, iy2, iz2, ired, igreen, iblue, 1.);
  return;
}
void ns2vline(XYZ iP1, XYZ iP2, COLOUR icol) {
  ns2vthline(iP1, iP2, icol, 1.);
  return;
}

/* thick line */
void ns2thline(float ix1, float iy1, float iz1, 
	       float ix2, float iy2, float iz2,
	       float ired, float igreen, float iblue,
	       float width) {
  XYZ P1 = {ix1, iy1, iz1};
  XYZ P2 = {ix2, iy2, iz2};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {ired, igreen, iblue, 1.};
#else
	COLOUR C = {ired, igreen, iblue};
#endif
  ns2vthline(P1, P2, C, width);
}
void ns2vthline(XYZ iP1, XYZ iP2, COLOUR icol, float iwid) {
  LINE *line_base = _s2priv_addlines(1);
  if (!line_base) {
    _s2error("ns2*line", "failed to allocate memory for line");
  }
  line_base->p[0].x = _S2WORLD2DEVICE(iP1.x, _S2XAX);
  line_base->p[0].y = _S2WORLD2DEVICE(iP1.y, _S2YAX);
  line_base->p[0].z = _S2WORLD2DEVICE(iP1.z, _S2ZAX);
  line_base->p[1].x = _S2WORLD2DEVICE(iP2.x, _S2XAX);
  line_base->p[1].y = _S2WORLD2DEVICE(iP2.y, _S2YAX);
  line_base->p[1].z = _S2WORLD2DEVICE(iP2.z, _S2ZAX);
  line_base->colour[0] = icol;
  line_base->colour[1] = icol;
  line_base->width = iwid;
  strcpy(line_base->whichscreen, _s2_whichscreen);
  strncpy(line_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  line_base->VRMLname[MAXVRMLLEN-1] = '\0';
  line_base->stipple_factor = 0;
  line_base->stipple_pattern = 0;
  line_base->alpha = 1.0;
  return;
}

/* Draw a wireframe cube (with axes parallel to the coordinate axes)
 * in a specific colour and thickness.*/
void ns2thwcube(float ix1, float iy1, float iz1, 
		float ix2, float iy2, float iz2,
		float ired, float igreen, float iblue,
		float iwidth) {
  XYZ P1 = {ix1, iy1, iz1};
  XYZ P2 = {ix2, iy2, iz2};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {ired, igreen, iblue, 1.};
#else
	COLOUR C = {ired, igreen, iblue};
#endif
  ns2vthwcube(P1, P2, C, iwidth);
}
void ns2vthwcube(XYZ P1, XYZ P2, COLOUR col, float width) {
  ns2thline(P1.x, P1.y, P1.z,   P1.x, P1.y, P2.z, col.r, col.g, col.b, width);
  ns2thline(P1.x, P2.y, P1.z,   P1.x, P2.y, P2.z, col.r, col.g, col.b, width);
  ns2thline(P2.x, P2.y, P1.z,   P2.x, P2.y, P2.z, col.r, col.g, col.b, width);
  ns2thline(P2.x, P1.y, P1.z,   P2.x, P1.y, P2.z, col.r, col.g, col.b, width);

  ns2thline(P1.x, P1.y, P1.z,   P1.x, P2.y, P1.z, col.r, col.g, col.b, width);
  ns2thline(P1.x, P1.y, P2.z,   P1.x, P2.y, P2.z, col.r, col.g, col.b, width);
  ns2thline(P2.x, P1.y, P2.z,   P2.x, P2.y, P2.z, col.r, col.g, col.b, width);
  ns2thline(P2.x, P1.y, P1.z,   P2.x, P2.y, P1.z, col.r, col.g, col.b, width);

  ns2thline(P1.x, P1.y, P1.z,   P2.x, P1.y, P1.z, col.r, col.g, col.b, width);
  ns2thline(P1.x, P1.y, P2.z,   P2.x, P1.y, P2.z, col.r, col.g, col.b, width);
  ns2thline(P1.x, P2.y, P2.z,   P2.x, P2.y, P2.z, col.r, col.g, col.b, width);
  ns2thline(P1.x, P2.y, P1.z,   P2.x, P2.y, P1.z, col.r, col.g, col.b, width);
}



/* coloured line */
void ns2cline(float ix1, float iy1, float iz1, 
	      float ix2, float iy2, float iz2,
	      float ired1, float igreen1, float iblue1,
	      float ired2, float igreen2, float iblue2) {
  ns2thcline(ix1, iy1, iz1, ix2, iy2, iz2, 
	     ired1, igreen1, iblue1, ired2, igreen2, iblue2,
	     1.0);
}
void ns2thcline(float ix1, float iy1, float iz1, 
		float ix2, float iy2, float iz2,
		float ired1, float igreen1, float iblue1,
		float ired2, float igreen2, float iblue2,
		float iwid) {
  XYZ P1 = {ix1, iy1, iz1};
  XYZ P2 = {ix2, iy2, iz2};
#if defined(S2TRIPLEFLOAT)
  COLOUR C1 = {ired1, igreen1, iblue2, 1.};
#else
  COLOUR C1 = {ired1, igreen1, iblue2};
#endif
#if defined(S2TRIPLEFLOAT)
  COLOUR C2 = {ired2, igreen2, iblue2, 1.};
#else
  COLOUR C2 = {ired2, igreen2, iblue2};
#endif
  ns2vthcline(P1, P2, C1, C2, iwid);
  return;
}
void ns2vcline(XYZ iP1, XYZ iP2, COLOUR icol1, COLOUR icol2) {
  ns2vthcline(iP1, iP2, icol1, icol2, 1.);
}
void ns2vthcline(XYZ iP1, XYZ iP2, COLOUR icol1, COLOUR icol2,
		 float iwid) {
  LINE *line_base = _s2priv_addlines(1);
  if (!line_base) {
    _s2error("ns2*cline", "failed to allocate memory for line");
  }
  line_base->p[0].x = _S2WORLD2DEVICE(iP1.x, _S2XAX);
  line_base->p[0].y = _S2WORLD2DEVICE(iP1.y, _S2YAX);
  line_base->p[0].z = _S2WORLD2DEVICE(iP1.z, _S2ZAX);
  line_base->p[1].x = _S2WORLD2DEVICE(iP2.x, _S2XAX);
  line_base->p[1].y = _S2WORLD2DEVICE(iP2.y, _S2YAX);
  line_base->p[1].z = _S2WORLD2DEVICE(iP2.z, _S2ZAX);
  line_base->colour[0] = icol1;
  line_base->colour[1] = icol2;
  line_base->width = iwid;
  strcpy(line_base->whichscreen, _s2_whichscreen);
  strncpy(line_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  line_base->VRMLname[MAXVRMLLEN-1] = '\0';
  line_base->stipple_factor = 0;
  line_base->stipple_pattern = 0;
  line_base->alpha = 1.0;
  return;
}



/* a 3-vertex facet with auto-normal and only one colour */
void ns2vf3(XYZ *iP, COLOUR icol) {
  COLOUR ocol[3];
  int i;
  for (i = 0; i < 3; i++) {
    ocol[i] = icol;
  }
  ns2vf3c(iP, ocol);
  return;
}
/* a 3-vertex facet with normals supplied but only one colour */
void ns2vf3n(XYZ *iP, XYZ *iN, COLOUR icol) {
  COLOUR ocol[3];
  int i;
  for (i = 0; i < 3; i++) {
    ocol[i] = icol;
  }
  ns2vf3nc(iP, iN, ocol);
  return;
}
/* a 3-vertex facet with colours supplied but auto-normal */
void ns2vf3c(XYZ *iP, COLOUR *icol) {
  XYZ oN[3];
  int i;
  for (i = 0; i < 3; i++) {
    oN[i] = CalcNormal(iP[0], iP[1], iP[2]);
  }
  ns2vf3nc(iP, oN, icol);
  return;
}
/* the completely general 3-vertex facet */
void ns2vf3nc(XYZ *iP, XYZ *iN, COLOUR *icol) {
  FACE3 *face3_base = _s2priv_addface3s(1);
  if (!face3_base) {
    _s2error("ns2*f3*", "failed to allocate memory for facet");
  }
  int i;
  for (i = 0; i < 3; i++) {
    face3_base->p[i].x = _S2WORLD2DEVICE(iP[i].x, _S2XAX);
    face3_base->p[i].y = _S2WORLD2DEVICE(iP[i].y, _S2YAX);
    face3_base->p[i].z = _S2WORLD2DEVICE(iP[i].z, _S2ZAX);
    // _SO added Jan2012 dbarnes
    face3_base->n[i].x = _S2WORLD2DEVICE_SO(iN[i].x, _S2XAX);
    face3_base->n[i].y = _S2WORLD2DEVICE_SO(iN[i].y, _S2YAX);
    face3_base->n[i].z = _S2WORLD2DEVICE_SO(iN[i].z, _S2ZAX);
    /* this has to be done because normals that are unit length in 
     * the world coord system can be non-unit in the device units.
     */
    Normalise(&(face3_base->n[i]));

    face3_base->colour[i] = icol[i];
    strcpy(face3_base->whichscreen, _s2_whichscreen);
    strncpy(face3_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
    face3_base->VRMLname[MAXVRMLLEN-1] = '\0';
  }
  return;
}

/* a 4-vertex facet with auto-normal and only one colour */
void ns2vf4(XYZ *iP, COLOUR icol) {
  COLOUR ocol[4];
  int i;
  for (i = 0; i < 4; i++) {
    ocol[i] = icol;
  }
  ns2vf4c(iP, ocol);
  return;
}
/* a 4-vertex facet with normals supplied but only one colour */
void ns2vf4n(XYZ *iP, XYZ *iN, COLOUR icol) {
  COLOUR ocol[4];
  int i;
  for (i = 0; i < 4; i++) {
    ocol[i] = icol;
  }
  ns2vf4nc(iP, iN, ocol);
  return;
}
/* a 4-vertex facet with colours supplied but auto-normal */
void ns2vf4c(XYZ *iP, COLOUR *icol) {
  XYZ oN[4];
  int i;
  for (i = 0; i < 4; i++) {
    oN[i] = CalcNormal(iP[(i-1+4)%4], iP[i], iP[(i+1)%4]);
  }
  ns2vf4nc(iP, oN, icol);
  return;
}
/* the completely general 4-vertex facet */
void ns2vf4nc(XYZ *iP, XYZ *iN, COLOUR *icol) {
  FACE4 *face4_base = _s2priv_addface4s(1);
  if (!face4_base) {
    _s2error("ns2*f4*", "failed to allocate memory for facet");
  }
  int i;
  for (i = 0; i < 4; i++) {
    face4_base->p[i].x = _S2WORLD2DEVICE(iP[i].x, _S2XAX);
    face4_base->p[i].y = _S2WORLD2DEVICE(iP[i].y, _S2YAX);
    face4_base->p[i].z = _S2WORLD2DEVICE(iP[i].z, _S2ZAX);
    face4_base->n[i].x = _S2WORLD2DEVICE(iN[i].x, _S2XAX);
    face4_base->n[i].y = _S2WORLD2DEVICE(iN[i].y, _S2YAX);
    face4_base->n[i].z = _S2WORLD2DEVICE(iN[i].z, _S2ZAX);
    /* this has to be done because normals that are unit length in 
     * the world coord system can be non-unit in the device units.
     */
    Normalise(&(face4_base->n[i]));

    face4_base->colour[i] = icol[i];
    strcpy(face4_base->whichscreen, _s2_whichscreen);
    strncpy(face4_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
    face4_base->VRMLname[MAXVRMLLEN-1] = '\0';
  }
  return;
}

/* textured 4-vertex facet */
void ns2vf4t(XYZ *iP, COLOUR icol, char *itexturefn, float iscale, 
	     char itrans) {
  FACE4T *face4t_base = _s2priv_addface4ts(1);
  if (!face4t_base) {
    _s2error("ns2*f4*", "failed to allocate memory for facet");
  }

  int i;
  for (i = 0; i < 4; i++) {
    face4t_base->p[i].x = _S2WORLD2DEVICE(iP[i].x, _S2XAX);
    face4t_base->p[i].y = _S2WORLD2DEVICE(iP[i].y, _S2YAX);
    face4t_base->p[i].z = _S2WORLD2DEVICE(iP[i].z, _S2ZAX);
  }
  face4t_base->colour = icol;
  face4t_base->trans = itrans;
  face4t_base->scale = iscale;
  face4t_base->alpha = 1.0;
  strcpy(face4t_base->whichscreen, _s2_whichscreen);
  strncpy(face4t_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  face4t_base->VRMLname[MAXVRMLLEN-1] = '\0';
  strcpy(face4t_base->texturename, itexturefn);
  
  int gotitalready = FALSE;
  for (i = 0; i < nface4t-1; i++) {
    if (strcmp(face4t[i].texturename, itexturefn)) {
      gotitalready = TRUE;
      break;
    }
  }
  if (gotitalready) {
    face4t_base->rgba = NULL;
    face4t_base->textureid = face4t[i].textureid;
  } else {
    face4t_base->rgba = NULL;
    face4t_base->textureid = s2loadtexture(itexturefn);
  }
  return;
}

/* textured 4-vertex facet */
void ns2vf4x(XYZ *iP, COLOUR icol, unsigned int itextureid, float iscale, 
	      char itrans) {
  ns2vf4xt(iP, icol, itextureid, iscale, itrans, 1.0);
}

void ns2vf4xt(XYZ *iP, COLOUR icol, unsigned int itextureid, float iscale, 
	       char itrans, float ialpha) {
  FACE4T *face4t_base = _s2priv_addface4ts(1);
  if (!face4t_base) {
    _s2error("ns2*f4*", "failed to allocate memory for facet");
  }

  int i;
  for (i = 0; i < 4; i++) {
    face4t_base->p[i].x = _S2WORLD2DEVICE(iP[i].x, _S2XAX);
    face4t_base->p[i].y = _S2WORLD2DEVICE(iP[i].y, _S2YAX);
    face4t_base->p[i].z = _S2WORLD2DEVICE(iP[i].z, _S2ZAX);
  }
  face4t_base->colour = icol;
  face4t_base->trans = itrans;
  face4t_base->scale = iscale;
  face4t_base->alpha = ialpha;
  strcpy(face4t_base->whichscreen, _s2_whichscreen);
  strcpy(face4t_base->texturename, "<cached>");
  
  face4t_base->rgba = NULL;
  face4t_base->textureid = itextureid;
  strcpy(face4t_base->whichscreen, _s2_whichscreen);
  strncpy(face4t_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  face4t_base->VRMLname[MAXVRMLLEN-1] = '\0';
  return;
}

/* solid cube */
void ns2scube(float ix1, float iy1, float iz1,
	      float ix2, float iy2, float iz2,
	      float ired, float igreen, float iblue,
	      float ialpha) {
  XYZ P1 = {ix1, iy1, iz1};
  XYZ P2 = {ix2, iy2, iz2};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {ired, igreen, iblue, 1.};
#else
	COLOUR C = {ired, igreen, iblue};
#endif
  ns2vscube(P1, P2, C, ialpha);
}
void ns2vscube(XYZ iP1, XYZ iP2, COLOUR icol, float ialpha) {
  char itrans = 't';
  XYZ tri[3];

  // z planes
  tri[0] = tri[1] = tri[2] = iP1;
  tri[1].y = iP2.y;
  tri[2].x = iP2.x;
  ns2vf3a(tri, icol, itrans, ialpha);
  tri[0].z = tri[1].z = tri[2].z = iP2.z;
  ns2vf3a(tri, icol, itrans, ialpha);

  tri[0] = tri[1] = tri[2] = iP1;
  tri[1].x = iP2.x;
  tri[2].y = iP2.y;
  tri[0].x = iP2.x;
  tri[0].y = iP2.y;
  ns2vf3a(tri, icol, itrans, ialpha);
  tri[0].z = tri[1].z = tri[2].z = iP2.z;
  ns2vf3a(tri, icol, itrans, ialpha);

  // y planes
  tri[0] = tri[1] = tri[2] = iP1;
  tri[1].z = iP2.z;
  tri[2].x = iP2.x;
  ns2vf3a(tri, icol, itrans, ialpha);
  tri[0].y = tri[1].y = tri[2].y = iP2.y;
  ns2vf3a(tri, icol, itrans, ialpha);

  tri[0] = tri[1] = tri[2] = iP1;
  tri[1].x = iP2.x;
  tri[2].z = iP2.z;
  tri[0].x = iP2.x;
  tri[0].z = iP2.z;
  ns2vf3a(tri, icol, itrans, ialpha);
  tri[0].y = tri[1].y = tri[2].y = iP2.y;
  ns2vf3a(tri, icol, itrans, ialpha);

  // x planes
  tri[0] = tri[1] = tri[2] = iP1;
  tri[1].y = iP2.y;
  tri[2].z = iP2.z;
  ns2vf3a(tri, icol, itrans, ialpha);
  tri[0].x = tri[1].x = tri[2].x = iP2.x;
  ns2vf3a(tri, icol, itrans, ialpha);

  tri[0] = tri[1] = tri[2] = iP1;
  tri[1].z = iP2.z;
  tri[2].y = iP2.y;
  tri[0].z = iP2.z;
  tri[0].y = iP2.y;
  ns2vf3a(tri, icol, itrans, ialpha);
  tri[0].x = tri[1].x = tri[2].x = iP2.x;
  ns2vf3a(tri, icol, itrans, ialpha);
}

/* markers */
void ns2m(int itype, float isize, float ix, float iy, float iz,
	  float ired, float igreen, float iblue) {
  XYZ P = {ix, iy, iz};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {ired, igreen, iblue, 1.};
#else
	COLOUR C = {ired, igreen, iblue};
#endif
	ns2vm(itype, isize, P, C);
  return;
}
void ns2vm(int itype, float isize, XYZ iP, COLOUR icol) {
  XYZ oP;
  oP.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  oP.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  oP.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  float osize = _S2W3RADIUS(isize);
  AddMarker2Database(itype, osize, oP, icol);
  return;
}

#if defined(S2_3D_TEXTURES)
void ns2texpoly3d(XYZ *iP, XYZ *iTC, float in, 
		  unsigned int itexid, char itrans, float ialpha) {
  _S2TEXPOLY3D *texpoly3d_base = _s2priv_addtexpoly3ds(1);
  if (!texpoly3d_base) {
    _s2warn("ns2texpoly3d3d", "could not allocate memory for meshed texture");
    return;
  }
  
  texpoly3d_base->nverts = in;
  texpoly3d_base->verts = (XYZ *)calloc(in, sizeof(XYZ));
  texpoly3d_base->texcoords = (XYZ *)calloc(in, sizeof(XYZ));
 
  int i;
  for (i = 0; i < in; i++) {
    texpoly3d_base->verts[i].x = _S2WORLD2DEVICE(iP[i].x, _S2XAX);
    texpoly3d_base->verts[i].y = _S2WORLD2DEVICE(iP[i].y, _S2YAX);
    texpoly3d_base->verts[i].z = _S2WORLD2DEVICE(iP[i].z, _S2ZAX);
    texpoly3d_base->texcoords[i] = iTC[i];
  }

  texpoly3d_base->texid = itexid;
  texpoly3d_base->trans = itrans;
  texpoly3d_base->alpha = ialpha;
  strcpy(texpoly3d_base->whichscreen, _s2_whichscreen);
  strncpy(texpoly3d_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  texpoly3d_base->VRMLname[MAXVRMLLEN-1] = '\0';
  return;
}
#endif


void ns2texmesh(int inverts, XYZ *iverts,
		int innorms, XYZ *inorms,
		int invtcs, XYZ *ivtcs,
		int infacets, int *ifacets, int *ifacets_tcs,
		unsigned int itexid,
		char itrans,
		float ialpha) {
  _S2TEXTUREDMESH *texmesh_base = _s2priv_addtexturedmesh(1);
  if (!texmesh_base) {
    _s2warn("ns2texmesh", "could not allocate memory for meshed texture");
    return;
  }
  
  texmesh_base->nverts = inverts;
  texmesh_base->verts = (XYZ *)malloc(inverts * sizeof(XYZ));
  int i;
  for (i = 0; i < inverts; i++) {
    texmesh_base->verts[i].x = iverts[i].x; //_S2WORLD2DEVICE(iverts[i].x, _S2XAX);
    texmesh_base->verts[i].y = iverts[i].y; //_S2WORLD2DEVICE(iverts[i].y, _S2YAX);
    texmesh_base->verts[i].z = iverts[i].z; //_S2WORLD2DEVICE(iverts[i].z, _S2ZAX);
  }

  texmesh_base->nnorms = innorms;
  texmesh_base->norms = (XYZ *)malloc(innorms * sizeof(XYZ));
  for (i = 0; i < innorms; i++) {
    texmesh_base->norms[i].x = inorms[i].x; // _S2WORLD2DEVICE_SO(inorms[i].x, _S2XAX);
    texmesh_base->norms[i].y = inorms[i].y; //_S2WORLD2DEVICE_SO(inorms[i].y, _S2YAX);
    texmesh_base->norms[i].z = inorms[i].z; //_S2WORLD2DEVICE_SO(inorms[i].z, _S2ZAX);
    //Normalise(&(texmesh_base->norms[i]));
  }

  texmesh_base->nvtcs = invtcs;
  texmesh_base->vtcs = (XYZ *)malloc(invtcs * sizeof(XYZ));
  bcopy(ivtcs, texmesh_base->vtcs, invtcs * sizeof(XYZ));

  texmesh_base->nfacets = infacets;
  texmesh_base->facets = (int *)malloc(3 * infacets * sizeof(int));
  texmesh_base->facets_vtcs = (int *)malloc(3 * infacets * sizeof(int));
  bcopy(ifacets, texmesh->facets, 3 * infacets * sizeof(int));
  bcopy(ifacets_tcs, texmesh->facets_vtcs, 3 * infacets * sizeof(int));
  
  texmesh_base->texid = itexid;
  texmesh_base->trans = itrans;
  texmesh_base->alpha = ialpha;
  strcpy(texmesh_base->whichscreen, _s2_whichscreen);
  strncpy(texmesh_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  texmesh_base->VRMLname[MAXVRMLLEN-1] = '\0';

}



/* 3-vertex facet with transparency (alpha) */
void ns2vf3a(XYZ *iP, COLOUR icol, char itrans, float ialpha) { 
  XYZ oN[3];
  int i;
  for (i = 0; i < 3; i++) {
    oN[i] = CalcNormal(iP[0], iP[1], iP[2]);
  }
  ns2vf3na(iP, oN, icol, itrans, ialpha);
}
  
void ns2vf3na(XYZ *iP, XYZ *iN, COLOUR icol, char itrans, float ialpha) { 
  int i;
  _S2FACE3A *face3a_base = _s2priv_addface3as(1);
  if (!face3a_base) {
    _s2error("ns2*3*", "failed to allocate memory for facet");
  }
  for (i = 0; i < 3; i++) {
    // normal (3d) coords
    face3a_base->p[i].x = _S2WORLD2DEVICE(iP[i].x, _S2XAX);
    face3a_base->p[i].y = _S2WORLD2DEVICE(iP[i].y, _S2YAX);
    face3a_base->p[i].z = _S2WORLD2DEVICE(iP[i].z, _S2ZAX);
    face3a_base->n[i].x = _S2WORLD2DEVICE_SO(iN[i].x, _S2XAX);
    face3a_base->n[i].y = _S2WORLD2DEVICE_SO(iN[i].y, _S2YAX);
    face3a_base->n[i].z = _S2WORLD2DEVICE_SO(iN[i].z, _S2ZAX);
    Normalise(&(face3a_base->n[i]));
    face3a_base->colour[i] = icol;
    face3a_base->alpha[i] = ialpha;    
  }
  face3a_base->trans = itrans;
  strcpy(face3a_base->whichscreen, _s2_whichscreen);
  strncpy(face3a_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  face3a_base->VRMLname[MAXVRMLLEN-1] = '\0';
  return;
}

void ns2vf3nca(XYZ *iP, XYZ *iN, COLOUR *icol, char itrans, float *ialpha) { 
  int i;
  _S2FACE3A *face3a_base = _s2priv_addface3as(1);
  if (!face3a_base) {
    _s2error("ns2*3*", "failed to allocate memory for facet");
  }
  for (i = 0; i < 3; i++) {
    // normal (3d) coords
    face3a_base->p[i].x = _S2WORLD2DEVICE(iP[i].x, _S2XAX);
    face3a_base->p[i].y = _S2WORLD2DEVICE(iP[i].y, _S2YAX);
    face3a_base->p[i].z = _S2WORLD2DEVICE(iP[i].z, _S2ZAX);
    face3a_base->n[i].x = _S2WORLD2DEVICE_SO(iN[i].x, _S2XAX);
    face3a_base->n[i].y = _S2WORLD2DEVICE_SO(iN[i].y, _S2YAX);
    face3a_base->n[i].z = _S2WORLD2DEVICE_SO(iN[i].z, _S2ZAX);
    Normalise(&(face3a_base->n[i]));
    face3a_base->colour[i] = icol[i];
    face3a_base->alpha[i] = ialpha[i];    
  }
  face3a_base->trans = itrans;
  strcpy(face3a_base->whichscreen, _s2_whichscreen);
  strncpy(face3a_base->VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  face3a_base->VRMLname[MAXVRMLLEN-1] = '\0';
  return;
}

int ns2cis(float ***grid, 
	   int adim, int bdim, int cdim, 
	   int a1, int a2, int b1, int b2, int c1, int c2,
	   float *tr, float level, int resolution, 
	   char itrans, float ialpha, float red, float green, float blue) {
  
  _s2priv_colrfn_r = red;
  _s2priv_colrfn_g = green;
  _s2priv_colrfn_b = blue;
  return ns2cisc(grid, adim, bdim, cdim,
		 a1, a2, b1, b2, c1, c2, 
		 tr, level, resolution, itrans, ialpha, _s2priv_colrfn);
}

int ns2cisc(float ***grid, 
	    int adim, int bdim, int cdim, 
	    int a1, int a2, int b1, int b2, int c1, int c2,
	    float *tr, float level, int resolution, 
	    char itrans, float ialpha,
	    void(*fcol)(float*,float*,float*,float*,float*,float*)) {
  
  _s2_isosurfs = (_S2ISOSURFACE *)realloc(_s2_isosurfs, (_s2_nisosurf + 1) *
					 sizeof(_S2ISOSURFACE));

  _S2ISOSURFACE *it = _s2_isosurfs + _s2_nisosurf;

  it->descr.grptr = grid;
  it->descr.adim = adim;
  it->descr.bdim = bdim;
  it->descr.cdim = cdim;
  it->descr.a1 = a1;
  it->descr.a2 = a2;
  it->descr.b1 = b1;
  it->descr.b2 = b2;
  it->descr.c1 = c1;
  it->descr.c2 = c2;
  if (tr) {
    bcopy(tr, it->descr.tr, 12 * sizeof(float));
  } else {
    it->descr.tr[2] = it->descr.tr[3] = it->descr.tr[5] = 0.;
    it->descr.tr[7] = it->descr.tr[9] = it->descr.tr[10] = 0.;
    it->descr.tr[0] = it->descr.tr[4] = it->descr.tr[8] = 0.;
    it->descr.tr[1] = it->descr.tr[6] = it->descr.tr[11] = 1.0;
  }
  it->descr.local_tr[2] = it->descr.local_tr[3] = it->descr.local_tr[5] = 0.;
  it->descr.local_tr[7] = it->descr.local_tr[9] = it->descr.local_tr[10] = 0.;
  it->descr.local_tr[0] = it->descr.local_tr[4] = it->descr.local_tr[8] = 0.;
  it->descr.local_tr[1] = it->descr.local_tr[6] = it->descr.local_tr[11] = 1.0;

  it->descr.level = level;
  it->descr.resolution = resolution;
  it->descr.trans = itrans;
  it->descr.alpha = ialpha;
  it->descr.fcol = fcol;
  bcopy(&_s2priv_colrfn_r, &it->descr.red, sizeof(float));
  bcopy(&_s2priv_colrfn_g, &it->descr.green, sizeof(float));
  bcopy(&_s2priv_colrfn_b, &it->descr.blue, sizeof(float));

  it->grptr = grid;
  it->ntri = 0;
  it->trivert = NULL;
  it->normals = NULL;
  it->col = NULL;

  _s2_nisosurf++;

  return _s2_nisosurf - 1;
}

/* draw isosurface */
void ns2dis(int isid, int force) {
  if (isid >= _s2_nisosurf) {
    _s2warn("ns2dis", "invalid isosurface object (isid)");
    return;
  }
  
  // 1. regenerate surface (if nec.) 
  _s2priv_generate_isosurface(isid, force);

  // 2. draw the surface
  _S2ISOSURFACE *it = _s2_isosurfs + isid;
  _s2priv_drawTriangleCache(it);
}

/* change isosurface level */
void ns2sisl(int isid, float level) {
  if (isid >= _s2_nisosurf) {
    _s2warn("ns2sisl", "invalid isosurface object (isid)");
    return;
  }
  _S2ISOSURFACE *it = _s2_isosurfs + isid;
  it->descr.level = level;
}
/* change isosurface alpha  & transparency */
void ns2sisa(int isid, float alpha, char trans) {
  if (isid >= _s2_nisosurf) {
    _s2warn("ns2sisa", "invalid isosurface object (isid)");
    return;
  }
  _S2ISOSURFACE *it = _s2_isosurfs + isid;
  it->descr.alpha = alpha;
  it->descr.trans = trans;
}

/* change isosurface colour */
void ns2sisc(int isid, float r, float g, float b) {
  if (isid >= _s2_nisosurf) {
    _s2warn("ns2sisc", "invalid isosurface object (isid)");
    return;
  }
  _S2ISOSURFACE *it = _s2_isosurfs + isid;
  it->descr.red = r;
  it->descr.green = g;
  it->descr.blue = b;
}

/* set / query isosurface quality */
void ns2ssq(int hiq) {
  _s2_fastsurfaces = !hiq;
}
int ns2qsq(void) {
  return !_s2_fastsurfaces;
}

/* create a volume rendering "object" - linear alpha ramp */
int ns2cvr(float ***grid, 
	   int adim, int bdim, int cdim,
	   int a1, int a2, int b1, int b2, int c1, int c2,
	   float *tr, char trans, 
	   float datamin, float datamax,
	   float alphamin, float alphamax) {
  return _s2priv_ns2cvr(grid, adim, bdim, cdim,
			a1, a2, b1, b2, c1, c2,
			tr, trans, datamin, datamax, 
			alphamin, alphamax, NULL);
}

/* create a volume rendering "object" - functionally defined alpha */
int ns2cvra(float ***grid,
	    int adim, int bdim, int cdim,
	    int a1, int a2, int b1, int b2, int c1, int c2,
	    float *tr, char trans,
	    float datamin, float datamax,
	    float(*ialphafn)(float*)) {
  return _s2priv_ns2cvr(grid, adim, bdim, cdim,
			a1, a2, b1, b2, c1, c2,
			tr, trans, datamin, datamax, 
			0., 1., ialphafn);
}  

/* change vol render data / alpha range / level */
void ns2svrl(int vrid, float datamin, float datamax,
	     float alphamin, float alphamax) {
  if (vrid >= _s2_nvol) {
    _s2warn("ns2svrl", "invalid volume rendering object (vrid)");
    return;
  }
  _S2VRVOLUME *it = _s2_volumes + vrid;
  it->datamin = datamin;
  it->datamax = datamax;
  it->alphamin = alphamin;
  it->alphamax = alphamax;
}

void ns2svas(int mode) {
  _s2_vralphascaling = mode;
}
int ns2qvas(void) {
  return _s2_vralphascaling;
}

void ns2sevas(float sx, float sy, float sz) {
  _s2_evas_x = sx;
  _s2_evas_y = sy;
  _s2_evas_z = sz;
}

/***********************************************************************
 *
 * DYNAMIC-ONLY GEOMETRY FUNCTIONS ("PRIMITIVES")
 *
 ***********************************************************************
 */

/* add a billboard */
void ds2bb(float x, float y, float z, float str_x, float str_y, float str_z,
	   float isize, float r, float g, float b, unsigned int itextid,
	   float ialpha, char itrans) {
  if (!_s2_dynamicEnabled) {
    _s2warn("ds2bb", "called from non-dynamic mode");
    return;
  }
  XYZ oP = {x, y, z};
  XYZ str = {str_x, str_y, str_z};
#if defined(S2TRIPLEFLOAT)
	COLOUR C = {r, g, b, 1.};
#else
	COLOUR C = {r, g, b};
#endif
	ds2vbb(oP, str, isize, C, itextid, ialpha, itrans);
}
void ds2vbb(XYZ iP, XYZ iStretch, 
	    float size, COLOUR icol, unsigned int iid, 
	    float ialpha, char itrans) {
  XYZ offset = {0., 0., 0.};
  _s2priv_bb(iP, iStretch, offset, 1.0, size, 0.0, icol, iid, ialpha, itrans);
}
// aspect ratio version
void ds2vbbp(XYZ iP, XYZ ioffset, float iaspect,
	     float size, COLOUR icol, unsigned int iid, 
	     float ialpha, char itrans) {
  XYZ ZeroStretch = {0., 0., 0.};
  _s2priv_bb(iP, ZeroStretch, ioffset, iaspect, size, 0.0, 
	     icol, iid, ialpha, itrans);
}
// input position angle in degrees
void ds2vbbr(XYZ iP, XYZ iStretch, float size, float pangle, 
	     COLOUR icol, unsigned int iid, float ialpha, 
	     char itrans) {
  XYZ offset = {0., 0., 0.};
  _s2priv_bb(iP, iStretch, offset, 1.0, size, pangle, icol, iid, ialpha, itrans);
}
// aspect ratio + offset + angle
void ds2vbbpr(XYZ iP, XYZ offset, float aspect, float isize,
	      float ipangle, COLOUR iC, unsigned int itexid,
	      float alpha, char trans) {
  XYZ iStretch = {0., 0., 0.};
  _s2priv_bb(iP, iStretch, offset, aspect, isize, ipangle, 
	     iC, itexid, alpha, trans);
}


/* draw text facing camera */
void ds2tb(float x, float y, float z, float x_off, float y_off,
	   char *text, int scaletext) {
  if (!_s2_dynamicEnabled) {
    _s2warn("ds2tb", "called from non-dynamic mode");
    return;
  }
  XYZ oP = {x, y, z};
  XYZ off = {x_off, y_off, 0.0};
  ds2vtb(oP, off, text, scaletext);
}
void ds2vtb(XYZ iP, XYZ ioff, char *text, int scaletext) {
  if (!_s2_dynamicEnabled) {
    _s2warn("ds2vtb", "called from non-dynamic mode");
    return;
  }

  static XYZ CAMP, UP, VIEW, RGT;
  static float posn[3], labelr[3], labelu[3];
  //CAMP = camera.vp;
  //UP = camera.vu;
  //VIEW = camera.vd;
  ss2qc(&CAMP, &UP, &VIEW, 0);
  CROSSPROD(VIEW, UP, RGT);

  CAMP.x -= _S2WORLD2DEVICE(iP.x, _S2XAX);
  CAMP.y -= _S2WORLD2DEVICE(iP.y, _S2YAX);
  CAMP.z -= _S2WORLD2DEVICE(iP.z, _S2ZAX);
  float sca;
  if (scaletext) {
    sca = Modulus(CAMP) / camera.focallength;
  } else {
    sca = 1.0;
  }

  SetVectorLength(&RGT, _s2_charsize * ioff.x * sca);
  SetVectorLength(&UP, _s2_charsize * ioff.y * sca);
  
  iP = VectorAdd(iP, RGT);
  iP = VectorAdd(iP, UP);

  SetVectorLength(&RGT, _s2_charsize * sca);
  SetVectorLength(&UP, _s2_charsize * sca);

  posn[0] = iP.x;
  posn[1] = iP.y;
  posn[2] = iP.z;
  labelr[0] = _S2DEVICE2WORLD_SO(RGT.x, _S2XAX);
  labelr[1] = _S2DEVICE2WORLD_SO(RGT.y, _S2YAX);
  labelr[2] = _S2DEVICE2WORLD_SO(RGT.z, _S2ZAX);
  labelu[0] = _S2DEVICE2WORLD_SO(UP.x, _S2XAX);
  labelu[1] = _S2DEVICE2WORLD_SO(UP.y, _S2YAX);
  labelu[2] = _S2DEVICE2WORLD_SO(UP.z, _S2ZAX);
  _s2priv_text(posn, labelr, labelu, text);

}

void ds2protect(void) {
  _s2_dynamic_erase = 0;
}
void ds2unprotect(void) {
  _s2_dynamic_erase = 1;
}
int ds2isprotected(void) {
  return !_s2_dynamic_erase;
}

#define CAMEPS 0.01
int CamChange(CAMERA c1, CAMERA c2) {
  if (VectorLength(c1.vp, c2.vp) > CAMEPS) {
    return 1;
  }
  if (VectorLength(c1.vd, c2.vd) > CAMEPS) {
    return 1;
  }
  if (VectorLength(c1.vu, c2.vu) > CAMEPS) {
    return 1;
  }
  return 0;
}

//#define PROGRESSIVERENDER 1
#define MAXSTEPS 3
/* draw vol ren "object" */
void ds2dvr(int vrid, int force) {
  if (!_s2_dynamicEnabled) {
    _s2warn("ds2dvr", "called from non-dynamic mode");
    return;
  }
  if (vrid >= _s2_nvol) {
    _s2warn("ds2dvr", "invalid volume rendering object (vrid)");
    return;
  }
#if defined(PROGRESSIVERENDER)
#define SLICE_TRANS 0.4
#else
#define SLICE_TRANS 0.9
#endif
  float strans = SLICE_TRANS;

  int sstep = 1; // slice step (1 = skip no slices)

#if defined(PROGRESSIVERENDER)
  static int beenhere = 0;
  static int slicestep;
  static int stillframes;
  static CAMERA previous_camera;
  if (!beenhere || force) {
    // first time, or forced redraw: reset
    slicestep = 1 << (MAXSTEPS-1);
    previous_camera = camera;
    beenhere = 1;
    stillframes = 0;
  } else if (CamChange(previous_camera, camera)) {
    // camera has moved: increase step
    slicestep *= 2;
    slicestep = slicestep > (1 << (MAXSTEPS-1)) ? (1 << (MAXSTEPS-1)) : slicestep;
    stillframes = 0;
  } else {
    stillframes++;
    if (stillframes > 10) {
      // camera has not moved: decrease step
      slicestep /= 2;
      slicestep = slicestep < 1 ? 1 : slicestep;
      stillframes = 0;
    }
  }
  sstep = slicestep;
  strans = SLICE_TRANS + (float)(sstep-1) / (float)(1 << (MAXSTEPS-1)) * (1. - SLICE_TRANS);

  previous_camera = camera;
#endif

  // 1. load textures (if nec.) (0 means auto-select axis)
  _s2priv_load_vr_textures(vrid, force, 0);

  // 2. draw textures
  _S2VRVOLUME *it = _s2_volumes + vrid;

  int pl2, plt;
  //float pl;
  XYZ p[4];
#if defined(S2TRIPLEFLOAT)
	COLOUR col = {1., 1., 1., 1.};
#else
  COLOUR col = {1., 1., 1.};
#endif
	
  int idx;
  float ic_a, ic_b, ic_c;

  float dia, dib, dic;
  dia = dib = dic = 0.5;

  switch (it->axis) {

  case 1:
    _s2debug("ds2dvr", "drawing textures for X-view");

#if defined(PROGRESSIVERENDER)
    // solve strans for 't' mode:
    float q_N = it->a2 - it->a1 + 1;
    float q_a = ((q_N / (float)sstep) - 1.0) * 0.5;
    float q_b = -1.0;
    float q_c = SLICE_TRANS * (1. - (q_N - 1.0) * 0.5 * SLICE_TRANS);
    float q_sqred = q_b * q_b - 4. * q_a * q_c;
    if (q_sqred > 0.0) {
      strans = (-1.0 * q_b + sqrt(q_sqred)) / (2. * q_a);
      //fprintf(stderr, "strans modified to %f\n", strans);
    }
#endif



    for (pl2 = 0; pl2 < (it->a2 - it->a1 + 1); pl2 += sstep) {
      if (it->reverse) {
	plt = it->a2 - pl2;
	//pl = it->a2 - 0.5 - 
	// (float)pl2 * (float)(it->a2 - it->a1 - 1) / (float)(it->a2 - it->a1);
      } else {
	plt = it->a1 + pl2;
	//pl = it->a1 + 0.5 + 
	// (float)pl2 * (float)(it->a2 - it->a1 - 1) / (float)(it->a2 - it->a1);
      }

      idx = 0;
      ic_a = plt;
      ic_b = it->b2 + dib;
      ic_c = it->c1 - dic;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;
      
      idx = 1;
      ic_a = plt;
      ic_b = it->b2 + dib;
      ic_c = it->c2 + dic;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;
      
      idx = 2;
      ic_a = plt;
      ic_b = it->b1 - dib;
      ic_c = it->c2 + dic;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;
      
      idx = 3;
      ic_a = plt;
      ic_b = it->b1 - dib;
      ic_c = it->c1 - dic;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;

      ns2vf4xt(p, col, it->textureids[plt - it->a1], 1., it->trans, strans);
    }
    break;

  case 2:
    _s2debug("ds2dvr", "drawing textures for Y-view");

#if defined(PROGRESSIVERENDER)
    // solve strans for 't' mode:
    float q_N = it->b2 - it->b1 + 1;
    float q_a = ((q_N / (float)sstep) - 1.0) * 0.5;
    float q_b = -1.0;
    float q_c = SLICE_TRANS * (1. - (q_N - 1.0) * 0.5 * SLICE_TRANS);
    float q_sqred = q_b * q_b - 4. * q_a * q_c;
    if (q_sqred > 0.0) {
      strans = (-1.0 * q_b + sqrt(q_sqred)) / (2. * q_a);
      //fprintf(stderr, "strans modified to %f\n", strans);
    }
#endif

    for (pl2 = 0; pl2 < (it->b2 - it->b1 + 1); pl2 += sstep) {
      if (it->reverse) {
	plt = it->b2 - pl2;
	//pl = it->b2 - 0.5 - 
	// (float)pl2 * (float)(it->b2 - it->b1 - 1) / (float)(it->b2 - it->b1);
      } else {
	plt = it->b1 + pl2;
	//pl = it->b1 + 0.5 +
	// (float)pl2 * (float)(it->b2 - it->b1 - 1) / (float)(it->b2 - it->b1);
      }
      
      idx = 0;
      ic_a = it->a1 - dia;
      ic_b = plt;
      ic_c = it->c2 + dic;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;
      
      idx = 1;
      ic_a = it->a2 + dia;
      ic_b = plt;
      ic_c = it->c2 + dic;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;
      
      idx = 2;
      ic_a = it->a2 + dia;
      ic_b = plt;
      ic_c = it->c1 - dic;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
      it->tr[11] * ic_c;
      
      idx = 3;
      ic_a = it->a1 - dia;
      ic_b = plt;
      ic_c = it->c1 - dic;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;
      
      ns2vf4xt(p, col, it->textureids[plt - it->b1], 1., it->trans, strans);
    }
    break;

  case 3:
    _s2debug("ds2dvr", "drawing textures for Z-view");

#if defined(PROGRESSIVERENDER)
    // solve strans for 't' mode:
    float q_N = it->c2 - it->c1 + 1;
    float q_a = ((q_N / (float)sstep) - 1.0) * 0.5;
    float q_b = -1.0;
    float q_c = SLICE_TRANS * (1. - (q_N - 1.0) * 0.5 * SLICE_TRANS);
    float q_sqred = q_b * q_b - 4. * q_a * q_c;
    if (q_sqred > 0.0) {
      strans = (-1.0 * q_b + sqrt(q_sqred)) / (2. * q_a);
      //fprintf(stderr, "strans modified to %f\n", strans);
    }
#endif

    for (pl2 = 0; pl2 < (it->c2 - it->c1 + 1); pl2 += sstep) {
      if (it->reverse) {
	plt = it->c2 - pl2;
	//pl = it->c2 - 0.5 - 
	// (float)pl2 * (float)(it->c2 - it->c1 - 1) / (float)(it->c2 - it->c1);
      } else {
	plt = it->c1 + pl2;
	//pl = it->c1 + 0.5 +
	// (float)pl2 * (float)(it->c2 - it->c1 - 1) / (float)(it->c2 - it->c1);
      }

      idx = 0;
      ic_a = it->a1 - dia;
      ic_b = it->b2 + dib;
      ic_c = plt;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;

      idx = 1;
      ic_a = it->a2 + dia;
      ic_b = it->b2 + dib;
      ic_c = plt;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;

      idx = 2;
      ic_a = it->a2 + dia;
      ic_b = it->b1 - dib;
      ic_c = plt;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;

      idx = 3;
      ic_a = it->a1 - dia;
      ic_b = it->b1 - dib;
      ic_c = plt;
      p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	it->tr[3] * ic_c;
      p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	it->tr[7] * ic_c;
      p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	it->tr[11] * ic_c;

      ns2vf4xt(p, col, it->textureids[plt - it->c1], 1., it->trans, strans);
    }
    break;


  } // case (it->axis)

}

// vrid is a volume rendering that we only want to draw on one axis.  So this function 
// will draw the volume rendering along that axis if specified axis matches what 
// should be drawn, or if force is true.
void ds2dvrx(int vrid, int force_reload, int axis, int force_draw) {
  if (!force_draw && !_s2_dynamicEnabled) {
    _s2warn("ds2dvrx", "called from non-dynamic mode");
    return;
  }

  if (vrid >= _s2_nvol) {
    _s2warn("ds2dvrx", "invalid volume rendering object (vrid)");
    return;
  }
 
  // 2. draw textures
  _S2VRVOLUME *it = _s2_volumes + vrid;

  int active_axis, active_reverse;
  _s2priv_choose_vr_axis(vrid, &active_axis, &active_reverse);

  //int axloop;
  //axloop = axis; {
  //for (axloop = 1; axloop <= 3; axloop++) {

  int axloop = axis;
  if (axloop == active_axis || force_draw) {

    // have to make axloop + / - to reflect correct order of drawing
    _s2priv_load_vr_textures(vrid, force_reload, axloop * (1 - active_reverse * 2));


    int pl2, plt;
    //float pl;
    XYZ p[4];
    COLOUR col = {1., 1., 1.};
    
    int idx;
    float ic_a, ic_b, ic_c;
    
    float dia, dib, dic;
    dia = dib = dic = 0.5;
    
    switch (it->axis) {
      
    case 1:
      _s2debug("ds2dvrx", "drawing textures for X-view, reverse=%d", it->reverse);
      
      for (pl2 = 0; pl2 < (it->a2 - it->a1 + 1); pl2++) {
	if (it->reverse) {
	  plt = it->a2 - pl2;
	  //pl = it->a2 - 0.5 - 
	  // (float)pl2 * (float)(it->a2 - it->a1 - 1) / (float)(it->a2 - it->a1);
	} else {
	  plt = it->a1 + pl2;
	  //pl = it->a1 + 0.5 + 
	  // (float)pl2 * (float)(it->a2 - it->a1 - 1) / (float)(it->a2 - it->a1);
	}
	
	idx = 0;
	ic_a = plt;
	ic_b = it->b2 + dib;
	ic_c = it->c1 - dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 1;
	ic_a = plt;
	ic_b = it->b2 + dib;
	ic_c = it->c2 + dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 2;
	ic_a = plt;
	ic_b = it->b1 - dib;
	ic_c = it->c2 + dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 3;
	ic_a = plt;
	ic_b = it->b1 - dib;
	ic_c = it->c1 - dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	ns2vf4xt(p, col, it->textureids[plt - it->a1], 1., it->trans, 0.6);
      }
      break;
      
    case 2:
      _s2debug("ds2dvrx", "drawing textures for Y-view, reverse=%d", it->reverse);
      
      for (pl2 = 0; pl2 < (it->b2 - it->b1 + 1); pl2++) {
	if (it->reverse) {
	  plt = it->b2 - pl2;
	  //pl = it->b2 - 0.5 - 
	  // (float)pl2 * (float)(it->b2 - it->b1 - 1) / (float)(it->b2 - it->b1);
	} else {
	  plt = it->b1 + pl2;
	  //pl = it->b1 + 0.5 +
	  // (float)pl2 * (float)(it->b2 - it->b1 - 1) / (float)(it->b2 - it->b1);
	}
	
	idx = 0;
	ic_a = it->a1 - dia;
	ic_b = plt;
	ic_c = it->c2 + dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 1;
	ic_a = it->a2 + dia;
	ic_b = plt;
	ic_c = it->c2 + dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 2;
	ic_a = it->a2 + dia;
	ic_b = plt;
	ic_c = it->c1 - dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 3;
	ic_a = it->a1 - dia;
	ic_b = plt;
	ic_c = it->c1 - dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	ns2vf4xt(p, col, it->textureids[plt - it->b1], 1., it->trans, 0.6);
      }
      break;
      
    case 3:
      _s2debug("ds2dvrx", "drawing textures for Z-view, reverse=%d", it->reverse);

      
      for (pl2 = 0; pl2 < (it->c2 - it->c1 + 1); pl2++) {
	if (it->reverse) {
	  plt = it->c2 - pl2;
	  //pl = it->c2 - 0.5 - 
	  // (float)pl2 * (float)(it->c2 - it->c1 - 1) / (float)(it->c2 - it->c1);
	} else {
	  plt = it->c1 + pl2;
	  //pl = it->c1 + 0.5 +
	  // (float)pl2 * (float)(it->c2 - it->c1 - 1) / (float)(it->c2 - it->c1);
	}
	
	idx = 0;
	ic_a = it->a1 - dia;
	ic_b = it->b2 + dib;
	ic_c = plt;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 1;
	ic_a = it->a2 + dia;
	ic_b = it->b2 + dib;
	ic_c = plt;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 2;
	ic_a = it->a2 + dia;
	ic_b = it->b1 - dib;
	ic_c = plt;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 3;
	ic_a = it->a1 - dia;
	ic_b = it->b1 - dib;
	ic_c = plt;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	ns2vf4xt(p, col, it->textureids[plt - it->c1], 1., it->trans, 0.6);
      }
      break;
      
      
    } // case (it->axis)
    
  } // axloop

}


/* add a handle */
void ds2ah(XYZ iP, float size, COLOUR icol, COLOUR ihilite, 
	   unsigned int iid, int iselected) {
  ds2ahx(iP, size, -1, -1, icol, ihilite, iid, iselected);
}

void ds2ahx(XYZ iP, float size, int itex, int ihitex,
	    COLOUR icol, COLOUR ihilite, 
	    unsigned int iid, int iselected) {
  if (!_s2_dynamicEnabled) {
    _s2warn("ds2ah*", "called from non-dynamic mode");
    return;
  }

  _S2HANDLE *handle_base = _s2priv_addhandles(1);
  if (!handle_base) {
    _s2error("ds2ah*", "failed to allocate memory for handle");
  }
  handle_base[0].p.x = _S2WORLD2DEVICE(iP.x, _S2XAX);
  handle_base[0].p.y = _S2WORLD2DEVICE(iP.y, _S2YAX);
  handle_base[0].p.z = _S2WORLD2DEVICE(iP.z, _S2ZAX);
  handle_base[0].col = icol;
  handle_base[0].hilite = ihilite;
  handle_base[0].id = iid;
  handle_base[0].selected = iselected;
  handle_base[0].size = 
    sqrt(0.33333 * (powf(_S2WORLD2DEVICE_SO(size, _S2XAX),2.) +
		    powf(_S2WORLD2DEVICE_SO(size, _S2YAX),2.) +
		    powf(_S2WORLD2DEVICE_SO(size, _S2ZAX),2.)));
  handle_base[0].texid = itex;
  handle_base[0].hitexid = ihitex;
  strcpy(handle_base[0].whichscreen, _s2_whichscreen);
  strncpy(handle_base[0].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
  handle_base[0].VRMLname[MAXVRMLLEN-1] = '\0';
  return;
}

/***********************************************************************
 *
 * ADVANCED (MISCELLANEOUS) ATTRIBUTES
 *
 ***********************************************************************
 */

/* set sphere resolution */
void ss2ssr(int res) {
  if (res >= 4 && res <= 100) {
    options.sphereresolution = res;
  } else {
    _s2warn("ss2ssr", "invalid sphere resolution");
  }
}

/* set render mode */
void ss2srm(int mode) {
  switch(mode) {
  case WIREFRAME:
    options.rendermode = WIREFRAME;
    break;
  case SHADE_FLAT:
    options.rendermode = SHADE_FLAT;
    break;
  case SHADE_DIFFUSE:
    options.rendermode = SHADE_DIFFUSE;
    break;
  case SHADE_SPECULAR:
    options.rendermode = SHADE_SPECULAR;
    break;
  default:
    options.rendermode = WIREFRAME;
    _s2warn("ss2srm", "invalid mode");
  }
}

/* get render mode */
int ss2qrm() {
  return options.rendermode;
}

/* set the entire lighting environment */
void ss2sl(COLOUR ambient, int nlights, XYZ *lightpos,
	   COLOUR *lightcol, int worldcoords) {
  globalambient[0] = ambient.r;
  globalambient[1] = ambient.g;
  globalambient[2] = ambient.b;
  globalambient[3] = 1.0;
  if (nlights > 8) {
    nlights = 8;
  }
  int i;
  for (i = 0; i < nlights; i++) {
    if (worldcoords) {
      lights[i].p[0] = _S2WORLD2DEVICE(lightpos[i].x, _S2XAX);
      lights[i].p[1] = _S2WORLD2DEVICE(lightpos[i].y, _S2YAX);
      lights[i].p[2] = _S2WORLD2DEVICE(lightpos[i].z, _S2ZAX);
    } else {
      lights[i].p[0] = lightpos[i].x;
      lights[i].p[1] = lightpos[i].y;
      lights[i].p[2] = lightpos[i].z;
    }
    lights[i].p[3] = 1; /* positional light */

    lights[i].c[0] = lightcol[i].r;
    lights[i].c[1] = lightcol[i].g;
    lights[i].c[2] = lightcol[i].b;
    lights[i].c[3] = 1;
  }
  nlight = nlights;
}


/* set background colour */
void ss2sbc(float r, float g, float b) {
  options.background.r = r;
  options.background.g = g;
  options.background.b = b;
  /* not done here: need to do in refresh cycle
  glClearColor(options.background.r, options.background.g, 
	       options.background.b, 0.);
  */
}

/* set the fisheye rotation angle for projections of this kind */
void ss2sfra(float rot) {
  camera.fishrotate = rot * DTOR;
}

/* get the projection type */
int ss2qpt() {
  return options.projectiontype;
}

/***********************************************************************
 *
 * CALLBACK SYSTEM
 *
 ***********************************************************************
 */
/* set the dynamic geometry callback function */
void cs2scb(void *icbfn) {
  _s2_callback = (void (*)(double *, int *))icbfn;
}
void *cs2qcb(void) {
  return (void *)_s2_callback;
}

/* set the drag handle callback function */
void cs2sdhcb(void *icbfn) {
  _s2_draghandle_callback = (void (*)(int *, XYZ *))icbfn;
}
void *cs2qdhcb(void) {
  return (void *)_s2_draghandle_callback;
}

/* are handles visible? */
int cs2qhv() {
  return _s2_handle_vis;
}

/* enable/disable/toggle selection mode (ie. handle visibility) */
void cs2thv(int enabledisable) {
  if (!(_s2_devcap & _S2DEVCAP_SELECTION)) {
    _s2warn("cs2thv", "selection mode (handles) not available on this device");
    return;
  }
  switch (enabledisable) {
  case -1:
    _s2_handle_vis = (_s2_handle_vis + 1) % 2;
    break;
  case 0: 
    _s2_handle_vis = 0;
    break;
  case 1:
    _s2_handle_vis = 1;
    break;
  default:
    _s2warn("cs2thv", "invalid argument 'enabledisable'");
    return;
  }  
}



/***********************************************************************
 *
 * ADVANCED CAMERA CONTROL
 *
 ***********************************************************************
 */

/* set the camera position, up, viewdi AND focus in one shot */
void ss2scaf(XYZ position, XYZ up, XYZ vdir, XYZ focus, int worldcoords) {
  if (worldcoords) {
    _s2_camera_vp.x = _S2WORLD2DEVICE(position.x, _S2XAX);
    _s2_camera_vp.y = _S2WORLD2DEVICE(position.y, _S2YAX);
    _s2_camera_vp.z = _S2WORLD2DEVICE(position.z, _S2ZAX);
    _s2_camera_vu.x = _S2WORLD2DEVICE_SO(up.x, _S2XAX);
    _s2_camera_vu.y = _S2WORLD2DEVICE_SO(up.y, _S2YAX);
    _s2_camera_vu.z = _S2WORLD2DEVICE_SO(up.z, _S2ZAX);
    _s2_camera_vd.x = _S2WORLD2DEVICE_SO(vdir.x, _S2XAX);
    _s2_camera_vd.y = _S2WORLD2DEVICE_SO(vdir.y, _S2YAX);
    _s2_camera_vd.z = _S2WORLD2DEVICE_SO(vdir.z, _S2ZAX);
  } else {
    _s2_camera_vp = position;
    _s2_camera_vu = up;
    _s2_camera_vd = vdir;
  }
  if (worldcoords) {
    _s2_camfocus.x = _S2WORLD2DEVICE(focus.x, _S2XAX);
    _s2_camfocus.y = _S2WORLD2DEVICE(focus.y, _S2YAX);
    _s2_camfocus.z = _S2WORLD2DEVICE(focus.z, _S2ZAX);
  } else {
    _s2_camfocus = focus;
  }    

  _s2_camexfocus = 1;
  _s2_cameraset = 1;

}

/* set the camera position etc. */
void ss2sc(XYZ position, XYZ up, XYZ vdir, int worldcoords) {
  if (worldcoords) {
    _s2_camera_vp.x = _S2WORLD2DEVICE(position.x, _S2XAX);
    _s2_camera_vp.y = _S2WORLD2DEVICE(position.y, _S2YAX);
    _s2_camera_vp.z = _S2WORLD2DEVICE(position.z, _S2ZAX);
    _s2_camera_vu.x = _S2WORLD2DEVICE_SO(up.x, _S2XAX);
    _s2_camera_vu.y = _S2WORLD2DEVICE_SO(up.y, _S2YAX);
    _s2_camera_vu.z = _S2WORLD2DEVICE_SO(up.z, _S2ZAX);
    _s2_camera_vd.x = _S2WORLD2DEVICE_SO(vdir.x, _S2XAX);
    _s2_camera_vd.y = _S2WORLD2DEVICE_SO(vdir.y, _S2YAX);
    _s2_camera_vd.z = _S2WORLD2DEVICE_SO(vdir.z, _S2ZAX);
  } else {
    _s2_camera_vp = position;
    _s2_camera_vu = up;
    _s2_camera_vd = vdir;
  }

  /* ensure cam focus point is along viewdir vector from viewpos */
  /* done by projecting this focus point onto viewdir vector from pos */
  XYZ fpos = VectorSub(camera.vp, _s2_camfocus);
  // assumption: camera.vd is normalised
  float flen = DotProduct(fpos, camera.vd);
  XYZ scaledvd = camera.vd;
  SetVectorLength(&scaledvd, flen); 

  _s2_cameraset = 1;
}

/* get the camera position etc. */
int ss2qc(XYZ *position, XYZ *up, XYZ *vdir, int worldcoords) {
  if (worldcoords) {
    if (position) {
      position->x = _S2DEVICE2WORLD(camera.vp.x, _S2XAX);
      position->y = _S2DEVICE2WORLD(camera.vp.y, _S2YAX);
      position->z = _S2DEVICE2WORLD(camera.vp.z, _S2ZAX);
    }
    if (up) {
      up->x = _S2DEVICE2WORLD_SO(camera.vu.x, _S2XAX);
      up->y = _S2DEVICE2WORLD_SO(camera.vu.y, _S2YAX);
      up->z = _S2DEVICE2WORLD_SO(camera.vu.z, _S2ZAX);
    }
    if (vdir) {
      vdir->x = _S2DEVICE2WORLD_SO(camera.vd.x, _S2XAX);
      vdir->y = _S2DEVICE2WORLD_SO(camera.vd.y, _S2YAX);
      vdir->z = _S2DEVICE2WORLD_SO(camera.vd.z, _S2ZAX);
    }
  } else {
    if (position) {
      *position = camera.vp;
    }
    if (up) {
      *up = camera.vu;
    }
    if (vdir) {
      *vdir = camera.vd;
    }
  }
  return (_s2_cameraset);
}

/* start or stop the camera rotation */
void ss2sas(int startstop) {
  XYZ origin = {0.0,0.0,0.0};
  if (startstop) {
    /* start rotation */
    options.autospin.x = 1.;
    options.autospin.y = 0.;
    options.autospin.z = 0.;
  } else {
    /* stop */
    options.autospin = origin;
  }
}

/* set/unset the camera focus point */
void ss2scf(XYZ position, int worldcoords) {
  if (worldcoords) {
    _s2_camfocus.x = _S2WORLD2DEVICE(position.x, _S2XAX);
    _s2_camfocus.y = _S2WORLD2DEVICE(position.y, _S2YAX);
    _s2_camfocus.z = _S2WORLD2DEVICE(position.z, _S2ZAX);
  } else {
    _s2_camfocus = position;
  }    
  /* ensure cam focus point is along viewdir vector from viewpos */
  /* done by projecting this focus point onto viewdir vector from pos */
  //fprintf(stderr, "camera.vd = %f,%f,%f\n", camera.vd.x, camera.vd.y, camera.vd.z);
  //fprintf(stderr, "camera.vp = %f,%f,%f\n", camera.vp.x, camera.vp.y, camera.vp.z);
  XYZ fpos = VectorSub(camera.vp, _s2_camfocus);
  // assumption: camera.vd is normalised
  float flen = DotProduct(fpos, camera.vd);
  XYZ scaledvd = camera.vd;
  SetVectorLength(&scaledvd, flen); 
  _s2_camfocus = VectorAdd(camera.vp, scaledvd);
  _s2_camexfocus = 1;
}

void ss2ucf(void) {
  _s2_camexfocus = 0;
}

void ss2sca(float aperture) {
  if ((aperture > 0.) && (aperture < 160.)) {
    camera.aperture = aperture;
  } else {
    _s2warn("ss2sca", "invalid camera aperture: outside (0., 160.)");
  }
}
float ss2qca(void) {
  return camera.aperture;
}

/* set spin speed */
void ss2sss(float spd) {
  options.interactspeed = spd;
}
/* get spin speed */
float ss2qss() {
  return options.interactspeed;
}

/* set camera speed */
void ss2scs(float spd) {
  _s2_cameraspeed = spd;
}
/* get camera speed */
float ss2qcs(void) {
  return _s2_cameraspeed;
}

/* enable/disable camera translation */
void ss2tc(int enabledisable) {
  _s2_transcam_enable = enabledisable;
}

/* set/query camera eye separation multiplier */
float ss2qess(void) {
  return _s2_eyesepmul;
}
void ss2sess(float ieyesep) {
  _s2_eyesepmul = ieyesep;
}


/***********************************************************************
 *
 * PANELS
 *
 ***********************************************************************
 */

// FIXME: panels need to be kept in sync with the global settings.  E.g. when
// a panel is moved, if it is also the active panel, the global location has
// to be updated as well.  Also, if the currently active panel is re-selected,
// no action should be taken etc.

int xs2ap(float x1, float y1, float x2, float y2) {
  //static int beenhere = 0;
  //if (!beenhere) {
  //  _s2warn("xs2ap", "panels have known state defects in S2PLOT code");
  //  beenhere = 1;
  //}
  static XYZ zero = {0., 0., 0.};
  static XYZ mone = {-1., -1., -1.};
  static XYZ pone = {1., 1., 1.};
  _s2_panels = (S2PLOT_PANEL *)realloc(_s2_panels, 
				       (_s2_npanels+1)*sizeof(S2PLOT_PANEL));
  S2PLOT_PANEL *it = &(_s2_panels[_s2_npanels]);

  it->x1 = _s2_scr_x1 + x1 * (_s2_scr_x2 - _s2_scr_x1);
  it->x2 = _s2_scr_x1 + x2 * (_s2_scr_x2 - _s2_scr_x1);
  it->y1 = _s2_scr_y1 + y1 * (_s2_scr_y2 - _s2_scr_y1);
  it->y2 = _s2_scr_y1 + y2 * (_s2_scr_y2 - _s2_scr_y1);
  
  it->active = 1;

  it->devicemin[_S2XAX] = -1.0;
  it->devicemax[_S2XAX] = +1.0;
  it->devicemin[_S2YAX] = -1.0;
  it->devicemax[_S2YAX] = +1.0;
  it->devicemin[_S2ZAX] = -1.0;
  it->devicemax[_S2ZAX] = +1.0;
  
  it->pmin = mone;
  it->pmax = pone;
  it->pmid = zero;
  it->rangemin = 1e32;
  it->rangemax = -1e32;


  it->camera = (CAMERA *)malloc(sizeof(CAMERA));
  it->autospin = (XYZ *)malloc(sizeof(XYZ));
  *(it->autospin) = zero;

  it->callback = NULL;
  it->callbackx = NULL;
  it->callbackx_data = NULL;

  it->userkeys = NULL;
  it->numcb = NULL;
  it->oglcb = NULL;
  it->remcb = NULL;
  it->handlecallback = NULL;
  it->draghandlecb = NULL;
  it->promptcb = NULL;
  it->promptcb_data = NULL;

  it->GL_listindex = -1;

  /* "current" geometry */
  it->nball = 0; it->ball = NULL;
  it->nballt = 0; it->ballt = NULL;
  it->ndisk = 0; it->disk = NULL;
  it->ncone = 0; it->cone = NULL;
  it->ndot = 0; it->dot = NULL;
  it->nline = 0; it->line = NULL;
  it->nface3 = 0; it->face3 = NULL;
  it->nface4 = 0; it->face4 = NULL;
  it->nface4t = 0; it->face4t = NULL;
  it->nlabel = 0; it->label = NULL;
  it->nhandle = 0; it->handle = NULL;
  it->nbboard = 0; it->bboard = NULL;
  it->nbbset = 0; it->bbset = NULL;
  it->nface3a = 0; it->face3a = NULL;
  it->ntrdot = 0; it->trdot = NULL;
#if defined(S2_3D_TEXTURES)
  it->ntexpoly3d = 0; it->texpoly3d = NULL;
#endif
  it->ntexmesh = 0; it->texmesh = NULL;

  /* "static" geometry */
  it->nball_s = 0; it->ball_s = NULL;
  it->nballt_s = 0; it->ballt_s = NULL;
  it->ndisk_s = 0; it->disk_s = NULL;
  it->ncone_s = 0; it->cone_s = NULL;
  it->ndot_s = 0; it->dot_s = NULL;
  it->nline_s = 0; it->line_s = NULL;
  it->nface3_s = 0; it->face3_s = NULL;
  it->nface4_s = 0; it->face4_s = NULL;
  it->nface4t_s = 0; it->face4t_s = NULL;
  it->nlabel_s = 0; it->label_s = NULL;
  it->nhandle_s = 0; it->handle_s = NULL;
  it->nbboard_s = 0; it->bboard_s = NULL;
  it->nbbset_s = 0; it->bbset_s = NULL;
  it->nface3a_s = 0; it->face3a_s = NULL;
  it->ntrdot_s = 0; it->trdot_s = NULL;
#if defined(S2_3D_TEXTURES)
  it->ntexpoly3d_s = 0; it->texpoly3d_s = NULL;
#endif
  it->ntexmesh_s = 0; it->texmesh_s = NULL;

  /* "dynamic" geometry */
  it->nball_d = 0; it->ball_d = NULL;
  it->nballt_d = 0; it->ballt_d = NULL;
  it->ndisk_d = 0; it->disk_d = NULL;
  it->ncone_d = 0; it->cone_d = NULL;
  it->ndot_d = 0; it->dot_d = NULL;
  it->nline_d = 0; it->line_d = NULL;
  it->nface3_d = 0; it->face3_d = NULL;
  it->nface4_d = 0; it->face4_d = NULL;
  it->nface4t_d = 0; it->face4t_d = NULL;
  it->nlabel_d = 0; it->label_d = NULL;
  it->nhandle_d = 0; it->handle_d = NULL;
  it->nbboard_d = 0; it->bboard_d = NULL;
  it->nbbset_d = 0; it->bbset_d = NULL;
  it->nface3a_d = 0; it->face3a_d = NULL;
  it->ntrdot_d = 0; it->trdot_d = NULL;
#if defined(S2_3D_TEXTURES)
  it->ntexpoly3d_d = 0; it->texpoly3d_d = NULL;
#endif
  it->ntexmesh_d = 0; it->texmesh_d = NULL;
  
  _s2_npanels++;
  return _s2_npanels-1;
}

void xs2cp(int panelid) {
  if (panelid < 0 || panelid >= _s2_npanels) {
    _s2warn("xs2cp", "invalid panel number");
    return;
  }
  if (panelid == _s2_activepanel) {
    static int beenhere = 0;
    if (!beenhere) {
      //_s2warn("xs2cp", "current panel reselected (suppressing further messages");
      beenhere = 1;
    }
    return;
  }
  _s2priv_pushGlobalsToPanel(_s2_activepanel);
  _s2_activepanel = panelid;
  _s2priv_pushPanelToGlobals(_s2_activepanel);
}

void xs2mp(int panelid, float x1, float y1, float x2, float y2) {
  if (panelid < 0 || panelid >= _s2_npanels) {
    _s2warn("xs2mp", "invalid panel number");
    return;
  }
  _s2_panels[panelid].x1 = _s2_scr_x1 + x1 * (_s2_scr_x2 - _s2_scr_x1);
  _s2_panels[panelid].x2 = _s2_scr_x1 + x2 * (_s2_scr_x2 - _s2_scr_x1);
  _s2_panels[panelid].y1 = _s2_scr_y1 + y1 * (_s2_scr_y2 - _s2_scr_y1);
  _s2_panels[panelid].y2 = _s2_scr_y1 + y2 * (_s2_scr_y2 - _s2_scr_y1);  
  if (panelid == _s2_activepanel) {
    // no action - there is no equivalent global setting
  }
}


void ss2tsc(char *whichscreen) {
  //strncpy(_s2_whichscreen, whichscreen, 9);
  strcpy(_s2_whichscreen, whichscreen);
  if (strlen(_s2_whichscreen)) {
    _s2_startScreenGeometry(FALSE);
  } else {
    _s2_endScreenGeometry();
  }
}


/***********************************************************************
 *
 * EXPERT FEATURES - USE WITH CAUTION !!!
 *
 ***********************************************************************
 */


void zs2retainlists(int retlists) {
  _s2_retain_lists = retlists;
}
