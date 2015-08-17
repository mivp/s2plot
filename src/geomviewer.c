/* geomviewer.c
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
 * $Id: geomviewer.c 5828 2012-11-15 00:13:35Z dbarnes $
 *
 */

#include "s2globals.h"
#include "geomviewer.h"

#if defined(BUILDING_S2PLOT)
#include "s2plot.h"
#endif

#include "s2privfn.h"

#if defined(BUILDING_S2PLOT)
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#if !defined(S2SUNOS)
#include <sys/stat.h>
#endif
#endif

// not sure if this breaks anything: it shouldn't!
#define WINWIDTH (options.screenwidth)
#define WINHEIGHT (options.screenheight)

#if defined(S2CLOSED)
#include "_CLOSED_misc/s2glu.c"
#else
#include "s2glu.c"
#endif

#include <dlfcn.h>
void loadDevices(char *drivername);
void(*_device_draw)(CAMERA cam);
void(*_device_resize)(void);
int(*_device_keybd)(char);

#if defined(S2OPENMP) && !defined(BUILDING_VIEWER)
//#define POINTER_SORT 1
#include <omp.h>
// mQsortMT algorithm/code from http://softwarecommunity.intel.com/articles/eng/2422.htm
// Rajiv Parikh
#if defined(POINTER_SORT)
int bbptrcomp(const void *, const void *);
#define QST _S2BBOARD
#define QSTcomp bbptrcomp
void mQsortMTptr(QST **data, int N) {
  QST **a, **b, **c;
  int i, amax, bmax, j, k;
  amax = N/2; 
  bmax = N-amax; 
  a = data;
  b = data+amax;

  // sort two halves
#pragma omp parallel sections
  {
#pragma omp section
    qsort(a, amax, sizeof(QST *), QSTcomp);
#pragma omp section
    qsort(b, bmax, sizeof(QST *), QSTcomp);
  }

  // merge
  i = amax - 1;
  j = N - 1;
  if (((a[amax-1]))->dist > ((b[0]))->dist) {
    c = (QST **)malloc((bmax)*sizeof(QST *));
    for (k = 0; k < bmax; k++) {
      c[k] = b[k];
    }
    k = bmax - 1;
    while ((i >= 0) && (k >= 0)) {
      if (((a[i]))->dist>=((c[k]))->dist) {
	data[j--] = a[i--];
      } else {
	data[j--] = c[k--];
      }
    }
    while (k>=0) {
      data[j--] = c[k--];
    }
    free(c);
  }
}

#else

int bboardcomp(const void *, const void *);
#define QST _S2BBOARD
#define QSTcomp bboardcomp
void mQsortMT(QST* data, int N) {
  QST *a,*b,*c;
  int i, amax, bmax, j, k;
  amax = N/2; 
  bmax = N-amax; 
  a = data;
  b = data+amax;

  // sort two halves
#pragma omp parallel sections
  {
#pragma omp section
    //QsortST(a,amax);
    qsort(a, amax, sizeof(QST), QSTcomp);
#pragma omp section
    //QsortST(b,bmax);
    qsort(b, bmax, sizeof(QST), QSTcomp);
  }

  // merge
  i = amax - 1;
  j = N - 1;
  if (a[amax-1].dist > b[0].dist) {
    c = (QST *)malloc((bmax)*sizeof(QST));
    for (k = 0; k < bmax; k++) {
      c[k] = b[k];
    }
    k = bmax - 1;
    while ((i >= 0) && (k >= 0)) {
      if (a[i].dist>=c[k].dist) {
	data[j--] = a[i--];
      } else {
	data[j--] = c[k--];
      }
    }
    while (k>=0) {
      data[j--] = c[k--];
    }
    free(c);
  }
}


int handlecomp(const void *, const void *);
#define QSTh _S2HANDLE
#define QSThcomp handlecomp
void mQsortMTh(QSTh* data, int N) {
  QSTh *a,*b,*c;
  int i, amax, bmax, j, k;
  amax = N/2; 
  bmax = N-amax; 
  a = data;
  b = data+amax;

  // sort two halves
#pragma omp parallel sections
  {
#pragma omp section
    qsort(a, amax, sizeof(QSTh), QSThcomp);
#pragma omp section
    qsort(b, bmax, sizeof(QSTh), QSThcomp);
  }

  // merge
  i = amax - 1;
  j = N - 1;
  if (a[amax-1].dist > b[0].dist) {
    c = (QSTh *)malloc((bmax)*sizeof(QSTh));
    for (k = 0; k < bmax; k++) {
      c[k] = b[k];
    }
    k = bmax - 1;
    while ((i >= 0) && (k >= 0)) {
      if (a[i].dist>=c[k].dist) {
	data[j--] = a[i--];
      } else {
	data[j--] = c[k--];
      }
    }
    while (k>=0) {
      data[j--] = c[k--];
    }
    free(c);
  }
}
#endif
#endif

#include "hiddenMouseCursor.h"

//#if defined(S2_USE_GLFLOAT)
#define float GLfloat
//#endif

#if defined(S2SUNOS)
#define powf pow
#define fabsf fabs
#define truncf(a) ((a) < 0 ? ceil(a) : floor(a))
#define log10f log10
#endif

// Interface state
INTERFACESTATE interfacestate;


// Global Materials, set through the materials form 
GLfloat specularcolour[4] = {1.0,1.0,1.0,1.0};
GLfloat shininess[1]      = {100.0};
GLfloat emission[4]       = {0.0,0.0,0.0,1.0};
GLfloat transparency      = 1.0;

float targetRoC_x = 0., targetRoC_y = 0.;
float currRoC_x = 0., currRoC_y = 0.;

#if defined(BUILDING_S2PLOT)
void _glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
  static float sum;
  if (_s2_devcap & _S2DEVCAP_NOCOLOR) {
    /* desaturate */
    sum = (red + green + blue) * 0.33;
    glColor4f(sum, sum, sum, alpha);
  } else {
    glColor4f(red, green, blue, alpha);    
  }
}
#else
#define _glColor4f(r,g,b,a) glColor4f((r),(g),(b),(a))
#endif
#if !defined(QUICKINTERSTEREO)
GLuint _interstereo_txt[2] = {0, 0};
int _interstereo_w, _interstereo_h;
#endif
void drawView(char *projinfo, double camsca);

// below switches to new handle system that supports panels
void handleView(int msx, int msy);

#if defined(BUILDING_VIEWER)
// prototypes for ss2ct, ss2dt
unsigned int ss2ct(int width, int height);
void ss2dt(unsigned int texid);
void _s2priv_dropTexture(unsigned int texid);
unsigned int _s2priv_setupTexture(int width, int height, 
				  BITMAP4 *bitmap, int usemipmaps);
unsigned int _s2priv_setupTexture3d(int width, int height, int depth,
				  BITMAP4 *bitmap, int usemipmaps);
BITMAP4 *_s2priv_redXtexture(int w, int h);
BITMAP4 *_s2priv_blankTexture3d(int w, int h, int d);
#endif

#if defined(BUILDING_S2PLOT)
#include <pthread.h>
void *remote_thread_sub(void *);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#define MTX_LOCK(a) if (_s2_remoteport && !_s2_skiplock) { pthread_mutex_lock((a)); } 
#define MTX_ULCK(a) if (_s2_remoteport && !_s2_skiplock) { pthread_mutex_unlock((a)); }

// handling keyboard: remote_cb must not call HandleKeyboard. Instead
// if it is a keyboard command (prefix K) then add the character to
// this string, have the string processed and emptied at the start of
// the refresh cycle, and mutex it.
pthread_mutex_t _kbd_mutex = PTHREAD_MUTEX_INITIALIZER;
#define KBD_CHARS_BUFSIZE 256
unsigned char _kbd_chars[KBD_CHARS_BUFSIZE];

#else
#define MTX_LOCK(a) 
#define MTX_ULCK(a)
#endif

#if defined(BUILDING_VIEWER)
int main(int argc,char **argv) {
  int i,gotdata = FALSE;
  char offname[64],geomname[64],viewname[64],autopilotname[64];
  //char meshfile[128] = "";
  COLOUR black = {0,0,0};
  XYZ unity = {1,1,1},zero = {0,0,0};
  
  // Defalt options
  options.debug             = FALSE;
  options.stereo            = NOSTEREO;   
  options.fullscreen        = FALSE;
  options.dometype          = NODOME;
  options.showdomeintensity = TRUE;
  options.rendermode        = WIREFRAME;  
  options.showtextures      = TRUE;
  options.showboundingbox   = FALSE;
  options.recordimages      = FALSE;
  options.windowdump        = 0;
  options.makeqtvr          = FALSE;
  options.showhelpinfo      = FALSE;
  options.projectiontype    = PERSPECTIVE;
  options.selectmode        = FALSE;
  options.targetfps         = 30;
  options.interaction       = INSPECT;
  options.autopilot         = FALSE;
  options.autospin          = zero;  // 0, 1, or -1 for still, left, right 
  options.interactspeed     = 1;     // Scale factor on deltamove
  options.deltamove         = -1;    // Movement/rotation increment 
  options.background        = black;
  options.sphereresolution  = 16;
  options.doantialias       = FALSE;
  options.pointscale        = 1;
  options.linescale         = 1;
  options.scalefactor       = unity;
  options.localviewer       = TRUE;   
  options.twosided          = FALSE;  
  options.faceorder         = GL_CCW;
  options.texturetype       = GL_TEXTURE_2D;
  camera.fishrotate         = 0;
  
  // Interface default
  interfacestate.currentbutton  = -1;
  interfacestate.mousexlast     = -1;
  interfacestate.mouseylast     = -1;
  interfacestate.spaceballxlast = -1;
  interfacestate.spaceballylast = -1;
  interfacestate.spaceballzlast = -1;
  interfacestate.framerate      = 0;

  // name of our device driver - default none
  char *_s2_driver = NULL;
  
  offname[0] = '\0';
  geomname[0] = '\0';
  viewname[0] = '\0';
  for (i=1;i<argc;i++) {
    if (strcmp(argv[i],"-h") == 0) 
      GiveUsage(argv[0]);
    if (strcmp(argv[i],"-s") == 0)
      options.stereo = ACTIVESTEREO;
    if (strcmp(argv[i],"-ss") == 0) 
      options.stereo = DUALSTEREO;
    if (strcmp(argv[i],"-si") == 0) {
      options.stereo = INTERSTEREO;
      _s2_driver = (char *)calloc(strlen("s2interstereo")+1, sizeof(char));
      strcpy(_s2_driver, "s2interstereo");      
    }
    if (strcmp(argv[i],"-sa") == 0) {
      options.stereo = ANAGLYPHSTEREO;
      _s2_driver = (char *)calloc(strlen("s2anaglyph")+1, sizeof(char));
      strcpy(_s2_driver, "s2anaglyph");
    }
    if (strcmp(argv[i],"-sw") == 0) {
      options.stereo = WDUALSTEREO;
      _s2_driver = (char *)calloc(strlen("s2warpstereo")+1, sizeof(char));
      strcpy(_s2_driver, "s2warpstereo");
    }
    if (strcmp(argv[i],"-f") == 0)
      options.fullscreen = TRUE;
    if (strcmp(argv[i],"-fly") == 0)
      options.interaction = FLY;
    if (strcmp(argv[i],"-walk") == 0)
      options.interaction = WALK;
    if (strcmp(argv[i],"-d") == 0)
      options.debug = TRUE;
    if (strcmp(argv[i],"-as") == 0) {
      options.autospin.x = 1; 
      options.autospin.y = 0; 
      options.autospin.z = 0;
    }
    if (strcmp(argv[i],"-cw") == 0)
      options.faceorder = GL_CW;
    
    // Fisheye/dome projection
    if (strcmp(argv[i],"-fish") == 0) {
      i++;
      options.dometype = atof(argv[i]);
      options.projectiontype = FISHEYE;
      if (options.dometype < TRUNCBOTTOM || options.dometype > MIRROR1) {
	_s2warn("main", "unrecognised dome type");
	options.dometype = NODOME;
	options.projectiontype = PERSPECTIVE;
      } else {
	_s2_driver = (char *)calloc(strlen("s2fishdome")+1, sizeof(char));
	strcpy(_s2_driver, "s2fishdome");      
      }
    }
    if (strcmp(argv[i],"-fishr") == 0) {
      i++;
      camera.fishrotate = DTOR*atof(argv[i]);
    }
    
    // Read warp file
    if (strcmp(argv[i],"-m") == 0) {
      i++;
      strcpy(options.meshfile,argv[i]);
      options.dometype = 5;
      options.projectiontype = FISHEYE;
      _s2_driver = (char *)calloc(strlen("s2fishdome")+1, sizeof(char));
      strcpy(_s2_driver, "s2fishdome");      
    }
    
    if (strcmp(argv[i],"-fps") == 0) {
      i++;
      options.targetfps = atof(argv[i]);
    }
    if (strcmp(argv[i],"-dm") == 0) {
      i++;
      options.deltamove = atof(argv[i]);
    }
    if (strcmp(argv[i],"-sh") == 0) {
      i++;
      shininess[0] = atof(argv[i]);	
    }
    if (strcmp(argv[i],"-ap") == 0) {
      options.autopilot = TRUE;
      i++;
      strcpy(autopilotname,argv[i]);
    }
    if (strcmp(argv[i],"-rw") == 0)
      options.rendermode = WIREFRAME;
    if (strcmp(argv[i],"-rf") == 0)
      options.rendermode = SHADE_FLAT;
    if (strcmp(argv[i],"-rd") == 0)
      options.rendermode = SHADE_DIFFUSE;
    if (strcmp(argv[i],"-rs") == 0)
      options.rendermode = SHADE_SPECULAR;
    if (strcmp(argv[i],"-og") == 0) {
      i++;
      strcpy(geomname,argv[i]);
    }
    if (strcmp(argv[i],"-oo") == 0) {
      i++;
      strcpy(offname,argv[i]);
    }
    if (strcmp(argv[i],"-vf") == 0) {
      i++;
      strcpy(viewname,argv[i]);
    }
    if (strcmp(argv[i],"-bb") == 0)
      options.showboundingbox = !options.showboundingbox;
    if (strcmp(argv[i],"-bg") == 0) {
      i++;
      options.background.r = atof(argv[i++]);
      options.background.g = atof(argv[i++]);
      options.background.b = atof(argv[i]);
    }
    if (strcmp(argv[i],"-sr") == 0) {
      i++;
      options.sphereresolution = atoi(argv[i]);
    }
    if (strcmp(argv[i],"-sc") == 0) {
      i++;
      options.scalefactor.x = atof(argv[i]);
      i++;
      options.scalefactor.y = atof(argv[i]);
      i++;
      options.scalefactor.z = atof(argv[i]);
    }
  }

  // Check fisheye vs stereo
  if (options.stereo != NOSTEREO && options.dometype != NODOME) {
    _s2warn("main", "stereo fisheye visual not available");
    options.stereo = NOSTEREO;
    options.dometype = NODOME;
    free(_s2_driver);
    _s2_driver = NULL;
  }
  
  // Setup, initialise OpenGL and windowing system (glut or other)
  _s2debug("main", "initialising window system (glut or other)");
  s2winInit(&argc, argv);

  _device_draw = NULL;
  _device_resize = NULL;
  _device_keybd = NULL;
  if (_s2_driver) {
    loadDevices(_s2_driver);
  }

  _s2x_ati = 0;
  {
    char *tmpchr = getenv("S2PLOT_INTERSCANATI");
    if (tmpchr) {
      if (!strcmp(tmpchr, "yes")) {
	_s2x_ati = 1;
      }
    }
  }
  _s2_ctext_count = 0;
  _s2_ctext = NULL;

  
  CreateOpenGL();
  
  // Can we handle non power of 2 textures?
#if GL_EXT_texture_rectangle
  options.texturetype = GL_TEXTURE_RECTANGLE_EXT;
#else
#if GL_NV_texture_rectangle
  options.texturetype = GL_TEXTURE_RECTANGLE_NV;
#else
  options.texturetype = GL_TEXTURE_2D;
#endif
#endif
  if (options.texturetype == GL_TEXTURE_2D) {
    _s2debug("main", "non-power-of-2 textures are not supported");
  } else {
    _s2debug("main", "non-power-of-2 textures are supported");
  }
    
  /* 
     Initial model if there is one 
     If none then create a 3D plus sign
  */
  DeleteGeometry();
  if (strlen(geomname) > 0) 
    gotdata = ReadGeometry(geomname);
  if (strlen(offname) > 0) 
    gotdata = ReadOFF(offname);
  if (!gotdata) {
    CreateAxes();
    gotdata = TRUE;
  }
  CleanGeometry();
  
  // Initialise things before starting 
  CameraHome(HOME);
  options.camerahome = camera;
  if (strlen(viewname) > 0)
    ReadViewFile(viewname);
  MakeGeometry(TRUE, FALSE, 'c');
  if (options.autopilot) 
    options.autopilot = AutoPilot(0,autopilotname);
  
  // Lets go, we're ready for event handling 
  _s2debug("main", "entering main event loop");
  s2winMainLoop();
  return(0);
}
#endif

/*
   This is where OpenGL and the windowing system (glut or other) are 
   initialised and configured.
*/
void CreateOpenGL(void)
{
  GLint num[10];
  GLfloat fsizes[2],fstep;
  s2winInitDisplayMode(options.stereo, _s2x_ati);

  // set default screen dimensions
  options.screenwidth  = 800;
  options.screenheight = 600;
#if defined(BUILDING_S2PLOT)
  {
    char *widthstr = getenv("S2PLOT_WIDTH");
    if (widthstr) {
      int widthval = atoi(widthstr);
      if (widthval) {
	options.screenwidth = widthval;
	char *heightstr = getenv("S2PLOT_HEIGHT");
	if (heightstr) {
	  int heightval = atoi(heightstr);
	  if (heightval) {
	    options.screenheight = heightval;
	  } else {
	    options.screenheight = 3 * widthval / 4;
	  }
	} else {
	  options.screenheight = 3 * widthval / 4;
	}
      }
    } 
  }
#endif
  if ((options.stereo == DUALSTEREO) || (options.stereo == WDUALSTEREO)) {
    options.screenwidth *= 2;
  } else if (options.stereo == TRIOSTEREO) {
    options.screenwidth *= 3;
  }
  s2winInitWindowSize(options.screenwidth, options.screenheight);

  // create our window
#if defined(BUILDING_S2PLOT)
  s2winCreateWindow("S2PLOT application");
#else
  s2winCreateWindow("Geom File Viewer");
#endif

  // go full screen if requested
  if (options.fullscreen) {
    s2winFullScreen();
#if defined(S2DARWIN)
    hideTheMouse();
#endif
  }
  
  // install handlers
  s2winReshapeFunc(HandleReshape);
  s2winDisplayFunc(HandleDisplay);
  s2winVisibilityFunc(HandleVisibility);
  s2winKeyboardFunc(HandleKeyboard);
  s2winIdleFunc(HandleIdle);
  s2winSpecialFunc(HandleSpecialKeyboard);
  s2winMouseFunc(HandleMouse);
  s2winMotionFunc(HandleMouseMotion);
  s2winPassiveMotionFunc(HandlePassiveMotion);
  
  s2winSpaceballButtonFunc(HandleSpaceballButton);
  s2winSpaceballMotionFunc(HandleSpaceballMotion);
  s2winSpaceballRotateFunc(HandleSpaceballRotate);

#if !defined(BUILDING_S2PLOT)
  if ((options.stereo == DUALSTEREO) || (options.stereo == WDUALSTEREO)) {
    s2winSetCursor(S2_CURSOR_NONE);
  }
#else
  if (_s2_devcap & _S2DEVCAP_CURSOR) {
    s2winSetCursor(S2_CURSOR_CROSSHAIR);
  } else {
    s2winSetCursor(S2_CURSOR_CROSSHAIR);
  }
#endif
  
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glDisable(GL_DITHER);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);
  glDisable(GL_CULL_FACE);
  
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glClearColor(options.background.r,options.background.g,options.background.b,0.0);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glFrontFace(options.faceorder);
  
  // Test miscellaneous aspects of the OpenGL state 
  if (options.debug) {
    fprintf(stderr,"Screen size: %d x %d\n",s2winGet(S2_SCREEN_WIDTH),s2winGet(S2_SCREEN_HEIGHT));
    
    glGetFloatv(GL_POINT_SIZE_RANGE,fsizes);
    glGetFloatv(GL_POINT_SIZE_GRANULARITY,&fstep);
    fprintf(stderr,"Point sizes supported: %g to %g in steps of %g\n",fsizes[0],fsizes[1],fstep);
    glGetFloatv(GL_LINE_WIDTH_RANGE,fsizes);
    glGetFloatv(GL_LINE_WIDTH_GRANULARITY,&fstep);
    fprintf(stderr,"Line widths supported: %g to %g in steps of %g\n",fsizes[0],fsizes[1],fstep);
    
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,num); 
    fprintf(stderr,"Maximum texture size: %d\n",(int)num[0]);
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH,num);
    fprintf(stderr,"Projection stack depth: %d\n",(int)num[0]);
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH,num);
    fprintf(stderr,"ModelView stack depth: %d\n",(int)num[0]);
    glGetIntegerv(GL_MAX_CLIP_PLANES,num);
    fprintf(stderr,"Number of clipping planes: %d\n",(int)num[0]);
    glGetIntegerv(GL_DEPTH_BITS,num);
    fprintf(stderr,"Depth buffer \"depth\": %d\n",(int)num[0]);
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS,num);
    fprintf(stderr,"Maximum viewport size: %d x %d\n",(int)num[0],(int)num[1]);
    glGetIntegerv(GL_AUX_BUFFERS,num);
    fprintf(stderr,"Number of aucillary buffers: %d\n",(int)num[0]);
  }
  glGetIntegerv(GL_SAMPLE_BUFFERS,&(num[0]));
  glGetIntegerv(GL_SAMPLES,&(num[1]));
  if (options.debug) {
    fprintf(stderr,"Sample buffers: %d, samples: %d\n",(int)num[0],(int)num[1]);
    if (num[0] > 0 && num[1] > 1)
      fprintf(stderr,"Multisampling enabled\n");
    else
      fprintf(stderr,"Multisampling not enabled\n");
  }
  if (num[0] > 0 && num[1] > 1)
    glEnable(GL_MULTISAMPLE);

}

/*
   Handle a window reshape/resize
*/
void HandleReshape(int w,int h)
{
  MTX_LOCK(&mutex);
  
  if ((w != options.screenwidth) || (h != options.screenheight)) {
    if (_device_resize) {
      _device_resize();
    }
  }
  
  options.screenwidth = w;
  options.screenheight = h;
  
  MTX_ULCK(&mutex);
}

/* Display callback routine.  Thoroughly re-worked July/August 2007 to
 * now use one or more calls to "drawView" function for all non-warped
 * devices, and to account for addition of panels and screen coordinate
 * geometry.  Still needs some minor tidying and hopefully compression
 * of the dome code.
 */
void HandleDisplay(void) {
  MTX_LOCK(&mutex);
  
  int i,j;
  static int framecount = -1;
  static double tstart; /* this is for frame rate counting */
  static double tbegin = -1.; /* this is when we really started! */
  double tstop;
  
  /* Set the time the first time around */
  if (framecount < 0) {
    framecount = 0;
    tstart = GetRunTime();
#if defined(BUILDING_S2PLOT)
    _s2x_ywinpos = s2winGet(S2_WINDOW_Y);
#endif
  }
  
  if (tbegin < 0.) {
    tbegin = GetRunTime();
  }
  
  if (_device_resize) {
    int curwiny = s2winGet(S2_WINDOW_Y);
    if (curwiny != _s2x_ywinpos) {
      (_device_resize)();
    }
    _s2x_ywinpos = curwiny;
  }
  
  /* 1. set the current time, then loop over panels, updating camera
   *    position and creating dynamic geometry lists via callbacks.
   */
  
#if defined(BUILDING_S2PLOT)
  unsigned int uj;
  
  /* loop over the panels */
  double tm = GetRunTime();
  
  /* push expired events into the queue */
  for (i = 0; i < _s2_nevents; i++) {
    if (!_s2_events[i].consumed && (_s2_fadestatus == 2) && 
	(tbegin + _s2_fadetime + _s2_events[i].t < tm)) {
      switch (_s2_events[i].type) {
      case ASCII_KEY_EVENT:
	for (uj = 0; uj < strlen((char *)(_s2_events[i].data)); uj++) {
	  _s2_skiplock = 1;
	  //HandleKeyboard(((char *)_s2_events[i].data)[uj], 0, 0);
	  _s2_process_key(((char *)_s2_events[i].data)[uj], 0, 0, 0);
	  _s2_skiplock = 0;
	}
	break;
	
      default:
	// no action
	break;
	
      }
      _s2_events[i].consumed = 1;
    }
  }

  /* process kbd list (via remote callback) */
  pthread_mutex_lock(&_kbd_mutex);
  for (i = 0; i < strlen((char *)_kbd_chars); i++) {
    _s2_skiplock = 1;
    fprintf(stderr, "processing key '%c' in main refresh loop\n", _kbd_chars[i]);
    _s2_process_key(_kbd_chars[i], 0, 0, 0);
    _s2_skiplock = 0;
  }
  _kbd_chars[0] = '\0';
  pthread_mutex_unlock(&_kbd_mutex);
  
  if ((_s2_runtime > 0) && (_s2_fadestatus == 2) && 
      (tbegin + _s2_fadetime + _s2_runtime < tm)) {
    _s2_fadestatus = 3;
  }
  
  int waspanel = _s2_activepanel;
  int spid;
  for (spid = 0; spid < _s2_npanels; spid+=1) {
    if (!_s2_panels[spid].active) {
      continue;
    }
    xs2cp(spid);
    
    /* set the camera if there is an explicitly set position */
    if (_s2_cameraset) {
      _s2priv_CameraSet();
    }
    
    /* modify the camera if we are in FLY mode */
    if (options.interaction  == FLY) {
      currRoC_x = (0.9 * currRoC_x + 0.1 * targetRoC_x);
      currRoC_y = (0.9 * currRoC_y + 0.1 * targetRoC_y);
      RotateCamera(currRoC_x, 0., 0., MOUSECONTROL);
      RotateCamera(0., currRoC_y, 0., MOUSECONTROL);
    }
    
#endif
    
    /* handle autopilot mode - what does this mean??? */
    if (options.autopilot) {
      options.autopilot = AutoPilot(1,"");
    }
    
    /* modify the camera if we are in autospin mode */
#if defined(BUILDING_S2PLOT)
    float tmpcs = ss2qcs();
    ss2scs(ss2qss());
#endif 

    if ((int)options.autospin.x != 0) {
      RotateCamera(options.autospin.x/5,0.0,0.0,KEYBOARDCONTROL);
    }
    if ((int)options.autospin.y != 0) {
      RotateCamera(0.0,options.autospin.y/5,0.0,KEYBOARDCONTROL);
    }
    if ((int)options.autospin.z != 0) {
      RotateCamera(0.0,0.0,options.autospin.z/5,KEYBOARDCONTROL);
    }
#if defined(BUILDING_S2PLOT)
    ss2scs(tmpcs);
#endif
    
    /* handle flying mode ??? */
    if (ABS(camera.speed) > EPSILON) 
      FlyCamera(camera.speed);
    
#if defined(BUILDING_S2PLOT)

    /* update the dynamic geometry lists for this panel */
    if ((_s2_callback || _s2_callbackx) && _s2_animation) {
      _s2_startDynamicGeometry(_s2_dynamic_erase /* TRUE */);
      // erase screen geom:
      _s2_startScreenGeometry(_s2_dynamic_erase /* TRUE */);
      _s2_endScreenGeometry();
      // callback
      if (_s2_callbackx_data) {
	_s2_callbackx(&tm, &_s2_callbackkey, _s2_callbackx_data);
      } else {
	_s2_callback(&tm, &_s2_callbackkey);
      }
      _s2_endDynamicGeometry();
    }

    /* set the camera if there is an explicitly set position */
    if (_s2_cameraset) {
      _s2priv_CameraSet();
    }
    
  }
  xs2cp(waspanel);
  
#endif 
  
  glClearColor(options.background.r, options.background.g, 
	       options.background.b, 0.);

  /* 2. now make one or more calls to drawView based on what type of
   *    display device is in use.
   */

  if (options.stereo == ACTIVESTEREO || options.stereo == DUALSTEREO) {

    // clear the buffer
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // right
    if (options.stereo == ACTIVESTEREO) {
      glViewport(0,0,options.screenwidth,options.screenheight);
    } else {
      glViewport(options.screenwidth/2,0,options.screenwidth/2,
		 options.screenheight);
    }
    glDrawBuffer(GL_BACK_RIGHT);
    if (options.stereo == ACTIVESTEREO) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    drawView("r", +1);
    DrawExtras();

    // left
    if (options.stereo == ACTIVESTEREO) {
      glViewport(0,0,options.screenwidth,options.screenheight);
    } else {
      glViewport(0,0,options.screenwidth/2,options.screenheight);
    }
    glDrawBuffer(GL_BACK_LEFT);
    if (options.stereo == ACTIVESTEREO) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    drawView("l", -1);
    DrawExtras();
    
  } else if (options.stereo == TRIOSTEREO) {
    /* draw left, right and centre views at lrc positions in window */

    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // left
    glViewport(options.screenwidth/3,0,options.screenwidth/3,
	       options.screenheight);
    drawView("l", -1);
    DrawExtras();

    // right
    glViewport(2*options.screenwidth/3,0,options.screenwidth/3,
	       options.screenheight);
    drawView("r", +1);
    DrawExtras();

    // centre 
    glViewport(0,0,options.screenwidth/3,options.screenheight);
    drawView("c", 0);
    DrawExtras();

  } else if (options.stereo == ANAGLYPHSTEREO) {

    if (_device_draw) {
      _device_draw(camera);
    }
    
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  } else if (options.stereo == INTERSTEREO) {

    if (_device_draw) {
      _device_draw(camera);
    }

  } else if (options.stereo == WDUALSTEREO) {
    
    if (_device_draw) {
      _device_draw(camera);
    }

  } else if (options.projectiontype == FISHEYE) {
    
    if (_device_draw) {
      _device_draw(camera);
    }

#if defined(BUILDING_S2PLOT)
    _s2_fadeinout();
#endif
    

  } else {
    /* it doesn't get any easier than this... :-) */
    
    /* SINGLE SCREEN MONO */

    glEnable(GL_MULTISAMPLE);

    /* clear the buffer */
    glDrawBuffer(GL_BACK);
    //glDrawBuffer(GL_FRONT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* just draw the centre view */
    glViewport(0, 0, options.screenwidth, options.screenheight);
    drawView("c", 0.);
    DrawExtras();

  }
  
  // High res dump hack, make a 4x4 grid of images 
  if (options.windowdump > 1) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    CreateProjection('c');
    for (i=0;i<options.windowdump;i++) {
      for (j=0;j<options.windowdump;j++) {
	//fprintf(stderr,"Frame (%d,%d)\n",i,j);
	glViewport(
		   -i*options.screenwidth,-j*options.screenheight,
		   options.windowdump*options.screenwidth,options.windowdump*options.screenheight);
	glMatrixMode(GL_MODELVIEW);
	glDrawBuffer(GL_BACK_LEFT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	s2LookAt(camera.vp.x,camera.vp.y,camera.vp.z,
		  camera.focus.x,camera.focus.y,camera.focus.z,
		  camera.vu.x,camera.vu.y,camera.vu.z);
	MakeLighting();
	MakeMaterial();
	MakeGeometry(FALSE, FALSE, 'c');
	DrawExtras();
	glFlush();
	if (options.stereo == ACTIVESTEREO)
	  WindowDump("",options.screenwidth,options.screenheight,TRUE,12);
	else
	  WindowDump("",options.screenwidth,options.screenheight,FALSE,12);
	s2winSwapBuffers();
      }
    }
    options.windowdump = 0;
  }
  
  // Are we recording images? 
  if (options.recordimages || options.windowdump == 1) {
    if (options.stereo == ACTIVESTEREO)
      WindowDump("",options.screenwidth,options.screenheight,TRUE,12);
    else
      WindowDump("",options.screenwidth,options.screenheight,FALSE,12);
    options.windowdump = 0;
  }
  
#if defined(BUILDING_S2PLOT)
  if (_s2_recstate) {
    /* set the frame string for use in filenames */
    char framestr[_S2FRAMESTRLEN + 1];
    int maxframe = (int)(powf(10, _S2FRAMESTRLEN) + 0.5);
    
    char geomstr[_S2FRAMESTRLEN + _S2GEOMEXTLEN + 1];
    char viewstr[_S2FRAMESTRLEN + _S2VIEWEXTLEN + 1];
    char dumpstr[_S2FRAMESTRLEN + _S2DUMPEXTLEN + 1];
    char dumpstrL[2 + _S2FRAMESTRLEN + _S2DUMPEXTLEN + 1];
    char dumpstrR[2 + _S2FRAMESTRLEN + _S2DUMPEXTLEN + 1];
    /* while the following files exist, increment the frame counter */
    
    FILE *tryfp;
    do {
      sprintf(framestr, "%0*d", _S2FRAMESTRLEN, _s2_recframe);
      
      sprintf(geomstr, "%s%s", framestr, _S2GEOMEXT);
      tryfp = fopen(geomstr, "rb");
      if (tryfp) {
	fclose(tryfp);
	_s2_recframe++;
	continue;
      }
      
      sprintf(viewstr, "%s%s", framestr, _S2VIEWEXT);
      tryfp = fopen(viewstr, "rb");
      if (tryfp) {
	fclose(tryfp);
	_s2_recframe++;
	continue;
      }
      
      sprintf(dumpstr, "%s%s", framestr, _S2DUMPEXT);
      tryfp = fopen(dumpstr, "rb");
      if (tryfp) {
	fclose(tryfp);
	_s2_recframe++;
	continue;
      }
      
      sprintf(dumpstrL, "L_%s%s", framestr, _S2DUMPEXT);
      tryfp = fopen(dumpstrL, "rb");
      if (tryfp) {
	fclose(tryfp);
	_s2_recframe++;
	continue;
      }
      
      sprintf(dumpstrR, "R_%s%s", framestr, _S2DUMPEXT);
      tryfp = fopen(dumpstrR, "rb");
      if (tryfp) {
	fclose(tryfp);
	_s2_recframe++;
	continue;
      }
      
      /* no matching files found - we have our frame number */
      break;
      
    } while (_s2_recframe < maxframe);
    
    if (_s2_recframe >= maxframe) {
      _s2warn("HandleDisplay", "unable to write to disk without overwrite");
    } else {
    
      /* give an advisory every 100 frames - helps prevent the user 
       * leaving the program running to fill the disk over lunchtime!
       */
      if (!(_s2_recframe % 100)) {
	fprintf(stderr, "*** Recording frame %s.\n", framestr);
      }
      
      /* save the geometry file */
      SaveGeomFile(geomstr);
      
      /* save the view file */
      SaveViewFile(camera, viewstr);
      
      /* and save the image if desired */
      if (_s2_recstate >= 2) {
	if (options.stereo == ACTIVESTEREO) {
	  WindowDump(framestr, options.screenwidth, options.screenheight, 
		     TRUE, 12);
	} else {
	  WindowDump(framestr, options.screenwidth, options.screenheight,
		     FALSE, 12);
	}
      }
    
      /* increment frame counter */
      _s2_recframe++;
      /* and turn off recording state unless we are in continuous mode */
      if (_s2_recstate != 3) {
	_s2_recstate = 0;
      }
    }
  }
#endif
  
#if defined(BUILDING_S2PLOT)
  if (_s2_bufswap) {
#endif
    s2winSwapBuffers();
#if defined(BUILDING_S2PLOT)
  }
#endif

  /* 
     Handle frame rate dependent features 
     Vary the resolution of spheres
  */
  framecount++;
  tstop = GetRunTime();
  if (tstop - tstart > 5) {
    interfacestate.framerate = framecount / (tstop - tstart);
    if (options.debug) {
      _s2debug("(internal)", "Frame rate = %.1f frames/second (%d)",
	       interfacestate.framerate, framecount);
    }
    framecount = 0;
    tstart = tstop;
  }

  MTX_ULCK(&mutex);
}

#if defined(S2_NO_S2GEOMVIEWER)
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
  ratio *= (_s2_panels[_s2_activepanel].x2 - _s2_panels[_s2_activepanel].x1) / (_s2_panels[_s2_activepanel].y2 - _s2_panels[_s2_activepanel].y1);
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
    //fprintf(stderr, "XXX S2PLOT HACKED FOR GLORTHO!!! XXX\n");
    //glOrtho(0,1,0,1,0,1);
    break;
  }

#if defined(BUILDING_S2PLOT)
    _s2_save_near = near;
    _s2_save_far = far;
#endif
}
#endif

/*
   Create the geometry
   Draw the shaded objects first, then turn off lighting for points and lines
   Draw the possibly transparent texture objects last
   There is no handling to ensure texture objects are resident
   
   If update and !doscreen then recreate the displaylist (for static
      geometry).
   Otherwise draw immediately,
*/
void MakeGeometry(int doupdate, int doscreen, int eye) {

  int i,j;
  XYZ linelist[300*MAXLABELLEN];
  XYZ normal;
  COLOUR white = {1,1,1};
  int nlinelist = 0;
#if !defined(BUILDING_S2PLOT)
  int objectid = 1;
#endif
  int oldsize = 1;
#if defined(BUILDING_S2PLOT)
  int oldstipplefactor = 0;
  unsigned short oldstipplepattern = 0xFFFF;
#endif
#if !defined(BUILDING_S2PLOT)
  static int listindex = -1;
#else
#define listindex (_s2_panels[_s2_activepanel].GL_listindex)
#endif
  
#if !defined(BUILDING_S2PLOT)
  if (doupdate) {             // Create list for main geometry 
    if (listindex > 0)
      glDeleteLists(listindex,1);
    listindex = glGenLists(1);
    if (options.debug)
      fprintf(stderr,"Recreating geometry, list index: %d\n",listindex);
    glNewList(listindex,GL_COMPILE);  // see end for end of list()
  } else {
    glCallList(listindex);
    return;
  }
#else
  // for S2PLOT, only do lists for static, non-screen geometry
  if (!_s2_dynamicEnabled && !doscreen) {
    //#define S2LISTS
#if defined(S2LISTS)
    //if (doupdate) {
    //if (doupdate || !_s2_retain_lists) {
    if (!_s2_retain_lists) {

    } else if (doupdate) {
      // Create list for main geometry
      if (listindex > 0) {
	if (options.debug) {
	  fprintf(stderr, "Deleting list index: %d\n", listindex);
	}
	glDeleteLists(listindex,1);
      }
      listindex = glGenLists(1);
      if (options.debug) {
	fprintf(stderr,"Recreating geometry, list index: %d\n",listindex);
      }
      glNewList(listindex,GL_COMPILE);  // see end for end of list()
      //glNewList(listindex,GL_COMPILE_AND_EXECUTE);  // see end for end of list()
    } else {
      glCallList(listindex);
      
      // s2 change: allow dynamic geometry
      _s2_startDynamicGeometry(FALSE);
      MakeGeometry(FALSE, doscreen, eye);
      _s2_endDynamicGeometry();
      
      return;
    }
#endif
  } else if (!_s2_dynamicEnabled && doscreen) {
    _s2_startDynamicGeometry(FALSE);
    MakeGeometry(FALSE, doscreen, eye);
    _s2_endDynamicGeometry();
  }

#endif
  
  glPushMatrix();
  glScalef(options.scalefactor.x,options.scalefactor.y,options.scalefactor.z);
  glLineWidth(options.linescale);
  glPointSize(options.pointscale);
  
#if defined(BUILDING_S2PLOT)
  /* get projections needed for screen coordinate drawing */
  GLdouble model[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  //glGetDoublev(GL_MODELVIEW_MATRIX, _ss_model);
  GLdouble proj[16];
  glGetDoublev(GL_PROJECTION_MATRIX, proj);
  //glGetDoublev(GL_PROJECTION_MATRIX, _ss_proj);
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);
  //glGetIntegerv(GL_VIEWPORT, _ss_view);
  GLdouble vtx, vty, vtz; /* vertices */
  XYZ vt, vtn;
#endif
  /* end projections needed for screen coordinate drawing */
  
  // Are the objects transparent? 
  if (transparency < 1)
    glDepthMask(GL_FALSE);
  
  // Balls 
  for (i=0;i<nball;i++) {
#if !defined(BUILDING_S2PLOT)
    glLoadName(objectid++);
#endif
    _glColor4f(ball[i].colour.r,ball[i].colour.g,ball[i].colour.b,transparency);

#if defined(BUILDING_S2PLOT)
    if (doscreen) {
      if (strlen(_s2_doingScreen) && 
	  strstr(ball[i].whichscreen, _s2_doingScreen)) {
	// screen geometry
	s2UnProject(view[0] + view[2] * ball[i].p.x + 0.5, 
		     view[1] + view[3] * ball[i].p.y + 0.5, 
		     ball[i].p.z, 
		     model, proj, view, &vt.x, &vt.y, &vt.z);
	CreateASphere(vt, ball[i].r, options.sphereresolution, 1,1);
      }
    } else if (!strlen(ball[i].whichscreen))
#endif
    CreateASphere(ball[i].p,ball[i].r,options.sphereresolution,1,1);
  }
  
  // Disks 
  for (i=0;i<ndisk;i++) {
#if !defined(BUILDING_S2PLOT)
    glLoadName(objectid++);
#endif
    _glColor4f(disk[i].colour.r,disk[i].colour.g,disk[i].colour.b,transparency);

#if defined(BUILDING_S2PLOT)
    if (doscreen) {
      if (strlen(_s2_doingScreen) &&
	  strstr(disk[i].whichscreen, _s2_doingScreen)) {
	// screen geometry
	s2UnProject(view[0] + view[2] * disk[i].p.x + 0.5, 
		     view[1] + view[3] * disk[i].p.y + 0.5, 
		     disk[i].p.z, 
		     model, proj, view, &vt.x, &vt.y, &vt.z);
	s2UnProject(view[0] + view[2] * (disk[i].p.x + disk[i].n.x), 
		     view[1] + view[3] * (disk[i].p.y + disk[i].n.y),
		     disk[i].p.z + disk[i].n.z, 
		     model, proj, view, &vtn.x, &vtn.y, &vtn.z);
	vtn.x -= vt.x;
	vtn.y -= vt.y;
	vtn.z -= vt.z;
	XYZ vtmp;
	float dor1, dor2;
	s2UnProject(view[0] + view[2] * (disk[i].p.x + disk[i].r1),
		     view[1] + view[3] * (disk[i].p.y + disk[i].r1),
		     disk[i].p.z,
		     model, proj, view, &vtmp.x, &vtmp.y, &vtmp.z);
	dor1 = sqrt((vtmp.x - vt.x)*(vtmp.x - vt.x) +
		    (vtmp.y - vt.y)*(vtmp.y - vt.y));
	s2UnProject(view[0] + view[2] * (disk[i].p.x + disk[i].r2),
		     view[1] + view[3] * (disk[i].p.y + disk[i].r2),
		     disk[i].p.z,
		     model, proj, view, &vtmp.x, &vtmp.y, &vtmp.z);
	dor2 = sqrt((vtmp.x - vt.x)*(vtmp.x - vt.x) +
		    (vtmp.y - vt.y)*(vtmp.y - vt.y));

	CreateDisk(vt, vtn, dor2, dor1, 32, 0.0, TWOPI);
      }
    } else if (!strlen(disk[i].whichscreen))
#endif
    CreateDisk(disk[i].p,disk[i].n,disk[i].r2,disk[i].r1,32,0.0,TWOPI);
  }
  

  // Cones 
  for (i=0;i<ncone;i++) {
#if !defined(BUILDING_S2PLOT)
    glLoadName(objectid++);
#endif
    _glColor4f(cone[i].colour.r,cone[i].colour.g,cone[i].colour.b,transparency);

#if defined(BUILDING_S2PLOT)
    if (doscreen) {
      if (strlen(_s2_doingScreen) && 
	  strstr(cone[i].whichscreen, _s2_doingScreen)) {
	// screen geometry
	s2UnProject(view[0] + view[2] * cone[i].p2.x + 0.5, 
		     view[1] + view[3] * cone[i].p2.y + 0.5, 
		     cone[i].p2.z, 
		     model, proj, view, &vt.x, &vt.y, &vt.z);
	s2UnProject(view[0] + view[2] * cone[i].p1.x + 0.5, 
		     view[1] + view[3] * cone[i].p1.y + 0.5, 
		     cone[i].p1.z, 
		     model, proj, view, &vtn.x, &vtn.y, &vtn.z);
	CreateCone(vt, vtn, cone[i].r2, cone[i].r1, 32, 0.0, TWOPI);
      }
    } else if (!strlen(cone[i].whichscreen))
#endif
    CreateCone(cone[i].p2,cone[i].p1,cone[i].r2,cone[i].r1,32,0.0,TWOPI);
  }

  // 3 vertex faces 
  if (nface3 > 0) {

#if defined(S2USEARRAYS)
    
    GLfloat *_f3_vertices = (GLfloat *)malloc(nface3 * 3 * 3 * sizeof(GLfloat));
    GLfloat *_f3_normals = (GLfloat *)malloc(nface3 * 3 * 3 * sizeof(GLfloat));
    GLfloat *_f3_colours = (GLfloat *)malloc(nface3 * 3 * 4 * sizeof(GLfloat));

    int nf3 = 0;
    for (i=0;i<nface3;i++) {
#if defined(BUILDING_S2PLOT)
      if (doscreen) {
	if (strlen(_s2_doingScreen) &&
	    strstr(face3[i].whichscreen, _s2_doingScreen)) {
	  // screen geometry
	  for (j = 0; j < 3; j++) {
	    s2UnProject(view[0] + view[2] * face3[i].p[j].x + 0.5, 
			view[1] + view[3] * face3[i].p[j].y + 0.5, 
			face3[i].p[j].z, 
			model, proj, view, &vtx, &vty, &vtz);
	    _f3_colours[nf3 * 3 * 4 + j * 4 + 0] = face3[i].colour[j].r;
	    _f3_colours[nf3 * 3 * 4 + j * 4 + 1] = face3[i].colour[j].g;
	    _f3_colours[nf3 * 3 * 4 + j * 4 + 2] = face3[i].colour[j].b;
	    _f3_colours[nf3 * 3 * 4 + j * 4 + 3] = transparency;
	    _f3_vertices[nf3 * 3 * 3 + j * 3 + 0] = vtx;
	    _f3_vertices[nf3 * 3 * 3 + j * 3 + 1] = vty;
	    _f3_vertices[nf3 * 3 * 3 + j * 3 + 2] = vtz;
	    _f3_normals[nf3 * 3 * 3 + j * 3 + 0] = 0.;
	    _f3_normals[nf3 * 3 * 3 + j * 3 + 1] = 0.;
	    _f3_normals[nf3 * 3 * 3 + j * 3 + 2] = -1.;	    
	  }
	  nf3++;
	}
      } else if (!strlen(face3[i].whichscreen)) 
#endif
	{
	  for (j=0;j<3;j++) {
	    //glNormal3f(face3[i].n[j].x,face3[i].n[j].y,face3[i].n[j].z);
	    _f3_colours[nf3 * 3 * 4 + j * 4 + 0] = face3[i].colour[j].r;
	    _f3_colours[nf3 * 3 * 4 + j * 4 + 1] = face3[i].colour[j].g;
	    _f3_colours[nf3 * 3 * 4 + j * 4 + 2] = face3[i].colour[j].b;
	    _f3_colours[nf3 * 3 * 4 + j * 4 + 3] = transparency;
	    _f3_vertices[nf3 * 3 * 3 + j * 3 + 0] = face3[i].p[j].x;
	    _f3_vertices[nf3 * 3 * 3 + j * 3 + 1] = face3[i].p[j].y;
	    _f3_vertices[nf3 * 3 * 3 + j * 3 + 2] = face3[i].p[j].z;
	    _f3_normals[nf3 * 3 * 3 + j * 3 + 0] = face3[i].n[j].x;
	    _f3_normals[nf3 * 3 * 3 + j * 3 + 1] = face3[i].n[j].y;
	    _f3_normals[nf3 * 3 * 3 + j * 3 + 2] = face3[i].n[j].z;
	  }
	  nf3++;
	}
    }
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, _f3_vertices);
    glNormalPointer(GL_FLOAT, 0, _f3_normals);
    glColorPointer(4, GL_FLOAT, 0, _f3_colours);
    glDrawArrays(GL_TRIANGLES, 0, nf3*3);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    free(_f3_colours);
    free(_f3_normals);
    free(_f3_vertices);

#else  

    glBegin(GL_TRIANGLES);
    for (i=0;i<nface3;i++) {
#if defined(BUILDING_S2PLOT)
      if (doscreen) {
	if (strlen(_s2_doingScreen) &&
	    strstr(face3[i].whichscreen, _s2_doingScreen)) {
	  // screen geometry
	  for (j = 0; j < 3; j++) {
	    s2UnProject(view[0] + view[2] * face3[i].p[j].x + 0.5, 
			 view[1] + view[3] * face3[i].p[j].y + 0.5, 
			 face3[i].p[j].z, 
			 model, proj, view, &vtx, &vty, &vtz);
	    _glColor4f(face3[i].colour[j].r, 
			      face3[i].colour[j].g,
			      face3[i].colour[j].b, transparency);
	    glVertex3f(vtx, vty, vtz);
	  }
	}
      } else if (!strlen(face3[i].whichscreen)) 
#endif
	{
	  for (j=0;j<3;j++) {
	    glNormal3f(face3[i].n[j].x,face3[i].n[j].y,face3[i].n[j].z);
	    _glColor4f(face3[i].colour[j].r,face3[i].colour[j].g,face3[i].colour[j].b,transparency);
	    glVertex3f(face3[i].p[j].x,face3[i].p[j].y,face3[i].p[j].z);
	  }
	}
    }
    glEnd();

#endif // #if defined(S2USEARRAYS)

  }
  
  // 4 vertex faces 
  if (nface4 > 0) {
    glBegin(GL_QUADS);
    for (i=0;i<nface4;i++) {
#if defined(BUILDING_S2PLOT)
      if (doscreen) {
	if (strlen(_s2_doingScreen) && 
	    strstr(face4[i].whichscreen, _s2_doingScreen)) {
	  // screen geometry
	  for (j = 0; j < 4; j++) {
	    s2UnProject(view[0] + view[2] * face4[i].p[j].x + 0.5, 
			 view[1] + view[3] * face4[i].p[j].y + 0.5, 
			 face4[i].p[j].z, 
			 model, proj, view, &vtx, &vty, &vtz);
	    _glColor4f(face4[i].colour[j].r, 
			      face4[i].colour[j].g,
			      face4[i].colour[j].b, transparency);
	    glVertex3f(vtx, vty, vtz);
	  }
	}
      } else if (!strlen(face4[i].whichscreen))
#endif
	{
	  // normal 3d geometry
	  for (j=0;j<4;j++) {
	    glNormal3f(face4[i].n[j].x,face4[i].n[j].y,face4[i].n[j].z);
	    _glColor4f(face4[i].colour[j].r,face4[i].colour[j].g,face4[i].colour[j].b,transparency);
	    glVertex3f(face4[i].p[j].x,face4[i].p[j].y,face4[i].p[j].z);
	  }
	}
      
    }
    glEnd();
  }
  

  // Turn of lighting for the points and lines 
  glDisable(GL_LIGHTING);
  
  // Labels 
  for (i=0;i<nlabel;i++) {
    CreateLabelVector(label[i].s,label[i].p,label[i].right,label[i].up,linelist,&nlinelist);
    _glColor4f(label[i].colour.r,label[i].colour.g,label[i].colour.b,transparency);

#if defined(BUILDING_S2PLOT)
    if (doscreen) {
      if (strlen(_s2_doingScreen) && 
	  strstr(label[i].whichscreen, _s2_doingScreen)) {
	for (j = 0; j < nlinelist; j += 2) {
	  glBegin(GL_LINES);
	  s2UnProject(view[0] + view[2] * linelist[j].x + 0.5, 
		       view[1] + view[3] * linelist[j].y + 0.5,
		       linelist[j].z,
		       model, proj, view, &vtx, &vty, &vtz);
	  glVertex3f(vtx, vty, vtz);
	  s2UnProject(view[0] + view[2] * linelist[j+1].x + 0.5, 
		       view[1] + view[3] * linelist[j+1].y + 0.5,
		       linelist[j+1].z,
		       model, proj, view, &vtx, &vty, &vtz);	  
	  glVertex3f(vtx, vty, vtz);
	  glEnd();
	}
      }
    } else if (!strlen(label[i].whichscreen)) {
#endif 
      for (j=0;j<nlinelist;j+=2) {
	glBegin(GL_LINES);
	glVertex3f(linelist[j].x,linelist[j].y,linelist[j].z);
	glVertex3f(linelist[j+1].x,linelist[j+1].y,linelist[j+1].z);
	glEnd();
      }
#if defined(BUILDING_S2PLOT)
    }
#endif
  }
  
  // Points 
  if (ndot > 0) {
    oldsize = dot[0].size;
#if defined(BUILDING_VIEWER)
    if (options.stereo == INTERSTEREO) {
      glPointSize(options.pointscale*oldsize<2. ? 2. : options.pointscale*oldsize);
    } else {
      glPointSize(options.pointscale*oldsize);
    }
#else
    glPointSize(options.pointscale*oldsize);
#endif
    glBegin(GL_POINTS);
    for (i=0;i<ndot;i++) {
      if (oldsize != dot[i].size) {
	glEnd();
	oldsize = dot[i].size;
#if defined(BUILDING_VIEWER)
    if (options.stereo == INTERSTEREO) {
      glPointSize(options.pointscale*oldsize<2. ? 2. : options.pointscale*oldsize);
    } else {
      glPointSize(options.pointscale*oldsize);
    }
#else
    glPointSize(options.pointscale*oldsize);
#endif
	glBegin(GL_POINTS);
      }
      _glColor4f(dot[i].colour.r,dot[i].colour.g,dot[i].colour.b,transparency);

#if defined(BUILDING_S2PLOT)
      if (doscreen) {
	if (strlen(_s2_doingScreen) && 
	    strstr(dot[i].whichscreen, _s2_doingScreen)) {
	  s2UnProject(view[0] + view[2] * dot[i].p.x, 
		       view[1] + view[3] * dot[i].p.y, 
		       dot[i].p.z, 
		       model, proj, view, &vtx, &vty, &vtz);
	  glVertex3f(vtx, vty, vtz);
	}
      } else if (!strlen(dot[i].whichscreen))
#endif
	{
	  glVertex3f(dot[i].p.x,dot[i].p.y,dot[i].p.z);
	}
    }
    glEnd();
    glPointSize(options.pointscale);
  }
  
  // Lines 
  if (nline > 0) {

    // addition DGB 20140423 to support transparent lines
    // NB this implementation of transparency is a hack without proper sorting, 
    // and only additive transparency (= result approaches 1.0)
#if defined(BUILDING_S2PLOT)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
#endif

    //oldsize = line[0].width;
    oldsize = line[0].width - 3; // make it NOT equal line[0].width
    //glLineWidth(options.linescale*oldsize);
    glBegin(GL_LINES);
    for (i=0;i<nline;i++) {
#if defined(BUILDING_S2PLOT)
      if (oldsize != line[i].width || 
	  oldstipplefactor != line[i].stipple_factor ||
	  oldstipplepattern != line[i].stipple_pattern) {
#else 
      if (oldsize != line[i].width) {
#endif
	glEnd();
	oldsize = line[i].width;
	glLineWidth(options.linescale*oldsize);
#if defined(BUILDING_S2PLOT)
	oldstipplefactor = line[i].stipple_factor;
	oldstipplepattern = line[i].stipple_pattern;
	if (oldstipplefactor > 0) {
	  glLineStipple(oldstipplefactor, oldstipplepattern);
	  glEnable(GL_LINE_STIPPLE);
	} else {
	  glDisable(GL_LINE_STIPPLE);
	}
#endif
	glBegin(GL_LINES);
      }
#if defined(BUILDING_S2PLOT)
      if (doscreen) {
	if (strlen(_s2_doingScreen) && 
	    strstr(line[i].whichscreen, _s2_doingScreen)) {
	  for (j = 0; j < 2; j++) {
	    s2UnProject(view[0] + view[2] * line[i].p[j].x + 0.5, 
			 view[1] + view[3] * line[i].p[j].y + 0.5, 
			 line[i].p[j].z, 
			 model, proj, view, &vtx, &vty, &vtz);
	    _glColor4f(line[i].colour[j].r, 
			      line[i].colour[j].g,
		       line[i].colour[j].b, line[i].alpha /*transparency*/);
	    glVertex3f(vtx, vty, vtz);
	  }		
	} 
      } else if (!strlen(line[i].whichscreen)) 
#endif
	{
	  for (j=0;j<2;j++) {
#if defined(BUILDING_S2PLOT) 
	    if (line[i].alpha < 0.999) {
	      glEnable(GL_BLEND);
	    } else {
	      glDisable(GL_BLEND);
	    }
	    _glColor4f(line[i].colour[j].r,line[i].colour[j].g,line[i].colour[j].b,line[i].alpha /*transparency*/);

#else
	    _glColor4f(line[i].colour[j].r,line[i].colour[j].g,line[i].colour[j].b,transparency);
#endif
	    glVertex3f(line[i].p[j].x,line[i].p[j].y,line[i].p[j].z);
	  }
	}
    }  
    glEnd();
    glLineWidth(options.linescale);

#if defined(BUILDING_S2PLOT)
    glDisable(GL_BLEND);
#endif
  }
#if defined(BUILDING_S2PLOT)
  glDisable(GL_LINE_STIPPLE);
#endif  

  // Draw the bounding box
  if (options.showboundingbox) {
    glPushMatrix();
    glScalef(options.scalefactor.x,options.scalefactor.y,options.scalefactor.z);
    CreateBox(pmin,pmax,0);
    glPopMatrix();
  }
  
  // Finally turn lighting back on for the textured objects 
  // Draw the textured objects last - for transparency reasons
  if (options.rendermode != SHADE_FLAT)
    glEnable(GL_LIGHTING);
  
  // Textured balls
  for (i=0;i<nballt;i++) {
    if (options.showtextures && (options.rendermode != WIREFRAME)) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,ballt[i].textureid);
    }
    _glColor4f(white.r,white.g,white.b,transparency);

#if defined(BUILDING_S2PLOT)
    if (doscreen) {
      if (strlen(_s2_doingScreen) && 
	  strstr(ballt[i].whichscreen, _s2_doingScreen)) {
	// screen geometry
	s2UnProject(view[0] + view[2] * ballt[i].p.x + 0.5,
		     view[1] + view[3] * ballt[i].p.y + 0.5, 
		     ballt[i].p.z, 
		     model, proj, view, &vt.x, &vt.y, &vt.z);
	CreateAPlanet(vt, ballt[i].r, options.sphereresolution, 1, 1,
		      ballt[i].texture_phase, ballt[i].axis,
		      ballt[i].rotation);
      }
    } else if (!strlen(ballt[i].whichscreen))
#endif
#if defined(BUILDING_S2PLOT)
      CreateAPlanet(ballt[i].p,ballt[i].r,options.sphereresolution,1,1,
		    ballt[i].texture_phase, ballt[i].axis,
		    ballt[i].rotation);
#elif defined(BUILDING_VIEWER)
    XYZ defaxis = {0., 1., 0.};
    CreateAPlanet(ballt[i].p, ballt[i].r, options.sphereresolution, 1, 1,
		  0., defaxis, 0.);
#endif
    if (options.showtextures && (options.rendermode != WIREFRAME)) {
      glDisable(GL_TEXTURE_2D);
    }
  }

  // Textured faces
#if defined(BUILDING_S2PLOT)
  // turn on blending with a simple addition blend
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
#endif

  for (i=0;i<nface4t;i++) {
    if (!options.showtextures || options.rendermode == WIREFRAME) {
      glBegin(GL_QUADS);
      _glColor4f(face4t[i].colour.r,face4t[i].colour.g,face4t[i].colour.b,
#if defined(BUILDING_S2PLOT)
			face4t[i].alpha);
#else
      transparency);
#endif

      for (j=0;j<4;j++) 
	glVertex3f(face4t[i].p[j].x,face4t[i].p[j].y,face4t[i].p[j].z);
      glEnd();
      continue;
    }
#if !defined(BUILDING_S2PLOT)
    if (face4t[i].trans == 't')
      glDepthMask(GL_FALSE);
#else
    if (face4t[i].trans == 't') {
      glDepthMask(GL_FALSE);
      // turn on blending with a simple addition blend
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    } else if (face4t[i].trans == 's') {
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
    }
#endif
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,face4t[i].textureid);
    //if (face4t[i].screen && _s2_screenEnabled) {
#if defined(BUILDING_S2PLOT)
    if (face4t[i].scale < 1.) {
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
      glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    }      
    if (doscreen) {
      if (strlen(_s2_doingScreen) && 
	  strstr(face4t[i].whichscreen, _s2_doingScreen)) {
	
	glBegin(GL_QUADS);
	_glColor4f(face4t[i].colour.r,face4t[i].colour.g,
			  face4t[i].colour.b,face4t[i].alpha);
	for (j = 0; j < 4; j++) {
	  s2UnProject(view[0] + view[2] * face4t[i].p[j].x + 0.5, 
		       view[1] + view[3] * face4t[i].p[j].y + 0.5, 
		       face4t[i].p[j].z, 
		       model, proj, view, &vtx, &vty, &vtz);
	  switch (j) {
	  case 0:
	    glTexCoord2f(0.0,1.0/face4t[i].scale);
	    break;
	  case 1:
	    glTexCoord2f(1.0/face4t[i].scale,1.0/face4t[i].scale);
	    break;
	  case 2:
	    glTexCoord2f(1.0/face4t[i].scale,0.0);
	    break;
	  case 3:
	    glTexCoord2f(0.0,0.0);
	    break;
	  }
	  glVertex3f(vtx, vty, vtz);
	}	   
	glEnd();
		
      }
      
    } else if (!strlen(face4t[i].whichscreen))
     
#endif
      {
	glBegin(GL_QUADS);
	_glColor4f(face4t[i].colour.r,face4t[i].colour.g,
			  face4t[i].colour.b,
#if defined(BUILDING_S2PLOT)
			  face4t[i].alpha);
#else
	transparency);
#endif

	normal = CalcNormal(face4t[i].p[0],face4t[i].p[1],face4t[i].p[2]);
	glNormal3f(normal.x,normal.y,normal.z);
	glTexCoord2f(0.0,1.0/face4t[i].scale);
	glVertex3f(face4t[i].p[0].x,face4t[i].p[0].y,face4t[i].p[0].z);
	glTexCoord2f(1.0/face4t[i].scale,1.0/face4t[i].scale);
	glVertex3f(face4t[i].p[1].x,face4t[i].p[1].y,face4t[i].p[1].z);
	glTexCoord2f(1.0/face4t[i].scale,0.0);
	glVertex3f(face4t[i].p[2].x,face4t[i].p[2].y,face4t[i].p[2].z);
	glTexCoord2f(0.0,0.0);
	glVertex3f(face4t[i].p[3].x,face4t[i].p[3].y,face4t[i].p[3].z);
	glEnd();
      }
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_TRUE);
  }


  // 3 vertex transparent faces 
#if defined(BUILDING_S2PLOT)
  if (nface3a > 0) {
    for (i=0;i<nface3a;i++) {
      if (face3a[i].trans == 't') {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      } else if (face3a[i].trans == 's') {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      } else {
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
      }
      if (doscreen) {
	if (strlen(_s2_doingScreen) && 
	    strstr(face3a[i].whichscreen, _s2_doingScreen)) {
	  glBegin(GL_TRIANGLES);
	  for (j = 0; j < 3; j++) {
	    /* screen z coord in glUnProject: 0.0 = near plane,
	     * 1.0 = far plane (ie. it's a depth buffer
	     * coordinate */
	    s2UnProject(view[0] + view[2] * face3a[i].p[j].x, 
			 view[1] + view[3] * face3a[i].p[j].y, 
			 face3a[i].p[j].z, 
			 model, proj, view, &vtx, &vty, &vtz);
	    _glColor4f(face3a[i].colour[j].r,
			      face3a[i].colour[j].g,
			      face3a[i].colour[j].b,face3a[i].alpha[j]);
	    glVertex3f(vtx, vty, vtz);
	  }
	  glEnd();
	  
	}
      } else if (!strlen(face3a[i].whichscreen)) {
	glBegin(GL_TRIANGLES);
	for (j=0;j<3;j++) {
	  glNormal3f(face3a[i].n[j].x,face3a[i].n[j].y,face3a[i].n[j].z);
	  _glColor4f(face3a[i].colour[j].r,face3a[i].colour[j].g,face3a[i].colour[j].b,face3a[i].alpha[j]);
	  glVertex3f(face3a[i].p[j].x,face3a[i].p[j].y,face3a[i].p[j].z);
	}
	glEnd();
      }
    }
  }
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);

#if defined(S2_3D_TEXTURES)
  // polygon textures with 3d texture coords
  if (ntexpoly3d > 0) {
    for (i = 0; i < ntexpoly3d; i++) {
      if (texpoly3d[i].trans == 't') {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      } else if (texpoly3d[i].trans == 's') {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      } else {
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
      }

      glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, texpoly3d[i].texid);
      glTexParameterf(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
      
      if (doscreen) {
	if (strlen(_s2_doingScreen) &&
	    strstr(texpoly3d[i].whichscreen, _s2_doingScreen)) {
	  _s2warn("MakeGeometry", "screen-meshed textures not supported");
	}
      } else if (!strlen(texpoly3d[i].whichscreen)) {

	glBegin(GL_POLYGON);
	glColor4f(1., 1., 1., texpoly3d[i].alpha);
	normal = CalcNormal(texpoly3d[i].verts[0],
			    texpoly3d[i].verts[1],
			    texpoly3d[i].verts[2]);
	glNormal3f(normal.x, normal.y, normal.z);
	int io;
	for (io = 0; io < texpoly3d[i].nverts; io++) {
	  glTexCoord3f(texpoly3d[i].texcoords[io].x,
		       texpoly3d[i].texcoords[io].y,
		       texpoly3d[i].texcoords[io].z);
	  glVertex3f(texpoly3d[i].verts[io].x,
		     texpoly3d[i].verts[io].y,
		     texpoly3d[i].verts[io].z);
	}
	glEnd();

      }
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_3D);
    glDepthMask(GL_TRUE);
  }
#endif

  // textured meshes
  if (ntexmesh > 0) {
    for (i = 0; i < ntexmesh; i++) {
      if (texmesh[i].trans == 't') {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      } else if (texmesh[i].trans == 's') {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      } else {
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
      }

      if (texmesh[i].texid > 0) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texmesh[i].texid);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
      }

      if (doscreen) {
	if (strlen(_s2_doingScreen) &&
	    strstr(texmesh[i].whichscreen, _s2_doingScreen)) {
	  _s2warn("MakeGeometry", "screen-meshed textures not supported");
	}
      } else if (!strlen(texmesh[i].whichscreen)) {

	glBegin(GL_TRIANGLES);
	glColor4f(1.0, 1.0, 1.0, texmesh[i].alpha);
	int j,k;
	for (j = 0; j < texmesh[i].nfacets; j++) {
	  for (k = 0; k < 3; k++) {
	    int vx = texmesh[i].facets[j*3+k];
	    
	    // glNormal3f has to go onto the stack BEFORE glVertex3f !!!
	    if (texmesh[i].nnorms == texmesh[i].nverts) {
	      glNormal3f(texmesh[i].norms[vx].x,
			 texmesh[i].norms[vx].y,
			 texmesh[i].norms[vx].z);
	    }

	    if (texmesh[i].texid > 0) {
	      int tx = texmesh[i].facets_vtcs[j*3+k];
	      glTexCoord2f(texmesh[i].vtcs[tx].x,
			   texmesh[i].vtcs[tx].y);
	    }


	    glVertex3f(texmesh[i].verts[vx].x, 
		       texmesh[i].verts[vx].y,
		       texmesh[i].verts[vx].z);

	  }
	}
	glEnd();
      }
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_TRUE);
  }

#endif
  

  // transparent points
#if defined(BUILDING_S2PLOT)


  if (ntrdot > 0) {
    for (i = 0; i < ntrdot; i++) {
      if (trdot[i].trans == 't') {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      } else if (trdot[i].trans == 's') {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      } else {
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
      }
      glPointSize(options.pointscale * trdot[i].size);
      glBegin(GL_POINTS);
      _glColor4f(trdot[i].colour.r,trdot[i].colour.g,trdot[i].colour.b,trdot[i].alpha);
      if (doscreen) {
	if (strlen(_s2_doingScreen) && 
	    strstr(trdot[i].whichscreen, _s2_doingScreen)) {
	  s2UnProject(view[0] + view[2] * trdot[i].p.x, 
		       view[1] + view[3] * trdot[i].p.y, 
		       trdot[i].p.z, 
		       model, proj, view, &vtx, &vty, &vtz);
	  glVertex3f(vtx, vty, vtz);
	  
	}
      } else if (!strlen(trdot[i].whichscreen)) {
	glVertex3f(trdot[i].p.x,trdot[i].p.y,trdot[i].p.z);
      }
      glEnd();
    }
    glPointSize(options.pointscale);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
  }

#endif
  
#if defined(BUILDING_S2PLOT)
  // turn off blending
  glDisable(GL_BLEND);
#endif
  
  glPopMatrix();
  glDepthMask(GL_TRUE);


#if !defined(BUILDING_S2PLOT)
  if (doupdate)
    glEndList();
#else
#if defined(S2LISTS)
  // s2 change: only do lists for static geom
  if (!_s2_dynamicEnabled) {
    if (!_s2_retain_lists) {
      // s2 change: allow dynamic geometry
      _s2_startDynamicGeometry(FALSE);
      MakeGeometry(FALSE, doscreen, eye);
      _s2_endDynamicGeometry();
    } else if (doupdate) {
      glEndList();
    }
    return;
  }
#else
  if (!_s2_dynamicEnabled) {
    _s2_startDynamicGeometry(FALSE);

    if (options.debug) {
      fprintf(stderr, "(recursively) calling MakeGeom(FALSE, %d)\n", doscreen);
    }
    MakeGeometry(FALSE, doscreen, eye);
    
    _s2_endDynamicGeometry();
    return;
  }
#endif
  
// CONDITIONAL added DGB 20141101 - probably redundant as 
// s2priv_drawBillboards probably checks
if (_s2_dynamicEnabled) {
  /* draw the billboards */
  if (nbboard) {
    _s2priv_drawBillboards();
  }

  if (nbbset) {
    _s2priv_drawBBsets();
  }
  
  /* draw the handles if they are visible */
  if (_s2_handle_vis && nhandle) {
    // draw 3d handles
    _s2priv_drawHandles(FALSE);
    // draw screen handles
    _s2priv_drawHandles(TRUE);
  }
 }

// CONDITIONAL added DGB 20141101 - definitely needed, oglcb was
// being called three times per eye/screen.
if (_s2_dynamicEnabled && !doscreen) {
  /* and call the direct OpenGL callback if one is present */
  if (_s2_oglcb) {
    _s2_oglcb(&eye);
  }
 }
  
  /* call the entry/exit fade in routine */
  //_s2_fadeinout();
#endif

}

#include "s2geomviewer.c"
  
#if defined(BUILDING_VIEWER)
#include "s2common.c"
#endif

#if defined(S2_NO_S2GEOMVIEWER)
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
#endif

#if defined(BUILDING_S2PLOT)
void _s2_prompt_kbdfunc(unsigned char key, int x, int y) {
  MTX_LOCK(&mutex);

  if (key == 27) {
    _s2priv_finiPrompt();
    MTX_ULCK(&mutex);
    return;
  } else if (key == 13) {
    char thecmd[S2PROMPT_LEN+1];
    strncpy(thecmd, _s2prompt, _s2prompt_length);
    thecmd[_s2prompt_length] = '\0';
    if (_s2_promptcbx) {
      _s2_promptcbx(thecmd, _s2_promptcbx_data);
    }
    _s2priv_finiPrompt();
    MTX_ULCK(&mutex);
    return;
  } else if ((key == 8) || (key == 127)) {
    if (_s2prompt_length > 0) {
      _s2prompt_length--;
    }
    MTX_ULCK(&mutex);
    return;
  }

  if (_s2prompt_length < (S2PROMPT_LEN-1)) {
    _s2prompt[_s2prompt_length] = key;
    _s2prompt_length++;
  }
  MTX_ULCK(&mutex);
}
#endif

void HandleKeyboard(unsigned char key, int x, int y) {
  _s2_process_key(key, x, y, 1);  // 4th arg 1 means glutModifiers ok to call and
  // x, y meaningful
}

/*
   Deal with plain key strokes
	The x and y are the screen coordinate of the mouse at the time
*/
void _s2_process_key(unsigned char key,int x, int y, int modifiers)
// if modifiers, then x and y can be used and s2winModifiers can be called
// (ie. this is a result of a call within a HandleKeyboard callback)
{
  MTX_LOCK(&mutex);
  static int shift_esc = -1;
  if (shift_esc < 0) {
    char *stmp = getenv("S2PLOT_ESCQUIT");
    if (stmp) {
      shift_esc = 0; // shift not required for quit
    } else {
      shift_esc = 1; // shift required for quit (default)
    }
  }
  XYZ origin = {0.0,0.0,0.0};
  
#if defined(BUILDING_S2PLOT)
  /* even if user has own keyboard function, Shift-ESC should still exit */
  //if ((key == 27) && s2winGetModifiers() & S2_ACTIVE_SHIFT) {
  if (modifiers && (key == 27) && (!shift_esc || (s2winGetModifiers() & S2_ACTIVE_SHIFT))) {
    _s2_fadestatus = 3;
    MTX_ULCK(&mutex);
    return;
  }
  if (_s2_user_keys) {
    if (_s2_user_keys(&key)) {
      // consumed, so return
      MTX_ULCK(&mutex);
      return;
    }
  }
#endif
  
  if (_device_keybd) {
    if ((_device_keybd)(key)) {
      // consumed, so return
      MTX_ULCK(&mutex);
      return;
    }
  }
  
  switch (key) {
  case 27: 									/* ESC 			*/
#if defined(BUILDING_S2PLOT)
    if (modifiers && (s2winGetModifiers() & S2_ACTIVE_SHIFT)) {
      _s2_fadestatus = 3;
    }
#else
    CleanExit();
#endif
    break;
    
  case 'Q':									/* Quit 			*/
#if !defined(BUILDING_S2PLOT)
  case 'q': 
    CleanExit();
#else
    _s2_fadestatus = 3;
#endif
    break;
    
#if defined(BUILDING_S2PLOT)
  case '~':
    /* enter command prompt mode! */
    if (_s2_promptcbx) {
      _s2priv_initPrompt();
    } else {
      _s2warnk('~', "this application doesn't use the prompt");
    }
    break;
  case '!':
    /* toggle fullscreen / constrained screen */
    _s2priv_togglefs();
    break;
#endif
    
  case 'a':									/* Start/stop autospin */
  case 'A':
    if (options.autospin.x != 0 || options.autospin.y != 0 || options.autospin.z != 0) {
      options.autospin = origin;
    } else {
      options.autospin.x = 1; 
      options.autospin.y = 0; 
      options.autospin.z = 0;
    }
    break;
  case 'd':
    options.debug = !options.debug;
    break;
#if !defined(BUILDING_S2PLOT)
  case 'D':									/* Delete everything */
    DeleteGeometry();
    break;
#endif

  case 'h':									/* Go home		*/
  case 'H':
#if defined(BUILDING_S2PLOT)
    if (_s2_transcam_enable) {
      camera = options.camerahome;
    }
#else
    camera = options.camerahome;
#endif
    break;

  case '1':									/* Special camera views */
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
#if defined(BUILDING_S2PLOT)
  case '8':
  case '9':
  case '0':
    if (_s2_numcb) {
      int thenum = key - '0';
      _s2_numcb(&thenum);
    } else {
      /* no callback registered, so act "normally" */
      if (_s2_transcam_enable) {
	CameraHome((key-'1')+3);
      }
    }
#else
    CameraHome((key-'1')+3);
#endif
    break;
  case 'f':      							/* Focus */
  case 'F':
    CameraHome(FOCUS);
    break;
  case '[':
    RotateCamera(0.0,0.0,-1.0,KEYBOARDCONTROL);
    break;
  case ']':
    RotateCamera(0.0,0.0,1.0,KEYBOARDCONTROL);
    break;
  case 'i':									/* Translate camera up */
    TranslateCamera(0.0,1.0,KEYBOARDCONTROL);
    break;
  case 'k':                           /* Translate camera down */
    TranslateCamera(0.0,-1.0,KEYBOARDCONTROL);
    break;
  case 'j':                           /* Translate camera left */
    TranslateCamera(-1.0,0.0,KEYBOARDCONTROL);
    break;
  case 'l':                           /* Translate camera right */
    TranslateCamera(1.0,0.0,KEYBOARDCONTROL);
    break;
  case '-':									/* Move backward or increase speed */
  case '_':
    if (options.interaction == INSPECT) {
#if defined(BUILDING_S2PLOT)
      FlyCamera(-50.0 * ss2qcs());
#else
      FlyCamera(-50.0);
#endif
    } else {
      if (ABS(camera.speed) < EPSILON) {
	camera.speed = -1;
      } else if (ABS(camera.speed-1) < EPSILON) {
	camera.speed = 0;
      } else {
	camera.speed--;
      }
    }
    break;
  case '+':									/* Move forward or increase speed */
  case '=':
    if (options.interaction == INSPECT) {
#if defined (BUILDING_S2PLOT)
      FlyCamera(50.0 * ss2qcs());
#else 
      FlyCamera(50.0);
#endif
    } else {
      if (ABS(camera.speed) < EPSILON) {
	camera.speed = 1;
      } else if (ABS(camera.speed+1) < EPSILON) {
	camera.speed = 0;
      } else {
	camera.speed++;
      }
    }
    break;
#if !defined(BUILDING_S2PLOT)
  case 'c':    								/* Write the camera position to log */
    AppendCameraPosition();
    break;
#endif
  case 's':
    break;
#if defined(BUILDING_S2PLOT)
  case 'C':
    /* toggle crosshair mode: only on certain devices */
    if (_s2_devcap & _S2DEVCAP_CROSSHAIR) {
      ss2txh(-1);
    } else {
      _s2warnk('C', "crosshair not available on this device");
    }
    break;
  case 'S':
    /* toggle selection mode: only on certain devices */
    if (_s2_devcap & _S2DEVCAP_SELECTION) {
      //_s2_handle_vis = (_s2_handle_vis + 1) % 2;
      cs2thv(-1);
    } else {
      _s2warnk('S', "selection mode (handles) not available on this device");
    }
    break;
#endif
    
#if defined(BUILDING_S2PLOT)
    /* S2PLOT changes */
  case 'n':
  case 'N':
    _s2_showannotations = !_s2_showannotations;
    break;
    
  case 'z':
  case 'Z':
    _s2_animation = !_s2_animation;
    break;
    
  case ' ':
    _s2_callbackkey++;
    break;
    
  case 13: /* Enter key */
    if (modifiers && (s2winGetModifiers() & S2_ACTIVE_SHIFT)) {
      _s2_skip = 1; /* set flag to stop calling event loop */
    }
    break;
    
  case 9: /* TAB key */
    /* move "focus" to next panel */
    if (_s2_npanels > 1) {
      xs2cp((_s2_activepanel + 1) % _s2_npanels);
    }
    break;
    
  case 'r': /* rendermode cycle */
  case 'R':
    {
      int rmode = ns2grenmode();
      switch (rmode) {
      case WIREFRAME:
	ns2srenmode(SHADE_FLAT);
	break;
      case SHADE_FLAT:
	ns2srenmode(SHADE_DIFFUSE);
	break;
      case SHADE_DIFFUSE:
	ns2srenmode(SHADE_SPECULAR);
	break;
      case SHADE_SPECULAR:
	ns2srenmode(WIREFRAME);
	break;
      default:
	_s2warnk('r', "impossible rendering state!");
	ns2srenmode(WIREFRAME);
	break;
      }
    }
    break;
    
  case '*': /* increase autospin rate */
    ss2sss(ss2qss() * 2.5);
    break;
  case '/': /* decrease autospin rate */
    ss2sss(ss2qss() * 0.4);
    break;

  case '>': /* increase camera speed */
    ss2scs(ss2qcs() * 2.5);
    break;
  case '<': /* decrease camera speed */
    ss2scs(ss2qcs() * 0.4);
    break;

  case '(': /* wider aperture - "zoom out" */
    camera.aperture *= 1.0/0.98 * ss2qcs();
    break;
  case ')': /* narrower aperture - "zoom in" */
    camera.aperture *= 0.98 * ss2qcs();
    break;
    
  case 'x':
  case 'X': /* toggle camera mode between INSPECT and FLY */
    if (options.interaction == FLY) {
      ss2sas(0); // stop camera rotation
      options.interaction = INSPECT;
    } else {
      ss2sas(0); // stop camera rotation
      options.interaction = FLY;
      
    }
    break;
    
  case 'W': /* write vrml */
    writeVRML20();
    break;
    
  case 'P': /* write PRC */
    writePRC();
    break;

  case '{': /* reduce eye separation */
    ss2sess(ss2qess() / 1.1);
    break;
  case '}': /* increase eye separation */
    ss2sess(ss2qess() * 1.1);
    break;
    
#endif
    
  }
  
  MTX_ULCK(&mutex);
}

void _s2_setVolume(int vol) {
#if defined(S2DARWIN)
  char cmd[255];
  sprintf(cmd, "/usr/bin/osascript -e \"set Volume %d\"", vol);
  system(cmd);
#endif
}

/*
   Deal with special key strokes
	The exact operation depends on whether we're inspecting or flying
	In inspect mode they rotate about the focal point
	In fly mode they direct the camera view direction
*/
void HandleSpecialKeyboard(int key,int x,int y)
{
  MTX_LOCK(&mutex);

  XYZ origin = {0.0,0.0,0.0};

   switch (key) {
   case S2_KEY_LEFT:  
		RotateCamera(-1.0,0.0,0.0,KEYBOARDCONTROL); 
		if (options.autospin.x != 0 || options.autospin.y != 0 || options.autospin.z != 0) {
			options.autospin = origin;
			options.autospin.x = 1;
		}
		break;

   case S2_KEY_RIGHT: 
		RotateCamera(1.0,0.0,0.0,KEYBOARDCONTROL);  
      if (options.autospin.x != 0 || options.autospin.y != 0 || options.autospin.z != 0) {
			options.autospin = origin;
         options.autospin.x = -1; 
		}
		break;

   case S2_KEY_UP:    
		if (options.interaction == WALK) {
			TranslateCamera(0.0,1.0,KEYBOARDCONTROL);
			break;
		}
		RotateCamera(0.0,1.0,0.0,KEYBOARDCONTROL);  
      if (options.autospin.x != 0 || options.autospin.y != 0 || options.autospin.z != 0) {
			options.autospin = origin;
         options.autospin.y = 1;
		}
		break;

   case S2_KEY_DOWN:  
      if (options.interaction == WALK) {
         TranslateCamera(0.0,-1.0,KEYBOARDCONTROL);
         break;
      }
		RotateCamera(0.0,-1.0,0.0,KEYBOARDCONTROL); 
      if (options.autospin.x != 0 || options.autospin.y != 0 || options.autospin.z != 0) {
			options.autospin = origin;
         options.autospin.y = -1;
		}
		break;
   case S2_KEY_F1:
#if defined(BUILDING_VIEWER)
      options.showhelpinfo = !options.showhelpinfo;
#else
      if (s2winGetModifiers() & S2_ACTIVE_SHIFT) {
	_s2_setVolume(0);
      } else {
	options.showhelpinfo = (options.showhelpinfo + 1) % 3;
	if ((options.showhelpinfo == 2) && !_s2_customhelpstr) {
	  options.showhelpinfo = 0;
	}
      }
#endif
		break;
   case S2_KEY_F2:
      if (s2winGetModifiers() & S2_ACTIVE_SHIFT) {
	_s2_setVolume(1);
      } else {     
	options.showboundingbox = !options.showboundingbox;
      }
      break;
   case S2_KEY_F3:
      if (s2winGetModifiers() & S2_ACTIVE_SHIFT) {
	_s2_setVolume(2);
      }
      break;
   case S2_KEY_F4:
      if (s2winGetModifiers() & S2_ACTIVE_SHIFT) {
	_s2_setVolume(3);
      }
      break;
   case S2_KEY_F5:
      if (s2winGetModifiers() & S2_ACTIVE_SHIFT) {
	_s2_setVolume(4);
      } else {
#if defined(BUILDING_S2PLOT)
	_s2_recstate = 1;
#endif
      }
      break;
   case S2_KEY_F6:
      if (s2winGetModifiers() & S2_ACTIVE_SHIFT) {
	_s2_setVolume(5);
      } else {
#if defined(BUILDING_S2PLOT)
	_s2_recstate = 2;
#endif
      }
      break;
   case S2_KEY_F7:
      if (s2winGetModifiers() & S2_ACTIVE_SHIFT) {
	_s2_setVolume(6);
      } else {
#if defined(BUILDING_S2PLOT)
	if (_s2_recstate == 3) {
	  _s2_recstate = 0;
	} else {
	  _s2_recstate = 3;
	}
#endif
      }
      break;
   case S2_KEY_F8:
      if (s2winGetModifiers() & S2_ACTIVE_SHIFT) {
	_s2_setVolume(7);
      }
      break;
#if defined(BUILDING_VIEWER)
   case S2_KEY_F9:
		options.windowdump = 3;
      break;
   case S2_KEY_F10:
		options.windowdump = 1;
      break;
   case S2_KEY_F11:
		options.makeqtvr = TRUE;
      break;
   case S2_KEY_F12:
		options.recordimages = !options.recordimages;
      break;
#endif
   case S2_KEY_PAGE_UP:
		break;
   case S2_KEY_PAGE_DOWN:
      break;
	case S2_KEY_HOME:
#if defined(BUILDING_S2PLOT)
	  if (_s2_transcam_enable) {
	    camera = options.camerahome;
	  }
#else
	  camera = options.camerahome;
#endif
		break;
	case S2_KEY_END:
#if !defined(BUILDING_S2PLOT)
	  CleanExit();
#else
	  //_s2_fadestatus = 3;
#endif
		break;
	case S2_KEY_INSERT:
		break;
   }

   MTX_ULCK(&mutex);
}

/*
   Translate (pan) the camera view point
	In response to i,j,k,l, and arrow keys
*/
void TranslateCamera(double ix,double iy,int source)
{
#if defined(BUILDING_S2PLOT) 
  if (!_s2_transcam_enable) {
    return;
  }
#endif
   XYZ vp,vu,vd,pr;
   XYZ right;
   double delta;

   vu = camera.vu;
   Normalise(&vu);
   vp = camera.vp;
   vd = camera.vd;
	pr = camera.pr;
   Normalise(&vd);
   CROSSPROD(vd,vu,right);
   Normalise(&right);

	/* Determine the amount to translate by */
	if (options.deltamove <= 0)
#if defined(BUILDING_S2PLOT)
	  delta = ss2qcs() * VectorLength(pmin,pmax) / 500.0;
#elif defined(BUILDING_VIEWER)
	  delta = options.interactspeed * VectorLength(pmin,pmax) / 500.0;
#endif
	else
		delta = options.deltamove;
	if (source == MOUSECONTROL)
		delta /= 2;
	
	/* Move the camera horizontally */
	vp.x += iy * vu.x * delta;
   vp.y += iy * vu.y * delta;
	vp.z += iy * vu.z * delta;
	if (options.projectiontype == PERSPECTIVE) {
   	pr.x += iy * vu.x * delta;
   	pr.y += iy * vu.y * delta;
   	pr.z += iy * vu.z * delta;
	}

	/* Move the camera vertically */
   vp.x += ix * right.x * delta;
   vp.y += ix * right.y * delta;
   vp.z += ix * right.z * delta;
	if (options.projectiontype == PERSPECTIVE) {
   	pr.x += ix * right.x * delta;
   	pr.y += ix * right.y * delta;
   	pr.z += ix * right.z * delta;
	}

	/* Update the camera */
	camera.vp = vp;
	camera.pr = pr;
   camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
   camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
   camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
}

/*
  Handle mouse events
  Right button events are passed to the menu handler
  Turn off autopilot mode on left and middle mouse click
*/
void HandleMouse(int button,int state,int x,int y) {
  MTX_LOCK(&mutex);
  
#if defined(BUILDING_VIEWER)
  int i,maxselect = 100,nhits = 0;
  GLuint selectlist[100];
  GLint viewport[4];
#endif
  
  if (state == S2_DOWN) {
    
    if (button == S2_LEFT_BUTTON) {
      
#if defined(BUILDING_VIEWER)
      if (options.selectmode) {
	glSelectBuffer(maxselect,selectlist);
	glRenderMode(GL_SELECT);
	//##glInitNames();
	//##glPushName(-1);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glGetIntegerv(GL_VIEWPORT,viewport); /* Get the viewport bounds */
	gluPickMatrix(x,viewport[3]-y,3.0,3.0,viewport);
	CreateProjection('c');
	glMatrixMode(GL_MODELVIEW);
	glDrawBuffer(GL_BACK_LEFT);
	glLoadIdentity();
	s2LookAt(camera.vp.x,camera.vp.y,camera.vp.z,
		  camera.focus.x,camera.focus.y,camera.focus.z,
		  camera.vu.x,camera.vu.y,camera.vu.z);
	MakeLighting();
	MakeMaterial();
	MakeGeometry(FALSE, FALSE, 'c');
	DrawExtras();
	
	if ((nhits = glRenderMode(GL_RENDER)) < 0) {
	  _s2warn("HandleMouse", "internal select buffer overflow");
	} else {
	  for (i=0;i<nhits;i++) {
	    fprintf(stderr,"Selected item %d, at depth %d -> %d\n",
		    (int)selectlist[4*i+3],(int)selectlist[4*i+1],(int)selectlist[4*i+2]);
	  }
	}	
      }
#endif
      
      interfacestate.currentbutton = S2_LEFT_BUTTON;
      options.autopilot = FALSE;
      
    } else if (button == S2_MIDDLE_BUTTON) {
      
      interfacestate.currentbutton = S2_MIDDLE_BUTTON;
      options.autopilot = FALSE;
      
    }
    
#if defined(BUILDING_S2PLOT) 
    else if ((button == S2_RIGHT_BUTTON) && 
	     (_s2_handle_vis) /* && (nhandle_d) */ ) { 
      
      /* new handle processing code */
      handleView(x, y);      
      
    }
#endif
  }
  
  
  else if (state == S2_UP) {
    interfacestate.currentbutton = -1;
    switch(button) {
    case S2_RIGHT_BUTTON:
#if defined(BUILDING_S2PLOT)
      if (_s2_draghandle) {
	_s2_draghandle = NULL;
	
      }
#endif
      break;
      
    case S2_WHEEL_DOWN:
#if defined(BUILDING_S2PLOT)
      if (_s2_draghandle) {
	int xwaspanel = _s2_activepanel;
	xs2cp(_s2_dragpanel);
	if (!_s2_draghandle_screen) {
	  // we are dragging a handle, so wheel down means move it closer!
	  _s2_draghandle_savez = MAX(0.01, _s2_draghandle_savez * 0.995);
	  double vtx, vty, vtz;
	  s2UnProject(_s2_draghandle_basex, _s2_draghandle_basey,
		       _s2_draghandle_savez,
		       _s2_dragmodel, _s2_dragproj, _s2_dragview,
		       &vtx, &vty, &vtz);
	  
	  int id = _s2_draghandle_id;
	  XYZ p;
	  p.x = _S2DEVICE2WORLD(vtx, _S2XAX);
	  p.y = _S2DEVICE2WORLD(vty, _S2YAX);
	  p.z = _S2DEVICE2WORLD(vtz, _S2ZAX);
	  _s2_draghandle_callback(&id, &p);
	}
	xs2cp(xwaspanel);
      } else
#endif
	{
      
	/* Move backward or increase speed */
	if (options.interaction == INSPECT) {
#if defined(BUILDING_S2PLOT)
	  FlyCamera(-50.0 * ss2qcs());
#else
	  FlyCamera(-50.0);
#endif
	} else {
	  if (ABS(camera.speed) < EPSILON) {
	    camera.speed = -1;
	  } else if (ABS(camera.speed-1) < EPSILON) {
	    camera.speed = 0;
	  } else {
	    camera.speed--;
	  }
	}
	}
      break;
      
    case S2_WHEEL_UP:
#if defined(BUILDING_S2PLOT)
      if (_s2_draghandle) {
	int xwaspanel = _s2_activepanel;
	xs2cp(_s2_dragpanel);
	if (!_s2_draghandle_screen) {
	  // we are dragging a handle, so wheel up means move it away!
	  _s2_draghandle_savez = MIN(0.9999, _s2_draghandle_savez * 1.005);
	  double vtx, vty, vtz;
	  s2UnProject(_s2_draghandle_basex, _s2_draghandle_basey,
		       _s2_draghandle_savez,
		       _s2_dragmodel, _s2_dragproj, _s2_dragview,
		       &vtx, &vty, &vtz);
	  int id = _s2_draghandle_id;
	  XYZ p;
	  p.x = _S2DEVICE2WORLD(vtx, _S2XAX);
	  p.y = _S2DEVICE2WORLD(vty, _S2YAX);
	  p.z = _S2DEVICE2WORLD(vtz, _S2ZAX);
	  _s2_draghandle_callback(&id, &p);
	}
	xs2cp(xwaspanel);
      } else
#endif
	{
      
	/* Move forward or increase speed */
	if (options.interaction == INSPECT) {
#if defined (BUILDING_S2PLOT)
	  FlyCamera(50.0 * ss2qcs());
#else 
	  FlyCamera(50.0);
#endif
	} else {
	  if (ABS(camera.speed) < EPSILON) {
	    camera.speed = 1;
	  } else if (ABS(camera.speed+1) < EPSILON) {
	    camera.speed = 0;
	  } else {
	    camera.speed++;
	  }
	}
      }
      break;

    default:
      break;
    }
  }

   interfacestate.mousexlast = x;
   interfacestate.mouseylast = y;
#if defined(BUILDING_S2PLOT)
   _s2_lastmousex = x;
   _s2_lastmousey = y;
#endif

   MTX_ULCK(&mutex);
}

/*
   Handle mouse dragging.
	Left button
		Rotate vertically and horizontally in INSPECT and FLY modes
		Rotate horizaontally and translate vertically in WALK mode
	Middle button rolls in all modes 
	Interaction rate is a fraction of that for keyboard control
	Right button is reserverd for the menus.
*/
void HandleMouseMotion(int x,int y) {
  MTX_LOCK(&mutex);

  double dx,dy;
  
  dx = x - interfacestate.mousexlast;
  dy = y - interfacestate.mouseylast;

#if defined(BUILDING_S2PLOT)
  if (_s2_draghandle && _s2_draghandle_callback) {
    
    int xwaspanel = _s2_activepanel;
    xs2cp(_s2_dragpanel);
    
    if (!_s2_draghandle_screen) {
      // stuff a new position into a global that gets drawn
      // as if its dynamic geom
      _s2_draghandle_basex += dx; // * (options.stereo == DUALSTEREO ? 0.5 : 1.);
      _s2_draghandle_basey -= dy;
      
      double vtx, vty, vtz;
      s2UnProject(_s2_draghandle_basex, _s2_draghandle_basey,
		   _s2_draghandle_savez,
		   _s2_dragmodel, _s2_dragproj, _s2_dragview,
		   &vtx, &vty, &vtz);
      
      int id = _s2_draghandle_id;
      XYZ p;
      p.x = _S2DEVICE2WORLD(vtx, _S2XAX);
      p.y = _S2DEVICE2WORLD(vty, _S2YAX);
      p.z = _S2DEVICE2WORLD(vtz, _S2ZAX);

      _s2_draghandle_callback(&id, &p);
    } else {
      float mmx = dx / (_s2_panels[_s2_dragpanel].x2 - 
			_s2_panels[_s2_dragpanel].x1);
      float mmy = dy / (_s2_panels[_s2_dragpanel].y2 - 
			_s2_panels[_s2_dragpanel].y1);
      if ((options.stereo == DUALSTEREO) || (options.stereo == WDUALSTEREO)) {
	_s2_draghandle_basex += mmx / (float)options.screenwidth * 2.;
      } else if (options.stereo == TRIOSTEREO) {
	_s2_draghandle_basex += mmx / (float)options.screenwidth * 3.;
      } else {
	_s2_draghandle_basex += mmx / (float)options.screenwidth;
      }
      _s2_draghandle_basey -= mmy / options.screenheight;
      int id = _s2_draghandle_id;
      XYZ p;
      p.x = _s2_draghandle_basex;
      p.y = _s2_draghandle_basey;
      p.z = _s2_draghandle_savez;
      _s2_draghandle_callback(&id, &p);
    }
    
    xs2cp(xwaspanel);
    
  } else 	
#endif
    
    if (interfacestate.currentbutton == S2_LEFT_BUTTON) {
      if (options.interaction == WALK) {
      	if (dy > 0)
	  TranslateCamera(0.0,1.0,MOUSECONTROL);
      	else if (dy < 0)
	  TranslateCamera(0.0,-1.0,MOUSECONTROL);
	RotateCamera(-dx,0.0,0.0,MOUSECONTROL);
      } else {
#if !defined(BUILDING_S2PLOT)
	RotateCamera(-dx,dy,0.0,MOUSECONTROL);
#else
	if (options.interaction == INSPECT) {
	  RotateCamera(-dx,dy, 0.0,MOUSECONTROL);
	} else {
#define ROCLIM 0.8
	  targetRoC_x = -dx * 0.1;
	  if (fabs(targetRoC_x) < 0.2) {
	    targetRoC_x = 0.;
	  }
	  if (targetRoC_x < -ROCLIM) {
	    targetRoC_x = -ROCLIM;
	  } else if (targetRoC_x > ROCLIM) {
	    targetRoC_x = ROCLIM;
	  }
	  
	  targetRoC_y = dy * 0.1;
	  if (fabs(targetRoC_y) < 0.2) {
	    targetRoC_y = 0.;
	  }
	  if (targetRoC_y < -ROCLIM) {
	    targetRoC_y = -ROCLIM;
	  } else if (targetRoC_y > ROCLIM) {
	    targetRoC_y = ROCLIM;
	  }
	}
#endif
      }
    } else if (interfacestate.currentbutton == S2_MIDDLE_BUTTON) {
      RotateCamera(0.0,0.0,dx,MOUSECONTROL);
    }
  
  interfacestate.mousexlast = x;
  interfacestate.mouseylast = y;
#if defined(BUILDING_S2PLOT)
  _s2_lastmousex = x;
  _s2_lastmousey = y;
#endif

  MTX_ULCK(&mutex);
}

/*
	Move the 3D mouse around on passive mouse movement events
	Only do it if we are in 3D cursor mode 
*/
void HandlePassiveMotion(int x,int y)
{
  MTX_LOCK(&mutex);

#if defined(BUILDING_S2PLOT)
  _s2_lastmousex = x;
  _s2_lastmousey = y;
#endif
  
  MTX_ULCK(&mutex);
}

/*
   Handle spaceball.
*/
void HandleSpaceballButton(int button, int state) {
  //fprintf(stderr, "HandleSpaceballButton button=%d, state=%d\n", button, state);
}
void HandleSpaceballMotion(int x, int y, int z) {
  MTX_LOCK(&mutex);

  double dx,dy,dz;
  
  dx = (double)x / 40.;
  dy = (double)y / 40.;
  dz = (double)z / 40.;

  if (fabs(dz) > 5) {
    // change options.interaction from being INSPECT because that makes
    // FOCUS restore to origin for FlyCamera function
    int sinter = options.interaction;
    options.interaction = WALK;
#if defined(BUILDING_S2PLOT)
    FlyCamera(-fabs(dz) * dz / 5. * 10.0 * ss2qcs()); 
#else
    FlyCamera(-fabs(dz) * dz / 5. * 10.0); 
#endif
    options.interaction = sinter;
  } else if (dx*dx+dy*dy > 2*2) {

    // translate (strafe) camera l/r/u/d
    TranslateCamera(fabs(dx)*dx/2.0,-fabs(dy)*dy/2.,KEYBOARDCONTROL);


  }
  
  MTX_ULCK(&mutex);
}
void HandleSpaceballRotate(int x, int y, int z) {
  MTX_LOCK(&mutex);
  
  //double dx = x / 150.0;
  double dy = y / 150.0;
  double dz = z / 150.0;

  if (fabs(dz) > 3) {
    RotateCamera(0.0,0.0,fabs(dz)*dz/3.0,MOUSECONTROL);    
    //  } else if (dx*dx+dy*dy > 2*2) {
    //  RotateCamera(-dy,dx, 0.0,MOUSECONTROL);
  } else if (fabs(dy) > 3) {
    RotateCamera(-fabs(dy)*dy/3.0, 0., 0., MOUSECONTROL);
  }

  MTX_ULCK(&mutex);
}



/*
   How to handle visibility
*/
void HandleVisibility(int visible) {
  MTX_LOCK(&mutex);

  if (visible == S2_VISIBLE)
    s2winIdleFunc(HandleIdle);
  else
    s2winIdleFunc(NULL);
  
  MTX_ULCK(&mutex);
}

/*
   What to do on an idle event
*/
void HandleIdle(void)
{
  MTX_LOCK(&mutex);

   static double tstart = -1;
	double tstop;

	// QuickTime VR 
	static int theta=0,phi=40;
   double range;
   XYZ right,up = {0,0,1};

	// Make qtvr frames 
   if (options.makeqtvr) {
		range = VectorLength(camera.vp,camera.pr);
		camera.vp.x = camera.pr.x + range * cos(theta*DTOR) * cos(phi*DTOR);
		camera.vp.y = camera.pr.y + range * sin(theta*DTOR) * cos(phi*DTOR);
		camera.vp.z = camera.pr.z + range * sin(phi*DTOR);
		camera.vd = VectorSub(camera.vp,camera.pr);
		Normalise(&camera.vd);
		right = CrossProduct(camera.vd,up);
		camera.vu = CrossProduct(right,camera.vd);
		Normalise(&camera.vu);
		theta += 10;
		if (theta % 360 == 0) {
			phi -= 10;
			theta = 0;
		}
		if (phi < -40) {
			options.makeqtvr = FALSE;
			theta = 0;
			phi = 60;
		}
		options.windowdump = 1;
		s2winPostRedisplay();

		MTX_ULCK(&mutex);
		return;
   }

	// Is it time for another update 
   if (tstart < 0) 
      tstart = GetRunTime();
   tstop = GetRunTime();
   if (tstop - tstart > 1.0/options.targetfps) {
     s2winPostRedisplay();
      tstart = tstop;
   }

   MTX_ULCK(&mutex);
}

/*
	Save a geometry file
	Note this upgrades the primitives so the output won't be the
	same as an input geom file. For example, "p" becomes "P", this
	reflects the general upgrading that occurs when reading a 
	geom file.
*/
#if defined(BUILDING_S2PLOT)
void SaveGeomFile(char *outname)
#else
void SaveGeomFile(void)
#endif
{
	int i,j;
   const char *fn;
   char fname[256];
   FILE *fptr;

#if defined(BUILDING_S2PLOT)
   if (!outname) {
#endif
   /* Get the file name */
   fn = NULL;
   if (fn == NULL || strlen(fn) <= 0)
      return;
   strcpy(fname,fn);
#if defined(BUILDING_S2PLOT) 
   } else {
     strcpy(fname, outname);
   }
#endif

   /* Try to open the file */
   if ((fptr = fopen(fname,"w")) == NULL)
      return;

#if defined(BUILDING_S2PLOT)
   /* change: DGB 20050812 for S2PLOT: loop twice, once to draw
    * static geom, once dynamic 
    */
   int lds;
   for (lds = 0; lds < 2; lds++) {
     if (lds) {
       _s2_startDynamicGeometry(FALSE);
     }
#endif
	for (i=0;i<nlight;i++) 
		fprintf(fptr,"i %g %g %g %g %g %g\n",
			lights[i].p[0],lights[i].p[1],lights[i].p[2],
			lights[i].c[0],lights[i].c[1],lights[i].c[2]);

	for (i=0;i<nball;i++)
		fprintf(fptr,"s %g %g %g %g %g %g %g\n",
			ball[i].p.x,ball[i].p.y,ball[i].p.z,
			ball[i].r,
			ball[i].colour.r,ball[i].colour.g,ball[i].colour.b);

   for (i=0;i<nballt;i++)
      fprintf(fptr,"st %g %g %g %g %g %g %g %s\n",
         ballt[i].p.x,ballt[i].p.y,ballt[i].p.z,
			ballt[i].r,
         ballt[i].colour.r,ballt[i].colour.g,ballt[i].colour.b,
			ballt[i].texturename);

	for (i=0;i<ndisk;i++) 
		fprintf(fptr,"d %g %g %g %g %g %g %g %g %g %g %g\n",
			disk[i].p.x,disk[i].p.y,disk[i].p.z,
         disk[i].n.x,disk[i].n.y,disk[i].n.z,
			disk[i].r1,disk[i].r2,
			disk[i].colour.r,disk[i].colour.g,disk[i].colour.b);

	for (i=0;i<ncone;i++)
		fprintf(fptr,"c %g %g %g %g %g %g %g %g %g %g %g\n",
			cone[i].p1.x,cone[i].p1.y,cone[i].p1.z,
			cone[i].p2.x,cone[i].p2.y,cone[i].p2.z,
			cone[i].r1,cone[i].r2,
			cone[i].colour.r,cone[i].colour.g,cone[i].colour.b);

	for (i=0;i<ndot;i++)
		fprintf(fptr,"P %g %g %g %g %g %g %g\n",
			dot[i].p.x,dot[i].p.y,dot[i].p.z,
			dot[i].colour.r,dot[i].colour.g,dot[i].colour.b,
			dot[i].size);

	for (i=0;i<nline;i++) {
		fprintf(fptr,"lc %g %g %g %g %g %g %g %g %g %g %g %g %g\n",
			line[i].p[0].x,line[i].p[0].y,line[i].p[0].z,
			line[i].p[1].x,line[i].p[1].y,line[i].p[1].z,
			line[i].colour[0].r,line[i].colour[0].g,line[i].colour[0].b,
         line[i].colour[1].r,line[i].colour[1].g,line[i].colour[1].b,
			line[i].width);
	}

	for (i=0;i<nface3;i++) {
		fprintf(fptr,"f3nc");
		for (j=0;j<3;j++)
			fprintf(fptr," %g %g %g",face3[i].p[j].x,face3[i].p[j].y,face3[i].p[j].z);
      for (j=0;j<3;j++)
         fprintf(fptr," %g %g %g",face3[i].n[j].x,face3[i].n[j].y,face3[i].n[j].z);
      for (j=0;j<3;j++)
         fprintf(fptr," %g %g %g",
				face3[i].colour[j].r,face3[i].colour[j].g,face3[i].colour[j].b);
		fprintf(fptr,"\n");
	}

	for (i=0;i<nface4;i++) {
      fprintf(fptr,"f4nc");
      for (j=0;j<4;j++)
         fprintf(fptr," %g %g %g",face4[i].p[j].x,face4[i].p[j].y,face4[i].p[j].z);
      for (j=0;j<4;j++)
         fprintf(fptr," %g %g %g",face4[i].n[j].x,face4[i].n[j].y,face4[i].n[j].z);
      for (j=0;j<4;j++)
         fprintf(fptr," %g %g %g",
            face4[i].colour[j].r,face4[i].colour[j].g,face4[i].colour[j].b);
      fprintf(fptr,"\n");
   }

	for (i=0;i<nface4t;i++) {
      fprintf(fptr,"f4t");
      for (j=0;j<4;j++)
         fprintf(fptr," %g %g %g",
				face4t[i].p[j].x,face4t[i].p[j].y,face4t[i].p[j].z);
      fprintf(fptr," %g %g %g",
         face4t[i].colour.r,face4t[i].colour.g,face4t[i].colour.b);
		fprintf(fptr," %s %g %c\n",
			face4t[i].texturename,face4t[i].scale,face4t[i].trans);
      fprintf(fptr,"\n");
   }

	for (i=0;i<nlabel;i++) {
		fprintf(fptr,"t %g %g %g %g %g %g %g %g %g %g %g %g %s\n",
			label[i].p.x,label[i].p.y,label[i].p.z,
			label[i].right.x,label[i].right.y,label[i].right.z,
			label[i].up.x,label[i].up.y,label[i].up.z,
			label[i].colour.r,label[i].colour.g,label[i].colour.b,
			label[i].s);
   }

#if defined(BUILDING_S2PLOT)
	/* change DGB 20051208 - to enable saving of static AND dynamic */
	if (lds) {
	  _s2_endDynamicGeometry();
	}
   }
#endif

	fclose(fptr);
}

/*
	Read a geometry file
	For documentation see
		http://www.swin.edu.au/astronomy/pbourke/3dformats/geom/
	In general primitives get upgraded to higher forms.
*/
int ReadGeometry(char *name)
{
	int i,d;
	int gotitalready = -1;
	int linecount = 0;
	FILE *fptr;
	char id[32],texturename[64],trans[32];
	double r,r2,scale,size,width;
	XYZ p[4],n[4];
	COLOUR c[4];
	const char *fn;
	XYZ origin = {0.0,0.0,0.0};

	if (strlen(name) < 1) {
      fn = NULL;
      if (fn == NULL || strlen(fn) <= 0) 
     		return(TRUE); 
      if ((fptr = fopen(fn,"r")) == NULL) {
         return(FALSE);
      }
	} else {
   	if ((fptr = fopen(name,"r")) == NULL) {
      	return(FALSE);
		}
   }

	while (fscanf(fptr,"%s",id) == 1) {
		if (strcmp(id,"#") == 0) {											/* Comment */
			;
		} else if (strcmp(id,"t") == 0) {								/* Label */
			if (!ReadVector(fptr,&(p[0]))) {
            fprintf(stderr,"Failed reading label position (%d)",linecount);
            fclose(fptr);
            continue;
			}
         if (!ReadVector(fptr,&(p[1]))) {
            fprintf(stderr,"Failed reading label right vector (%d)",linecount);
            fclose(fptr);
            continue;
         }
         if (!ReadVector(fptr,&(p[2]))) {
            fprintf(stderr,"Failed reading label up vector (%d)",linecount);
            fclose(fptr);
            continue;
         }
         if (!ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading label colour (%d)",linecount);
            fclose(fptr);
            continue;
         }
         if ((label = (LABEL *)realloc(label,(nlabel+1)*sizeof(LABEL))) == NULL) {
	   _s2error("(internal)", "memory allocatino for label failed");
         }
         label[nlabel].p      = p[0];
			label[nlabel].right  = p[1];
			label[nlabel].up     = p[2];
			label[nlabel].colour = c[0];
         i = 0;
         while ((d = fgetc(fptr)) != '\n' && d != '\r' && i < (MAXLABELLEN-1)) {
				if (d == ' ' && i == 0)
					;
				else
            	label[nlabel].s[i++] = d;
			}
			label[nlabel].s[i] = '\0';
#if defined(BUILDING_S2PLOT)
			strcpy(label[nlabel].whichscreen, "");
			strncpy(label[nlabel].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif
			nlabel++;

		} else if (strcmp(id,"m") == 0) {                        /* Marker */

         if (fscanf(fptr,"%d %lf",&d,&size) != 2 ||
				 !ReadVector(fptr,&(p[0])) ||
             !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading marker (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
			if (d < 0 || d > 3) {
				fprintf(stderr,"Unexpected marker: %d (%d)\n",d,linecount);
				continue;
			} 
			AddMarker2Database(d,size,p[0],c[0]);

		} else if (strcmp(id,"s") == 0) {								/* Ball */

			if (!ReadVector(fptr,&(p[0])) || 
				  fscanf(fptr,"%lf",&r) != 1 ||
				 !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading ball (%d)",linecount);
            fclose(fptr);
            continue;
         }
			if ((ball = (BALL *)realloc(ball,(nball+1)*sizeof(BALL))) == NULL) {
			  _s2error("(internal)", "memory allocation for ball failed");
         }
			ball[nball].p = p[0];
			ball[nball].r = r;
			ball[nball].colour = c[0];
#if defined(BUILDING_S2PLOT)
			strcpy(ball[nball].whichscreen, "");
			strncpy(ball[nball].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif
			nball++;

      } else if (strcmp(id,"st") == 0) {                        /* Ball */
		  
         if (!ReadVector(fptr,&(p[0])) ||
	     fscanf(fptr,"%lf",&r) != 1 ||
             !ReadColour(fptr,&(c[0]))) {
	   fprintf(stderr,"Failed reading ball (%d)",linecount);
	   fclose(fptr);
	   continue;
         }
         if (fscanf(fptr,"%s",texturename) != 1) {
	   fprintf(stderr,"Failed reading texture name for ballt (%d)\n",linecount);
	   fclose(fptr);
	   continue;
         }
         if ((ballt = (BALLT *)realloc(ballt,(nballt+1)*sizeof(BALLT))) == NULL) {
	   _s2error("(internal)", "memory allocation for textured ball failed");
         }
         ballt[nballt].p = p[0];
         ballt[nballt].r = r;
         ballt[nballt].colour = c[0];
	 strcpy(ballt[nballt].texturename,texturename);
	 /* Check to see if the texture already exists */
	 gotitalready = -1;
	 for (i=0;i<nballt;i++) {
	   if (strcmp(ballt[i].texturename,texturename) == 0) {
	     gotitalready = i;
	     break;
	   }
	 }
	 if (gotitalready >= 0) {
	   ballt[nballt].rgba = NULL;
	   ballt[nballt].textureid = ballt[gotitalready].textureid;
	   if (options.debug)
	     fprintf(stderr,"   Resusing texture \"%s\"\n",texturename);
	 } else {
#if defined(BUILDING_S2PLOT)
	   ballt[nballt].rgba = NULL;
	   ballt[nballt].textureid = s2loadtexture(texturename);
	   ballt[nballt].texture_phase = 0.;
	   ballt[nballt].axis.x = 1.;
	   ballt[nballt].axis.y = 0.;
	   ballt[nballt].axis.z = 0.;
	   ballt[nballt].rotation = 0.;
#else
	   ballt[nballt].rgba = ReadTGATexture(texturename,&(ballt[nballt].width),&(ballt[nballt].height));
	   glGenTextures(1,&(ballt[nballt].textureid));
	   if (options.debug) 
	     fprintf(stderr,"Texture: %s (%dx%d), id: %d\n",
		     texturename,ballt[nballt].width,ballt[nballt].height,(int)ballt[nballt].textureid);
	   glBindTexture(GL_TEXTURE_2D,ballt[nballt].textureid);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	   glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	   glTexImage2D(GL_TEXTURE_2D,0,4,
			ballt[nballt].width,ballt[nballt].height,
			0,GL_RGBA,GL_UNSIGNED_BYTE,ballt[nballt].rgba);
	   glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
#endif
	 }
#if defined(BUILDING_S2PLOT)
	 ballt[nballt].texture_phase = 0.;
	 ballt[nballt].axis.x = 0.;
	 ballt[nballt].axis.y = 1.;
	 ballt[nballt].axis.z = 0.;
	 ballt[nballt].rotation = 0.;
	 strcpy(ballt[nballt].whichscreen, "");
	 //strncpy(ballt[nballt].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif
         nballt++;
	 
      } else if (strcmp(id,"d") == 0) {                        /* Disk */

         if (!ReadVector(fptr,&(p[0])) ||
				 !ReadVector(fptr,&(n[0])) ||
              fscanf(fptr,"%lf %lf",&r,&r2) != 2 ||
             !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading disk (%d)",linecount);
            fclose(fptr);
            continue;
         }
         if ((disk = (DISK *)realloc(disk,(ndisk+1)*sizeof(DISK))) == NULL) {
	   _s2error("(internal)", "memory allocation for disk failed");
         }
         disk[ndisk].p = p[0];
			disk[ndisk].n = n[0];
         disk[ndisk].r1 = r;
			disk[ndisk].r2 = r2;
         disk[ndisk].colour = c[0];

#if defined(BUILDING_S2PLOT)
	 strcpy(disk[ndisk].whichscreen, "");
#endif
         ndisk++;

      } else if (strcmp(id,"c") == 0) {                        /* Cone */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
              fscanf(fptr,"%lf %lf",&r,&r2) != 2 ||
             !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading cone (%d)",linecount);
            fclose(fptr);
            continue;
         }
         if ((cone = (CONE *)realloc(cone,(ncone+1)*sizeof(CONE))) == NULL) {
	   _s2error("(internal)", "memory allocation for cone failed");
         }
         cone[ncone].p1 = p[0];
         cone[ncone].p2 = p[1];
         cone[ncone].r1 = r;
         cone[ncone].r2 = r2;
         cone[ncone].colour = c[0];
#if defined(BUILDING_S2PLOT)
	 strcpy(cone[ncone].whichscreen, "");
	 strncpy(cone[ncone].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif
         ncone++;

      } else if (strcmp(id,"i") == 0) {                        /* Light */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading light (%d)",linecount);
            fclose(fptr);
            continue;
         }
			if (nlight < MAXLIGHT) {
         	lights[nlight].p[0] = p[0].x;
            lights[nlight].p[1] = p[0].y;
            lights[nlight].p[2] = p[0].z;
            lights[nlight].p[3] = 1;			/* Positional lights */
         	lights[nlight].c[0] = c[0].r;
            lights[nlight].c[1] = c[0].g;
            lights[nlight].c[2] = c[0].b;
            lights[nlight].c[3] = 1;
         	nlight++;
			}

		} else if (strcmp(id,"p") == 0 || strcmp(id,"P") == 0) {		/* Dot */	

			size = 1;
         if (!ReadVector(fptr,&(p[0])) ||
             !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading dot (%d)",linecount);
            fclose(fptr);
            continue;
         }
			if (strcmp(id,"P") == 0) {
				if (fscanf(fptr,"%lf",&size) != 1) {
					fprintf(stderr,"Failed reading dot size (%d)",linecount);
					fclose(fptr);
            	continue;
				}
			}
			if ((dot = (DOT *)realloc(dot,(ndot+1)*sizeof(DOT))) == NULL) {
			  _s2error("(internal)", "memory allocation for dot failed");
         }
			dot[ndot].p = p[0];
			dot[ndot].colour = c[0];
			dot[ndot].size = ABS(size);
#if defined(BUILDING_S2PLOT)
			strcpy(dot[ndot].whichscreen, "");
			strncpy(dot[ndot].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif
			ndot++;

		} else if (strcmp(id,"l") == 0 || strcmp(id,"L") == 0) {		/* Line	*/

			width = 1;
         if (!ReadVector(fptr,&(p[0])) || 
				 !ReadVector(fptr,&(p[1])) ||
             !ReadColour(fptr,&(c[0]))) {   
            fprintf(stderr,"Failed reading line (%d)\n",linecount);   
            fclose(fptr);  
            continue;
         }
			if (strcmp(id,"L") == 0) {
				if (fscanf(fptr,"%lf",&width) != 1) {
					fprintf(stderr,"Failed reading line width (%d)\n",linecount);
					continue;
				}
			}
			AddLine2Database(p[0],p[1],c[0],c[0],width);

      } else if (strcmp(id,"lc") == 0) {    /* Colour Line  */

         width = 1;
         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
             !ReadColour(fptr,&(c[0])) ||
				 !ReadColour(fptr,&(c[1]))) {
            fprintf(stderr,"Failed reading colour line (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
         AddLine2Database(p[0],p[1],c[0],c[1],width);

		} else if (strcmp(id,"f3") == 0) {					/* 3 face */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
				 !ReadVector(fptr,&(p[2])) || 
             !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading face3 (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
			AddFace2Database(p,3,c[0],1.0,origin);

      } else if (strcmp(id,"f3n") == 0) {              /* 3 face */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
             !ReadVector(fptr,&(p[2])) ||
             !ReadVector(fptr,&(n[0])) ||
             !ReadVector(fptr,&(n[1])) ||
             !ReadVector(fptr,&(n[2])) ||
             !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading face3n (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
         if ((face3 = (FACE3 *)realloc(face3,(nface3+1)*sizeof(FACE3))) == NULL) {
	   _s2error("(internal)", "memory allocation for face3n failed");
         }
	 for (i=0;i<3;i++) {
	   face3[nface3].p[i] = p[i];
	   face3[nface3].n[i] = n[i];
	   face3[nface3].colour[i] = c[0];
	 }
#if defined(BUILDING_S2PLOT)
	 strcpy(face3[nface3].whichscreen, "");
	 strncpy(face3[nface3].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif		
         nface3++;

      } else if (strcmp(id,"f3c") == 0) {              /* 3 face */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
             !ReadVector(fptr,&(p[2])) ||
             !ReadColour(fptr,&(c[0])) ||
             !ReadColour(fptr,&(c[1])) ||
             !ReadColour(fptr,&(c[2]))) {
            fprintf(stderr,"Failed reading face3 (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
         if ((face3 = (FACE3 *)realloc(face3,(nface3+1)*sizeof(FACE3)))==NULL) {
	   _s2error("(internal)", "memory allocation for face3 failed");
         }
         for (i=0;i<3;i++) {
            face3[nface3].p[i] = p[i];
            face3[nface3].n[i] = CalcNormal(p[0],p[1],p[2]);
            face3[nface3].colour[i] = c[i];
         }
#if defined(BUILDING_S2PLOT)
	 strcpy(face3[nface3].whichscreen, "");
	 strncpy(face3[nface3].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif		
         nface3++;

      } else if (strcmp(id,"f3nc") == 0) {              /* 3 face */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
             !ReadVector(fptr,&(p[2])) ||
             !ReadVector(fptr,&(n[0])) ||
             !ReadVector(fptr,&(n[1])) ||
             !ReadVector(fptr,&(n[2])) ||
             !ReadColour(fptr,&(c[0])) ||
             !ReadColour(fptr,&(c[1])) ||
             !ReadColour(fptr,&(c[2]))) {
            fprintf(stderr,"Failed reading face3 (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
         if ((face3 = (FACE3 *)realloc(face3,(nface3+1)*sizeof(FACE3)))==NULL) {
	   _s2error("(internal)", "memory allocation for face3 failed");
         }
	 for (i=0;i<3;i++) {
	   face3[nface3].p[i] = p[i];
	   face3[nface3].n[i] = n[i];
	   face3[nface3].colour[i] = c[i];
	 }
#if defined(BUILDING_S2PLOT)
	 strcpy(face3[nface3].whichscreen, "");
	 strncpy(face3[nface3].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif		
         nface3++;

		} else if (strcmp(id,"f4") == 0) {              /* 4 face */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
             !ReadVector(fptr,&(p[2])) ||
				 !ReadVector(fptr,&(p[3])) ||
             !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading face4 (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
			AddFace2Database(p,4,c[0],1.0,origin);

      } else if (strcmp(id,"f4n") == 0) {             /* 4 face, normal */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
             !ReadVector(fptr,&(p[2])) ||
             !ReadVector(fptr,&(p[3])) ||
             !ReadVector(fptr,&(n[0])) ||
             !ReadVector(fptr,&(n[1])) ||
             !ReadVector(fptr,&(n[2])) ||
				 !ReadVector(fptr,&(n[3])) ||
             !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading face4 (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
	 if ((face4 = (FACE4 *)realloc(face4,(nface4+1)*sizeof(FACE4))) == NULL) {
	   _s2error("(internal)", "memory allocation for face4 failed");
         }
	 for (i=0;i<4;i++) {
	   face4[nface4].p[i] = p[i];
	   face4[nface4].n[i] = n[i];
	   face4[nface4].colour[i] = c[0];
	 }
#if defined(BUILDING_S2PLOT)
	   strcpy(face4[nface4].whichscreen, "");
	   strncpy(face4[nface4].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif		
	 nface4++;

      } else if (strcmp(id,"f4c") == 0) {            /* 4 face, col */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
             !ReadVector(fptr,&(p[2])) ||
             !ReadVector(fptr,&(p[3])) ||
             !ReadColour(fptr,&(c[0])) ||
             !ReadColour(fptr,&(c[1])) ||
             !ReadColour(fptr,&(c[2])) ||
             !ReadColour(fptr,&(c[3]))) {
            fprintf(stderr,"Failed reading face4 (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
         if ((face4 = (FACE4 *)realloc(face4,(nface4+1)*sizeof(FACE4)))==NULL) {
	   _s2error("(internal)", "memory allocation for face4 failed");
         }
         for (i=0;i<4;i++) {
            face4[nface4].p[i] = p[i];
            face4[nface4].n[i] = CalcNormal(p[(i-1+4)%4],p[i],p[(i+1)%4]);
            face4[nface4].colour[i] = c[i];
         }
#if defined(BUILDING_S2PLOT)
	 strcpy(face4[nface4].whichscreen, "");
	 strncpy(face4[nface4].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif		
         nface4++;

      } else if (strcmp(id,"f4nc") == 0) {            /* 4 face, normal, col */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
             !ReadVector(fptr,&(p[2])) ||
             !ReadVector(fptr,&(p[3])) ||
             !ReadVector(fptr,&(n[0])) ||
             !ReadVector(fptr,&(n[1])) ||
             !ReadVector(fptr,&(n[2])) ||
             !ReadVector(fptr,&(n[3])) ||
				 !ReadColour(fptr,&(c[0])) ||
				 !ReadColour(fptr,&(c[1])) ||
				 !ReadColour(fptr,&(c[2])) ||
             !ReadColour(fptr,&(c[3]))) {
            fprintf(stderr,"Failed reading face4 (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
	 if ((face4 = (FACE4 *)realloc(face4,(nface4+1)*sizeof(FACE4)))==NULL) {
	   _s2error("(internal)", "memory allocation for face4 failed");
	 }
	 for (i=0;i<4;i++) {
	   face4[nface4].p[i] = p[i];
	   face4[nface4].n[i] = n[i];
	   face4[nface4].colour[i] = c[i];
	 }
#if defined(BUILDING_S2PLOT)
	 strcpy(face4[nface4].whichscreen, "");
	 strncpy(face4[nface4].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif		
	 nface4++;

      } else if (strcmp(id,"f4t") == 0) {             /* 4 face textured */

         if (!ReadVector(fptr,&(p[0])) ||
             !ReadVector(fptr,&(p[1])) ||
             !ReadVector(fptr,&(p[2])) ||
             !ReadVector(fptr,&(p[3])) ||
             !ReadColour(fptr,&(c[0]))) {
            fprintf(stderr,"Failed reading face4t (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
			if (fscanf(fptr,"%s",texturename) != 1) {
            fprintf(stderr,"Failed reading texture name for face4t (%d)\n",linecount);
            fclose(fptr);
            continue;
			}
         if (fscanf(fptr,"%lf",&scale) != 1 || scale <= 0) {
            fprintf(stderr,"Failed reading texture scale for face4t (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
         if (fscanf(fptr,"%s",trans) != 1) {
            fprintf(stderr,"Failed reading tex transparency for face4t (%d)\n",linecount);
            fclose(fptr);
            continue;
         }
         if ((face4t = (FACE4T *)realloc(face4t,(nface4t+1)*sizeof(FACE4T))) == NULL) {
	   _s2error("(internal)", "memory allocation for face4t failed");
         }
	 for (i=0;i<4;i++)
	   face4t[nface4t].p[i] = p[i];
         face4t[nface4t].colour = c[0];
	 strcpy(face4t[nface4t].texturename,texturename);
	 face4t[nface4t].trans  = trans[0];
	 face4t[nface4t].scale  = scale;
         gotitalready = -1;
         for (i=0;i<nface4t;i++) {
	   if (strcmp(face4t[i].texturename,texturename) == 0) {
	     gotitalready = i;
	     break;
	   }
         }
         if (gotitalready > 0) {
	   face4t[nface4t].rgba = NULL;
	   face4t[nface4t].textureid = face4t[gotitalready].textureid;
	   if (options.debug)
	     fprintf(stderr,"   Resusing texture \"%s\"\n",texturename);
         } else {
#if defined(BUILDING_S2PLOT)
	   face4t[nface4t].rgba = NULL;
	   face4t[nface4t].textureid = s2loadtexture(texturename);
#else
	   face4t[nface4t].rgba = ReadTGATexture(texturename,&(face4t[nface4t].width),&(face4t[nface4t].height)); 
	   glGenTextures(1,&(face4t[nface4t].textureid));
	   if (options.debug)
	     fprintf(stderr,"Texture: %s (%dx%d), id: %d\n",
		     texturename,face4t[nface4t].width,face4t[nface4t].height,(int)face4t[nface4t].textureid);
	   glBindTexture(GL_TEXTURE_2D,face4t[nface4t].textureid);
	   glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	   glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	   glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	   glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	   glTexImage2D(GL_TEXTURE_2D,0,4,
			face4t[nface4t].width,face4t[nface4t].height,
			0,GL_RGBA,GL_UNSIGNED_BYTE,face4t[nface4t].rgba);
	   glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
#endif
	 }
#if defined(BUILDING_S2PLOT)
	 strcpy(face4t[nface4t].whichscreen, "");
	 strncpy(face4t[nface4t].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
#endif		
	 nface4t++;

		}

		// Skip everything to the end of the line, treat as a comment 
		while (strcmp(id,"t") != 0 && (d = fgetc(fptr)) != '\n' && d != '\r')
			;

		linecount++;
	}
	fclose(fptr);

	if (options.debug) {
	  fprintf(stderr,"Read %d lines\n",linecount);
	  fprintf(stderr,"Dot:     %d\n",ndot);
	  fprintf(stderr,"Ball:    %d\n",nball);
	  fprintf(stderr,"Ballt:   %d\n",nballt);
	  fprintf(stderr,"Disk:    %d\n",ndisk);
	  fprintf(stderr,"Cone:    %d\n",ncone);
	  fprintf(stderr,"Lights:  %d\n",nlight);
	  fprintf(stderr,"Line:    %d\n",nline);
	  fprintf(stderr,"Face3:   %d\n",nface3);
	  fprintf(stderr,"Face4:   %d\n",nface4);
	  fprintf(stderr,"Face4t:  %d\n",nface4t);
	  fprintf(stderr,"Label:   %d\n",nlabel);
	}

	if (linecount > 0)
		return(TRUE);
	else
		return(FALSE);
}

/*
	Read an OFF file
	An off file consists of 
	- number of vertices
	- the list of vertices, normals, colours
	- number of faces
	- the list of faces
*/
int ReadOFF(char *name)
{
   int i,j;
   FILE *fptr;
	int noffvert     = 0;
	XYZ *offvert     = NULL;
	XYZ *offnorm     = NULL;
	COLOUR *offcol   = NULL;
	int noffface     = 0;
	OFFFACE *offface = NULL;
   const char *fn;

	/* Get the file name */
	if (strlen(name) < 1) {
	fn = NULL;
   	if (fn == NULL || strlen(fn) <= 0) 
      	return(TRUE);
   	if ((fptr = fopen(fn,"r")) == NULL) {
      	return(FALSE);
   	}
	} else {
   	if ((fptr = fopen(name,"r")) == NULL) {
      	return(FALSE);
		}
   }

	/* Read the vertices */
	if (fscanf(fptr,"%d",&noffvert) != 1) {
	  _s2error("(internal)", "failed to read the number of vertices");
	}

	/* Allocate the storage for the vertices, colour, and normals */
	if ((offvert = (XYZ *)malloc(noffvert*sizeof(XYZ))) == NULL) {
	  _s2error("(internal)", "failed to allocate memory for vertices");
	}
	if ((offnorm = (XYZ *)malloc(noffvert*sizeof(XYZ))) == NULL) {
	  _s2error("(internal)", "failed to allocate memory for normals");
	}
	if ((offcol = (COLOUR *)malloc(noffvert*sizeof(COLOUR))) == NULL) {
	  _s2error("(internal)", "failed to allocate memory for colours");
	}

	/* Read the vertices, normals, colours */
	for (i=0;i<noffvert;i++) {
	  if (fscanf(fptr,"%lf %lf %lf",
		     &(offvert[i].x),&(offvert[i].y),&(offvert[i].z)) != 3) {
	    _s2error("(internal)", "failed to read vertices");
	  }
	  if (fscanf(fptr,"%lf %lf %lf",
		     &(offnorm[i].x),&(offnorm[i].y),&(offnorm[i].z)) != 3) {
	    _s2error("(internal)", "failed to read normals");
	  }
	  if (fscanf(fptr,"%lf %lf %lf",
		     &(offcol[i].r),&(offcol[i].g),&(offcol[i].b)) != 3) {
	    _s2error("(internal)", "failed to read colours");
	  }
	}

	/* Read the number of faces */
	if (fscanf(fptr,"%d",&noffface) != 1) {
	  _s2error("(internal)", "failed to read number of faces");
	}

   /* Allocate the storage for the faces */
   if ((offface = (OFFFACE *)malloc(noffface*sizeof(OFFFACE))) == NULL) {
     _s2error("(internal)", "failed to allocate memory for faces");
   }

	/* Read the faces */
	for (i=0;i<noffface;i++) {
		if (fscanf(fptr,"%d",&(offface[i].nv)) != 1) {
		  _s2error("(internal)", "failed to read number of vertices");
		}
		if (offface[i].nv < 1 || offface[i].nv > 4) {
		  _s2error("(internal)", "illegal number of vertices in a face");
		}
		for (j=0;j<offface[i].nv;j++) {
			if (fscanf(fptr,"%d",&(offface[i].p[j])) != 1) {
			  _s2error("(internal)", "failed to read a faces vertex index");
			}
		}
	}

	/* Convert the off geometry */
	for (i=0;i<noffface;i++) {
		switch (offface[i].nv) {
		case 1:
		  if ((dot = (DOT *)realloc(dot,(ndot+1)*sizeof(DOT))) == NULL) {
		    _s2error("(internal)", "memory allocation for dot failed");
         }
         dot[ndot].p = offvert[offface[i].p[0]];
         dot[ndot].colour = offcol[offface[i].p[0]];
			dot[ndot].size = 1;
         ndot++;
			break;
		case 2:
		  if ((line = (LINE *)realloc(line,(nline+1)*sizeof(LINE))) == NULL) {
		    _s2error("(internal)", "memory allocation for line failed");
         }
         line[nline].p[0]   = offvert[offface[i].p[0]];
         line[nline].p[1]   = offvert[offface[i].p[1]];
         line[nline].colour[0] = offcol[offface[i].p[0]];
         line[nline].colour[1] = offcol[offface[i].p[0]];
			line[nline].width = 1;
         nline++;
			break;
		case 3:
		  if ((face3 = (FACE3 *)realloc(face3,(nface3+1)*sizeof(FACE3)))==NULL) {
		    _s2error("(internal)", "memory allocation for face3 failed");
         }
			for (j=0;j<3;j++) {
         	face3[nface3].p[j]      = offvert[offface[i].p[j]];
         	face3[nface3].n[j]      = offnorm[offface[i].p[j]];
         	face3[nface3].colour[j] = offcol[offface[i].p[j]];
			}
         nface3++;
			break;
		case 4:
		  if ((face4 = (FACE4 *)realloc(face4,(nface4+1)*sizeof(FACE4)))==NULL) {
		    _s2error("(internal)", "memory allocation for face4 failed");
         }
			for (j=0;j<4;j++) {
         	face4[nface4].p[j]      = offvert[offface[i].p[j]];
         	face4[nface4].n[j]      = offnorm[offface[i].p[j]];
         	face4[nface4].colour[j] = offcol[offface[i].p[j]];
			}
         nface4++;
			break;
		}
	}

	if (options.debug) {
		fprintf(stderr,"Read %d vertices\n",noffvert);
		fprintf(stderr,"Read %d faces\n",noffface);
	}

	return(TRUE);

	if (offvert != NULL) free(offvert);
	if (offnorm != NULL) free(offnorm);
	if (offcol  != NULL) free(offcol);
	if (offface != NULL) free(offface);
	exit(-1);
}

/*
	Clean up the geometry
	face4's get decomposed to face3
	face3's get decomposed to lines
	lines to points
*/
void CleanGeometry(void)
{
	int i,j;
	double e;
	int badface3 = 0,badface4 = 0,badline = 0;
	int badnormal = 0;
	XYZ n;

	/* Deal with 4 vertex lines */
   for (i=0;i<nface4;i++) {
      for (j=0;j<4;j++) {
         e = VectorLength(face4[i].p[j],face4[i].p[(j+1)%4]);
         if (e < EPSILON) {
	   if ((face3 = (FACE3 *)realloc(face3,(nface3+1)*sizeof(FACE3))) == NULL) {
	     _s2error("(internal)", "memory allocation for face3 failed");
            }
            face3[nface3].p[0]   = face4[i].p[j];
            face3[nface3].p[1]   = face4[i].p[(j+2)%4];
				face3[nface3].p[2]   = face4[i].p[(j+3)%4];
            face3[nface3].n[0]   = face4[i].n[j];
            face3[nface3].n[1]   = face4[i].n[(j+2)%4];
            face3[nface3].n[2]   = face4[i].n[(j+3)%4];
            face3[nface3].colour[0] = face4[i].colour[j];
            face3[nface3].colour[1] = face4[i].colour[(j+2)%4];
            face3[nface3].colour[2] = face4[i].colour[(j+3)%4];
            nface3++;
            if (nface4 > 1)
               face4[i] = face4[nface4-1];
            nface4--;
            i--;
            badface4++;
				break;
         }
      }
	}

	/* Fix up face3 */
	for (i=0;i<nface3;i++) {
		for (j=0;j<3;j++) {
			e = VectorLength(face3[i].p[j],face3[i].p[(j+1)%3]);
			if (e < EPSILON) {
			  if ((line = (LINE *)realloc(line,(nline+1)*sizeof(LINE))) == NULL) {
			    _s2error("(internal)", "memory allocation for line failed");
         	}
         	line[nline].width  = 1;
         	line[nline].p[0]   = face3[i].p[j];
         	line[nline].p[1]   = face3[i].p[(j+2)%3];
         	line[nline].colour[0] = face3[i].colour[j];
            line[nline].colour[1] = face3[i].colour[j];
				nline++;
				if (nface3 > 1)
					face3[i] = face3[nface3-1];
				nface3--;
				i--;
				badface3++;
				break;
			}
		} 
	}

	/* Degenerate lines are turned into dots */
	for (i=0;i<nline;i++) {
		e = VectorLength(line[i].p[0],line[i].p[1]);
		if (e < EPSILON) {
		  if ((dot = (DOT *)realloc(dot,(ndot+1)*sizeof(DOT))) == NULL) {
		    _s2error("(internal)", "memory allocation for dot failed");
         }
         dot[ndot].size   = line[i].width;
         dot[ndot].p      = line[i].p[0];
         dot[ndot].colour = line[i].colour[0];
         ndot++;
			if (nline > 1)
         	line[i] = line[nline-1];
         nline--;
         i--;
         badline++;
		}
	}

	/* Check for any null normals */
	for (i=0;i<nface3;i++) {
		for (j=0;j<3;j++) {
			if (Modulus(face3[i].n[j]) < EPSILON) {
				n = CalcNormal(face3[i].p[j],face3[i].p[(j+1)%3],face3[i].p[(j+2)%3]);
				face3[i].n[j] = n;
				badnormal++;
			}
		}
	}
   for (i=0;i<nface4;i++) {
      for (j=0;j<4;j++) {
         if (Modulus(face4[i].n[j]) < EPSILON) {
            n = CalcNormal(face4[i].p[j],face4[i].p[(j+1)%4],face4[i].p[(j+3)%4]);
            face4[i].n[j] = n;
            badnormal++;
         }
      }
   }

	if (options.debug) {
		fprintf(stderr,"Simplified face4s = %d\n",badface4);
		fprintf(stderr,"Simplified face3s = %d\n",badface3);
		fprintf(stderr,"Simplified lines  = %d\n",badline);
		fprintf(stderr,"Bad normals       = %d\n",badnormal);
	}
}

/*
	Delete/free all the data structures
*/
void DeleteGeometry(void)
{
	int i;

   ndot     = 0; if (dot     != NULL) { free(dot);     dot     = NULL; }
   nline    = 0; if (line    != NULL) { free(line);    line    = NULL; }
   nface3   = 0; if (face3   != NULL) { free(face3);   face3   = NULL; }
   nface4   = 0; if (face4   != NULL) { free(face4);   face4   = NULL; }
	nball    = 0; if (ball    != NULL) { free(ball);    ball    = NULL; }
	ndisk    = 0; if (disk    != NULL) { free(disk);    disk    = NULL; }
	ncone    = 0; if (cone    != NULL) { free(cone);    cone    = NULL; }
	nlabel   = 0; if (label   != NULL) { free(label);   label   = NULL; }
#if defined(BUILDING_S2PLOT)
	nhandle  = 0; if (handle  != NULL) { free(handle);  handle  = NULL; }
	nbboard  = 0; if (bboard  != NULL) { free(bboard);  bboard  = NULL; }
	nbbset   = 0; if (bbset   != NULL) { free(bbset);   bbset   = NULL; }
	nface3a  = 0; if (face3a  != NULL) { free(face3a);  face3a  = NULL; }
#if defined(S2_3D_TEXTURES)
	ntexpoly3d = 0; if (texpoly3d != NULL) { free(texpoly3d); texpoly3d = NULL; }
#endif
	ntexmesh = 0; if (texmesh != NULL) { free(texmesh); texmesh = NULL; }
#endif

	/* Remove texture facets */
	for (i=0;i<nface4t;i++) {
		glDeleteTextures(1,&(face4t[i].textureid));
		if (face4t[i].rgba != NULL) { 	
			free(face4t[i].rgba); 
			face4t[i].rgba = NULL; 
		}
	}
	nface4t = 0; 
	if (face4t != NULL) { 
		free(face4t); 
		face4t = NULL; 
	}

	/* Remove texture balls */
   for (i=0;i<nballt;i++) {
		glDeleteTextures(1,&(ballt[i].textureid));
      if (ballt[i].rgba != NULL) {
         free(ballt[i].rgba);
         ballt[i].rgba = NULL;
      }
   }
   nballt = 0;
   if (ballt != NULL) {
      free(ballt);
      ballt = NULL;
   }

   MakeGeometry(TRUE, FALSE, 'c');
}


/*
	Set the auto camera position
	The first time, get the file name and read the flight path
	See AppendCameraPosition() for the order of the camera attributes.
		vp,vd,vu,focallength,aperture,eyesep
*/
int AutoPilot(int mode,char *animfile)
{
   const char *fn;
   char fname[256];
	static FILE *fptr = NULL;
	static int cycle = 0;
	XYZ vp,vd,vu;
	double focallength,eyesep,aperture;
	
	if (!options.autopilot)
		return(FALSE);

	/* Are we getting a new flight path ? */
	if (mode == 0) {

		/* If we already have one then close the file */
      if (fptr != NULL)
         fclose(fptr);

		/* Ask the user for the file name or was it passed to us? */
		if (strlen(animfile) < 1) {
		fn = NULL;
   		if (fn == NULL || strlen(fn) <= 0) {
      		fname[0] = '\0';
				fptr = NULL;
      		return(FALSE);
   		}
   		strcpy(fname,fn);
		} else {
			strcpy(fname,animfile);
		}

		/* Attempt to open the file */
		if ((fptr = fopen(fname,"r")) == NULL) {
			fptr = NULL;
			return(FALSE);
		}
	}

	/* Get the next camera position */
	tryagain:
	if (fscanf(fptr,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
		&vp.x,&vp.y,&vp.z, 
		&vd.x,&vd.y,&vd.z, 
		&vu.x,&vu.y,&vu.z,
		&focallength,&aperture,&eyesep) != 12) {
		if (cycle > 0) {
			rewind(fptr);
			cycle = 0;
			goto tryagain;
		} else {
			fclose(fptr);
			fptr = NULL;
			return(FALSE);
		}
	}
	cycle++;
	Normalise(&vd);
	Normalise(&vu);
	camera.vp = vp;
	camera.vd = vd;
	camera.vu = vu;
	camera.focallength = focallength;
   camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
   camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
   camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
	camera.aperture = aperture;
	camera.eyesep = eyesep;

	return(TRUE);
}

/*
	Create a pair of axes as the initial scene
	No error checking....sorry.
*/
void CreateAxes(void)
{
	COLOUR red = {1,0,0},green = {0,1,0}, blue = {0,0,1};

	nline = 3;
	line = (LINE *)malloc(nline*sizeof(LINE));
	line[0].p[0].x   = -1; line[0].p[0].y   = 0;  line[0].p[0].z   = 0;
   line[0].p[1].x   =  1; line[0].p[1].y   = 0;  line[0].p[1].z   = 0;
	line[0].width = 1;
	line[0].colour[0] = red;
   line[0].colour[1] = red;
   line[1].p[0].x   = 0;  line[1].p[0].y   = -1; line[1].p[0].z   = 0;
   line[1].p[1].x   = 0;  line[1].p[1].y   =  1; line[1].p[1].z   = 0;
	line[1].width = 1;
   line[1].colour[0] = green;
	line[1].colour[1] = green;
   line[2].p[0].x   = 0;  line[2].p[0].y   = 0;  line[2].p[0].z   = -1;
   line[2].p[1].x   = 0;  line[2].p[1].y   = 0;  line[2].p[1].z   =  1;
	line[2].width = 1;
   line[2].colour[0] = blue;
	line[2].colour[1] = blue;
}

/*
	Save attributes in a tagged type format
	Do nothing if the file can't be opened
	See ReadViewFile()
*/
#if defined(BUILDING_S2PLOT)
void SaveViewFile(CAMERA c, char *outname)
#else
void SaveViewFile(CAMERA c)
#endif
{
	const char *fn;
	char fname[256];
	FILE *fptr;
	
#if defined(BUILDING_S2PLOT)
	if (!outname) {
#endif
	/* Get the file name */
	fn = NULL;
   if (fn == NULL || strlen(fn) <= 0) 
      return;
	strcpy(fname,fn);
#if defined(BUILDING_S2PLOT)
  } else {
	  strcpy(fname, outname);
  }
#endif

	/* Try to open the file */
   if ((fptr = fopen(fname,"w")) == NULL) 
		return;

	/* Write the attributes */
	fprintf(fptr,"vp %g %g %g\n",c.vp.x,c.vp.y,c.vp.z);
	fprintf(fptr,"vd %g %g %g\n",c.vd.x,c.vd.y,c.vd.z);
	fprintf(fptr,"vu %g %g %g\n",c.vu.x,c.vu.y,c.vu.z);
	fprintf(fptr,"pr %g %g %g\n",c.pr.x,c.pr.y,c.pr.z);
	fprintf(fptr,"focallength %g\n",c.focallength);
	fprintf(fptr,"aperture %g\n",c.aperture);
	fprintf(fptr,"eyeseparation %g\n",c.eyesep);
	if (options.showboundingbox)
		fprintf(fptr,"-bb\n");
	fprintf(fptr,"-sh %g\n",shininess[0]);
	if (options.deltamove > 0)
		fprintf(fptr,"-dm %g\n",options.deltamove);
	fprintf(fptr,"-bg %g %g %g\n",options.background.r,options.background.g,options.background.b);
	fclose(fptr);
}

/*
	Read the saved settings
	Do nothing if the file open failed
	Since tags are used, an attribute that doesn't exist in the
   file won't be changed.
*/
void ReadViewFile(char *name)
{
   const char *fn;
   char fname[256];
	char s[64];
   FILE *fptr;

	/* Get the file name if one wasn't provided */
	if (strlen(name) < 1) {
	fn = NULL;
   	if (fn == NULL || strlen(fn) <= 0) {
			fprintf(stderr,"Failed to open view file \"%s\"\n",name);
      	return;
		}
   	strcpy(fname,fn);
	} else {
		strcpy(fname,name);
	}

	/* Try to open the file */
   if ((fptr = fopen(fname,"r")) == NULL) 
      return;

	/* Read the tags and associated data */
	while (fscanf(fptr,"%s",s) == 1) {
		if (strcmp(s,"vp") == 0)
			fscanf(fptr,"%lf %lf %lf",&camera.vp.x,&camera.vp.y,&camera.vp.z);
   	if (strcmp(s,"vd") == 0)
      	fscanf(fptr,"%lf %lf %lf",&camera.vd.x,&camera.vd.y,&camera.vd.z);
   	if (strcmp(s,"vu") == 0)
      	fscanf(fptr,"%lf %lf %lf",&camera.vu.x,&camera.vu.y,&camera.vu.z);
   	if (strcmp(s,"pr") == 0)
      	fscanf(fptr,"%lf %lf %lf",&camera.pr.x,&camera.pr.y,&camera.pr.z);
   	if (strcmp(s,"focallength") == 0)
      	fscanf(fptr,"%lf",&camera.focallength);
      if (strcmp(s,"aperture") == 0)
         fscanf(fptr,"%lf",&camera.aperture);
      if (strcmp(s,"eyeseparation") == 0)
         fscanf(fptr,"%lf",&camera.eyesep);
		if (strcmp(s,"-bg") == 0)
			fscanf(fptr,"%lf %lf %lf",&options.background.r,&options.background.g,&options.background.b);
		if (strcmp(s,"-dm") == 0)
			fscanf(fptr,"%lf",&options.deltamove);
		if (strcmp(s,"-sh") == 0)
			fscanf(fptr,"%f",&(shininess[0]));
      if (strcmp(s,"-bb") == 0)
         options.showboundingbox = TRUE;
		options.camerahome = camera;
	}
   fclose(fptr);

	if (options.debug) {
		fprintf(stderr,"vp = %g %g %g\n",camera.vp.x,camera.vp.y,camera.vp.z);
		fprintf(stderr,"vd = %g %g %g\n",camera.vd.x,camera.vd.y,camera.vd.z);
		fprintf(stderr,"vu = %g %g %g\n",camera.vu.x,camera.vu.y,camera.vu.z);
		fprintf(stderr,"pr = %g %g %g\n",camera.pr.x,camera.pr.y,camera.pr.z);
		fprintf(stderr,"focal length = %g\n",camera.focallength);
		fprintf(stderr,"aperture = %g\n",camera.aperture);
		fprintf(stderr,"eye separation = %g\n",camera.eyesep);
	}

	Normalise(&camera.vd);
   camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
   camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
   camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
}

/*
	Append the camera attributes to "camera.path"
	Do nothing if the file can't be opened
*/
void AppendCameraPosition(void)
{
	FILE *fptr;

   if ((fptr = fopen("camera.path","a")) != NULL) {
      fprintf(fptr,"%g %g %g ",camera.vp.x,camera.vp.y,camera.vp.z);
      fprintf(fptr,"%g %g %g ",camera.vd.x,camera.vd.y,camera.vd.z);
      fprintf(fptr,"%g %g %g ",camera.vu.x,camera.vu.y,camera.vu.z);
		fprintf(fptr,"%g ",camera.focallength);
		fprintf(fptr,"%g ",camera.aperture);
		fprintf(fptr,"%g\n",camera.eyesep);
      fclose(fptr);
   }
}

/*
	Display extra stuff such as
	- help text
	- bounding box
	- crosshair
*/
void DrawExtras(void) {
  int i,y = 10;
  XYZ p;
  char s[64];
  double delta;
  
  // Inverse colour for the text
#if !defined(BUILDING_S2PLOT)
  COLOUR colour = {0.0,0.0,0.0};
  colour.r = 1 - options.background.r;
  colour.g = 1 - options.background.g;
  colour.b = 1 - options.background.b;
  glColor3f(colour.r,colour.g,colour.b);
#else
  glColor3f(_s2_foreground.r, _s2_foreground.g, _s2_foreground.b);
#endif
  
  // Turn off OpenGL stuff
  //##glLoadName(-1);
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  
  // Draw the light sources in debug mode 
  if (options.debug) {
    for (i=0;i<nlight;i++) {
      p.x = lights[i].p[0];
      p.y = lights[i].p[1];
      p.z = lights[i].p[2];
      Draw3DPlus(p,VectorLength(pmin,pmax)/200);
    }
    if (nlight < 0) {
      delta = VectorLength(pmin,pmax) / 20;
      for (i=0;i<defaultlights;i++) {
	if (deflightpos[i].x == 0)
	  p.x = pmin.x - delta;
	else
	  p.x = pmax.x + delta;
	if (deflightpos[i].y == 0)
	  p.y = pmin.y - delta;
	else
	  p.y = pmax.y + delta;
	if (deflightpos[i].z == 0)
	  p.z = pmin.z - delta;
	else
	  p.z = pmax.z + delta;
	Draw3DPlus(p,VectorLength(pmin,pmax)/200);
      }
    }
  }
  
#if !defined(BUILDING_S2PLOT)
static char *_s2x_keyboardstring = 
"Key Strokes    h,H   home position for camera (also \"home\")\n\
               f,F   focus camera\n\
             1...7   standard camera views\n\
               i,k   translate camera up, down\n\
               j,l   translate camera left, right\n\
               -,+   increase/decrease velocity\n\
               [,]   roll anticlockwise, clockwise\n\
               a,A   toggle autospin\n\
                 c   append camera position to \"camera.path\"\n\
        left arrow   rotate camera left\n\
       right arrow   rotate camera right\n\
        down arrow   rotate camera down\n\
          up arrow   rotate camera up\n\
                 D   delete all geometry\n\
                 d   toggle debug mode\n\
               q,Q   quit (also ESC, \"end\")\n\
Function keys   F1   toggle screen info\n\
                F2   toggle bounding box\n\
                F9   write large (3 x 3) image to disk\n\
               F10   write current image to disk\n\
               F11   create QTVR object images\n\
               F12   toggle image recording\n\
";
#else
static char *_s2x_keyboardstring = 
"S2PLOT Standard Key Actions\n\
Shift-q,Shift-ESC   quit\n\
         h,H,Home   return camera to home position\n\
              f,F   focus camera on centre of geometry\n\
            1...7   standard camera views (*)\n\
       left arrow   rotate camera left\n\
      right arrow   rotate camera right\n\
       down arrow   rotate camera down\n\
         up arrow   rotate camera up\n\
              i,k   translate camera up, down\n\
              j,l   translate camera left, right\n\
              -,+   move backwards, forwards\n\
              [,]   roll anticlockwise, clockwise\n\
              (,)   wider/narrower view angle\n\
              a,A   toggle autospin\n\
                d   toggle debug mode\n\
                r   cycle rendering modes (wireframe, flat, ...)\n\
              <,>   decrease, increase camera delta (\"speed\")\n\
              /,*   decrease, increase autospin rate\n\
----------------------------------------------------------------------\n\
               F1   toggle/cycle help screens\n\
               F2   toggle bounding box\n\
               F5   write geometry and view file to disk\n\
               F6   write geometry, view file and image to disk\n\
               F7   toggle recording of geometry, view file and image\n\
----------------------------------------------------------------------\n\
(*) numeric keys may be remapped by the application you are using\n\
";
#endif


  // Are we showing text help information? 
  if (options.showhelpinfo) {
#if defined(BUILDING_VIEWER)
    DrawGLText(10,options.screenheight - 20,_s2x_keyboardstring);
#else

    // draw a grey semi-trans square for the writing to go on top of
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    float xlo = 4, xhi = 4;
    float ylo = 4, yhi = WINHEIGHT - ylo;
    if ((options.stereo == DUALSTEREO) || (options.stereo == WDUALSTEREO)) {
      glOrtho(0,WINWIDTH /2,0,WINHEIGHT,-1,1); 
      xhi = WINWIDTH/2 - xlo;
    } else if (options.stereo == TRIOSTEREO) {
      glOrtho(0,WINWIDTH/3, 0,WINHEIGHT,-1,1);
      xhi = WINWIDTH/3 - xlo;
    } else {
      glOrtho(0,WINWIDTH,0,WINHEIGHT,-1,1); 
      xhi = WINWIDTH - xlo;
    }
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    s2sci(S2_PG_DKGREY);
    _glColor4f(_S2PENRED, _S2PENGRN, _S2PENBLU, 0.4);
    glBegin(GL_QUADS);
    glVertex3f(xlo, ylo, 0.1);
    glVertex3f(xlo, yhi, 0.1);
    glVertex3f(xhi, yhi, 0.1);
    glVertex3f(xhi, ylo, 0.1);
    glEnd();
    s2sci(S2_PG_WHITE);
    _glColor4f(_S2PENRED, _S2PENGRN, _S2PENBLU, 0.8);
    glBegin(GL_LINES);
    glVertex3f(xlo-1, ylo-1, 0.11);
    glVertex3f(xlo-1, yhi+1, 0.11);
    glVertex3f(xlo-1, yhi+1, 0.11);
    glVertex3f(xhi+1, yhi+1, 0.11);
    glVertex3f(xhi+1, yhi+1, 0.11);
    glVertex3f(xhi+1, ylo-1, 0.11);
    glVertex3f(xhi+1, ylo-1, 0.11);
    glVertex3f(xlo-1, ylo-1, 0.11);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    s2sci(S2_PG_WHITE);
    _glColor4f(_S2PENRED, _S2PENGRN, _S2PENBLU, 1.0);
    glDisable(GL_LIGHTING);
    if (options.showhelpinfo == 2 && _s2_customhelpstr) {
      DrawGLText(10, options.screenheight - 20, _s2_customhelpstr);
    } else {
      DrawGLText(10, options.screenheight - 20, _s2x_keyboardstring);
    }
#endif
  }
  
  if (options.debug) {
    sprintf(s,"Frame rate: %.1f frames/second",interfacestate.framerate);
    DrawGLText(10,y+=13,s);
    sprintf(s,"vp: %g %g %g",camera.vp.x,camera.vp.y,camera.vp.z);
    DrawGLText(10,y+=13,s);
    sprintf(s,"vd: %g %g %g",camera.vd.x,camera.vd.y,camera.vd.z);
    DrawGLText(10,y+=13,s);
    sprintf(s,"vu: %g %g %g",camera.vu.x,camera.vu.y,camera.vu.z);
    DrawGLText(10,y+=13,s);
    sprintf(s,"pr: %g %g %g",camera.pr.x,camera.pr.y,camera.pr.z);
    DrawGLText(10,y+=13,s);
    sprintf(s,"aperture: %g",camera.aperture);
    DrawGLText(10,y+=13,s);
    sprintf(s,"focal length: %g",camera.focallength);
    DrawGLText(10,y+=13,s);
    sprintf(s,"eye sep: %g",camera.eyesep);
    DrawGLText(10,y+=13,s);
  }
  
#if defined(BUILDING_S2PLOT)
  if (_s2_showannotations) { /* use 'n' or 'N' to toggle */
    /* modifications for S2PLOT */
    if (_s2_title) {
      DrawGLText(10, options.screenheight - 20, _s2_title);
    }
    if (_s2_iden) {
      /* can do the following because DrawGLText uses precisely 
       * 8_by_13 font.
       */
      int vpos = 10;
      int maxw = -1;
      char *lastcr = _s2_iden;
      char *p;
      for (p = _s2_iden; p < _s2_iden + strlen(_s2_iden); p++) {
	if ((*p =='\n') || (*p == '\r')) {
	  if ((p - lastcr) > maxw) {
	    maxw = p - lastcr;
	  }
	  lastcr = p;
	  vpos += 13;
	}
      }
      if (maxw < 0) {
	maxw = strlen(_s2_iden);
      }
      int rpos = options.screenwidth - 
	((((options.stereo == DUALSTEREO) || (options.stereo == WDUALSTEREO)) ? 2 : 1)) *
	(10 + 8 * maxw);
      
      DrawGLText(rpos, vpos, _s2_iden);
    }
        
  }
  
  if (_s2_crosshair_vis) {
    _s2priv_drawCrosshair();
  }
  
  if (_s2prompt_length >= 0) {
    char thecmd[S2PROMPT_LEN+1];
    strncpy(thecmd, _s2prompt, _s2prompt_length);
    thecmd[_s2prompt_length] = '\0';
    char screentext[S2PROMPT_LEN + 30];
    sprintf(screentext, "%s %s_", _s2baseprompt, thecmd);
    //DrawGLText(200, 200, screentext);
    DrawGLText(_s2prompt_x * WINWIDTH, _s2prompt_y * WINHEIGHT, screentext);
  }
#endif
  
  //`glEnable(GL_DEPTH_TEST);
  //`glEnable(GL_LIGHTING);
  glPopAttrib();
}


void CleanExit(void)
{
  s2winDestroyWindow();
   ClearAllBuffers();
	exit(0);
}

#include "misc.c"

#if defined(BUILDING_S2PLOT)

/***********************************************************************
 * S2PLOT internal functions
 */


/* if devstr is a valid device, return its index, otherwise -1 */
int _s2priv_find_device(char *devstr) {
  if (!devstr) {
    return -1;
  }
  int i = 0;
  while ((i < _s2_ndevices) && 
	 strncasecmp(devstr, _s2_valid_devices[i].devicename,
		     strlen(devstr))) {
    i++;
  }
  if (i < _s2_ndevices) {
    return i;
  }
  return -1;
}



/* draw the crosshair */
#define XHWIDTH 120
void _s2priv_drawCrosshair(void) {
  if (!_s2_crosshair_vis) {
    /* not supposed to draw - fail silently */
    return;
  }

  /* draw the crosshair */
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  if ((options.stereo == DUALSTEREO) || (options.stereo == WDUALSTEREO)) {
    glOrtho(0,WINWIDTH /2,0,WINHEIGHT,-1,1); 
  } else if (options.stereo == TRIOSTEREO) {
    glOrtho(0,WINWIDTH/3, 0,WINHEIGHT,-1,1);
  } else {
    glOrtho(0,WINWIDTH,0,WINHEIGHT,-1,1); 
  }
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_LIGHTING);
  
  glLineWidth(1);

  float x = _s2_lastmousex;
  float y = WINHEIGHT - 1 - _s2_lastmousey;

  float xlo = MAX(0, x - XHWIDTH/2);
  float xhi = MIN(WINWIDTH - 1, x + XHWIDTH/2);
  float ylo = MAX(0, y - XHWIDTH/2);
  float yhi = MIN(WINHEIGHT - 1, y + XHWIDTH/2);
  
  glBegin(GL_LINES);
  s2sci(S2_PG_LTGREY);
  _glColor4f(_S2PENRED, _S2PENGRN, _S2PENBLU, 0.7);
  glVertex3f(xlo, y, 0);
  glVertex3f(xhi, y, 0);
  glEnd();

  glBegin(GL_LINES);
  s2sci(S2_PG_LTGREY);
  _glColor4f(_S2PENRED, _S2PENGRN, _S2PENBLU, 0.7);
  glVertex3f(x, ylo, 0);
  glVertex3f(x, yhi, 0);
  glEnd();

  glEnd();

  glEnable(GL_LIGHTING);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

}

#if defined(BUILDING_S2PLOT)
#if defined(S2_NO_S2GEOMVIEWER)
/* draw a frame around the active panel */
void _s2priv_drawActiveFrame(int active) {
#define AF_BORDER ((int)(_s2_panelframewidth/2))
  /* draw a frame around the current panel */
  int xw = (_s2_panels[_s2_activepanel].x2 - _s2_panels[_s2_activepanel].x1) * (float)options.screenwidth + 0.5;
  int yw = (_s2_panels[_s2_activepanel].y2 - _s2_panels[_s2_activepanel].y1) * (float)options.screenwidth + 0.5;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  //glOrtho(0,WINWIDTH,0,WINHEIGHT,-1,1); 
  glOrtho(0, xw, 0, yw, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_LIGHTING);
  
  glLineWidth(_s2_panelframewidth);
  
  glBegin(GL_LINE_STRIP);
  if (active) {
    //s2sci(S2_PG_WHITE);
    _glColor4f(_s2_activepanelframecolour.r, 
	       _s2_activepanelframecolour.g,
	       _s2_activepanelframecolour.b, 1.0);
  } else {
    //s2sci(S2_PG_DKGREY);
    _glColor4f(_s2_panelframecolour.r, 
	       _s2_panelframecolour.g,
	       _s2_panelframecolour.b, 1.0);
  }
  //_glColor4f(_S2PENRED, _S2PENGRN, _S2PENBLU, 0.7);
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
#endif
#endif

int handlecomp(const void *a, const void *b) {
  if (((_S2HANDLE *)a)->dist > ((_S2HANDLE *)b)->dist) {
    return -1;
  } else {
    return 1;
  }
}

int bboardcomp(const void *a, const void *b) {
  if (((_S2BBOARD *)a)->dist > ((_S2BBOARD *)b)->dist) {
    return 1;
  } else if (((_S2BBOARD *)a)->dist < ((_S2BBOARD *)b)->dist) {
    return -1;
  } else {
    return 0;
  }
}

int bbptrcomp(const void *a, const void *b) {
  if ((*((_S2BBOARD **)a))->dist > (*((_S2BBOARD **)b))->dist) {
    return -1;
  } else {
    return 1;
  }
}

/* draw the Handles */
void _s2priv_drawHandles(int doscreen) {

  // 0. load texture first time around
  static int htexid = -1;
  if (htexid < 0) {
    htexid = s2loadtexture("target1i.png.128x128.tga");
    int sbw = 0, sbh = 0;
    unsigned char *starbits = ss2gt(htexid, &sbw, &sbh);
    unsigned char *idx = starbits;
    float val;
    int i;
    for (i = 0; i < sbw * sbh; i++) {
      val = (idx[0] + idx[1] + idx[2]) * 0.33333;
      if (val > 255.) {
	val = 255.;
      } else if (val < 0.) {
	val = 0.;
      }
      idx[3] = (char)(val);
      idx[0] = idx[1] = idx[2] = 255;
      idx += 4;
    }
    s2pushtexture(htexid);
  }

  // 1. calculate distance to camera for each handle
  static XYZ CAMP, UP, VIEW, RGT;
  CAMP = camera.vp;
  UP = camera.vu;
  VIEW = camera.vd;
  CROSSPROD(VIEW, UP, RGT);
  //SetVectorLength(&UP, 7.0);
  //SetVectorLength(&RGT, 7.0);

  int i=0;  
#if defined(S2OPENMP)
#pragma omp parallel shared(handle,CAMP,nhandle) private(i)
  {
#pragma omp for schedule(dynamic,1000)
#endif
    for (i = 0; i < nhandle; i++) {
      handle[i].dist = (CAMP.x - handle[i].p.x) * (CAMP.x - handle[i].p.x) +
	(CAMP.y - handle[i].p.y) * (CAMP.y - handle[i].p.y) +
	(CAMP.z - handle[i].p.z) * (CAMP.z - handle[i].p.z);
    }
#if defined(S2OPENMP)
  }
#endif
  
  // 2. sort the handles
#if defined(S2OPENMP)
  mQsortMTh(handle, nhandle);
#else
  qsort(handle, nhandle, sizeof(_S2HANDLE), handlecomp);
#endif  

  // 3. draw the handles
  static XYZ vertices[4];
  static XYZ normal;
  static float size;

  // for handles: no lighting, normal blending
  GLint _gl_light = glIsEnabled(GL_LIGHTING);
  glDisable(GL_LIGHTING);
  GLint _gl_blend = glIsEnabled(GL_BLEND);
  glEnable(GL_BLEND);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (i = 0; i < nhandle; i++) {

    if ((doscreen && !strlen(handle[i].whichscreen)) ||
	(!doscreen && strlen(handle[i].whichscreen))) {
      continue;
    }
    if (doscreen && !(strlen(_s2_doingScreen) &&
		      strstr(handle[i].whichscreen, _s2_doingScreen))) {
      continue;
    }
    
    if (doscreen) {
      size = handle[i].size;

      vertices[0].x = handle[i].p.x - size;
      vertices[0].y = handle[i].p.y + size * ss2qar();
      vertices[0].z = handle[i].p.z;

      vertices[1].x = handle[i].p.x + size;
      vertices[1].y = handle[i].p.y + size * ss2qar();
      vertices[1].z = handle[i].p.z;

      vertices[2].x = handle[i].p.x + size;
      vertices[2].y = handle[i].p.y - size * ss2qar();
      vertices[2].z = handle[i].p.z;

      vertices[3].x = handle[i].p.x - size;
      vertices[3].y = handle[i].p.y - size * ss2qar();
      vertices[3].z = handle[i].p.z;

      int jj;
      for (jj = 0; jj < 4; jj++) {
	/*
	s2UnProject(_ss_view[0] + _ss_view[2] * vertices[jj].x + 0.5, 
		     _ss_view[1] + _ss_view[3] * vertices[jj].y + 0.5,
		     vertices[jj].z,
		     _ss_model, _ss_proj, _ss_view, &vertices[jj].x,
		     &vertices[jj].y, &vertices[jj].z);
	*/
	s2UnProject(_s2_dragview[0] + _s2_dragview[2] * vertices[jj].x + 0.5, 
		     _s2_dragview[1] + _s2_dragview[3] * vertices[jj].y + 0.5,
		     vertices[jj].z,
		     _s2_dragmodel, _s2_dragproj, _s2_dragview, &vertices[jj].x,
		     &vertices[jj].y, &vertices[jj].z);
      }

    } else {
      // XXX - remove scaling?
      size = handle[i].size * powf(handle[i].dist, 0.3);

      vertices[0].x = handle[i].p.x + size * (UP.x - RGT.x);
      vertices[0].y = handle[i].p.y + size * (UP.y - RGT.y);
      vertices[0].z = handle[i].p.z + size * (UP.z - RGT.z);
      
      vertices[1].x = handle[i].p.x + size * (UP.x + RGT.x);
      vertices[1].y = handle[i].p.y + size * (UP.y + RGT.y);
      vertices[1].z = handle[i].p.z + size * (UP.z + RGT.z);
      
      vertices[2].x = handle[i].p.x + size * (-UP.x + RGT.x);
      vertices[2].y = handle[i].p.y + size * (-UP.y + RGT.y);
      vertices[2].z = handle[i].p.z + size * (-UP.z + RGT.z);
      
      vertices[3].x = handle[i].p.x + size * (-UP.x - RGT.x);
      vertices[3].y = handle[i].p.y + size * (-UP.y - RGT.y);
      vertices[3].z = handle[i].p.z + size * (-UP.z - RGT.z);
      
    } 

    glPushName(handle[i].id);

    // cannot use s2 Vf4tIDt function here because that would store
    // the geometry rather than drawing it NOW!
    glDepthMask(GL_FALSE);
    glEnable(GL_TEXTURE_2D);
    if (handle[i].selected) {
      if (handle[i].hitexid >= 0) {
	glBindTexture(GL_TEXTURE_2D, handle[i].hitexid);
      } else {
	glBindTexture(GL_TEXTURE_2D, htexid);
      } 
    } else {
      if (handle[i].texid >= 0) {
	glBindTexture(GL_TEXTURE_2D, handle[i].texid);
      } else {
	glBindTexture(GL_TEXTURE_2D, htexid);
      }
    }
    glBegin(GL_QUADS);
    if (handle[i].selected) {
      _glColor4f(handle[i].hilite.r, handle[i].hilite.g, handle[i].hilite.b,
		  0.99);
    } else {
      _glColor4f(handle[i].col.r, handle[i].col.g, handle[i].col.b,
		  0.99);
    }

    normal = CalcNormal(vertices[0], vertices[1], vertices[2]);
    glNormal3f(normal.x, normal.y, normal.z);
    glTexCoord2f(0.0,1.0);
    glVertex3f(vertices[0].x,vertices[0].y,vertices[0].z);
    glTexCoord2f(1.0,1.0);
    glVertex3f(vertices[1].x,vertices[1].y,vertices[1].z);
    glTexCoord2f(1.0,0.0);
    glVertex3f(vertices[2].x,vertices[2].y,vertices[2].z);
    glTexCoord2f(0.0,0.0);
    glVertex3f(vertices[3].x,vertices[3].y,vertices[3].z);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_TRUE);
    
    glPopName();

  }

  // return lighting, blending to previous state
  if (_gl_light) {
    glEnable(GL_LIGHTING);
  }
  if (!_gl_blend) {
    glDisable(GL_BLEND);
  }
}


void _s2priv_drawBBsets(void) {
  // for bboards: no lighting, normal blending
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  int i, j;
  for (i = 0; i < nbbset; i++) {
    if (bbset[i].trans=='t') {
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    } else if (bbset[i].trans=='s') {
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
    }

    glBindTexture(GL_TEXTURE_2D, bbset[i].texid);

    // This is how will our point sprite's size will be modified by 
    // distance from the viewer
    float quadratic[] =  { 1.0f, 0.0f, 0.01f };
    glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
    
    // Query for the max point size supported by the hardware
    float maxSize = 0.0f;
    glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );
    
    // Clamp size to 100.0f or the sprites could get a little too big on some  
    // of the newer graphic cards. My ATI card at home supports a max point 
    // size of 1024.0f!
    if( maxSize > 100.0f )
      maxSize = 100.0f;

    if (bbset[i].size > maxSize) {
      bbset[i].size = maxSize;
    }
    
    glPointSize(bbset[i].size);
    
    // The alpha of a point is calculated to allow the fading of points 
    // instead of shrinking them past a defined threshold size. The threshold 
    // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
    // the minimum and maximum point sizes.
    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );
    
    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );
    
    // Specify point sprite texture coordinate replacement mode for each 
    // texture unit
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
    
    //
    // Render point sprites...
    //
    
    glEnable( GL_POINT_SPRITE_ARB );
    glBegin( GL_POINTS );

    for(j = 0; j < bbset[i].n; j++) {
      //glPointSize(j % 100);
      //glPointSize(bbset[i].size * (0.8 + 0.4 * rand() / (float)(RAND_MAX)));
      //glPointSize(bbset[i].size);
      //glBegin( GL_POINTS );
      glColor4f( bbset[i].colarray[j*4+0],
		 bbset[i].colarray[j*4+1],
		 bbset[i].colarray[j*4+2],
		 bbset[i].colarray[j*4+3]);
      glVertex3f(bbset[i].vertarray[j*3+0],
		 bbset[i].vertarray[j*3+1],
		 bbset[i].vertarray[j*3+2]);
      //glEnd();
    }
    glEnd();
    
    glDisable( GL_POINT_SPRITE_ARB );
    
    
  }
  
  glDisable(GL_TEXTURE_2D);
  glDepthMask(GL_TRUE);
  // re-enable lighting, disable blending
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
  
}

//#define NEW_BILLBOARD_CODE 1
#if defined(NEW_BILLBOARD_CODE) 

// NOTE THE FOLLOWING CODE HAS NOT HAD UPDATES MADE FOR eg. ASPECT RATIO

/* draw the billboards */
void _s2priv_drawBillboards(void) {
  /* billboard texture method, correctly sorted  */

  // 1. calculate distance to camera for each bboard
  static XYZ CAMP, UP, VIEW, RGT;
  CAMP = camera.vp;
  UP = camera.vu;
  VIEW = camera.vd;
  CROSSPROD(VIEW, UP, RGT);
  SetVectorLength(&UP, 7.0);
  SetVectorLength(&RGT, 7.0);

  int i;  
  for (i = 0; i < nbboard; i++) {
    bboard[i].dist = (CAMP.x - bboard[i].p.x) * (CAMP.x - bboard[i].p.x) +
      (CAMP.y - bboard[i].p.y) * (CAMP.y - bboard[i].p.y) +
      (CAMP.z - bboard[i].p.z) * (CAMP.z - bboard[i].p.z);
  }
  
  // 2. sort the bboards
  qsort(bboard, nbboard, sizeof(_S2BBOARD), bboardcomp);
  
  // 3. draw the bboards
  static XYZ vertices[4];
  static XYZ normal;
  static float size;

  // for bboards: no lighting, normal blending
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);

  XYZ nRGT = RGT;
  Normalise(&nRGT);
  XYZ nUP = UP;
  Normalise(&nUP);
  XYZ nVIEW = VIEW;
  Normalise(&nVIEW);
  XYZ tmpb, dilRGT, dilUP;

  // from http://nehe.gamedev.net/data/articles/article.asp?article=19
  GLfloat matrix[16];
  matrix[0] = nRGT.x;
  matrix[1] = nRGT.y;
  matrix[2] = nRGT.z;
  matrix[3] = 0.;
  
  matrix[4] = nUP.x;
  matrix[5] = nUP.y;
  matrix[6] = nUP.z;
  matrix[7] = 0.;
  
  matrix[8] = nVIEW.x;
  matrix[9] = nVIEW.y;
  matrix[10] = nVIEW.z;
  matrix[11] = 0.;
  
  //matrix[12] = bboard[i].p.x;
  //matrix[13] = bboard[i].p.y;
  //matrix[14] = bboard[i].p.z;
  matrix[15] = 1.0;

#define BBITI 7.0
  vertices[0].x = -BBITI;
  vertices[0].y = +BBITI;
  vertices[0].z = 0.0;
  
  vertices[1].x = +BBITI;
  vertices[1].y = +BBITI;
  vertices[1].z = 0.0;
  
  vertices[2].x = +BBITI;
  vertices[2].y = -BBITI;
  vertices[2].z = 0.0;
  
  vertices[3].x = -BBITI;
  vertices[3].y = -BBITI;
  vertices[3].z = 0.0;

  normal = CalcNormal(vertices[0], vertices[1], vertices[2]);
  
  glEnable(GL_TEXTURE_2D);

  GLuint fastbboard = glGenLists(1);
  glNewList(fastbboard, GL_COMPILE);
  //glBindTexture(GL_TEXTURE_2D, bboard[0].texid);
  glBegin(GL_QUADS);
  glNormal3f(normal.x, normal.y, normal.z);
  glTexCoord2f(0.0,1.0);
  glVertex3f(vertices[0].x,vertices[0].y,vertices[0].z);
  glTexCoord2f(1.0,1.0);
  glVertex3f(vertices[1].x,vertices[1].y,vertices[1].z);
  glTexCoord2f(1.0,0.0);
  glVertex3f(vertices[2].x,vertices[2].y,vertices[2].z);
  glTexCoord2f(0.0,0.0);
  glVertex3f(vertices[3].x,vertices[3].y,vertices[3].z);
  glEnd();
  glEndList();

  for (i = 0; i < nbboard; i++) {

    size = bboard[i].size; // * powf(bboard[i].dist, 0.3);    

    if (bboard[i].trans == 't') {
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    } else if (bboard[i].trans == 's') {
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
    }

    // cannot use s2 Vf4tIDt function here because that would store
    // the geometry rather than drawing it NOW!
    glPushMatrix();

    matrix[12] = bboard[i].p.x;
    matrix[13] = bboard[i].p.y;
    matrix[14] = bboard[i].p.z;
    glMultMatrixf(matrix);
    glScalef(size * (1. + bboard[i].str.x), 
	     size * (1. + bboard[i].str.y), 
	     size * (1. + bboard[i].str.z));
    glBindTexture(GL_TEXTURE_2D, bboard[i].texid);
    _glColor4f(bboard[i].col.r, bboard[i].col.g, bboard[i].col.b,
		bboard[i].alpha);
    glCallList(fastbboard);

    glPopMatrix();

  }
  glDisable(GL_TEXTURE_2D);
  glDepthMask(GL_TRUE);

  // re-enable lighting, disable blending
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);

  glDeleteLists(fastbboard, 1);

}

#else 

/* draw the billboards */
void _s2priv_drawBillboards(void) {
  /* billboard texture method, correctly sorted  */

  // 1. calculate distance to camera for each bboard
  static XYZ CAMP, UP, VIEW, RGT;
  CAMP = camera.vp;
  UP = camera.vu;
  VIEW = camera.vd;
  CROSSPROD(VIEW, UP, RGT);
  SetVectorLength(&UP, 7.0);
  SetVectorLength(&RGT, 7.0);
  Normalise(&VIEW);

#if defined(POINTER_SORT)
  static _S2BBOARD **bbptr = NULL;
  bbptr = (_S2BBOARD **)realloc(bbptr, nbboard * sizeof(_S2BBOARD *));
#endif

  int i = 0;
#if defined(S2OPENMP)
  //int tid, nthreads;
#pragma omp parallel shared(bboard,CAMP,nbboard) private(i)
  {
#pragma omp for schedule(dynamic,1000)
#endif
    for (i = 0; i < nbboard; i++) {
      bboard[i].dist = (CAMP.x - bboard[i].p.x) * (CAMP.x - bboard[i].p.x) +
	(CAMP.y - bboard[i].p.y) * (CAMP.y - bboard[i].p.y) +
	(CAMP.z - bboard[i].p.z) * (CAMP.z - bboard[i].p.z);
#if defined(POINTER_SORT)
      bbptr[i] = &(bboard[i]);
#endif
    }
#if defined(S2OPENMP)
  }
#endif

  // SORT RESULTS IN ORDERED LIST: CLOSEST TO FURTHEST

  // 2. sort the bboards
#if !defined(POINTER_SORT)
  // direct method
#if defined(S2OPENMP)
  mQsortMT(bboard, nbboard);
#else
  qsort(bboard, nbboard, sizeof(_S2BBOARD), bboardcomp);
#endif  

#else
  // offset pointer method
#if defined(S2OPENMP)
  mQsortMTptr(bbptr, nbboard);
#else
  qsort(bbptr, nbboard, sizeof(_S2BBOARD *), bbptrcomp);
#endif

#endif

  // 3. calculate the bboard vertices and normals
  static GLfloat *_bb_vertices = NULL;
  _bb_vertices = (GLfloat *)realloc(_bb_vertices, 4 * 3 * nbboard * sizeof(GLfloat));
  static GLfloat *_bb_normal = NULL;
  _bb_normal = (GLfloat *)realloc(_bb_normal, 4 * 3 * nbboard * sizeof(GLfloat));
  static GLfloat *_bb_colours = NULL;
  _bb_colours = (GLfloat *)realloc(_bb_colours, 4 * 4 * nbboard * sizeof(GLfloat));
  static GLfloat *_bb_texcoords = NULL;
  _bb_texcoords = (GLfloat *)realloc(_bb_texcoords, 4 * 2 * nbboard * sizeof(GLfloat));
  static float size;

  XYZ nRGT = RGT;
  Normalise(&nRGT);
  XYZ nUP = UP;
  Normalise(&nUP);
  XYZ tmpb, dilRGT, dilUP;

  int j, bi;
#if defined(S2OPENMP)
#pragma omp parallel shared(bboard,nRGT,nUP,_bb_vertices,_bb_normal,_bb_colours,_bb_texcoords) private(tmpb,dilRGT,dilUP,size,i,j,bi)
  {
#pragma omp for schedule(dynamic,500)
#endif
    for (i = 0; i < nbboard; i++) {

      // SORT RESULTS IN ORDERED LIST: CLOSEST TO FURTHEST
      // SO HERE WE INVERT THE ORDER AS WE WANT TO DRAW FARTHEST FIRST
      bi = nbboard-1-i;
      
      // dilate the UP and RIGHT vectors to stretch the texture
      tmpb = bboard[bi].str;
      SetVectorLength(&tmpb, DotProduct(bboard[bi].str, nRGT));
      dilRGT = VectorAdd(VectorMul(RGT, bboard[bi].aspect), tmpb);
      
      tmpb = bboard[bi].str;
      SetVectorLength(&tmpb, DotProduct(bboard[bi].str, nUP));
      dilUP = VectorAdd(UP, tmpb);

      //XYZ xoff = nRGT;
      //SetVectorLength(&xoff, bboard[i].offset.x);
      //XYZ yoff = nUP;
      //SetVectorLength(&yoff, bboard[i].offset.y);
      //XYZ toff = VectorAdd(xoff, yoff);
      XYZ toff = VectorAdd(VectorMul(nRGT, bboard[bi].offset.x),
			   VectorMul(nUP, bboard[bi].offset.y));

      // rotate dilRGT and dilUP about the view direction by position angle
      // "Unit" suffix is the function which requires VIEW already normalised
      dilRGT = ArbitraryRotateUnit(dilRGT, bboard[bi].pa, VIEW);
      dilUP = ArbitraryRotateUnit(dilUP, bboard[bi].pa, VIEW);
      
      size = bboard[bi].size; // * powf(bboard[bi].dist, 0.3);

      _bb_vertices[i*3*4+0*3+0] = bboard[bi].p.x + toff.x + size * (dilUP.x - dilRGT.x);
      _bb_vertices[i*3*4+0*3+1] = bboard[bi].p.y + toff.y + size * (dilUP.y - dilRGT.y);
      _bb_vertices[i*3*4+0*3+2] = bboard[bi].p.z + toff.z + size * (dilUP.z - dilRGT.z);
      
      _bb_vertices[i*3*4+1*3+0] = bboard[bi].p.x + toff.x + size * (dilUP.x + dilRGT.x);
      _bb_vertices[i*3*4+1*3+1] = bboard[bi].p.y + toff.y + size * (dilUP.y + dilRGT.y);
      _bb_vertices[i*3*4+1*3+2] = bboard[bi].p.z + toff.z + size * (dilUP.z + dilRGT.z);
      
      _bb_vertices[i*3*4+2*3+0] = bboard[bi].p.x + toff.x + size * (-dilUP.x + dilRGT.x);
      _bb_vertices[i*3*4+2*3+1] = bboard[bi].p.y + toff.y + size * (-dilUP.y + dilRGT.y);
      _bb_vertices[i*3*4+2*3+2] = bboard[bi].p.z + toff.z + size * (-dilUP.z + dilRGT.z);
      
      _bb_vertices[i*3*4+3*3+0] = bboard[bi].p.x + toff.x + size * (-dilUP.x - dilRGT.x);
      _bb_vertices[i*3*4+3*3+1] = bboard[bi].p.y + toff.y + size * (-dilUP.y - dilRGT.y);
      _bb_vertices[i*3*4+3*3+2] = bboard[bi].p.z + toff.z + size * (-dilUP.z - dilRGT.z);
      
      // assign normal
      for (j = 0; j < 4; j++) {
	_bb_normal[i*3*4+j*3+0] = -VIEW.x;
	_bb_normal[i*3*4+j*3+1] = -VIEW.y;
	_bb_normal[i*3*4+j*3+2] = -VIEW.z;
      }

      // assign colours (desaturating if necessary)
      for (j = 0; j < 4; j++) {
	if (_s2_devcap & _S2DEVCAP_NOCOLOR) {
	  _bb_colours[i*4*4+j*4+0] = _bb_colours[i*4*4+j*4+1] = _bb_colours[i*4*4+j*4+2] = 
	    0.33 * (bboard[bi].col.r + bboard[bi].col.g + bboard[bi].col.b);
	} else {
	  _bb_colours[i*4*4+j*4+0] = bboard[bi].col.r;
	  _bb_colours[i*4*4+j*4+1] = bboard[bi].col.g;
	  _bb_colours[i*4*4+j*4+2] = bboard[bi].col.b;
	}
	_bb_colours[i*4*4+j*4+3] = bboard[bi].alpha;
      }

      // assign texture coordinates
      // corner 1
      _bb_texcoords[i*2*4+0*2+0] = 0.0;
      _bb_texcoords[i*2*4+0*2+1] = 1.0;
      // corner 2
      _bb_texcoords[i*2*4+1*2+0] = 1.0;
      _bb_texcoords[i*2*4+1*2+1] = 1.0;
      // corner 3
      _bb_texcoords[i*2*4+2*2+0] = 1.0;
      _bb_texcoords[i*2*4+2*2+1] = 0.0;
      // corner 4
      _bb_texcoords[i*2*4+3*2+0] = 0.0;
      _bb_texcoords[i*2*4+3*2+1] = 0.0;

    }
#if defined(S2OPENMP)
  }
#endif

  // for bboards: no lighting, normal blending, textures on
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);

  for (i = 0; i < nbboard; i++) {
  
    // SORT RESULTS IN ORDERED LIST: CLOSEST TO FURTHEST
    // SO HERE WE INVERT THE ORDER AS WE WANT TO DRAW FARTHEST FIRST
    // [for indexing into bboard[...] array only!]
    bi = nbboard-1-i;

    if (bboard[bi].trans == 't') {
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    } else if (bboard[bi].trans == 's') {
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
    }
    glBindTexture(GL_TEXTURE_2D, bboard[bi].texid);

    int j = i+1;
    int bj = nbboard-1-j;
    while ((j < nbboard) && 
	   // (j-i < 5000)) &&
	   (bboard[bj].trans == bboard[bi].trans) &&
	   (bboard[bj].texid == bboard[bi].texid)) {
      j++;
      bj--;
    }
    j--;
    //j = i;

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, _bb_vertices+(i*3*4));

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, _bb_texcoords+(i*2*4));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, _bb_normal+(i*3*4));

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_FLOAT, 0, _bb_colours+(i*4*4));

    glDrawArrays(GL_QUADS, 0, (j-i+1)*4);

    i = j; // loop will increment it!

    glDepthMask(GL_TRUE);

  }

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);

  // texturing off, disable blending, lighting on
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);

}

#endif

_S2HANDLE *_s2priv_findHandle(GLuint item) {
  // look through all geometry lists to find this item...
  
  // for now, just the current dynamic list
  _S2HANDLE *hd = NULL;
  int i = 0;
  while (i < nhandle_d && !hd) {
    if (handle_d[i].id == (int)item) {
      hd = &(handle_d[i]);
    }
    i++;
  }
  
  return hd;
}

/* handle clicks on handles */
void _s2priv_handleHits(int nhits, GLuint *list) {
  int i, id;
  GLuint numnames;
  GLuint *bufp = list;
  for (i=0;i<nhits;i++) {
    numnames = *bufp++;
    bufp++;
    bufp++;
    while(numnames--) {
      id = (int) *bufp++;
      if (id > -1 && _s2_handlecallback) {
	/* call callback */
	_s2_handlecallback(&id);
      }
    }
  }
}

#if (S2_NO_S2GEOMVIEWER)
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
#endif

/***********************************************************************
 * S2PLOT public function implementations 
 */


/***********************************************************************
 *
 * OPENING, CLOSING AND SELECTING DEVICES
 *
 ***********************************************************************
 */

int s2open(int ifullscreen, int istereo, int iargc, char **iargv) {
  static int beenhere = 0;
  if (beenhere) {
    _s2warn("s2open", "Not executing s2open [been here before!]");
    return 0;
  }
  beenhere = 1;

  int i = 1;
  //char meshfile[128] = "";
  COLOUR black = {0,0,0};
  XYZ unity = {1,1,1},zero = {0,0,0};

  // Default options
  options.debug             = FALSE;
  options.stereo            = NOSTEREO;   
  options.fullscreen        = FALSE;
  options.dometype          = NODOME;
  options.showdomeintensity = TRUE;
#if defined(BUILDING_S2PLOT)
  options.rendermode        = SHADE_DIFFUSE;
#else
  options.rendermode        = WIREFRAME;  
#endif
  options.showtextures      = TRUE;
  options.showboundingbox   = FALSE;
  options.recordimages      = FALSE;
  options.windowdump        = 0;
  options.makeqtvr          = FALSE;
  options.showhelpinfo      = FALSE;
  options.projectiontype    = PERSPECTIVE;
  options.selectmode        = FALSE;
  options.targetfps         = 90;
  options.interaction       = INSPECT;
  options.autopilot         = FALSE;
  options.autospin          = zero;  // 0, 1, or -1 for still, left, right 
  options.interactspeed     = 1;     // Scale factor on deltamove
  options.deltamove         = -1;    // Movement/rotation increment 
  options.background        = black;
  options.sphereresolution  = 16;
  options.doantialias       = FALSE;
  options.pointscale        = 1;
  options.linescale         = 1;
  options.scalefactor       = unity;
  options.localviewer       = TRUE;
  options.twosided          = TRUE;
  options.faceorder         = GL_CCW;
  options.texturetype       = GL_TEXTURE_2D;
  camera.fishrotate         = 0;
  
  // Interface default
  interfacestate.currentbutton  = -1;
  interfacestate.mousexlast     = -1;
  interfacestate.mouseylast     = -1;
  interfacestate.spaceballxlast = -1;
  interfacestate.spaceballylast = -1;
  interfacestate.spaceballzlast = -1;
  interfacestate.framerate      = 0;
  
  
  /* default device capabilities */
  _s2_devcap = _S2DEVCAP_CURSOR | _S2DEVCAP_CROSSHAIR | _S2DEVCAP_SELECTION;

  // name of our device driver - default none
  char *_s2_driver = NULL;

  if (istereo == 1) {
    options.stereo = ACTIVESTEREO;
  } else if (istereo == 2) {
    options.stereo = DUALSTEREO;
    _s2_devcap = _S2DEVCAP_CROSSHAIR | _S2DEVCAP_SELECTION;
  } else if (istereo == 3) {
    /* used for now to support fisheye */
    options.stereo = NOSTEREO;
    options.projectiontype = FISHEYE;
    options.dometype = VSPHERICAL;
    _s2_devcap = _S2DEVCAP_CURSOR;
    _s2_driver = (char *)calloc(strlen("s2fishdome")+1, sizeof(char));
    strcpy(_s2_driver, "s2fishdome");
  } else if (istereo == 6) {
    /* used for now to support truncated base fisheye */
    options.stereo = NOSTEREO;
    options.projectiontype = FISHEYE;
    options.dometype = TRUNCBOTTOM;
    _s2_devcap = _S2DEVCAP_CURSOR;
    _s2_driver = (char *)calloc(strlen("s2fishdome")+1, sizeof(char));
    strcpy(_s2_driver, "s2fishdome");
  } else if (istereo == 7) {
    /* used for now to support truncated top fisheye */
    options.stereo = NOSTEREO;
    options.projectiontype = FISHEYE;
    options.dometype = TRUNCTOP;
    _s2_devcap = _S2DEVCAP_CURSOR;
    _s2_driver = (char *)calloc(strlen("s2fishdome")+1, sizeof(char));
    strcpy(_s2_driver, "s2fishdome");
  } else if (istereo == 5) {
    /* used for now to support warped (fisheye) */
    options.stereo = NOSTEREO;
    options.projectiontype = FISHEYE;
    options.dometype = WARPMAP;
    _s2_devcap = _S2DEVCAP_CURSOR;
    _s2_driver = (char *)calloc(strlen("s2fishdome")+1, sizeof(char));
    strcpy(_s2_driver, "s2fishdome");
    /* get meshfile name from environ variable */
    char *s2meshfn = getenv("S2PLOT_MESHFILE");
    if (s2meshfn) {
      strcpy(options.meshfile, s2meshfn);
    } else {
      char *tmpchr = getenv("S2PATH");
      char sufx[] = "/meshes/uvxy.map"; 
      if (tmpchr) {
	sprintf(options.meshfile, "%s%s", tmpchr, sufx);
      }
    }
  } else if (istereo == 8) {
    /* used for now to support anaglyph (red/blue) mode */
    options.stereo = ANAGLYPHSTEREO;
    _s2_driver = (char *)calloc(strlen("s2anaglyph")+1, sizeof(char));
    strcpy(_s2_driver, "s2anaglyph");
  } else if (istereo == 9) {
    /* pre-desaturated anaglyph stereo */
    options.stereo = ANAGLYPHSTEREO;
    _s2_devcap |= _S2DEVCAP_NOCOLOR;
    _s2_driver = (char *)calloc(strlen("s2anaglyph")+1, sizeof(char));
    strcpy(_s2_driver, "s2anaglyph");
  } else if (istereo == 30) {
    /* trio stereo: center + left + right on passive screen */
    options.stereo = TRIOSTEREO;
  } else if (istereo == 31) {
    /* scan-line interleaved stereo */
    options.stereo = INTERSTEREO;
    _s2_driver = (char *)calloc(strlen("s2interstereo")+1, sizeof(char));
    strcpy(_s2_driver, "s2interstereo");
  } else if (istereo == 32) {
    /* warped passive stereo */
    options.stereo = WDUALSTEREO;
    _s2_devcap = _S2DEVCAP_CROSSHAIR | _S2DEVCAP_SELECTION;
    _s2_driver = (char *)calloc(strlen("s2warpstereo")+1, sizeof(char));
    strcpy(_s2_driver, "s2warpstereo");
  } else if (istereo == -1) {
    /* null device */
    options.stereo = NULLSTEREO;
    _s2_devcap = 0;
  } else if (istereo) {
    /* unknown stereo mode */
    _s2warn("s2open*", "unsupported stereo mode");
    return(0);
  }
  
  if (ifullscreen == 1) {
    options.fullscreen = TRUE;
  } else if (ifullscreen) {
    /* unknown screen mode */
    _s2warn("s2open*", "unknown fullscreen mode");
    return(0);
  }
  
  /* Setup, initialise OpenGL and windowing system (glut or other) */
  _s2debug("s2open*", "initialising window system (glut or other)");

#if defined(S2DARWIN)
  /* darwin glut framework changes directory and doesn't restore it
   * correctly. */
  char cwd[256];
  getcwd(cwd, 255);
#endif
  if (options.stereo >= 0 && iargc > -1) {
    s2winInit(&iargc,iargv);
  } else {
    _s2warn("s2open", "not calling s2winInit (iargc < 0) !");
  }
#if defined(S2DARWIN)
  chdir(cwd);
#endif

  _s2x_ati = 0;
  {
    char *tmpchr = getenv("S2PLOT_INTERSCANATI");
    if (tmpchr) {
      if (!strcmp(tmpchr, "yes")) {
	_s2x_ati = 1;
      }
    }
  }

  if (options.stereo >= 0 && iargc > -1) {
    CreateOpenGL();
  } else {
    options.screenwidth = 100;
    options.screenheight = 100;
    _s2warn("s2open", "not calling CreateOpenGL (iargc < 0) !");
  }

	// Can we handle non power of 2 textures?
#if GL_EXT_texture_rectangle
  options.texturetype = GL_TEXTURE_RECTANGLE_EXT;
#else
#if GL_NV_texture_rectangle
  options.texturetype = GL_TEXTURE_RECTANGLE_NV;
#else
  options.texturetype = GL_TEXTURE_2D;
#endif
#endif
  if (options.texturetype == GL_TEXTURE_2D) {
    _s2debug("s2open*", "non-power-of-2 textures not supported");
  } else {
    _s2debug("s2open*", "non-power-of-2 textures supported");
  }
        
   /* define the device coordinate system bounds */
   _s2devicemin[_S2XAX] = -1.0;
   _s2devicemax[_S2XAX] = +1.0;
   _s2devicemin[_S2YAX] = -1.0;
   _s2devicemax[_S2YAX] = +1.0;
   _s2devicemin[_S2ZAX] = -1.0;
   _s2devicemax[_S2ZAX] = +1.0;

   /* default expansion of near/far clipping planes = 1.0 */
   _s2_nearfar_expand = 1.0;
   _s2_save_near = _s2_save_far = -1; // not set yet

   /* default character size/height */
   _s2_charsize = 1.0;

   /* default foreground */
   _s2_foreground.r = _s2_foreground.g = _s2_foreground.b = 1.0;

   /* default line width */
   _s2_linewidth = 1.0;

   /* default line style */
   _s2_linestyle = 1;

   /* title is default empty */
   _s2_title = NULL;
   /* iden is null by default too */
   _s2_iden = NULL;

   /* lines are just lines not cones */
   _s2_conelines = 0;

   /* install default colors (0 - 15) following PGPlot choices */
   _s2_cmapsize = _s2_pg_cmapsize;
   _s2_colormap = (COLOUR *)calloc(_s2_cmapsize, sizeof(COLOUR));
   if (!_s2_colormap) {
     _s2warn("s2open*", "failed to allocate colormap memory");
     return(0);
   }
   for (i = 0; i < _s2_cmapsize; i++) {
     _s2_colormap[i].r = _s2_pg_colormap[i].r;
     _s2_colormap[i].g = _s2_pg_colormap[i].g;
     _s2_colormap[i].b = _s2_pg_colormap[i].b;
   }
   /* set default pen color (foreground color, white) */
   _s2_colidx = S2_PG_WHITE;

   /* add 64-element greyscale colormap */
   int ngreys = 64;
   _s2_colormap = (COLOUR *)realloc(_s2_colormap, (_s2_cmapsize+ngreys) *
				    sizeof(COLOUR));
   if (!_s2_colormap) {
     _s2warn("s2open*", "failed to allocate colormap memory");
     return(0);
   }
   for (i = 0; i < ngreys; i++) {
     _s2_colormap[_s2_cmapsize + i].r = _s2_colormap[_s2_cmapsize + i].g = 
       _s2_colormap[_s2_cmapsize + i].b =
       (float)i / (float)(ngreys - 1);
   }
   _s2_cmapsize += ngreys;
   /* set default color index range for shading */
   _s2_colr1 = _s2_cmapsize - ngreys;
   _s2_colr2 = _s2_cmapsize - 1;

   /* default is to show annotations */
   _s2_showannotations = 1;

   /* default is no custom help string */
   _s2_customhelpstr = NULL;

   /* no callback function by default */
   _s2_callback = NULL;

   /* etc. */
   _s2_callbackx = NULL;
   _s2_callbackx_data = NULL;

   /* no user-installed keyboard handler by default */
   _s2_user_keys = NULL;

   /* default list is static geometry */
   _s2_dynamicEnabled = 0;

   /* default is to erase dynamic geometry each iteration */
   _s2_dynamic_erase = 1;

   /* we are NOT animating by default */
   _s2_animation = 1;

   /* the space bar (callback key) has not been pressed yet */
   _s2_callbackkey = 0;

   /* no pre-loaded cached textures */
   _s2_ctext_count = 0;
   _s2_ctext = NULL;
   //_s2_ctext_width = _s2_ctext_height = NULL;
   //_s2_ctext_bitmap = NULL;
   //_s2_ctext_id = NULL;

   /* default state is to not record geom, view or image */
   _s2_recstate = 0;
   _s2_recframe = 0;

   /* set skip of loop to 0 - this only changes to 1 by key press */
   _s2_skip = 0;

   /* set things so that camera location etc. will NOT be changed
    * from "HOME" position */
   _s2_cameraset = 0;

   /* enable camera translations */
   _s2_transcam_enable = 1;

   /* focus point */
   _s2_camexfocus = 0;
   _s2_camfocus.x = _s2_camfocus.y = _s2_camfocus.z = 0.0;

   /* camera speed */
   _s2_cameraspeed = 1.0;

   /* eye sep */
     _s2_eyesepmul = 1.0;

     /* buffer swapping: default allowed! */
     _s2_bufswap = 1;

     /* list retention: default yes! */
     _s2_retain_lists = 1;

   /* numeric key press callback is not installed by default */
   _s2_numcb = NULL;

   /* opengl graphics callback is not installed by default */
   _s2_oglcb = NULL;

   /* remote handler */
   _s2_remcb = NULL;
   _s2_remcb_sock = NULL;
   _s2_skiplock = 0;
   _s2_remoteport = 0;
   
   /* handles are not initially visible (toggle with Shift-S key) */
   _s2_handle_vis = 0;

   /* no handle callback function by default */
   _s2_handlecallback = NULL;

   /* no handle currently dragged */
   _s2_draghandle = NULL;

   /* no handle drag callback */
   _s2_draghandle_callback = NULL;

   /* crosshair is not intially visible (toggle with Shift-C) */
   _s2_crosshair_vis = 0;
   _s2_lastmousex = _s2_lastmousey = -1;

   /* clipping is not initially enabled */
   _s2_clipping = 0;
   
   /* set default latex template file name */
   strcpy(_s2_latex_template_file, "s2latex-tmpl.tex");

   /* set default isosurface color */
   _s2priv_colrfn_r = _s2priv_colrfn_g = _s2priv_colrfn_b = 0.3;

   /* no volumes are loaded for vol rendering */
   _s2_nvol = 0;
   _s2_volumes = NULL;
   _s2_vralphascaling = 0;  // no scaling by default
   _s2_evas_x = _s2_evas_y = _s2_evas_z = -1.0;

   /* isosurfaces */
   _s2_nisosurf = 0;
   _s2_isosurfs = NULL;
   _s2_fastsurfaces = 1;

   _device_draw = NULL;
   if (_s2_driver) {
     loadDevices(_s2_driver);
   }
   
   /* fade-in */
   _s2_fadestatus = 0;

   /* fadetime */
   _s2_fadetime = 0.2;
   {
     char *s2fadetime = getenv("S2PLOT_FADETIME");
     if (s2fadetime) {
       _s2_fadetime = atof(s2fadetime);
     }
   }

   /* runtime */
   _s2_runtime = -1.;
   {
     char *s2runtime = getenv("S2PLOT_RUNTIME");
     if (s2runtime) {
       _s2_runtime = atof(s2runtime);
     }
   }

   /* event file */
   _s2_events = NULL;
   _s2_nevents = 0;
   {
     char *s2evfile = getenv("S2PLOT_EVENTFILE");
     if (s2evfile) {
       _s2priv_readEvents(s2evfile);
     }
   }

   /* normal (3d) coordinates */
   //_s2_screenco = 0;
   strcpy(_s2_whichscreen, "");
   _s2_screenEnabled = 0;
   strcpy(_s2_doingScreen, "");

   /* command prompt */
   _s2prompt_length = -1; // not prompting
   _s2_promptcbx = NULL;
   _s2_promptcbx_data = NULL;
   strncpy(_s2baseprompt, "S2PLOT% ", S2BASEPROMPT_LEN);
   _s2baseprompt[S2BASEPROMPT_LEN-1] = '\0';
   _s2prompt_x = 0.05;
   _s2prompt_y = 0.40;

#if defined(BUILDING_S2PLOT)
   /* get screen constraints from environment or default */
   char *panelstr;
   _s2_scr_x1 = 0.;
   panelstr = getenv("S2PLOT_X1");
   if (panelstr) {
     _s2_scr_x1 = atof(panelstr);
   }

   _s2_scr_x2 = 1.;
   panelstr = getenv("S2PLOT_X2");
   if (panelstr) {
     _s2_scr_x2 = atof(panelstr);
   }

   _s2_scr_y1 = 0.;
   panelstr = getenv("S2PLOT_Y1");
   if (panelstr) {
     _s2_scr_y1 = atof(panelstr);
   }

   _s2_scr_y2 = 1.;
   panelstr = getenv("S2PLOT_Y2");
   if (panelstr) {
     _s2_scr_y2 = atof(panelstr);
   }

   _s2_bg_texid = -1;
   panelstr = getenv("S2PLOT_BGIMG");
   if (panelstr) {
     _s2_bg_texid = ss2lt(panelstr);
   }
   
   _s2_bg_clear = 1;
   if (getenv("S2PLOT_BGCLEAR")) {
     _s2_bg_clear = 0;
   }

   /* set up for multiple panel capability */
   /* allocate space for one panel */
#if (1)
   _s2_panels = NULL;
   _s2_npanels = 0;
   xs2ap(_s2_scr_x1, _s2_scr_y1, _s2_scr_x2, _s2_scr_y2);
   xs2cp(0);
#else
   _s2_panels = (S2PLOT_PANEL *)malloc(sizeof(S2PLOT_PANEL));
   _s2_npanels = 1;
   _s2_activepanel = 0;
   /* set the panel location */
   //_s2_panels[0].x1 = _s2_panels[0].y1 = 0.;
   //_s2_panels[0].x2 = _s2_panels[0].y2 = 1.;
   _s2_panels[0].x1 = _s2_scr_x1;
   _s2_panels[0].x2 = _s2_scr_x2;
   _s2_panels[0].y1 = _s2_scr_y1;
   _s2_panels[0].y2 = _s2_scr_y2;
   _s2_panels[0].active = 1;
   _s2_panels[0].camera = (CAMERA *)malloc(sizeof(CAMERA));
   _s2_panels[0].autospin = (XYZ *)malloc(sizeof(XYZ));
   _s2_panels[0].callback = NULL;
   _s2_panels[0].GL_listindex = -1;

   _s2_activepanelframecolour.r = 1.;
   _s2_activepanelframecolour.g = 1.;
   _s2_activepanelframecolour.b = 1.;
   _s2_panelframecolour.r = 0.2;
   _s2_panelframecolour.g = 0.2;
   _s2_panelframecolour.b = 0.2;
   _s2_panelframewidth = 1.0;
#endif

   _s2_nVRMLnames = 1;
   _s2_VRMLnames = (char **)malloc(sizeof(char *));
   _s2_VRMLnames[0] = (char *)malloc(MAXVRMLLEN*sizeof(char));
   strncpy(_s2_VRMLnames[0], "ANON", MAXVRMLLEN);
   _s2_VRMLnames[0][MAXVRMLLEN-1] = '\0';
   _s2_currVRMLidx = 0;
				   

   /* no need to "push" globals onto the zeroth panel yet: only do
    * this when panel is changed by user, or when about to enter
    * drawing section of code. */
#endif

   // initialise character list to process to null
   pthread_mutex_lock(&_kbd_mutex);
   _kbd_chars[0] = '\0';
   pthread_mutex_unlock(&_kbd_mutex);

   _s2_remoteport = 0;
   char *remoteportstr = getenv("S2PLOT_REMOTEPORT");
   if (remoteportstr) {
     _s2_remoteport = atoi(remoteportstr);
     if (_s2_remoteport < 1000) {
       _s2warn("s2open*", "invalid setting of S2PLOT_REMOTEPORT");
       _s2_remoteport = 0;
     }
   }
   if (_s2_remoteport) {
     pthread_t p_thread;
     int a = 1;
     pthread_create(&p_thread, NULL, remote_thread_sub, (void *)&a);
   }

   /* everything seems to be ok */
   return 1;
}

#include "sock.c"
#include <assert.h>
void *remote_thread_sub(void *data) {
  static char ackstr[] = "ack\n";
  char *ack = ackstr;
  char bigquit = 0;
  int sfd;
  int cfd;
  FILE* sockin = 0;
  FILE* sockout = 0;
  char* rgot = NULL;
  char hostname [100];
  int port;
  unsigned bufsize = 4096;

  char* inmsg = (char*) malloc ( bufsize );
  assert (inmsg != 0);

  port = _s2_remoteport;

  if (sock_getname (hostname, 100, 1) < 0) {
    perror ("Error getting hostname.\n");
    return NULL;
  }

  sfd = sock_create (&port);
  if (sfd < 0)  {
    perror ("Error creating socket\n");
    return NULL;
  }

  do  {

    fprintf (stderr, "%s available on %d\n",hostname, port);
    cfd = sock_accept (sfd);
    fprintf (stderr, "Connection accepted.\n");
    
    sockin = fdopen (cfd, "r");
    sockout = fdopen(cfd, "w");

    // set the socket output to be line-buffered
    setvbuf(sockout, 0, _IOLBF, 0);

    while (sockin) {

      rgot = fgets (inmsg, bufsize, sockin);

      if (rgot && !feof(sockin)) {
	//fprintf (stderr, "Received %d bytes: %s\n", (int)strlen(rgot), inmsg);

	int consumed = 0;
	if (_s2_remcb) {
	  consumed = _s2_remcb(rgot);
	}

	if (_s2_remcb_sock) {
      consumed = _s2_remcb_sock(rgot, sockout);
    }

	if (!consumed) {

	  if (strstr(rgot, "_req_gui_strings") == rgot) {
	    fprintf(stderr, "gui strings requested...\n");
	    //ack = as2writestrings();

	  } else if (strstr(rgot, "_set_gui_string") == rgot) {
	    fprintf(stderr, "gui string setting received...\n");
	    //as2readstrings(rgot);

	  } else 


	  if (strlen(rgot) > 1) {
	    if (rgot[0] == 'K') {
	      pthread_mutex_lock(&_kbd_mutex);
	      int tx = strlen((char *)_kbd_chars);
	      int jj = 1;
	      while ((jj < strlen(inmsg)) && 
		     (inmsg[jj] != '\n') && (tx < KBD_CHARS_BUFSIZE-1)) {
		_kbd_chars[tx] = inmsg[jj];
		tx++;
		jj++;
	      }
	      _kbd_chars[tx] = '\0';
	      pthread_mutex_unlock(&_kbd_mutex);
	    } else if (rgot[0] == 'M') {
	      float dx, dy;
	      if (sscanf(rgot+1, "%f %f", &dx, &dy) == 2) {
		MTX_LOCK(&mutex);
		RotateCamera(-dx,dy, 0.0,MOUSECONTROL);
		MTX_ULCK(&mutex);
	      }
	    } else if (rgot[0] == 'F') {
	      float dd;
	      if (sscanf(rgot+1, "%f", &dd) == 1) {
		MTX_LOCK(&mutex);
		FlyCamera(50.0 * dd * ss2qcs());
		MTX_ULCK(&mutex);
	      }
	    } else if (rgot[0] == 'R') {
	      float dd;
	      if (sscanf(rgot+1, "%f", &dd) == 1) {
		MTX_LOCK(&mutex);
		RotateCamera(0.0,0.0,dd,KEYBOARDCONTROL);
		MTX_ULCK(&mutex);
	      }
	    }
	  }
	}

	// send reply
	fprintf(sockout, "%s", ack);

      }

      else {

	if (feof (sockin))
	  fprintf (stderr, "Socket connection terminated.\n");

	else if (!rgot) {
	  if (ferror (sockin))
	    perror ("ferror after fgets");
	  else
	    perror ("fgets");
	}

	fclose (sockin); sockin = NULL;

      }

    }
  }

  while (!bigquit);

  fprintf (stderr, "Server exiting\n");
  sock_close (sfd);
  return 0;
  
  //fprintf(stderr, ">>> Leaving remote_thread_sub.\n");
}


/* open device, using string device specification */
int s2opend(char *idevice, int iargc, char **iargv) {
  /* sanity check */
  if (!idevice) {
    _s2warn("s2opend", "device string invalid");
    return 0;
  }

  char *devstr = idevice;
  char reply[255];
    
  /* do we have a blank device? */
  if (!strcmp(idevice, " ")) {
    /* attempt to fetch environment setting */
    devstr = getenv("S2PLOT_DEV");
    if (!devstr) {
      _s2warn("s2opend", "cannot determine device (check S2PLOT_DEV variable)");
      return 0;
    }
  } else if (!strcmp(idevice, "/?")) {
    /* prompt for device */
    /* default from env. if available */
    char *defstr = getenv("S2PLOT_DEV");
    int retval;
    do {
      if (defstr) {
	fprintf(stdout, "Graphics device type (? to see list, default %s): ",
		defstr);
      } else {
	fprintf(stdout, "Graphics device type (? to see list): ");
      }

      fgets(reply, 255, stdin);
      retval = strlen(reply);

      /* replace the "terminating" CR/NL with null */
      reply[retval-1] = '\0';
      if (strlen(reply) == 0 && defstr) {
	strcpy(reply, defstr);
      } else if (!strcmp(reply, "?")) {
	s2ldev();
      }
    } while (_s2priv_find_device(reply) < 0);
    devstr = reply;
  }

  int deviceid = _s2priv_find_device(devstr);
  if (deviceid < 0) {
    fprintf(stderr, "s2opend: device \"%s\" is unknown.\n", devstr);
    return 0;
  }
  
  int nretval = s2open(_s2_valid_devices[deviceid].fullscreen,
		       _s2_valid_devices[deviceid].stereo, iargc, iargv);
  return nretval;

}

/* open device without argc/argv data */
int s2opendo(char *device) {
  int iargc = 0;
  char *iargv[1];
  iargv[0] = (char *)calloc(strlen("s2plot.exe")+1, sizeof(char));
  strcpy(iargv[0], "s2plot.exe");
  int result = s2opend(device, iargc, iargv);
  free(iargv[0]);
  return result;
}

/* list the devices we know about */
void s2ldev() {
  fprintf(stdout, "Valid interactive S2PLOT devices include:\n");
  int i;
  for (i = 0; i < _s2_ndevices; i++) {
    if (_s2_valid_devices[i].advertise) {
      fprintf(stdout, "%10s (%s)\n", _s2_valid_devices[i].devicename,
	      _s2_valid_devices[i].devicedesc);
    }
  }
  fprintf(stdout, " (*) Full-screen support depends on windowmanager.\n");
  fprintf(stdout, " (*) Case-insensitive, minimum ordered match applies.\n");
}

/* draw the scene and enter interactive mode - never return */
void s2show(int iinteractive) {

  static int beenhere = 0;
  if (beenhere) {
    _s2warn("s2open", "Not executing s2show [been here before!]");
    return;
  }
  beenhere = 1;

  if (options.stereo < 0) {
    _s2warn("s2open", "Not executing s2show [null device]");
    return;
  }

#if defined(BUILDING_S2PLOT)
  int waspanel = _s2_activepanel;
  int spid;
  //for (spid = _s2_npanels-1; spid >= 0; spid--) {
    for (spid = 0; spid < _s2_npanels; spid+=1) {
    if (!_s2_panels[spid].active) {
      continue;
    }

    xs2cp(spid);
#endif

    CameraHome(HOME);
    options.camerahome = camera;
    /* moved to HandleDisplay function
    if (_s2_cameraset) {
      _s2priv_CameraSet();
    }
    */

    MakeGeometry(TRUE, FALSE, 'c');
#if defined(BUILDING_S2PLOT)
  }
    xs2cp(waspanel);
#endif

  if (iinteractive < 0) {
    _s2warn("s2show", "not dislaying (interactive < 0) !");
    return;
  }

  if (!iinteractive) {
    fprintf(stderr, "WARNING!!!  s2show is not re-entrant, and cannot return");
    fprintf(stderr, "\n          control to your main program.\n");
    fprintf(stderr, " - proceeding anyway; consider using s2disp instead.\n");
  }
  
  //fprintf(stderr, "Entering interactive mode.  Shift-ESC or Shift-Q to quit.\n");

  s2winMainLoop();
}

void s2eras() {
  DeleteGeometry();
  /* clear iden label */
  if (_s2_iden) {
    free(_s2_iden);
    _s2_iden = NULL;
  }
  /* clear title label */
  if (_s2_title) {
    free(_s2_title);
    _s2_title = NULL;
  }
  
}

/***********************************************************************
 *
 * PRIMITIVES
 *
 ***********************************************************************
 */

/* draw a circle */
void s2circxy(float ipx, float ipy, float ipz, float ir, int inseg,
	      float iasp) {
  if (inseg < 3) {
    _s2warn("s2circxy", "cannot draw a 2-sided circle");
    return;
  }
  float *xpts, *ypts, *zpts;
  xpts = (float *)calloc(inseg + 1, sizeof(float));
  ypts = (float *)calloc(inseg + 1, sizeof(float));
  zpts = (float *)calloc(inseg + 1, sizeof(float));

  // calculate the "radii" in x and y
  float radx, rady;
  if (iasp < 0) {
    /* apparent circle */
    float rad = sqrt(0.5 * (powf(_S2WORLD2DEVICE_SO(ir, _S2XAX), 2.0) +
			    powf(_S2WORLD2DEVICE_SO(ir, _S2YAX), 2.0)));
    radx = rad / _S2WORLD2DEVICE_SO(1.0, _S2XAX);
    rady = rad / _S2WORLD2DEVICE_SO(1.0, _S2YAX);
  } else {
    radx = ir;
    rady = ir * iasp;
  }

  float theta;
  int i;
  for (i = 0; i < inseg; i++) {
    theta = TWOPI * (float)i / (float)(inseg);
    xpts[i] = ipx + radx * cos(theta);
    ypts[i] = ipy + rady * sin(theta);
    zpts[i] = ipz;
  }
  xpts[inseg] = xpts[0];
  ypts[inseg] = ypts[0];
  zpts[inseg] = zpts[0];
  s2line(inseg+1, xpts, ypts, zpts);
  return;
}
void s2circxz(float ipx, float ipy, float ipz, float ir, int inseg,
	      float iasp) {
  if (inseg < 3) {
    _s2warn("s2circxz", "cannot draw a 2-sided circle");
    return;
  }
  float *xpts, *ypts, *zpts;
  xpts = (float *)calloc(inseg + 1, sizeof(float));
  ypts = (float *)calloc(inseg + 1, sizeof(float));
  zpts = (float *)calloc(inseg + 1, sizeof(float));

  // calculate the "radii" in x and z
  float radx, radz;
  if (iasp < 0) {
    /* apparent circle */
    float rad = sqrt(0.5 * (powf(_S2WORLD2DEVICE_SO(ir, _S2XAX), 2.0) +
			    powf(_S2WORLD2DEVICE_SO(ir, _S2ZAX), 2.0)));
    radx = rad / _S2WORLD2DEVICE_SO(1.0, _S2XAX);
    radz = rad / _S2WORLD2DEVICE_SO(1.0, _S2ZAX);
  } else {
    radx = ir;
    radz = ir * iasp;
  }

  float theta;
  int i;
  for (i = 0; i < inseg; i++) {
    theta = TWOPI * (float)i / (float)(inseg);
    xpts[i] = ipx + radx * cos(theta);
    ypts[i] = ipy;
    zpts[i] = ipz + radz * sin(theta);
  }
  xpts[inseg] = xpts[0];
  ypts[inseg] = ypts[0];
  zpts[inseg] = zpts[0];
  s2line(inseg+1, xpts, ypts, zpts);
  return;
}
void s2circyz(float ipx, float ipy, float ipz, float ir, int inseg,
	      float iasp) {
  if (inseg < 3) {
    _s2warn("s2circyz", "cannot draw a 2-sided circle");
    return;
  }
  float *xpts, *ypts, *zpts;
  xpts = (float *)calloc(inseg + 1, sizeof(float));
  ypts = (float *)calloc(inseg + 1, sizeof(float));
  zpts = (float *)calloc(inseg + 1, sizeof(float));

  // calculate the "radii" in x and y
  float rady, radz;
  if (iasp < 0) {
    /* apparent circle */
    float rad = sqrt(0.5 * (powf(_S2WORLD2DEVICE_SO(ir, _S2YAX), 2.0) +
			    powf(_S2WORLD2DEVICE_SO(ir, _S2ZAX), 2.0)));
    rady = rad / _S2WORLD2DEVICE_SO(1.0, _S2YAX);
    radz = rad / _S2WORLD2DEVICE_SO(1.0, _S2ZAX);
  } else {
    rady = ir;
    radz = ir * iasp;
  }

  float theta;
  int i;
  for (i = 0; i < inseg; i++) {
    theta = TWOPI * (float)i / (float)(inseg);
    xpts[i] = ipx;
    ypts[i] = ipy + rady * cos(theta);
    zpts[i] = ipz + radz * sin(theta);
  }
  xpts[inseg] = xpts[0];
  ypts[inseg] = ypts[0];
  zpts[inseg] = zpts[0];
  s2line(inseg+1, xpts, ypts, zpts);
  return;
}


/* draw a (filled) rectangle */
void s2rectxy(float ixmin, float ixmax, float iymin, float iymax, float iz) {
  XYZ p[4];
  XYZ origin = {0.0,0.0,0.0};
  float wxmin = _S2WORLD2DEVICE(ixmin, _S2XAX);
  float wxmax = _S2WORLD2DEVICE(ixmax, _S2XAX);
  float wymin = _S2WORLD2DEVICE(iymin, _S2YAX);
  float wymax = _S2WORLD2DEVICE(iymax, _S2YAX);
  float wz = _S2WORLD2DEVICE(iz, _S2ZAX);
  p[0].x = wxmin;  p[0].y = wymin;  p[0].z = wz;
  p[1].x = wxmin;  p[1].y = wymax;  p[1].z = wz;
  p[2].x = wxmax;  p[2].y = wymax;  p[2].z = wz;
  p[3].x = wxmax;  p[3].y = wymin;  p[3].z = wz;
  AddFace2Database(p, 4, _s2_colormap[_s2_colidx], 1.0, origin);
  return;
}
void s2rectxz(float ixmin, float ixmax, float izmin, float izmax, float iy) {
  XYZ p[4];
  XYZ origin = {0.0,0.0,0.0};
  float wzmin = _S2WORLD2DEVICE(izmin, _S2ZAX);
  float wzmax = _S2WORLD2DEVICE(izmax, _S2ZAX);
  float wxmin = _S2WORLD2DEVICE(ixmin, _S2XAX);
  float wxmax = _S2WORLD2DEVICE(ixmax, _S2XAX);
  float wy = _S2WORLD2DEVICE(iy, _S2YAX);
  p[0].x = wxmin;  p[0].y = wy;  p[0].z = wzmin;
  p[1].x = wxmax;  p[1].y = wy;  p[1].z = wzmin;
  p[2].x = wxmax;  p[2].y = wy;  p[2].z = wzmax;
  p[3].x = wxmin;  p[3].y = wy;  p[3].z = wzmax;
  AddFace2Database(p, 4, _s2_colormap[_s2_colidx], 1.0, origin);
  return;
}
void s2rectyz(float iymin, float iymax, float izmin, float izmax, float ix) {
  XYZ p[4];
  XYZ origin = {0.0,0.0,0.0};
  float wymin = _S2WORLD2DEVICE(iymin, _S2YAX);
  float wymax = _S2WORLD2DEVICE(iymax, _S2YAX);
  float wzmin = _S2WORLD2DEVICE(izmin, _S2ZAX);
  float wzmax = _S2WORLD2DEVICE(izmax, _S2ZAX);
  float wx = _S2WORLD2DEVICE(ix, _S2XAX);
  p[0].x = wx;  p[0].y = wymin;  p[0].z = wzmin;
  p[1].x = wx;  p[1].y = wymin;  p[1].z = wzmax;
  p[2].x = wx;  p[2].y = wymax;  p[2].z = wzmax;
  p[3].x = wx;  p[3].y = wymax;  p[3].z = wzmin;
  AddFace2Database(p, 4, _s2_colormap[_s2_colidx], 1.0, origin);
  return;
}

/* draw a wireframe cube */
void s2wcube(float ixmin, float ixmax,
	     float iymin, float iymax,
	     float izmin, float izmax) {
  ns2thwcube(ixmin, iymin, izmin, ixmax, iymax, izmax,
	     _S2PENRED, _S2PENGRN, _S2PENBLU, _s2_linewidth);
}


/* draw a set of points with different symbols */
void s2pnts(int inp, float *xpts, float *ypts, float *zpts,
	    int *isymbols, int ins) {
  // sanity check
  if (!xpts || !ypts || !zpts || !isymbols) {
    _s2warn("s2pnts", "invalid function arguments [NULL pointer(s)]");
    return;
  }

  int i;
  int sym;
  for (i = 0; i < inp; i++) {
    if (i < ins) {
      sym = isymbols[i];
    } else {
      sym = isymbols[0];
    }
    s2pt1(xpts[i], ypts[i], zpts[i], sym);
  }
  return;
}



  /* Set the custom help string.  This is shown on the 2nd press of the 
   * F1 key for S2PLOT programs, if it is set.
   */
void s2help(char *helpstr) {
  if (_s2_customhelpstr) {
    free(_s2_customhelpstr);
    _s2_customhelpstr = NULL;
  }
  if (helpstr) {
    _s2_customhelpstr = (char *)malloc(strlen(helpstr) * sizeof(char));
    strcpy(_s2_customhelpstr, helpstr);
  }
}




/***********************************************************************
 *
 * VECTOR PLOTS
 *
 ***********************************************************************
 */

/* draw 3d vector "map" */
void s2vect3(float ***ia, float ***ib, float ***ic,
	     int iadim, int ibdim, int icdim, 
	     int ia1, int ia2, int ib1, int ib2, int ic1, int ic2,
	     float iscale, int inc,
	     float *itr, float iminlength,
	     int icolbylength, float iminl, float imaxl) {

  if ((ia2 <= ia1) || (ia1 < 0) || (ia2 >= iadim) ||
      (ib2 <= ib1) || (ib1 < 0) || (ib2 >= ibdim) ||
      (ic2 <= ic1) || (ic1 < 0) || (ic2 >= icdim)) {
    _s2warn("s2vect3", "invalid data dimensions or slice");
    return;
  }
  
  if (!ia || !ib || !ic) {
    _s2warn("s2vect3", "invalid function arguments [NULL pointer(s)]");
    return;
  }

  int preserve_col_idx = _s2_colidx;

  int i, j, k;
  float px, py, pz; /* transformed position of array index */
  float vx, vy, vz; /* vector */
  float x1, y1, z1, x2, y2, z2; /* start + end points to plot */
  float length;
  for (i = ia1; i <= ia2; i++) {
    for (j = ib1; j <= ib2; j++) {
      for (k = ic1; k <= ic2; k++) {
	/* position to plot vector */
	px = itr[0] + itr[1]*(float)i + itr[2]*(float)j + itr[3]*(float)k;
	py = itr[4] + itr[5]*(float)i + itr[6]*(float)j + itr[7]*(float)k;
	pz = itr[8] + itr[9]*(float)i + itr[10]*(float)j + itr[11]*(float)k;
	/* vector itself */
	vx = ia[i][j][k] * iscale;
	vy = ib[i][j][k] * iscale;
	vz = ic[i][j][k] * iscale;
	/* check length of vector */
	length = sqrt(vx*vx + vy*vy + vz*vz) / (iscale);
	if (length < iminlength) {
	  continue;
	}
	/* based on "inc", select endpoints for vector */
	if (inc < 0) {
	  /* head of vector on coords */
	  x2 = px;
	  y2 = py;
	  z2 = pz;
	  x1 = px - vx;
	  y1 = py - vy;
	  z1 = pz - vz;
	} else if (inc > 0) {
	  /* base of vector on coords */
	  x1 = px;
	  y1 = py;
	  z1 = pz;
	  x2 = px + vx;
	  y2 = py + vy;
	  z2 = pz + vz;
	} else {
	  /* centre vector on coords */
	  x1 = px - 0.5 * vx;
	  y1 = py - 0.5 * vy;
	  z1 = pz - 0.5 * vz;
	  x2 = px + 0.5 * vx;
	  y2 = py + 0.5 * vy;
	  z2 = pz + 0.5 * vz;
	}
	if (icolbylength) {
	  length = MAX(0.0, MIN(1.0, (length - iminl) / (imaxl - iminl)));
	  s2sci(MAX(_s2_colr1, MIN(_s2_colr2, _s2_colr1 + length * (float)(_s2_colr2-_s2_colr1))));
	}

	s2arro(x1, y1, z1, x2, y2, z2);
      }
    }
  }

  _s2_colidx = preserve_col_idx;
  return;
}




/***********************************************************************
 *
 * MISCELLANEOUS ROUTINES
 *
 ***********************************************************************
 */

#include <sys/types.h>
#include <pwd.h>
/* record identity, date, time for later use in plotting */
void s2iden(char *itextra) {
  if (_s2_iden) {
    free(_s2_iden);
  }
  
  struct passwd *passwdstr = getpwuid(getuid());
  char *username_str;
  if (passwdstr) {
    username_str = passwdstr->pw_name;
  } else {
    username_str = (char *)calloc(strlen(_S2NULLIDSTR)+1, sizeof(char));
    strcpy(username_str, _S2NULLIDSTR);
  }
  time_t tmptime = time(NULL);
  char *timedate_str = ctime(&tmptime);
  /* chop off \n */
  timedate_str[strlen(timedate_str) - 1] = '\0';

  int idenlen = strlen(_S2IDENPREFIX) + strlen(username_str) +
    strlen(_S2IDENMIDFIX) + strlen(timedate_str) + 
    strlen(itextra) + 1;
  _s2_iden = (char *)calloc(idenlen, sizeof(char));
  sprintf(_s2_iden, "%s%s%s%s%s", _S2IDENPREFIX, username_str,
	  _S2IDENMIDFIX, timedate_str, itextra);

  if (!strcmp(username_str, _S2NULLIDSTR)) {
    free(username_str);
  }
  /* no need to free passwdstr - it was not alloc'ed by malloc */
}


/***********************************************************************
 *
 * "NATIVE" S2 FUNCTIONS
 *
 ***********************************************************************
 */


// XXX XXX XXX
// screen coords done to here
// XXX XXX XXX


/***********************************************************************
 *
 * CALLBACK SYSTEM
 *
 ***********************************************************************
 */
/* set the dynamic callback function - this one takes a void data ptr
 * that the user can use arbitrarily.
 */
void cs2scbx(void *icbfnx, void *data) {
  _s2_callbackx = (void (*)(double *, int *, void *))icbfnx;
  _s2_callbackx_data = data;
}
void *cs2qcbx(void) {
  return (void *)_s2_callback;
}

/* enable/disable the callback function */
void cs2ecb() {
  _s2_animation = 1;
}

void cs2dcb() {
  _s2_animation = 0;
}

void cs2tcb() {
  _s2_animation = !_s2_animation;
}

/* set the user-installed keybard callback function */
void cs2skcb(void *icbfn) {
  _s2_user_keys = (int (*)(unsigned char *))icbfn;
}
void *cs2qkcb(void) {
  return (void *)_s2_user_keys;
}

/* set the number-key callback function */
void cs2sncb(void *icbfn) {
  _s2_numcb = (void (*)(int *))icbfn;
}
void *cs2qncb(void) {
  return (void *)_s2_numcb;
}

#if (0) // moved to s2plot.c
/* register the function that draws direct OpenGL graphics when called */
void cs2socb(void *icbfn) {
  _s2_oglcb = (void (*)(int *))icbfn;
}
void *cs2qocb(void) {
  return (void *)_s2_oglcb;
}
#endif

/* register the function that handles remote control commands */
void cs2srcb(void *rcbfn) {
  _s2_remcb = (int (*)(char *))rcbfn;
}
void *cs2qrcb(void) {
  return (void *)_s2_remcb;
}
void cs2srcb_sock(void *rcbfn) {
  _s2_remcb_sock = (int (*)(char *, FILE *))rcbfn;
}


/* set the handle callback function */
void cs2shcb(void *icbfn) {
  _s2_handlecallback = (void (*)(int *))icbfn;
}
void *cs2qhcb(void) {
  return (void *)_s2_handlecallback;
}

/* toggle the state of an identified dynamic handle */
void cs2th(unsigned int id) {
  int j;
  for (j = 0; j < nhandle_d; j++) {
    if (handle_d[j].id == id) {
      handle_d[j].selected = (handle_d[j].selected + 1) % 2;
      break;
    }
  }
}

/***********************************************************************
 *
 * ADVANCED TEXTURE HANDLING
 *
 ***********************************************************************
 */

#if defined(S2_NO_S2GEOMVIEWER)
/* load a texture */
unsigned int ss2lt(char *itexturefn) {
  int width, height;
  BITMAP4 *bitmap = NULL;
  //GLuint id;

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
	long idx = 0;
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

      if (options.stereo < 0) {
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
#endif

/* load a colourmap into mem, starting at index startidx */
int ss2lcm(char *imapfile, int startidx, int maxn) {
  if (startidx < 0 || maxn < 1 || !imapfile) {
    _s2warn("ss2lcm", "invalid arguments");
    return -1;
  }

  // 1. if filename starts with "." or "/" then attempt to open 
  //    precisely this file.  No other attempts will be made.
#if !defined(S2SUNOS)
  struct stat filestat;
#else
  int filestat;
#endif
  FILE *fp = NULL;
  if (!strncmp(imapfile, ".", 1) || !strncmp(imapfile, "/", 1)) {
    if (!stat(imapfile, &filestat)) {
      fp = fopen(imapfile, "r");
    }
  } else {
    // 2. in order, try to load named map from current dir, then from
    //    S2PLOT_TEXPATH (if defined), then from S2PATH/textures
    //    (if defined).
    char cwd[200], mapname[200];
    if (getcwd(cwd, 200)) {
      sprintf(mapname, "%s/%s", cwd, imapfile);
      if (!stat(mapname, &filestat)) {
	fp = fopen(mapname, "r");
      }
    }
    if (!fp) {
      char *s2texpath = getenv("S2PLOT_TEXPATH");
      if (s2texpath) {
	sprintf(mapname, "%s/%s", s2texpath, imapfile);
	if (!stat(mapname, &filestat)) {
	  fp = fopen(mapname, "r");
	}
      }
    }
    if (!fp) {
      char *s2path = getenv("S2PATH");
      if (s2path) {
	sprintf(mapname, "%s/textures/%s", s2path, imapfile);
	if (!stat(mapname, &filestat)) {
	  fp = fopen(mapname, "r");
	}
      }
    }
  }

  int idx = startidx;
  if (fp) {
    int maxidx = startidx + maxn;
    char string[255];
    while ((idx < maxidx) && !feof(fp) && fgets(string, 254, fp)) {
      if (string[0] == '#') {
	continue;
      } else {
	int ignore, r, g, b;
	if (sscanf(string, "%d %d %d %d", &ignore, &r, &g, &b)) {
	  s2scr(idx, (float)r / 255., (float)g / 255., (float)b / 255.);
	  idx++;
	}
      }
    }
    fclose(fp);
  } else {
    fprintf(stderr, "Advisory: unable to load map %s, using RGB triplet\n",
	    imapfile);
    s2scr(idx, 1., 0., 0.);
    s2scr(++idx, 0., 1., 0.);
    s2scr(++idx, 0., 0., 1.);
  }

  return idx - startidx;
}


/***********************************************************************
 *
 * ADDITIONAL PRIMITIVES
 *
 * NOTE: these use non-standard interfaces at this point, hence the
 *       segregation in this section.
 *
 ***********************************************************************
 */

/* chromastereoscopic plot of points */
void s2chromapts(int inp, float *ilong, float *ilat, float *idist,
		 float *isize, float iradius, float idmin, float idmax) {
  if (inp < 1) {
    _s2warn("s2chromapts", "need at least one point");
    return;
  }
  if (!ilong || !ilat || !idist || !isize ) {
    _s2warn("s2chromapts", "invalid function argument(s) [NULL pointer(s)]");
    return;
  }

  int idx1 = _s2_colr1;
  int idx2 = _s2_colr2;
  int i, idx;
  float x, y, z;
  float r, g, b;
  for (i = 0; i < inp; i++) {
    idx = idx1 + (int)((idist[i] - idmin) / (idmax - idmin) * 
		       (float)(idx2 - idx1) + 0.5);
    idx = (idx < idx1) ? idx1 : ((idx > idx2) ? idx2 : idx);
    s2qcr(idx, &r, &g, &b);
    x = iradius * cos(ilat[i] * DTOR) * cos(ilong[i] * DTOR);
    y = iradius * cos(ilat[i] * DTOR) * sin(ilong[i] * DTOR);
    z = iradius * sin(ilat[i] * DTOR);
    ns2thpoint(x, y, z, r, g, b, isize[i]);
  }
}

/* cartesian chroma pts */
void s2chromacpts(int inp, float *ix, float *iy, float *iz, 
		  float *idist, float *isize, float idmin, float idmax) {
  if (inp < 1) {
    _s2warn("s2chromacpts", "need at least one point");
    return;
  }
  if (!ix || !iy || !iz || !idist || !isize) {
    _s2warn("s2chromacpts", "invalid function argument(s) [NULL pointer(s)]");
    return;
  }

  int idx1 = _s2_colr1;
  int idx2 = _s2_colr2;
  int i, idx;
  float r, g, b;
  for (i = 0; i < inp; i++) {
    idx = idx1 + (int)((idist[i] - idmin) / (idmax - idmin) * 
		       (float)(idx2 - idx1) + 0.5);
    idx = (idx < idx1) ? idx1 : ((idx > idx2) ? idx2 : idx);
    s2qcr(idx, &r, &g, &b);
    ns2thpoint(ix[i], iy[i], iz[i], r, g, b, isize[i]);
  }
}



/***********************************************************************
 *
 * FUNCTIONS IN TESTING / DEVELOPMENT
 *
 * NOTE: please implement FORTRAN stubs prior to moving functions out of 
 *       this category.
 *
 ***********************************************************************
 */



/* create a texture with LaTeX commands. */
unsigned int s2latexture(char *latexcmd, float *aspect) {

  char latexbin[200], dvipngbin[200];
  if (getenv("S2PLOT_LATEXBIN")) {
    strcpy(latexbin, getenv("S2PLOT_LATEXBIN"));
  } else {
    strcpy(latexbin, "/usr/bin/latex");
  }
  if (getenv("S2PLOT_DVIPNGBIN")) {
    strcpy(dvipngbin, getenv("S2PLOT_DVIPNGBIN"));
  } else {
    strcpy(dvipngbin, "/usr/bin/dvipng");
  }
  
  /* 1. make sure we can find the LaTeX template file ... */
#if !defined(S2SUNOS)
  struct stat filestat;
#else
  int filestat;
#endif
  FILE *templin = NULL;
  // a. does template file name start with . or / ?
  if (!strncmp(_s2_latex_template_file, ".", 1) ||
      !strncmp(_s2_latex_template_file, "/", 1)) {
    if (!stat(_s2_latex_template_file, &filestat)) {
      templin = fopen(_s2_latex_template_file, "r");
    }
  } else {
    // 2. in order, try current dir, then S2PLOT_TEXPATH dir, then
    //    from S2PATH/textures...
    char cwd[200], texname[200];
    if (getcwd(cwd, 200)) {
      sprintf(texname, "%s/%s", cwd, _s2_latex_template_file);
      if (!stat(texname, &filestat)) {
	templin = fopen(texname, "r");
      }
    }
    if (!templin) {
      char *s2texpath = getenv("S2PLOT_TEXPATH");
      if (s2texpath) {
	sprintf(texname, "%s/%s", s2texpath, _s2_latex_template_file);
	if (!stat(texname, &filestat)) {
	  templin = fopen(texname, "r");
	}
      }
    }
    if (!templin) {
      char *s2path = getenv("S2PATH");
      if (s2path) {
	sprintf(texname, "%s/textures/%s", s2path, _s2_latex_template_file);
	if (!stat(texname, &filestat)) {
	  templin = fopen(texname, "r");
	}
      }
    }
  }

  if (!templin) {
    _s2warn("s2latexture", "unable to open LaTeX template file");
    if (aspect) {
      *aspect = 1.;
    }
    return s2loadtexture("<latex-failed>");
  }

  /* generate temporary directory */
  char tempbase[] = "/tmp/s2latex-XXXXXX";
  char latexfn[255];
#if !defined(S2SUNOS)
  mkdtemp(tempbase);
#else
  {
    char cmdcmd[200];
    char *tmptmp = tempnam("/tmp", "s2latex-");
    strcpy(tempbase, tmptmp);
    sprintf(cmdcmd, "mkdir -p %s", tempbase);
    free(tmptmp);
  }
#endif
    
  
  /* create latex file */
  sprintf(latexfn, "%s/s2plot.tex", tempbase);
  FILE *latexf = fopen(latexfn, "w");
  char string1[255];
  while (!feof(templin) && fgets(string1, 255, templin)) {
    if (!strncmp(string1, "S2PLOT", 6)) {
      fprintf(latexf, "%s\n", latexcmd);
    } else {
      fprintf(latexf, "%s", string1);
    }
  }
  fclose(templin);
  fclose(latexf);

  /* latex, convert, turn into a texture ... */
  char command[500];
  sprintf(command, 
	  "cd %s && %s -interaction=batchmode s2plot.tex 2>&1 > /dev/null",
	  tempbase, latexbin);
  if (system(command)) {
    _s2warn("s2latexture", "LaTeX failed");
    if (aspect) {
      *aspect = 1.;
    }
    return s2loadtexture("<latex-failed>");
  }
  sprintf(command, 
	  "cd %s && %s -D 300 -T tight s2plot.dvi 2>&1 > /dev/null", 
	  tempbase, dvipngbin);
  if (system(command)) {
    _s2warn("s2latexture", "DVI -> PNG conversion failed");
    if (aspect) {
      *aspect = 1.;
    }
    return s2loadtexture("<dvipng-failed>");
  }
  sprintf(command, 
	  "cd %s && %s/texturise1.csh -scale+ s2plot1.png s2plot1.tga 2>&1 > texres.txt",
	  tempbase, getenv("S2PATH"));
  if (system(command)) {
    _s2warn("s2latexture", "conversion of png to texture failed");
    if (aspect) {
      *aspect = 1.;
    }
    return s2loadtexture("<texturise1-failed>");
  }
  sprintf(command, "%s/texres.txt", tempbase);
  FILE *tmp = fopen(command, "r");
  int texx = 0, texy = 0;
  if (fgets(command, 255, tmp)) {
    sscanf(command, "%d %d", &texx, &texy);
  } else {
    _s2warn("s2latexture", "cannot obtain texture information");
    if (aspect) {
      *aspect = 1.;
    }
    return s2loadtexture("<dvipng-failed>");
  }
  if (aspect) {
    *aspect = (float)texx / (float)texy;
  }

  sprintf(command, "%s/s2plot1.tga", tempbase);
  unsigned int texnum = ss2lt(command);

  sprintf(command, "rm -rf %s", tempbase);
  system(command);
  
  return texnum;
}


void ds2dvrXXX(int vrid, int force, int axis) {
  if (!_s2_dynamicEnabled) {
    _s2warn("ds2dvrXXX", "called from non-dynamic mode");
    return;
  }
  if (vrid >= _s2_nvol) {
    _s2warn("ds2dvrXXX", "invalid volume rendering object (vrid)");
    return;
  }
 
  // 2. draw textures
  _S2VRVOLUME *it = _s2_volumes + vrid;

  int axloop;
  axloop = axis; {
  //for (axloop = 1; axloop <= 3; axloop++) {
    _s2priv_load_vr_textures(vrid, force, axloop);


    int pl2, plt;
    //float pl;
    XYZ p[4];
    COLOUR col = {1., 1., 1.};
    
    int idx;
    float ic_a, ic_b, ic_c;
    
    float dia, dib, dic;
    dia = dib = dic = 0.5;
    
    switch (it->axis) {
      
    case 1:
      _s2debug("ds2dvrXXX", "drawing textures for X-view");
      
      for (pl2 = 0; pl2 < (it->a2 - it->a1 + 1); pl2++) {
	if (it->reverse) {
	  plt = it->a2 - pl2;
	  //pl = it->a2 - 0.5 - 
	  // (float)pl2 * (float)(it->a2 - it->a1 - 1) / (float)(it->a2 - it->a1);
	} else {
	  plt = it->a1 + pl2;
	  //pl = it->a1 + 0.5 + 
	  // (float)pl2 * (float)(it->a2 - it->a1 - 1) / (float)(it->a2 - it->a1);
	}
	
	idx = 0;
	ic_a = plt;
	ic_b = it->b2 + dib;
	ic_c = it->c1 - dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 1;
	ic_a = plt;
	ic_b = it->b2 + dib;
	ic_c = it->c2 + dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 2;
	ic_a = plt;
	ic_b = it->b1 - dib;
	ic_c = it->c2 + dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 3;
	ic_a = plt;
	ic_b = it->b1 - dib;
	ic_c = it->c1 - dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	ns2vf4xt(p, col, it->textureids[plt - it->a1], 1., it->trans, 0.6);
      }
      break;
      
    case 2:
      _s2debug("ds2dvrXXX", "drawing textures for Y-view");
      
      for (pl2 = 0; pl2 < (it->b2 - it->b1 + 1); pl2++) {
	if (it->reverse) {
	  plt = it->b2 - pl2;
	  //pl = it->b2 - 0.5 - 
	  // (float)pl2 * (float)(it->b2 - it->b1 - 1) / (float)(it->b2 - it->b1);
	} else {
	  plt = it->b1 + pl2;
	  //pl = it->b1 + 0.5 +
	  // (float)pl2 * (float)(it->b2 - it->b1 - 1) / (float)(it->b2 - it->b1);
	}
	
	idx = 0;
	ic_a = it->a1 - dia;
	ic_b = plt;
	ic_c = it->c2 + dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 1;
	ic_a = it->a2 + dia;
	ic_b = plt;
	ic_c = it->c2 + dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 2;
	ic_a = it->a2 + dia;
	ic_b = plt;
	ic_c = it->c1 - dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 3;
	ic_a = it->a1 - dia;
	ic_b = plt;
	ic_c = it->c1 - dic;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	ns2vf4xt(p, col, it->textureids[plt - it->b1], 1., it->trans, 0.6);
      }
      break;
      
    case 3:
      _s2debug("ds2dvrXXX", "drawing textures for Z-view");

      
      for (pl2 = 0; pl2 < (it->c2 - it->c1 + 1); pl2++) {
	if (it->reverse) {
	  plt = it->c2 - pl2;
	  //pl = it->c2 - 0.5 - 
	  // (float)pl2 * (float)(it->c2 - it->c1 - 1) / (float)(it->c2 - it->c1);
	} else {
	  plt = it->c1 + pl2;
	  //pl = it->c1 + 0.5 +
	  // (float)pl2 * (float)(it->c2 - it->c1 - 1) / (float)(it->c2 - it->c1);
	}
	
	idx = 0;
	ic_a = it->a1 - dia;
	ic_b = it->b2 + dib;
	ic_c = plt;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 1;
	ic_a = it->a2 + dia;
	ic_b = it->b2 + dib;
	ic_c = plt;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 2;
	ic_a = it->a2 + dia;
	ic_b = it->b1 - dib;
	ic_c = plt;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	idx = 3;
	ic_a = it->a1 - dia;
	ic_b = it->b1 - dib;
	ic_c = plt;
	p[idx].x = it->tr[0] + it->tr[1] * ic_a + it->tr[2] * ic_b +
	  it->tr[3] * ic_c;
	p[idx].y = it->tr[4] + it->tr[5] * ic_a + it->tr[6] * ic_b +
	  it->tr[7] * ic_c;
	p[idx].z = it->tr[8] + it->tr[9] * ic_a + it->tr[10] * ic_b +
	  it->tr[11] * ic_c;
	
	ns2vf4xt(p, col, it->textureids[plt - it->c1], 1., it->trans, 0.6);
      }
      break;
      
      
    } // case (it->axis)
    
  } // axloop

}

unsigned int ss2ctt(int width, int height) {
  BITMAP4 *bitmap = _s2priv_redXtexture(width, height);
  // no mipmaps
  return _s2priv_setupTexture(width, height, bitmap, 0);
}

/* toggle or set crosshair mode: only on certain devices */
void ss2txh(int enabledisable) {
  if (!(_s2_devcap & _S2DEVCAP_CROSSHAIR)) {
    _s2warn("ss2txh", "crosshair not available on this device");
    return;
  }
  switch (enabledisable) {
  case -1:
    _s2_crosshair_vis = (_s2_crosshair_vis + 1) % 2;
    break;
  case 0: 
    _s2_crosshair_vis = 0;
    break;
  case 1:
    _s2_crosshair_vis = 1;
    break;
  default:
    _s2warn("ss2txh", "invalid argument 'enabledisable'");
    return;
  }
  if (_s2_devcap & _S2DEVCAP_CURSOR) {
    if (_s2_crosshair_vis) {
      s2winSetCursor(S2_CURSOR_NONE);
    } else {
      s2winSetCursor(S2_CURSOR_CROSSHAIR);
    }
  }
}

/* query crosshair visibility */
int ss2qxh(void) {
  return _s2_crosshair_vis;
}


void _s2priv_fullTrans(XYZ *pxyz, int n, float *tr) {
  /* apply a full transformation to the vertices */
  float ttx, tty, ttz;
  int i;
  for (i = 0; i < n; i++) {
    ttx = pxyz[i].x;
    tty = pxyz[i].y;
    ttz = pxyz[i].z;
    pxyz[i].x = tr[0] + tr[1] * ttx + tr[2] * tty + tr[3] * ttz;
    pxyz[i].y = tr[4] + tr[5] * ttx + tr[6] * tty + tr[7] * ttz;
    pxyz[i].z = tr[8] + tr[9] * ttx + tr[10]* tty + tr[11]* ttz;
  }
}

/* "skyscraper" plot with arbitrary rotation / skew / translation */
void s2skypa(float **data, int nx, int ny, 
	     int i1, int i2, int j1, int j2,
	     float datamin, float datamax, 
	     float *tr, int walls, int idx_left, int idx_front) {
  
  if ((i2 <= i1) || (i1 < 0) || (i1 >= nx) || (i2 < 0) || (i2 >= nx) ||
      (j2 <= j1) || (j1 < 0) || (j1 >= ny) || (j2 < 0) || (j2 >= ny) ||
      (datamax <= datamin)) {
    _s2warn("s2skypa", "invalid data dimension or slice");
    return;
  }

  if (_s2_cmapsize < 1) {
    _s2warn("s2skypa", "colormap too small or not set");
    return;
  }

  if (!data || !tr) {
    _s2warn("s2skypa", "invalid function args (NULL ptr)");
    return;
  }
  
  int i, j;
  
  float left, right, top, bottom; /* corners of building top */
  float height; /* height of buiding */
  float otherheight; /* height of next-door building */
  XYZ Pface[4]; /* building face */
  int colidx;

  COLOUR leftcol, frontcol;
  leftcol = _s2_colormap[idx_left];
  frontcol = _s2_colormap[idx_front];

  for (i = i1; i <= i2; i++) {
    left = (float)i - 0.5;
    right = (float)i + 0.5;

    for (j = j1; j <= j2; j++) {
      bottom = (float)j - 0.5;
      top = (float)j + 0.5;
      height = data[i][j];


      // 1. top of building
      Pface[0].x = left;
      Pface[0].y = bottom;

      Pface[1].x = left;
      Pface[1].y = top;
      
      Pface[2].x = right;
      Pface[2].y = top;
      
      Pface[3].x = right;
      Pface[3].y = bottom;

      Pface[0].z = Pface[1].z = Pface[2].z = Pface[3].z = height;

      /* transform Pface... */
      _s2priv_fullTrans(Pface, 4, tr);
      
      colidx = MAX(_s2_colr1, MIN(_s2_colr2, _s2_colr1 + (height - datamin) / 
				  (datamax - datamin) * 
				  (float)(_s2_colr2 - _s2_colr1 + 1)));
      ns2Vf4(Pface, _s2_colormap[colidx]);

      if (walls) {

	if (idx_left < 0 || idx_front < 0) {
	  leftcol = _s2_colormap[colidx];
	  leftcol.r *= 0.7;
	  leftcol.g *= 0.7;
	  leftcol.b *= 0.7;
	  
	  frontcol = _s2_colormap[colidx];
	  frontcol.r *= 0.4;
	  frontcol.g *= 0.4;
	  frontcol.b *= 0.4;
	}

	// 2. left side of building
	if (i > i1) {
	  otherheight = data[i-1][j];
	} else {
	  otherheight = datamin;
	}
	Pface[0].x = Pface[1].x = Pface[2].x = Pface[3].x = left;
	
	Pface[0].y = bottom;
	Pface[0].z = otherheight;
	
	Pface[1].y = top;
	Pface[1].z = otherheight;
	
	Pface[2].y = top;
	Pface[2].z = height;
	
	Pface[3].y = bottom;
	Pface[3].z = height;
	
	_s2priv_fullTrans(Pface, 4, tr);
	//ns2Vf4(Pface, _s2_colormap[idx_left]);
	ns2Vf4(Pface, leftcol);
	
	// 3. bottom side of building
	if (j > j1) {
	  otherheight = data[i][j-1];
	} else {
	  otherheight = datamin;
	}
	Pface[0].y = Pface[1].y = Pface[2].y = Pface[3].y = bottom;
	
	Pface[0].x = right;
	Pface[0].z = otherheight;
	
	Pface[1].x = left;
	Pface[1].z = otherheight;
	
	Pface[2].x = left;
	Pface[2].z = height;
	
	Pface[3].x = right;
	Pface[3].z = height;
	
	_s2priv_fullTrans(Pface, 4, tr);
	//ns2Vf4(Pface, _s2_colormap[idx_front]);
	ns2Vf4(Pface, frontcol);
	
	// 4. right side of building if last one
	if (i == i2) {
	  otherheight = datamin;

	  Pface[0].x = Pface[1].x = Pface[2].x = Pface[3].x = right;
	
	  Pface[0].y = bottom;
	  Pface[0].z = otherheight;
	  
	  Pface[1].y = top;
	  Pface[1].z = otherheight;
	  
	  Pface[2].y = top;
	  Pface[2].z = height;
	  
	  Pface[3].y = bottom;
	  Pface[3].z = height;
	  
	  _s2priv_fullTrans(Pface, 4, tr);
	  //ns2Vf4(Pface, _s2_colormap[idx_left]);
	  ns2Vf4(Pface, leftcol);
	}

	if (j == j2) {
	  otherheight = datamin;
	  Pface[0].y = Pface[1].y = Pface[2].y = Pface[3].y = top;
	  
	  Pface[0].x = right;
	  Pface[0].z = otherheight;
	  
	  Pface[1].x = left;
	  Pface[1].z = otherheight;
	  
	  Pface[2].x = left;
	  Pface[2].z = height;
	  
	  Pface[3].x = right;
	  Pface[3].z = height;
	  
	  _s2priv_fullTrans(Pface, 4, tr);
	  //ns2Vf4(Pface, _s2_colormap[idx_front]);
	  ns2Vf4(Pface, frontcol);
	}

      } // if (walls) {
    }
  }
}      

/* "impulse" plot with arbitrary rotation / skew / translation */
void s2impa(float **data, int nx, int ny, 
	    int i1, int i2, int j1, int j2,
	    float datamin, float datamax, 
	    float *tr, int trunk, int symbol) {
  
  if ((i2 <= i1) || (i1 < 0) || (i1 >= nx) || (i2 < 0) || (i2 >= nx) ||
      (j2 <= j1) || (j1 < 0) || (j1 >= ny) || (j2 < 0) || (j2 >= ny) ||
      (datamax <= datamin)) {
    _s2warn("s2impa", "invalid data dimension or slice");
    return;
  }

  if (_s2_cmapsize < 1) {
    _s2warn("s2impa", "colormap too small or not set");
    return;
  }

  if (!data || !tr) {
    _s2warn("s2impa", "invalid function args (NULL ptr)");
    return;
  }
  
  int i, j;
  
  int colidx;
  XYZ P[2];
  float x[2], y[2], z[2];

  for (i = i1; i <= i2; i++) {
    for (j = j1; j <= j2; j++) {
      P[0].x = P[1].x = (float)i;
      P[0].y = P[1].y = (float)j;
      P[0].z = data[i][j];
      P[1].z = datamin;

      colidx = MAX(_s2_colr1, MIN(_s2_colr2, _s2_colr1 + (P[0].z - datamin) / 
				  (datamax - datamin) * 
				  (float)(_s2_colr2 - _s2_colr1 + 1)));
      s2sci(colidx);

      _s2priv_fullTrans(P, 2, tr);
      
      
      s2pt1(P[0].x, P[0].y, P[0].z, symbol);

      if (trunk) {
	x[0] = P[0].x;
	x[1] = P[1].x;
	y[0] = P[0].y;
	y[1] = P[1].y;
	z[0] = P[0].z;
	z[1] = P[1].z;
	s2line(2, x, y, z);
      }

    }
  }
}

/* set the projection type */
void ss2spt(int projtype) {
  switch(options.projectiontype) {

  case PERSPECTIVE:
  case ORTHOGRAPHIC:
    /* change to either of these allowed */
    switch(projtype) {
    case PERSPECTIVE:
    case ORTHOGRAPHIC:
      options.projectiontype = projtype;
      break;
    default:
      _s2warn("ss2spt", "invalid change in projection type requested");
      break;
    }
    break;

  case FISHEYE:
    _s2warn("ss2spt", "cannot change out of this projection mode");
    break;

  }
}

void ss2snfe(float expand) {
  _s2_nearfar_expand = expand;
}

void ss2qnfp(double *near, double *far) {
  *near = _s2_save_near;
  *far = _s2_save_far;
}

/* set foreground color */
void ss2sfc(float r, float g, float b) {
  _s2_foreground.r = r;
  _s2_foreground.g = g;
  _s2_foreground.b = b;
}

#if (S2_NO_S2GEOMVIEWER)
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

  // added 20071213 to half-fix dome fade-in/out
  //glViewport(0, 0, options.screenwidth, options.screenheight);

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
#endif

#if defined(BUILDING_S2PLOT)

// toggle between honouring screen constraints (from environment vars)
// and not.  ie. toggle fs to constrained scr
void _s2priv_togglefs() {
  // start is NOT full screen
  static int fs = 0;

  // toggle
  fs = (fs + 1) % 2;

  // adjust panels
  int i;
  S2PLOT_PANEL *it;
  for (i = 0; i < _s2_npanels; i++) {
    it = &(_s2_panels[i]);

    if (fs) {
      it->x1 = (it->x1 - _s2_scr_x1) / (_s2_scr_x2 - _s2_scr_x1);
      it->x2 = (it->x2 - _s2_scr_x1) / (_s2_scr_x2 - _s2_scr_x1);
      it->y1 = (it->y1 - _s2_scr_y1) / (_s2_scr_y2 - _s2_scr_y1);
      it->y2 = (it->y2 - _s2_scr_y1) / (_s2_scr_y2 - _s2_scr_y1);
    } else {
      it->x1 = _s2_scr_x1 + it->x1 * (_s2_scr_x2 - _s2_scr_x1);
      it->x2 = _s2_scr_x1 + it->x2 * (_s2_scr_x2 - _s2_scr_x1);
      it->y1 = _s2_scr_y1 + it->y1 * (_s2_scr_y2 - _s2_scr_y1);
      it->y2 = _s2_scr_y1 + it->y2 * (_s2_scr_y2 - _s2_scr_y1);
    }
  }
}

void xs2tp(int panelid) {
  if (panelid < 0 || panelid >= _s2_npanels) {
    _s2warn("xs2tp", "invalid panel number");
    return;
  }
  _s2_panels[panelid].active = (_s2_panels[panelid].active + 1) % 2;
}
void xs2lpc(int masterid, int slaveid) {
  // good grief this is such a cheeky hack
  _s2_panels[slaveid].camera = _s2_panels[masterid].camera;
  _s2_panels[slaveid].autospin = _s2_panels[masterid].autospin;
}

int xs2qpa(int panelid) {
  if (panelid < 0 || panelid >= _s2_npanels) {
    _s2warn("xs2qpa", "invalid panelid provided\n");
    return 0;
  }
  return _s2_panels[panelid].active;
}
int xs2qcpa(void) {
  if (_s2_activepanel < 0 || _s2_activepanel >= _s2_npanels) {
    _s2warn("xs2qcpa", "internal error: active panel is invalid\n");
    return 0;
  }
  return _s2_panels[_s2_activepanel].active;
}
int xs2qsp(void) {
  if (_s2_activepanel < 0 || _s2_activepanel >= _s2_npanels) {
    _s2warn("xs2qsp", "internal error: active panel is invalid\n");
    return -1;
  }
  return _s2_activepanel;
}

/* set panel frame properties */
void xs2spp(COLOUR active, COLOUR inactive, float width) {
  _s2_activepanelframecolour = active;
  _s2_panelframecolour = inactive;
  _s2_panelframewidth = width;
}


#endif // BUILDING_S2PLOT possibly nested for no good reason

/* query color index */
int s2qci(void) {
  return _s2_colidx;
}
/* query line width */
int s2qlw(void) {
  return _s2_linewidth;
}
/* query line style */
int s2qls(void) {
  return _s2_linestyle;
}
/* query character height */
float s2qch(void) {
  return _s2_charsize;
}
/* query arrow-head style */
void s2qah(int *fs, float *angle, float *barb) {
  if (fs) {
    *fs = _s2_arrow_fs;
  }
  if (angle) {
    *angle = _s2_arrow_angle;
  }
  if (barb) {
    *barb = _s2_arrow_barb;
  }
}

/* enable/disable/toggle clipping */
void s2twc(int enabledisable) {
  switch(enabledisable) {
  case 1:
    _s2_clipping = 1;
    break;
  case 0:
    _s2_clipping = 0;
    break;
  case -1:
    _s2_clipping = (_s2_clipping + 1) % 2;
    break;
  default:
    _s2warn("s2twc", "enabledisable argument must be 0, 1 or -1");
    break;
  }
}

/* query clipping state */
int s2qwc(void) {
  return _s2_clipping;
}

/* query sphere resolution */
int ss2qsr(void) {
  return options.sphereresolution;
}
/* query background colour */
void ss2qbc(float *r, float *g, float *b) {
  if (r) {
    *r = options.background.r;
  }
  if (g) {
    *g = options.background.g;
  }
  if (b) {
    *b = options.background.b;
  }
}
/* query foreground colour */
void ss2qfc(float *r, float *g, float *b) {
  if (r) {
    *r = _s2_foreground.r;
  }
  if (g) {
    *g = _s2_foreground.g;
  }
  if (b) {
    *b = _s2_foreground.b;
  }
}
/* query fisheye rotation angle (in degrees) */
float ss2qfra(void) {
  return camera.fishrotate * RTOD;
}
/* query autospin state */
int ss2qas(void) {
  if ((options.autospin.x > 0.) || 
      (options.autospin.y > 0.) || 
      (options.autospin.z > 0.)) {
    return 1;
  } else {
    return 0;
  }
}

/* query camera focus point */
void ss2qcf(int *set, XYZ *position, int worldcoords) {
  if (worldcoords) {
    if (position) {
      position->x = _S2DEVICE2WORLD(_s2_camfocus.x, _S2XAX);
      position->y = _S2DEVICE2WORLD(_s2_camfocus.y, _S2YAX);
      position->z = _S2DEVICE2WORLD(_s2_camfocus.z, _S2ZAX);
    }
  } else {
    if (position) {
      *position = _s2_camfocus;
    }
  }  
  if (set) {
    *set = _s2_camexfocus;
  }
}

/* query camera point to rotate about - sometimes not the same as focus point */
void ss2qpr(XYZ *position, int worldcoords) {
  if (worldcoords) {
    if (position) {
      position->x = _S2DEVICE2WORLD(camera.pr.x, _S2XAX);
      position->y = _S2DEVICE2WORLD(camera.pr.y, _S2YAX);
      position->z = _S2DEVICE2WORLD(camera.pr.z, _S2ZAX);
    } 
  } else {
    if (position) {
      *position = camera.pr;
    }
  }
}


/* query aspect ratio: width to height ratio */
float ss2qar(void) {
  float ratio = 1.;
  if ((options.stereo == DUALSTEREO) || (options.stereo == WDUALSTEREO)) {
    ratio *= 0.5;
  } else if (options.stereo == TRIOSTEREO) {
    ratio /= 3.0;
  }
#if defined(BUILDING_S2PLOT)
  //return ratio * (float)options.screenwidth / (float)options.screenheight *
  return (ratio * (float)s2winGet(S2_WINDOW_WIDTH) / 
	  (float)s2winGet(S2_WINDOW_HEIGHT) *
	  (_s2_panels[_s2_activepanel].x2 - _s2_panels[_s2_activepanel].x1) /
	  (_s2_panels[_s2_activepanel].y2 - _s2_panels[_s2_activepanel].y1));
#else
  //return ratio * (float)options.screenwidth / (float)options.screenheight;
  return ratio * (float)s2winGet(S2_WINDOW_WIDTH) / 
    (float)s2WinGet(S2_WINDOW_HEIGHT);
#endif
}

/* query screen dimensions */
void ss2qsd(int *x, int *y) {
  float ratio = 1.;
  if ((options.stereo == DUALSTEREO) || (options.stereo == WDUALSTEREO)) {
    ratio *= 0.5;
  } else if (options.stereo == TRIOSTEREO) {
    ratio /= 3.0;
  }
#if defined(BUILDING_S2PLOT)
  if (x) {
    *x = ratio * (float)options.screenwidth * 
      (_s2_panels[_s2_activepanel].x2 - _s2_panels[_s2_activepanel].x1);
  }
  if (y) {
    *y = (float)options.screenheight * 
      (_s2_panels[_s2_activepanel].y2 - _s2_panels[_s2_activepanel].y1);
  }
#else
  if (x) {
    *x = (float)options.screenwidth * ratio;
  }
  if (y) {
    *y = (float)options.screenheight * ratio;
  }
#endif
}

/* query screen attributes: stereo, fullscreen and dome status */
void ss2qsa(int *stereo, int *fullscreen, int *dome) {
  if (stereo) {
    *stereo = options.stereo;
  }
  if (fullscreen) {
    *fullscreen = options.fullscreen;
  }
  if (dome) {
    *dome = options.dometype;
  }
}

//#define S2THREADS
#if defined(S2THREADS)
/* Draw the scene.  Argument interactive should be non-zero.  If zero,
 * a warning will be issued.  Presence of this argument is historical.
 * This function never returns.  If you need to regain control after
 * displaying graphics, consider using s2disp.
 */
void s2show_thr(int interactive);
#endif

#if defined(S2THREADS)
#include <pthread.h>
int thr_id;
pthread_t p_thread;
//int a = 1;
void *s2thread_sub(void *data) {
  s2winMainLoop();
  fprintf(stderr, "s2win (glut or other) finished!\n");
  pthread_exit(NULL);
}
void s2show_thr(int iinteractive) {

#if defined(BUILDING_S2PLOT)
  int waspanel = _s2_activepanel;
  int spid;
  for (spid = 0; spid < _s2_npanels; spid+=1) {
    if (!_s2_panels[spid].active) {
      continue;
    }
    xs2cp(spid);
#endif
    
    CameraHome(HOME);
    options.camerahome = camera;
    if (_s2_cameraset) {
      _s2priv_CameraSet();
    }
    MakeGeometry(TRUE, FALSE, 'c');
#if defined(BUILDING_S2PLOT)
  }
  xs2cp(waspanel);
#endif
  
  fprintf(stderr, "THREADING interactive mode.  Shift-ESC or Shift-Q to quit.\n");

  if (pthread_create(&p_thread, NULL, s2thread_sub, NULL)) {
    fprintf(stderr, "threading failed\n");
  } else {
    fprintf(stderr, "threading succeeded\n");
  }

}
#endif // # if defined(S2THREADS)


void _s2priv_initPrompt() {
  _s2prompt_length = 0;
  s2winKeyboardFunc(_s2_prompt_kbdfunc);
}
void _s2priv_finiPrompt() {
  _s2prompt_length = -1;
  s2winKeyboardFunc(HandleKeyboard);
}
/* set the prompt callback function - this one takes a void data ptr
 * that the user can use arbitrarily.
 */
void cs2spcb(void *ipcbx, void *data) {
  _s2_promptcbx = (void (*)(char *, void *))ipcbx;
  _s2_promptcbx_data = data;
}
void *cs2qpcb(void) {
  return (void *)_s2_promptcbx;
}
/* set the prompt text and location */
void cs2sptxy(char *prompt, float xfrac, float yfrac) {
  strncpy(_s2baseprompt, prompt, S2BASEPROMPT_LEN);
  _s2baseprompt[S2BASEPROMPT_LEN-1] = '\0';
  _s2prompt_x = xfrac;
  _s2prompt_y = yfrac;
}


void _s2_drawBGimage(void) {
  GLfloat white[4] = {1.0,1.0,1.0,1.0};
  GLfloat black[4] = {0.0,0.0,0.0,1.0};
  int deflightlist[8] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,
			 GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};
  int i;
  for (i = 0; i < 8; i++) {
    glDisable(deflightlist[i]);
    glLightfv(deflightlist[i], GL_AMBIENT, black);
  }
  GLfloat lightp[] = {0.5, 0.5, 0.5};
  glLightfv(deflightlist[0], GL_POSITION, lightp);
  glLightfv(deflightlist[0], GL_AMBIENT, white);
  glEnable(deflightlist[0]);

  glDisable(GL_LIGHTING);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glShadeModel(GL_FLAT);

  glPushMatrix();
  glDisable(GL_DEPTH_TEST);
  
  glDisable(GL_BLEND);
  glColor4f(1., 1., 1., 1.);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, _s2_bg_texid);

  glBegin(GL_QUADS);
  glTexCoord2f(0., 0.); 
  glVertex2f(0., 0.);

  glTexCoord2f(0., 1.);
  glVertex2f(0., 1.);
  
  glTexCoord2f(1., 1.);
  glVertex2f(1., 1.);
  
  glTexCoord2f(1., 0.);
  glVertex2f(1., 0.);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);
  glPopMatrix();
}

void _s2_blankPanel(void) {
  // blank the visible area for this panel...
  glPushMatrix();
  glDisable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glMatrixMode (GL_PROJECTION); 
  glPushMatrix (); 
  glLoadIdentity ();
  glOrtho(0, 1, 0, 1, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_TEXTURE_2D);

  // outer transparent border
  if (_s2_bg_clear) {
    //glColor4f(0., 0., 0., 0.7);
    glColor4f(options.background.r, options.background.g, 
	      options.background.b, 0.7);
    glBegin (GL_QUADS); 
    glVertex2f(0., 0.);
    glVertex2f(0., 1.);
    glVertex2f(1., 1.);
    glVertex2f(1., 0.);
    glEnd();
  }

  // inner solid box
  float dddel = 0.02;
  float asr = ss2qar();
  if (_s2_bg_clear) {
    glColor4f(options.background.r, options.background.g, 
	      options.background.b, 1.);
    //  glColor4f(0., 0., 0., 1.);
    glBegin (GL_QUADS); 
    glVertex2f(0.+dddel/asr, 0.+dddel);
    glVertex2f(0.+dddel/asr, 1.-dddel);
    glVertex2f(1.-dddel/asr, 1.-dddel);
    glVertex2f(1.-dddel/asr, 0.+dddel);
    glEnd();
  }
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glEnable(GL_DEPTH_TEST);
  //glPopMatrix();
}

#endif




void drawView(char *projinfo, double camsca) {
  if (strlen(projinfo) < 1) {
    _s2warn("(internal)", "no projection info for draw context");
    return;
  }
  
  // viewport is set before this fn is called: get it
  GLint view[4];
  glGetIntegerv(GL_VIEWPORT, view);
  int x0 = view[0];
  int y0 = view[1];
  int dx = view[2];
  int dy = view[3];

  XYZ vright, camoff;

#if defined(BUILDING_S2PLOT)
  // possibly draw the background image 
  if (_s2_bg_texid >= 0) {
    //glViewport(0,0,options.screenwidth, options.screenheight);
    _s2_drawBGimage();
  }
  
  // loop over the panels
  int waspanel = _s2_activepanel;
  int spid;
  for (spid = 0; spid < _s2_npanels; spid+=1) {
    if (!_s2_panels[spid].active) {
      continue;
    }
    xs2cp(spid);
#endif

    CROSSPROD(camera.vd,camera.vu,vright);
    Normalise(&vright);
    vright.x *= camera.eyesep / 2;
    vright.y *= camera.eyesep / 2;
    vright.z *= camera.eyesep / 2;
#if defined(BUILDING_S2PLOT)
    camoff = VectorMul(vright, camsca * _s2_eyesepmul);
#else
    camoff = VectorMul(vright, camsca);
#endif

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    CreateProjection(projinfo[0]);
    
#if defined(BUILDING_S2PLOT)
    glViewport((int)(x0 + _s2_panels[spid].x1 * (float)dx),
	       (int)(y0 + _s2_panels[spid].y1 * (float)dy),
	       (int)((_s2_panels[spid].x2 - _s2_panels[spid].x1) * (float)dx),
	       (int)((_s2_panels[spid].y2 - _s2_panels[spid].y1) * (float)dy));
#endif

    glMatrixMode(GL_MODELVIEW);

#if defined(BUILDING_S2PLOT)
    if (_s2_bg_texid >= 0) {
      _s2_blankPanel();
    }
#endif
    
    glLoadIdentity();
    s2LookAt(camera.vp.x + camoff.x,
	      camera.vp.y + camoff.y,
	      camera.vp.z + camoff.z,
	      camera.vp.x + camoff.x + camera.vd.x,
	      camera.vp.y + camoff.y + camera.vd.y,
	      camera.vp.z + camoff.z + camera.vd.z,
	      camera.vu.x,camera.vu.y,camera.vu.z);
    
#if defined(BUILDING_S2PLOT)
    /* get projections needed for coordinate trans */
    glGetDoublev(GL_MODELVIEW_MATRIX, _s2_dragmodel);
    glGetDoublev(GL_PROJECTION_MATRIX, _s2_dragproj);
    glGetIntegerv(GL_VIEWPORT, _s2_dragview);
#endif    

    MakeLighting();
    MakeMaterial();
    

    MakeGeometry(FALSE, FALSE, projinfo[0]);
    
#if defined(BUILDING_S2PLOT)
    /* draw the dynamic geometry */

    // draw the screen geometry
    strcpy(_s2_doingScreen, projinfo);
    glDisable(GL_LIGHTING);
    int tmp = options.rendermode;
    options.rendermode = SHADE_FLAT;
    MakeGeometry(FALSE, TRUE, projinfo[0]);
    glEnable(GL_LIGHTING);
    options.rendermode = tmp;
    strcpy(_s2_doingScreen, "");

    // draw the panel surround
    if (_s2_npanels > 1) { 	
      _s2priv_drawActiveFrame(spid == waspanel);
    }


    // an excursion: now generate the projection, view and model
    // matrices for a 'c' view: these are the ones kept for drag
    // use.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    CreateProjection('c');
    glViewport((int)(x0 + _s2_panels[spid].x1 * (float)dx),
	       (int)(y0 + _s2_panels[spid].y1 * (float)dy),
	       (int)((_s2_panels[spid].x2 - _s2_panels[spid].x1) * (float)dx),
	       (int)((_s2_panels[spid].y2 - _s2_panels[spid].y1) * (float)dy));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    s2LookAt(camera.vp.x,
	      camera.vp.y,
	      camera.vp.z,
	      camera.vp.x + camera.vd.x,
	      camera.vp.y + camera.vd.y,
	      camera.vp.z + camera.vd.z,
	      camera.vu.x,camera.vu.y,camera.vu.z);
    glGetDoublev(GL_MODELVIEW_MATRIX, _s2_dragmodel);
    glGetDoublev(GL_PROJECTION_MATRIX, _s2_dragproj);
    glGetIntegerv(GL_VIEWPORT, _s2_dragview);
    // done

  }
  xs2cp(waspanel);

#endif
  
  glViewport(x0, y0, dx, dy);

#if defined(BUILDING_S2PLOT)
  // global fade-in/out for this view 
  _s2_fadeinout();
#endif

  return;
}

#if defined(BUILDING_S2PLOT)
void handleView(int msx, int msy) {
#define MAXSELECT 100
  int maxselect = MAXSELECT;
  int nhits = 0;
  GLuint selectlist[MAXSELECT];
  GLint viewport[4];
  
  // viewport is set before this fn is called: get it
  glGetIntegerv(GL_VIEWPORT, viewport);
  int x0 = viewport[0];
  int y0 = viewport[1];
  int dx = viewport[2];
  int dy = viewport[3];

  // loop over the panels
  int waspanel = _s2_activepanel;
  int spid;
  for (spid = 0; spid < _s2_npanels; spid+=1) {
    if (!_s2_panels[spid].active) {
      continue;
    }

    xs2cp(spid);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glViewport((int)(x0 + _s2_panels[spid].x1 * (float)dx),
	       (int)(y0 + _s2_panels[spid].y1 * (float)dy),
	       (int)((_s2_panels[spid].x2 - _s2_panels[spid].x1) * (float)dx),
	       (int)((_s2_panels[spid].y2 - _s2_panels[spid].y1) * (float)dy));

    glGetIntegerv(GL_VIEWPORT, viewport); /* Get the viewport bounds */
    gluPickMatrix(msx, options.screenheight-msy, 3.0, 3.0, viewport);

    CreateProjection('c');
    glMatrixMode(GL_MODELVIEW);
    glDrawBuffer(GL_BACK_LEFT);
    glLoadIdentity();
    s2LookAt(camera.vp.x,
	      camera.vp.y,
	      camera.vp.z,
	      camera.vp.x + camera.vd.x,
	      camera.vp.y + camera.vd.y,
	      camera.vp.z + camera.vd.z,
	      camera.vu.x,camera.vu.y,camera.vu.z);
    
    glInitNames();

    // prepare for selection rather than drawing
    glSelectBuffer(maxselect, selectlist);
    glRenderMode(GL_SELECT);

    // draw only the dynamic handles
    
    /* draw the dynamic geometry */
    _s2_startDynamicGeometry(FALSE);
    // 3d handles
    _s2priv_drawHandles(FALSE);
    // screen handles
    strcpy(_s2_doingScreen, "c");
    _s2priv_drawHandles(TRUE);
    strcpy(_s2_doingScreen, "");
    _s2_endDynamicGeometry();

    glInitNames();

    // did we hit any handle/s?
    if ((nhits = glRenderMode(GL_RENDER)) < 0) {
      _s2warn("(internal)", "selection buffer overflow");

    } else if ((nhits >= 1) && (selectlist[0] == 1)) {
      
      // now switch behaviour depending on which modifier
      // keys are down:
      // SHIFT = drag in screen plane
      // CONTROL = drag in/out of screen
      // no modifier = select / deselect
      
      if (s2winGetModifiers() & S2_ACTIVE_SHIFT) {

	// note: we skip to element 3 in selectlist array:
	// this is safe because we have already checked there
	// is just one hit, with just one name to fetch
	// (see opengl docs on select buffer)
	_s2_draghandle = _s2priv_findHandle(selectlist[3]);
	
	_s2_dragpanel = spid;

	if (strlen(_s2_draghandle->whichscreen)) {
	  _s2_draghandle_screen = 1;
	  _s2_draghandle_id = _s2_draghandle->id;
	  _s2_draghandle_basex = _s2_draghandle->p.x;
	  _s2_draghandle_basey = _s2_draghandle->p.y;
	  _s2_draghandle_savez = _s2_draghandle->p.z;
	} else {
	  _s2_draghandle_screen = 0;
	  // get position of this handle in the zbuffer
	  GLdouble scx, scy, scz; /* screen vertices */
	  s2Project(_s2_draghandle->p.x, 
		     _s2_draghandle->p.y,
		     _s2_draghandle->p.z,
		     _s2_dragmodel, _s2_dragproj, _s2_dragview,
		     &scx, &scy, &scz);
	  // fetched scx and scy appear to be rather disconnected
	  // from screen pixels.  Probably scaled by the 
	  // pickmatrix set above.	  
	  _s2_draghandle_id = _s2_draghandle->id;
	  _s2_draghandle_basex = (int)scx + 0.5;
	  _s2_draghandle_basey = (int)scy + 0.5;
	  _s2_draghandle_savez = scz;
	}
	
	
      } else if (s2winGetModifiers() && S2_ACTIVE_CTRL) {

      } else {

	_s2priv_handleHits(nhits, selectlist);
	
      }

      // exit this loop as we've found and processed a hit
      break;
    }
    
  }
  xs2cp(waspanel);
  
  glViewport(x0, y0, dx, dy);
  return;
}

#define SAMECOLOUR(x,y) ((fabs((x).r-(y).r)<0.001) && (fabs((x).g-(y).g)<0.001) && (fabs((x).b-(y).b)<0.001))
static int _s2x_shapecount;

void pushVRMLname(char *iname) {
  // 1. find out if we already have the name...
  int i = 0;
  while ((i < _s2_nVRMLnames) && strcmp(_s2_VRMLnames[i], iname)) {
    i++;
  }
  if (i < _s2_nVRMLnames) {
    _s2_currVRMLidx = i;
    return;
  }

  _s2_VRMLnames = (char **)realloc(_s2_VRMLnames, 
				   (_s2_nVRMLnames+1)*sizeof(char *));
  _s2_VRMLnames[_s2_nVRMLnames] = (char *)malloc((strlen(iname)+1)*sizeof(char));
  /*
  strncpy(_s2_VRMLnames[_s2_nVRMLnames], iname, MAXVRMLLEN);
  _s2_VRMLnames[_s2_nVRMLnames][MAXVRMLLEN-1] = '\0';
  */
  strcpy(_s2_VRMLnames[_s2_nVRMLnames], iname);
  //_s2_VRMLnames[_s2_nVRMLnames][MAXVRMLLEN-1] = '\0';
  _s2_currVRMLidx = _s2_nVRMLnames;
  _s2_nVRMLnames++;
}

// sort face3a based on vertex colours.  Only sort on first vertex,
// as subsequent optimisation requires all vertices of a single
// triangle to be the same anyway
#define COLOUREPS 0.01
int _s2_face3a_compare_colour(const void *va, const void *vb) {
  _S2FACE3A *a = (_S2FACE3A *)va;
  _S2FACE3A *b = (_S2FACE3A *)vb;
  static float cmpstring;
  cmpstring = strcmp(a->VRMLname, b->VRMLname);
  if (cmpstring < 0) {
    return -1;
  } else if (cmpstring > 0) {
    return 1;
  } else if (a->colour[0].r < b->colour[0].r - COLOUREPS) {
    return -1;
  } else if (a->colour[0].r > b->colour[0].r + COLOUREPS) {
    return +1;
  } else if (a->colour[0].g < b->colour[0].g - COLOUREPS) {
    return -1;
  } else if (a->colour[0].g > b->colour[0].g + COLOUREPS) {
    return +1;
  } else if (a->colour[0].b < b->colour[0].b - COLOUREPS) {
    return -1;
  } else if (a->colour[0].b > b->colour[0].b + COLOUREPS) {
    return +1;
  }
  return 0;
}
int _s2_face3_compare_colour(const void *va, const void *vb) {
  FACE3 *a = (FACE3 *)va;
  FACE3 *b = (FACE3 *)vb;
  static float cmpstring;
  cmpstring = strcmp(a->VRMLname, b->VRMLname);
  if (cmpstring < 0) {
    return -1;
  } else if (cmpstring > 0) {
    return 1;
  } else if (a->colour[0].r < b->colour[0].r - COLOUREPS) {
    return -1;
  } else if (a->colour[0].r > b->colour[0].r + COLOUREPS) {
    return +1;
  } else if (a->colour[0].g < b->colour[0].g - COLOUREPS) {
    return -1;
  } else if (a->colour[0].g > b->colour[0].g + COLOUREPS) {
    return +1;
  } else if (a->colour[0].b < b->colour[0].b - COLOUREPS) {
    return -1;
  } else if (a->colour[0].b > b->colour[0].b + COLOUREPS) {
    return +1;
  }
  return 0;
}


void writeVRML20(void) {

  FILE *fptr;
  if ((fptr = fopen("test.wrl", "w")) == NULL) {
    _s2warn("writeVRML20", "could not open file for writing");
    return;
  }
  
  _s2x_shapecount = 0;

  fprintf(fptr, "#VRML V2.0 utf8\n");
  fprintf(fptr, "#\n");
  fprintf(fptr, "# created by S2PLOT\n");
  fprintf(fptr, "\n");

  fprintf(fptr, "WorldInfo {\n");
  fprintf(fptr, "title \"S2PLOT VRML 2.0 model\"\n");
  fprintf(fptr, "info [ \"created by username using S2PLOT YYYYMMDD_HHMMSS\" ]\n");
  fprintf(fptr, "}\n");

  //xs2cp(_s2_activepanel);
  int dloop;
  int i, j;

  // lights
  if (nlight < 0) {
    GLfloat p[3];
    float delta;
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
      //p[3] = 1.0;
      fprintf(fptr, "DEF LIGHT%d PointLight {\n", i);
      fprintf(fptr, "  location %f %f %f\n", p[0], p[1], p[2]);
      fprintf(fptr, "  color 1 1 1\n");
      fprintf(fptr, "  radius 1000000.0\n");
      fprintf(fptr, "}\n\n");
    }
  } else {
    for (i = 0; i < nlight; i++) {
      fprintf(fptr, "DEF LIGHT%d PointLight {\n", i);
      fprintf(fptr, "  location %f %f %f\n", lights[i].p[0],
	      lights[i].p[1], lights[i].p[2]);
      fprintf(fptr, "  color %f %f %f\n", lights[i].c[0],
	      lights[i].c[1], lights[i].c[2]);
      fprintf(fptr, "  radius 1000000.0\n");
      fprintf(fptr, "}\n\n");
    }
  }


  int nidx;
  for (nidx = 0; nidx < _s2_nVRMLnames; nidx++) {
    fprintf(fptr, "DEF %s Group { \n children [\n", _s2_VRMLnames[nidx]);


    for (dloop = 0; dloop < 2; dloop++) {
      if (dloop == 1) {
	_s2_startDynamicGeometry(FALSE);
      }
      
      if (ndot > 0) {
	fprintf(fptr, "DEF DOTS%d Group { \n children [\n", dloop);
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
	  while ((j < ndot) && (dot[i].size == dot[j].size) &&
		 !strcmp(dot[i].VRMLname, dot[j].VRMLname) &&
		 !strlen(dot[i].whichscreen) && 
		 SAMECOLOUR(dot[i].colour, dot[j].colour)) {
	    j++;
	  }
	  if (j > i) {
	    j--;
	  }
	  // emit geometry
	  doVRMLdots(fptr, i,j);
	  i = j + 1;
	}
	fprintf(fptr, "] \n }\n\n");
      }
      
      if (nline > 0) {
	fprintf(fptr, "DEF LINES%d Group {\n children [\n", dloop);
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
	  while ((j < nline) && (line[i].width == line[j].width) &&
		 !strcmp(line[i].VRMLname, line[j].VRMLname) && 
		 !strlen(line[i].whichscreen) && 
		 SAMECOLOUR(line[i].colour[0], line[j].colour[0])) {
	    j++;
	  }
	  if (j > i) {
	    j--;
	  }
	  // emit geometry
	  doVRMLlines(fptr, i, j);
	  i = j + 1;
	}
	fprintf(fptr, "] \n }\n\n");
      }
      
      if (nface4 > 0) {
	fprintf(fptr, "DEF FACE4S%d Group {\n children [\n", dloop);
	
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
	  // get end point
	  j = i;
	  // emit geometry
	  doVRMLface4(fptr, i, j);
	  i = j + 1;
	}
	
	fprintf(fptr, "] \n }\n\n");
      }
      
      if (nface4t > 0) {
	fprintf(fptr, "DEF FACE4TS%d Group {\n children [\n", dloop);
	
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

	  doVRMLface4t(fptr, i, j);
	  i = j + 1;
	}
	
	fprintf(fptr, "] \n }\n\n");
      }
            
      if (nface3 > 0) {

	// sort face3 by colour, so that close colours are all 
	// together and can be compiled (below) into single 
	// groups.
	qsort((void *)face3, nface3, sizeof(FACE3),
	      _s2_face3_compare_colour);

	fprintf(fptr, "DEF FACE3S%d Group {\n children [\n", dloop);
	
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
	  
	  if (!S2_COLOURWITHIN(face3[i].colour[0], face3[i].colour[1], 0.01) ||
	      !S2_COLOURWITHIN(face3[i].colour[0], face3[i].colour[2], 0.01)) {
	    // individual vertices differ, no point looking for same 
	    // coloured vertices to draw in one set
	  } else {
	    while ((j < nface3) &&
		   !(strcmp(face3[j].VRMLname, _s2_VRMLnames[nidx]) ||
		     strlen(face3[j].whichscreen)) &&
		   S2_COLOURWITHIN(face3[i].colour[0],face3[j].colour[0],0.01) &&
		   S2_COLOURWITHIN(face3[i].colour[0],face3[j].colour[1],0.01) &&
		   S2_COLOURWITHIN(face3[i].colour[0],face3[j].colour[2],0.01)) {
	      j++;
	    }
	  }
	  j--;
	  if (j == nface3) {
	      j--;
	  }
	 
	  // emit geometry
	  doVRMLface3(fptr, i, j);
	  i = j + 1;
	}
	
	fprintf(fptr, "] \n }\n\n");
      }

      if (nface3a > 0) {

	// sort face3a by colour, so that close colours are 
	// all together and can be compiled (below) into single
	// groups.
	qsort((void *)face3a, nface3a, sizeof(_S2FACE3A), 
	      _s2_face3a_compare_colour);

	fprintf(fptr, "DEF FACE3AS%d Group {\n children [\n", dloop);

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

	  if (!S2_COLOURWITHIN(face3a[i].colour[0], face3a[i].colour[1], 0.01) ||
	      !S2_COLOURWITHIN(face3a[i].colour[0], face3a[i].colour[2], 0.01)) {
	    // individual vertices differ, no point looking for same 
	    // coloured vertices to draw in one set
	  } else {
	    while ((j < nface3a) &&
		   !(strcmp(face3a[j].VRMLname, _s2_VRMLnames[nidx]) ||
		     strlen(face3a[j].whichscreen)) &&
		   S2_COLOURWITHIN(face3a[i].colour[0],face3a[j].colour[0],0.01) &&
		   S2_COLOURWITHIN(face3a[i].colour[0],face3a[j].colour[1],0.01) &&
		   S2_COLOURWITHIN(face3a[i].colour[0],face3a[j].colour[2],0.01)) {
	      j++;
	    }
	  }
	  j--;
	  if (j == nface3a) {
	      j--;
	  }

	  // emit geometry
	  doVRMLface3a(fptr, i, j);
	  i = j + 1;
	}

	fprintf(fptr, "] \n }\n\n");
      }

      if (nball > 0) {
	fprintf(fptr, "DEF BALLS%d Group {\n children [\n", dloop);

	for (i = 0; i < nball; ){
	  // get start point
	  while ((i < nball) &&
		 (strcmp(ball[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(ball[i].whichscreen))) {
	    i++;
	  }
	  if (i == nball) {
	    break;
	  }
	  // get end point
	  j = i;
	  doVRMLball(fptr, i, j);
	  i = j + 1;
	}

	fprintf(fptr, "] \n }\n\n");
      }
      
      // cones in S2PLOT can be standard cones, truncated cones or cylinders
      // here we do the standard cones
      if (ncone > 0) {
	fprintf(fptr, "DEF CONES%d Group {\n children [\n", dloop);

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
	  // get end point
	  j = i;
	  doVRMLcone(fptr, i, j);
	  i = j + 1;
	}

	fprintf(fptr, "] \n }\n\n");
      }

      // repeat the cones, but for cylinders (r1 == r2)
      if (ncone > 0) {
	fprintf(fptr, "DEF CYLINDERS%d Group {\n children [\n", dloop);

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
	  // get end point
	  j = i;
	  doVRMLcylinder(fptr, i, j);
	  i = j + 1;
	}

	fprintf(fptr, "] \n }\n\n");
      }
      
      if (nlabel > 0) {
	fprintf(fptr, "DEF LABELS%d Group {\n children [\n", dloop);
	
	for (i = 0; i < nlabel; ){
	  // get start point
	  while ((i < nlabel) &&
		 (strcmp(label[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(label[i].whichscreen))) {
	    i++;
	  }
	  if (i == nlabel) {
	    break;
	  }
	  // get end point
	  j = i;
	  // emit geometry
	  doVRMLlabel(fptr, i, j);
	  i = j + 1;
	}
	fprintf(fptr, "] \n }\n\n");
      }
      
      if (nbboard > 0) {
	fprintf(fptr, "DEF BBOARDS%d Group {\n children [\n", dloop);
	
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
	  // get end point
	  j = i;
	  // emit geometry
	  doVRMLbboard(fptr, i, j);
	  i = j + 1;
	}
	
	fprintf(fptr, "] \n }\n\n");
      }
            

      if (nhandle > 0) {
	fprintf(fptr, "DEF HANDLES%d Group {\n children [\n", dloop);
	
	for (i = 0; i < nhandle; ) {
	  // get start point
	  while ((i < nhandle) && 
		 (strcmp(handle[i].VRMLname, _s2_VRMLnames[nidx]) ||
		  strlen(handle[i].whichscreen))) {
	    i++;
	  }
	  if (i == nhandle) {
	    break;
	  }
	  // get end point
	  j = i;
	  // emit geometry
	  doVRMLhandle(fptr, i, j);
	  i = j + 1;
	}
	
	fprintf(fptr, "] \n }\n\n");
      }
            

      if (dloop == 1) {
	_s2_endDynamicGeometry();
      }
    }

    fprintf(fptr, "] \n }\n\n");
  }

  fclose(fptr);
  fprintf(stderr, "Wrote VRML file \"test.wrl\" with c. %d shapes.\n", 
	  _s2x_shapecount);
}


void doVRMLdots(FILE *fp, int start, int end) {

  int i;

  fprintf(fp, "DEF SHAPE%d Shape {\n\t appearance Appearance { \n\t\t lineProperties LineProperties {\n", _s2x_shapecount++);
  fprintf(fp, "\t\t\t linewidthScaleFactor %f\n", dot[start].size);
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t\t material Material {\n");
  fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", dot[start].colour.r,
	  dot[start].colour.g, dot[start].colour.b);
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");

  fprintf(fp, "\t geometry PointSet {\n\t\t coord Coordinate {\n\t\t\t point [\n");
  for (i = start; i <= end; i++) {
    fprintf(fp, "\t\t\t\t %f %f %f", dot[i].p.x, dot[i].p.y, dot[i].p.z);
    if (i == end) {
      fprintf(fp, "\n\t\t\t ]\n");
    } else {
      fprintf(fp, ",\n");
    }
  }
  fprintf(fp, "\t\t}\n");

  fprintf(fp, "\t\t color Color {\n\t\t\t color [\n");
  for (i = start; i <= end; i++) {
    fprintf(fp, "\t\t\t\t %f %f %f", dot[i].colour.r, dot[i].colour.g, 
	    dot[i].colour.b);
    if (i == end) {
      fprintf(fp, "\n\t\t\t ]\n");
    } else {
      fprintf(fp, ",\n");
    }
  }
  fprintf(fp, "\t\t}\n");
  fprintf(fp, "\t}\n}\n");
 
}


void doVRMLlines(FILE *fp, int start, int end) {
  
  int i;
  
  fprintf(fp, "DEF SHAPE%d Shape {\n\t appearance Appearance { \n\t\t lineProperties LineProperties {\n", _s2x_shapecount++);
  fprintf(fp, "\t\t\t linewidthScaleFactor %f\n", line[start].width);
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t\t material Material {\n");
  fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", line[start].colour[0].r,
	  line[start].colour[0].g, line[start].colour[0].b);
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");

  fprintf(fp, "\t geometry IndexedLineSet {\n\t\t coord Coordinate {\n\t\t\t point [\n");
  for (i = start; i <= end; i++) {
    fprintf(fp, "\t\t\t\t %f %f %f,", line[i].p[0].x, line[i].p[0].y,
	    line[i].p[0].z);
    fprintf(fp, "\t\t\t\t %f %f %f", line[i].p[1].x, line[i].p[1].y,
	    line[i].p[1].z);
    if (i == end) {
      fprintf(fp, "\n\t\t\t ]\n");
    } else {
      fprintf(fp, ",\n");
    }
  }
  fprintf(fp, "\t\t}\n");

  fprintf(fp, "\t\t coordIndex [\n");
  for (i = start; i <= end; i++) {
    fprintf(fp, "\t\t\t %d, %d, -1", (i-start)*2, (i-start)*2 + 1);
    if (i == end) {
      fprintf(fp, "\n\t\t\t ]\n");
    } else {
      fprintf(fp, ",\n");
    }
  }
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");

}

void doVRMLface4(FILE *fp, int start, int end) {

  int i, j;

  fprintf(fp, "DEF SHAPE%d Shape {\n\t appearance Appearance { \n", _s2x_shapecount++);
  fprintf(fp, "\t\t material Material {\n");
  fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", face4[start].colour[0].r,
	  face4[start].colour[0].g, face4[start].colour[0].b);
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");


  fprintf(fp, "\t geometry IndexedFaceSet {\n\t\t coord Coordinate {\n\t\t\t point [\n");
  for (i = start; i <= end; i++) {
    for (j = 0; j < 4; j++) {
      fprintf(fp, "\t\t\t\t %f %f %f", face4[i].p[j].x, face4[i].p[j].y,
	      face4[i].p[j].z);
      if (i == end && j == 3) {
	fprintf(fp, "\n\t\t\t ]\n");
      } else {
	fprintf(fp, ",\n");
      }
    }
  }
  fprintf(fp, "\t\t}\n");

  fprintf(fp, "\t\t color Color {\n\t\t\t color [\n");
  for (i = start; i <= end; i++) {
    for (j = 0; j < 4; j++) {
      fprintf(fp, "\t\t\t\t %f %f %f", face4[i].colour[j].r, 
	      face4[i].colour[j].g, face4[i].colour[j].b);
      if (i == end && j == 3) {
	fprintf(fp, "\n\t\t\t ]\n");
      } else {
	fprintf(fp, ",\n");
      }
    }
  }
  fprintf(fp, "\t\t}\n");
  
  fprintf(fp, "\t\t coordIndex [\n");
  for (i = start; i <= end; i++) {
    fprintf(fp, "\t\t\t %d, %d, %d, %d, -1", (i-start)*4, (i-start)*4 + 1,
	    (i-start)*4 + 2, (i-start)*4 + 3);
    if (i == end) {
      fprintf(fp, "\n\t\t\t ]\n");
    } else {
      fprintf(fp, ",\n");
    }
  }
  fprintf(fp, "\t\t solid FALSE\n");
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");

}

void doVRMLface4t(FILE *fp, int start, int end) {

  int i, j;

  /* write this texture if it's needed... */
  char texfname[128];
  sprintf(texfname, "vrml-tex%04d.tga", (int)face4t[start].textureid);
  static struct stat fsb;
  static FILE *texf;
  if (stat(texfname, &fsb)) {

    texf = fopen(texfname, "wb");
    if (!texf) {
      _s2error("(internal)", "failed to write texture in VRML output");
    }

    // format 11 = TGA with alpha channel
    int nx = 0, ny = 0;
    unsigned char *bits;
    bits = ss2gt(face4t[start].textureid, &nx, &ny);

    Write_Bitmap(texf, (BITMAP4 *)bits, nx, ny, 11);

    fclose(texf);
  }
  
  fprintf(fp, "DEF SHAPE%d Shape {\n\t appearance Appearance { \n", _s2x_shapecount++);
  fprintf(fp, "\t\t material Material {\n");
  fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", face4t[start].colour.r,
	  face4t[start].colour.g, face4t[start].colour.b);
  fprintf(fp, "\t\t\t transparency %f\n", 1.0 - face4t[start].alpha);

  // for textured objects, we generally don't want reflections or 
  // shading depending on lights
  fprintf(fp, "\t\t\t ambientIntensity 0.0\n");
  fprintf(fp, "\t\t\t diffuseColor 0.0 0.0 0.0\n");
  fprintf(fp, "\t\t\t specularColor 0.0 0.0 0.0\n");

  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t\t texture ImageTexture {\n");
  fprintf(fp, "\t\t\t url [ \"%s.png\" ] \n", texfname);
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");

  fprintf(fp, "\t geometry IndexedFaceSet {\n\t\t coord Coordinate {\n\t\t\t point [\n");
  for (i = start; i <= end; i++) {
    for (j = 0; j < 4; j++) {
      fprintf(fp, "\t\t\t\t %f %f %f", face4t[i].p[j].x, face4t[i].p[j].y,
	      face4t[i].p[j].z);
      if (i == end && j == 3) {
	fprintf(fp, "\n\t\t\t ]\n");
      } else {
	fprintf(fp, ",\n");
      }
    }
  }
  fprintf(fp, "\t\t}\n");

  fprintf(fp, "\t\t color Color {\n\t\t\t color [\n");
  for (i = start; i <= end; i++) {
    for (j = 0; j < 4; j++) {
      fprintf(fp, "\t\t\t\t %f %f %f", face4t[i].colour.r, 
	      face4t[i].colour.g, face4t[i].colour.b);
      if (i == end && j == 3) {
	fprintf(fp, "\n\t\t\t ]\n");
      } else {
	fprintf(fp, ",\n");
      }
    }
  }
  fprintf(fp, "\t\t}\n");
  
  fprintf(fp, "\t\t coordIndex [\n");
  for (i = start; i <= end; i++) {
    fprintf(fp, "\t\t\t %d, %d, %d, %d, -1", (i-start)*4, (i-start)*4 + 1,
	    (i-start)*4 + 2, (i-start)*4 + 3);
    if (i == end) {
      fprintf(fp, "\n\t\t\t ]\n");
    } else {
      fprintf(fp, ",\n");
    }
  }

  fprintf(fp, "\t\t texCoord TextureCoordinate {\n");
  //fprintf(fp, "\t\t\t point [1 0, 0 0, 0 1, 1 1]\n");
  fprintf(fp, "\t\t\t point [0 1, 1 1, 1 0, 0 0]\n");
  fprintf(fp, "\t\t }\n");
  /*
# added manually
	    texCoord TextureCoordinate {
	       point [1 0, 0 0, 0 1, 1 1]
            }
  */

  fprintf(fp, "\t\t solid FALSE\n");
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");

}


void doVRMLface3(FILE *fp, int start, int end) {

  int i, j;

  fprintf(fp, "DEF SHAPE%d Shape {\n\t appearance Appearance { \n", _s2x_shapecount++);
  fprintf(fp, "\t\t material Material {\n");
  fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", face3[start].colour[0].r,
	  face3[start].colour[0].g, face3[start].colour[0].b);
  fprintf(fp, "\t\t\t transparency %f\n", 0.0);

  fprintf(fp, "\t\t\t diffuseColor %f %f %f\n", face3[start].colour[0].r*0.7,
	  face3[start].colour[0].g*0.7, face3[start].colour[0].b*0.7);
  fprintf(fp, "\t\t\t ambientIntensity 0.250000\n");
  fprintf(fp, "\t\t\t specularColor 0.92 0.92 0.92\n");
  fprintf(fp, "\t\t\t emissiveColor 0.00000 0.00000 0.00000\n");
  fprintf(fp, "\t\t\t shininess 0.35000\n");
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");


  fprintf(fp, "\t geometry IndexedFaceSet {\n\t\t coord Coordinate {\n\t\t\t point [\n");
  for (i = start; i <= end; i++) {
    for (j = 0; j < 3; j++) {
      fprintf(fp, "\t\t\t\t %f %f %f", face3[i].p[j].x, face3[i].p[j].y,
	      face3[i].p[j].z);
      if (i == end && j == 2) {
	fprintf(fp, "\n\t\t\t ]\n");
      } else {
	fprintf(fp, ",\n");
      }
    }
  }
  fprintf(fp, "\t\t}\n");

  fprintf(fp, "\t\t normal Normal {\n\t\t\t vector[\n");
  for (i = start; i <= end; i++) {
    for (j = 0; j < 3; j++) {
      fprintf(fp, "\t\t\t\t %f %f %f", face3[i].n[j].x, face3[i].n[j].y,
	      face3[i].n[j].z);
      if (i == end && j == 2) {
	fprintf(fp, "\n\t\t\t ]\n");
      } else {
	fprintf(fp, ",\n");
      }
    }
  }
  fprintf(fp, "\t\t}\n");

  fprintf(fp, "\t\t coordIndex [\n");
  for (i = start; i <= end; i++) {
    fprintf(fp, "\t\t\t %d, %d, %d, -1", (i-start)*3, (i-start)*3 + 1,
	    (i-start)*3 + 2);
    if (i == end) {
      fprintf(fp, "\n\t\t\t ]\n");
    } else {
      fprintf(fp, ",\n");
    }
  }

  fprintf(fp, "\t\t normalIndex [\n");
  for (i = start; i <= end; i++) {
    fprintf(fp, "\t\t\t %d, %d, %d, -1", (i-start)*3, (i-start)*3 + 1,
	    (i-start)*3 + 2);
    if (i == end) {
      fprintf(fp, "\n\t\t\t ]\n");
    } else {
      fprintf(fp, ",\n");
    }
  }

  fprintf(fp, "\t\t solid FALSE\n");
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");

}

void doVRMLface3a(FILE *fp, int start, int end) {
  int i, j;

  fprintf(fp, "DEF SHAPE%d Shape {\n\t appearance Appearance { \n", _s2x_shapecount++);
  fprintf(fp, "\t\t material Material {\n");
  fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", face3a[start].colour[0].r,
  	  face3a[start].colour[0].g, face3a[start].colour[0].b);
  fprintf(fp, "\t\t\t transparency %f\n", 
	  1.0 - 0.33 * (face3a[start].alpha[0] + face3a[start].alpha[1] + face3a[start].alpha[2]));

  fprintf(fp, "\t\t\t diffuseColor %f %f %f\n", face3a[start].colour[0].r*0.7,
	  face3a[start].colour[0].g*0.7, face3a[start].colour[0].b*0.7);
  fprintf(fp, "\t\t\t ambientIntensity 0.250000\n");
  fprintf(fp, "\t\t\t specularColor 0.92 0.92 0.92\n");
  fprintf(fp, "\t\t\t emissiveColor 0.00000 0.00000 0.00000\n");
  fprintf(fp, "\t\t\t shininess 0.35000\n");

  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");


  fprintf(fp, "\t geometry IndexedFaceSet {\n\t\t coord Coordinate {\n\t\t\t point [\n");
  for (i = start; i <= end; i++) {

    for (j = 0; j < 3; j++) {
      fprintf(fp, "\t\t\t\t %f %f %f", face3a[i].p[j].x, face3a[i].p[j].y,
	      face3a[i].p[j].z);
      if (i == end && j == 2) {
	fprintf(fp, "\n\t\t\t ]\n");
      } else {
	fprintf(fp, ",\n");
      }
    }
  }
  fprintf(fp, "\t\t}\n");

#if (1)
  fprintf(fp, "\t\t normal Normal {\n\t\t\t vector[\n");
  for (i = start; i <= end; i++) {
    for (j = 0; j < 3; j++) {
      fprintf(fp, "\t\t\t\t %f %f %f", face3a[i].n[j].x, face3a[i].n[j].y,
	      face3a[i].n[j].z);
      if (i == end && j == 2) {
	fprintf(fp, "\n\t\t\t ]\n");
      } else {
	fprintf(fp, ",\n");
      }
    }
  }
  fprintf(fp, "\t\t}\n");
#endif

  fprintf(fp, "\t\t coordIndex [\n");
  for (i = start; i <= end; i++) {
    fprintf(fp, "\t\t\t %d, %d, %d, -1", (i-start)*3, (i-start)*3 + 1,
	    (i-start)*3 + 2);
    if (i == end) {
      fprintf(fp, "\n\t\t\t ]\n");
    } else {
      fprintf(fp, ",\n");
    }
  }

#if (1)
  fprintf(fp, "\t\t normalIndex [\n");
  for (i = start; i <= end; i++) {
    fprintf(fp, "\t\t\t %d, %d, %d, -1", (i-start)*3, (i-start)*3 + 1,
	    (i-start)*3 + 2);
    if (i == end) {
      fprintf(fp, "\n\t\t\t ]\n");
    } else {
      fprintf(fp, ",\n");
    }
  }
#endif

  fprintf(fp, "\t\t solid FALSE\n");
  fprintf(fp, "\t\t normalPerVertex TRUE\n");
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");

}

void doVRMLball(FILE *fp, int start, int end) {

  int lbi;
  for (lbi = start; lbi <= end; lbi++) {

    fprintf(fp, "Transform {\n");
    fprintf(fp, "  translation %f %f %f\n", ball[lbi].p.x,
	    ball[lbi].p.y, ball[lbi].p.z);

    fprintf(fp, "children DEF SHAPE%d Shape {\n\t appearance Appearance { \n", _s2x_shapecount++);
    fprintf(fp, "\t\t material Material {\n");
    fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", ball[lbi].colour.r,
	    ball[lbi].colour.g, ball[lbi].colour.b);
    fprintf(fp, "\t\t\t diffuseColor 0.2 0.2 0.2\n");
    fprintf(fp, "\t\t }\n");
    fprintf(fp, "\t}\n");

    fprintf(fp, "\t geometry Sphere { radius %f }\n", ball[lbi].r);
   
    fprintf(fp, "\t} \n");
    fprintf(fp, "}\n");
  } 

}

void doVRMLcone(FILE *fp, int start, int end) {

  XYZ Pcen, Ptip;
  double Crad;
  XYZ Pnorm;
  double ang;
  XYZ Yvec = {0., 1., 0.};

  int lbi;
  for (lbi = start; lbi <= end; lbi++) {

    // centre of cone
    Pcen = MidPoint(cone[lbi].p1, cone[lbi].p2);
    // unit vector from centre to tip
    if (cone[lbi].r1 > EPS) {
      Ptip = VectorSub(Pcen, cone[lbi].p2);
      Crad = cone[lbi].r1;
    } else {
      Ptip = VectorSub(Pcen, cone[lbi].p1);
      Crad = cone[lbi].r2;
    }
    Normalise(&Ptip);

    // normal that cone is rotated about:
    Pnorm = CrossProduct(Yvec, Ptip);
    // angle...
    ang = acos(DotProduct(Yvec, Ptip));
    
    fprintf(fp, "Transform {\n");
    fprintf(fp, "  translation %f %f %f\n", Pcen.x, Pcen.y, Pcen.z);
    fprintf(fp, "   rotation %f %f %f %f\n", Pnorm.x, Pnorm.y, Pnorm.z, ang);

    fprintf(fp, "children DEF SHAPE%d Shape {\n\t appearance Appearance { \n", _s2x_shapecount++);
    fprintf(fp, "\t\t material Material {\n");
    fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", cone[lbi].colour.r,
	    cone[lbi].colour.g, cone[lbi].colour.b);
    fprintf(fp, "\t\t\t diffuseColor 0.2 0.2 0.2\n");
    fprintf(fp, "\t\t }\n");
    fprintf(fp, "\t}\n");

    fprintf(fp, "\t geometry Cone { bottomRadius %f height %f }\n", 
	    Crad, VectorLength(cone[lbi].p1, cone[lbi].p2));
   
    fprintf(fp, "\t} \n");
    fprintf(fp, "}\n");
  } 
}

void doVRMLcylinder(FILE *fp, int start, int end) {

  XYZ Pcen, Ptip;
  double Crad;
  XYZ Pnorm;
  double ang;
  XYZ Yvec = {0., 1., 0.};

  int lbi;
  for (lbi = start; lbi <= end; lbi++) {

    // centre of cylinder
    Pcen = MidPoint(cone[lbi].p1, cone[lbi].p2);
    // unit vector from centre to one end
    Ptip = VectorSub(Pcen, cone[lbi].p2);
    Crad = cone[lbi].r1;
    Normalise(&Ptip);

    // normal that cone is rotated about:
    Pnorm = CrossProduct(Yvec, Ptip);
    // angle...
    ang = acos(DotProduct(Yvec, Ptip));
    
    fprintf(fp, "Transform {\n");
    fprintf(fp, "  translation %f %f %f\n", Pcen.x, Pcen.y, Pcen.z);
    fprintf(fp, "   rotation %f %f %f %f\n", Pnorm.x, Pnorm.y, Pnorm.z, ang);

    fprintf(fp, "children DEF SHAPE%d Shape {\n\t appearance Appearance { \n", _s2x_shapecount++);
    fprintf(fp, "\t\t material Material {\n");
    fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", cone[lbi].colour.r,
	    cone[lbi].colour.g, cone[lbi].colour.b);
    fprintf(fp, "\t\t\t diffuseColor 0.2 0.2 0.2\n");
    fprintf(fp, "\t\t }\n");
    fprintf(fp, "\t}\n");

    fprintf(fp, "\t geometry Cylinder { radius %f height %f }\n", 
	    Crad, VectorLength(cone[lbi].p1, cone[lbi].p2));
   
    fprintf(fp, "\t} \n");
    fprintf(fp, "}\n");
  }
}



void doVRMLlabel(FILE *fp, int start, int end) {
  
  int lbi, i;
  int nlinelist;
  XYZ linelist[300*MAXLABELLEN];

  for (lbi = start; lbi <= end; lbi++) {
    CreateLabelVector(label[lbi].s, label[lbi].p, label[lbi].right,
		      label[lbi].up, linelist, &nlinelist);
    
    fprintf(fp, "DEF SHAPE%d Shape {\n\t appearance Appearance { \n\t\t lineProperties LineProperties {\n", _s2x_shapecount++);
    fprintf(fp, "\t\t\t linewidthScaleFactor %f\n", 1.0);
    fprintf(fp, "\t\t }\n");
    fprintf(fp, "\t\t material Material {\n");
    fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", label[lbi].colour.r,
	    label[lbi].colour.g, label[lbi].colour.b);
    fprintf(fp, "\t\t }\n");
    fprintf(fp, "\t}\n");
    
    fprintf(fp, "\t geometry IndexedLineSet {\n\t\t coord Coordinate {\n\t\t\t point [\n");
    for (i = 0; i < nlinelist; i+=2) {
      if (i > 0) {
	fprintf(fp, ",\n");
      }
      fprintf(fp, "\t\t\t\t %f %f %f,", linelist[i].x, linelist[i].y,
	      linelist[i].z);
      fprintf(fp, "\t\t\t\t %f %f %f", linelist[i+1].x, linelist[i+1].y,
	      linelist[i+1].z);
    }
    fprintf(fp, "\n\t\t\t ]\n");
    fprintf(fp, "\t\t}\n");

    fprintf(fp, "\t\t coordIndex [\n");
    for (i = 0; i < nlinelist; i+=2) {
      if (i > 0) {
	fprintf(fp, ",\n");
      }
      fprintf(fp, "\t\t\t %d, %d, -1", i, i+1);
    }
    fprintf(fp, "\n\t\t\t ]\n");

    fprintf(fp, "\t\t }\n");
    fprintf(fp, "\t}\n");
    
  }

}

void doVRMLbboard(FILE *fp, int start, int end) {

  if (start != end) {
    _s2warn("doVRMLbboard", "start != end ... not supported yet!\n");
    return;
  }
  
  // loop from start to end would have to be here
  int hid = start;
  
  int i, j;

  // billboards are drawn at the origin then transformed in VRML
  fprintf(fp, "Transform {\n");
  fprintf(fp, "  translation %f %f %f\n", bboard[hid].p.x,
	  bboard[hid].p.y, bboard[hid].p.z);
  fprintf(fp, "  children [\n");
  
  fprintf(fp, "Billboard {\n");
  fprintf(fp, "  axisOfRotation 0 0 0\n");
  fprintf(fp, "  children [\n");

  /* write this texture if it's needed... */
  char texfname[128];
  sprintf(texfname, "vrml-tex%04d.tga", (int)bboard[hid].texid);
  static struct stat fsb;
  static FILE *texf;
  if (stat(texfname, &fsb)) {
    texf = fopen(texfname, "wb");
    if (!texf) {
      _s2error("(internal)", "failed to write texture in VRML output");
    }
    // format 11 = TGA with alpha channel
    int nx = 0, ny = 0;
    unsigned char *bits;
    bits = ss2gt(bboard[start].texid, &nx, &ny);
    Write_Bitmap(texf, (BITMAP4 *)bits, nx, ny, 11);
    fclose(texf);
  }
  
  fprintf(fp, "DEF SHAPE%d Shape {\n\t appearance Appearance { \n", _s2x_shapecount++);
  fprintf(fp, "\t\t material Material {\n");
  fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", bboard[hid].col.r,
	  bboard[hid].col.g, bboard[hid].col.b);
  fprintf(fp, "\t\t\t transparency %f\n", 1.0 - bboard[hid].alpha);

  // for textured objects, we generally don't want reflections or 
  // shading depending on lights
  fprintf(fp, "\t\t\t ambientIntensity 0.0\n");
  fprintf(fp, "\t\t\t diffuseColor 0.0 0.0 0.0\n");
  fprintf(fp, "\t\t\t specularColor 0.0 0.0 0.0\n");

  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t\t texture ImageTexture {\n");
  fprintf(fp, "\t\t\t url [ \"%s.png\" ] \n", texfname);
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");
  
  fprintf(fp, "\t geometry IndexedFaceSet {\n\t\t coord Coordinate {\n\t\t\t point [\n");
  //for (i = start; i <= end; i++) {
  i = hid; {
    for (j = 0; j < 4; j++) {
      float xoff = 0, yoff = 0;
      switch (j) {
      case 0:
	xoff = +1;
	yoff = -1;
	break;
      case 1:
	xoff = +1;
	yoff = +1;
	break;
      case 2:
	xoff = -1;
	yoff = +1;
	break;
      case 3:
	xoff = -1;
	yoff = -1;
	break;
      }
      //xoff /= 7.;
      //yoff /= 7.;
      xoff *= 7. * bboard[hid].size * bboard[hid].aspect;
      yoff *= 7. * bboard[hid].size;
      fprintf(fp, "\t\t\t\t %f %f %f", xoff + bboard[hid].offset.x, 
	      yoff + bboard[hid].offset.y, 0.0);
      if (j == 3) {
	fprintf(fp, "\n\t\t\t ]\n");
      } else {
	fprintf(fp, ",\n");
      }
    }
  }
  fprintf(fp, "\t\t}\n");

  fprintf(fp, "\t\t color Color {\n\t\t\t color [\n");
  //for (i = start; i <= end; i++) {
  i = hid; {
    for (j = 0; j < 4; j++) {
      fprintf(fp, "\t\t\t\t %f %f %f", bboard[i].col.r, 
	      bboard[i].col.g, bboard[i].col.b);
      if (j == 3) {
	fprintf(fp, "\n\t\t\t ]\n");
      } else {
	fprintf(fp, ",\n");
      }
    }
  }
  fprintf(fp, "\t\t}\n");
  
  fprintf(fp, "\t\t coordIndex [\n");
  //for (i = start; i <= end; i++) {
  i = hid; {
    fprintf(fp, "\t\t\t %d, %d, %d, %d, -1", (i-hid)*4, (i-hid)*4 + 1,
	    (i-hid)*4 + 2, (i-hid)*4 + 3);
    fprintf(fp, "\n\t\t\t ]\n");
  }
  
  fprintf(fp, "\t\t texCoord TextureCoordinate {\n");
  //fprintf(fp, "\t\t\t point [0 1, 1 1, 1 0, 0 0]\n");
  fprintf(fp, "\t\t\t point [1 0, 1 1, 0 1, 0 0]\n");
  fprintf(fp, "\t\t }\n");
  
  fprintf(fp, "\t\t solid FALSE\n");
  fprintf(fp, "\t\t }\n");
  fprintf(fp, "\t}\n");
  
  // and close billboard
  fprintf(fp, "  ]\n");
  fprintf(fp, "}\n\n");

  // and close Transform
  fprintf(fp, "  ]\n");
  fprintf(fp, "}\n\n");  
}


void doVRMLhandle(FILE *fp, int start, int end) {

  if (start != end) {
    _s2warn("doVRMLhandle", "start != end ... not supported yet!\n");
    return;
  }

  // loop from start to end would have to be here
  int hid = start;

  int i, j;

  // handles are billboards, which in VRML must be drawn at the origin
  // then translated to their correct location...
  fprintf(fp, "Transform {\n");
  fprintf(fp, "  translation %f %f %f\n", handle[hid].p.x,
	  handle[hid].p.y, handle[hid].p.z);
  fprintf(fp, "  children [\n");
  
  // one handle goes in one VRML Billboard construct
  fprintf(fp, "Billboard {\n");
  fprintf(fp, "  axisOfRotation 0 0 0\n");
  fprintf(fp, "  children [\n");

  _s2x_shapecount+=2; // two shapes per handle

  int this_count = handle[hid].id;

  // LOOP OVER OFF,ON HANDLE GEOMETRY
  int wh;
  for (wh = 0; wh < 2; wh++) {

    int this_texid = -1;
    COLOUR this_col = {0., 0., 0.};
    char this_suffix[10];

    switch (wh) {
    case 0:
      // handle off:
      this_texid = handle[hid].texid;
      this_col = handle[hid].col;
      strcpy(this_suffix, "-OFF");
      break;
    case 1:
      // handle on:
      this_texid = handle[hid].hitexid;
      this_col = handle[hid].hilite;
      strcpy(this_suffix, "-ON");
      break;
    }

    /* write this texture if it's needed... */
    char texfname[128];
    sprintf(texfname, "vrml-tex%04d.tga", (int)this_texid);
    static struct stat fsb;
    static FILE *texf;
    if (stat(texfname, &fsb)) {

      texf = fopen(texfname, "wb");
      if (!texf) {
	_s2error("(internal)", "failed to write texture for VRML output");
      }
      
      // format 11 = TGA with alpha channel
      int nx = 0, ny = 0;
      unsigned char *bits;
      bits = ss2gt(this_texid, &nx, &ny);

      Write_Bitmap(texf, (BITMAP4 *)bits, nx, ny, 11);
      
      fclose(texf);
    }
  
    fprintf(fp, "DEF HANDLE%d%s Shape {\n\t appearance Appearance { \n", 
	    this_count, this_suffix);
    fprintf(fp, "\t\t material Material {\n");
    fprintf(fp, "\t\t\t emissiveColor %f %f %f\n", this_col.r,
	    this_col.g, this_col.b);

    // hmmm, handles don't have an alpha value !!! ???
    //fprintf(fp, "\t\t\t transparency %f\n", 1.0 - handle[hid].alpha);
  
    // for textured objects, we generally don't want reflections or 
    // shading depending on lights
    fprintf(fp, "\t\t\t ambientIntensity 0.0\n");
    fprintf(fp, "\t\t\t diffuseColor 0.0 0.0 0.0\n");
    fprintf(fp, "\t\t\t specularColor 0.0 0.0 0.0\n");
    
    fprintf(fp, "\t\t }\n");
    fprintf(fp, "\t\t texture ImageTexture {\n");
    fprintf(fp, "\t\t\t url [ \"%s.png\" ] \n", texfname);
    fprintf(fp, "\t\t }\n");
    fprintf(fp, "\t}\n");
    
    fprintf(fp, "\t geometry IndexedFaceSet {\n\t\t coord Coordinate {\n\t\t\t point [\n");
    //for (i = hid; i <= end; i++) {
    i = hid; {
      for (j = 0; j < 4; j++) {
	float xoff = 0, yoff = 0;
	switch (j) {
	case 0:
	  xoff = +1;
	  yoff = -1;
	  break;
	case 1:
	  xoff = +1;
	  yoff = +1;
	  break;
	case 2:
	  xoff = -1;
	  yoff = +1;
	  break;
	case 3:
	  xoff = -1;
	  yoff = -1;
	  break;
	}
	//xoff /= 7.;
	//yoff /= 7.;
	xoff *= handle[hid].size * 3.0;
	yoff *= handle[hid].size * 3.0;
	fprintf(fp, "\t\t\t\t %f %f %f", xoff, yoff, 0.0);
	if (j == 3) {
	  fprintf(fp, "\n\t\t\t ]\n");
	} else {
	  fprintf(fp, ",\n");
	}
      }
    }
    fprintf(fp, "\t\t}\n");
    
    fprintf(fp, "\t\t color Color {\n\t\t\t color [\n");
    //for (i = hid; i <= end; i++) {
    i = hid; {
      for (j = 0; j < 4; j++) {
	fprintf(fp, "\t\t\t\t %f %f %f", this_col.r, 
		this_col.g, this_col.b);
	if (j == 3) {
	  fprintf(fp, "\n\t\t\t ]\n");
	} else {
	  fprintf(fp, ",\n");
	}
      }
    }
    fprintf(fp, "\t\t}\n");
    
    fprintf(fp, "\t\t coordIndex [\n");
    //for (i = hid; i <= end; i++) {
    i = hid; {
      fprintf(fp, "\t\t\t %d, %d, %d, %d, -1", (i-hid)*4, (i-hid)*4 + 1,
	      (i-hid)*4 + 2, (i-hid)*4 + 3);
      fprintf(fp, "\n\t\t\t ]\n");
    }

    fprintf(fp, "\t\t texCoord TextureCoordinate {\n");
    fprintf(fp, "\t\t\t point [0 1, 1 1, 1 0, 0 0]\n");
    fprintf(fp, "\t\t }\n");
    
    fprintf(fp, "\t\t solid FALSE\n");
    fprintf(fp, "\t\t }\n");
    fprintf(fp, "\t}\n");
    
  } // complete loop of HANDLE -OFF,-ON

  // and close billboard
  fprintf(fp, "  ]\n");
  fprintf(fp, "}\n\n");

  // and close Transform
  fprintf(fp, "  ]\n");
  fprintf(fp, "}\n\n");  


}

//extern void cppWritePRC(void);
void writePRC(void) {
  static int beenhere = 0;
  //static void (*)(void) writePRCfunc = NULL;
  static void(*writePRCfunc)(void) = NULL;
  if (!beenhere) {
    // attempt to load module for PRC export
    char full_driver_name[400];
    char *tmpchr = getenv("S2PATH");
    char *tmp2chr = getenv("S2ARCH");
    char *driver = getenv("S2PLOT_PRCDRIVER");
    sprintf(full_driver_name, "%s/%s/%s.so", tmpchr, tmp2chr, driver);
    void *sdl_library = dlopen(full_driver_name, RTLD_LAZY);
    if (sdl_library == NULL) {
      // report error
      fprintf(stderr, "Failed to load %s module for PRC output\n", driver);
      fprintf(stderr, "dlerror: %s\n", dlerror());
      beenhere = -1;
      return;
    } else {
      void *prcfunc = dlsym(sdl_library, "prcwriter");
      if (prcfunc == NULL) {
	fprintf(stderr, "Cannot initialise PRC writer\n");
	fprintf(stderr, "dlerror: %s\n", dlerror());
	beenhere = -1;
	return;
      }
      writePRCfunc = (void (*)(void))prcfunc;
    }
    beenhere = 1;
  }

  if (beenhere < 0) {
    fprintf(stderr, "The PRC module is not loaded, so I cannot export PRC sorry.\n");
  } else {
    if (writePRCfunc) {
      fprintf(stderr, "S2PLOT PRC export module:\n");
      writePRCfunc();
    } else {
      fprintf(stderr, "Unable to call PRC write module.  Don't know why!\n");
    }
  }
  //cppWritePRC();
}


#endif

#if defined(BUILDING_S2PLOT)
void ss2wtga(char *fname) {
  if (options.stereo == ACTIVESTEREO) {
    WindowDump(fname, options.screenwidth, options.screenheight, 
	       TRUE, 12);
  } else {
    WindowDump(fname, options.screenwidth, options.screenheight,
	       FALSE, 12);
  }
}

unsigned char *ss2gpix(unsigned int *width, unsigned int *height) {
  *width = options.screenwidth;
  *height = options.screenheight;
  unsigned char *image = (unsigned char *)malloc(*width * *height * 3);
  glFinish();
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_BACK_LEFT);
  glReadPixels(0, 0, *width, *height, GL_RGB, GL_UNSIGNED_BYTE, image);
  return image;
}

void _s2priv_readEvents(char *fname) {

  FILE *fptr = NULL;
  fptr = fopen(fname, "r");
  if (!fptr) {
    _s2warn("_s2priv_readEvents", "cannot read S2PLOT_EVENTFILE");
    return;
  }
  
#define RELENGTH 80
  char line[RELENGTH];
  char type;
  double when;
  char data[RELENGTH];
  while (fgets(line, RELENGTH, fptr)) {
    if (line[0] == '#') {
      continue;
    }
    sscanf(line, "%c %lf %s", &type, &when, data);
    switch (type) {
    case 'k':
      _s2_events = (_S2EVENT *)realloc(_s2_events, 
				       (_s2_nevents+1)*sizeof(_S2EVENT));
      _s2_events[_s2_nevents].t = when;
      _s2_events[_s2_nevents].type = ASCII_KEY_EVENT;
      _s2_events[_s2_nevents].data = (char *)malloc((strlen((char *)data) + 1) * sizeof(char));
      strcpy((char *)(_s2_events[_s2_nevents].data), (char *)data);
      _s2_events[_s2_nevents].consumed = 0;
      _s2_nevents++;
      break;

    default:
      _s2warn("_s2priv_readEvents", "unknown event type ignored");
      break;

    }
  }
  fclose(fptr);
  fprintf(stderr, "read %d events\n", _s2_nevents);
}

#endif


#if defined(BUILDING_VIEWER)
#include "opengllib.c"
#include "bitmaplib.c"
#include "paulslib.c"
#include "hiddenMouseCursor.c"
#endif

#if defined(S2_NO_S2GEOMVIEWER)
unsigned int _s2priv_setupTexture(int width, int height, 
				  BITMAP4 *bitmap, int usemipmaps) {

#if defined(BUILDING_S2PLOT)
  if (_s2_devcap & _S2DEVCAP_NOCOLOR) {
    // desaturate the bitmap data
    int i, j;
    long idx = 0;
    float sum;
    for (i = 0; i < width; i++) {
      for (j = 0; j < height; j++, idx++) {
	sum = (bitmap[idx].r + bitmap[idx].g + bitmap[idx].b) * 0.33;
	bitmap[idx].r = bitmap[idx].g = bitmap[idx].b = sum;
      }
    }
  }
#endif
  
  
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

  /* assume all went well - possibly a bad assumption */
  _s2_ctext = (_S2CACHEDTEXTURE *)realloc(_s2_ctext, (_s2_ctext_count+1) *
					  sizeof(_S2CACHEDTEXTURE));
  _s2_ctext[_s2_ctext_count].width = width;
  _s2_ctext[_s2_ctext_count].height = height;
  _s2_ctext[_s2_ctext_count].depth = 0;
  _s2_ctext[_s2_ctext_count].bitmap = bitmap;
  _s2_ctext[_s2_ctext_count].id = id;
  _s2_ctext_count++;
  return (unsigned int)id;
}

#if defined(S2_3D_TEXTURES)
/* 3d version */
unsigned int _s2priv_setupTexture3d(int width, int height, int depth,
				    BITMAP4 *bitmap, int usemipmaps) {
#if defined(BUILDING_S2PLOT)
  if (_s2_devcap & _S2DEVCAP_NOCOLOR) {
    // desaturate the bitmap data
    int i, j, k;
    long idx = 0;
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

  /* assume all went well - possibly a bad assumption */
  _s2_ctext = (_S2CACHEDTEXTURE *)realloc(_s2_ctext, (_s2_ctext_count+1) *
					   sizeof(_S2CACHEDTEXTURE));
  _s2_ctext[_s2_ctext_count].width = width;
  _s2_ctext[_s2_ctext_count].height = height;
  _s2_ctext[_s2_ctext_count].depth = depth;
  _s2_ctext[_s2_ctext_count].bitmap = bitmap;
  _s2_ctext[_s2_ctext_count].id = id;

  _s2_ctext_count++;
  return (unsigned int)id;
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
    //glDeleteTextures(1, (GLuint *)(&texid));
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
#endif

#if defined(BUILDING_S2PLOT)

XYZ _s2_world2device(XYZ world) {
  XYZ retval;
  retval.x = _S2WORLD2DEVICE(world.x, _S2XAX);
  retval.y = _S2WORLD2DEVICE(world.y, _S2YAX);
  retval.z = _S2WORLD2DEVICE(world.z, _S2ZAX);
  return retval;
}
XYZ _s2_world2device_so(XYZ world) {
  XYZ retval;
  retval.x = _S2WORLD2DEVICE_SO(world.x, _S2XAX);
  retval.y = _S2WORLD2DEVICE_SO(world.y, _S2YAX);
  retval.z = _S2WORLD2DEVICE_SO(world.z, _S2ZAX);
  return retval;
}

XYZ _s2_device2world(XYZ device) {
  XYZ retval;
  retval.x = _S2DEVICE2WORLD(device.x, _S2XAX);
  retval.y = _S2DEVICE2WORLD(device.y, _S2YAX);
  retval.z = _S2DEVICE2WORLD(device.z, _S2ZAX);
  return retval;
}
void zs2debug(int debug) {
  options.debug = debug;
}
void zs2bufswap(int bufswap) {
  _s2_bufswap = bufswap;
}


#endif

void loadDevices(char *drivername) {
  char full_driver_name[400];
  char *tmpchr = getenv("S2PATH");
  char *tmp2chr = getenv("S2ARCH");
  sprintf(full_driver_name, "%s/%s/%s.so", tmpchr, tmp2chr, drivername);

  //fprintf(stderr, "opening %s ...\n", full_driver_name);
  void* sdl_library = dlopen(full_driver_name, RTLD_LAZY);
  if(sdl_library == NULL) {
    // report error ...
    fprintf(stderr, "Failed to load %s device driver\n", drivername);
    fprintf(stderr, "%s\n", dlerror());
    exit(-1);
  } else {
    char method_name[64];
    sprintf(method_name, "prep_%s", drivername);
    void* initializer = dlsym(sdl_library, method_name);
    if(initializer == NULL) {
      // report error ...
      fprintf(stderr, "Cannot initialise %s device driver\n", drivername);
      exit(-1);
    } else {
      // cast initializer to its proper type and use
      //void(*callfn)(void) = (void (*)(void))initializer;
      //(*callfn)();
      void(*callfn)(OPTIONS *) = (void (*)(OPTIONS *))initializer;
      (*callfn)(&options);
    }
    
    sprintf(method_name, "draw_%s", drivername);
    initializer = dlsym(sdl_library, method_name);
    if (initializer == NULL) {
      // report error ...
      fprintf(stderr, "Cannot find draw method in %s device driver\n", 
	      drivername);
      exit(-1);
    } else {
      _device_draw = (void (*)(CAMERA))initializer;
    }

    sprintf(method_name, "resize_%s", drivername);
    initializer = dlsym(sdl_library, method_name);
    if (initializer == NULL) {
      // not a critical error - no resize func no problem!
      _device_resize = NULL;
    } else {
      _device_resize = (void (*)(void))initializer;
    }
    
    sprintf(method_name, "keybd_%s", drivername);
    initializer = dlsym(sdl_library, method_name);
    if (initializer == NULL) {
      // not a critical error - no keybd func no problem!
      _device_keybd = NULL;
    } else {
      _device_keybd = (int (*)(char))initializer;
    }
  }  
}

XYZ _s2priv_pmin(void) {
  return pmin;
}
XYZ _s2priv_pmax(void) {
  return pmax;
}
