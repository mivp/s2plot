/* s2geomviewer.c
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
 * $Id: s2geomviewer.c 5828 2012-11-15 00:13:35Z dbarnes $
 *
 */

#include "s2globals.h"
#include "geomviewer.h"
#if defined(BUILDING_S2PLOT)
#include "s2plot.h"
#endif
#include "s2privfn.h"
#include "bitmaplib.h"

#if defined(BUILDING_S2PLOT)
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#if !defined(S2SUNOS)
#include <sys/stat.h>
#endif
#endif

#if defined(S2DARWIN)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#if defined(BUILDING_S2PLOT)
void _glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
#endif

#if defined(BUILDING_VIEWER)
#include <sys/stat.h>
#endif

/*
   Set up the lighting environment
*/
void MakeLighting(void) {
  int i;
  double delta;
  GLfloat p[4];
  GLfloat white[4] = {1.0,1.0,1.0,1.0};
  GLfloat black[4] = {0.0,0.0,0.0,1.0};
  int deflightlist[8] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,
			 GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};
  
  /* 
     Turn off all the lights 
     Leave the ambient component up to the global setting
     Set the specular component appropriately
  */
  for (i=0;i<8;i++) {
    glDisable(deflightlist[i]);
    glLightfv(deflightlist[i],GL_AMBIENT,black);
    switch (options.rendermode) {
    case WIREFRAME:
    case SHADE_DIFFUSE:
      glLightfv(deflightlist[i],GL_SPECULAR,black);
      break;
    case SHADE_SPECULAR:
      glLightfv(deflightlist[i],GL_SPECULAR,white);
      break;
    }
  }
  
  // Lighting options, specified in the lightform 
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,options.localviewer);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,options.twosided);
  
  // Rendering mode specific settings 
  switch (options.rendermode) {
  case WIREFRAME:
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,white);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_LIGHTING);
    break;
  case SHADE_FLAT:
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,white);
    glShadeModel(GL_FLAT);
    glDisable(GL_LIGHTING);
    break;
  case SHADE_DIFFUSE:
  case SHADE_SPECULAR:
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,globalambient);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    break;
  }
  
  /*
    Create a default lighting environment if no lights were
    specified in the scene. Two lights are placed at the two
    extremities of the model, they are positional lights.
  */
  if (nlight < 0) {
    delta = VectorLength(pmin,pmax) / 20;
    for (i=0;i<defaultlights;i++) {
      if (deflightpos[i].x == 0)
	p[0] = pmin.x - delta;
      else
	p[0] = pmax.x + delta;
      if (deflightpos[i].y == 0)
	p[1] = pmin.y - delta;
      else
	p[1] = pmax.y + delta;
      if (deflightpos[i].z == 0)
	p[2] = pmin.z - delta;
      else
	p[2] = pmax.z + delta;
      p[3] = 1.0;
      glLightfv(deflightlist[i],GL_POSITION,p);
      glLightfv(deflightlist[i],GL_DIFFUSE,white);
      glEnable(deflightlist[i]);
    }
  } else {
    for (i=0;i<nlight;i++) {
      glLightfv(deflightlist[i],GL_POSITION,lights[i].p);
      glLightfv(deflightlist[i],GL_DIFFUSE,lights[i].c);
      glEnable(deflightlist[i]);
    }
  }
}

/*
	Set up the material attributes
*/
void MakeMaterial(void)
{
  GLfloat white[4] = {1.0,1.0,1.0,1.0};
  GLfloat shin[1]  = {0.0};
  
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emission);
  
  switch (options.rendermode) {
  case WIREFRAME:
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shin);
    break;
  case SHADE_DIFFUSE:
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shin);
    break;
  case SHADE_SPECULAR:
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specularcolour);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
    break;
  }
}

