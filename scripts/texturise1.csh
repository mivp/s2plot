#!/bin/csh
## texturise1.csh
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
 # $Id: texturise1.csh 5796 2012-10-09 02:12:55Z dbarnes $
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

if (!(${?S2PLOT_IMPATH})) then
  echo "S2PLOT_IMPATH environment variable not set.  Please check your"
  echo "s2plot.csh file.  *ATTEMPTING* to proceed with default (/usr/bin)."
  setenv S2PLOT_IMPATH /usr/bin
endif
# should check for presence of required binaries...


# usage: "texturise.csh [-crop | -scale | -scale+] inputimage outputimage"
#
# -crop:  crop both sides to largest power-of-2 that is still smaller
#         than each sidelength; image will not be stretched/squashed,
#         but the sides of the image will probably be cropped off.
#
# -scale: scale both sides to largest power-of-2 that is still smaller
#         than each sidelength; the entire image will be present, but
#         will probably be stretched/squashed.
#
# -scale+: scale both sides to largest power-of-2 that is just larger
#          than each sidelength; the entire image will be present, but
#          will probably be stretched/squashed.
#
# ONE OF -crop OR -scale OR -scale+ MUST BE GIVEN!
#
# inputimage: name of input image to convert.  All formats supported 
#             by ImageMagick are allowed.
#
# script *requires* ImageMagick, awk and bc to operate.  Very little
# sanity checking is done.

if ($#argv < 3) then
  echo
  echo "Usage: textureise.csh [-crop | -scale | -scale+] inputimage outputimage"
  echo "Must give one of -crop or -scale or -scale+"
  echo
  exit(-1)
endif

set action=$1
if (("$action" != "-crop") && ("$action" != "-scale") && ("$action" != "-scale+")) then
  echo
  echo "Usage: textureise.csh [-crop | -scale | -scale+] inputimage outputimage"
  echo "Must give one of -crop, -scale or -scale+"
  echo
  exit(-1)
endif

set imp=$S2PLOT_IMPATH

set iname=$argv[2]
set oname=$argv[3]

$imp/identify $iname >& /dev/null
if ($status) then
  echo "  ! cannot find or understand file - $iname - skipping !"
  exit(-1)
endif

# find input image dimensions ix, iy
set ixydim=`$imp/identify $iname | awk '{print $3}'`
set ix=`echo $ixydim | awk -F x '{print $1}'`
set iy=`echo $ixydim | awk -F x '{print $2}'`
set iy=`echo $iy | awk -F + '{print $1}'`

echo $ix $iy

# figure out output image dimensions ox, oy
set ox=1;
while ($ox <= $ix) 
  @ ox = $ox + $ox
end
if ("$action" != "-scale+") then
  set ox=`echo $ox / 2 | bc`
endif
set oy=1;
while ($oy <= $iy) 
  @ oy = $oy + $oy
end
if ("$action" != "-scale+") then
  set oy=`echo $oy / 2 | bc`
endif

# switch based on requested action  
if ("$action" == "-crop") then
  # calculate x and y offsets for centred crop, dx and dy
  set dx=`echo "($ix - $ox) / 2" | bc`
  set dy=`echo "($iy - $oy) / 2" | bc`
  $imp/convert -crop ${ox}x${oy}+${dx}+${dy} -colorspace RGB $iname $oname
else if (("$action" == "-scale") || ("$action" == "-scale+")) then
  set geom="${ox}x${oy}+0+0"
  $imp/convert -resize ${ox}x${oy}! -colorspace RGB -type TrueColor $iname $oname
endif

if ($status == 0) then
  #echo "  > wrote output file $oname"
  #echo $oname
else 
  #echo "  ! failed to convert file !"
  exit(-1)
endif

