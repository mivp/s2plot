/* s2layer.c
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
 * $Id: s2layer.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 * s2layer.c: initialisation and drawing routines for s2plot when embedded
 *            in a non-glut user interface but with normal OpenGL. Many
 * FIXME's to attend to.
 */

#include <stdio.h>


#if defined(S2DARWIN)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "s2geomviewer.c"

#include "s2globals.h"
#include "geomviewer.h"
#include "s2plot.h"
#include "s2privfn.h"

#include "bitmaplib.c"
#include "s2plot.c"
#include "paulslib.c"
#include "rainbow.c"
#include "hotiron.c"
#include "opengllib.c"
#include "s2glu.c"
#include "misc.c"

void HandleDisplay(void);
void s2setWH(int w, int h);
void inits2plot(void);
void finals2plot(void);

// external - specific user code
void s2main(void);



int _s2_landscape; // landscape mode?
int _s2_drawing_enabled; 
float _s2_diagonal; // screen scale used for handles, billboards etc.
int _s2_viewport[4];
unsigned short _s2_handling; // are we interacting with a handle?
int _s2_which_handle; // which handle are we interacting with?
float _s2_handle_base_scx, _s2_handle_base_scy; // where handle interact started
unsigned short _s2_handle_moved; 

void s2setWH(int w, int h) {
    options.screenwidth = w;
    options.screenheight = h;
}

