/* s2types.h
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
 * $Id: s2types.h 5815 2012-10-19 04:51:26Z dbarnes $
 *
 */

#ifndef S2TYPES_H
#define S2TYPES_H

#include "s2plot_glodef.h"
#include "s2base.h"
#include "s2const.h"
#include "paulslib.h"
#include "bitmaplib.h"

/* the devices we support */
typedef struct {
  char *devicename;
  int fullscreen;
  int stereo;
  int advertise;
  char *devicedesc;
} _S2DEVICE;

// One triangle 
typedef struct {
  double u[3],v[3]; // Texture coordinates
  XYZ p[3];         // Vertices 
  XYZ wp[3];        // Vertices for warpmap
  double wi[3];     // Intensity of node, for warpmap
} DOMEFACE;

typedef struct {
  double x,y;         // Coordinate
  double u,v;         // UV coordinate
  double i;           // Intensity
} MESHNODE;

typedef struct {
  int debug;
  int stereo;
  int fullscreen;
  int screenwidth;
  int screenheight;
  int dometype;
  char meshfile[128];
  int showdomeintensity;
  int rendermode;
  int showtextures;
  int showboundingbox;
  int recordimages;
  int windowdump;
  int makeqtvr;
  int showhelpinfo;
  int projectiontype;
  int selectmode;
  int topbottom;
  double targetfps;
  int doantialias;
  int cursortype;
  int interaction;
  int autopilot;
  XYZ autospin;
  XYZ scalefactor;
  int combineeyes;
  double interactspeed;
  double deltamove;
  COLOUR background;
  int sphereresolution;
  double pointscale,linescale;
  int faceorder;
  int twosided;
  int localviewer;
  int texturetype;
  CAMERA camerahome;
} OPTIONS;

typedef struct {
  int currentbutton;
  int mousexlast,mouseylast;
  int spaceballxlast, spaceballylast, spaceballzlast;
  double framerate;
} INTERFACESTATE;

typedef struct {
  XYZ p;
  COLOUR colour;
  float size;
#if defined(BUILDING_S2PLOT)
  char whichscreen[10];
  char VRMLname[MAXVRMLLEN];
#endif
} DOT;

typedef struct {
  XYZ p;
  COLOUR colour;
  double size;
  int trans;
  double alpha;
  char whichscreen[10];
} TRDOT; /* transparent dot */

typedef struct {
  XYZ p[2];
  COLOUR colour[2];
  double width;
#if defined(BUILDING_S2PLOT)
  char whichscreen[10];
  char VRMLname[MAXVRMLLEN];
  int stipple_factor; // 0 means no stipple
  unsigned short stipple_pattern;
  float alpha;
#endif
} LINE;

typedef struct {
  XYZ p[3];
  XYZ n[3];
  COLOUR colour[3];
#if defined(BUILDING_S2PLOT)
  char whichscreen[10];
  char VRMLname[MAXVRMLLEN];
#endif
} FACE3;

typedef struct {
  XYZ p[4];
  XYZ n[4];
  COLOUR colour[4];
#if defined(BUILDING_S2PLOT)
  char whichscreen[10];
  char VRMLname[MAXVRMLLEN];
#endif
} FACE4;

typedef struct {
  XYZ p[4];
  COLOUR colour;
  char texturename[64];
  int width,height;
  double scale;
  int trans;
  BITMAP4 *rgba;
  unsigned int textureid; // wasGL
#if defined(BUILDING_S2PLOT)
  double alpha; /* transparency: 1.0 = default = opaque */
  char whichscreen[10]; 
  char VRMLname[MAXVRMLLEN];
#endif
} FACE4T;

typedef struct {
  XYZ p;
  XYZ n;
  double r1,r2;
  COLOUR colour;
#if defined(BUILDING_S2PLOT)
  char whichscreen[10];
#endif
} DISK;

typedef struct {
  XYZ p1,p2;
  double r1,r2;
  COLOUR colour;
#if defined(BUILDING_S2PLOT)
  char whichscreen[10];
  char VRMLname[MAXVRMLLEN];
#endif
} CONE;

