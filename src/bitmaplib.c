/* bitmaplib.c
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
 * $Id: bitmaplib.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "paulslib.h"
#include "bitmaplib.h"


/*
   Create a bitmap structure
*/
BITMAP4 *Create_Bitmap(int nx,int ny)
{
   return((BITMAP4 *)malloc(nx*ny*sizeof(BITMAP4)));
}

/*
   Destroy the bitmap structure
*/
void Destroy_Bitmap(BITMAP4 *bm)
{
   free(bm);
}

/*
	Compare two pixels
*/
int Same_BitmapPixel(BITMAP4 p1,BITMAP4 p2)
{
	if (p1.r != p2.r) return(FALSE);
   if (p1.g != p2.g) return(FALSE);
   if (p1.b != p2.b) return(FALSE);
   if (p1.a != p2.a) return(FALSE);
	return(TRUE);
}

/*
   Write a bitmap to a file
	The format is as follows
     1 == tga 
    11 == tga with alpha
    12 == compressed tga
    13 == compressed tga with alpha
     2 == ppm
	  3 == rgb
	  4 == raw grey scale
     5 == tiff
	  6 == EPS colour (Encapsulated PostScript)
	  7 == EPS black and white
 	  8 == raw
     9 == BMP
	A negative format indicates a vertical flip
*/
void Write_Bitmap(FILE *fptr,BITMAP4 *bm,int nx,int ny,int format)
{
   int i,j,offset;
	long index,rowindex;
	int linelength = 0,size;
	char buffer[1024];

	/* Write the header */
	switch (ABS(format)) {
	case 1:
	case 11:
	case 12:
	case 13:
		putc(0,fptr);  /* Length of ID */
		putc(0,fptr);  /* No colour map */
		if (ABS(format) == 12 || ABS(format) == 13) 
			putc(10,fptr); /* compressed RGB */
		else
			putc(2,fptr); /* uncompressed RGB  */ 
		putc(0,fptr); /* Index of colour map entry */
		putc(0,fptr);
		putc(0,fptr); /* Colour map length */
		putc(0,fptr);
		putc(0,fptr); /* Colour map size */
		putc(0,fptr); /* X origin */
		putc(0,fptr);
		putc(0,fptr); /* Y origin */
		putc(0,fptr);
   	putc((nx & 0x00ff),fptr); /* X width */
   	putc((nx & 0xff00) / 256,fptr);
   	putc((ny & 0x00ff),fptr); /* Y width */
   	putc((ny & 0xff00) / 256,fptr);
		if (ABS(format) == 11 || ABS(format) == 13) {
         putc(32,fptr);                      /* 32 bit bitmap     */
			putc(0x08,fptr);
		} else {
			putc(24,fptr);                 		/* 24 bit bitmap 		*/
			putc(0x00,fptr);
		}
		break;
	case 2:
		fprintf(fptr,"P6\n%d %d\n255\n",nx,ny);
		break;
	case 3:
		putc(0x01,fptr);
		putc(0xda,fptr);
		putc(0x00,fptr);
		putc(0x01,fptr);
		putc(0x00,fptr);
		putc(0x03,fptr);
		putc((nx & 0xFF00) / 256,fptr);
		putc((nx & 0x00FF),fptr);
		putc((ny & 0xFF00) / 256,fptr);
      putc((ny & 0x00FF),fptr);
		BM_WriteHexString(fptr,"000300000000000000ff00000000");
		fprintf(fptr,"WriteBitmap, pdb");
      putc(0x00,fptr);
      putc(0x00,fptr);
      putc(0x00,fptr);
      putc(0x00,fptr);
      putc(0x00,fptr);
      putc(0x00,fptr);
      putc(0x00,fptr);
      putc(0x00,fptr);
		break;
	case 4:
		break;
	case 5:
		BM_WriteHexString(fptr,"4d4d002a");	/* Little endian & TIFF identifier */
		offset = nx * ny * 3 + 8;
		BM_WriteLongInt(fptr,buffer,offset);
		break;
	case 6:
		fprintf(fptr,"%%!PS-Adobe-3.0 EPSF-3.0\n");
		fprintf(fptr,"%%%%Creator: Created from bitmaplib by Paul Bourke\n");
		fprintf(fptr,"%%%%BoundingBox: %d %d %d %d\n",0,0,nx,ny);
		fprintf(fptr,"%%%%LanguageLevel: 2\n");
		fprintf(fptr,"%%%%Pages: 1\n");
		fprintf(fptr,"%%%%DocumentData: Clean7Bit\n");
		fprintf(fptr,"%d %d scale\n",nx,ny);
		fprintf(fptr,"%d %d 8 [%d 0 0 -%d 0 %d]\n",nx,ny,nx,ny,ny);
      fprintf(fptr,"{currentfile 3 %d mul string readhexstring pop} bind\n",nx);
      fprintf(fptr,"false 3 colorimage\n");
		break;
   case 7:
      fprintf(fptr,"%%!PS-Adobe-3.0 EPSF-3.0\n");
      fprintf(fptr,"%%%%Creator: Created from bitmaplib by Paul Bourke\n");
      fprintf(fptr,"%%%%BoundingBox: %d %d %d %d\n",0,0,nx,ny);
      fprintf(fptr,"%%%%LanguageLevel: 2\n");
      fprintf(fptr,"%%%%Pages: 1\n");
      fprintf(fptr,"%%%%DocumentData: Clean7Bit\n");
      fprintf(fptr,"%d %d scale\n",nx,ny);
      fprintf(fptr,"%d %d 8 [%d 0 0 -%d 0 %d]\n",nx,ny,nx,ny,ny);
      fprintf(fptr,"{currentfile %d string readhexstring pop} bind\n",nx);
      fprintf(fptr,"false 1 colorimage\n");
      break;
	case 8:
		break;
	case 9:
		/* Header 10 bytes */
		putc('B',fptr);
		putc('M',fptr);
		size = nx * ny * 3 + 14 + 40;
		putc((size) % 256,fptr);
		putc((size / 256) % 256,fptr);
		putc((size / 65536) % 256,fptr);
		putc((size / 16777216),fptr);
		putc(0,fptr); putc(0,fptr); 
		putc(0,fptr); putc(0,fptr);
		/* Offset to image data */
		putc(14+40,fptr); putc(0,fptr); putc(0,fptr); putc(0,fptr); 
		/* Information header 40 bytes */
		putc(0,fptr); putc(0,fptr); putc(0,fptr); putc(0,fptr); 
      putc((nx) % 256,fptr);
      putc((nx / 256) % 256,fptr);
      putc((nx / 65536) % 256,fptr);
      putc((nx / 16777216),fptr);
      putc((ny) % 256,fptr);
      putc((ny / 256) % 256,fptr);
      putc((ny / 65536) % 256,fptr);
      putc((ny / 16777216),fptr);
		putc(1,fptr); putc(0,fptr); /* One plane */
		putc(24,fptr); putc(0,fptr); /* 24 bits */
		/* Compression type == 0 */
		putc(0,fptr); putc(0,fptr); putc(0,fptr); putc(0,fptr); 
		size = nx * ny * 3;
      putc((size) % 256,fptr);
      putc((size / 256) % 256,fptr);
      putc((size / 65536) % 256,fptr);
      putc((size / 16777216),fptr);
      putc(1,fptr); putc(0,fptr); putc(0,fptr); putc(0,fptr); 
      putc(1,fptr); putc(0,fptr); putc(0,fptr); putc(0,fptr); 
      putc(0,fptr); putc(0,fptr); putc(0,fptr); putc(0,fptr); /* No palette */
      putc(0,fptr); putc(0,fptr); putc(0,fptr); putc(0,fptr); 
		break;
	}

	/* Write the binary data */
   for (j=0;j<ny;j++) {
		if (format > 0)
			rowindex = j * nx;
		else
			rowindex = (ny - 1 - j) * nx;
		switch (ABS(format)) {
	   case 12:
         WriteTGACompressedRow(fptr,&(bm[rowindex]),nx,3);
         break;
      case 13:
         WriteTGACompressedRow(fptr,&(bm[rowindex]),nx,4);
         break;
		}	
      for (i=0;i<nx;i++) {
			if (format > 0) 
         	index = rowindex + i;
			else
				index = rowindex + i;
			switch (ABS(format)) {
			case 1:
			case 11:
            putc(bm[index].b,fptr);
            putc(bm[index].g,fptr);
            putc(bm[index].r,fptr);
				if (ABS(format) == 11)
					putc(bm[index].a,fptr);
            break;
			case 2:
			case 3:
			case 5:
			case 8:
			case 9:
            putc(bm[index].r,fptr);
            putc(bm[index].g,fptr);
            putc(bm[index].b,fptr);
				break;
			case 4:
				putc((bm[index].r+bm[index].g+bm[index].b)/3,fptr);
				break;
			case 6:
				fprintf(fptr,"%02x%02x%02x",bm[index].r,bm[index].g,bm[index].b);
				linelength += 6;
				if (linelength >= 72 || linelength >= nx) {
					fprintf(fptr,"\n");
					linelength = 0;
				}	
				break;
         case 7:
            fprintf(fptr,"%02x",(bm[index].r+bm[index].g+bm[index].b)/3);
            linelength += 2;
            if (linelength >= 72 || linelength >= nx) {
               fprintf(fptr,"\n");
               linelength = 0;
            } 
            break;
			}
      }
   }

	/* Write the footer */
	switch (ABS(format)) {
	case 1:
	case 11:
	case 12:
	case 13:
	case 2:
	case 3:
	case 4:
		break;
	case 5:
		putc(0x00,fptr); /* The number of directory entries (14) */
      putc(0x0e,fptr);

		/* Width tag, short int */
		BM_WriteHexString(fptr,"0100000300000001");
		putc((nx & 0xff00) / 256,fptr);		/* Image width */
		putc((nx & 0x00ff),fptr);
		putc(0x00,fptr);
		putc(0x00,fptr);

		/* Height tag, short int */
		BM_WriteHexString(fptr,"0101000300000001");
      putc((ny & 0xff00) / 256,fptr);    /* Image height */
      putc((ny & 0x00ff),fptr);
		putc(0x00,fptr);
		putc(0x00,fptr);

		/* bits per sample tag, short int */
		BM_WriteHexString(fptr,"0102000300000003");
		offset = nx * ny * 3 + 182;
		BM_WriteLongInt(fptr,buffer,offset);

		/* Compression flag, short int */
		BM_WriteHexString(fptr,"010300030000000100010000");

		/* Photometric interpolation tag, short int */
		BM_WriteHexString(fptr,"010600030000000100020000");

		/* Strip offset tag, long int */
		BM_WriteHexString(fptr,"011100040000000100000008");

		/* Orientation flag, short int */
		BM_WriteHexString(fptr,"011200030000000100010000");

		/* Sample per pixel tag, short int */
		BM_WriteHexString(fptr,"011500030000000100030000");

		/* Rows per strip tag, short int */
		BM_WriteHexString(fptr,"0116000300000001");
      putc((ny & 0xff00) / 256,fptr); 
      putc((ny & 0x00ff),fptr);
		putc(0x00,fptr);
		putc(0x00,fptr);

		/* Strip byte count flag, long int */
		BM_WriteHexString(fptr,"0117000400000001");
      offset = nx * ny * 3;
		BM_WriteLongInt(fptr,buffer,offset);

		/* Minimum sample value flag, short int */
		BM_WriteHexString(fptr,"0118000300000003");
      offset = nx * ny * 3 + 188;
		BM_WriteLongInt(fptr,buffer,offset);

		/* Maximum sample value tag, short int */
		BM_WriteHexString(fptr,"0119000300000003");
      offset = nx * ny * 3 + 194;
		BM_WriteLongInt(fptr,buffer,offset);

		/* Planar configuration tag, short int */
		BM_WriteHexString(fptr,"011c00030000000100010000");

		/* Sample format tag, short int */
		BM_WriteHexString(fptr,"0153000300000003");
      offset = nx * ny * 3 + 200;
		BM_WriteLongInt(fptr,buffer,offset);

		/* End of the directory entry */
		BM_WriteHexString(fptr,"00000000");

		/* Bits for each colour channel */
		BM_WriteHexString(fptr,"000800080008");

		/* Minimum value for each component */
		BM_WriteHexString(fptr,"000000000000");

		/* Maximum value per channel */
		BM_WriteHexString(fptr,"00ff00ff00ff");

		/* Samples per pixel for each channel */
		BM_WriteHexString(fptr,"000100010001");

		break;
	case 6:
	case 7:
		fprintf(fptr,"\n%%%%EOF\n");
		break;
   case 8:
   case 9:
		break;
	}
}

