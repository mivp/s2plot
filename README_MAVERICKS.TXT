These instructions are for s2plot-3.2.1 for Mavericks on OSX

version 1.0 by C.Fluke (cfluke@swin.edu.au)

0. Assumptions:

A correct installation of gcc4.8 (e.g. gcc48 obtained via macports)
http://gcc.gnu.org/gcc-4.8

A correct installation of libharu (e.g. libharu obtained via macports)
http://libharu.org

A correct installation of the Freetype engine (e.g. freetype obtained via 
macports)
http://www.freetype.org

A correct installation of ImageMagick applications. 



----------

1.Download s2plot-3.2.1 from

http://astronomy.swin.edu.au/s2plot/index.php?title=S2PLOT:Download

2. Bunzip and untar such that you have a directory: s2plot-3.2.1

3. Change into the s2plot-3.2.1 directory and read the INSTALL.TXT and 
ENVIRONMENT.TXT files.

4. Set the following environment variables (preferably in your .cshrc 
or .bashrc file -- I'm using .cshrc examples throughtout), making sure 
you use the relevant path to your s2plot-3.2.1 directory:

S2PATH=/your/path/to/s2plot-3.2.1
S2ARCH=darwin-gnu-x86_64
S2PLOT_PRCDRIVER=s2prcwriter

Add the s2plot scripts directory to your path:

set path = ($path . ${S2PATH}/scripts)

Set any other environment variables as suggested in INSTALL.TXT and 
EVIRONMENT.TXT

5. Give the build.csh command and watch as it fails spectacularly! We are now 
going to fix this...

6. Change into the ${S2PATH}/scripts directory and edit the 
darwin-gnu-x86_64.in file

6a. Modify:

set COMPPATH="/usr/local/bin"

so that the COMPPATH variable points to the location of gcc and g++
(hopefully /usr/local/bin or /usr/bin)

6b. Modify: 

set BASEFLAGS="-D_GNU_SOURCE -I${S2PATH}/src -m64 -DS2DARWIN"

to 

set BASEFLAGS="-D_GNU_SOURCE -I${S2PATH}/src -I/opt/local/include/gcc48/c++/x86_64-apple-darwin13 -m64 -DS2DARWIN"

Make sure that you use the relevant directory for gcc48/c++


6c. Modify: 

set COMMFLAGS="${BASEFLAGS} -O3 -ftree-vectorize -Wall"

to include:  -Wdeprecated-declarations

set COMMFLAGS="${BASEFLAGS} -O3 -ftree-vectorize -Wall -Wdeprecated-declarations"

6d. Modify:

set S2FTDIR="/usr/X11R6"

to the relevant directory for your freetype library installation,
possibly:

set S2FTDIR="/opt/local"


6e. Add after MPLINKS definition: 

set GLUTDIR="/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/System/Library/Frameworks"

ensuring that a relevant path to MacOSX10.9 System/Library/Frameworks is selected. 


6f. Modify: 

  set GLLINKS="${GLLINKS},-framework,GLUT"

to

  set GLLINKS="${GLLINKS} -F${GLUTDIR} -framework GLUT"


6g. Modify:

set GLLINKS="${GLLINKS},-framework,OpenGL,-framework,Foundation,-framework,ApplicationServices,-framework,Accelerate"

to


set GLLINKS="${GLLINKS} -F${GLUTDIR} -framework OpenGL -framework Foundation -framework ApplicationServices -framework Accelerate"

We have now finished editing darwin-gnu-x86_64.in

7. Now edit the file: cbuild.csh

Modify:

$S2CLINKER -o $target ${object} -L${S2PATH}/${S2KERNEL} ${S2LINKS} ${MLLINKS} ${SWLINKS} ${GLLINKS} -L${S2X11PATH}/lib${S2LBITS} ${S2FORMSLINK}  -lX11 ${IMATH} -lm ${XLINKPATH} ${S2EXTRALIB}

to

$S2CLINKER -o $target ${object} -L${S2PATH}/${S2KERNEL} ${S2LINKS} ${MLLINKS} ${SWLINKS} ${GLLINKS} -L${S2X11PATH}/lib${S2LBITS} ${S2FORMSLINK}  ${IMATH} -lm ${XLINKPATH} ${S2EXTRALIB}


8. Now we need to fix 3D-PDF functionality


Change to directory ${S2PATH}/s2prc/newprc



8a. Edit writePRC.h

Comment out:
#ifdef __GNUC__
#include <ext/slist>
#endif

as

//#ifdef __GNUC__
//#include <ext/slist>
//#endif

8b. Find the lines:

#ifdef __GNUC__
typedef __gnu_cxx::slist<PRCAttribute> PRCAttributeList;
#else
typedef std::list<PRCAttribute> PRCAttributeList;
#endif

and modify to:

//#ifdef __GNUC__
//typedef __gnu_cxx::slist<PRCAttribute> PRCAttributeList;
//#else
typedef std::list<PRCAttribute> PRCAttributeList;
//#endif


9. We are now ready to build s2plot
 
9a. Change to the ${S2PATH} directory and run the build.csh script
You may get some warnings, but hopefully samples will start to compile.

> build.csh


9b. Change to the ${S2PATH}/darwin-gnu-x86_64 directory and try some of 
the samples.

eg. ns2point

While the demonstration program is running, press <shift>-<p> to generate
the 3D-PDF.

If the export works correctly, you will see a message like: 
====================================================================
S2PLOT PRC export module

The authors of S2PLOT would appreciate it if research outcomes using
S2PLOT would provide the following acknowledgement: Three-dimensional
visualisation was conducted with the S2PLOT progamming library, and a
reference to:
  D.G.Barnes, C.J.Fluke, P.D.Bourke & O.T.Parry, 2006, Publications
  of the Astronomical Society of Australia, 23(2), 82-93,
or, if you have created 3-d PDF figures using S2PLOT, 
  D.G.Barnes & C.J.Fluke, 2008, New Astronomy, 13, 599.

Thank you!
 ====================================================================

9c. On exiting the demonstration program, use Adobe Reader to open the
generated test 3D PDF: s2plotprc.pdf


10. Next, we will look at modifications to s2volsurf

10a. Get s2volsurf:

https://code.google.com/p/s2volsurf/

Bunzip2 and untar the s2volsurf distribution, will create a directory named
something like s2volsurf-2.0

11. Download zlib library from http://www.zlib.net and copy into the s2volsurf 
directory
(Current: zlib-1.2.8)

Create a symbolic link to this distribtution called zlib:

zlib@ -> zlib-1.2.8


12. Download copies of nifti1.h, nfiti1_io.h, and znzlib.h from 

http://nifti.nimh.nih.gov/pub/dist/src/niftilib/nifti1.h
http://nifti.nimh.nih.gov/pub/dist/src/niftilib/nifti1_io.h
http://nifti.nimh.nih.gov/pub/dist/src/znzlib/znzlib.h

13. Copy the cbuild.csh from ${S2PATH}/scripts into the s2volsurf directory and
modfiy the S2CLINKER definition to be:

$S2CLINKER -o $target ${object} -L${S2PATH}/${S2KERNEL} ${S2LINKS} ${MLLINKS} ${SWLINKS} ${GLLINKS} -L${S2X11PATH}/lib${S2LBITS} ${S2FORMSLINK}  ${IMATH} -lm ${XLINKPATH} ${S2EXTRALIB} -Izlib -Lzlib -lz


(this adds the include path to zlib)


14. In nifit1_io.h change

#include <znzlib.h>
to
#include "znzlib.h"


15. Change into the zlib directory and issue the following commands:

./configure
make

//#define USEGZ 1

16. Now issue the build command:

build.csh

(NOTE: these instructions do not build xrwhist as this requires a PGPLOT 
installation)


