#!/bin/csh -f
## build-lib.csh
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
 # $Id: build-lib.csh 5803 2012-10-09 03:46:28Z dbarnes $
 #
 # usage: ./build-lib.csh [-n = don't clean target dir]
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
  echo "You must be in directory ${S2PATH} to run the build.csh script."
  echo "(or if you are, try removing any trailing '/' in your S2PATH setting)"
  exit(-1);
endif

if ($S2SHARED == yes) then
  echo "Building shared S2PLOT library for ${S2KERNEL}."
else
  echo "Building static S2PLOT library for ${S2KERNEL}."
endif

if (! -d $S2KERNEL) then
  echo "Creating target directory ${S2KERNEL}."
  mkdir $S2KERNEL
else 
  if ($1 != "-n") then  
    echo "Cleaning target directory ${S2KERNEL}."
    rm -rf ${S2KERNEL}/*
  endif
endif
cd $S2KERNEL

echo Compiling support code
$S2LIBCOMPILER ../src/paulslib.c ../src/bitmaplib.c ../src/opengllib.c ../src/hiddenMouseCursor.c ${S2ADDINCL}
set S2OBJECTS="paulslib.o bitmaplib.o opengllib.o hiddenMouseCursor.o"

echo Compiling miscellaneous support code
$S2LIBCOMPILER ../src/rainbow.c ../src/hotiron.c
set S2OBJECTS="${S2OBJECTS} rainbow.o hotiron.o"

echo Compiling S2PLOT interface ...
$S2LIBCOMPILER -DBUILDING_S2PLOT ../src/geomviewer.c ../src/s2plot.c -I${S2X11PATH}/include/X11 ${S2FORMSINCL} ${S2ADDINCL}
set S2OBJECTS="${S2OBJECTS} geomviewer.o s2plot.o"

# for darwin builds, s2disp is included in the S2PLOT library
#  (see below for linux builds)
if ($S2OSTYPE == darwin) then
  echo "Compiling s2disp function ..."
  $S2LIBCOMPILER -DBUILDING_S2PLOT ../src/s2disp.c -I${S2X11PATH}/include/X11 ${S2FORMSINCL}
  set S2OBJECTS="${S2OBJECTS} s2disp.o"
endif

echo Compiling S2PLOT FORTRAN stubs ...
$S2LIBCOMPILER ../src/s2plot_fstubs.c -I${S2X11PATH}/include/X11 ${S2FORMSINCL}
set S2OBJECTS="${S2OBJECTS} s2plot_fstubs.o"

echo Creating S2PLOT library ...
if ($S2OSTYPE == darwin && $S2SHARED == yes) then
  $S2LIBCTR $S2OBJECTS $GLLINKS -L${S2X11PATH}/lib ${S2FORMSLINK} -lXpm -lX11 -lm
  echo "Modifying dynamic library path ..."
  install_name_tool -change ${S2PATH}/${S2KERNEL}/${S2LIBNAME} @executable_path/${S2LIBNAME} ${S2PATH}/${S2KERNEL}/${S2LIBNAME}
else
  $S2LIBCTR $S2OBJECTS
  #echo done
endif
$S2LIBINDEX $S2LIBNAME

# create glut windowing interface
if ($S2WINLIB == glut) then
  echo Creating glut interface library ...
  $S2LIBCOMPILER -DBUILDING_S2PLOT ../src/s2winglut.c
  $S2GLUTWINLIBCTR s2winglut.o
  $S2LIBINDEX $S2GLUTWINNAME
  rm -f s2winglut.o
endif

# create freetype2 library
if ($S2FREETYPE == yes) then
  echo Creating s2freetype library ...
  $S2LIBCOMPILER -I${S2FTDIR}/include -I${S2FTDIR}/include/freetype2 ../src/s2freetype.c
  $S2FTLIBCTR s2freetype.o
  $S2LIBINDEX $S2FTNAME
  rm -f s2freetype.o
endif

# for linux and darwin builds, three implementations of ReadBinMesh are
# available, and placed in their own libraries
echo Creating ReadBinMesh libraries ...

# 1. stub that does nothing but tell the user it does nothing
$S2LIBCOMPILER ../src/s2meshstub.c
$S2MESHSTUBLIBCTR s2meshstub.o
$S2LIBINDEX $S2MESHSTUBNAME
rm -f s2meshstub.o

# 2. stub that handles free (unlocked) binary mesh files
$S2LIBCOMPILER ../src/s2freemesh.c -I${S2X11PATH}/include/X11 ${S2FORMSINCL}
$S2FREEMESHLIBCTR s2freemesh.o
$S2LIBINDEX $S2FREEMESHNAME
rm -f s2freemesh.o

# 3. stub that handles node-locked binary mesh files
if (-e ../closed/misc/s2lockedmesh.c) then
  $S2LIBCOMPILER ../closed/misc/s2lockedmesh.c -I${S2X11PATH}/include/X11 ${S2FORMSINCL}
  $S2LOCKEDMESHLIBCTR s2lockedmesh.o
  $S2LIBINDEX $S2LOCKEDMESHNAME
  rm -f s2lockedmesh.o
endif

if ($S2OSTYPE == darwin && $S2SHARED == yes) then
  install_name_tool -change ${S2PATH}/${S2KERNEL}/${S2MESHSTUBNAME} @executable_path/${S2MESHSTUBNAME} ${S2PATH}/${S2KERNEL}/${S2MESHSTUBNAME}
  install_name_tool -change ${S2PATH}/${S2KERNEL}/${S2FREEMESHNAME} @executable_path/${S2FREEMESHNAME} ${S2PATH}/${S2KERNEL}/${S2FREEMESHNAME}
  if (-e ${S2PATH}/${S2KERNEL}/${S2LOCKEDMESHNAME}) then
    install_name_tool -change ${S2PATH}/${S2KERNEL}/${S2LOCKEDMESHNAME} @executable_path/${S2LOCKEDMESHNAME} ${S2PATH}/${S2KERNEL}/${S2LOCKEDMESHNAME}
  endif
endif

if (-e ../src/maps/swinsoft/lib/${S2OSTYPE}/libmapFile.a) then
  cp ../src/maps/swinsoft/lib/${S2OSTYPE}/libmapFile.a .
  ranlib libmapFile.a
endif

# for linux builds, s2disp versions are placed in external libraries
#  (see above for darwin builds)
if ($S2OSTYPE == linux) then
  echo Creating s2disp libraries ...

  $S2LIBCOMPILER ../src/s2dispstub.c -I${S2X11PATH}/include/X11 ${S2FORMSINCL}
  $S2DISPSTUBLIBCTR s2dispstub.o
  $S2LIBINDEX $S2DISPSTUBNAME
  rm -f s2dispstub.o
 
  if ($S2OSTYPE == linux) then
    $S2LIBCOMPILER ../src/s2disp.c -I${S2X11PATH}/include/X11 ${S2FORMSINCL}
    $S2DISPFGLIBCTR s2disp.o
    $S2LIBINDEX $S2DISPFGNAME
    rm -f s2disp.o
  endif

endif

echo Removing objects ...
rm -rf $S2OBJECTS

# these device drivers are closed source (not distributed with S2PLOT)
foreach driver (s2interstereo s2anaglyph s2fishdome s2warpstereo)
  if (-e ../closed/devices/${driver}.c) then
    echo Building device driver ${driver} ...
    $S2MODCMPLR ../closed/devices/${driver}.c
    if ($driver == s2warpstereo) then
      $S2MODMAKER -o ${driver}.so ${driver}.o -L. ${MLLINKS} 
    else
      $S2MODMAKER -o ${driver}.so ${driver}.o -L.
    endif  
    rm -f ${driver}.o
  endif
end

echo Done!