/*
	Write a compressed TGA row
	Depth is either 3 or 4
*/
void WriteTGACompressedRow(FILE *fptr,BITMAP4 *bm,int width,int depth)
{
	int i;
	int counter = 1;
	int pixelstart = 0;
	int packettype = 0;
	int readytowrite = FALSE;
	BITMAP4 currentpixel,nextpixel = {0,0,0,0};

	currentpixel = bm[0];
	for (;;) {
      if (pixelstart+counter >= width)  // Added April to fix strange bug
         readytowrite = TRUE;
      else
         nextpixel = bm[pixelstart+counter];

		if (!readytowrite) {
			if (Same_BitmapPixel(currentpixel,nextpixel)) {
				if (packettype == 0) {
					counter++;
					if (counter >= 128 || (pixelstart + counter) >= width) 
						readytowrite = TRUE;
				} else {
					counter--;
					readytowrite = TRUE;
				}
			} else {
				if (packettype == 1 || counter <= 1) {
					packettype = 1;
					currentpixel = nextpixel;
					counter++;
					if (counter >= 128 || (pixelstart + counter) >= width)
						readytowrite = TRUE;
				} else {
					readytowrite = TRUE;
				}
			}
		}

		if (readytowrite) {
			if (pixelstart + counter > width)
				counter = width - pixelstart;
			if (packettype == 0) {
				putc(((counter-1) | 0x80),fptr);
            putc(currentpixel.b,fptr);
            putc(currentpixel.g,fptr);
            putc(currentpixel.r,fptr);
            if (depth == 4)
               putc(currentpixel.a,fptr);
				currentpixel = nextpixel;
			} else {
				putc(counter-1,fptr);
				for (i=0;i<counter;i++) {
					putc(bm[pixelstart+i].b,fptr);
            	putc(bm[pixelstart+i].g,fptr);
            	putc(bm[pixelstart+i].r,fptr);
            	if (depth == 4)
               	putc(bm[pixelstart+i].a,fptr);
				}
			}
			if ((pixelstart = pixelstart + counter) >= width)
            break; /* From for (;;) */
			readytowrite = FALSE;
			packettype = 0;
			counter = 1;
		}
	}
}