typedef struct {
  int nv;
  int p[4];
} OFFFACE;

typedef struct {
  XYZ p;
  XYZ right,up;
  COLOUR colour;
  char s[MAXLABELLEN];
#if defined(BUILDING_S2PLOT)
  char whichscreen[10];
  char VRMLname[MAXVRMLLEN];
#endif
} LABEL;

typedef struct {
  float p[4]; // wasGL
  float c[4]; // wasGL
} ALIGHT;


typedef struct {
  XYZ p;
  double r;
  COLOUR colour;
#if defined(BUILDING_S2PLOT)
  char whichscreen[10];
  char VRMLname[MAXVRMLLEN];
#endif
} BALL;

typedef struct {
  XYZ p;
  double r;
  COLOUR colour;
  char texturename[64];
  int width,height;
  BITMAP4 *rgba;
  unsigned int textureid; // wasGL
#if defined(BUILDING_S2PLOT)
  float texture_phase; // phase of texture [longitude in 0->1]
  XYZ axis; // axis of rotation
  float rotation; // angle of rotation in degrees
  char whichscreen[10];
#endif
} BALLT;



typedef struct {
  XYZ p;         /* position */
  COLOUR col;    /* colour to draw when not selected */
  COLOUR hilite; /* colour to draw when selected */
  int id;        /* unique identifier (= glLoadName value) */
  int selected;  /* 0 not selected, 1 selected */
  double dist;   /* distance to camera, used in sort */
  float size;    /* size of handle: approx. the diameter of billboard */
  int texid;     /* texture for unselected draw, -1 for default */
  int hitexid;   /* texture for selected draw, -1 for default */
  char whichscreen[10];
  char VRMLname[32];
} _S2HANDLE;

#if !defined(_S2BBOARD_STRUCT_DEFINED)
typedef struct {
  XYZ p;         /* position */
  XYZ str;       /* stretch (direction + magnitude): 0 = no stretch */
  XYZ offset;    /* offset (X and Y only, Z ignored) */
  float aspect;  /* aspect ratio: width to height "on the screen" */
  float size;    /* size of billboard */
  float pa;      /* position angle in radians, clockwise from 12 o'clock */
  COLOUR col;    /* colour */
  unsigned int texid; /* texture identifier */
  float alpha;   /* transparency */
  double dist;   /* distance to camera - used internally */
  char trans;    /* 'o', 's', 't' */
  char whichscreen[10];
  char VRMLname[32];
} _S2BBOARD;
#define _S2BBOARD_STRUCT_DEFINED 1
#endif

typedef struct {
  float *vertarray; // wasGL
  float *colarray;  // wasGL
  float size;
  unsigned int texid;
  char trans;
  int n;
  char whichscreen[10];
  char VRMLname[32];
} _S2BBSET;

/* transparent 3-vertex facet */
#if !defined(_S2FACE3A_STRUCT_DEFINED)
typedef struct {
  XYZ p[3];
  XYZ n[3];
  float dist;
  COLOUR colour[3];
  int trans; /* 'o' = opaque, 't'/'s' = transparent */
  double alpha[3]; /* transparency: 1.0 = default = opaque */
  char whichscreen[10]; 
  char VRMLname[32];
} _S2FACE3A;
#define _S2FACE3A_STRUCT_DEFINED 1
#endif

#if defined(S2_3D_TEXTURES)
/* 3d textured polygon, (3d texture, maybe 2d, 4d later) */
typedef struct {
  int nverts;
  XYZ *verts;
  XYZ *texcoords;
  unsigned int texid;
  int trans; /* 'o' = opaque, 't'/'s' = transparent */
  double alpha; /* 1.0 = opaque, 0.0 = totally transparent */
  char whichscreen[10];
  char VRMLname[32];
} _S2TEXPOLY3D;
#endif