/*
	Create the projection matrix
	Support perspective and parallel.
	eye is left or right for stereo
*/
void CreateProjection(int eye) {
  double dist,ratio,radians=0,wd2,ndfl;
  double left,right,top,bottom,near,far;
  
  // Sort out near and far cutting plane for ultra stereo
#if defined(BUILDING_S2PLOT)
  near = VectorLength(pmin,pmax) / 10;
  //near *= 0.01;
  near /= _s2_nearfar_expand;
  far = MAX(camera.focallength,VectorLength(pmin,pmax)) * 20. * 
    _s2_nearfar_expand;
  if (options.stereo != NOSTEREO) {
    near = camera.focallength / (5. * _s2_nearfar_expand);
  }
#else
  near = VectorLength(pmin,pmax) / 100;
  far  = MAX(camera.focallength,VectorLength(pmin,pmax)) * 20;
  if (options.stereo != NOSTEREO)
    near = camera.focallength / 5;
#endif
  
  // Window width to height ratio 
  ratio = options.screenwidth / (double)(options.screenheight);
#if defined(BUILDING_S2PLOT)
  //#if defined(FIXME)
  ratio *= (_s2_panels[_s2_activepanel].x2 - _s2_panels[_s2_activepanel].x1) / (_s2_panels[_s2_activepanel].y2 - _s2_panels[_s2_activepanel].y1);
  //#endif
#endif
  if ((options.stereo == DUALSTEREO) || (options.stereo == WDUALSTEREO)) {
    ratio /= 2;
  } else if (options.stereo == TRIOSTEREO) {
    ratio /= 3;
  }
  radians = DTOR * camera.aperture / 2;
  
  switch (options.projectiontype) {
  case PERSPECTIVE:
    wd2     = near * tan(radians);
    ndfl    = near / camera.focallength;
    if (eye == 'l') {
      left  = - ratio * wd2 + 0.5 * camera.eyesep * ndfl;
      right =   ratio * wd2 + 0.5 * camera.eyesep * ndfl;
    } else if (eye == 'r') {
      left  = - ratio * wd2 - 0.5 * camera.eyesep * ndfl;
      right =   ratio * wd2 - 0.5 * camera.eyesep * ndfl;
    } else {
      left  = - ratio * wd2;
      right =   ratio * wd2;
    }
    top    =   wd2;
    bottom = - wd2;
    glFrustum(left,right,bottom,top,near,far);
    break;
  case ORTHOGRAPHIC:
    dist = VectorLength(camera.vp,camera.pr);
    left   = - dist * ratio * tan(radians);
    right  =   dist * ratio * tan(radians);
    bottom = - dist * tan(radians);
    top    =   dist * tan(radians);
    glOrtho(left,right,bottom,top,near,far);
    break;
  }

#if defined(BUILDING_S2PLOT)
  _s2_save_near = near;
  _s2_save_far = far;
#endif
}

/* draw a frame around the active panel */
void _s2priv_drawActiveFrame(int active) {
#define AF_BORDER ((int)(_s2_panelframewidth/2))
  /* draw a frame around the current panel */
  int xw = (_s2_panels[_s2_activepanel].x2 - _s2_panels[_s2_activepanel].x1) * (float)options.screenwidth + 0.5;
  int yw = (_s2_panels[_s2_activepanel].y2 - _s2_panels[_s2_activepanel].y1) * (float)options.screenwidth + 0.5;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, xw, 0, yw, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_LIGHTING);
  
  glLineWidth(_s2_panelframewidth);
  
  glBegin(GL_LINE_STRIP);
  if (active) {
    _glColor4f(_s2_activepanelframecolour.r, 
	       _s2_activepanelframecolour.g,
	       _s2_activepanelframecolour.b, 1.0);
  } else {
    _glColor4f(_s2_panelframecolour.r, 
	       _s2_panelframecolour.g,
	       _s2_panelframecolour.b, 1.0);
  }
  glVertex3f(0 + AF_BORDER, 0 + AF_BORDER, 0);
  glVertex3f(0 + AF_BORDER, yw - 1 - AF_BORDER, 0);
  glVertex3f(xw - 1 - AF_BORDER, yw - 1 - AF_BORDER, 0);
  glVertex3f(xw - 1 - AF_BORDER, AF_BORDER, 0);
  glVertex3f(0 + AF_BORDER, 0 + AF_BORDER, 0);
  glEnd();

  glEnable(GL_LIGHTING);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

