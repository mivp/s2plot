## README.TXT
 #
 # Copyright 2006-2012 David G. Barnes, Paul Bourke, Christopher Fluke
 #
 # This file is part of S2PLOT.
 #
 # S2PLOT is free software: you can redistribute it and/or modify it
 # under the terms of the GNU General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 #
 # S2PLOT is distributed in the hope that it will be useful, but
 # WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 # General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with S2PLOT.  If not, see <http://www.gnu.org/licenses/>. 
 #
 # We would appreciate it if research outcomes using S2PLOT would
 # provide the following acknowledgement:
 #
 # "Three-dimensional visualisation was conducted with the S2PLOT
 # progamming library"
 #
 # and a reference to
 #
 # D.G.Barnes, C.J.Fluke, P.D.Bourke & O.T.Parry, 2006, Publications
 # of the Astronomical Society of Australia, 23(2), 82-93.
 #
 # $Id: README.TXT 5799 2012-10-09 02:29:33Z dbarnes $
 #

# ==================================================================
# S2PLOT includes code written by Eric Grosse and W. M. Coughran, Jr.
# We are grateful for the provision of this code, and reproduce the
# required notice forthwith:
#
#/*
# *  Creates texture array with saturation levels in the y (t) direction
# *  and hue and lightness changes in x (s) direction.
# *  Black band on both hue ends, for plotting data out of range.
# *			[0,black_border-1] and [texture_size-black_border,
# *                      texture_size-1] are black.  Otherwise, use cindex =
# *			black_border + h*(texture_size-2*black_border-1).
# *			Returns a malloc'ed array of texture_size*texture_size
# *			longs with successive texture_size blocks having
# *			different saturation values; each long has r,g,b,0
# *			stored as unsigned chars.
# *
# * The authors of this software are Eric Grosse and W. M. Coughran, Jr.
# * Copyright (c) 1991 by AT&T.
# * Permission to use, copy, modify, and distribute this software for any
# * purpose without fee is hereby granted, provided that this entire notice
# * is included in all copies of any software which is or includes a copy
# * or modification of this software and in all copies of the supporting
# * documentation for such software.
# * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
# * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
# * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
# * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
# *
# * We thank Cleve Moler for describing the "hot iron" scale to us.
# */
#
# ==================================================================

Hi, and welcome to S2PLOT, the Swinburne 3d Plotting Library.

To get started quickly, follow 'INSTALL.TXT'.

For a quick review of environment variables used by S2PLOT, consult
'ENVIRONMENT.TXT'.

Documentation for the S2PLOT functions can be found on the S2PLOT 
web page: http://astronomy.swin.edu.au/s2plot

For further information, consult the demonstration programs found in
the "examples" subdirectory.

Please note that the state of documentation of the architecture and
build system of S2PLOT is very much out of date.

 - David G. Barnes, September 2012.
   
david.g.barnes@gmail.com