// textured mesh
#if !defined(_S2TEXTUREDMESH_STRUCT_DEFINED)
typedef struct {
  int reference; // if 1, many of my members simply point to another texmesh
  int nverts; // vertices
  XYZ *verts;
  int nnorms; // normals
  XYZ *norms;
  int nvtcs;  // vertex texture coordinates
  XYZ *vtcs; // only XY used (as u,v)
  int nfacets; // (triangular) facets
  int *facets; // 3 * int per facet = 3 * vertex INDICES (look-up)
  int *facets_vtcs; // 3 * int per facet = 3 * texture coordinate INDICES
  unsigned int texid;
  int trans; /* 'o' = opaque, 't'/'s' = transparent */
  double alpha; /* 1.0 = opaque, 0.0 = totally transparent */
  char whichscreen[10];
  char VRMLname[32];
} _S2TEXTUREDMESH;
#define _S2TEXTUREDMESH_STRUCT_DEFINED
#endif

typedef struct {
  float ***grptr;
  int adim, bdim, cdim;
  int a1, a2, b1, b2, c1, c2;
  float tr[12];
  char trans; /* 'o', 't' or 's' */
  float alpha; /* 1. is opaque */
  float level;
  int resolution;
  float red, green, blue;
  void (*fcol)(float*,float*,float*,float*,float*,float*);
  float local_tr[12];
} _S2TRIANGLE_CACHE_DESCR;

typedef struct {
  /* parameters for this cached triangle list */
  float ***grptr;
  _S2TRIANGLE_CACHE_DESCR descr;
  _S2TRIANGLE_CACHE_DESCR cached_descr;
  /* the triangle list */
  int ntri;
  XYZ *trivert; /* ntri * 3 in length */
  XYZ *normals; /* ntri * 3 in length */
  COLOUR *col;  /* ntri in length */
} _S2TRIANGLE_CACHE;

/* volume rendering types, settings */
typedef struct {
  float ***grid;    /* pointer to original data - NOT a copy! */
  int adim, bdim, cdim; /* dimensions of the grid */
  int a1, a2, b1, b2, c1, c2;
  float tr[12];
  char trans; /* 'o', 't' or 's' */
  float datamin, datamax;
  float alphamin, alphamax; /* 1. is opaque */
  float (*alphafn)(float *);
  int ntexts;
  unsigned int *textureids;
  int axis;
  int reverse; /* if non-zero, reverse drawing order */
} _S2VRVOLUME;

typedef struct {
  double t;       // event time
  int type; // event type
  void *data;    // event data
  int consumed; // has event been used?
} _S2EVENT;

