/* s2dispstub.c
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
 * $Id: s2dispstub.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#include "s2plot.h"
#include <stdio.h>

/* Draw the scene and enter interactive mode.  Based on argument, return
 * immediately after drawing view (0), return after arg seconds, or 
 * only return when user hits 'TAB' key.  If restorecamera > 0, then 
 * the camera position will be returned to "home".
 */

/* This is the "stub" version, which basically issues an advisory
 * and calls s2show.
 */
void s2disp(int idelay, int icamerarestore) {

  fprintf(stderr, "Advisory: this 's2disp' function is not re-entrant, and");
  fprintf(stderr, "\ncannot return control to your main program.\n");
  fprintf(stderr, " - proceeding anyway, review documentation to learn how\n");
  fprintf(stderr, "   to overcome this limitation.\n");

  s2show(1);

  /* code never gets to here */
  return;
}