void BM_WriteLongInt(FILE *fptr,char *s,long n)
{
	int i;

	s[0] = (n & 0xff000000) / 16777216;
	s[1] = (n & 0x00ff0000) / 65536;
	s[2] = (n & 0x0000ff00) / 256;
	s[3] = (n & 0x000000ff);

	for (i=0;i<4;i++)
		putc(s[i],fptr);
}

void BM_WriteHexString(FILE *fptr,char *s)
{
	unsigned int i;
	int c;
	char hex[3];

	for (i=0;i<strlen(s);i+=2) {
      hex[0] = s[i];
      hex[1] = s[i+1];
      hex[2] = '\0';
      sscanf(hex,"%X",&c);
		putc(c,fptr);
	}
}

/*
   Clear the bitmap to a particular colour
*/
void Erase_Bitmap(BITMAP4 *bm, int nx, int ny, BITMAP4 col)
{
   int i,j;
	long index;

   for (i=0;i<nx;i++) {
      for (j=0;j<ny;j++) {
         index = j * nx + i;
         bm[index] = col;
      }
   }
}

/*
	Scale an image using bicubic interpolation
*/
void BiCubicScale(
   BITMAP4 *bm_in,int nx,int ny,
   BITMAP4 *bm_out,int nnx,int nny)
{
   int i_out,j_out,i_in,j_in,ii,jj;
   int n,m;
	long index;
   double cx,cy,dx,dy,weight;
   double red,green,blue,alpha;
   BITMAP4 col;
 
   for (i_out=0;i_out<nnx;i_out++) {
      for (j_out=0;j_out<nny;j_out++) {
         i_in = (i_out * nx) / nnx;
         j_in = (j_out * ny) / nny;
         cx = i_out * nx / (double)nnx;
         cy = j_out * ny / (double)nny;
         dx = cx - i_in;
         dy = cy - j_in;
         red   = 0;
         green = 0;
         blue  = 0;
			alpha = 0;
         for (m=-1;m<=2;m++) {
            for (n=-1;n<=2;n++) {
               ii = i_in + m;
               jj = j_in + n;
               if (ii < 0)   ii = 0;
               if (ii >= nx) ii = nx-1;
               if (jj < 0)   jj = 0;
               if (jj >= ny) jj = ny-1;
               index = jj * nx + ii;
               weight = BiCubicR(m-dx) * BiCubicR(n-dy);
					// weight = BiCubicR(m-dx) * BiCubicR(dy-n);
               red   += weight * bm_in[index].r;
               green += weight * bm_in[index].g;
               blue  += weight * bm_in[index].b;
					alpha += weight * bm_in[index].a;
            }
         }
         col.r = (int)red;
         col.g = (int)green;
         col.b = (int)blue;
			col.a = (int)alpha;
         bm_out[j_out * nnx + i_out] = col;
      }
   }
}

