/* s2anaglyph.c: S2PLOT anaglyph (red-blue) stereo code
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

void drawView(char *projinfo, double camsca);


/* All of these should probable become *local* variables and never
 * known about to S2PLOT.
 */
OPTIONS *_s2a_options;

#define moptions (*_s2a_options)

void prep_s2anaglyph(OPTIONS *ioptions) {
  _s2a_options = ioptions;
  _s2debug("(internal)", "/S2*ANA* device support loaded");
}

void resize_s2anaglyph(void) {
  // do nothing!
}

int keybd_s2anaglyph(char c) {
  int consumed = 0;
  return consumed;
}

void draw_s2anaglyph(CAMERA cam) {
  /* draw the right and left views to different color channels */
    
  // Right 
  glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE);
  glViewport(0, 0, moptions.screenwidth, moptions.screenheight);
  glDrawBuffer(GL_BACK_RIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawView("r", 1.);
  DrawExtras();
  
  // Left
  glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
  glViewport(0, 0, moptions.screenwidth, moptions.screenheight);
  glDrawBuffer(GL_BACK_LEFT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawView("l", -1.);
  DrawExtras();
  
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}
