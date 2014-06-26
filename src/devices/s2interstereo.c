/* s2interstereo.c: S2PLOT scan-line interleaved stereo code
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


unsigned int ss2ct(int width, int height);
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
OPTIONS *_s2i_options;
int _s2i_stencilready;
int _s2i_interjiggle;
int _s2i_ywinpos;

#define QUICKINTERSTEREO 1
#if !defined(QUICKINTERSTEREO)
int _s2i_interstereo_txt[2] = {-1, -1};
int _s2i_interstereo_w, _s2i_interstereo_h;
#endif

#define moptions (*_s2i_options)

void prep_s2interstereo(OPTIONS *ioptions) {
  _s2i_options = ioptions;
  _s2i_stencilready = 0;
  _s2i_interjiggle = 0;
  _s2i_ywinpos = 0;
#if !defined(QUICKINTERSTEREO)
  _s2i_interstereo_txt[0] = _s2i_interstereo_txt[1] = -1;
  _s2i_interstereo_w = _s2i_interstereo_h = 0; 
#endif
  char *s2interss = getenv("S2PLOT_INTERSCANBUMP");
  if (s2interss) {
    _s2i_interjiggle = (_s2i_interjiggle + 1) % 2;
  }
  _s2debug("(internal)", "/S2INTER* device support loaded");
}

void resize_s2interstereo(void) {
  // Following code is preferred: clear screen as soon as window
  // is resized or moved, BUT it tends to cause UI hangs on Mac !!!
#if (0)
  glDrawBuffer(GL_FRONT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glFlush();
#endif 
  _s2i_ywinpos = s2winGet(S2_WINDOW_Y);
  _s2i_stencilready = 0;
}

int keybd_s2interstereo(char c) {
  int consumed = 0;
  switch(c) {
  case '`': /* jiggle left/right frame interleave */
    _s2i_interjiggle = (_s2i_interjiggle + 1) % 2;
    _s2i_stencilready = 0;
    consumed = 1;
    break;
  }
  return consumed;
}

void draw_s2interstereo(CAMERA cam) {

  /* draw the right and left views to interleaved scan lines */
  
#if !defined(QUICKINTERSTEREO)
#if !defined(S2CYGWIN) && !defined(S2SUNOS)
  // draw stencil without multisampling
  glDisable(GL_MULTISAMPLE);
#endif
#endif

  if (!_s2i_stencilready) {
    glPushAttrib(GL_ENABLE_BIT);
    glViewport(0, 0, mWINWIDTH, mWINHEIGHT);
    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,mWINWIDTH,0,mWINHEIGHT,
	    -1,1); 
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColorMask(0, 0, 0, 0);
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glDisable(GL_DEPTH_TEST);
    int jsl;
    glColor3i(1,1,1);
    glLineWidth(1.);
    for (jsl = (_s2i_interjiggle + _s2i_ywinpos + mWINHEIGHT) % 2; jsl < mWINHEIGHT; jsl+=2) {
      glBegin(GL_LINES);
      glVertex2i(0, jsl);
      glVertex2i(mWINWIDTH-1, jsl);
      glEnd();
    }

    //#define SIMULATE_PANELS 1
#if defined(SIMULATE_PANELS)
    fprintf(stderr, "SIMULATE_PANELS is ON!\n");
    for (jsl = -100; jsl < 100; jsl++) {
      glBegin(GL_LINES);
      glVertex2i(mWINWIDTH/2+jsl,0);
      glVertex2i(mWINWIDTH/2+jsl,mWINHEIGHT-1);
      glEnd();
    }

#endif

    glColorMask(1,1,1,1);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
  }

#if !defined(QUICKINTERSTEREO)
#if !defined(S2CYGWIN) && !defined(S2SUNOS)
  glEnable(GL_MULTISAMPLE);
#endif
#endif
  