double BiCubicR(double x)
{
   double xp2,xp1,xm1;
   double r = 0;

   xp2 = x + 2;
   xp1 = x + 1;
   xm1 = x - 1;

   if (xp2 > 0)
      r += xp2 * xp2 * xp2;
   if (xp1 > 0)
      r -= 4 * xp1 * xp1 * xp1;
   if (x > 0)
      r += 6 * x * x * x;
   if (xm1 > 0)
      r -= 4 * xm1 * xm1 * xm1;

   return(r / 6.0);
}

/*
	Scale a bitmap
	Apply a gaussian radial average if r > 0
	r is in units of the input image
*/
void GaussianScale(
	BITMAP4 *bm_in,int nx,int ny,
	BITMAP4 *bm_out,int nnx,int nny,double r)
{
	int i,j,ii,jj,ci,cj;
	long index;
	double x,y,cx,cy,red,green,blue,alpha,dist2,r2,weight,sum;
	BITMAP4 col,black = {0,0,0,255};

	r2 = r*r;

	for (i=0;i<nnx;i++) {
		for (j=0;j<nny;j++) {
			col = black;
			if (r2 <= 0) {
         	ci = (i * nx) / nnx;
         	cj = (j * ny) / nny;
				index = cj * nx + ci;
				col = bm_in[index];
			} else {
            cx = i * nx / (double)nnx;
            cy = j * ny / (double)nny;
				red   = 0;
				green = 0;
				blue  = 0;
				alpha = 0;
				sum = 0;
				for (x=cx-4*r;x<=cx+4*r+0.01;x++) {
					for (y=cy-4*r;y<=cy+4*r+0.01;y++) {
						ii = (int)x;
						jj = (int)y;
						if (ii < 0)
							ii = 0;
						if (ii >= nx)
							ii = nx-1;;
						if (jj < 0)
							jj = 0;
						if (jj >= ny) 
							jj = ny-1;
						dist2 = (cx-x)*(cx-x) + (cy-y)*(cy-y);
						weight = exp(-0.5*dist2/r2) / (r2*TWOPI);
						index = jj * nx + ii;
						red   += weight * bm_in[index].r;
						green += weight * bm_in[index].g;
						blue  += weight * bm_in[index].b;
						alpha += weight * bm_in[index].a;
						sum += weight;
					}
				}
				col.r = (int)red;
            col.g = (int)green;
            col.b = (int)blue;
				col.a = (int)alpha;
			}
			bm_out[j * nnx + i] = col;
		}
	}
}

