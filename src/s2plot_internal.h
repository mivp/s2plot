/* s2plot_internal.h
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
 * $Id: s2plot_internal.h 5810 2012-10-12 03:56:20Z dbarnes $
 *
 */

#ifndef S2PLOT_INTERNAL_H
#define S2PLOT_INTERNAL_H

#if defined(__cplusplus) && !defined(S2_CPPBUILD)
extern "C" {
#endif
  
#if defined(BUILDING_S2PLOT)
#include "paulslib.h"
#include "s2opengl.h"
#include "s2globals.h"
  
  char _s2_opt_store[3][20]; /* for storing axis labelling options */
  
  
  
  /* current character size/height - affects tickmarks and symbols too */
  float _s2_charsize;
  
  COLOUR _s2_foreground;
  
  /* near & far clipping plane expansion (1.0 = original version) */
  float _s2_nearfar_expand;
  /* near and far clipping planes from last iteration */
  double _s2_save_near;
  double _s2_save_far;
  
  /* colormap size, ie. colors are 0 .. (_s2_cmapsize -1) */
  int _s2_cmapsize;
  /* the colormap itself */
  COLOUR *_s2_colormap;
  /* the current color index in use */
  int _s2_colidx;
  /* the current color index range for shading */
  int _s2_colr1, _s2_colr2;
  
  /* arrow properties */
  int _s2_arrow_fs = 1; /* 1 = "solid cone"; 2 is not yet supported */
  float _s2_arrow_angle = 45.0;
  float _s2_arrow_barb = 0.3;
  
  /* title and iden strings */
  char *_s2_title;
  char *_s2_iden;
  
  /* show annotations? - can be toggled with 'n' or 'N' in interact mode */
  int _s2_showannotations;
  
  /* custom help string for second press of F1 */
  char *_s2_customhelpstr;
  
  /* default colormap from PGPlot */
  int _s2_pg_cmapsize = 16;
#if defined(S2TRIPLEFLOAT)
  COLOUR _s2_pg_colormap[] = {{0.00, 0.00, 0.00, 1.0}, // black 
			      {1.00, 1.00, 1.00, 1.0}, // white
			      {1.00, 0.00, 0.00, 1.0}, // red
			      {0.00, 1.00, 0.00, 1.0}, // green
			      {0.00, 0.00, 1.00, 1.0}, // blue
			      {0.00, 1.00, 1.00, 1.0}, // cyan
			      {1.00, 0.00, 1.00, 1.0}, // magenta
			      {1.00, 1.00, 0.00, 1.0}, // yellow
			      {1.00, 0.50, 0.00, 1.0}, // red+yellow (orange)
			      {0.50, 1.00, 0.00, 1.0}, // green+yellow
			      {0.00, 1.00, 0.50, 1.0}, // green+cyan
			      {0.00, 0.50, 1.00, 1.0}, // blue+cyan
			      {0.50, 0.00, 1.00, 1.0}, // blue+magenta
			      {1.00, 0.00, 0.50, 1.0}, // red+magenta
			      {0.33, 0.33, 0.33, 1.0}, // dark gray
			      {0.66, 0.66, 0.66, 1.0}};// light gray
#else
  COLOUR _s2_pg_colormap[] = {{0.00, 0.00, 0.00}, // black 
			      {1.00, 1.00, 1.00}, // white
			      {1.00, 0.00, 0.00}, // red
			      {0.00, 1.00, 0.00}, // green
			      {0.00, 0.00, 1.00}, // blue
			      {0.00, 1.00, 1.00}, // cyan
			      {1.00, 0.00, 1.00}, // magenta
			      {1.00, 1.00, 0.00}, // yellow
			      {1.00, 0.50, 0.00}, // red+yellow (orange)
			      {0.50, 1.00, 0.00}, // green+yellow
			      {0.00, 1.00, 0.50}, // green+cyan
			      {0.00, 0.50, 1.00}, // blue+cyan
			      {0.50, 0.00, 1.00}, // blue+magenta
			      {1.00, 0.00, 0.50}, // red+magenta
			      {0.33, 0.33, 0.33}, // dark gray
			      {0.66, 0.66, 0.66}};// light gray
#endif

  /* the devices we support */
  _S2DEVICE _s2_valid_devices[] = 
    {{"/S2MONO",   0, 0, 1, "Monoscopic, windowed display"},
     {"/S2MONOF",  1, 0, 1, "Monoscopic, full-screen display"},
     {"/S2ACTIV",  0, 1, 1, "Active stereoscopic, windowed display"},
     {"/S2ACTIVF", 1, 1, 1, "Active stereoscopic, full-screen display"},
     {"/S2PASSV",  0, 2, 1, "Passive stereoscopic, windowed display"},
     {"/S2PASSVF", 1, 2, 1, "Passive stereoscopic, full-screen display"},
     {"/S2FISH",   0, 3, 1, "Fisheye projection, windowed display"},
     {"/S2FISHF",  1, 3, 1, "Fisheye projection, full-screen display"},
     {"/S2TRUNCB", 0, 6, 0, "Truncated-base fisheye, windowed display"},
     {"/S2TRUNCBF",1, 6, 0, "Truncated-base fisheye, full-screen display"},
     {"/S2TRUNCT", 0, 7, 0, "Truncated-top fisheye, windowed display"},
     {"/S2TRUNCTF",1, 7, 0, "Truncated-top fisheye, full-screen display"},
     {"/S2WARP",   0, 5, 0, "Warped projection (eg. MirrorDome), windowed"},
     {"/S2WARPF",  1, 5, 0, "Warped projection (eg. MirrorDome), full-screen"},
     {"/S2ANA",    0, 8, 1, "Anaglyph stereo (red(L), blue(R)), windowed"},
     {"/S2ANAF",   1, 8, 1, "Anaglyph stereo (red(L), blue(R)), full-screen"},
     {"/S2DSANA",   0, 9, 1, "Anaglyph stereo (red(L), blue(R)), pre-desaturated, windowed"},
     {"/S2DSANAF",  1, 9, 1, "Anaglyph stereo (red(L), blue(R)), pre-desaturated, full-screen"},
     {"/S2TRIO",   0, 30, 0, "Passive stereo w/ control display"},
     {"/S2INTER",  0, 31, 1, "Interleaved stereoscopic, windowed display"},
     {"/S2INTERF", 1, 31, 1, "Interleaved stereoscopic, full-screen display"},
     {"/S2WPASSV", 0, 32, 0, "Warped passive stereoscopic, windowed display"},
     {"/S2WPASSVF",1, 32, 0, "Warped passive stereoscopic, full-screen display"},
     {"/S2NULL",   0, -1, 1, "Null device (no display)"}};
  int _s2_ndevices = 24;
  
  /* pointer to callback function */
  void (*_s2_callback)(double*, int *);
  
  /* pointer to callback function that also handles a void data ptr */
  void (*_s2_callbackx)(double*, int*, void *);
  /* pointer to the data */
  void *_s2_callbackx_data;

  /* pointer to user-installed keyboard callback function */
  int (*_s2_user_keys)(unsigned char *);

  int _s2_dynamicEnabled;
  
  /* whether to erase dynamic geometry; controlled via ds2protect()
   * and ds2unprotect calls */
  int _s2_dynamic_erase;
  
  /* status flag: animating or frozen? */
  int _s2_animation;
  
  /* count flag: how many times has the spacebar been pressed? */
  int _s2_callbackkey;
  
  /* store state for what we are recording (1 = geom + view, 2 = geom +
   * view + image, 3 = continuous geom + view + image), and what frame
   * number are we up to?
   */
  int _s2_recstate;
  int _s2_recframe;
  
  int _s2_skip; /* if 1, set back to 0 and leave glut loop */
  
  /* camera control */
  int _s2_cameraset; /* should we set the camera position ? */
  XYZ _s2_camera_vp; /* view position to set */
  XYZ _s2_camera_vu; /* view up       to set */
  XYZ _s2_camera_vd; /* view dirn     to set */
  int _s2_transcam_enable; /* is translating the camera allowed (eg. +/- keys) */
  
  /* camera focus control */
  int _s2_camexfocus; /* specific focus (mid) point provided ? */
  XYZ _s2_camfocus;   /* mid point to use when _s2_camexfocus = 1 */
  float _s2_cameraspeed; /* speed / increment size for camera movements */
  
  /* camera eye sep scale: default 1.0 */
  float _s2_eyesepmul;
  
  /* is buffer swapping allowed? - turn off for Cocoa use */
  int _s2_bufswap;

  /* should display lists be retained? - turn off for Mac Screensaver use */
  int _s2_retain_lists;
  
  /* callback function for handling keypresses Shift-1 through Shift-0 */
  
  /* pointer to callback function - provides number of key pressed */
  void (*_s2_numcb)(int *);
  
  /* pointer to opengl callback function */
  void (*_s2_oglcb)(int *);
  
  /* pointer to remote control callback function */
  int (*_s2_remcb)(char *);

  /* pointer to remote control callback function which also sends information back to client*/
  int (*_s2_remcb_sock)(char *, FILE *);

  /* pointer to remote control callback function which also sends information back to client via write method*/
  int (*_s2_remcb_sock_write)(char *, int);

  /* this can be set to 1 in very special circumstances to avoid
   * waiting for a mutex lock in threads */
  int _s2_skiplock; 

  /* what port do remote controllers connect on? */
  int _s2_remoteport;

  /* are the handles visible? */
  int _s2_handle_vis;
  
  /* which handle is being dragged?  NULL for none */
  _S2HANDLE *_s2_draghandle;
  int _s2_dragpanel; /* which panel is drag on? */
  int _s2_draghandle_id;
  double _s2_draghandle_basex, _s2_draghandle_basey; // maybe not required?
  double _s2_draghandle_savez; /* z buffer value that is kept */
  double _s2_dragmodel[16], _s2_dragproj[16]; // wasGL
  int _s2_dragview[4]; // wasGL
  int _s2_draghandle_screen; /* if !0 drag is in screen coordinates */

  /* pointer to handle callback function */
  void (*_s2_handlecallback)(int *);
  
  /* handle drag callback function, gives handle id and new position
   * in world coords */
  void (*_s2_draghandle_callback)(int *, XYZ *);

  /* device capabilities */
  int _s2_devcap;
  
  /* is the crosshair visible? */
  int _s2_crosshair_vis;
  
  /* mouse coords */
  int _s2_lastmousex, _s2_lastmousey;
  
  /* are we clipping within the world bounds? */
  int _s2_clipping;
  
  /* latex template file name */
  char _s2_latex_template_file[200];
  
  float _s2priv_colrfn_r, _s2priv_colrfn_g, _s2priv_colrfn_b;
  _S2ISOSURFACE *_s2_isosurfs;
  int _s2_nisosurf;
  int _s2_fastsurfaces;
  
  /* volume rendering types, settings */
  _S2VRVOLUME *_s2_volumes;
  int _s2_nvol;
  int _s2_vralphascaling;
  
  float _s2_evas_x, _s2_evas_y, _s2_evas_z;
  
  /* s2plot fade in/out routine */
  double _s2_fadetime;
  int _s2_fadestatus; /* 0 = start fade-in, 1 = fade-in, 2 = normal running, 
		       * 3 = start fade-out, 4 = fade-out, 5 = exiting */
  
  /* runtime: if > 0 this means quit after so many seconds */
  double _s2_runtime;
  
  /* event structure */
  _S2EVENT *_s2_events;
  int _s2_nevents;
  
  char _s2_whichscreen[10]; /* can be one or more of l, c, r for screen coordinate drawing; or empty for normal drawing */
  
  int _s2_screenEnabled; /* 0 = drawing normal geometry; 1 = drawing screen geometry */

  char _s2_doingScreen[2]; /* which screen are we currently drawing? */

  char _s2prompt[S2PROMPT_LEN];
  int _s2prompt_length;
  char _s2baseprompt[S2BASEPROMPT_LEN];
  float _s2prompt_x, _s2prompt_y;
  /* prompt callback function */
  void (*_s2_promptcbx)(char *, void *);
  void *_s2_promptcbx_data;
  
  
  /* multipanel capability */
  S2PLOT_PANEL *_s2_panels;
  int _s2_npanels;
  int _s2_activepanel;

  COLOUR _s2_activepanelframecolour;
  COLOUR _s2_panelframecolour;
  float _s2_panelframewidth;

  // what are the screen constraints?
  float _s2_scr_x1, _s2_scr_x2, _s2_scr_y1, _s2_scr_y2;

  // background image texture id: if < 0, not available
  int _s2_bg_texid;
  int _s2_bg_clear;

  int _s2_nVRMLnames;
  char **_s2_VRMLnames;
  int _s2_currVRMLidx;

#endif

  /* are we using an ati card? (requires no multisampling) */
  int _s2x_ati;

  /* what is the top scan line number? */
  int _s2x_ywinpos;

  /* cache of textures (generally used in callbacks) */
  int _s2_ctext_count;
  _S2CACHEDTEXTURE *_s2_ctext;
  
#if defined(__cplusplus) && !defined(S2_CPPBUILD)
} // extern "C" {
#endif

#endif /* S2PLOT_INTERNAL_H */
