#!/bin/csh -f
## build-viewer.csh
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
 # $Id: build-viewer.csh 5793 2012-10-09 02:06:45Z dbarnes $
 #
 # usage: ./build-viewer.csh
 #

if (!(${?S2PATH})) then
  echo "S2PATH environment variable MUST be set ... please fix and retry."
  exit(-1);
endif
if (! -d $S2PATH || ! -e ${S2PATH}/scripts/s2plot.csh) then
  echo "S2PATH is set but invalid: ${S2PATH} ... please fix and retry."
  exit(-1);
endif
source ${S2PATH}/scripts/s2plot.csh
if ($status) then
  exit(-1)
endif

set thisdir=`pwd`

if ($thisdir != $S2PATH) then
  echo "You must be in directory ${S2PATH} to run build-samples.csh script."
  echo "(or if you are, try removing any trailing '/' in your S2PATH setting)"
  exit(-1);
endif

if ($S2SHARED == yes) then
  echo "Using shared S2PLOT library for ${S2KERNEL}."
else
  echo "Using static S2PLOT library for ${S2KERNEL}."
endif

if (! -d $S2KERNEL) then
  echo "Directory ${S2KERNEL} does not exist!  Cannot build!  Try running"
  echo "build-lib.csh script first, or re-installing your distribution."
  exit(-1);
endif

if (! -e ${S2KERNEL}/${S2LIBNAME}) then
  echo "Library ${S2LIBNAME} is missing!  Cannot build!  Try running "
  echo "build-lib.csh script first, or re-installing your distribution."
  exit(-1);
endif

echo "Entering target directory ${S2KERNEL}."
cd $S2KERNEL

set S2OBJECTS=""

echo "Compiling S2PLOT viewer utility ..."
$S2COMPILER -DBUILDING_VIEWER  ../src/geomviewer.c -I${S2X11PATH}/include/X11 ${S2FORMSINCL}
set S2OBJECTS="${S2OBJECTS} geomviewer.o"
echo "Linking S2PLOT viewer utility ..."

# this is the normal one
$S2CLINKER -o s2view geomviewer.o -L. ${S2LINKS} ${MLLINKS} ${SWLINKS} ${GLLINKS} -L${S2X11PATH}/lib${S2LBITS} ${S2FORMSLINK} -lXpm -lX11 ${IMATH} -lm ${XLINKPATH}  -ldl

if ($S2KERNEL == darwin && $S2SHARED == yes) then
  echo "Modifying dynamic library path ..."
  install_name_tool -change ${S2PATH}/${S2KERNEL}/${S2LIBNAME} @executable_path/${S2LIBNAME} s2view
endif

echo Removing objects ...
rm -rf $S2OBJECTS

echo Done!