/* multi-panel capability */
typedef struct {
  
  /* layout */
  float x1, y1, x2, y2; /* where is this panel? x,y in [0,1] */
  
  /* draw this panel? */
  int active;
  
  /* coordinates */
  float devicemin[3]; /* normalised device coord system lower bounds */
  float devicemax[3]; /* normalised device coord system upper bounds */
  float axismin[3]; /* world coord system axis minima */
  float axismax[3]; /* world coord system axis maxima */
  float axissgn[3]; /* axis sign - ie. +1 if max > min, -1 otherwise */
  
  /* clipping? */
  int clipping;
  
  /* camera */
  CAMERA *camera;
  
  /* more coord info */
  XYZ pmin, pmax, pmid;
  double rangemin, rangemax;
  
  /* various parts of the global option space */
  XYZ *autospin;
  CAMERA camerahome;
  
  /* callbacks */
  void (*callback)(double *, int *);
  void (*callbackx)(double *, int *, void *);
  void *callbackx_data;
  
  int (*userkeys)(unsigned char *);
  void (*numcb)(int *);
  void (*oglcb)(int *);
  int (*remcb)(char *);
  
  void (*handlecallback)(int *);
  void (*draghandlecb)(int *, XYZ *);
  
  void (*promptcb)(char *, void *);
  void *promptcb_data;
  char baseprompt[S2BASEPROMPT_LEN];
  float prompt_x, prompt_y;
  
  /* dragging state */
  double dragmodel[16], dragproj[16]; // wasGL
  int dragview[4]; // wasGL
  
  int GL_listindex; /* for GL list of static geom */
  
  /* "current" geometry */
  int nball    ; BALL    *ball;
  int nballt   ; BALLT   *ballt;
  int ndisk    ; DISK    *disk;
  int ncone    ; CONE    *cone;
  int ndot     ; DOT     *dot;
  int nline    ; LINE    *line;
  int nface3   ; FACE3   *face3;
  int nface4   ; FACE4   *face4;
  int nface4t  ; FACE4T  *face4t;
  int nlabel   ; LABEL   *label;
  int nhandle  ; _S2HANDLE  *handle;
  int nbboard  ; _S2BBOARD  *bboard;
  int nbbset   ; _S2BBSET *bbset;
  int nface3a  ; _S2FACE3A  *face3a;
  int ntrdot   ; TRDOT   *trdot;
#if defined(S2_3D_TEXTURES)
  int ntexpoly3d ; _S2TEXPOLY3D *texpoly3d;
#endif
  int ntexmesh ; _S2TEXTUREDMESH *texmesh;
  
  /* "static" geometry */
  int nball_s    ; BALL    *ball_s;
  int nballt_s   ; BALLT   *ballt_s;
  int ndisk_s    ; DISK    *disk_s;
  int ncone_s    ; CONE    *cone_s;
  int ndot_s     ; DOT     *dot_s;
  int nline_s    ; LINE    *line_s;
  int nface3_s   ; FACE3   *face3_s;
  int nface4_s   ; FACE4   *face4_s;
  int nface4t_s  ; FACE4T  *face4t_s;
  int nlabel_s   ; LABEL   *label_s;
  int nhandle_s  ; _S2HANDLE  *handle_s;
  int nbboard_s  ; _S2BBOARD  *bboard_s;
  int nbbset_s   ; _S2BBSET *bbset_s;
  int nface3a_s  ; _S2FACE3A  *face3a_s;
  int ntrdot_s   ; TRDOT   *trdot_s;
#if defined(S2_3D_TEXTURES)
  int ntexpoly3d_s ; _S2TEXPOLY3D *texpoly3d_s;
#endif
  int ntexmesh_s ; _S2TEXTUREDMESH *texmesh_s;

  /* "dynamic" geometry */
  int nball_d    ; BALL    *ball_d;
  int nballt_d   ; BALLT   *ballt_d;
  int ndisk_d    ; DISK    *disk_d;
  int ncone_d    ; CONE    *cone_d;
  int ndot_d     ; DOT     *dot_d;
  int nline_d    ; LINE    *line_d;
  int nface3_d   ; FACE3   *face3_d;
  int nface4_d   ; FACE4   *face4_d;
  int nface4t_d  ; FACE4T  *face4t_d;
  int nlabel_d   ; LABEL   *label_d;
  int nhandle_d  ; _S2HANDLE  *handle_d;
  int nbboard_d  ; _S2BBOARD  *bboard_d;
  int nbbset_d   ; _S2BBSET *bbset_d;
  int nface3a_d  ; _S2FACE3A  *face3a_d;
  int ntrdot_d   ; TRDOT   *trdot_d;
#if defined(S2_3D_TEXTURES)
  int ntexpoly3d_d ; _S2TEXPOLY3D *texpoly3d_d;
#endif
  int ntexmesh_d ; _S2TEXTUREDMESH *texmesh_d;
  
} S2PLOT_PANEL;

#if !defined(_S2CACHEDTEXTURE_STRUCT_DEFINED)
typedef struct {
  int width, height; // "user" width and height
  int width2, height2; // "internal" width and height (powers of two)
  float wfrac, hfrac; // width and height fraction for texture coordinates
                      // when width2 > width &/ height2 > height
  BITMAP4 *bitmap;
  unsigned int id; // wasGL
  int depth, depth2; // "user" and "internal" depth for 3d textures
} _S2CACHEDTEXTURE;
#define _S2CACHEDTEXTURE_STRUCT_DEFINED 1
#endif

#endif // S2TYPES_H