/*
   Turn on a pixel of a bitmap
*/
int Draw_Pixel(BITMAP4 *bm, int nx, int ny, int x, int y, BITMAP4 col)
{
   long index;

   if (x < 0 || y < 0 || x >= nx || y >= ny)
      return(FALSE);
   index = y * nx + x;
   bm[index] = col;
	return(TRUE);
}

/*
   Return the value of a pixel
*/
BITMAP4 Get_Pixel(BITMAP4 *bm, int nx, int ny, int x, int y)
{
   long index;
	BITMAP4 black = {0,0,0,255};

	if (x < 0 || y < 0 || x >= nx || y >= ny)
		return(black);
   index = y * nx + x;
   return(bm[index]);
}

/*
   Draw a line from (x1,y1) to (x2,y2)
   Use colour col
*/
void Draw_Line(BITMAP4 *bm,int nx,int ny,int x1,int y1,int x2,int y2,BITMAP4 col)
{
   int i,j;
	long index;
   double mu,dx,dy,dxy;

   dx = x2 - x1;
   dy = y2 - y1;
   dxy = sqrt(dx*dx + dy*dy);
	if (dxy <= 0) {
		Draw_Pixel(bm,nx,ny,x1,y1,col); 
		return;
	}
   for (mu=0;mu<=2*dxy;mu++) {
      i = (int)(x1 + 0.5 * mu * dx / dxy);
      j = (int)(y1 + 0.5 * mu * dy / dxy);
		if (i < 0 || j < 0 || i >= nx || j >= ny)
			continue;
      index = j * nx + i;
      bm[index] = col;
   }
}

