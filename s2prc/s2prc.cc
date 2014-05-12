/* s2prc.cc
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
 * $Id: s2prc.cc 5815 2012-10-19 04:51:26Z dbarnes $
 *
 */

#define PRC_SUPPRESS_SUBSTRUCTURE 1


#include <sys/stat.h>
#include <strings.h>
#include "oPRCFile.h"

// handy C++ objects
#include <cmath>
#include <vector>
using namespace std;

#define HPDF 1
#if defined(HPDF)
#include <hpdf.h>
#include <hpdf_conf.h>
#include <hpdf_u3d.h>
#include <hpdf_annotation.h>
HPDF_Dict HPDF_LoadJSFromFile  (HPDF_Doc     pdf, const char  *filename);
#endif

// structures for PRC export
struct textureCoords {
  textureCoords() : u(0),v(0) {}
  textureCoords(double U, double V) : u(U),v(V) {}
  double u;
  double v;
};

struct vertex {
  vertex() : x(0.0),y(0.0),z(0.0) {}
  vertex(double X, double Y, double Z) : x(X),y(Y),z(Z) {}
  double x,y,z;
};

struct triangleTextured {
  triangleTextured() :
    texture_a(0),point_a(0),
    texture_b(0),point_b(0),
    texture_c(0),point_c(0) {}
  triangleTextured(uint32_t ta, uint32_t pa,
		   uint32_t tb, uint32_t pb,
		   uint32_t tc, uint32_t pc ) : 
    texture_a(ta),point_a(pa),
    texture_b(tb),point_b(pb),
    texture_c(tc),point_c(pc) {}
  triangleTextured(uint32_t pa,
		   uint32_t pb,
		   uint32_t pc ) : 
    texture_a(pa),point_a(pa),
    texture_b(pb),point_b(pb),
    texture_c(pc),point_c(pc) {}
  uint32_t push_indexes(vector<uint32_t> &triangulated_index) const {
    triangulated_index.push_back(2*texture_a);
    triangulated_index.push_back(3*point_a);
    triangulated_index.push_back(2*texture_b);
    triangulated_index.push_back(3*point_b);
    triangulated_index.push_back(2*texture_c);
    triangulated_index.push_back(3*point_c);
    return 9;
  }
  uint32_t texture_a;
  uint32_t point_a;
  uint32_t texture_b;
  uint32_t point_b;
  uint32_t texture_c;
  uint32_t point_c;
};

struct triangleFanTextured {
  triangleFanTextured(uint32_t ta, uint32_t pa,
		      uint32_t tb, uint32_t pb,
		      uint32_t tc, uint32_t pc,
		      uint32_t td, uint32_t pd ) {
    textures.push_back(ta); points.push_back(pa);
    textures.push_back(tb); points.push_back(pb);
    textures.push_back(tc); points.push_back(pc);
    textures.push_back(td); points.push_back(pd);
  }
  triangleFanTextured(uint32_t pa,
		      uint32_t pb,
		      uint32_t pc,
		      uint32_t pd ) {
    textures.push_back(pa); points.push_back(pa);
    textures.push_back(pb); points.push_back(pb);
    textures.push_back(pc); points.push_back(pc);
    textures.push_back(pd); points.push_back(pd);
  }
  uint32_t push_indexes(vector<uint32_t> &triangulated_index) const {
    const size_t numPoints = points.size();
    for(size_t i=0; i< numPoints; i++) {
      triangulated_index.push_back(2*textures[i]);
      triangulated_index.push_back(3*points[i]);
    }
    return numPoints*3;
  }
  uint32_t size() const {
    return points.size();
  }
  vector<uint32_t> textures;
  vector<uint32_t> points;
};

struct triangleStripe {
  vector<uint32_t> points;
};

// s2plot macros and structures
#define BUILDING_S2PLOT 1
extern "C" {
#include "s2plot.h"
#include "geomviewer.h"
  void _s2_startDynamicGeometry(int);
  void _s2_endDynamicGeometry(void);
}
#undef BUILDING_S2PLOT
#include "s2plot_glodef.h"
#undef shininess
#undef transparency
#if !defined(FALSE)
#define FALSE 0
#endif

/* cached texture structure */
#if !defined(_S2CACHEDTEXTURE_STRUCT_DEFINED)
typedef struct {
  int width, height;    // "user" width and height
  int width2, height2;  // "internal" width and height (poss. powers of two)
  float wfrac, hfrac;   // width and height fraction for texture coordinates
                        // when width2 > width &/ height2 > height
  BITMAP4 *bitmap;      // the pixels
  GLuint id;            // OpenGL texture id
  int depth, depth2;    // "user" and "internal" depth for 3d textures
} _S2CACHEDTEXTURE;
#define _S2CACHEDTEXTURE_STRUCT_DEFINED 1
#endif

/* transparent 3-vertex facet structure */
#if !defined(_S2FACE3A_STRUCT_DEFINED)
typedef struct {
  XYZ p[3];               // vertices
  XYZ n[3];               // vertex normals
  COLOUR colour[3];       // vertex colours
  int trans;              // 'o' = opaque, 't'/'s' = transparent
  double alpha[3];        // transparency: 1.0 = default = opaque
  char whichscreen[10];   // for screen geometry, which screen/s?
  char VRMLname[32];      // which named branch to put in VRML / PRC output
} _S2FACE3A;
#define _S2FACE3A_STRUCT_DEFINED 1
#endif

/* billboard structure - NOT ALL FIELDS SUPPORTED IN PRC EXPORT */
#if !defined(_S2BBOARD_STRUCT_DEFINED)
typedef struct {
  XYZ p;                 // vertex (position)
  XYZ str;               // stretch (direction + magnitude): 0 = no stretch
  XYZ offset;            // offset (X and Y only, Z ignored)
  float aspect;          // aspect ratio: width to height "on the screen"
  float size;            // size of billboard
  float pa;              // position angle in radians, clockwise from 12 o'clock
  COLOUR col;            // colour
  unsigned int texid;    // OpenGL texture id
  float alpha;           // transparency: 1.0 = default = opaque
  double dist;           // distance to camera - used internally for sorting
  char trans;            // 'o' = opaque, 't'/'s' = transparent
  char whichscreen[10];  // for screen geometry, which screen/s?
  char VRMLname[32];     // which named branch to put in VRML / PRC output
} _S2BBOARD;
#define _S2BBOARD_STRUCT_DEFINED
#endif

// textured mesh
#if !defined(_S2TEXTUREDMESH_STRUCT_DEFINED)
typedef struct {
  int nverts; // vertices
  XYZ *verts;
  int nnorms; // normals
  XYZ *norms;
  int nvtcs;  // vertex texture coordinates
  XYZ *vtcs; // only XY used (as u,v)
  int nfacets; // (triangular) facets
  int *facets; // 3 * int per facet = 3 * vertex INDICES (look-up)
  int *facets_vtcs; // 3 * int per facet = 3 * texture coordinate INDICES
  unsigned int texid;
  int trans; /* 'o' = opaque, 't'/'s' = transparent */
  double alpha; /* 1.0 = opaque, 0.0 = totally transparent */
  char whichscreen[10];
  char VRMLname[32];
} _S2TEXTUREDMESH;
#define _S2TEXTUREDMESH_STRUCT_DEFINED
#endif



// s2plot global variables
extern int _s2_nVRMLnames;
extern char **_s2_VRMLnames;
extern int nline;
extern LINE *line;
extern int ndot;
extern DOT *dot;
extern int nface4;
extern FACE4 *face4;
extern int nface4t;
extern FACE4T *face4t;
extern int nface3;
extern FACE3 *face3;
extern int nface3a;
extern _S2FACE3A *face3a;
extern int nball;
extern BALL *ball;
extern int ncone;
extern CONE *cone;
extern int nlabel;
extern LABEL *label;
extern int nbboard;
extern _S2BBOARD *bboard;
extern int _s2_ctext_count;
extern _S2CACHEDTEXTURE *_s2_ctext;
extern int ntexmesh;
extern _S2TEXTUREDMESH *texmesh;

// local globals
int _s2x_prccount = 0; // informational: count how many objects we have exported
char group[256];       // convenience: write group names to this variable

// local function declarations - many of these do not honour "end", and simply export
// just the 'start-th' piece of geometry.  Needs to be fixed / optimised in future,
// but no impact at this stage on end-user, just code developer :-)
PRCmaterial doPRCsingleMaterial(unsigned int texid);
void doPRClines(oPRCFile &f, int start, int end);
void doPRCdots(oPRCFile &f, int start, int end);
void doPRCtextures(oPRCFile &f);
void doPRCface4(oPRCFile &f, int which);
void doPRCface4t(oPRCFile &f, int start, int end);
void doPRCface3(oPRCFile &f, int start, int end);
void doPRCface3a(oPRCFile &f, int start, int end);
void doPRCball(oPRCFile &f, int which);
void doPRCcone(oPRCFile &f, int which);
void doPRCcylinder(oPRCFile &f, int which);
void doPRClabel(oPRCFile &f, int which);
void doPRCbboard(oPRCFile &f, int which);
void doPRCtexmesh(oPRCFile &f, int which);

void _clear_li_mesh(void);
void _prealloc_li_mesh(int nli);
void _accumulate_li_mesh(int which);
void _emit_li_mesh(oPRCFile &f);

void _clear_f3_mesh(void);
void _prealloc_f3_mesh(int ntri);
void _accumulate_f3_mesh(int which);
void _emit_f3_mesh(oPRCFile &f);

void _clear_f3a_mesh(void);
void _prealloc_f3a_mesh(int ntri);
void _accumulate_f3a_mesh(int which);
void _emit_f3a_mesh(oPRCFile &f);