void inits2plot(void) {
    
  /* "current" geometry */
  nball = 0; ball = NULL;
  nballt = 0; ballt = NULL;
  ndisk = 0; disk = NULL;
  ncone = 0; cone = NULL;
  ndot = 0; dot = NULL;
  nline = 0; line = NULL;
  nface3 = 0; face3 = NULL;
  nface4 = 0; face4 = NULL;
  nface4t = 0; face4t = NULL;
  nlabel = 0; label = NULL;
  nhandle = 0; handle = NULL;
  nbboard = 0; bboard = NULL;
  nbbset = 0; bbset = NULL;
  nface3a = 0; face3a = NULL;
  ntrdot = 0; trdot = NULL;
#if defined(S2_3D_TEXTURES)
  ntexpoly3d = 0; texpoly3d = NULL;
#endif
  ntexmesh = 0; texmesh = NULL;

  /* "static" geometry */
  nball_s = 0; ball_s = NULL;
  nballt_s = 0; ballt_s = NULL;
  ndisk_s = 0; disk_s = NULL;
  ncone_s = 0; cone_s = NULL;
  ndot_s = 0; dot_s = NULL;
  nline_s = 0; line_s = NULL;
  nface3_s = 0; face3_s = NULL;
  nface4_s = 0; face4_s = NULL;
  nface4t_s = 0; face4t_s = NULL;
  nlabel_s = 0; label_s = NULL;
  nhandle_s = 0; handle_s = NULL;
  nbboard_s = 0; bboard_s = NULL;
  nbbset_s = 0; bbset_s = NULL;
  nface3a_s = 0; face3a_s = NULL;
  ntrdot_s = 0; trdot_s = NULL;
#if defined(S2_3D_TEXTURES)
  ntexpoly3d_s = 0; texpoly3d_s = NULL;
#endif
  ntexmesh_s = 0; texmesh_s = NULL;

  /* "dynamic" geometry */
  nball_d = 0; ball_d = NULL;
  nballt_d = 0; ballt_d = NULL;
  ndisk_d = 0; disk_d = NULL;
  ncone_d = 0; cone_d = NULL;
  ndot_d = 0; dot_d = NULL;
  nline_d = 0; line_d = NULL;
  nface3_d = 0; face3_d = NULL;
  nface4_d = 0; face4_d = NULL;
  nface4t_d = 0; face4t_d = NULL;
  nlabel_d = 0; label_d = NULL;
  nhandle_d = 0; handle_d = NULL;
  nbboard_d = 0; bboard_d = NULL;
  nbbset_d = 0; bbset_d = NULL;
  nface3a_d = 0; face3a_d = NULL;
  ntrdot_d = 0; trdot_d = NULL;
#if defined(S2_3D_TEXTURES)
  ntexpoly3d_d = 0; texpoly3d_d = NULL;
#endif
  ntexmesh_d = 0; texmesh_d = NULL;

    _s2_landscape = 0;
    _s2_drawing_enabled = 1;
    _s2_diagonal = 0;
    
    _s2_handling = 0;
    _s2_which_handle = -1;
    _s2_handle_base_scx = _s2_handle_base_scy = 0;
    _s2_handle_moved = 0;
    
    /* define the device coordinate system bounds */
    _s2devicemin[_S2XAX] = -1.0;
    _s2devicemax[_S2XAX] = +1.0;
    _s2devicemin[_S2YAX] = -1.0;
    _s2devicemax[_S2YAX] = +1.0;
    _s2devicemin[_S2ZAX] = -1.0;
    _s2devicemax[_S2ZAX] = +1.0;
    
    _s2axismin[_S2XAX] = _s2axismin[_S2YAX] = _s2axismin[_S2ZAX] = -1.0;
    _s2axismax[_S2XAX] = _s2axismax[_S2YAX] = _s2axismax[_S2ZAX] = 1.0;

    /* default device capabilities */
    _s2_devcap = _S2DEVCAP_SELECTION;
    
    // Default options
    COLOUR black = {0,0,0};
    XYZ unity = {1,1,1},zero = {0,0,0};
    options.debug             = FALSE;
    options.stereo            = NOSTEREO;   
    options.fullscreen        = FALSE;
    options.dometype          = NODOME;
    options.showdomeintensity = TRUE;
    options.rendermode        = SHADE_DIFFUSE;
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
    options.autospin.x = 1.01;
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
    options.screenwidth = 440;
    options.screenheight = 253;
    
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
    
    int ifullscreen = 0;
    int istereo = 0;
    
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
    } else if (istereo) {
        /* unknown stereo mode */
        return; //(0);
    }
    
    if (ifullscreen == 1) {
        options.fullscreen = TRUE;
    } else if (ifullscreen) {
        /* unknown screen mode */
        return; //(0);
    }
    
    
    float size = 3.0;
    camera.focallength = 1.75 * size;
    camera.aperture = 45;
    camera.eyesep = size / 3.0;
    camera.vp.x = camera.vp.y = camera.vp.z = 0.0;
    camera.vp.z += 1.75 * size;
    camera.vu.x = 0;
    camera.vu.y = 1.0;
    camera.vu.z = 0.0;
    
    pmin.x = pmin.y = pmin.z = -1.0;
    pmax.x = pmax.y = pmax.z = 1.0;
    pmid.x = pmid.y = pmid.z = 0.0;
    rangemin = 1e32;
    rangemax = -1e32;
    
    camera.vd.x = pmid.x - camera.vp.x;
    camera.vd.y = pmid.y - camera.vp.y;
    camera.vd.z = pmid.z - camera.vp.z;
    
    camera.pr.x = camera.pr.y = camera.pr.z = 0.0;
    
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
        fprintf(stderr, "failed to allocate colormap memory\n");
        exit(-1);
    }
    int i;
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
        fprintf(stderr, "failed to allocate colormap memory\n");
        exit(-1);
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
    
    /* object mode control */
    _s2_object_trans.x = _s2_object_trans.y = _s2_object_trans.z = 0.0;

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
    _s2_skiplock = 0;
    _s2_remoteport = 0;
    
    /* handle are not initially visible (toggle with Shift-S key) */
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
    
    /* get screen constraints from environment or default */
    _s2_scr_x1 = 0.;
    _s2_scr_x2 = 1.;
    _s2_scr_y1 = 0.;
    _s2_scr_y2 = 1.;
    
    _s2_bg_texid = -1;
    
    _s2_bg_clear = 1;

    // FIXME: there is a devious bug somewhere in the initialisation of panels
    // (I think) that expresses itself as a BAD EXC on *some* platforms, not all,
    // e.g. iOS using Juicer.
    
    _s2_panels = NULL;
    _s2_npanels = 0;
    _s2_activepanel = xs2ap(_s2_scr_x1, _s2_scr_y1, _s2_scr_x2, _s2_scr_y2);

    _s2_panels[0].active = 1;

    
    _s2_activepanelframecolour.r = 1.;
    _s2_activepanelframecolour.g = 1.;
    _s2_activepanelframecolour.b = 1.;
    _s2_panelframecolour.r = 0.2;
    _s2_panelframecolour.g = 0.2;
    _s2_panelframecolour.b = 0.2;
    _s2_panelframewidth = 1.0;
    
    _s2_nVRMLnames = 1;
    _s2_VRMLnames = (char **)malloc(sizeof(char *));
    _s2_VRMLnames[0] = (char *)malloc(MAXVRMLLEN*sizeof(char));
    strncpy(_s2_VRMLnames[0], "ANON", MAXVRMLLEN);
    _s2_VRMLnames[0][MAXVRMLLEN-1] = '\0';
    _s2_currVRMLidx = 0;
    
    
    /* no need to "push" globals onto the zeroth panel yet: only do
     * this when panel is changed by user, or when about to enter
     * drawing section of code. */
}