/*
	Scale a RGB value, dealing with clipping issues
*/
BITMAP4 Scale_Pixel(BITMAP4 pixelin,double scale)
{
	BITMAP4 pixelout;
	double r,g,b,a=0;

	r = pixelin.r * scale;
   g = pixelin.g * scale;
   b = pixelin.b * scale;
	a = pixelin.a * scale;
	
	if (r < 000) r = 0;
	if (r > 255) r = 255;
   if (g < 000) g = 0;
   if (g > 255) g = 255;
   if (b < 000) b = 0;
   if (b > 255) b = 255;
   if (a < 000) a = 0;
   if (a > 255) a = 255;

	pixelout.r = (int)r;
	pixelout.g = (int)g;
	pixelout.b = (int)b;
	pixelout.a = (int)a;

	return(pixelout);
}

/*
	Flip an image about an axis
	mode == 0 for horizontal
	mode == 1 for vertical
	This library assumes the (0,0) coordinate is top left
*/
void Flip_Bitmap(BITMAP4 *image,int width,int height,int mode)
{
	int i,j;
	long index1,index2;
	BITMAP4 p;

	switch (mode) {
	case 0:
		for (j=0;j<height/2;j++) {
			for (i=0;i<width;i++) {
				index1 = j * width + i;
				index2 = (height-1-j) * width + i;
				p = image[index1];
				image[index1] = image[index2];
				image[index2] = p;
			}
		}
		break;
	case 1:
      for (j=0;j<height;j++) {
         for (i=0;i<width/2;i++) {
            index1 = j * width + i;
            index2 = j * width + (width-1-i);
            p = image[index1];
            image[index1] = image[index2];
            image[index2] = p;
         }
      }
		break;
	}
}

/*
   Get the size and depth of a TGA file
*/
void TGA_Info(FILE *fptr,int *width,int *height,int *depth)
{
	int lo,hi;
   TGAHEADER header;

   header.idlength = fgetc(fptr);
   header.colourmaptype = fgetc(fptr);
   header.datatypecode = fgetc(fptr);
   fread(&header.colourmaporigin,2,1,fptr);
   fread(&header.colourmaplength,2,1,fptr);
   header.colourmapdepth = fgetc(fptr);
   fread(&header.x_origin,2,1,fptr);
   fread(&header.y_origin,2,1,fptr);
   lo = fgetc(fptr);
   hi = fgetc(fptr);
   header.width = hi*256 + lo;
   lo = fgetc(fptr);
   hi = fgetc(fptr);
   header.height = hi*256 + lo;
   header.bitsperpixel = fgetc(fptr);
	header.imagedescriptor = fgetc(fptr);

	*width  = header.width;
	*height = header.height;
	*depth  = header.bitsperpixel;

	rewind(fptr);
}

