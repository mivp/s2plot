/* s2freetype.c
 *
 * Copyright 2006-2012 David G. Barnes, Paul Bourke, Christopher Fluke
 *
 * This file is part of S2PLOT.
 *
 * S2PLOT is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * S2PLOT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with S2PLOT.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * We would appreciate it if research outcomes using S2PLOT would
 * provide the following acknowledgement:
 *
 * "Three-dimensional visualisation was conducted with the S2PLOT
 * progamming library"
 *
 * and a reference to
 *
 * D.G.Barnes, C.J.Fluke, P.D.Bourke & O.T.Parry, 2006, Publications
 * of the Astronomical Society of Australia, 23(2), 82-93.
 *
 * $Id: s2freetype.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "s2plot.h"

/* Based on FreeType2 Tutorial code: 
 * http://www.freetype.org/freetype2/docs/tutorial/example1.c
 */

void draw_bitmap( unsigned char **image,
	     int imagewidth, int imageheight,
	     FT_Bitmap*  bitmap,
             FT_Int      x,
             FT_Int      y) {
  FT_Int  i, j, p, q;
  FT_Int  x_max = x + bitmap->width;
  FT_Int  y_max = y + bitmap->rows;
  for ( i = x, p = 0; i < x_max; i++, p++ ) {
    for ( j = y, q = 0; j < y_max; j++, q++ ) {
      if ( i >= imagewidth || j >= imageheight )
        continue;
      image[j][i] |= bitmap->buffer[q * bitmap->width + p];
    }
  }
}


/* fontfilename = eg "/System/Library/Arial"
 * text = eg. "Hello World!"
 * fontsizepx = font size in PIXELS not points
 * border = ~number of pixels around text
 */
unsigned int ss2ftt(char *fontfilename, char *text, int fontsizepx,
		    int border) {
  FT_Library    library;
  FT_Face       face;
  FT_GlyphSlot  slot;
  FT_Matrix     matrix;                 /* transformation matrix */
  FT_Vector     pen;                    /* untransformed origin  */
  FT_Error      error;

  int i,j, n, loop;
  int tmp;
  int bbot = 0, btop = 0;
  
  int num_chars = strlen( text );
  double angle = ( 0.0 / 360.0 ) * 3.14159 * 2.0;      /* use 0 degrees     */

  unsigned char **image = NULL;
  int WIDTH = 0, HEIGHT = 0;

  error = FT_Init_FreeType( &library );              /* initialize library */
  if (error) {
    fprintf(stderr, "Failed to initialise freetype library!\n");
    return ss2ct(16, 16);
  }

  error = FT_New_Face( library, fontfilename, 0, &face ); /* create face object */
  if (error) {
    fprintf(stderr, "Failed to load font \"%s\"\n", fontfilename);
    return ss2ct(16, 16);
  }
  
  /* use characters that are 24 x 24 pixels */
  error = FT_Set_Pixel_Sizes(face, fontsizepx, fontsizepx);
  if (error) {
    fprintf(stderr, "Failed to set character size\n");
    return ss2ct(16, 16);
  }

  slot = face->glyph;

  /* set up matrix */
  matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
  matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
  matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
  matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

  for (loop = 0; loop < 2; loop++) {
    // first loop: figure out dimensions without drawing
    // second loop: draw to bitmap
    if (!loop) {
      pen.x = 0;
      pen.y = 0;
      bbot = btop = 0;
    } else {
      // start at (border, fontsizepx + border) relative to TLC
      pen.x = border * 64;
      pen.y = (border + bbot) * 64;
    }

    for ( n = 0; n < num_chars; n++ ) {
      /* set transformation */
      FT_Set_Transform( face, &matrix, &pen );
      
      /* load glyph image into the slot (erase previous one) */
      error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
      if ( error )
	continue;                 /* ignore errors */

      if (!loop) {
	/* see if we have a new max height... */
	tmp = -slot->bitmap_top;
	if (tmp < btop) {
	  btop = tmp;
	}
	tmp = -slot->bitmap_top + slot->bitmap.rows;
	if (tmp > bbot) {
	  bbot = tmp;
	}
      } else {
	/* draw to our target surface (convert position) */
	draw_bitmap( image, WIDTH, HEIGHT, &slot->bitmap,
		     slot->bitmap_left,
		     HEIGHT - slot->bitmap_top );
      }

      /* increment pen position */
      pen.x += slot->advance.x;
      pen.y += slot->advance.y;
    }

    if (!loop) {
      WIDTH = pen.x / 64 + 2 * border;
      HEIGHT = (bbot - btop) + 2 * border;
      image = (unsigned char **)malloc(HEIGHT * sizeof(unsigned char *));
      for (j = 0; j < HEIGHT; j++) {
	image[j] = (unsigned char *)malloc(WIDTH * sizeof(unsigned char));
	for (i = 0; i < WIDTH; i++) {
	  image[j][i] = 0;
	}
      }
    }
  }

  FT_Done_Face    ( face );
  FT_Done_FreeType( library );

  /* ok at this point image contains our bitmap ... turn it into a 
   * texture */
  unsigned int texid = ss2ct(WIDTH, HEIGHT);
  unsigned char *bits = ss2gt(texid, NULL, NULL);
  int idx;
  for (j = 0; j < HEIGHT; j++) {
    for (i = 0; i < WIDTH; i++) {
      // HEIGHT-1-j to flip texture vertically
      idx = ((HEIGHT-1-j) * WIDTH + i) * 4;
      bits[idx] = image[j][i];
      bits[idx+1] = image[j][i];
      bits[idx+2] = image[j][i];
      bits[idx+3] = image[j][i]; /* half transparent */
    }
  }
  ss2pt(texid);

  for (j = 0; j < HEIGHT; j++) {
    free(image[j]);
  }
  free(image);

  return texid;
}

/* Convert a FORTRAN string and length to a C string with null
 * termination.  It is the caller's responsibility to free the
 * returned pointer when it is finished with.
 */
char *_s2_ff2cstr(char *text, int textlen) {
  int l = textlen + 1;
  char *retval = (char *)calloc(l, sizeof(char));
  if (!retval) {
    return NULL;
  }
  strncpy(retval, text, l-1);
  retval[l-1] = '\0';
  return retval;
}

// fortran stub
unsigned int ss2ftt_(char *fontfilename, char *text, int *fontsizepx, 
		    int *border, long int fontlen, long int textlen) {
  char *font = _s2_ff2cstr(fontfilename, fontlen);
  char *tt = _s2_ff2cstr(text, textlen);
  unsigned int result = ss2ftt(font, tt, *fontsizepx, *border);
  free(tt);
  free(font);
  return result;
}

