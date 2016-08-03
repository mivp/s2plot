/* s2globals.h
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
 * $Id: s2globals.h 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#ifndef S2GLOBALS_H
#define S2GLOBALS_H

#include "s2types.h"

// Options
extern OPTIONS options;

// Interface state
extern INTERFACESTATE interfacestate;

/* Geometry Structure */
extern int nball    ; extern BALL    *ball    ;
extern int nballt   ; extern BALLT   *ballt   ;
extern int ndisk    ; extern DISK    *disk    ;
extern int ncone    ; extern CONE    *cone    ;
extern int ndot     ; extern DOT     *dot     ;
extern int nline    ; extern LINE    *line    ;
extern int nface3   ; extern FACE3   *face3   ;
extern int nface4   ; extern FACE4   *face4   ;
extern int nface4t  ; extern FACE4T  *face4t  ;
extern int nlabel   ; extern LABEL   *label   ;
#if defined(BUILDING_S2PLOT)
extern int nhandle  ; extern _S2HANDLE  *handle  ;
extern int nbboard  ; extern _S2BBOARD  *bboard  ;
extern int nbbset   ; extern _S2BBSET   *bbset   ;
extern int nface3a  ; extern _S2FACE3A  *face3a  ;
extern int ntrdot   ; extern TRDOT      *trdot   ;
#if defined(S2_3D_TEXTURES)
extern int ntexpoly3d ; extern _S2TEXPOLY3D *texpoly3d ;
#endif
extern int ntexmesh ; extern _S2TEXTUREDMESH *texmesh;
#endif
extern XYZ pmin,pmax,pmid;
extern double rangemin,rangemax;

#if defined(BUILDING_S2PLOT)
/* s2 changes: add lists for static geometry */
extern int nball_s    ; extern BALL    *ball_s    ;
extern int nballt_s   ; extern BALLT   *ballt_s   ;
extern int ndisk_s    ; extern DISK    *disk_s    ;
extern int ncone_s    ; extern CONE    *cone_s    ;
extern int ndot_s     ; extern DOT     *dot_s     ;
extern int nline_s    ; extern LINE    *line_s    ;
extern int nface3_s   ; extern FACE3   *face3_s   ;
extern int nface4_s   ; extern FACE4   *face4_s   ;
extern int nface4t_s  ; extern FACE4T  *face4t_s  ;
extern int nlabel_s   ; extern LABEL   *label_s   ;
extern int nhandle_s  ; extern _S2HANDLE  *handle_s  ;
extern int nbboard_s  ; extern _S2BBOARD  *bboard_s  ;
extern int nbbset_s   ; extern _S2BBSET   *bbset_s   ;
extern int nface3a_s  ; extern _S2FACE3A  *face3a_s  ;
extern int ntrdot_s   ; extern TRDOT   *trdot_s   ;
#if defined(S2_3D_TEXTURES)
extern int ntexpoly3d_s ; extern _S2TEXPOLY3D *texpoly3d_s ;
#endif
extern int ntexmesh_s ; extern _S2TEXTUREDMESH *texmesh;

/* ... and for dynamic geometry */
extern int nball_d    ; extern BALL    *ball_d    ;
extern int nballt_d   ; extern BALLT   *ballt_d   ;
extern int ndisk_d    ; extern DISK    *disk_d    ;
extern int ncone_d    ; extern CONE    *cone_d    ;
extern int ndot_d     ; extern DOT     *dot_d     ;
extern int nline_d    ; extern LINE    *line_d    ;
extern int nface3_d   ; extern FACE3   *face3_d   ;
extern int nface4_d   ; extern FACE4   *face4_d   ;
extern int nface4t_d  ; extern FACE4T  *face4t_d  ;
extern int nlabel_d   ; extern LABEL   *label_d   ;
extern int nhandle_d  ; extern _S2HANDLE  *handle_d  ;
extern int nbboard_d  ; extern _S2BBOARD  *bboard_d  ;
extern int nbbset_d   ; extern _S2BBSET   *bbset_d   ;
extern int nface3a_d  ; extern _S2FACE3A  *face3a_d  ;
extern int ntrdot_d   ; extern TRDOT   *trdot_d   ;
#if defined(S2_3D_TEXTURES)
extern int ntexpoly3d_d ; extern _S2TEXPOLY3D *texpoly3d_d ;
#endif
extern int ntexmesh_d ; extern _S2TEXTUREDMESH *texmesh_d;

