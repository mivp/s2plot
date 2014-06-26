/* s2warpstereo.c: S2PLOT warped passive (/s2wpassv) stereo driver
 *
 * Copyright 2006-2014 David G. Barnes, Paul Bourke, Christopher Fluke
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
 */

#include "s2opengl.h"
#include "s2types.h"
#include "s2win.h"

void DrawExtras(void);
void _s2debug(char *fn, char *messg, ...);
void _s2warn(char *fn, char *messg, ...);

unsigned int ss2ctt(int width, int height);
void ss2dt(unsigned int texid);
void drawView(char *projinfo, double camsca);

#if !defined(mWINWIDTH)
#define mWINWIDTH (moptions.screenwidth)
#endif
#if !defined(mWINHEIGHT)
#define mWINHEIGHT (moptions.screenheight)
#endif

/* All of these should probable become *local* variables and never
 * known about to S2PLOT.
 */
OPTIONS *_s2w_options;
int _s2w_textureready = 0;
int _s2w_warpingactive = 1;
char *_s2w_meshfn = NULL;

// texture, width and height of (right) image to be warped
int _s2w_warpstereo_txt = -1;
int _s2w_warpstereo_w, _s2w_warpstereo_h;

#define moptions (*_s2w_options)

// Optional mapper
MESHNODE **mesh = NULL;
int meshtype = 0;
int meshnx = 0;
int meshny = 0;
int ReadMesh(char *fname);

MESHNODE **getmesh_s2warpstereo(int *nx, int *ny) {
  *nx = meshnx;
  *ny = meshny;
  return mesh;
}

void prep_s2warpstereo(OPTIONS *ioptions) {
  _s2w_options = ioptions;
  _s2w_textureready = 0;
  _s2w_warpingactive = 1;
  _s2w_warpstereo_txt = -1;
  _s2w_warpstereo_w = _s2w_warpstereo_h = 0; 
  _s2w_meshfn = NULL;

  _s2w_meshfn = getenv("S2PLOT_MESHFILE");
  if (_s2w_meshfn) {
    if (!ReadMesh(_s2w_meshfn)) {
      _s2w_meshfn = NULL;
    }
  }

  if (!_s2w_meshfn) {
    _s2warn("(internal)", "/S2WPASSV*: no meshfile available");
  }
  _s2debug("(internal)", "/S2WPASSV* device support loaded");

}

void resize_s2warpstereo(void) {
  // invalidate the texture we have
  _s2w_textureready = 0;
}

int keybd_s2warpstereo(char c) {
  int consumed = 0;

  switch(c) {
  case '`': /* toggle whether warping is on or off */
    _s2w_warpingactive = (_s2w_warpingactive + 1) % 2;
    _s2w_textureready = 0;
    consumed = 1;
    break;
  }

  return consumed;
}