/*
	Read the TGA image data
	Return 0 on success
	Error codes
		1 - Failed to get legal colour type code
		2 - Failed to get legal bits per pixel
		3 - Failed to get legal colour map type
		4 - Failed to read colour data
		5 - Failed to read colour table
*/
int TGA_Read(FILE *fptr,BITMAP4 *image,int *width,int *height)
{
	int n=0,i,j;
	int lo,hi,index;
	int bytes2read,skipover = 0;  
	TGAHEADER header;
	unsigned char p[5];
	BITMAP4 *ctable = NULL;

   /* Read the header */
   header.idlength = fgetc(fptr);
   header.colourmaptype = fgetc(fptr);
   header.datatypecode = fgetc(fptr);
   fread(&header.colourmaporigin,2,1,fptr);
   fread(&header.colourmaplength,2,1,fptr);
   header.colourmapdepth = fgetc(fptr);
   fread(&header.x_origin,2,1,fptr);
   fread(&header.y_origin,2,1,fptr);
	lo = fgetc(fptr);
	hi = fgetc(fptr);
	header.width = hi*256 + lo;
	*width = header.width;
   lo = fgetc(fptr);
   hi = fgetc(fptr);
	header.height = hi*256 + lo;
	*height = header.height;
   header.bitsperpixel = fgetc(fptr);
   header.imagedescriptor = fgetc(fptr);
	
	/* 
		Can only handle image type 1, 2, 3 and 10 
		 1 - index colour uncompressed
		 2 - rgb uncompressed
		10 - rgb rle comrpessed
		 3 - grey scale uncompressed
       9 - rle index colour (unsupported)
	*/ 
   if (header.datatypecode != 1 &&
		 header.datatypecode != 2 && 
		 header.datatypecode != 3 &&
		 header.datatypecode != 10) {
      return(1);
   }

	/* Can only handle pixel depths of 8, 16, 24, and 32 */
   if (header.bitsperpixel != 8 &&
		 header.bitsperpixel != 16 &&
       header.bitsperpixel != 24 && 
		 header.bitsperpixel != 32) {
      return(2);
   }

	/* 
		Can only handle colour map types of 0 and 1
		Ignore the colour map case (1) for RGB images!
	*/
   if (header.colourmaptype != 0 && header.colourmaptype != 1) {
      return(3);
   }

	/* Read the colour index table */
	if (header.datatypecode == 1) {
		ctable = (BITMAP4 *)malloc(header.colourmaplength*sizeof(BITMAP4));
		bytes2read = header.colourmapdepth / 8;
		for (i=0;i<header.colourmaplength;i++) {
         if ((int)fread(p,1,bytes2read,fptr) != bytes2read) 
            return(5);
         TGA_MergeBytes(&(ctable[i]),p,bytes2read);
			//fprintf(stderr,"%d %d %d %d\n",i,ctable[i].r,ctable[i].g,ctable[i].b);
		}
	} 

   /* Go to the start of the image data */
   skipover = 18;
   skipover += header.idlength;
   skipover += header.colourmaptype * header.colourmaplength;
   fseek(fptr,skipover,SEEK_SET);

   /* Read the image */
   bytes2read = header.bitsperpixel / 8;
   while (n < header.width * header.height) {
		if (header.datatypecode == 1) {                     /* Indexed uncompressed */
         if ((index = fgetc(fptr)) == EOF) {
            return(4);
         }
			if (index < 0) 
				index = 0;
			if (index >= header.colourmaplength)
				index = header.colourmaplength-1;
			image[n] = ctable[index];
			n++;
      } else if (header.datatypecode == 2) {              /* RGB Uncompressed */
         if ((int)fread(p,1,bytes2read,fptr) != bytes2read) {
            return(4);
			}
         TGA_MergeBytes(&(image[n]),p,bytes2read);
         n++;
      } else if (header.datatypecode == 3) {              /* Grey Uncompressed */
         if ((int)fread(p,1,bytes2read,fptr) != bytes2read) {
            return(4);
         }
         TGA_MergeBytes(&(image[n]),p,bytes2read);
         n++;
      } else if (header.datatypecode == 10) {             /* RGB Compressed */
         if ((int)fread(p,1,bytes2read+1,fptr) != bytes2read+1) 
            return(4);
         j = p[0] & 0x7f;
         TGA_MergeBytes(&(image[n]),&(p[1]),bytes2read);
         n++;
         if (p[0] & 0x80) {         /* RLE chunk */
            for (i=0;i<j;i++) {
               TGA_MergeBytes(&(image[n]),&(p[1]),bytes2read);
               n++;
            }
         } else {                   /* Normal chunk */
            for (i=0;i<j;i++) {
               if ((int)fread(p,1,bytes2read,fptr) != bytes2read) 
                  return(6);
               TGA_MergeBytes(&(image[n]),p,bytes2read);
               n++;
            }
         }
      }
   }

	/* Flip the image ? */
	if ((header.imagedescriptor & 0x20) == 32) 
		Flip_Bitmap(image,header.width,header.height,0);

	return(0);
}