#if defined(QUICKINTERSTEREO)
  // fast version: striped stencil.  Thin horizontal lines don't work!
  glDisable(GL_MULTISAMPLE);

  // Right
  glViewport(0, 0, mWINWIDTH, mWINHEIGHT);
  glDrawBuffer(GL_BACK_RIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_EQUAL, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  drawView("r", 1.);
  DrawExtras();
  
  // Left
  glDrawBuffer(GL_BACK_LEFT);
  glStencilFunc(GL_NOTEQUAL, 1, 1);
  drawView("l", -1.);
  DrawExtras();
  glDisable(GL_STENCIL_TEST);
  
#else

  // slow version: draw to textures then interleave them

  // only make (sufficiently large) textures ONCE!
  static int doonce = 0;
  if (!_s2i_stencilready && !doonce) {
    doonce = 1;
    /*
    int i;    
    for (i = 0; i < 2; i++) {
      if (_s2i_interstereo_txt[i] >= 0) {
	fprintf(stderr, "deleting texture %d: %d\n", i, _s2i_interstereo_txt[i]);
	ss2dt(_s2i_interstereo_txt[i]);
      }
    }
    int tmp = mWINWIDTH - 1;
    _s2i_interstereo_w = 1;
    while (tmp) {
      tmp >>= 1;
      _s2i_interstereo_w <<= 1;
    }
    tmp = mWINHEIGHT - 1;
    _s2i_interstereo_h = 1;
    while (tmp) {
      tmp >>= 1;
      _s2i_interstereo_h <<= 1;
    }
    */
    _s2i_interstereo_w = 2048;
    _s2i_interstereo_h = 2048;
    
    _s2i_interstereo_txt[0] = ss2ct(_s2i_interstereo_w, _s2i_interstereo_h);
    _s2i_interstereo_txt[1] = ss2ct(_s2i_interstereo_w, _s2i_interstereo_h);

  }
  
#define IS_DIVISOR 2

  //#define ONETEXTUREONLY
  
  // Right
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
  glViewport(0, 0, mWINWIDTH, mWINHEIGHT/IS_DIVISOR);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawView("r", 0.5);
  DrawExtras();
  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glBindTexture(GL_TEXTURE_2D, _s2i_interstereo_txt[1]);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
		      mWINWIDTH,
		      mWINHEIGHT/IS_DIVISOR);
  glPopAttrib();
  
  // Left
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
  glViewport(0, 0, mWINWIDTH, mWINHEIGHT/IS_DIVISOR);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawView("l", -0.5);
  DrawExtras();

#if !defined(ONETEXTUREONLY)
  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glBindTexture(GL_TEXTURE_2D, _s2i_interstereo_txt[0]);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 
		      mWINWIDTH,
		      mWINHEIGHT/IS_DIVISOR);
  glPopAttrib();
#endif  

  // Remember the graphics state and return it at the end
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  
  // the key for when we are in wireframe mode! - 
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glColor3f(1.0,1.0,1.0);
  
  // interleave the rows in the back buffer!
  // Right
  glViewport(0, 0, mWINWIDTH, mWINHEIGHT);
  glDrawBuffer(GL_BACK_RIGHT);
#if !defined(ONETEXTUREONLY)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
  glStencilFunc(GL_EQUAL, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  
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
  glBindTexture(GL_TEXTURE_2D, _s2i_interstereo_txt[1]);
  glBegin(GL_QUADS);
  glTexCoord2f(0., 0.); 
  glVertex2f(0., 0.);
  glTexCoord2f(0., (float)mWINHEIGHT / (float)_s2i_interstereo_h / 
	       (float)IS_DIVISOR);
  glVertex2f(0., 1.);
  glTexCoord2f((float)mWINWIDTH / (float)_s2i_interstereo_w,
	       (float)mWINHEIGHT / (float)_s2i_interstereo_h / 
	       (float)IS_DIVISOR);
  glVertex2f(1., 1.);
  glTexCoord2f((float)mWINWIDTH / (float)_s2i_interstereo_w, 0.);
  glVertex2f(1., 0.);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  
#if !defined(ONETEXTUREONLY)
  // Left
  glViewport(0, 0, mWINWIDTH, mWINHEIGHT);
  glDrawBuffer(GL_BACK_LEFT);
  glStencilFunc(GL_NOTEQUAL, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  
  // draw the texture we just captured...
  glPushMatrix();
  glColor4f(1., 1., 1., 1.);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, _s2i_interstereo_txt[0]);
  glBegin(GL_QUADS);
  glTexCoord2f(0., 0.); 
  glVertex2f(0., 0.);
  glTexCoord2f(0., (float)mWINHEIGHT / (float)_s2i_interstereo_h / 
	       (float)IS_DIVISOR);
  glVertex2f(0., 1.);
  glTexCoord2f((float)mWINWIDTH / (float)_s2i_interstereo_w,
	       (float)mWINHEIGHT / (float)_s2i_interstereo_h / 
	       (float)IS_DIVISOR);
  glVertex2f(1., 1.);
  glTexCoord2f((float)mWINWIDTH / (float)_s2i_interstereo_w, 0.);
  glVertex2f(1., 0.);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
#endif
  
#endif
  glPopAttrib();
  

  // mark the stencil and/or textures as ready for use
  _s2i_stencilready = 1;
  
}
