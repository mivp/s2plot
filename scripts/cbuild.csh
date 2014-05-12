#!/bin/csh -f
## cbuild.csh
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
 # $Id: cbuild.csh 5815 2012-10-19 04:51:26Z dbarnes $
 #
 # usage: cbuild.csh target
 # - compiles 'target.c' and links to produce executable 'target'
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

if ($thisdir == $S2PATH) then
  echo "You must NOT be in directory ${S2PATH} to build your own programs."
  exit(-1);
endif

set target=`echo $1 | sed -e 's/\.c$//g'`
set source=${target}.c
set object=${target}.o
if (! -e ${source}) then
  echo "Cannot find source code: ${source}[.c]"
  exit(-1);
endif 

echo "Compiling source code file ${source} ..."
$S2COMPILER ${S2EXTRAINC} -I${S2PATH}/src $source

echo "Linking object file ${target}.o ..."
$S2CLINKER -o $target ${object} -L${S2PATH}/${S2KERNEL} ${S2LINKS} ${MLLINKS} ${SWLINKS} ${GLLINKS} -L${S2X11PATH}/lib${S2LBITS} ${S2FORMSLINK}  -lX11 ${IMATH} -lm ${XLINKPATH} ${S2EXTRALIB}

if ((${?S2INSTALLPATH})) then
  if (! -d $S2INSTALLPATH) then
    echo "S2INSTALLPATH is set but invalid (i.e. doesn't exist) ... please fix and retry."
    exit(-1);
  else
    echo "Installing ${target} in ${S2INSTALLPATH} ..."
    cp -f $target $S2INSTALLPATH
    rm -f $target
  endif
endif

echo "Cleaning up ..."
rm -rf $object

echo "Done!"
