/* s2disp.c
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
 * $Id: s2disp.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#if defined(S2DARWIN) || defined(S2LINUX)

#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

void CameraHome(int);
void MakeGeometry(int, int);
void HandleDisplay(void);
void _s2debug(char *fn, char *messg, ...);

#if defined(__cplusplus)
extern "C" {
#endif

extern int _s2_skip;
extern int _s2_cameraset;
#if defined(S2LINUX)
void glutMainLoopEvent(void);
#elif defined(S2DARWIN)
void glutCheckLoop(void);
#endif
void _s2priv_CameraSet(void);
#define HOME 1
#define FALSE 0
#define TRUE 1

#if defined(BUILDING_S2PLOT)
extern int _s2_activepanel;
extern int _s2_npanels;
void xs2cp(int);
extern int _s2_skip;
#endif
  void s2disp(int, int);
#if defined(__cplusplus)
} // extern "C" {
#endif

/* Draw the scene and enter interactive mode.  Based on argument, return
 * immediately after drawing view (0), return after arg seconds, or 
 * only return when user hits 'TAB' key.  If restorecamera > 0, then 
 * the camera position will be returned to "home".
 */

/* This is the "real" version.  
 */
void s2disp(int idelay, int irestorecamera) {
#if defined(BUILDING_S2PLOT)
  int waspanel = _s2_activepanel;
  int spid;
  for (spid = 0; spid < _s2_npanels; spid++) {
    //fprintf(stderr, "s2disp: multi-panel support ALPHA\n");
    _s2debug("s2disp", "multi-panel support in s2disp is not thoroughly tested");
    // solve below by creating s2plot function to provide activeness of
    // a given panel ... means S2PANL struct not needed here
    /*
    if (!_s2_panels[spid].active) {
      continue;
    }
    */
    xs2cp(spid);
#endif

    if (irestorecamera) {
      CameraHome(HOME);
      // options.camerahome = camera;
      /* moved to HandleDisplay function 
	 if (_s2_cameraset) {
	 _s2priv_CameraSet();
	 }
      */
    }
    MakeGeometry(TRUE, FALSE);
#if defined(BUILDING_S2PLOT)
  }
  xs2cp(waspanel);
#endif

  struct timeval starttime, currtime;
  gettimeofday(&starttime, NULL);

  _s2_skip = 0;
  
  do {
#if defined(S2LINUX) 
    glutMainLoopEvent();
#elif defined(S2DARWIN)
    glutCheckLoop();
#endif
    HandleDisplay();
    usleep(20);

    if (idelay == 0) { /* just do one loop */
      _s2_skip = 1;
    } else if (idelay > 0) { /* check time elapsed */
      gettimeofday(&currtime, NULL);
      if ((currtime.tv_sec - starttime.tv_sec) > idelay) {
	_s2_skip = 1;
      }
    }
  } while (!_s2_skip);

}

extern int (*_s2_user_keys)(unsigned char *);
char _s2disptk_key;
int _s2disptk_return = 1;
int (*_s2disptk_userkcb)(unsigned char *);
int _s2disptk_kcb(unsigned char *key) {
  if ((_s2disptk_key) && (_s2disptk_key == *key)) {
    _s2disptk_return = 1;
    return 1; // consume this keypress
  } else if (_s2disptk_userkcb) {
    return _s2disptk_userkcb(key);
  } 
  return 0; // not consumed
}

/* Draw the scene and enter interactive mode.  Based on arguments,
 * return immediately after drawing view (timeout == 0), return after
 * fabs(timeout) seconds of inactivity (timeout < 0: NYI), or return after
 * timeout seconds regardless of activity.  If key is non-zero, return
 * on press of proposed key. If restorecamera > 0, then the camera
 * position will be returned to "home".
 */
void s2disptk(float timeout, char key, int irestorecamera) {
#if defined(BUILDING_S2PLOT)
  int waspanel = _s2_activepanel;
  int spid;
  for (spid = 0; spid < _s2_npanels; spid++) {
    //fprintf(stderr, "s2disp: multi-panel support ALPHA\n");
    _s2debug("s2disp", "multi-panel support in s2disp is not thoroughly tested");
    // solve below by creating s2plot function to provide activeness of
    // a given panel ... means S2PANL struct not needed here
    /*
    if (!_s2_panels[spid].active) {
      continue;
    }
    */
    xs2cp(spid);
#endif

    if (irestorecamera) {
      CameraHome(HOME);
      // options.camerahome = camera;
      /* moved to HandleDisplay function 
	 if (_s2_cameraset) {
	 _s2priv_CameraSet();
	 }
      */
    }
    MakeGeometry(TRUE, FALSE);
#if defined(BUILDING_S2PLOT)
  }
  xs2cp(waspanel);
#endif

  struct timeval starttime, currtime;
  gettimeofday(&starttime, NULL);

  _s2disptk_return = 0;
  if (key) {
    fprintf(stderr, "Press '%c' to continue", key);
    _s2disptk_key = key;
    _s2disptk_userkcb = _s2_user_keys;
    _s2_user_keys = _s2disptk_kcb;
  }
  if (timeout > 0) {
    fprintf(stderr, ", timeout in %d seconds.\n", (int)timeout);
  } else {
    fprintf(stderr, ".\n");
  }

  _s2_skip = 0;
  
  do {
#if defined(S2LINUX) 
    glutMainLoopEvent();
#elif defined(S2DARWIN)
    glutCheckLoop();
#endif
    HandleDisplay();
    usleep(20);

    if (timeout == 0.0) { /* just do one loop */
      _s2_skip = 1;
    } else if (timeout > 0) { /* check time elapsed */
      gettimeofday(&currtime, NULL);
      if ((currtime.tv_sec - starttime.tv_sec) > timeout) {
	_s2_skip = 1;
      }
    }
  } while (!_s2_skip && !_s2disptk_return);

  if (key) {
    _s2disptk_key = 0;
    _s2_user_keys = _s2disptk_userkcb;
  }

}



#endif 
