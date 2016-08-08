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
$nscreens = 3;           # ahem, number of screens :-)
$radius = 3800.0;        # radius (centre to edge of screen NOT centre of screen) in mm
$device = "/S2MONO";
$dx = 320;
$dy = 720;

# convert to metres
$diagonal /= 1000.0;
$radius /= 1000.0;

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

    printf STDOUT "%d $device $dx $dy %f %f %f %f %f %f %f %f %f\n", $i, $pa_x,$pa_y,$pa_z, $pb_x,$pb_y,$pb_z, $pc_x,$pc_y,$pc_z;
}


    
    
