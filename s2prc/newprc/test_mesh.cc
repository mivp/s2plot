/************
*
*   This file is part of a tool for producing 3D content in the PRC format.
*   Copyright (C) 2008  Orest Shardt <shardtor (at) gmail dot com>
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include "oPRCFile.h"

#define PRINTJSNAMES \
  std::cout << file.lastgroupname << std::endl; \
  for(std::vector<std::string>::const_iterator it=file.lastgroupnames.begin(); it!=file.lastgroupnames.end(); it++) \
    std::cout << " " << *it << std::endl; \

using namespace std;

extern const double pi;

int main()
{
  // List of pictures used; keep track of memory allocated to free it in the end
  // shared pointers or garbage collector may be an alternative
  uint8_t *picture1 = NULL;
  uint8_t *picture2 = NULL;
  uint8_t *picture3 = NULL;
  uint8_t *picture4 = NULL;

  oPRCFile file("test_mesh.prc");

  const size_t N_COLOURS = 32;
  RGBAColour colours[N_COLOURS];
  for(size_t i = 0; i < N_COLOURS; ++i)
  {
    colours[i%N_COLOURS].R = 0.0;
    colours[i%N_COLOURS].G = (i%N_COLOURS)/static_cast<double>(N_COLOURS);
    colours[i%N_COLOURS].B = 0.95;
    colours[i%N_COLOURS].A = 0.75;
  }
  
  PRCmaterial materials[N_COLOURS];
  for(size_t i = 0; i < N_COLOURS; ++i)
  {
    materials[i%N_COLOURS].diffuse.R = 0.0;
    materials[i%N_COLOURS].diffuse.G = (i%N_COLOURS)/static_cast<double>(N_COLOURS);
    materials[i%N_COLOURS].diffuse.B = 0.95;
    materials[i%N_COLOURS].diffuse.A = 0.75;
    materials[i%N_COLOURS].specular.R = 0.01*0.0;
    materials[i%N_COLOURS].specular.G = 0.01*(i%N_COLOURS)/static_cast<double>(N_COLOURS);
    materials[i%N_COLOURS].specular.B = 0.01*0.95;
    materials[i%N_COLOURS].specular.A = 0.01*0.75;
    materials[i%N_COLOURS].emissive.R = 0.20*0.0;
    materials[i%N_COLOURS].emissive.G = 0.20*(i%N_COLOURS)/static_cast<double>(N_COLOURS);
    materials[i%N_COLOURS].emissive.B = 0.20*0.95;
    materials[i%N_COLOURS].emissive.A = 0.20*0.75;
    materials[i%N_COLOURS].ambient.R  = 0.05*0.0;
    materials[i%N_COLOURS].ambient.G  = 0.05*(i%N_COLOURS)/static_cast<double>(N_COLOURS);
    materials[i%N_COLOURS].ambient.B  = 0.05*0.95;
    materials[i%N_COLOURS].ambient.A  = 0.05*0.75;
    materials[i%N_COLOURS].alpha      = 0.75;
    materials[i%N_COLOURS].shininess  = 0.1;
  }
  
if(1) {
  const size_t NUMBER_OF_POINTS = 31;
  double points[NUMBER_OF_POINTS][3];
  for(size_t i = 0; i < NUMBER_OF_POINTS; ++i)
  {
    points[i][0] = 3.5*cos(3.0*i/NUMBER_OF_POINTS*2.0*pi);
    points[i][1] = 3.5*sin(3.0*i/NUMBER_OF_POINTS*2.0*pi);
    points[i][2] = 5.0*i/NUMBER_OF_POINTS-1.0;
  }
  const size_t NUMBER_OF_WIRES = 2;
  double shifted_points[NUMBER_OF_WIRES][NUMBER_OF_POINTS][3];
  for(size_t wire = 0; wire < NUMBER_OF_WIRES; ++wire)
    for(size_t point = 0; point < NUMBER_OF_POINTS; ++point)
      {
        shifted_points[wire][point][0] = points[point][0];
        shifted_points[wire][point][1] = points[point][1];
        shifted_points[wire][point][2] = points[point][2]+0.1*wire+0.1;
      }
  double knots[3+NUMBER_OF_POINTS+1];
  knots[0] = 1;
  for(size_t i = 1; i < 3+NUMBER_OF_POINTS; ++i)
  {
    knots[i] = (i+2)/3; // integer division is intentional
  }
  knots[3+NUMBER_OF_POINTS] = (3+NUMBER_OF_POINTS+1)/3;

  PRCoptions grpopt;
  grpopt.no_break = true;
  grpopt.do_break = false;
  grpopt.tess = true;
  if(1){
    double point1[3] = {11,0,0};
    double point2[3] = {12,0,0};
    double points[2][3] = {{9,0,0},{10,0,0}};
    file.begingroup("points",&grpopt);
    file.addPoint(point1, RGBAColour(1.0,0.0,0.0));
    file.addPoint(point2, RGBAColour(1.0,0.0,0.0));
    file.addPoints(2, points, RGBAColour(1.0,0.0,0.0,0.5),10);
    file.endgroup();
    PRINTJSNAMES
  }

  if(1){
    PRCoptions grpopt;
    grpopt.no_break = true;
    grpopt.do_break = false;
    grpopt.tess = true;
    grpopt.closed = true;

    double t[16];

    const size_t nP = 5;
    double P[nP][3] = {{0,0,0},{1,0,0},{1,1,0},{0,1,0},{0,2,0}};
    const size_t nI = 3;
    uint32_t PI[nI][3] = {{0,1,3},{1,2,3},{3,2,4}};
    const size_t nM = 2;
    uint32_t M[nM];
    M[0] = file.addMaterial(PRCmaterial(
      RGBAColour(0.0,0.0,0.18),
      RGBAColour(0.0,0.0,0.878431),
      RGBAColour(0.0,0.0,0.32),
      RGBAColour(0.0,0.0,0.072),
      1.0,0.1));
    M[1] = file.addMaterial(PRCmaterial(
      RGBAColour(0.18,0.0,0.0),
      RGBAColour(0.878431,0.0,0.0),
      RGBAColour(0.32,0.0,0.0),
      RGBAColour(0.072,0.0,0.0),
      0.5,0.1));
    uint32_t MI[nI] = {0,1,0};
    const size_t nN = 2;
    double N[nN][3] = {{0,0,1},{0,0,-1}};
    uint32_t NI[nI][3] = {{0,0,0},{0,0,0},{1,1,1}};

    const uint32_t nC = 3;
    RGBAColour C[nC];
    uint32_t CI[nI][3] = {{0,0,0},{1,1,1},{1,1,2}};

    PRCmaterial materialGreen(
      RGBAColour(0.0,0.18,0.0),
      RGBAColour(0.0,0.878431,0.0),
      RGBAColour(0.0,0.32,0.0),
      RGBAColour(0.0,0.072,0.0),
      1.0,0.1);

    if(1){
    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=-1;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    file.begingroup("triangles_onecolor_with_normals",&grpopt,t);
    file.addTriangles(nP, P, nI, PI, materialGreen, nN, N, NI, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES
    }

    if(1){
    file.begingroup("triangles_onecolor",&grpopt);
    file.addTriangles(nP, P, nI, PI, materialGreen, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES
    }

    if(1){
    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=1;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    file.begingroup("triangles_manymaterials",&grpopt,t);
    const uint32_t mesh_index = file.createTriangleMesh(nP, P, nI, PI, PRCNOMATERIALINDEX, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, nM, M, MI);
    file.useMesh(mesh_index, PRCNOMATERIALINDEX);
    file.endgroup();
    PRINTJSNAMES
    }

    if(1){
    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=2;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    PRCmaterial materialBase(
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(1.0,1.0,1.0,1),
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(0.1,0.1,0.1,1),
      1.0,0.1);
    C[0] = RGBAColour(1,0,0,0.1);
    C[1] = RGBAColour(0,1,0,0.5);
    C[2] = RGBAColour(0,0,1,0.9);
    file.begingroup("triangles_rgba_vertexcolors_on_opaque_white_a_component_of_vertexcolor_ignored",&grpopt,t);
    file.addTriangles(nP, P, nI, PI, materialBase, 0, NULL, NULL, 0, NULL, NULL, nC, C, CI, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES

    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=2.5;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    PRCmaterial materialRed(
      RGBAColour(0.1,0.0,0.0,1),
      RGBAColour(1.0,0.0,0.0,1),
      RGBAColour(0.1,0.0,0.0,1),
      RGBAColour(0.1,0.0,0.0,1),
      1.0,0.1);
    C[0] = RGBAColour(1,0,0,0.1);
    C[1] = RGBAColour(0,1,0,0.5);
    C[2] = RGBAColour(0,0,1,0.9);
    file.begingroup("triangles_rgba_vertexcolors_on_opaque_red_a_component_of_vertexcolor_ignored",&grpopt,t);
    file.addTriangles(nP, P, nI, PI, materialRed, 0, NULL, NULL, 0, NULL, NULL, nC, C, CI, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES

    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=3;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    PRCmaterial materialTransparent(
      RGBAColour(0.1,0.1,0.1,0.3),
      RGBAColour(1.0,1.0,1.0,0.3),
      RGBAColour(0.1,0.1,0.1,0.3),
      RGBAColour(0.1,0.1,0.1,0.3),
      0.3,0.1);
    C[0] = RGBAColour(1,0,0,0.1);
    C[1] = RGBAColour(0,1,0,0.5);
    C[2] = RGBAColour(0,0,1,0.9);
    file.begingroup("triangles_rgba_vertexcolors_on_transparent_a_component_of_vertexcolor_ignored",&grpopt,t);
    file.addTriangles(nP, P, nI, PI, materialTransparent, 0, NULL, NULL, 0, NULL, NULL, nC, C, CI, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES

    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=4;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    C[0] = RGBAColour(1,0,0,0.5);
    C[1] = RGBAColour(0,1,0,0.5);
    C[2] = RGBAColour(0,0,1,0.5);
    file.begingroup("triangles_rgb_vertexcolors_on_transparent_may_not_work_in_OpenGL",&grpopt,t);
    file.addTriangles(nP, P, nI, PI, materialTransparent, 0, NULL, NULL, 0, NULL, NULL, nC, C, CI, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES
    }

    if(1){
    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=5;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    const uint32_t picture_width=2;
    const uint32_t picture_height=2;
    const uint8_t picRGB[picture_width*picture_height*3] =
      {255,0,0, 0,255,0, 0,0,255, 0,0,0 };
//      {255,255,0, 255,0,0, 255,0,0, 255,0,0 };
//     { 255,0,0, 255,255,0, 255,255,0, 255,255,255 };
//      {255,0,0, 0,255,0, 0,0,255, 0,0,0 };

    const uint8_t picRGBA[picture_width*picture_height*4] =
      {255,0,0,255, 0,255,0,150, 0,0,255,150, 0,0,0,100 };
// (1,0) 2 3 (1,1)
// (0,0) 0 1 (1,0)
    uint8_t *pictureRGB = new uint8_t[picture_width*picture_height*3];
    for(size_t i=0; i<picture_width*picture_height*3; i++) pictureRGB[i]=picRGB[i];
    picture1 = pictureRGB;
    uint8_t *pictureRGBA = new uint8_t[picture_width*picture_height*4];
    for(size_t i=0; i<picture_width*picture_height*4; i++) pictureRGBA[i]=picRGBA[i];
    picture2 = pictureRGBA;
    const uint32_t nT = 4;
    const double T[nT][2] = { {0.1,0.1}, {0.9,0.1}, {0.9,0.9}, {0.9,0.1} };
    uint32_t TI[nI][3] = {{0,1,3},{1,2,3},{3,2,3}};

    PRCmaterial materialBase(
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(1,1,1,1),
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(0.1,0.1,0.1,1),
      1.0,0.1,
      pictureRGB, KEPRCPicture_BITMAP_RGB_BYTE, picture_width, picture_height, picture_width*picture_height*3, false);
    file.begingroup("triangles_rgb_texture",&grpopt,t);
    file.addTriangles(nP, P, nI, PI, materialBase, 0, NULL, NULL, nT, T, TI, 0, NULL, NULL, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES

    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=6;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    PRCmaterial materialTransparent(
      RGBAColour(0.1,0.1,0.1,0.3),
      RGBAColour(1,1,1,0.3),
      RGBAColour(0.1,0.1,0.1,0.3),
      RGBAColour(0.1,0.1,0.1,0.3),
      0.3,0.1,
      pictureRGBA, KEPRCPicture_BITMAP_RGBA_BYTE, picture_width, picture_height, picture_width*picture_height*4, true);
    file.begingroup("triangles_rgba_texture_replacing",&grpopt,t);
    file.addTriangles(nP, P, nI, PI, materialTransparent, 0, NULL, NULL, nT, T, TI, 0, NULL, NULL, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES
    }

  }

  if(1){
    PRCoptions grpopt;
    grpopt.no_break = true;
    grpopt.do_break = false;
    grpopt.tess = true;
    grpopt.closed = true;

    double t[16];

    const size_t nP = 6;
    double P[nP][3] = {{3+0,0,0},{3+1,0,0},{3+0,1,0},{3+1,1,0},{3+0,2,0},{3+1,2,0}};
    const size_t nI = 2;
    uint32_t PI[nI][4] = {{0,1,3,2},{2,3,5,4}};
    const size_t nM = 2;
    uint32_t M[nM];
    M[0] = file.addMaterial(PRCmaterial(
      RGBAColour(0.0,0.0,0.18),
      RGBAColour(0.0,0.0,0.878431),
      RGBAColour(0.0,0.0,0.32),
      RGBAColour(0.0,0.0,0.072),
      1.0,0.1));
    M[1] = file.addMaterial(PRCmaterial(
      RGBAColour(0.18,0.0,0.0),
      RGBAColour(0.878431,0.0,0.0),
      RGBAColour(0.32,0.0,0.0),
      RGBAColour(0.072,0.0,0.0),
      0.5,0.1));
    uint32_t MI[nI] = {0,1};
    const size_t nN = 2;
    double N[nN][3] = {{0,0,1},{0,0,-1}};
    uint32_t NI[nI][4] = {{0,0,0,0},{1,1,1,1}};

    const uint32_t nC = 3;
    RGBAColour C[nC];
    uint32_t CI[nI][4] = {{0,0,1,1},{1,1,2,2}};
    C[0] = RGBAColour(1,0,0,0.5);
    C[1] = RGBAColour(0,1,0,0.5);
    C[2] = RGBAColour(0,0,1,0.5);

    PRCmaterial materialGreen(
      RGBAColour(0.0,0.18,0.0),
      RGBAColour(0.0,0.878431,0.0),
      RGBAColour(0.0,0.32,0.0),
      RGBAColour(0.0,0.072,0.0),
      1.0,0.1);

    PRCmaterial materialBase(
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(1,1,1,1),
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(0.1,0.1,0.1,1),
      1.0,0.1);

    PRCmaterial materialTransparent(
      RGBAColour(0.1,0.1,0.1,0.3),
      RGBAColour(1,1,1,0.3),
      RGBAColour(0.1,0.1,0.1,0.3),
      RGBAColour(0.1,0.1,0.1,0.3),
      0.3,0.1);

    if(1){
    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=-1;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    file.begingroup("quads_onecolor_with_normals",&grpopt,t);
    file.addQuads(nP, P, nI, PI, materialGreen, nN, N, NI, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES
    }

    if(1){
    file.begingroup("quads_onecolor",&grpopt);
    file.addQuads(nP, P, nI, PI, materialGreen, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES
    }

    if(1){
    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=1;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    file.begingroup("quads_manymaterials",&grpopt,t);
    const uint32_t mesh_index = file.createQuadMesh(nP, P, nI, PI, PRCNOMATERIALINDEX, 0, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, nM, M, MI);
    file.useMesh(mesh_index, PRCNOMATERIALINDEX);
    file.endgroup();
    PRINTJSNAMES
    }

    if(1){
    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=2;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    file.begingroup("quads_rgb_vertexcolors_on_transparent_may_not_work_in_OpenGL",&grpopt,t);
    file.addQuads(nP, P, nI, PI, materialTransparent, 0, NULL, NULL, 0, NULL, NULL, nC, C, CI, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES
    }

    if(1){
    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=5;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    const uint32_t picture_width=2;
    const uint32_t picture_height=2;
    const uint8_t picRGB[picture_width*picture_height*3] =
      {255,0,0, 0,255,0, 0,0,255, 0,0,0 };
//      {255,255,0, 255,0,0, 255,0,0, 255,0,0 };
//     { 255,0,0, 255,255,0, 255,255,0, 255,255,255 };
//      {255,0,0, 0,255,0, 0,0,255, 0,0,0 };

    const uint8_t picRGBA[picture_width*picture_height*4] =
      {255,0,0,255, 0,255,0,150, 0,0,255,150, 0,0,0,100 };
// (1,0) 2 3 (1,1)
// (0,0) 0 1 (1,0)
    uint8_t *pictureRGB = new uint8_t[picture_width*picture_height*3];
    for(size_t i=0; i<picture_width*picture_height*3; i++) pictureRGB[i]=picRGB[i];
    picture3 = pictureRGB;
    uint8_t *pictureRGBA = new uint8_t[picture_width*picture_height*4];
    for(size_t i=0; i<picture_width*picture_height*4; i++) pictureRGBA[i]=picRGBA[i];
    picture4 = pictureRGBA;
    const uint32_t nT = 3;
    const double T[nT][2] = { {0.1,0.1}, {0.5,0.5}, {0.9,0.9} };
    uint32_t TI[nI][4] = {{0,0,1,1},{1,1,2,2}};

    PRCmaterial materialBase(
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(1.0,1.0,1.0,1),
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(0.1,0.1,0.1,1),
      1.0,0.1,
      pictureRGB, KEPRCPicture_BITMAP_RGB_BYTE, picture_width, picture_height, picture_width*picture_height*3, false);
    file.begingroup("quads_rgb_texture",&grpopt,t);
    file.addQuads(nP, P, nI, PI, materialBase, 0, NULL, NULL, nT, T, TI, 0, NULL, NULL, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES

    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=6;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    PRCmaterial materialTransparent(
      RGBAColour(0.1,0.1,0.1,0.3),
      RGBAColour(1,1,1,0.3),
      RGBAColour(0.1,0.1,0.1,0.3),
      RGBAColour(0.1,0.1,0.1,0.3),
      0.6,0.1,
      pictureRGBA, KEPRCPicture_BITMAP_RGBA_BYTE, picture_width, picture_height, 0, true);
    file.begingroup("quads_rgba_texture_replacing",&grpopt,t);
    file.addQuads(nP, P, nI, PI, materialTransparent, 0, NULL, NULL, nT, T, TI, 0, NULL, NULL, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES
    }

    if(1){
    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=7;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    int length;
    uint8_t *buffer;

    {
     ifstream is;
     is.open ("test.png", ios::binary );
     if (!is.is_open())
     {
       fprintf(stderr, "can not open texture file test.png\n");
       exit(1);
     }
     // get length of file:
     is.seekg (0, ios::end);
     length = is.tellg();
     if(length==0 || length==1)
     {
       fprintf(stderr, "can not read texture file test.png\n");
       exit(1);
     }
     is.seekg (0, ios::beg);

     // allocate memory:
     buffer = new uint8_t [length];

     // read data as a block:
     is.read ((char *)buffer,length);

     is.close();
    }

    const uint32_t nT = 6;
    const double T[nT][2] = { {0.1,0.1}, {0.9,0.1}, {0.1,0.5}, {0.9,0.5}, {0.1,0.9}, {0.9,0.9} };
    uint32_t TI[nI][4] = {{0,1,3,2},{2,3,5,4}};

    PRCmaterial materialBase(
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(1.0,1.0,1.0,1),
      RGBAColour(0.1,0.1,0.1,1),
      RGBAColour(0.1,0.1,0.1,1),
      1.0,0.1,
      buffer, KEPRCPicture_PNG, 0, 0, length, false);
    PRCmaterial materialBlue(
      RGBAColour(0.0,0.0,0.1,1),
      RGBAColour(0.0,0.0,1.0,1),
      RGBAColour(0.0,0.0,0.1,1),
      RGBAColour(0.0,0.0,0.1,1),
      1.0,0.1,
      buffer, KEPRCPicture_PNG, 0, 0, length, false);
    file.begingroup("quads_png_texture",&grpopt,t);
    file.addQuads(nP, P, nI, PI, materialBlue, 0, NULL, NULL, nT, T, TI, 0, NULL, NULL, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES
    }

    if(1){
    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=8;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;

    const uint32_t nT = 6;
    const double T[nT][2] = { {0.1,0.1}, {0.9,0.1}, {0.1,0.5}, {0.9,0.5}, {0.1,0.9}, {0.9,0.9} };
    uint32_t TI[nI][4] = {{0,1,3,2},{2,3,5,4}};

    const uint32_t picture_width=2;
    const uint32_t picture_height=2;
    const uint8_t picGREYA[picture_width*picture_height*2] = {255,0, 255,0, 255,0, 128,255};
    const uint8_t picRGBA[picture_width*picture_height*5] = {255,255,255,0, 255,255,255,0, 255,255,255,0, 128,128,128,255};

    PRCmaterial materialBlueTextured(
      RGBAColour(0.0,0.0,0.1,1.0),
      RGBAColour(0.0,0.0,1.0,1.0),
      RGBAColour(0.0,0.0,0.1,1.0),
      RGBAColour(0.0,0.0,0.1,1.0),
      1.0,0.1,
      picGREYA, KEPRCPicture_BITMAP_GREYA_BYTE, picture_width, picture_height);
    file.begingroup("quads_texture",&grpopt,t);
    file.begingroup("quads_graya_texture_over_blue_alpha_does_not_work");
    file.addQuads(nP, P, nI, PI, materialBlueTextured, 0, NULL, NULL, nT, T, TI, 0, NULL, NULL, 0, NULL, NULL);
    file.endgroup();
    PRINTJSNAMES

    t[0+0*4]=1; t[0+1*4]=0; t[0+2*4]=0; t[0+3*4]=0;
    t[1+0*4]=0; t[1+1*4]=1; t[1+2*4]=0; t[1+3*4]=0;
    t[2+0*4]=0; t[2+1*4]=0; t[2+2*4]=1; t[2+3*4]=1;
    t[3+0*4]=0; t[3+1*4]=0; t[3+2*4]=0; t[3+3*4]=1;
    materialBlueTextured.picture_format = KEPRCPicture_BITMAP_RGBA_BYTE;
    materialBlueTextured.picture_data = picRGBA;
    materialBlueTextured.picture_size = picture_width*picture_height*4;
    const uint32_t mesh = file.createQuadMesh(nP, P, nI, PI, PRCNOMATERIALINDEX, 0, NULL, NULL, nT, T, TI, 0, NULL, NULL, 0, NULL, NULL);
    file.begingroup("reused_quads_graya_as_rgba_reused_texture_over_blue",&grpopt,t);
    file.useMesh(mesh, materialBlueTextured);
    file.endgroup();
    PRINTJSNAMES
    materialBlueTextured.diffuse = RGBAColour(1.0,0.0,0.0,1.0),
    file.begingroup("reused_quads_graya_as_rgba_reused_texture_over_red",&grpopt);
    const double origin[3] = { 0, 0, 2 };
    file.useMesh(mesh, materialBlueTextured, origin, NULL, NULL, 1);
    file.endgroup();
    PRINTJSNAMES

    file.endgroup();
    PRINTJSNAMES
    }
  }

  }

  file.finish();

  delete[] picture1;
  delete[] picture2;
  delete[] picture3;
  delete[] picture4;

  return 0;
}