void _s2_fadeinout(void) {
  static double starttime;
  float trans = 0.;

  // normal operation:
  if (_s2_fadestatus == 2) {
    return;
  }

  // commence fade-in
  else if (_s2_fadestatus == 0) {
    starttime = GetRunTime();
    trans = 1.;
    _s2_fadestatus = 1;
  }

  // fading in...
  else if (_s2_fadestatus == 1) {
    double del = GetRunTime() - starttime;
    if (del > _s2_fadetime) {
      _s2_fadestatus = 2;
      trans = 0.;
    } else {
      trans = 1. - del / _s2_fadetime;
    }
  }

  // commence fade-out
  else if (_s2_fadestatus == 3) {
    starttime = GetRunTime();
    trans = 0.;
    _s2_fadestatus = 4;
    return;
  }

  // fading out...
  else if (_s2_fadestatus == 4) {
    double del = GetRunTime() - starttime;
    if (del > _s2_fadetime) {
      _s2_fadestatus = 5;
      trans = 1.;
      CleanExit();
    } else {
      trans = del / _s2_fadetime;
    }
  }

  // draw a transparent screen over the view
  glMatrixMode (GL_MODELVIEW); 
  glPushMatrix (); 
  glLoadIdentity (); 

  glMatrixMode (GL_PROJECTION); 
  glPushMatrix (); 
  glLoadIdentity ();
  
  // added 20071213 to half-fix dome fade-in/out
  glOrtho(-1,1,-1,1,-1,1);
  
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH);
  glDisable(GL_LIGHTING);
  glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
  glBegin (GL_QUADS); 
  glColor4f(0., 0., 0., trans);
  glVertex3f (-1, -1, 0.95); 
  glVertex3f (1, -1, 0.95); 
  glVertex3f (1, 1, 0.95); 
  glVertex3f (-1, 1, 0.95); 
  glEnd ();
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH);
  glDisable(GL_BLEND);
  glPopMatrix (); 
  glMatrixMode (GL_MODELVIEW); 
  glPopMatrix ();

  return;
}


/* Set the camera position based on internally recorded settings.
 *  - based on "CameraHome" function 
 */
void _s2priv_CameraSet(void) {
  double size;

  /* Stop any forward movement */
  camera.speed = 0;
  
  /* Find the bounds of the geometry */
  CalcBounds();
  
  size = MAX((pmax.x-pmin.x),(pmax.y-pmin.y));
  size = MAX(size,(pmax.z-pmin.z));

  /* addition dbarnes for S2PLOT: simply suggest the geometry is
   * larger than it really is! */
  size = sqrt(2.0) * size;
  
  /* Set the camera attributes */
  camera.pr = pmid; /* point to rotate about */
  camera.vp = _s2_camera_vp;
  camera.vu = _s2_camera_vu;
  camera.vd = _s2_camera_vd;
  
  /* Update the focal point */
  Normalise(&camera.vu);
  Normalise(&camera.vd);
  camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
  camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
  camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
  
  /* prevent this position being "re-set" - user must call hs2setcam
   * if they want to reposition the camera again */
  _s2_cameraset = 0;
}


/* load a texture */
unsigned int ss2lt(char *itexturefn) {
  int width, height;
  BITMAP4 *bitmap = NULL;

  /* textures loaded this way are all called "<cached>" so that they
   * are not deleted when geometry is deleted.  Consequently, we do
   * not look for an existing texture of same name!
   */

  // 1. if filename starts with "." or "/" then attempt to open
  //    precisely this file.  No other attempts will be made.  
#if !defined(S2SUNOS)
  struct stat filestat;
#else
  int filestat;
#endif
  if (!strncmp(itexturefn, ".", 1) || !strncmp(itexturefn, "/", 1)) {
    if (!stat(itexturefn, &filestat)) {
      bitmap = ReadTGATexture(itexturefn, &width, &height);
    }
  } else {
    // 2. in order, try to load texture from current dir, then from
    //    S2PLOT_TEXPATH (if defined), then from S2PATH/textures
    //    (if defined).
    char cwd[200], texname[200];
    if (getcwd(cwd, 200)) {
      sprintf(texname, "%s/%s", cwd, itexturefn);
      if (!stat(texname, &filestat)) {
	bitmap = ReadTGATexture(texname, &width, &height);
      }
    }
    if (!bitmap) {
      char *s2texpath = getenv("S2PLOT_TEXPATH");
      if (s2texpath) {
	sprintf(texname, "%s/%s", s2texpath, itexturefn);
	if (!stat(texname, &filestat)) {
	  bitmap = ReadTGATexture(texname, &width, &height);
	}
      }
    }
    if (!bitmap) {
      char *s2path = getenv("S2PATH");
      if (s2path) {
	sprintf(texname, "%s/textures/%s", s2path, itexturefn);
	if (!stat(texname, &filestat)) {
	  bitmap = ReadTGATexture(texname, &width, &height);
	}
      }
    }
  }
  if (!bitmap) {
    fprintf(stderr, "Advisory: unable to load texture %s, using 'red X'\n",
	    itexturefn);
    width = height = 16;
    bitmap = _s2priv_redXtexture(width, height);
  }

  // use mipmaps for loads from file
  return _s2priv_setupTexture(width, height, bitmap, 1);

}


