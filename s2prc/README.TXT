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
 #
 # S2PLOT makes use of the "newprc" software code, found in the 
 # subdirectory "$S2PATH/s2prc/newprc".  The newprc code is 
 # Copyright 2008 Orest Shardt with enhancements contributed by 
 # Michail Vidiassov.  Please refer to the copyright notices in
 # the newprc code for license information.
 #
 # $Id: README.TXT 5775 2012-09-25 04:56:01Z dbarnes $
 #

Creating 3-d PDF figures with S2PLOT and the PRC export module.

You must have S2PLOT (version 3.0 or newer) installed and working
correctly, with appropriately set S2PATH and S2ARCH environment
variables, as well as a recent installation of TeX that includes the
"pdflatex" executable.

The Haru free PDF library ( http://libharu.org/ ) is also required.

0. Install S2PLOT and ensure you have set the necessary environment
   variables, including S2PLOT_PRCDRIVER.

1. Obtain and install Alexander Grahn's "movie15" LaTeX style package:

   http://www.ctan.org/tex-archive/macros/latex/contrib/movie15/

   Note that installation of this package simply entails: "Put the
   file movie15.sty in a place where TeX can find it."  The movie15
   package may have dependencies on other TeX packages.

2. In another working directory, build and run an S2PLOT program.  In
   the S2PLOT functions directory, "s2box.c", "s2skypa.c" and
   "s2funtc.c" make good test examples.  While it is running, press
   "Shift-P" to export a PRC-format file (and PDF preview) of the 3-d
   geometry.  Upon exiting the S2PLOT program you should find
   newly-written files "s2direct.prc" and "s2plotprc.pdf" in the
   current directory.

3. Open the document "s2plotprc.pdf" in Adobe Reader to see a quick
   preview of your 3d figure.  Adjust your code, and run it again,
   and view this file again until you are happy with the result.

4. To include the figure in your own PDF file, copy the provided
   "s2direct.tex" and "s2plot-prc.js" files to your working directory.
   Making sure both "s2direct.tex" and "s2direct.prc" are in this
   directory, and that "movie15.sty" is available to TeX (most easily
   accomplished by also copying it to your working directory), now run

   pdflatex s2direct

   Provided it doesn't fail, run it twice more to get page references
   etc. correct (standard LaTeX operation procedure!), then view the
   output file "s2direct.pdf" in Adobe Reader.  You may need to enable
   3-d and adjust the 3-d options in the Adobe Reader preferences, to
   suit.

N.B. S2PLOT handles and screen (2-d) geometry are not exported.
Polygons textured via 3-d textures (ns2texpoly3d) are also not
exported (3-d textures are not available in the PRC format), nor are
transparent dots (ns2vpa - the PRC format does not handle transparent,
coloured vertices adequately).

N.B. This is the first public release of the S2PLOT PRC export
module.  Clarity and correctness of operation has been favoured over
optimisation and speed.

Please remember to acknowledge the authors if you use the PRC export
module for your research and/or publications.