extern "C" {

void cppWritePRC(void) {
  oPRCFile file("s2direct.prc");

  doPRCtextures(file);
  
  _s2x_prccount = 0;
  int nidx, dloop, i, j;

  FILE *MAPF = fopen("s2direct.map", "w");

  // this loop is over the VRML branch / group names the user has 
  // "pushed" in their S2PLOT program ... the PRC tree is structured
  // the same way.
  for (nidx = 0; nidx < _s2_nVRMLnames; nidx++) {

    file.begingroup(_s2_VRMLnames[nidx]);


    // this loop accounts for static geometry (created once only) and
    // dynamic geometry (created in the users callback function/s).
    for (dloop = 0; dloop < 2; dloop++) {

      if (dloop == 1) {
	_s2_startDynamicGeometry(FALSE);
      }

      // dots
      if (ndot > 0) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	sprintf(group, "DOTS%d", dloop);
	file.begingroup(group);
#endif
	for (i = 0; i < ndot; ) {
	  // get start point
	  while (i < ndot && 
		 (strcmp(dot[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(dot[i].whichscreen))) {
	    i++;
	  }
	  if (i == ndot) {
	    break;
	  }
	  // get end point
	  j = i;
	  // emit geometry
	  doPRCdots(file, i, j);
	  i = j + 1;
	}
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	file.endgroup();
#endif
      }

#define COMPRESS_LI 1
      // lines
#if defined(COMPRESS_LI)
      _clear_li_mesh();
      _prealloc_li_mesh(nline);
#endif
      if (nline > 0) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	sprintf(group, "LINES%d", dloop);
	file.begingroup(group);
#endif
	for (i = 0; i < nline; ) {
	  // get start point
	  while (i < nline && 
		 (strcmp(line[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(line[i].whichscreen))) {
	    i++;
	  }
	  if (i == nline) {
	    break;
	  }
	  // get end point
	  j = i;
#if defined(COMPRESS_LI)
	  _accumulate_li_mesh(i);
#else
	  // emit geometry
	  doPRClines(file, i, j);
#endif
	  i = j + 1;
	}
#if defined(COMPRESS_LI)
	_emit_li_mesh(file);
#endif
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	file.endgroup();	       
#endif
      }

      // 4-vertex facets
      if (nface4 > 0) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	sprintf(group, "FACE4S%d", dloop);
	file.begingroup(group);
#endif
	for (i = 0; i < nface4; ) {
	  // get start point
	  while ((i < nface4) && 
		 (strcmp(face4[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(face4[i].whichscreen))) {
	    i++;
	  }
	  if (i == nface4) {
	    break;
	  }
	  // emit geometry
	  doPRCface4(file, i);
	  i++;
	}
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	file.endgroup();
#endif
      }

      // 4-vertex textured facets 
      if (nface4t > 0) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	sprintf(group, "FACE4TS%d", dloop);
	file.begingroup(group);
#endif
	for (i = 0; i < nface4t; ) {
	  // get start point
	  while ((i < nface4t) && 
		 (strcmp(face4t[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(face4t[i].whichscreen))) {
	    i++;
	  }
	  if (i == nface4t) {
	    break;
	  }
	  // get end point
	  j = i;
	  // emit geometry
	  doPRCface4t(file, i, j);
	  i = j + 1;
	}
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	file.endgroup();
#endif
      }

#define COMPRESS_F3 1
      // 3-vertex facets
#if defined(COMPRESS_F3)
      // reset list of points (vertices), triangles (vertex index triplets), colours
      _clear_f3_mesh();
      _prealloc_f3_mesh(nface3);
#endif
      if (nface3 > 0) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	sprintf(group, "FACE3S%d", dloop);
	//fprintf(stderr, "nface3: there are %d face3s ...\n", nface3);
	file.begingroup(group);
#endif
	for (i = 0; i < nface3; ) {
	  // get start point
	  while ((i < nface3) && 
		 (strcmp(face3[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(face3[i].whichscreen))) {
	    i++;
	  }
	  if (i == nface3) {
	    break;
	  }
	  // get end point
	  j = i;
#if defined(COMPRESS_F3)
	  // add only new vertices to list of points; store indices for triangle
	  _accumulate_f3_mesh(i);
#else
	  // original direct version
	  // emit geometry
	  doPRCface3(file, i, j);
#endif
	  i = j + 1;
	}
#if defined(COMPRESS_F3)
	_emit_f3_mesh(file);
#endif
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	file.endgroup();
#endif
      }
	
      // 3-vertex facets with transparency
      //#undef COMPRESS_F3
#if defined(COMPRESS_F3)
      // reset list of points (vertices), triangles (vertex index triplets), colours
      _clear_f3a_mesh();
      _prealloc_f3a_mesh(nface3a);
#endif
      if (nface3a > 0) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	sprintf(group, "FACE3AS%d", dloop);
	//fprintf(stderr, "nface3a: there are %d face3as...\n", nface3a);
	file.begingroup(group);
#endif
	for (i = 0; i < nface3a; ) {
	  // get start point
	  while ((i < nface3a) && 
		 (strcmp(face3a[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(face3a[i].whichscreen))) {
	    i++;
	  }
	  if (i == nface3a) {
	    break;
	  }
	  // get end point
	  j = i;
#if defined(COMPRESS_F3)
	  // add only new vertices to list of points; store indices for triangle
	  _accumulate_f3a_mesh(i);
#else
	  // original direct version
	  // emit geometry
	  doPRCface3a(file, i, j);
#endif
	  i = j + 1;
	}
#if defined(COMPRESS_F3)
	_emit_f3a_mesh(file);
#endif
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	file.endgroup();
#endif
      }

      // balls
      if (nball > 0) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	sprintf(group, "BALLS%d", dloop);
	file.begingroup(group);
#endif
	for (i = 0; i < nball; ) {
	  // get start point
	  while ((i < nball) && 
		 (strcmp(ball[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(ball[i].whichscreen))) {
	    i++;
	  }
	  if (i == nball) {
	    break;
	  }
	  // emit geometry
	  doPRCball(file, i);
	  i++;
	}
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	file.endgroup();
#endif
      }

      // cones
      if (ncone > 0) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	sprintf(group, "CONES%d", dloop);
	file.begingroup(group);
#endif
	for (i = 0; i < ncone; ){
	  // get start point
	  while ((i < ncone) &&
		 (strcmp(cone[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(cone[i].whichscreen) ||
		  ((cone[i].r2 > EPS) && (cone[i].r1 > EPS)))) {
	    i++;
	  }
	  if (i == ncone) {
	    break;
	  }
	  // emit geometry
	  doPRCcone(file, i);
	  i++;
	}
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	file.endgroup();
#endif
      }

      // cylinders
      if (ncone > 0) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	sprintf(group, "CYLINDERS%d", dloop);
	file.begingroup(group);
#endif
	for (i = 0; i < ncone; ){
	  // get start point
	  while ((i < ncone) &&
		 (strcmp(cone[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(cone[i].whichscreen) ||
		  (fabs(cone[i].r2 - cone[i].r1) > EPS))) {
	    i++;
	  }
	  if (i == ncone) {
	    break;
	  }
	  // emit geometry
	  doPRCcylinder(file, i);
	  i++;
	}
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	file.endgroup();
#endif
      }
      
      // text labels
      if (nlabel > 0) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	sprintf(group, "LABELS%d", dloop);
	file.begingroup(group);
#endif
	for (i = 0; i < nlabel; ) {
	  // get start point
	  while ((i < nlabel) && 
		 (strcmp(label[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(label[i].whichscreen))) {
	    i++;
	  }
	  if (i == nlabel) {
	    break;
	  }
	  // emit geometry
	  doPRClabel(file, i);
	  i++;
	}
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
	file.endgroup();
#endif
      }

      // billboards
      if (nbboard > 0) {
	sprintf(group, "BBOARDS%d", dloop);
	file.begingroup(group);
	for (i = 0; i < nbboard; ) {
	  // get start point
	  while ((i < nbboard) && 
		 (strcmp(bboard[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(bboard[i].whichscreen))) {
	    i++;
	  }
	  if (i == nbboard) {
	    break;
	  }
	  // emit geometry
	  doPRCbboard(file, i);
	  i++;
	}
	file.endgroup();
      } 

      if (ntexmesh > 0) {
	sprintf(group, "TEXMESH%d", dloop);
	file.begingroup(group);
	for (i = 0; i < ntexmesh; i++) {
	  while ((i < ntexmesh) &&
		 (strcmp(texmesh[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(texmesh[i].whichscreen))) {
	    i++;
	  }
	  if (i == ntexmesh) {
	    break;
	  }
	  // emit geometry
	  doPRCtexmesh(file, i);
	  i++;
	}
	file.endgroup();
      }

      if (dloop == 1) {
	_s2_endDynamicGeometry();
      }
    }
    file.endgroup();
    //fprintf(MAPF, "%s maps to %s\n", _s2_VRMLnames[nidx], file.lastgroupname.c_str());
    fprintf(MAPF, "  PART={%s}\n    VISIBLE=true\n  END\n", file.lastgroupname.c_str());

  }
  fclose(MAPF);
  
  file.finish();
  fprintf(stderr, "* N.B.: ballt, disk, trdot, bbset, handle, texpoly3d NOT YET EXPORTED.\n");
  fprintf(stderr, "* N.B.: screen (2-d) geometry                        NOT YET EXPORTED.\n");

  fprintf(stderr, "Wrote PRC file \"s2direct.prc\"\n");
  fprintf(stderr, "Wrote MAP file \"s2direct.map\"\n");

#if defined(HPDF)
  char *tmpchr = getenv("S2PATH");
  if (!tmpchr) {
    fprintf(stderr, "S2PATH environment variable not set: cannot export PRC");
    return;
  }

  //fprintf(stderr, "Writing test PDF file ...\n");

  HPDF_Rect rect = {0, 0, 600, 600};
  
  HPDF_Doc  pdf;
  HPDF_Page page;
  HPDF_Annotation annot;
  HPDF_U3D u3d;
  
  pdf = HPDF_New (NULL, NULL);
  
  pdf->pdf_version = HPDF_VER_17;
  
  page = HPDF_AddPage (pdf);
  
  HPDF_Page_SetWidth (page, 600);
  HPDF_Page_SetHeight (page, 600);
  
  HPDF_Dict js = NULL;
  js = HPDF_DictStream_New (pdf->mmgr, pdf->xref);
  js->header.obj_class |= HPDF_OSUBCLASS_XOBJECT;
  
  /* add required elements */
  js->filter = HPDF_STREAM_FILTER_NONE;

  //fprintf(stderr, "A\n");

  char js_src[400];
  sprintf(js_src, "%s/s2prc/s2plot-prc.js", tmpchr);

  js = HPDF_LoadJSFromFile  (pdf, js_src);
  
  u3d = HPDF_LoadU3DFromFile (pdf, "s2direct.prc");

  //fprintf(stderr, "B\n");

  // add javeascript  action
  HPDF_Dict_Add (u3d, "OnInstantiate", js);

#define NS2VIEWS 7
  HPDF_Dict views[NS2VIEWS+1];
  const char *view_names[] = {"Front perspective ('1','H')",
			      "Back perspective ('2')",
			      "Right perspective ('3')",
			      "Left perspective ('4')",
			      "Bottom perspective ('5')", 
			      "Top perspective ('6')",
			      "Oblique perspective ('7')"};
  const float view_c2c[][3] = {{0., 0., 1.},
			       {0., 0., -1.},
			       {-1., 0., 0.},
			       {1., 0., 0.},
			       {0., 1., 0.},
			       {0., -1., 0.},
			       {-1., 1., -1.}};
  const float view_roll[] = {0., 180., 90., -90., 0., 0., 60.};

  //fprintf(stderr, "C\n");

  float b_r, b_g, b_b;
  ss2qbc(&b_r, &b_g, &b_b);

  int iv;

  XYZ focus, pos, pr;
  ss2qpr(&pr, 1);
  int set;
  ss2qcf(&set, &focus, 1);
  ss2qc(&pos, NULL, NULL, 1);
  float camrot = VectorLength(pos, pr);
  camrot = 5.0;
  //fprintf(stderr, "cam pos = %f %f %f\n", pos.x, pos.y, pos.z);
  //fprintf(stderr, "cam foc = %f %f %f\n", focus.x, focus.y, focus.z);
  //fprintf(stderr, "cam pr  = %f %f %f\n", pr.x, pr.y, pr.z);
  //fprintf(stderr, "camrot = %f\n", camrot);

  for (iv = 0; iv < NS2VIEWS; iv++) {
    views[iv] = HPDF_Create3DView(u3d->mmgr, view_names[iv]);
    HPDF_3DView_SetCamera(views[iv], 0., 0., 0., 
			  view_c2c[iv][0], view_c2c[iv][1], view_c2c[iv][2],
			  camrot, view_roll[iv]);
    HPDF_3DView_SetPerspectiveProjection(views[iv], 45.0);
    HPDF_3DView_SetBackgroundColor(views[iv], b_r, b_g, b_b);
    HPDF_3DView_SetLighting(views[iv], "White");

    HPDF_U3D_Add3DView(u3d, views[iv]);
  }

  // add a psuedo-orthographic for slicing (actually perspective with point at infinity)
  views[NS2VIEWS] = HPDF_Create3DView(u3d->mmgr, "Orthgraphic slicing view");
  HPDF_3DView_SetCamera(views[NS2VIEWS], 0., 0., 0., 
			view_c2c[0][0], view_c2c[0][1], view_c2c[0][2],
			camrot*82.70, view_roll[0]);
  HPDF_3DView_SetPerspectiveProjection(views[NS2VIEWS], 0.3333);
  //HPDF_3DView_SetOrthogonalProjection(views[NS2VIEWS], 45.0/1000.0);
  HPDF_3DView_SetBackgroundColor(views[NS2VIEWS], b_r, b_g, b_b);
  HPDF_3DView_SetLighting(views[NS2VIEWS], "White");
  HPDF_U3D_Add3DView(u3d, views[NS2VIEWS]);


  HPDF_U3D_SetDefault3DView(u3d, "Front perspective");

  //  Create annotation
  annot = HPDF_Page_Create3DAnnot (page, rect, u3d );
  
  // make the toolbar appear by default
  HPDF_Dict action = (HPDF_Dict)HPDF_Dict_GetItem (annot, "3DA", HPDF_OCLASS_DICT);
  HPDF_Dict_AddBoolean (action, "TB", HPDF_TRUE);
  

  /* save the document to a file */
  HPDF_SaveToFile (pdf, "s2plotprc.pdf");
  
  /* clean up */
  HPDF_Free (pdf);

  //fprintf(stderr, "... finished PDF file.\n");
  fprintf(stderr, "Wrote PDF file \"s2plotprc.pdf\"\n");

#endif

}

  void prcwriter(void) {
    cppWritePRC();
    fprintf(stderr, "\n");
    fprintf(stderr, "====================================================================\n");
    fprintf(stderr, "S2PLOT PRC export module\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "The authors of S2PLOT would appreciate it if research outcomes using\n");
    fprintf(stderr, "S2PLOT would provide the following acknowledgement: Three-dimensional\n");
    fprintf(stderr, "visualisation was conducted with the S2PLOT progamming library, and a\n");
    fprintf(stderr, "reference to:\n");
    fprintf(stderr, "  D.G.Barnes, C.J.Fluke, P.D.Bourke & O.T.Parry, 2006, Publications\n");
    fprintf(stderr, "  of the Astronomical Society of Australia, 23(2), 82-93,\n");
    fprintf(stderr, "or, if you have created 3-d PDF figures using S2PLOT, \n");
    fprintf(stderr, "  D.G.Barnes & C.J.Fluke, 2008, New Astronomy, 13, 599.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Thank you!\n");
    fprintf(stderr, " ====================================================================\n");
    fprintf(stderr, "\n");
  }

}

#define BASE_RGBA_AMBI RGBAColour(0.1,0.1,0.1)
#define BASE_RGBA_EMIS RGBAColour(0.1,0.1,0.1)
#define BASE_RGBA_SPEC RGBAColour(0.4,0.4,0.4)
#define BASE_SHININESS 0.6
// amb, diff, emiss, spec
const PRCmaterial BASE_MATERIAL(BASE_RGBA_AMBI, 
				RGBAColour(0.9,0.9,0.9),
				BASE_RGBA_EMIS, BASE_RGBA_SPEC, 1.0, BASE_SHININESS);

RGBAColour average3rgba(RGBAColour &a, RGBAColour &b, RGBAColour &c) {
  RGBAColour ret;
  ret.R = (a.R + b.R + c.R) * 0.333333;
  ret.G = (a.G + b.G + c.G) * 0.333333;
  ret.B = (a.B + b.B + c.B) * 0.333333;
  ret.A = (a.A + b.A + c.A) * 0.333333;
  return ret;
}
RGBAColour average4rgba(RGBAColour &a, RGBAColour &b, RGBAColour &c, RGBAColour &d) {
  RGBAColour ret;
  ret.R = (a.R + b.R + c.R + d.R) * 0.25;
  ret.G = (a.G + b.G + c.G + d.G) * 0.25;
  ret.B = (a.B + b.B + c.B + d.B) * 0.25;
  ret.A = (a.A + b.A + c.A + d.A) * 0.25;
  return ret;
}



map<unsigned int, PRCmaterial> PRCmaterials; // key is S2PLOT textureid, value is PRC material
void doPRCtextures(oPRCFile &file) {
  PRCmaterials.clear();
  int i;
  for (i = 0; i < _s2_ctext_count; i++) {
    unsigned int texid = _s2_ctext[i].id;
    PRCmaterial mat = doPRCsingleMaterial(texid);
    PRCmaterials[texid] = mat;
  }
  if (_s2_ctext_count > 0) {
    fprintf(stderr, "Wrote %d textures.\n", _s2_ctext_count);
  }
}
PRCmaterial doPRCsingleMaterial(unsigned int texid) {
  int nx = 0, ny = 0;
  unsigned char *bits;
  bits = ss2gt(texid, &nx, &ny); // fetch pixels from S2PLOT cache store
  //fprintf(stderr, "doPRCsingleMaterial: %d %d %d\n", texid, nx, ny);
  if ((nx < 1) || (ny < 1)) {
    //fprintf(stderr, "cannot store textureid %d in PRC (invalid dimensions)\n", texid);
    return PRCmaterial(RGBAColour(0,0,0,0),RGBAColour(0,0,0,0),RGBAColour(1,1,1,1),RGBAColour(0,0,0,0),1.,0.);
  }
#define DO_COMPRESSED_TEXTURES 1
#if defined(DO_COMPRESSED_TEXTURES)

  // if all pixels opaque, do jpeg, otherwise png
  int do_png = 0;
  int i;
  for (i = 0; i < nx * ny; i++) {
    if (bits[i*4+3] < 255) {
      do_png = 1;
      break;
    }
  }
  
  // export tga
  FILE *fptr = fopen("S2PRCTMP.TGA", "wb");
  Write_Bitmap(fptr, (BITMAP4 *)bits, nx, ny, do_png ? 11 : 1);  // format==1 -> basic tga; 11 = with alpha
  fclose(fptr);

  if (do_png) {
    if (system("convert S2PRCTMP.TGA -dither FloydSteinberg -colors 256 S2PRCTMP.PNG")) {
      fprintf(stderr, "failed to convert tga to png!\n");
      exit(1);
    }
  } else {
    if (system("convert S2PRCTMP.TGA S2PRCTMP.JPG")) {
      fprintf(stderr, "failed to convert tga to jpg!\n");
      exit(1);
    }
  }

  // read jpg/png buffer
  int length;
  uint8_t *buffer;  
  {
    ifstream is;
    if (do_png) {
      is.open("S2PRCTMP.PNG", ios::binary);
    } else {
      is.open ("S2PRCTMP.JPG", ios::binary );
    }
    if (!is.is_open())
      {
	fprintf(stderr, "can not open temporary compressed texture file\n");
	exit(1);
      }
    // get length of file:
    is.seekg (0, ios::end);
    length = is.tellg();
    if(length==0 || length==1)
      {
	fprintf(stderr, "can not read temporary compressed texture file\n");
	exit(1);
      }
    is.seekg (0, ios::beg);
    
    // allocate memory:
    buffer = new uint8_t [length];
    
    // read data as a block:
    is.read ((char *)buffer,length);
    
    is.close();
  }
  
  // remove tmp files
  system("rm -f S2PRCTMP.TGA S2PRCTMP.PNG S2PRCTMP.JPG");

  PRCmaterial materialTransparent(RGBAColour(0,0,0), // ambient
				  RGBAColour(0,0,0), // diffuse
				  RGBAColour(1,1,1), // emissive (was BASE_RGBA_EMIS)
				  RGBAColour(0,0,0), // specular
				  1.0, 0.0, // alpha, shininess
				  buffer, do_png ? KEPRCPicture_PNG : KEPRCPicture_JPG, 
				  0, 0, length, false);

#else
  PRCmaterial materialTransparent(RGBAColour(0,0,0), // ambient
				  RGBAColour(0,0,0), // diffuse
				  //BASE_RGBA_EMIS, BASE_RGBA_SPEC,
				  RGBAColour(1,1,1), // emissive (was BASE_RGBA_EMIS)
				  RGBAColour(0,0,0), // specular
				  1.0, 0.0, // alpha, shininess
				  bits, KEPRCPicture_BITMAP_RGBA_BYTE, nx, ny, nx*ny*4, true, false);
#endif
  return materialTransparent;
}

void doPRCdots(oPRCFile &file, int start, int end) {
  int i;
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif
  for (i = start; i <= end; i++) {
    double pts[3];
    pts[0] = dot[i].p.x;
    pts[1] = dot[i].p.y;
    pts[2] = dot[i].p.z;
    file.addPoint(pts, RGBAColour(dot[i].colour.r, dot[i].colour.g, dot[i].colour.b));
  } 
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}

void doPRClines(oPRCFile &file, int start, int end) {
  int i;
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif
  for (i = start; i <= end; i++) {
    double pts[2][3];
    pts[0][0] = line[i].p[0].x;
    pts[0][1] = line[i].p[0].y;
    pts[0][2] = line[i].p[0].z;
    pts[1][0] = line[i].p[1].x;
    pts[1][1] = line[i].p[1].y;
    pts[1][2] = line[i].p[1].z;
    file.addLine(2, pts, RGBAColour(line[i].colour[0].r, line[i].colour[0].g, line[i].colour[0].b));
  }
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}

void doPRCface4(oPRCFile &file, int wh) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif
  
  // see _emit_f3_mesh for details on colouring

  double P[4][3];
  RGBAColour C[4];
  for (uint32_t i = 0; i < 4; i++) {
    P[i][0] = face4[wh].p[i].x;
    P[i][1] = face4[wh].p[i].y;
    P[i][2] = face4[wh].p[i].z;
    C[i] = RGBAColour(face4[wh].colour[i].r, face4[wh].colour[i].g, face4[wh].colour[i].b);
  }
  uint32_t PPI[1][4] = {{0,1,2,3}};

  PRCmaterial mat = BASE_MATERIAL;
  mat.diffuse = average4rgba(C[0], C[1], C[2], C[3]);

  file.addQuads(4 /*points*/,P,1 /* quads */,PPI,mat,0,NULL,NULL,0,NULL,NULL,0,NULL,NULL,0,NULL,NULL);
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}

void doPRCface4t(oPRCFile &file, int start, int end) {
  unsigned int texid = face4t[start].textureid;
  int picture_width, picture_height;
  ss2gt(texid, &picture_width, &picture_height);
  float dw = 0.5 * 1.0 / (float)picture_width;
  float dh = 0.5 * 1.0 / (float)picture_height;
  PRCmaterial mat = PRCmaterials[texid];
  mat.alpha = face4t[start].alpha;
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif
  double P[4][3];
  for (uint32_t i = 0; i < 4; i++) {
    P[i][0] = face4t[start].p[i].x;
    P[i][1] = face4t[start].p[i].y;
    P[i][2] = face4t[start].p[i].z;
  }
  uint32_t PPI[1][4] = {{0,1,2,3}};
  // tex coords should really be [0,0] -> [1,1] but need small delta based on 
  // image width/height to get PRC -> PDF rendering working acceptably
  double T[4][2] = { {dw,1.0-dh}, {1.0-dw,1.0-dh}, {1.0-dw,dh}, {dw,dh} };
  uint32_t TI[1][4] = {{0,1,2,3}};
  file.addQuads(4 /*points*/,P,1 /* quads */,PPI,mat,0,NULL,NULL,4,T,TI,0,NULL,NULL,0,NULL,NULL);
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}

void doPRCface3(oPRCFile &file, int start, int end) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif
  PRCmaterial materialBase(RGBAColour(0.0,0.0,0.0,1), RGBAColour(1,1,1,1),
			   RGBAColour(0.0,0.0,0.0,1), RGBAColour(0.0,0.0,0.0,1),
			   1.0,0.0);
  int j;
  uint32_t PPI[1][3] = {{0,1,2}};
  for (j = start; j <= end; j++) {
    double P[3][3];
    RGBAColour C[3];
    for (uint32_t i = 0; i < 3; i++) {
      P[i][0] = face3[j].p[i].x;
      P[i][1] = face3[j].p[i].y;
      P[i][2] = face3[j].p[i].z;
      C[i] = RGBAColour(face3[j].colour[i].r, face3[j].colour[i].g, face3[j].colour[i].b);
    }
    // works for multicoloured vertices but transparency (in materialBase & C colours) ignored
    file.addTriangles(3 /*points*/,P,1 /* tris */,PPI,materialBase,0,NULL,NULL,0,NULL,NULL,3,C,PPI,0,NULL,NULL);
    // unicoloured vertices, but transparency works (in materialBase)
    // file.addTriangles(3 /*points*/,P,1 /* tris */,PPI,materialBase,0,NULL,NULL,0,NULL,NULL,0,NULL,NULL,0,NULL,NULL);
  }
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}

void doPRCface3a(oPRCFile &file, int start, int end) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif
  PRCmaterial materialBase(RGBAColour(0.0,0.0,0.0,1), RGBAColour(1,1,1,1),
			   RGBAColour(0.0,0.0,0.0,1), RGBAColour(0.0,0.0,0.0,1),
			   1.0,0.0);
  int j;
  uint32_t PPI[1][3] = {{0,1,2}};
  for (j = start; j <= end; j++) {
    double P[3][3];
    RGBAColour C[3];
    for (uint32_t i = 0; i < 3; i++) {
      P[i][0] = face3a[j].p[i].x;
      P[i][1] = face3a[j].p[i].y;
      P[i][2] = face3a[j].p[i].z;
      C[i] = RGBAColour(face3a[j].colour[i].r, face3a[j].colour[i].g, face3a[j].colour[i].b, face3a[j].alpha[i]);
    }
    // works for multicoloured vertices but transparency (in materialBase & C colours) ignored
    file.addTriangles(3 /*points*/,P,1 /* tris */,PPI,materialBase,0,NULL,NULL,0,NULL,NULL,3,C,PPI,0,NULL,NULL);
    // unicoloured vertices, but transparency works (in materialBase)
    // file.addTriangles(3 /*points*/,P,1 /* tris */,PPI,materialBase,0,NULL,NULL,0,NULL,NULL,0,NULL,NULL,0,NULL,NULL);
  }
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}

void doPRCball(oPRCFile &file, int wh) {
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  PRCoptions propts;
  propts.granularity=12.0; // reduce the tesslation resolution of the sphere
  file.begingroup(group, &propts);
  PRCmaterial mat(RGBAColour(0.0,0.0,0.0,1), 
		  RGBAColour(ball[wh].colour.r, ball[wh].colour.g, ball[wh].colour.b,1),
		  RGBAColour(0.0,0.0,0.0,1), RGBAColour(0.0,0.0,0.0,1),
		  1.0,0.0);
  double sp_origin[3] = {ball[wh].p.x, ball[wh].p.y, ball[wh].p.z};
  const double sp_x_axis[3] = {1,0,0};
  const double sp_y_axis[3] = {0,1,0};
  const double sp_scale = 1;
  file.addSphere(ball[wh].r, mat, sp_origin, sp_x_axis, sp_y_axis, sp_scale);
  file.endgroup();
}

void doPRCcone(oPRCFile &file, int wh) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif
  PRCmaterial mat(RGBAColour(0.0,0.0,0.0,1), 
		  RGBAColour(cone[wh].colour.r, cone[wh].colour.g, cone[wh].colour.b,1),
		  RGBAColour(0.0,0.0,0.0,1), RGBAColour(0.0,0.0,0.0,1),
		  1.0,0.0);
  XYZ Pcen, Ptip;
  double Crad;
  XYZ Porig;
  
  // centre of cone
  Pcen = MidPoint(cone[wh].p1, cone[wh].p2);
  // unit vector from centre to one end
  if (cone[wh].r1 > EPS) {
    Ptip = VectorSub(Pcen, cone[wh].p2);
    Crad = cone[wh].r1;
    Porig = cone[wh].p1;
  } else {
    Ptip = VectorSub(Pcen, cone[wh].p1);
    Crad = cone[wh].r2;
    Porig = cone[wh].p2;
  }
  Normalise(&Ptip);
  
  // basic identify transformation matrix
  double t[4][4];
  t[0][0]=1; t[0][1]=0; t[0][2]=0; t[0][3]=0;
  t[1][0]=0; t[1][1]=1; t[1][2]=0; t[1][3]=0;
  t[2][0]=0; t[2][1]=0; t[2][2]=1; t[2][3]=0;
  t[3][0]=0; t[3][1]=0; t[3][2]=0; t[3][3]=1;
  // matrix storage changed with newprc7
  double tt[16];
  int i;
  for (i = 0; i < 16; i++) {
    //tt[i] = t[i/4][i%4];
    tt[i] = t[i%4][i/4];
  }
  

  double cyl_origin[3]; 
  double cyl_x_axis[3]; 
  double cyl_y_axis[3]; 

  // PRC cylinders have their axis in the z direction; base in x-y plane
  // ... we will rotate the x and y unit vectors to the appropriate locations
  XYZ xv = {1.,0.,0.}, yv = {0.,1.,0.}, zv = {0.,0.,1.};
  Normalise(&Ptip);

  double cosdelta = DotProduct(Ptip, zv);
  if (cosdelta > (1.-EPS)) {
    // do nothing - Ptip == zv
  } else if (cosdelta < (-1.+EPS)) {
    // flip x
    xv.x = -xv.x;
  } else {
    XYZ rvec = CrossProduct(Ptip, zv);
    double delta = -acos(cosdelta);
    xv = ArbitraryRotate(xv, delta, rvec);
    yv = ArbitraryRotate(yv, delta, rvec);
  }

  cyl_origin[0] = Porig.x;
  cyl_origin[1] = Porig.y;
  cyl_origin[2] = Porig.z;
  Normalise(&xv);
  Normalise(&yv);
  cyl_x_axis[0] = xv.x;
  cyl_x_axis[1] = xv.y;
  cyl_x_axis[2] = xv.z;
  cyl_y_axis[0] = yv.x;
  cyl_y_axis[1] = yv.y;
  cyl_y_axis[2] = yv.z;
  
  const double cyl_scale = 1;
  file.addCone(Crad,VectorLength(cone[wh].p1, cone[wh].p2),
		   mat,cyl_origin,cyl_x_axis,cyl_y_axis,cyl_scale,tt);
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}

void doPRCcylinder(oPRCFile &file, int wh) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif
  PRCmaterial mat(RGBAColour(0.0,0.0,0.0,1), 
		  RGBAColour(cone[wh].colour.r, cone[wh].colour.g, cone[wh].colour.b,1),
		  RGBAColour(0.0,0.0,0.0,1), RGBAColour(0.0,0.0,0.0,1),
		  1.0,0.0);
  XYZ Pcen, Ptip;
  //double Crad;

  // centre of cylinder
  Pcen = MidPoint(cone[wh].p1, cone[wh].p2);
  // unit vector from centre to one end
  Ptip = VectorSub(Pcen, cone[wh].p2);
  //Crad= cone[wh].r1;
  Normalise(&Ptip);
  
  // basic identify transformation matrix
  double t[4][4];
  t[0][0]=1; t[0][1]=0; t[0][2]=0; t[0][3]=0;
  t[1][0]=0; t[1][1]=1; t[1][2]=0; t[1][3]=0;
  t[2][0]=0; t[2][1]=0; t[2][2]=1; t[2][3]=0;
  t[3][0]=0; t[3][1]=0; t[3][2]=0; t[3][3]=1;
  // matrix storage changed with newprc7
  double tt[16];
  int i;
  for (i = 0; i < 16; i++) {
    //tt[i] = t[i/4][i%4];
    tt[i] = t[i%4][i/4];
  }

  double cyl_origin[3]; 
  double cyl_x_axis[3]; 
  double cyl_y_axis[3]; 

  // PRC cylinders have their axis in the z direction; base in x-y plane
  // ... we will rotate the x and y unit vectors to the appropriate locations
  XYZ xv = {1.,0.,0.}, yv = {0.,1.,0.}, zv = {0.,0.,1.};
  Normalise(&Ptip);

  double cosdelta = DotProduct(Ptip, zv);
  if (cosdelta > (1.-EPS)) {
    // do nothing - Ptip == zv
  } else if (cosdelta < (-1.+EPS)) {
    // flip x
    xv.x = -xv.x;
  } else {
    XYZ rvec = CrossProduct(Ptip, zv);
    double delta = -acos(cosdelta);
    xv = ArbitraryRotate(xv, delta, rvec);
    yv = ArbitraryRotate(yv, delta, rvec);
  }

  cyl_origin[0] = cone[wh].p1.x;
  cyl_origin[1] = cone[wh].p1.y;
  cyl_origin[2] = cone[wh].p1.z;
  Normalise(&xv);
  Normalise(&yv);
  cyl_x_axis[0] = xv.x;
  cyl_x_axis[1] = xv.y;
  cyl_x_axis[2] = xv.z;
  cyl_y_axis[0] = yv.x;
  cyl_y_axis[1] = yv.y;
  cyl_y_axis[2] = yv.z;
  
  const double cyl_scale = 1;
  file.addCylinder(cone[wh].r1,VectorLength(cone[wh].p1, cone[wh].p2),
		   mat,cyl_origin,cyl_x_axis,cyl_y_axis,cyl_scale,tt);
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}


void doPRClabel(oPRCFile &file, int wh) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif
  int lbi = wh;
  int nlinelist;
  XYZ linelist[300*MAXLABELLEN];

  // internal S2PLOT function to create line segments for a character string
  CreateLabelVector(label[lbi].s, label[lbi].p, label[lbi].right,
		    label[lbi].up, linelist, &nlinelist);

  uint32_t nP = nlinelist;
  double P[nP][3];
  for (uint32_t i = 0; i < nP; i++) {
    P[i][0] = linelist[i].x;
    P[i][1] = linelist[i].y;
    P[i][2] = linelist[i].z;
  }
  uint32_t nI = nP / 2 * 3;
  uint32_t PPI[nI];
  for (uint32_t i = 0; i < nP; i+=2) {
    PPI[i/2*3] = 2; // how many points this segment?
    PPI[i/2*3+1] = i;
    PPI[i/2*3+2] = i+1;
  }
  file.addLines(nP, P, nI, PPI, RGBAColour(label[lbi].colour.r, label[lbi].colour.g, label[lbi].colour.b),
		1.0, false, 0, NULL, 0, NULL);
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}

void doPRCtexmesh(oPRCFile &file, int wh) {
  // fetch the material
  unsigned int texid = texmesh[wh].texid;
  int picture_width, picture_height;
  ss2gt(texid, &picture_width, &picture_height);
  PRCmaterial loc_M = PRCmaterials[texid];

  loc_M.diffuse = RGBAColour(0,0,0);
  loc_M.specular = RGBAColour(0.15,0.15,0.15);
  loc_M.ambient = RGBAColour(0,0,0);
  loc_M.emissive = RGBAColour(1,1,1);
  loc_M.shininess = 0.4; // bigger values -> more focussed shininess

  //PRCmaterial loc_M = BASE_MATERIAL;
  //loc_M.diffuse = average3rgba(_f3_C[(_f3_TRI[3*currp+0])], 
  //			       _f3_C[(_f3_TRI[3*currp+1])],
  //		       _f3_C[(_f3_TRI[3*currp+2])]);

  PRCoptions grpopt;
  grpopt.no_break = true;
  grpopt.do_break = false;
  grpopt.tess = true;
  grpopt.closed = true;
  
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group, &grpopt/*, tt*/);

  int i;
  int np = texmesh[wh].nverts;
  double *loc_f3_P = (double *)malloc(np * 3 * 3 * sizeof(double));
  for (i = 0; i < np; i++) {
    loc_f3_P[i*3+0] = texmesh[wh].verts[i].x;
    loc_f3_P[i*3+1] = texmesh[wh].verts[i].y;
    loc_f3_P[i*3+2] = texmesh[wh].verts[i].z;
  }

  int ntri = texmesh[wh].nfacets;
  uint32_t *loc_f3_TRI = (uint32_t *)malloc(ntri * 3 * sizeof(uint32_t));
  for (i = 0; i < ntri; i++) {
    loc_f3_TRI[i*3+0] = texmesh[wh].facets[i*3+0];
    loc_f3_TRI[i*3+1] = texmesh[wh].facets[i*3+1];
    loc_f3_TRI[i*3+2] = texmesh[wh].facets[i*3+2];
  }

  int nnrs = texmesh[wh].nnorms;
  double *loc_f3_N = (double *)malloc(nnrs * 3 * 3 * sizeof(double));
  for (i = 0; i < nnrs; i++) {
    loc_f3_N[i*3+0] = texmesh[wh].norms[i].x;
    loc_f3_N[i*3+1] = texmesh[wh].norms[i].y;
    loc_f3_N[i*3+2] = texmesh[wh].norms[i].z;
  }
  

  int ntcs = texmesh[wh].nvtcs;
  double *loc_vtcs = (double *)malloc(ntcs * 2 * sizeof(double));
  for (i = 0; i < ntcs; i++) {
    loc_vtcs[i*2+0] = texmesh[wh].vtcs[i].x;
    loc_vtcs[i*2+1] = texmesh[wh].vtcs[i].y;
  }

  uint32_t *loc_vtcs_IDX = (uint32_t *)malloc(ntri * 3 * sizeof(uint32_t));
  for (i = 0; i < ntri; i++) {
    loc_vtcs_IDX[i*3+0] = texmesh[wh].facets_vtcs[i*3+0];
    loc_vtcs_IDX[i*3+1] = texmesh[wh].facets_vtcs[i*3+1];
    loc_vtcs_IDX[i*3+2] = texmesh[wh].facets_vtcs[i*3+2];
  }
    

  file.addTriangles(texmesh[wh].nverts, (const double (*)[3])loc_f3_P, 
		    texmesh[wh].nfacets, (const uint32_t (*)[3])loc_f3_TRI,
		    loc_M, 

		    // normals
		    nnrs, (const double (*)[3])loc_f3_N, (const uint32_t (*)[3])loc_f3_TRI,
		    //0, NULL, NULL,

		    // texture coordinates
		    ntcs, (const double (*)[2])loc_vtcs, (const uint32_t (*)[3])loc_vtcs_IDX,
		    //0, NULL, NULL,

		    // vertex colours
		    0, NULL, NULL, 

		    // materials
		    0, NULL, NULL);
  file.endgroup();

  fprintf(stderr, "nverts = %d, nnorms = %d, nvtcs = %d, nfacets = %d\n",
	  texmesh[wh].nverts, texmesh[wh].nnorms, texmesh[wh].nvtcs,
	  texmesh[wh].nfacets);
  
  free(loc_vtcs_IDX);
  free(loc_vtcs);
  free(loc_f3_N);
  free(loc_f3_TRI);
  free(loc_f3_P);
  
}

void doPRCbboard(oPRCFile &file, int wh) {
  // fetch the material
  unsigned int texid = bboard[wh].texid;
  int picture_width, picture_height;
  ss2gt(texid, &picture_width, &picture_height);
  PRCmaterial mat = PRCmaterials[texid];
  float dw = 0.5 * 1.0 / (float)picture_width;
  float dh = 0.5 * 1.0 / (float)picture_height;
  dw *= 3.0;
  dh *= 3.0;

  double t[4][4];
  t[0][0]=1; t[0][1]=0; t[0][2]=0; t[0][3]=bboard[wh].p.x;
  t[1][0]=0; t[1][1]=1; t[1][2]=0; t[1][3]=bboard[wh].p.y;
  t[2][0]=0; t[2][1]=0; t[2][2]=1; t[2][3]=bboard[wh].p.z;
  t[3][0]=0; t[3][1]=0; t[3][2]=0; t[3][3]=1;
  // matrix storage changed with newprc7
  double tt[16];
  int i;
  for (i = 0; i < 16; i++) {
    //tt[i] = t[i/4][i%4];
    tt[i] = t[i%4][i/4];
  }

  PRCoptions grpopt;
  grpopt.no_break = true;
  grpopt.do_break = false;
  grpopt.tess = true;
  grpopt.closed = true;
  
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group, &grpopt, tt);

  double P[4][3];
  for (int j = 0; j < 4; j++) {
    float xoff = 0, yoff = 0;
    switch (j) {
    case 0:
      xoff = +1;
      yoff = -1;
      
      xoff = -1;
      yoff = +1;
      break;
    case 1:
      xoff = +1;
      yoff = +1;
      break;
    case 2:
      xoff = -1;
      yoff = +1;

      xoff = +1;
      yoff = -1;
      break;
    case 3:
      xoff = -1;
      yoff = -1;
      break;
    }
    xoff *= 7. * bboard[wh].size * bboard[wh].aspect;
    yoff *= 7. * bboard[wh].size;
    //P[j][0] = xoff + bboard[wh].offset.x;
    //P[j][1] = yoff + bboard[wh].offset.y;
    float x1 = xoff + bboard[wh].offset.x;
    float y1 = yoff + bboard[wh].offset.y;
    P[j][0] = x1 * cos(-bboard[wh].pa) - y1 * sin(-bboard[wh].pa);
    P[j][1] = x1 * sin(-bboard[wh].pa) + y1 * cos(-bboard[wh].pa);
    P[j][2] = 0.0;
  }

  uint32_t PPI[1][4] = {{0,3,2,1}};
  double T[4][2] = { {dw,1.0-dh}, {1.0-dw,1.0-dh}, {1.0-dw,dh}, {dw,dh} };
  uint32_t TI[1][4] = {{0,3,2,1}};

  uint32_t single_idx[4] = {0,0,0,0};
  mat.diffuse = RGBAColour(bboard[wh].col.r, bboard[wh].col.g, bboard[wh].col.b);
  file.addQuads(4 /*points*/,P,1 /* quads */,PPI,mat,0,NULL,NULL,4,T,TI,
		//0,NULL,NULL,
		1, &mat.diffuse, (uint32_t (*)[4])&single_idx,
		0,NULL,NULL);
  file.endgroup();
}


#if defined(HPDF)
HPDF_Dict HPDF_LoadJSFromFile  (HPDF_Doc     pdf, const char  *filename) {
  HPDF_Stream js_data;
  HPDF_Dict js = NULL;
  
  //	HPDF_PTRACE ((" HPDF_LoadJSFromFile\n"));
  
  if (!HPDF_HasDoc (pdf)) {
    return NULL;
  }
  
  /* create file stream */
  js_data = HPDF_FileReader_New (pdf->mmgr, filename);
  
  if (!HPDF_Stream_Validate (js_data)) {
    HPDF_Stream_Free (js_data);
    return NULL;
  }
  
  js = HPDF_DictStream_New (pdf->mmgr, pdf->xref);
  if (!js) {
    HPDF_Stream_Free (js_data);
    return NULL;
  }
  
  js->header.obj_class |= HPDF_OSUBCLASS_XOBJECT;
  
  /* add required elements */
  js->filter = HPDF_STREAM_FILTER_NONE;
  
  for (;;) {
    HPDF_BYTE buf[HPDF_STREAM_BUF_SIZ];
    HPDF_UINT len = HPDF_STREAM_BUF_SIZ;
    HPDF_STATUS ret = HPDF_Stream_Read (js_data, buf, &len);
    
    if (ret != HPDF_OK) {
      if (ret == HPDF_STREAM_EOF) {
	if (len > 0) {
	  ret = HPDF_Stream_Write (js->stream, buf, len);
	  if (ret != HPDF_OK) {
	    HPDF_Stream_Free (js_data);
	    HPDF_Dict_Free(js);
	    return NULL;
	  }
	}
	break;
      } else {
	HPDF_Stream_Free (js_data);
	HPDF_Dict_Free(js);
	return NULL;
      }
    }
    
    if (HPDF_Stream_Write (js->stream, buf, len) != HPDF_OK) {
      HPDF_Stream_Free (js_data);
      HPDF_Dict_Free(js);
      return NULL;
    }
  }
  
  /* destroy file stream */
  HPDF_Stream_Free (js_data);
  
  if (!js) {
    HPDF_CheckError (&pdf->error);
  }
  return js;
}
#endif

#define KEPSI 0.001

  int _li_np;
double *_li_P;
  int _li_nli;
  uint32_t *_li_LI;
  RGBAColour *_li_C;

  void _clear_li_mesh(void) {
    if (_li_P) {
      free(_li_P);
      _li_P = NULL;
    }
    if (_li_C) {
      free(_li_C);
      _li_C = NULL;
    }
    _li_np = 0;
    if (_li_LI) {
      free(_li_LI);
      _li_LI = NULL;
    }
    _li_nli = 0;
  }

  void _prealloc_li_mesh(int nli) {
    // to prevent continual realloc (slow) just alloc the max required in one shot
    _li_LI = (uint32_t *)malloc(nli * 3 * sizeof(uint32_t)); // each segment is: (2, idx1, idx2)
    _li_P = (double *)malloc(nli * 2 * 3 * sizeof(double));
    _li_C = (RGBAColour *)malloc(nli * sizeof(RGBAColour));
  }

  void _accumulate_li_mesh(int which) {

    // this guy is:
    // line[which].p[0,1].{x,y,z}
    // line[which].colour[0,1].{r,g,b}

    // 1. loop over 2 vertices, if no matches, add new one, record indices in 
    //    new space in _f3_LI
    int vi, ki;
    double x, y, z;

    // 2 points this segment
    _li_LI[3*_li_nli+0] = 2;

    for (vi = 0; vi < 2; vi++) {
      x = line[which].p[vi].x;
      y = line[which].p[vi].y;
      z = line[which].p[vi].z;
      for (ki = _li_np-1; ki >= 0; ki--) {
	if (fabs(_li_P[3*ki+0]-x) > fabs(_li_P[3*ki+0]+x) * KEPSI ||
	    fabs(_li_P[3*ki+1]-y) > fabs(_li_P[3*ki+1]+y) * KEPSI ||
	    fabs(_li_P[3*ki+2]-z) > fabs(_li_P[3*ki+2]+z) * KEPSI) {
	  // NB SHOULD TEST COLOUR TOO !!!
	  continue;
	}
	// found
	break;
      }
      if (ki < 0) {
	ki = _li_np;
      }
      _li_LI[3*_li_nli+1+vi] = ki;
      if (ki >= _li_np) {
	// this vertex is unique, make space and copy it in
	_li_P[3*_li_np+0] = x;
	_li_P[3*_li_np+1] = y;
	_li_P[3*_li_np+2] = z;
	
	_li_np++;
      }
    }
    
    _li_C[_li_nli] = RGBAColour(0.5 * (line[which].colour[0].r + line[which].colour[1].r), 
			       0.5 * (line[which].colour[0].g + line[which].colour[1].g),
			       0.5 * (line[which].colour[0].b + line[which].colour[1].b));
	
    _li_nli++;
    
    //fprintf(stderr, "line: nP = %d, nli * 2 = %d\n", _li_np, _li_nli*2);
    
  }
  
  void _emit_li_mesh(oPRCFile &file) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
    sprintf(group, "LINE%d", _s2x_prccount++);
    file.begingroup(group);
#endif

#if (0) // grey version
    file.addLines(_li_np, (const double (*)[3])_li_P, _li_nli*3/2, _li_LI, RGBAColour(0.5, 0.5, 0.5),
    	  1.0, false, 
    		  0, NULL,
    		  0, NULL);

#else

    int nC = _li_nli;
    int nCI = _li_nli;
    uint32_t *CI = (uint32_t*)malloc(nCI * sizeof(uint32_t));
    int i;
    for (i = 0; i < _li_nli; i++) {
      CI[i] = i;
    }

    // version has a subtle bug that shows when *other* geometry is added to file...
    file.addLines(_li_np, (const double (*)[3])_li_P, _li_nli*3, _li_LI, RGBAColour(0.5, 0.5, 0.5),
		  1.0, true, 
		  nC, _li_C,
		  nCI, CI);


    free(CI);
#endif

#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
    file.endgroup();
#endif
  }



int _f3_np;
double *_f3_P;
int _f3_ntri;
double *_f3_N;
uint32_t *_f3_TRI;
RGBAColour *_f3_C;

void _clear_f3_mesh(void) {
  if (_f3_P) {
    free(_f3_P);
    _f3_P = NULL;
  }
  if (_f3_N) {
    free(_f3_N);
    _f3_N = NULL;
  }
  if (_f3_C) {
    free(_f3_C);
    _f3_C = NULL;
  }
  _f3_np = 0;
  if (_f3_TRI) {
    free(_f3_TRI);
    _f3_TRI = NULL;
  }	     
  _f3_ntri = 0;
}

void _prealloc_f3_mesh(int ntri) {
  // to prevent necessary continual realloc (slow),
  // just alloc the max required in one shot
  _f3_TRI = (uint32_t *)malloc(ntri * 3 * sizeof(uint32_t));
  _f3_P = (double *)malloc(ntri * 3 * 3 * sizeof(double));
  _f3_N = (double *)malloc(ntri * 3 * 3 * sizeof(double));
  _f3_C = (RGBAColour *)malloc(ntri * 3 * sizeof(RGBAColour));
}

#define CEPSI 0.05
// < changed to <= to support components that are 0
#define RGBAnearlyEqual(a,b) (((a.R-b.R)<=(a.R+b.R)*CEPSI) && ((a.G-b.G)<=(a.G+b.G)*CEPSI) && ((a.B-b.B)<=(a.B+b.B)*CEPSI))

void _accumulate_f3_mesh(int which) {

  // this guy is:
  // face3[which].p[0,1,2].x,y,z
  // face3[which].colour[0,1,2].r,g,b

  // 0. create new space in _f3_TRI
  //_f3_TRI = (uint32_t **)realloc(_f3_TRI, (_f3_ntri+1)*sizeof(uint32_t *));
  //_f3_TRI[_f3_ntri] = (uint32_t *)malloc(3 * sizeof(uint32_t));
  
  // 1. loop over 3 vertices - if no matches, add new one, record indices in 
  //    new space in _f3_TRI
  int vi, ki;
  double x, y, z, nx, ny, nz;
  //float r, g, b;
  RGBAColour vxcol;
  for (vi = 0; vi < 3; vi++) {
    x = face3[which].p[vi].x;
    y = face3[which].p[vi].y;
    z = face3[which].p[vi].z;
    nx = face3[which].n[vi].x;
    ny = face3[which].n[vi].y;
    nz = face3[which].n[vi].z;
    vxcol.R = face3[which].colour[vi].r;
    vxcol.G = face3[which].colour[vi].g;
    vxcol.B = face3[which].colour[vi].b;
    for (ki = _f3_np-1; ki >= 0; ki--) {
      if (fabs(_f3_P[3*ki+0]-x) > fabs(_f3_P[3*ki+0]+x) * KEPSI ||
	  fabs(_f3_P[3*ki+1]-y) > fabs(_f3_P[3*ki+1]+y) * KEPSI ||
	  fabs(_f3_P[3*ki+2]-z) > fabs(_f3_P[3*ki+2]+z) * KEPSI ||
	  !RGBAnearlyEqual(vxcol, _f3_C[ki])) {
	//	  fabs(_f3_C[ki].R-r) > fabs(_f3_C[ki].R+r) * KEPSI ||
	//fabs(_f3_C[ki].G-g) > fabs(_f3_C[ki].G+g) * KEPSI ||
	//fabs(_f3_C[ki].B-b) > fabs(_f3_C[ki].B+b) * KEPSI ) {
	/* CANNOT COMPARE NORMALS: NORMALS will be DIFFERENT FOR DIFFERENT FACETS!!!
	   fabs(_f3_N[3*ki+0]-nx) > fabs(_f3_N[3*ki+0]+nx) * KEPSI ||
	   fabs(_f3_N[3*ki+1]-ny) > fabs(_f3_N[3*ki+1]+ny) * KEPSI ||
	   fabs(_f3_N[3*ki+2]-nz) > fabs(_f3_N[3*ki+2]+nz) * KEPSI ) { */
	continue;
      }
      // found
      break;
    }
    if (ki < 0) {
      ki = _f3_np;
    }
    _f3_TRI[3*_f3_ntri+vi] = ki;
    if (ki >= _f3_np) {
      // this vertex is unique, make space and copy it in
      _f3_P[3*_f3_np+0] = x;
      _f3_P[3*_f3_np+1] = y;
      _f3_P[3*_f3_np+2] = z;
      _f3_N[3*_f3_np+0] = nx;
      _f3_N[3*_f3_np+1] = ny;
      _f3_N[3*_f3_np+2] = nz;
      
      // DGB IS THIS A BUG?  SHOULD IT BE [vi] ???
      _f3_C[_f3_np] = RGBAColour(face3[which].colour[vi].r, face3[which].colour[vi].g,
				 face3[which].colour[vi].b);

      _f3_np++;
    }
  }

  _f3_ntri++;

}

void _emit_f3_mesh(oPRCFile &file) {
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif

  // 1. if nC, C[], CI[][3] given in addTriangles: shininess/normals irrelevant and
  //    only materialBase "diff" bit relevant.  Lighting mode in Reader has no 
  //    effect.
  //
  // 2. bulk single colour with shininess can be achieved by NOT giving nc,C,CI to
  //    addTriangles, and instead just giving a global material, with shininess etc.
  //    And now lighting mode in Reader works, but original colour of mesh lost.
  //
  // 3. so to get shininess (and perhaps opacity?) with correct shadings we should
  //    try to create an array of materials, one per facet... this is what we do.
  //     BUT IT DOESN'T WORK!
  //
  // 4. the next try would be to make several calls to addTriangles, once per 
  //    material we want to use.  Make one sufficiently-large temp space for 
  //    vertices and facets, and a "done" array for each facet, and work through
  //    copying as required.  No drama that this is expensive as it's a once off.
  //    T.B.D. ... ^^^ 

#if (1)

  // to prevent necessary continual realloc (slow),
  // just prealloc the max required in one shot
  uint32_t *loc_f3_TRI = (uint32_t *)malloc(_f3_ntri * 3 * sizeof(uint32_t));
  double *loc_f3_P = (double *)malloc(_f3_np * 3 * 3 * sizeof(double));
  double *loc_f3_N = (double *)malloc(_f3_np * 3 * 3 * sizeof(double));

  int i, idx, cp, vi;

  // store whether we've outputted this facet yet
  unsigned char *facet_done = (unsigned char *)malloc(_f3_ntri * sizeof(unsigned char));
  for (i = 0; i < _f3_ntri; i++) {
    facet_done[i] = 0;
  }

  // store a mapping of all vertex indices to local vertex indices
  int *vertex_map = (int *)malloc(_f3_np * sizeof(int));

  // currp is our current starting point in the complete facet list
  int currp = 0;
  int loc_ntri, loc_np;
  int total_emit_tri = 0, total_emit_p = 0;
  while (currp < _f3_ntri) {
    
    PRCmaterial loc_M = BASE_MATERIAL;
    loc_M.diffuse = average3rgba(_f3_C[(_f3_TRI[3*currp+0])], 
				 _f3_C[(_f3_TRI[3*currp+1])],
				 _f3_C[(_f3_TRI[3*currp+2])]);
    
    loc_ntri = 0;
    loc_np = 0;
    idx = currp;

    // reset vertex map
    for (i = 0; i < _f3_np; i++) {
      vertex_map[i] = -1; // not mapped (= not written yet)
    }

    while (idx < _f3_ntri) {
      
      // if average colour of idx facet matches average colour of currp facet
      if ((idx == currp) ||
	  RGBAnearlyEqual(loc_M.diffuse,  average3rgba(_f3_C[(_f3_TRI[3*idx+0])], 
						       _f3_C[(_f3_TRI[3*idx+1])],
						       _f3_C[(_f3_TRI[3*idx+2])]))) {
	// loop over vertices
	for (vi = 0; vi < 3; vi++) {
	  // have we emitted this vertex yet?  If not, do so.
	  if (vertex_map[_f3_TRI[3*idx+vi]] < 0) {
	    for (cp = 0; cp < 3; cp++) {
	      loc_f3_P[3*loc_np+cp] = _f3_P[3*_f3_TRI[idx*3+vi]+cp];
	      loc_f3_N[3*loc_np+cp] = _f3_N[3*_f3_TRI[idx*3+vi]+cp];
	      // ditto normal
	    }
	    vertex_map[_f3_TRI[idx*3+vi]] = loc_np;
	    loc_np++;
	  }
	  loc_f3_TRI[loc_ntri * 3 + vi] = vertex_map[_f3_TRI[idx*3+vi]];
	}
	facet_done[idx] = 1;	  
	loc_ntri++;
      }

      idx++;
      while ((idx < _f3_ntri) && facet_done[idx]) {
	idx++;
      }

    }
    
    // emit triangle/s
    file.addTriangles(loc_np, (const double (*)[3])loc_f3_P, 
		      loc_ntri, (const uint32_t (*)[3])loc_f3_TRI, 
		      loc_M, 
		      
		      // normals
		      0, NULL, NULL, 
		      //loc_np, (const double (*)[3])loc_f3_N, (const uint32_t (*)[3])loc_f3_TRI,
		      
		      // texture coords
		      0, NULL, NULL, 
		      
		      // vertex colours
		      0, NULL, NULL,
		      
		      // materials
		      0, NULL, NULL
		      );
    
    total_emit_tri +=loc_ntri;
    total_emit_p += loc_np;
    
    // update currp
    while ((currp < _f3_ntri) && facet_done[currp]) {
      currp++;
    }
  }
  
  free(vertex_map);
  free(facet_done);
  free(loc_f3_P);
  free(loc_f3_TRI);
  free(loc_f3_N);

  //fprintf(stderr, "EMITTED %d TOTAL TRIANGLES, and %d TOTAL VERTICES\n", total_emit_tri,
  //	  total_emit_p);

#else
  
  PRCmaterial *M = new PRCmaterial[_f3_ntri]; // crash if use malloc rather than new :-)
  uint32_t *MI = new uint32_t[_f3_ntri];
  int i;
  for (i = 0; i < _f3_ntri; i++) {
    M[i] = BASE_MATERIAL;
    M[i].diffuse = average3rgba(_f3_C[(_f3_TRI[3*i+0])], 
				_f3_C[(_f3_TRI[3*i+1])],
				_f3_C[(_f3_TRI[3*i+2])]);
    MI[i] = i;
  }

  file.addTriangles(_f3_np, (const double (*)[3])_f3_P, 
		    _f3_ntri, (const uint32_t (*)[3])_f3_TRI, 
		    BASE_MATERIAL, 
		    //M[i],

		    // normals
		    //0, NULL, NULL, 
		    _f3_np, (const double (*)[3])_f3_N, (const uint32_t (*)[3])_f3_TRI,

		    // texture coords
		    0, NULL, NULL, 

		    // vertex colours
		    //_f3_np, _f3_C, (const uint32_t (*)[3])_f3_TRI, 
		    0, NULL, NULL,

		    // materials
		    //0, NULL, NULL
		    _f3_ntri, M, MI
		    );

#endif

#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}






int _f3a_np;
double *_f3a_P;
int _f3a_ntri;
double *_f3a_N;
uint32_t *_f3a_TRI;
RGBAColour *_f3a_C;

void _clear_f3a_mesh(void) {
  if (_f3a_P) {
    free(_f3a_P);
    _f3a_P = NULL;
  }
  if (_f3a_N) {
    free(_f3a_N);
    _f3a_N = NULL;
  }
  if (_f3a_C) {
    free(_f3a_C);
    _f3a_C = NULL;
  }
  _f3a_np = 0;
  if (_f3a_TRI) {
    free(_f3a_TRI);
    _f3a_TRI = NULL;
  }	     
  _f3a_ntri = 0;
}

void _prealloc_f3a_mesh(int ntri) {
  // to prevent necessary continual realloc (slow),
  // just alloc the max required in one shot
  _f3a_TRI = (uint32_t *)malloc(ntri * 3 * sizeof(uint32_t));
  _f3a_P = (double *)malloc(ntri * 3 * 3 * sizeof(double));
  _f3a_N = (double *)malloc(ntri * 3 * 3 * sizeof(double));
  _f3a_C = (RGBAColour *)malloc(ntri*3*sizeof(RGBAColour));
}

void _accumulate_f3a_mesh(int which) {

  // this guy is:
  // face3a[which].p[0,1,2].x,y,z
  // face3a[which].colour[0,1,2].r,g,b

  // 0. create new space in _f3a_TRI
  //- _f3a_TRI = (uint32_t **)realloc(_f3a_TRI, (_f3a_ntri+1)*sizeof(uint32_t *));
  //_f3a_TRI[_f3a_ntri] = (uint32_t *)malloc(3 * sizeof(uint32_t));
  
  // 1. loop over 3 vertices - if no matches, add new one, record indices in 
  //    new space in _f3a_TRI
  int vi, ki;
  double x, y, z, nx, ny, nz;
  float r, g, b;
  for (vi = 0; vi < 3; vi++) {
    x = face3a[which].p[vi].x;
    y = face3a[which].p[vi].y;
    z = face3a[which].p[vi].z;
    nx = face3a[which].n[vi].x;
    ny = face3a[which].n[vi].y;
    nz = face3a[which].n[vi].z;
    r = face3a[which].colour[vi].r;
    g = face3a[which].colour[vi].g;
    b = face3a[which].colour[vi].b;
    //= for (ki = 0; ki < _f3a_np; ki++) {
    for (ki = _f3a_np-1; ki >= 0; ki--) { // loop from most recent - most likely to be a match
      if (fabs(_f3a_P[3*ki+0]-x) > fabs(_f3a_P[3*ki+0]+x) * KEPSI ||
	  fabs(_f3a_P[3*ki+1]-y) > fabs(_f3a_P[3*ki+1]+y) * KEPSI ||
	  fabs(_f3a_P[3*ki+2]-z) > fabs(_f3a_P[3*ki+2]+z) * KEPSI ||
	  fabs(_f3a_C[ki].R-r) > fabs(_f3a_C[ki].R+r) * KEPSI ||
	  fabs(_f3a_C[ki].G-g) > fabs(_f3a_C[ki].G+g) * KEPSI ||
	  fabs(_f3a_C[ki].B-b) > fabs(_f3a_C[ki].B+b) * KEPSI ) {
	/* CANNOT COMPARE NORMALS: ADJOINING FACETS WILL HAVE DIFFERENT NORMALS!
	  fabs(_f3a_N[3*ki+0]-nx) > fabs(_f3a_N[3*ki+0]+nx) * KEPSI ||
	  fabs(_f3a_N[3*ki+1]-ny) > fabs(_f3a_N[3*ki+1]+ny) * KEPSI ||
	  fabs(_f3a_N[3*ki+2]-nz) > fabs(_f3a_N[3*ki+2]+nz) * KEPSI */
	continue;
      }
      // found
      break;
    }
    if (ki < 0) {
      ki = _f3a_np;
    }
    _f3a_TRI[3*_f3a_ntri+vi] = ki;
    if (ki >= _f3a_np) {
      // this vertex is unique, make space and copy it in
      _f3a_P[3*_f3a_np+0] = x;
      _f3a_P[3*_f3a_np+1] = y;
      _f3a_P[3*_f3a_np+2] = z;
      _f3a_N[3*_f3a_np+0] = nx;
      _f3a_N[3*_f3a_np+1] = ny;
      _f3a_N[3*_f3a_np+2] = nz;

      _f3a_C[_f3a_np] = RGBAColour(face3a[which].colour[vi].r, face3a[which].colour[vi].g,
				   face3a[which].colour[vi].b, face3a[which].alpha[vi]);

      _f3a_np++;
    }
  }

  _f3a_ntri++;

  //fprintf(stderr, "face3a: nP = %d, nTri*3 = %d\n", _f3a_np, _f3a_ntri*3);
}


void _emit_f3a_mesh(oPRCFile &file) {
  if (_f3a_ntri < 1) {
    fprintf(stderr, "UNEXPECTED STATE in _emit_f3a_mesh: _f3a_ntri = %d\n", _f3a_ntri);
    return;
  }
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  sprintf(group, "SHAPE%d", _s2x_prccount++);
  file.begingroup(group);
#endif
  // amb, diff, emiss, spec
    PRCmaterial materialBase = BASE_MATERIAL;
    int i = 0;
    materialBase.diffuse = average3rgba(_f3a_C[(_f3a_TRI[3*i+0])], 
					_f3a_C[(_f3a_TRI[3*i+1])],
					_f3a_C[(_f3a_TRI[3*i+2])]);
    //MI[i] = i;

    //PRCmaterial materialBase(RGBAColour(0.1,0.1,0.1,1),
    //			   RGBAColour(0.1,0.2,0.7,1),
    //			   RGBAColour(0.3,0.3,0.3,1),
    //			   RGBAColour(0.2,0.2,0.2,1),
    //			   1., 0.);
  file.addTriangles(_f3a_np, (const double (*)[3])_f3a_P, 
		    _f3a_ntri, (const uint32_t (*)[3])_f3a_TRI, 
		    materialBase, 

		    // 0, NULL, NULL, 
		    _f3a_np, (const double (*)[3])_f3a_N, (const uint32_t (*)[3])_f3a_TRI,

		    0, NULL, NULL, 

		    //_f3a_np, _f3a_C, (const uint32_t (*)[3])_f3a_TRI, 
		    0, NULL, NULL,

		    0, NULL, NULL);
#if !defined(PRC_SUPPRESS_SUBSTRUCTURE)
  file.endgroup();
#endif
}



// thank  you for reading s2prc.cc :-)