/* reinstall a texture, eg. after modifying the map returned by 
 * s2ext_gettexture.
 */
void ss2pt(unsigned int itextureID) {
  // use mipmaps
  _s2priv_ss2pt(itextureID, 1);
}
void ss2ptt(unsigned int itextureID) {
  // don't use mipmaps
  _s2priv_ss2pt(itextureID, 0);
}

void _s2priv_ss2pt(unsigned int itextureID, int usemipmaps) {
  int i;
  for (i = 0; i < _s2_ctext_count; i++) {
    if (_s2_ctext[i].id == itextureID) {
      /* fetch known properties */
      int width = _s2_ctext[i].width;
      int height = _s2_ctext[i].height;
      int depth = _s2_ctext[i].depth;
      int doing3d = (depth > 0);
#if !defined(S2_3D_TEXTURES)
      if (doing3d) {
	_s2error("ss2pt/ss2ptt", "Cannot use 3D texture on this platform");
      }
#endif
      BITMAP4 *bitmap = _s2_ctext[i].bitmap;

      if (_s2_devcap & _S2DEVCAP_NOCOLOR) {
	// desaturate the bitmap data
	int i, j, k;
	int idx = 0;
	float sum;
	for (i = 0; i < width; i++) {
	  for (j = 0; j < height; j++) {
	    if (doing3d) {
	      for (k = 0; k < depth; k++) {
		sum = (bitmap[idx].r + bitmap[idx].g + bitmap[idx].b) * 0.33;
		bitmap[idx].r = bitmap[idx].g = bitmap[idx].b = sum;
		idx++;
	      }
	    } else {
	      sum = (bitmap[idx].r + bitmap[idx].g + bitmap[idx].b) * 0.33;
	      bitmap[idx].r = bitmap[idx].g = bitmap[idx].b = sum;
	      idx++;
	    }
	  }
	}
      }

      if(options.stereo < 0) {
	return;
      }
#if defined(S2_3D_TEXTURES)
      int gl_tex_mode = doing3d ? GL_TEXTURE_3D : GL_TEXTURE_2D;
#else
      int gl_tex_mode = GL_TEXTURE_2D;
#endif

      /* and rebind */
      glBindTexture(gl_tex_mode,itextureID);
      glTexParameterf(gl_tex_mode,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
      glTexParameterf(gl_tex_mode,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
#if defined(S2_3D_TEXTURES)
      if (doing3d) {
	glTexParameterf(gl_tex_mode,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
      }
#endif

      if (usemipmaps) {
	glTexParameterf(gl_tex_mode,GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
      } else {
	glTexParameterf(gl_tex_mode, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR);
      }
      glTexParameterf(gl_tex_mode,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
#if defined(S2_3D_TEXTURES)
      if (doing3d) {
	glTexImage3D(GL_TEXTURE_3D,0,4, width, height,depth,
		     0,GL_RGBA,GL_UNSIGNED_BYTE,bitmap);
	if (usemipmaps) {
	  gluBuild3DMipmaps(GL_TEXTURE_3D,4,
			    width, height, depth,
			    GL_RGBA,GL_UNSIGNED_BYTE,bitmap);
	}
      } else
#endif
	{
	glTexImage2D(GL_TEXTURE_2D,0,4, width, height,
		     0,GL_RGBA,GL_UNSIGNED_BYTE,bitmap);
	if (usemipmaps) {
	  gluBuild2DMipmaps(GL_TEXTURE_2D,4,
			    width, height,
			    GL_RGBA,GL_UNSIGNED_BYTE,bitmap);
	}
      }
      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
      return;
    }
  }
  return;
}

int __texid = 0;
unsigned int _s2priv_setupTexture(int width, int height, 
				  BITMAP4 *bitmap, int usemipmaps) {

#if defined(BUILDING_S2PLOT)
  if (_s2_devcap & _S2DEVCAP_NOCOLOR) {
    // desaturate the bitmap data
    int i, j;
    int idx = 0;
    float sum;
    for (i = 0; i < width; i++) {
      for (j = 0; j < height; j++, idx++) {
	sum = (bitmap[idx].r + bitmap[idx].g + bitmap[idx].b) * 0.33;
	bitmap[idx].r = bitmap[idx].g = bitmap[idx].b = sum;
      }
    }
  }
#endif

  int assign_id = -1;
  if (options.stereo >= 0) {
    GLuint id;
    glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_2D,id);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    if (usemipmaps) {
      glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
		      GL_LINEAR_MIPMAP_LINEAR);
    } else {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		      GL_LINEAR);
    }
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,4, width, height,
		 0,GL_RGBA,GL_UNSIGNED_BYTE,bitmap);
    if (usemipmaps) {
      gluBuild2DMipmaps(GL_TEXTURE_2D,4,
			width, height,
			GL_RGBA,GL_UNSIGNED_BYTE,bitmap);
    }
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    assign_id = id;
  } else {
    assign_id = __texid;
    __texid++;
  }
  /* assume all went well - possibly a bad assumption */
  _s2_ctext = (_S2CACHEDTEXTURE *)realloc(_s2_ctext, (_s2_ctext_count+1) *
					   sizeof(_S2CACHEDTEXTURE));
  _s2_ctext[_s2_ctext_count].width = width;
  _s2_ctext[_s2_ctext_count].height = height;
  _s2_ctext[_s2_ctext_count].depth = 0;
  _s2_ctext[_s2_ctext_count].bitmap = bitmap;
  _s2_ctext[_s2_ctext_count].id = assign_id;

  _s2_ctext_count++;
  return (unsigned int)assign_id;
}

#if defined(S2_3D_TEXTURES)
/* 3d version */
unsigned int _s2priv_setupTexture3d(int width, int height, int depth,
				    BITMAP4 *bitmap, int usemipmaps) {

#if defined(BUILDING_S2PLOT)
  if (_s2_devcap & _S2DEVCAP_NOCOLOR) {
    // desaturate the bitmap data
    int i, j, k;
    int idx = 0;
    float sum;
    for (i = 0; i < width; i++) {
      for (j = 0; j < height; j++) {
	for (k = 0; k < depth; k++, idx++) {
	  sum = (bitmap[idx].r + bitmap[idx].g + bitmap[idx].b) * 0.33;
	  bitmap[idx].r = bitmap[idx].g = bitmap[idx].b = sum;
	}
      }
    }
  }
#endif

  int assign_id = -1;
  if (options.stereo >= 0) {
    GLuint id;
    glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_3D,id);
    glTexParameterf(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    if (usemipmaps) {
      glTexParameterf(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,
		      GL_LINEAR_MIPMAP_LINEAR);
    } else {
      glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,
		      GL_LINEAR);
    }
    glTexParameterf(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D,0,4, width, height,depth,
		 0,GL_RGBA,GL_UNSIGNED_BYTE,bitmap);
    if (usemipmaps) {
      gluBuild3DMipmaps(GL_TEXTURE_3D,4,
			width, height, depth,
			GL_RGBA,GL_UNSIGNED_BYTE,bitmap);
    }
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    assign_id = id;
  } else {
    assign_id = __texid;
    __texid++;
  }
  /* assume all went well - possibly a bad assumption */
  _s2_ctext = (_S2CACHEDTEXTURE *)realloc(_s2_ctext, (_s2_ctext_count+1) *
					   sizeof(_S2CACHEDTEXTURE));
  _s2_ctext[_s2_ctext_count].width = width;
  _s2_ctext[_s2_ctext_count].height = height;
  _s2_ctext[_s2_ctext_count].depth = depth;
  _s2_ctext[_s2_ctext_count].bitmap = bitmap;
  _s2_ctext[_s2_ctext_count].id = assign_id;

  _s2_ctext_count++;
  return (unsigned int)assign_id;
}
#endif


void _s2priv_dropTexture(unsigned int texid) {
  int i = 0, j;
  while (i < _s2_ctext_count && _s2_ctext[i].id != texid) {
    i++;
  }
  if (i >= _s2_ctext_count) {
    return;
  }

  if (options.stereo >= 0) {
    // remove the texture from OpenGL internal
    glDeleteTextures(1, (const GLuint *)(void *)(&texid));
  }

  free(_s2_ctext[i].bitmap);

  /* this loop could be removed and replaced with one calculated
   * call to bcopy ... */
  for (j = i; j < (_s2_ctext_count-1); j++) {
    bcopy(_s2_ctext + j + 1, _s2_ctext + j, sizeof(_S2CACHEDTEXTURE));
  }
  _s2_ctext = (_S2CACHEDTEXTURE *)realloc(_s2_ctext, (_s2_ctext_count - 1) *
					   sizeof(_S2CACHEDTEXTURE));
  _s2_ctext_count--;
}