void finals2plot(void) {
    
    static int beenhere = 0;
    if (beenhere) {
        _s2warn("s2open", "Not executing s2show [been here before!]");
        return;
    }
    beenhere = 1;
    
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

}

void CleanExit(void) {
    exit(-1);
}

#if defined(FIXME)
// s2winGet hacked for now!!!
#endif
int s2winGet(int which) {
    switch(which) {
        case S2_SCREEN_WIDTH:
            return 2560;
            break;
        case S2_SCREEN_HEIGHT:
            return 1440;
            break;
        case S2_WINDOW_Y:
            return 1;
            break;
        case S2_WINDOW_WIDTH:
            return 400;
            break;
        case S2_WINDOW_HEIGHT:
            return 200;
            break;
        default:
            return 0;
            break;
    }
}


void _glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    static float sum;
    if (_s2_devcap & _S2DEVCAP_NOCOLOR) {
        // desaturate
        sum = (red + green + blue) * 0.33;
        glColor4f(sum, sum, sum, alpha);
    } else {
        glColor4f(red, green, blue, alpha);    
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




void CreateProjection(int eye);
void HandleDisplay(void);
void MakeLighting(void);
void MakeMaterial(void);
void MakeGeometry(int doupdate, int doscreen, int eye);
void drawView(char *projinfo, double camsca);



/* Display callback routine.  Thoroughly re-worked July/August 2007 to
 * now use one or more calls to "drawView" function for all non-warped
 * devices, and to account for addition of panels and screen coordinate
 * geometry.  Still needs some minor tidying and hopefully compression
 * of the dome code.
 */
void HandleDisplay(void) {
#if defined(FIXME)
    MTX_LOCK(&mutex);
#endif
    
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
#if defined(FIXME)
        _s2x_ywinpos = s2winGet(S2_WINDOW_Y);
#endif
#endif
    }
    
    if (tbegin < 0.) {
        tbegin = GetRunTime();
    }
    
#if defined(FIXME)
    if (_device_resize) {
        int curwiny = s2winGet(S2_WINDOW_Y);
        if (curwiny != _s2x_ywinpos) {
            (_device_resize)();
        }
        _s2x_ywinpos = curwiny;
    }
#endif
    
    /* 1. set the current time, then loop over panels, updating camera
     *    position and creating dynamic geometry lists via callbacks.
     */
    
#if defined(BUILDING_S2PLOT)
    unsigned int uj;
    
    /* loop over the panels */
    double tm = GetRunTime();
    
    /* push expired events into the queue */
#if defined(FIXME)
    for (i = 0; i < _s2_nevents; i++) {
        if (!_s2_events[i].consumed && (_s2_fadestatus == 2) && 
            (tbegin + _s2_fadetime + _s2_events[i].t < tm)) {
            switch (_s2_events[i].type) {
                case ASCII_KEY_EVENT:
                    for (uj = 0; uj < strlen((char *)(_s2_events[i].data)); uj++) {
                        _s2_skiplock = 1;
                        HandleKeyboard(((char *)_s2_events[i].data)[uj], 0, 0);
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
#endif
    
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
#if defined(FIXME)
        if (options.autopilot) {
            options.autopilot = AutoPilot(1,"");
        }
#endif
        
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
#if defined(FIXME)
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
        
        
    } else
#endif
    {
        /* it doesn't get any easier than this... :-) */
        
        /* SINGLE SCREEN MONO */
        
        glEnable(GL_MULTISAMPLE);
        
#if (1)
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
#endif
        
        /* clear the buffer */
        glDrawBuffer(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /* just draw the centre view */
        glViewport(0, 0, options.screenwidth, options.screenheight);
        drawView("c", 0.);
#if defined(FIXME)
        DrawExtras();
#endif
        
    }
    
#if defined(FIXME)
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
    
#endif // FIXME
    
#if defined(BUILDING_S2PLOT)
    if (_s2_bufswap) {
#endif
#if defined(FIXME)
        s2winSwapBuffers();
#endif
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
    
#if defined(FIXME)
    MTX_ULCK(&mutex);
#endif
}



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
    //fprintf(stderr, "viewport is: %d %d %d %d\n", x0, y0, dx, dy);
    
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
        
#if defined(FIXME)
#if defined(BUILDING_S2PLOT)
        /* get projections needed for coordinate trans */
        glGetDoublev(GL_MODELVIEW_MATRIX, _s2_dragmodel);
        glGetDoublev(GL_PROJECTION_MATRIX, _s2_dragproj);
        glGetIntegerv(GL_VIEWPORT, _s2_dragview);
#endif    
#endif
        
        MakeLighting();
        MakeMaterial();
        
        //fprintf(stderr, "camera.vp = %f %f %f\n", camera.vp.x, camera.vp.y, camera.vp.z);
        
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

// Interface state
INTERFACESTATE interfacestate;


// Global Materials, set through the materials form 
GLfloat specularcolour[4] = {1.0,1.0,1.0,1.0};
GLfloat shininess[1]      = {100.0};
GLfloat emission[4]       = {0.0,0.0,0.0,1.0};
GLfloat transparency      = 1.01;

float targetRoC_x = 0., targetRoC_y = 0.;
float currRoC_x = 0., currRoC_y = 0.;

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
    
    // modification DBARNES March 2011: lighting must be disabled otherwise backgrounds
    // change as scene rotated
    //--glEnable(GL_LIGHTING);
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

void _s2priv_pushPanelToGlobals(int panelid);


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
  //fprintf(stderr, "in MakeGeometry: doscreen=%d, dynamic=%d\n", doscreen, _s2_dynamicEnabled);
    int i = 0,j = 0;
    XYZ linelist[300*MAXLABELLEN];
    XYZ normal = {1.f,0.f,0.f};
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
    COLOUR white = {1,1,1};
    
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
        
#if defined(BUILDING_S2PLOT) && defined(FIXME)
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
        
#if defined(BUILDING_S2PLOT) && defined(FIXME)
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
        
#if defined(BUILDING_S2PLOT) && defined(FIXME)
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
      //fprintf(stderr, "ndot = %d, (dynamic = %d, screen = %d, doingScreen = %s)\n", ndot, _s2_dynamicEnabled, doscreen, _s2_doingScreen);
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
		  fprintf(stderr, "dot[%d].whichscreen = %s\n", i, dot[i].whichscreen);
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
                                       line[i].colour[j].b, transparency);
                            glVertex3f(vtx, vty, vtz);
                        }		
                    } 
                } else if (!strlen(line[i].whichscreen)) 
#endif
                {
                    for (j=0;j<2;j++) {
                        _glColor4f(line[i].colour[j].r,line[i].colour[j].g,line[i].colour[j].b,transparency);
                        glVertex3f(line[i].p[j].x,line[i].p[j].y,line[i].p[j].z);
                    }
                }
            }  
            glEnd();
            glLineWidth(options.linescale);
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
            
#if defined(BUILDING_S2PLOT) && defined(FIXME)
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

        if (ntexmesh > 0) {
	  fprintf(stderr, "No drawing of texmesh type yet!\n");
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
        
#if defined(FIXME)
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
#endif
        
// CONDITIONAL added DGB 20141101 - definitely needed, oglcb was
// being called three times per eye/screen.
if (_s2_dynamicEnabled && !doscreen) {
        /* and call the direct OpenGL callback if one is present */
        if (_s2_oglcb) {
            _s2_oglcb();
        }
 }        
        /* call the entry/exit fade in routine */
        //_s2_fadeinout();
#endif
        
    }

    

