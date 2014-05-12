/* s2privfn.h
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
 * $Id: s2privfn.h 5815 2012-10-19 04:51:26Z dbarnes $
 *
 */

#ifndef S2PRIVFN_H
#define S2PRIVFN_H

#if defined(__cplusplus) && !defined(S2_CPPBUILD)
extern "C" {
#endif
  
#include "s2types.h"
  
  void _s2debug(char *fn, char *messg, ...);
  void _s2warn(char *fn, char *messg, ...);
  void _s2error(char *fn, char *messg, ...);
  void _s2warnk(char key, char *messg, ...);

  int face3a_comp(const void *a, const void *b);

  void _s2priv_setBounds();
  void CreateLabelVector(char *s,XYZ p,XYZ right,XYZ up,XYZ *list,int *nlist);

  void AddLine2Database(XYZ p1,XYZ p2,COLOUR c1,COLOUR c2,double w);
  void AddFace2Database(XYZ *p,int n,COLOUR c,double scale,XYZ shift);
  void AddMarker2Database(int type,double size,XYZ p,COLOUR c);
  DISK *_s2priv_adddisks(int in);
  BALL *_s2priv_addballs(int in);
  LABEL *_s2priv_addlabels(int in);
  _S2HANDLE *_s2priv_addhandles(int in);
  _S2BBOARD *_s2priv_addbboards(int in);
  _S2BBSET *_s2priv_addbbset(int in);
  CONE *_s2priv_addcones(int in);
  DOT *_s2priv_adddots(int in);
  TRDOT *_s2priv_addtrdots(int in);
  LINE *_s2priv_addlines(int in);
  FACE3 *_s2priv_addface3s(int in);
  _S2FACE3A *_s2priv_addface3as(int in);
#if defined(S2_3D_TEXTURES)
  _S2TEXPOLY3D *_s2priv_addtexpoly3ds(int in);
#endif
  _S2TEXTUREDMESH *_s2priv_addtexturedmesh(int in);
  FACE4 *_s2priv_addface4s(int in);
  FACE4T *_s2priv_addface4ts(int in);

  void _s2priv_quad(float *xpts, float *ypts, float *zpts, float *normal);
  void _s2priv_text(float *labelp, float *labelr, float *labelu, char *text);
  void _s2_priv_textab(float ix, float iy, float iz, 
		       int axis1, int axis2, 
		       float flip1, float flip2, char *itext);
  void _s2_priv_qtxtab(float *a1, float *a2, float *b1, float *b2,
		       float ix, float iy, float iz, 
		       int axis1, int axis2,
		       float flip1, float flip2,
		       char *itext, float ipad);
  float _s2priv_round(float x, int *nsub);
  void _s2priv_coneline(float *xpts, float *ypts, float *zpts);
  int _s2priv_ns2cvr(float ***grid, 
		     int adim, int bdim, int cdim,
		     int a1, int a2, int b1, int b2, int c1, int c2,
		     float *tr, char trans, 
		     float datamin, float datamax,
		     float alphamin, float alphamax,
		     float (*ialphafn)(float*));
  


  void _s2priv_list_devices();

  void CameraHome(int mode);
  void RotateCamera(double,double,double,int); 
  void FlyCamera(double);
  void CalcBounds(void);
  void UpdateBounds(XYZ);
  

  
  void _s2priv_drawCrosshair();
  
  /* create a red-X bitmap for texture use */
  BITMAP4 *_s2priv_redXtexture(int w, int h);
  BITMAP4 *_s2priv_redXtexture3d(int w, int h, int d);
  
  /* setup a texture from a bitmap of given width and height */
  unsigned int _s2priv_setupTexture(int width, int height, BITMAP4 *bitmap,
				    int usemipmaps);
  unsigned int _s2priv_setupTexture3d(int width, int height, int depth,
				      BITMAP4 *bitmap,
				      int usemipmaps);
  
  void _s2priv_ss2pt(unsigned int itextureID, int usemipmaps);
  
  /* drop a texture from memory */
  void _s2priv_dropTexture(unsigned int texid);
  
  /* switch the geometry lists so new geometry is added to / drawn from 
   * the dynamic lists.  A global flag is set so we know in MakeGeometry
   * to just draw, not create a list for later use.  If erase is non-zero
   * then the dynamic lists will be cleared.  This could be modified in 
   * future to erase some of the lists but not all.
   */
  void _s2_startDynamicGeometry(int erase);
  
  /* switch the geometry lists so new geometry is added to / drawn from 
   * the standard (static) lists.
   */
  void _s2_endDynamicGeometry();

  /* draw the handles */
  void _s2priv_drawHandles(int doscreen);
  
  /* draw the billboards */
  void _s2priv_drawBillboards(void);
  
  /* draw billboard sets */
  void _s2priv_drawBBsets(void);

  /* add a billboard */
  void _s2priv_bb(XYZ iP, XYZ iStretch, XYZ ioffset, 
		  float aspect, float size, 
		  float pangle, 
		  COLOUR icol, unsigned int iid, float ialpha, 
		  char itrans);

  /* handle clicks on handles */
  void _s2priv_handleHits(int nhits, unsigned int *list); // wasGL
  
  /* handle clicks on handles IN DOME mode */
  void _s2priv_handleDomeHits(int nhits, unsigned int *list); // wasGL
  
  /* find the handle with this id. */
  _S2HANDLE *_s2priv_findHandle(unsigned int item); // wasGL

  /* routines and structure/s for isosurfaces */
  int Polygonise(GRIDCELL g,double iso,TRIANGLE *tri);
  void _s2priv_drawTriangleCache(_S2TRIANGLE_CACHE *cache);
  void _s2priv_addToTriangleCache(_S2TRIANGLE_CACHE *cache, XYZ *trivert, 
				  //				XYZ cellnormal,
				  float *tr, COLOUR col);
  void _s2priv_colrfn(float *ix, float *iy, float *iz, 
		      float *r, float *g, float *b);
  void _s2priv_generate_isosurface(int isid, int force);
  void _s2priv_calcNormalsForTriangleCache(_S2TRIANGLE_CACHE *cache);

  int _s2priv_ns2cvr(float ***grid, 
		     int adim, int bdim, int cdim,
		     int a1, int a2, int b1, int b2, int c1, int c2,
		     float *tr, char trans, 
		     float datamin, float datamax,
		     float alphamin, float alphamax,
		     float (*ialphafn)(float*));
  void _s2priv_load_vr_textures(int vrid, int force, int iaxis);

  // internal fn for parametric functions
  void _s2priv_s2funuv(float(*fx)(float*,float*), float(*fy)(float*,float*), 
		       float(*fz)(float*,float*), float(*fcol)(float*,float*), 
		       char trans, float(*ialphafn)(float*,float*),
		       float umin, float umax, int uDIV,
		       float vmin, float vmax, int vDIV);
  
  // toggle fs / constrained window
  void _s2priv_togglefs();
  
  void _s2_fadeinout(void);

  void _s2priv_CameraSet(void);


  void _s2priv_readEvents(char *filename);

  // colormap functions
  void rainbow(double h, double s, double v, double *r, double *g, double *b);
  void dhsv2rgb(double h, double s, double v, double *r, double *g, double *b);
  int32_t *texture_gen(char *mapstr,int saturation_lvls,int texture_size,int black_border);

  int XYZCompare(const void *v1, const void *v2);

  /* switch the geometry lists so new geometry is added to / drawn from 
   * the screen lists.  A global flag is set so we know in MakeGeometry
   * to just draw, not create a list for later use.  If erase is non-zero
   * then the screen lists will be cleared.
   */
  void _s2_startScreenGeometry(int erase);
  
  /* switch the geometry lists so new geometry is added to / drawn from 
   * the previously set list (either static or dynamic).
   */
  void _s2_endScreenGeometry();
  
  /* clear/erase the screen geometry */
  void _s2_clearGeometryList();
  
  /* "command prompt" */
  void _s2priv_initPrompt();
  void _s2priv_finiPrompt();

  void _s2_prompt_kbdfunc(unsigned char, int, int);
  
  void _s2priv_pushGlobalsToPanel(int panelid);
  void _s2priv_pushPanelToGlobals(int panelid);
  void _s2priv_drawActiveFrame(int active);

  void _s2_drawBGimage(void);
  void _s2_blankPanel(void);

  void writeVRML20(void);
  void doVRMLdots(FILE *fp, int start, int end);
  void doVRMLlines(FILE *fp, int start, int end);
  void doVRMLface4(FILE *fp, int start, int end);
  void doVRMLface4t(FILE *fp, int start, int end);
  void doVRMLface3(FILE *fp, int start, int end);
  void doVRMLface3a(FILE *fp, int start, int end);
  void doVRMLball(FILE *fp, int start, int end);
  void doVRMLcone(FILE *fp, int start, int end);
  void doVRMLcylinder(FILE *fp, int start, int end);
  void doVRMLlabel(FILE *fp, int start, int end);
  void doVRMLbboard(FILE *fp, int start, int end);
  void doVRMLhandle(FILE *fp, int start, int end);

  void writePRC(void);



#if defined(__cplusplus) && !defined(S2_CPPBUILD)
}
#endif

#endif // S2PRIVFN_H
