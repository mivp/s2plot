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

s2anim <args>

Required: 
          -np n      number of points to read from each file
          -fn        read in different formats (currently n=0..5)
          -Fr <name>    framefile
Optional:
          -f         full screen
          -s         frame sequential stereo
          -ss        dual screen stereo
          -si        interleaved stereo
          -bb        show bounding box (default: off)
          -ax        show axes (default: off)
          -gr        show grid (default: off)
          -as        start in autospin mode (default: off)
          -os n      object size (default: 1)
          -l         circular looping (default: off)
          -sl        show slider (default: off)
          -m <file>  file with static point markers
          -swap      big/small endian swap
          -vf <file> start with view file
          -d         debug mode
          -h         this text

Press <space> to toggle animation
Press <0> to toggle slider
Press <1> to choose 'back to start' looping
Press <2> to choose 'back and forth' looping
<shift>-<right mouse button> to drag (red) frame slider

Example usage:
> s2anim -ss -f -np 20000 -Fr myfiles.dat -f6 -sl

Use passive stereo (-ss), full screen (-f), reading data (-Fr) from 
"myfiles.dat" which contains one filename per line.  
Each data file has (-np) 20000 points, and the input file comprises 
binary floats (-f5): x, y, z, r, g, b.  The slider bar is drawn (-sl) at
start-up.

Formats:
-f0	Binary floats x, y, z
-f1 	ASCII x, y, z
-f2 	Binary floats x, y, z
-f3	ASCII x, y, z, r, g, b 
-f4 	ASCII x, y, z, c
-f5	Binary floats x, y, z, r, g, b
	
Colours:
r, g, b colour values are between 0.0 and 1.0
c is a colour index usually between 0 (black) and 15 (dark grey)

View file:
Contains camera position (VP), up-vector (VU), view direction vector (VD)
and rotation point (PR) in device coordinates.  These values may be found 
by pressing <d>.

Format:
VP x y z
VU x y z
VD x y z
PR x y z

Static markers:
The marker file comprises additional point geometry, with each line as:

p x y z r g b 
OR
P x y z r g b th

where th is the point thickness
