#!/usr/bin/perl

use Math::Trig;

# coordinate system:
# centre of CAVE2 is at (0,0,0)
# +ve x is to the right looking into the CAVE2
# +ve z is backwards looking into the CAVE2
# and therefore $ve y is up, for a right hand coordinate system

# constants
$diagonal = sqrt(2400.0*2400.0 + 1000.0*1000.0); # screen diagonal in mm
$aspect = 16.0 / 36.0;   # screen aspect ratio x/y
$nscreens = 4;           # ahem, number of screens :-)
$screenx = 320;          # npixels in x 
$device = "/S2MONO";

$nmasterscreens = 1;       # 0 no master, 1 = master
$masterdiagonal = sqrt(2400.0*2400.0 + 4000.0*4000.0);
$masteraspect = 16.0 / 9.0; # master screen aspect ration x/y
$masterx = 800;           
$masterdevice = "/S2MONO";

$radius = 3800.0;        # radius (centre to edge of screen NOT centre of screen) in mm


# END USER CONFIG SECTION
$radius /= 1000.0;
$count = 0;

if ($nmasterscreens > 1) {
    print "Only one masterscreen supported!\n";
    exit;
} elsif ($nmasterscreens == 1) {

    $dx = $masterx;
    $dy = $dx / $masteraspect;
# convert to metres
    $masterdiagonal /= 1000.0;
    
# derived values
    $sy = $masterdiagonal / sqrt(1 + $masteraspect*$masteraspect);        # screen height in mm
    $sx = $masterdiagonal / sqrt(1 + 1.0/($masteraspect*$masteraspect));  # screen width in mm
    $theta = 2.0 * asin(0.5 * $sx / $radius);       # screen subtends this angle
    $offset = ($nmasterscreens % 2) ? ($nmasterscreens-1)/2 : $nmasterscreens/2 - 1;   # offset to apply to screen number

    if ($nmasterscreens % 2) {
	$offset += 0.5;
    } else {
	$offset += 1;
    }



# loop over master screens
    for ($i = 0; $i < $nmasterscreens; $i++) {
	
	$idx = $i - $offset;
	$pa_x = $radius * sin($idx * $theta);
	$pa_z = -$radius * cos($idx * $theta);
	$pa_y = -0.5 * $sy;
	
	$pb_x = $radius * sin(($idx + 1) * $theta);
	$pb_z = -$radius * cos(($idx + 1) * $theta);
	$pb_y = $pa_y;
	
	$pc_x = $pa_x;
	$pc_z = $pa_z;
	$pc_y = +0.5 * $sy;
	
	# my positon on global screen (0,0) -> (1,1)
	$canvas_x1 = $i / $nmasterscreens;
	$canvas_x2 = ($i + 1) / $nmasterscreens;
	#$canvas_x1 = 0.0;
	#$canvas_x2 = 1.0;
	$canvas_y1 = 0.0;
	$canvas_y2 = 1.0;
	
	printf STDOUT "%d m $device $dx $dy %f %f %f %f %f %f %f %f %f %f %f %f %f\n", $count, $canvas_x1, $canvas_y1, $canvas_x2, $canvas_y2, $pa_x,$pa_y,$pa_z, $pb_x,$pb_y,$pb_z, $pc_x,$pc_y,$pc_z;
	$count = $count + 1;
    }

}



$dx = $screenx;
$dy = $dx / $aspect;

# convert to metres
$diagonal /= 1000.0;

# derived values
$sy = $diagonal / sqrt(1 + $aspect*$aspect);        # screen height in mm
$sx = $diagonal / sqrt(1 + 1.0/($aspect*$aspect));  # screen width in mm
$theta = 2.0 * asin(0.5 * $sx / $radius);       # screen subtends this angle
$offset = ($nscreens % 2) ? ($nscreens-1)/2 : $nscreens/2 - 1;   # offset to apply to screen number

if ($nscreens % 2) {
    $offset += 0.5;
} else {
    $offset += 1;
}


# loop over screens
for ($i = 0; $i < $nscreens; $i++) {
    
    $idx = $i - $offset;
    $pa_x = $radius * sin($idx * $theta);
    $pa_z = -$radius * cos($idx * $theta);
    $pa_y = -0.5 * $sy;

    $pb_x = $radius * sin(($idx + 1) * $theta);
    $pb_z = -$radius * cos(($idx + 1) * $theta);
    $pb_y = $pa_y;

    $pc_x = $pa_x;
    $pc_z = $pa_z;
    $pc_y = +0.5 * $sy;

    # my positon on global screen (0,0) -> (1,1)
    $canvas_x1 = $i / $nscreens;
    $canvas_x2 = ($i + 1) / $nscreens;
    #$canvas_x1 = 0.0;
    #$canvas_x2 = 1.0;
    $canvas_y1 = 0.0;
    $canvas_y2 = 1.0;

    printf STDOUT "%d s $device $dx $dy %f %f %f %f %f %f %f %f %f %f %f %f %f\n", $count, $canvas_x1, $canvas_y1, $canvas_x2, $canvas_y2, $pa_x,$pa_y,$pa_z, $pb_x,$pb_y,$pb_z, $pc_x,$pc_y,$pc_z;
	$count = $count + 1;
}




    
    
