#!/bin/csh -f
## clbuild.csh
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
 # $Id: clbuild.csh 5796 2012-10-09 02:12:55Z dbarnes $
 #
 # usage: clbuild.csh target file1 [file2 ...]
 # - compiles 'file1.c' (and 'file2.c' ...) and produces a shared or
 #   static library libtarget according to S2SHARED variable.
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
  echo "You must NOT be in directory ${S2PATH} to build your own library."
  exit(-1);
endif

if ($#argv < 2) then
  echo "Usage: $0 target file1 [file2 ...]"
  echo " e.g.: clbuild.csh s2ext s2e-campath s2e-cfastro s2e-cfsmooth s2e-cfslider"
  exit(-1);
endif
set lastarg = $#argv

set target=$1
set objs=""
@ ac = 2;
while ($ac <= $lastarg) 
  set source=$argv[$ac]
  if (! -e ${source}) then
    set source=$argv[$ac].c
    if (! -e ${source}) then
      echo "Cannot find source code: ${source}."
      exit(-1);
    endif 
  endif
  echo "Compiling source code file ${source} ..."
  $S2LIBCOMPILER ${S2EXTRAINC} -I. -I${S2PATH}/src $source
  if ($status) then
    exit(-1)
  endif

  set newobj = `echo $source | sed -e 's/\.c/\.o/g'`
  set objs="${objs} ${newobj}"

  @ ac += 1;
end

set S2ARBLIBNAME="lib${target}".${S2ARBLIBEXT}
set newname=$S2ARBLIBNAME
echo Creating library $newname ...

# use eval here because we have set the variable S2ARBLIBNAME that needs 
# to be expanded in $S2ARBLIBCTR
eval $S2ARBLIBCTR $objs

# for dynamic darwin libs, change the name of the library
if (($S2ARCH == "darwin-gnu-i386") && ($S2SHARED == "yes")) then
  install_name_tool -id $newname $newname
endif

$S2LIBINDEX $newname

if (! -d ./{$S2ARCH}) then
  mkdir -p ./${S2ARCH}
endif
if (-e ./${S2ARCH}/$newname) then
  rm -f ./${S2ARCH}/$newname
endif
/bin/mv -f $newname ./${S2ARCH}

echo "Cleaning up ..."
rm -rf $objs

echo "Done!"
