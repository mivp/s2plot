## s2plot.csh
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
 # $Id: s2plot.csh 5829 2012-11-16 03:43:51Z dbarnes $
 #

#####################################################################
## Users may set the following environment variables.  If they are ##
## not set, then the defaults below apply.                         ##
#####################################################################

# S2ARCH
# Set to one of the following OS-compiler-CPU choices.  If
# not set, then an attempt is made to select a reasonable default.
#
#   linux-gnu-i386
#   linux-gnu-x86_64
#   darwin-gnu-i386
#   darwin-gnu-x86_64
#
# See ENVIRONMENT.TXT for system details for the above builds.
#
# Note that definitions for the selected S2ARCH are found in 
# the ${S2ARCH}.in file in ${S2PATH}.

# S2SHARED
# Set to "no" or "yes" without quotes to choose whether shared or static
# libraries are used when linking.  Change this with care.

# S2MPI
# Set to "no" or "yes" without quotes to choose whether S2PLOT is built
# with MPICH and thus support for multihead devices (/S2MULTI)

# S2FREEGLUT
# Set to "no" or "yes" without quotes to choose whether the re-entrant
# s2disp function will be used on Linux systems that use the freeglut
# library.  ONLY works on Linux with freeglut version >= 2.2.0.

# S2FREETYPE
# Set to "no" or "yes" without quotes to choose whether the freetype2
# extensions will be linked in with programs.

#####################################################################
## Options that may need to be set for your installation of s2plot ##
#####################################################################

# You may need to change the following path settings.  Include files
# should be in ${*PATH}/include and libraries in ${*PATH}/lib.
set S2X11PATH=/usr/X11R6


######################################################################
## CHANGES BELOW HERE ARE NOT GENERALLY NEEDED FOR END-USERS        ##
######################################################################

set S2VERSION="3.4.0"

######################################################################

# S2SHARED should be "no" or "yes" (without quotes).  Change this with
# care.  After changing this value, all binaries must be (recompiled
# and) relinked using the build scripts.
if (!(${?S2SHARED})) then
  set S2SHARED=yes
endif
if (($S2SHARED != yes) && ($S2SHARED != no)) then
  echo S2PLOT environment variable S2SHARED must be "yes" or "no".
  exit -1
endif

# set S2MPI to yes to build with MPI support.
if (!(${?S2MPI})) then
  set S2MPI=no
endif
if (($S2MPI != yes) && ($S2MPI != no)) then
  echo S2PLOT environment variable S2MPI must be "yes" or "no".
  exit -1
endif

# If you are on a linux machine AND you know you are using freeglut
# version >= 2.2.0, then you can set the following to "yes" so that
# the re-entrant s2disp function will be available via the freeglut
# extension.  After changing this value, all binaries must be 
# (recompiled and) relinked using the build scripts.
if (!(${?S2FREEGLUT})) then
  set S2FREEGLUT=no
endif
if (($S2FREEGLUT != yes) && ($S2FREEGLUT != no)) then
  echo S2PLOT environment variable S2FREEGLUT must be "yes" or "no".
  exit -1
endif

# Freetype2 font support
if (!(${?S2FREETYPE})) then
  set S2FREETYPE=yes
endif
if (($S2FREETYPE != yes) && ($S2FREETYPE != no)) then
  echo S2PLOT environment variable S2FREETYPE must be "yes" or "no".
  exit -1
endif

# Mesh file support: set this value only if advised by your 
# MirrorDome supplier.  After changing this value, all binaries
# must be (recompiled and) relinked using the build scripts.
if (!(${?S2MESHLIB})) then
  setenv S2MESHLIB "stub"
endif

# Which window system is used?
if (!(${?S2WINLIB})) then
  set S2WINLIB="glut"
endif

#####################################################################
## It is unlikely that you need to change anything below this line ##
#####################################################################

if (!(${?S2ARCH})) then
    set S2KERNEL=`uname -s`
    if (($S2KERNEL != Linux) && ($S2KERNEL != Darwin) && ($S2KERNEL != SunOS)) then
        echo "Unknown kernel/operating system: ${S2KERNEL}.  Cannot deduce "
	echo "architecture for building."
        exit  -1
    endif
    if ($S2KERNEL == Linux) then
        set S2CPU=`uname -i`
        if ($S2CPU == i386) then
	    set S2ARCH=linux-gnu-i386
	else if ($S2CPU == x86_64) then
            set S2ARCH=linux-gnu-x86_64
        else
	    echo "Unknown CPU type: ${S2CPU}.  Cannot deduce architecture "
	    echo "for building."
	    exit  -1
	endif
    endif
    if ($S2KERNEL == Darwin) then
	set S2CPU=`uname -p`
	if ($S2CPU == i386) then
	    set S2ARCH=darwin-gnu-i386
        else if (($S2CPU == ppc) || ($S2CPU == powerpc)) then
	    set S2ARCH=darwin-gnu-ppc
	else
	    echo "Unknown CPU type: ${S2CPU}.  Cannot deduce architecture "
	    echo "for building."
	    exit  -1
	endif
    endif
    if ($S2KERNEL == SunOS) then
        set S2CPU=`uname -p`
	if ($S2CPU == sparc) then
	    set S2ARCH=sunos-gnu-sparc
	else
	    echo "Unknown CPU type: ${S2CPU}.  Cannot deduce architecture "
	    echo "for building."
	    exit  -1
        endif
    endif
endif
if (!(${?S2ARCH})) then
    echo "Unable to auto-determine S2ARCH.  Bailing out!"
    exit  -1
endif

set S2KERNEL=${S2ARCH}

# initialise variables
set S2COMPILER=""
set S2FOMPILER=""
set S2CLINKER=""
set S2FLINKER=""
set fext="f90"
set IMATH=""
set DARCHFLAGS=" "
set DLARCHFLAGS=" "
set S2ADDINCL=" "
# below gets changed to 64 if we are building on x86_64
set S2LBITS=""


if ($S2SHARED == yes) then
  set S2LINKS="-ls2plot"
else
  set S2LINKS="${S2PATH}/${S2ARCH}/libs2plot.a"
endif

if ($S2FREETYPE == yes) then
  if ($S2SHARED == yes) then
    set S2LINKS="${S2LINKS} -ls2freetype"
  else
    set S2LINKS="${S2LINKS} ${S2PATH}/${S2ARCH}/libs2freetype.a"
  endif
endif

# read settings for this architecture
source ${S2PATH}/scripts/${S2ARCH}.in

set S2FORMSINCL=" "
set S2FORMSLINK=" "

# propagate whether to support freetype2 fonts
if ($S2FREETYPE == yes) then
  set S2COMPILER="${S2COMPILER} -DS2FREETYPE"
  set S2CCMPILER="${S2CCMPILER} -DS2FREETYPE"
  set S2FORMSLINK="${S2FORMSLINK} -L${S2FTDIR}/lib${S2LBITS} -lfreetype"
endif

# set up for no extra incs / libs by default
if (!(${?S2EXTRAINC})) then
  set S2EXTRAINC=" "
endif
if (!(${?S2EXTRALIB})) then
  set S2EXTRALIB=" "
endif

exit