#endif

// Camera 
extern CAMERA camera;

// Lights as provided in the scene file 
extern int nlight;
extern ALIGHT lights[MAXLIGHT];

// Global lights, set through the light form 
extern float globalambient[4];
extern int defaultlights;
#if defined(BUILDING_S2PLOT)
extern XYZ deflightpos[8];
#else
extern XYZ deflightpos[8];
#endif

// Global Materials, set through the materials form 
extern float specularcolour[4]; // wasGL
extern float shininess[1]; // wasGL
extern float emission[4]; // wasGL
extern float transparency; // wasGL

extern float targetRoC_x, targetRoC_y;
extern float currRoC_x, currRoC_y;

extern float _s2devicemin[3]; /* normalised device coord system lower bounds */
extern float _s2devicemax[3]; /* normalised device coord system upper bounds */

extern float _s2axismin[3]; /* world coord system axis minima */
extern float _s2axismax[3]; /* world coord system axis maxima */
extern float _s2axissgn[3]; /* axis sign - ie. +1 if max > min, -1 otherwise */

extern int _s2_conelines; /* for private control */

extern char _s2_opt_store[3][20]; /* for storing axis labelling options */


/* current character size/height - affects tickmarks and symbols too */
extern float _s2_charsize;

extern COLOUR _s2_foreground;

/* near & far clipping plane expansion (1.0 = original version) */
extern float _s2_nearfar_expand;
/* near and far clipping planes from last iteration */
extern double _s2_save_near, _s2_save_far;

/* current line width */
extern float _s2_linewidth;

/* current line style */
extern int _s2_linestyle;

/* colormap size, ie. colors are 0 .. (_s2_cmapsize -1) */
extern int _s2_cmapsize;
/* the colormap itself */
extern COLOUR *_s2_colormap;
/* the current color index in use */
extern int _s2_colidx;
/* the current color index range for shading */
extern int _s2_colr1, _s2_colr2;

/* arrow properties */
extern int _s2_arrow_fs; /* 1 = "solid cone"; 2 is not yet supported */
extern float _s2_arrow_angle, _s2_arrow_barb;

/* title and iden strings */
extern char *_s2_title;
extern char *_s2_iden;

/* show annotations? - can be toggled with 'n' or 'N' in interact mode */
extern int _s2_showannotations;

/* custom help string for second press of F1 */
extern char *_s2_customhelpstr;

/* default colormap from PGPlot */
extern int _s2_pg_cmapsize;
extern COLOUR _s2_pg_colormap[];

/* the devices we support */
extern _S2DEVICE _s2_valid_devices[];
extern int _s2_ndevices;

/* pointer to callback function */
extern void (*_s2_callback)(double*, int *);

/* pointer to callback function that also handles a void data ptr */
extern void (*_s2_callbackx)(double*, int*, void *);
/* pointer to the data */
extern void *_s2_callbackx_data;

/* pointer to user-installed keyboard callback function */
extern int (*_s2_user_keys)(unsigned char *);


/* dynamic */
extern int _s2_dynamicEnabled;
extern int _s2_dynamic_erase;

/* status flag: animating or frozen? */
extern int _s2_animation;

/* count flag: how many times has the spacebar been pressed? */
extern int _s2_callbackkey;

/* store state for what we are recording (1 = geom + view, 2 = geom +
 * view + image, 3 = continuous geom + view + image), and what frame
 * number are we up to?
 */
extern int _s2_recstate;
extern int _s2_recframe;

extern int _s2_skip; /* if 1, set back to 0 and leave glut loop */

/* camera control */
extern int _s2_cameraset; /* should we set the camera position ? */
extern XYZ _s2_camera_vp; /* view position to set */
extern XYZ _s2_camera_vu; /* view up       to set */
extern XYZ _s2_camera_vd; /* view dirn     to set */
extern int _s2_transcam_enable; /* is translating the camera allowed (eg. +/- keys) */

/* camera focus control */
extern int _s2_camexfocus; /* specific focus (mid) point provided ? */
extern XYZ _s2_camfocus;   /* mid point to use when _s2_camexfocus = 1 */
extern float _s2_cameraspeed; /* speed / increment size for camera movements */

/* camera eye sep scale: default 1.0 */
extern float _s2_eyesepmul;

/* is buffer swapping allowed? - turn off for Cocoa use */
extern int _s2_bufswap;

/* should display lists be retained? - turn off for Mac Screensaver use */
extern int _s2_retain_lists;

