/* ss2ltt.c
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
 */

#include <stdio.h>
#include <stdlib.h>
#include "s2plot.h"

int main(int argc, char *argv[])
{
   char *latexcmd = "$f(x) = \\Sigma^{\\infty}_{i=0} a_i \\sin(2\\pi x)$";
   XYZ pos[4];					/* Vertices for texture */
   COLOUR col = { 1.0, 1.0, 1.0 };		/* Polygon colour */
   float aspect;				/* Aspect ratio */
   unsigned int id;				/* Texture id */

   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   id = ss2ltt(latexcmd, &aspect);		/* Create latex texture */
	/* Must have S2PLOT_LATEXBIN environment variable correctly set */

   aspect = 1.0/aspect;

   pos[0].x = -1.0; pos[0].y = +aspect; pos[0].z = 0.0;
   pos[1].x = +1.0; pos[1].y = +aspect; pos[1].z = 0.0;
   pos[2].x = +1.0; pos[2].y = -aspect; pos[2].z = 0.0;
   pos[3].x = -1.0; pos[3].y = -aspect; pos[3].z = 0.0;

   ns2vf4x(pos, col, id, 1.0, 'o');		/* Draw the texture */

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