void TGA_MergeBytes(BITMAP4 *pixel,unsigned char *p,int bytes)
{
   if (bytes == 4) {
      pixel->r = p[2];
      pixel->g = p[1];
      pixel->b = p[0];
      pixel->a = p[3];
   } else if (bytes == 3) {
      pixel->r = p[2];
      pixel->g = p[1];
      pixel->b = p[0];
      pixel->a = 255; 
   } else if (bytes == 2) {
      pixel->r = (p[1] & 0x7c) << 1;
      pixel->g = ((p[1] & 0x03) << 6) | ((p[0] & 0xe0) >> 2);
      pixel->b = (p[0] & 0x1f) << 3;
      pixel->a = (p[1] & 0x80);
   } else if (bytes == 1) {
      pixel->r = p[0];
      pixel->g = p[0];
      pixel->b = p[0];
      pixel->a = 0;
	}
}

/*
   Read a TGA texture file and return a pointer to the pixelmap
   and the size of the image.
   Always return a texture unless the memory allocation fails.
   Return random noise if the file reading fails.
	Required bitmaplib tga functions.
*/
BITMAP4 *ReadTGATexture(char *fname,int *w,int *h)
{
   int i,width,height,depth;
   FILE *fptr;
   BITMAP4 *ptr;

   /* Allocate memory for the texture */
   *w = 64;
   *h = 64;
   if ((ptr = (BITMAP4 *)malloc((*w)*(*h)*sizeof(BITMAP4))) == NULL) {
      fprintf(stderr,"Failed to allocate memory for texture \"%s\"\n",fname);
      exit(-1);
   }

   /* Start off with a random texture, totally opaque */
   for (i=0;i<(*w)*(*h);i++) {
      ptr[i].r = rand() % 255;
      ptr[i].g = rand() % 255;
      ptr[i].b = rand() % 255;
      ptr[i].a = 255;
   }

   /* Try to open the texture file */
   if ((fptr = fopen(fname,"rb")) == NULL) {
      fprintf(stderr,"Failed to open texture file \"%s\"\n",fname);
      return(ptr);
   }

   /* Read the header */
   TGA_Info(fptr,&width,&height,&depth);
   if (width < 4 || height < 4 || width > 30000 || height > 30000) {
      fprintf(stderr,"Failed to read TGA header\n");
      fclose(fptr);
      return(ptr);
   }

   /* Allocate memory for the texture */
   if ((ptr = (BITMAP4 *)realloc(ptr,width*height*sizeof(BITMAP4))) == NULL) {
      fprintf(stderr,"Failed to allocate memory for texture \"%s\"\n",fname);
		fclose(fptr);
      return(ptr);
   }
   *w = width;
   *h = height;

   /* Start off with a random texture, totally opaque */
   for (i=0;i<(*w)*(*h);i++) {
      ptr[i].r = rand() % 255;
      ptr[i].g = rand() % 255;
      ptr[i].b = rand() % 255;
      ptr[i].a = 255;
   }

   /* Actually read the texture */
   int fl = TGA_Read(fptr,ptr,&width,&height);
   if (fl != 0) {
     fprintf(stderr,"TGA file read error %d\n", fl);
   }

   fclose(fptr);
   return(ptr);
}
BITMAP4 YUV_to_Bitmap(int y,int u,int v)
{  
   int r,g,b; 
   BITMAP4 bm = {0,0,0,0};
   
   // u and v are +-0.5
   u -= 128;
   v -= 128;
   
   r = y + 1.370705 * v;
   g = y - 0.698001 * v - 0.337633 * u;
   b = y + 1.732446 * u;

   if (r < 0) r = 0;
   if (g < 0) g = 0;
   if (b < 0) b = 0;
   if (r > 255) r = 255;
   if (g > 255) g = 255;
   if (b > 255) b = 255;
   bm.r = r;
   bm.g = g;
   bm.b = b;
   bm.a = 0;

   return(bm);
}
  
