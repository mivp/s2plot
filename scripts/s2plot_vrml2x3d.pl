#!/usr/bin/perl
## s2plot_vrml2x3d.pl
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
 # $Id: s2plot_vrml2x3d.pl 5791 2012-10-09 01:51:42Z dbarnes $
 #
 # s2plot-vrml2x3d.pl: post-process an S2PLOT-exported VRML (.wrl) file
 #                     to remove unsupported elements (eg lineProperties)
 #                     and create duplicate textures that are used with
 #                     more than one colour.

$in_appearance = 0;
$in_material = 0;
$has_colour = 0;
$in_texture = 0;
$has_texture = 0;
$this_texture = "";
@this_colour = (-1., -1., -1.);
$in_lineproperties = 0;

if ($#ARGV != 0) {
  die "Please provide wrl filename on command line\n";
}
$fin = $ARGV[0];
$fout = "mod-" . $fin;

open(FIN, "< $fin");
open(FOUT, "> $fout");

$nextout = "NULL";

# assoc. array for unique texture - colour combinations
%coltexts = ();
# counter for creating new (copied) files of textures
$idx = 0;

while(<FIN>) {

  if (!($nextout eq "NULL")) {
    print FOUT $nextout, "\n";
  }

  chop;
  $nextout = $_;

  if (/appearance Appearance {/) {
    $in_appearance = 1;
    next;
  }

  if ($in_appearance) {

    if (/material Material {/) {
      $in_material = 1;
      next;
    } elsif (/texture ImageTexture {/) {
      $in_texture = 1;
      next;
    } elsif (/lineProperties LineProperties {/) {
      $in_lineproperties = 1;
      $nextout = "NULL";
      next;
    }

    if ($in_lineproperties) {
      if (/}/) {
	$in_lineproperties = 0;
      }
      $nextout = "NULL";
      next;
    }

    if ($in_material) {

      #print "in material: ", $_, "\n";

      if (/emissiveColor\s+(.+)\s+(.+)\s+(.+)/) {
	@this_colour = ($1, $2, $3);
	$has_colour = 1;
	next;
      }
      if (/}/) {
	$in_material = 0;
	next;
      }
    }

    if ($in_texture) {

      #print "in texture: ", $_, "\n";

      if (/url\s+\[\s*"(.+)"\s*\]/) {
	$this_texture = $1;
	$has_texture = 1;

	if ($has_texture && $has_colour) {
	  # round colour components to 2 decimal places
	  # create hash of colours with this texture
	  # see if we've already made a copy for this combination
	  # copy texture and add to "dictionary" if new one needed
	  # use from dictionary
	  #print $this_texture, "\n";

	  $string = sprintf("%s:%4.2f:%4.2f:%4.2f", $this_texture,
			    $this_colour[0], $this_colour[1],
			    $this_colour[2]);

	  if (!exists($coltexts{$string})) {
	    $idx += 1;
	    
	    $texname = sprintf("duptex%05d.png", $idx);
	    system "cp $this_texture $texname";
	    $coltexts{$string} = $texname;

	  }

	  if (exists($coltexts{$string})) {

	    $nextout = sprintf("\t\t\turl [ \"%s\" ]\n",
			       $coltexts{$string});

	  } else {
	    die "Cannot find colour-texture combination to use!\n";
	  }


	}


	$has_texture = 0;
	$this_texture = "";
	$has_colour = 0;
	$this_colour = (-1., -1., -1.);
	next;
      }

      if (/}/) {
	$in_texture = 0;
	next;
      }
    }


    if (/}/ && !$in_texture && !$in_material) {
      $in_appearance = 0;
      next;
    }

  }
}

if (!($nextout eq "NULL")) {
  print FOUT $nextout, "\n";
}

close(FOUT);
close(FIN);

$xout = $fin;
$xout =~ s/\.wrl/\.x3d/;
system "v2x3d $fout $xout";
system "rm -f $fout";