void draw_s2warpstereo(CAMERA cam) {

  /* draw the left view directly; draw the right view to a texture
   * then onto the screen using a warp */
  
  // 1. create the texture if required.
  if (!_s2w_textureready) {

    if (_s2w_warpstereo_txt >= 0) {
      ss2dt(_s2w_warpstereo_txt);
    }

    int tmp = mWINWIDTH - 1;
    _s2w_warpstereo_w = 1;
    while (tmp) {
      tmp >>= 1;
      _s2w_warpstereo_w <<= 1;
    }
    tmp = mWINHEIGHT - 1;
    _s2w_warpstereo_h = 1;
    while (tmp) {
      tmp >>= 1;
      _s2w_warpstereo_h <<= 1;
    }

    _s2w_warpstereo_txt = ss2ctt(_s2w_warpstereo_w, _s2w_warpstereo_h);
  }
  
#define H_DIVISOR 2

  // 2. draw the right and save a texture of it
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
  glViewport(0, 0, mWINWIDTH / H_DIVISOR, mWINHEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawView("r", 0.5);
  DrawExtras();
  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glBindTexture(GL_TEXTURE_2D, _s2w_warpstereo_txt);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
		      mWINWIDTH / H_DIVISOR,
		      mWINHEIGHT);
  glPopAttrib();
  
  // 3. draw the left image
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
  glViewport(0, 0, mWINWIDTH / H_DIVISOR, mWINHEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawView("l", -0.5);
  DrawExtras();

  // Remember the graphics state and return it at the end
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  
  // the key for when we are in wireframe mode! - 
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glColor3f(1.0,1.0,1.0);
  
  // interleave the rows in the back buffer!
  // Right
  glViewport(mWINWIDTH/H_DIVISOR, 0, mWINWIDTH/H_DIVISOR, mWINHEIGHT);
  glDrawBuffer(GL_BACK);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if (1)
  
  // draw the texture we just captured...
  glColor4f(1., 1., 1., 1.);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glEnable(GL_TEXTURE_2D);
  //fprintf(stderr, "using texture id %d to draw...\n", _s2w_warpstereo_txt);
  glBindTexture(GL_TEXTURE_2D, _s2w_warpstereo_txt);
  
  if (!_s2w_warpingactive || !_s2w_meshfn) {
    // just one big quad filling the screen
    glBegin(GL_QUADS);
    glTexCoord2f(0., 0.); 
    glVertex2f(0., 0.);
    glTexCoord2f(0., (float)mWINHEIGHT / (float)_s2w_warpstereo_h);
    glVertex2f(0., 1.);
    glTexCoord2f((float)mWINWIDTH / (float)_s2w_warpstereo_w / H_DIVISOR,
		 (float)mWINHEIGHT / (float)_s2w_warpstereo_h);
    glVertex2f(1., 1.);
    glTexCoord2f((float)mWINWIDTH / (float)_s2w_warpstereo_w / H_DIVISOR, 0.);
    glVertex2f(1., 0.);
    glEnd();
    
  } else {
    // (meshnx-1) * (meshny-1) quads across the screen
    
    glBegin(GL_QUADS);

    float max_tc_x = (float)mWINWIDTH / (float)H_DIVISOR / 
      (float)_s2w_warpstereo_w;
    float max_tc_y = (float)mWINHEIGHT / (float)_s2w_warpstereo_h;
    float on2asp = 0.5;

    int ix, iy;
    for (ix = 0; ix < meshnx-1; ix++) {
      for (iy = 0; iy < meshny-1; iy++) {
	// four corners
	glTexCoord2f(mesh[ix][iy].x * max_tc_x, 
		     mesh[ix][iy].y * max_tc_y); 
	glVertex2f(0.5 + on2asp * mesh[ix][iy].u, 
		   0.5 + 0.5 * mesh[ix][iy].v);
	
	glTexCoord2f(mesh[ix][iy+1].x * max_tc_x, 
		     mesh[ix][iy+1].y * max_tc_y);
	glVertex2f(0.5 + on2asp * mesh[ix][iy+1].u, 
		   0.5 + 0.5 * mesh[ix][iy+1].v);
	
	glTexCoord2f(mesh[ix+1][iy+1].x * max_tc_x,
		     mesh[ix+1][iy+1].y * max_tc_y);
	glVertex2f(0.5 + on2asp * mesh[ix+1][iy+1].u, 
		   0.5 + 0.5 * mesh[ix+1][iy+1].v);
	
	glTexCoord2f(mesh[ix+1][iy].x * max_tc_x, 
		     mesh[ix+1][iy].y * max_tc_y);
	glVertex2f(0.5 + on2asp * mesh[ix+1][iy].u, 
		   0.5 + 0.5 * mesh[ix+1][iy].v);  
      }
    }
    glEnd();
    
  }
  
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix(); 

#endif

  glPopAttrib();

  
  // mark the texture as ready for use
  _s2w_textureready = 1;
  
}