/* pointer to callback function - provides number of key pressed */
extern void (*_s2_numcb)(int *);

/* pointer to opengl callback function */
extern void (*_s2_oglcb)(int *);

/* pointer to remote control callback function */
extern int (*_s2_remcb)(char *);

/* pointer to remote control callback function which also sends information back to client*/
extern int (*_s2_remcb_sock)(char *, FILE *);

/* pointer to remote control callback function which also sends information back to client via write method*/
extern int (*_s2_remcb_sock_write)(char *, int);

/* this can be set to 1 in very special circumstances to avoid
 * waiting for a mutex lock in threads */
extern int _s2_skiplock; 

/* what port do remote controllers connect on? */
extern int _s2_remoteport;

/* are the handles visible? */
extern int _s2_handle_vis;

/* which handle is being dragged?  NULL for none */
extern _S2HANDLE *_s2_draghandle;
extern int _s2_dragpanel; /* which panel is drag on? */
extern int _s2_draghandle_id;
extern double _s2_draghandle_basex, _s2_draghandle_basey; // maybe not required?
extern double _s2_draghandle_savez; /* z buffer value that is kept */
extern double _s2_dragmodel[16], _s2_dragproj[16]; // wasGL
extern int _s2_dragview[4]; //wasGL
extern int _s2_draghandle_screen; /* if !0 drag is in screen coordinates */

/* pointer to handle callback function */
extern void (*_s2_handlecallback)(int *);
  
/* handle drag callback function, gives handle id and new position
 * in world coords */
extern void (*_s2_draghandle_callback)(int *, XYZ *);

/* device capabilities */
extern int _s2_devcap;

/* is the crosshair visible? */
extern int _s2_crosshair_vis;

/* mouse coords */
extern int _s2_lastmousex, _s2_lastmousey;

/* are we clipping within the world bounds? */
extern int _s2_clipping;

/* latex template file name */
extern char _s2_latex_template_file[200];

extern float _s2priv_colrfn_r, _s2priv_colrfn_g, _s2priv_colrfn_b;
extern _S2ISOSURFACE *_s2_isosurfs;
extern int _s2_nisosurf;
extern int _s2_fastsurfaces;

/* volume rendering types, settings */
extern _S2VRVOLUME *_s2_volumes;
extern int _s2_nvol;
extern int _s2_vralphascaling;
extern float _s2_evas_x, _s2_evas_y, _s2_evas_z;


extern double _s2_fadetime;
extern int _s2_fadestatus; /* 0 = start fade-in, 1 = fade-in, 2 = normal running, */

/* runtime: if > 0 this means quit after so many seconds */
extern double _s2_runtime;

/* event structure */
extern _S2EVENT *_s2_events;
extern int _s2_nevents;

extern char _s2_whichscreen[10]; /* can be one or more of l, c, r for screen coordinate drawing; or empty for normal drawing */

extern int _s2_screenEnabled; /* 0 = drawing normal geometry; 1 = drawing screen geometry */

extern char _s2_doingScreen[2]; /* which screen are we currently drawing? */

extern char _s2prompt[S2PROMPT_LEN];
extern int _s2prompt_length;
extern char _s2baseprompt[S2BASEPROMPT_LEN];
extern float _s2prompt_x, _s2prompt_y;
/* prompt callback function */
extern void (*_s2_promptcbx)(char *, void *);
extern void *_s2_promptcbx_data;

/* multipanel capability */
extern S2PLOT_PANEL *_s2_panels;
extern int _s2_npanels;
extern int _s2_activepanel;

extern COLOUR _s2_activepanelframecolour;
extern COLOUR _s2_panelframecolour;
extern float _s2_panelframewidth;

// what are the screen constraints?
extern float _s2_scr_x1, _s2_scr_x2, _s2_scr_y1, _s2_scr_y2;

// background image texture id: if < 0, not available
extern int _s2_bg_texid;
extern int _s2_bg_clear;

extern int _s2_nVRMLnames;
extern char **_s2_VRMLnames;
extern int _s2_currVRMLidx;

/* are we using an ati card? (requires no multisampling) */
extern int _s2x_ati;

/* what is the top scan line number? */
extern int _s2x_ywinpos;

/* cache of textures (generally used in callbacks) */
extern int _s2_ctext_count;
extern _S2CACHEDTEXTURE *_s2_ctext;


#endif // S2GLOBALS_H
