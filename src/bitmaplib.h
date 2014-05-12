/* bitmaplib.h
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
 * $Id: bitmaplib.h 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#ifndef BITMAPLIB_H
#define BITMAPLIB_H

#if defined(__cplusplus) && !defined(S2_CPPBUILD)
extern "C" {
#endif
  
  typedef struct {
    unsigned char r,g,b,a;
  } BITMAP4;
  
  /* 18 bytes long */
  typedef struct {
    unsigned char  idlength;
    char  colourmaptype;
    char  datatypecode;
    short int colourmaporigin;
    short int colourmaplength;
    char  colourmapdepth;
    short int x_origin;
    short int y_origin;
    short int width;
    short int height;
    char  bitsperpixel;
    char  imagedescriptor;
  } TGAHEADER;
  
  BITMAP4 *Create_Bitmap(int,int);
  void Destroy_Bitmap(BITMAP4 *);
  void Write_Bitmap(FILE *,BITMAP4 *,int,int,int);
  void Erase_Bitmap(BITMAP4 *,int,int,BITMAP4);
  void GaussianScale(BITMAP4 *,int,int,BITMAP4 *,int,int,double);
  void BiCubicScale(BITMAP4 *,int,int,BITMAP4 *,int,int);
  double BiCubicR(double);
  int Draw_Pixel(BITMAP4 *,int,int,int,int,BITMAP4);
  BITMAP4 Get_Pixel(BITMAP4 *,int,int,int,int);
  void Draw_Line(BITMAP4 *,int,int,int,int,int,int,BITMAP4);
  BITMAP4 Scale_Pixel(BITMAP4,double);
  void Flip_Bitmap(BITMAP4 *,int,int,int);
  int Same_BitmapPixel(BITMAP4,BITMAP4);
  BITMAP4 YUV_to_Bitmap(int,int,int);
  
  void BM_WriteLongInt(FILE *,char *,long);
  void BM_WriteHexString(FILE *,char *);
  
  void TGA_Info(FILE *,int *,int *,int *);
  int TGA_Read(FILE *,BITMAP4 *,int *,int *);
  void TGA_MergeBytes(BITMAP4 *,unsigned char *,int);
  void WriteTGACompressedRow(FILE *,BITMAP4 *,int,int);

  BITMAP4 *ReadTGATexture(char *,int *,int *);
  
#if defined(__cplusplus) && !defined(S2_CPPBUILD)
} /* extern "C" { */
#endif

#endif /* BITMAPLIB_H */
