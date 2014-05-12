/* s2slides.cc
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
 * $Id: s2slides.cc 5764 2012-09-25 04:16:14Z dbarnes $
 *
 */

#include "s2slides.h"
#ifdef __APPLE__ 
#include "hiddenMouseCursor.h"
#endif

/* Initial image size */
#define RAWWIDTH 1024
#define RAWHEIGHT 768
int L_WIDTH  = 1024;
int L_HEIGHT = 768;
int R_WIDTH  = 1024;
int R_HEIGHT = 768;
int screenwidth = 400;
int screenheight = 300;
double lowleft=-1,lowright=-1;

/* List of image names */
IMAGELIST *imagelist;
int nimage = 0;
int currentimage = 0;

/* Image buffers */
BITMAP4 *bm_left  = NULL;
BITMAP4 *bm_right = NULL;

ANIMOPTIONS options;
int windowcovered = FALSE;

void callbackx(double *, int *, void *); /* callback to update + draw */
int *epanel = NULL;
#include <pthread.h>
void *thread_sub(void *);
pthread_t the_thread;
int readimage = 0;

#include <string>
#include <map>
#include <list>
using namespace std;
map<string, IMAGETEX> imagetexs;

// textures for the ajax-created spinner
#include <vector>
#define AJAXF "spinner%02d.128x128.tga"
#define AJAXN 12 
int n_spinner_images;
vector<unsigned int> ajaxtexs;
void *thread_exec_sub(void *); /* thread which executes a program */
pthread_t exec_thread;
int exec_thread_id;
int execing = 0; /* change to 1 before calling thread_exec; change to 0 
		    before returning */

// caching of textures... how many to keep:
#define NTEXEDFRAMES 12
list<int> recentframes; /* most recent image is at front of list */

#include <sstream>
stringstream flags; /* may contain "-f", "-ss", "-s" */
#define SSRESET(a) (a).str(""); (a).clear(stringstream::goodbit);

// keyboard callback
int kbdcallback(unsigned char *key);

int main(int argc,char **argv) {
  int i,n;
  FILE *fptr;
  int delay,flip;
  char s1[256],s2[256],s3[256],s4[256],aline[256];
  
  /* Are we being called correctly */
  if (argc < 2) {
    lGiveUsage(argv[0]);
    exit(-1);
  }
  
  /* Default options */
  options.stereo     = NOSTEREO;
  options.fullscreen = 0;
  options.debug      = FALSE;
  options.framedelay = 2000; // ms
  options.showhelp   = FALSE;
  options.autoplay   = FALSE;
  options.exitsafe   = FALSE;
  options.exitatend  = FALSE;
  options.leftoffset = 0;
  options.rightoffset = 0;
  options.xoffset = 0;
  options.yoffset = 0;
  options.targetfps = 20;
  options.edit = 0;
  SSRESET(flags);
  
  /* Check/get the command line arguments */
  for (i=1;i<argc;i++) {
    if (strcmp(argv[i],"-h") == 0) {
      lGiveUsage(argv[0]);
      exit(-1);
    }
    if (strcmp(argv[i],"-s") == 0) {
      options.stereo = ACTIVESTEREO;
      flags << "-s ";
    }
    if (strcmp(argv[i],"-ss") == 0) {
      options.stereo = DUALSTEREO;
      flags << "-ss ";
    }
    if (strcmp(argv[i],"-si") == 0) {
      // interstereo
      options.stereo = 31;
      flags << "-si ";
    }
    if (strcmp(argv[i],"-sw") == 0) {
      // warped stereo
      options.stereo = 32;
      flags << "-sw ";
    }
    if (strcmp(argv[i],"-f") == 0) {
      options.fullscreen = 1;
      flags << "-f ";
    }
    if (strcmp(argv[i],"-E") == 0)
      options.edit = 1;
    if (strcmp(argv[i],"-d") == 0)
      options.debug = TRUE;
    if (strcmp(argv[i],"-a") == 0)
      options.autoplay = TRUE;
    if (strcmp(argv[i],"-e") == 0)
      options.exitatend = TRUE;
    if (strcmp(argv[i],"-i") == 0) {
      if ((imagelist = (IMAGELIST *)realloc(imagelist,(nimage+1)*sizeof(IMAGELIST))) == NULL) {
	fprintf(stderr,"Failed to allocate space for the imagelist\n");
	return(-1);
      }
      i++;
      strcpy(imagelist[nimage].left,argv[i]);
      if (i < argc-1) {
	i++;
	strcpy(imagelist[nimage].right,argv[i]);
      } else {
	strcpy(imagelist[nimage].right,argv[i]);
      }
      imagelist[nimage].delay = 1;
      imagelist[nimage].flip = FALSE;
      nimage++;
    }
    if (strcmp(argv[i],"-nx") == 0) {
      i++;
      L_WIDTH = atoi(argv[i]);
      R_WIDTH = L_WIDTH;
    }
    if (strcmp(argv[i],"-ny") == 0) {
      i++;
      L_HEIGHT = atoi(argv[i]);
      R_HEIGHT = L_HEIGHT;
    }
    if (strcmp(argv[i],"-Ims") == 0) { // framedelay (interval) in ms
      i++;
      options.framedelay = atof(argv[i]);
    }
    if (strcmp(argv[i],"-safe") == 0)
      options.exitsafe = TRUE;
  }
  
  if (options.stereo == DUALSTEREO)
    screenwidth *= 2;
  
  /* 
     Read the image list 
     Lines starting with a "#" are comments 
  */
  if (nimage == 0) {
    if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
      fprintf(stderr,"Reading slide list failed\n");
      exit(-1);
    }
    while (fgets(aline,255,fptr) != NULL) {
      n = sscanf(aline,"%s %s %s %s",s1,s2,s3,s4);
      
      if (options.debug) {
	fprintf(stderr,"%s\n",aline);
      }

      if (n < 1)
	continue;
      
      /* Skip comments */
      if (s1[0] == '#') 
	continue;
      
      /* Single column for mono playback */
      if (n < 2) 
	strcpy(s2,s1);
      
      /* Read the delay */
      delay = 0;
      if (n >= 3) {
	if (atoi(s3) >= 0 && atoi(s3) <= 10000) 
	  delay = atoi(s3);
      }
      
      /* Read the optional flipping flag */
      flip = FALSE;
      if (n >= 4) {
	if (strcmp(s4,"-f") == 0 || strcmp(s4,"-F") == 0)
	  flip = TRUE;
      }
      
      /* Add to the image list */
      if ((imagelist = (IMAGELIST *)realloc(imagelist,(nimage+1)*sizeof(IMAGELIST))) == NULL) {
	fprintf(stderr,"Failed to allocate space for the imagelist\n");
	return(-1);
      }
      strcpy(imagelist[nimage].left,s1);
      strcpy(imagelist[nimage].right,s2);
      imagelist[nimage].delay = delay;
      imagelist[nimage].flip = flip;
      imagelist[nimage].status = 0;
      imagelist[nimage].lw = -1;
      imagelist[nimage].lh = -1;
      imagelist[nimage].rw = -1;
      imagelist[nimage].rh = -1;
      imagelist[nimage].lbits = NULL;
      imagelist[nimage].rbits = NULL;
      imagelist[nimage].ltex = 0;
      imagelist[nimage].rtex = 0;
      nimage++;
    }
    fclose(fptr);
  }
  if (options.debug) {
    fprintf(stderr,"Found %d images\n",nimage);
  }
  if (nimage < 1) {
    fprintf(stderr,"Didn't find any slides\n");
    exit(-1);
  }
  
  HandleMemory('b',L_WIDTH,L_HEIGHT);
  
  
  if (!s2open(options.fullscreen, options.stereo, argc, argv)) {
    fprintf(stderr, "Failed to open S2PLOT device.\n");
    exit(-1);
  }
  
  // create the slide loading thread
  pthread_t p_thread;
  int a = 1;
  pthread_create(&p_thread, NULL, thread_sub, (void *)&a);
  
  while (imagelist[0].status == 0) {
    usleep(10000);
  }
  
  if (options.edit) {
    xs2mp(0, 0., 0., 0.795, 1.0);
    
#define NEPANELS 5
    epanel = (int *)malloc(NEPANELS * sizeof(int));
#define DDdy (0.9 * 1.0 / (float)NEPANELS)
#define DDsy (0.1 * 1.0 / (float)NEPANELS)
    for (int ip = 0; ip < NEPANELS; ip++) {
      epanel[ip] = xs2ap(0.8, 1. - (ip * (DDdy + DDsy) + 0.5 * DDsy + DDdy),
			 1.0, 1. - (ip * (DDdy + DDsy) + 0.5 * DDsy));
      xs2cp(epanel[ip]);
      s2swin(-1, 1, -1, 1, -1, 1);
      cs2scbx((void *)callbackx, (void *)&(epanel[ip]));
    }
  }
  
  xs2cp(0);
  s2swin(-1, 1, -1, 1, -1, 1);
  ss2srm(SHADE_FLAT);
  
  int scr0 = 0;
  cs2scbx((void *)callbackx, (void *)&scr0);
  cs2skcb((void *)kbdcallback);
  
  currentimage = 0;
  s2show(1);
    
  return(0);
}

void callbackx(double *tm, int *kc, void *data) {
  static int beenhere = 0;
  static double tstart;
  static double lasttime = -1;
  double dt;
  XYZ P[4] = { {0., 1., 0.01}, 
	       {1., 1., 0.01},
	       {1., 0., 0.01},
	       {0., 0., 0.01} };
  static COLOUR col = {1., 1., 1.};
  static int lastkc = -1;
  static int spinner_idx = 0;

  static XYZ Pspin[4] = { {0.45, 0.55, 0.01},
			  {0.55, 0.55, 0.01},
			  {0.55, 0.45, 0.01},
			  {0.45, 0.45, 0.01} };
  static COLOUR Cspin = {0.33, 0.3, 0.3};

  if (!beenhere) {
    // initialise
    lasttime = tstart = GetRunTime();
    lastkc = *kc;

    // load the ajax textures
    int i;
    char fn[80];
    ajaxtexs.resize(0);

    char *ajaxf = getenv("S2SLIDES_SPINNERF");
    float asscr = ss2qar();
    float dx, dy;
    if (ajaxf) {
      unsigned int ajtexid = ss2lt(ajaxf);
      ajaxtexs.push_back(ajtexid);
      n_spinner_images = 1;
      int ajw, ajh;
      ss2gt(ajtexid, &ajw, &ajh);
      float asimg = (float)ajw / (float)ajh;
      fprintf(stderr, "asscr = %f, asimg = (%d / %d) = %f\n", 
	      asscr, ajw, ajh, asimg);
      if (asimg >= asscr) {
	dx = 0.5;
	dy = 0.5 * asscr / asimg;
      } else {
	dx = 0.5 * asimg / asscr;
	dy = 0.5;
      }
      Cspin.r = Cspin.g = Cspin.b = 1.0;
    } else {
      for (i = 0; i < AJAXN; i++) {
	sprintf(fn, AJAXF, i);
	ajaxtexs.push_back(ss2lt(fn));
      }
      n_spinner_images = AJAXN;
      float asimg = 1.; /* aspect ratio of spinner should be 1 */
      if (asimg >= asscr) {
	dx = 0.05;
	dy = 0.05 * asscr / asimg;
      } else {
	dx = 0.05 * asimg / asscr;
	dy = 0.05;
      }
    }
    Pspin[0].x = Pspin[3].x = 0.5 - dx;
    Pspin[1].x = Pspin[2].x = 0.5 + dx;
    Pspin[0].y = Pspin[1].y = 0.5 + dy;
    Pspin[2].y = Pspin[3].y = 0.5 - dy;

    recentframes.resize(0);

    beenhere = 1;
  }

  spinner_idx = (spinner_idx + 1) % n_spinner_images;

  if (execing) {
    // currently can only exec for non-editing s2slides, so 
    // don't need to select panel or anything like that

    ss2tsc((char *)"lc");
    ns2vf4x(Pspin, Cspin, ajaxtexs[spinner_idx], 1., 'o');
    ss2tsc((char *)"");

    ss2tsc((char *)"r");
    ns2vf4x(Pspin, Cspin, ajaxtexs[spinner_idx], 1., 'o');
    ss2tsc((char *)"");

    usleep(80000);
    lasttime = GetRunTime();

    lastkc = *kc;
    return;
  }

  if (options.autoplay) {
    dt = (options.framedelay + imagelist[currentimage].delay) / 1000.0;
    if (GetRunTime() - lasttime < dt) {
      //return;
    } else {
      currentimage += 1;
      if (options.exitatend && currentimage >= nimage) {
	exit(0);
      }
      currentimage %= nimage;
      lasttime = GetRunTime();
    }
  } else if (*kc > lastkc) {
    currentimage = (currentimage + 1) % nimage;
  }

  // choose panel
  int whichpanel = *(int *)(data);
  xs2cp(whichpanel);
  int doimage;

  if (whichpanel == 0) {
    doimage = currentimage;
  } else {
    doimage = currentimage + whichpanel - (NEPANELS+1)/2;
  }

  if (doimage < 0 || doimage > (nimage-1)) {
    // image not possible just return...
    return;
  }

  // image not available yet, just return (but could in future put in a 
  // "pending" texture)
  if (imagelist[doimage].status == 0) {
    return;
  }

  if (imagelist[doimage].status == 1) {
    // create textures
    // left
    if (imagetexs[imagelist[doimage].left].status == 1) {
      imagetexs[imagelist[doimage].left].texid = 
	ss2ctt(imagetexs[imagelist[doimage].left].w,
	       imagetexs[imagelist[doimage].left].h);
      unsigned char *bits = ss2gt(imagetexs[imagelist[doimage].left].texid,
				  NULL, NULL);
      bcopy(imagetexs[imagelist[doimage].left].bits, bits, 
	    imagetexs[imagelist[doimage].left].size);
      ss2ptt(imagetexs[imagelist[doimage].left].texid);
      imagetexs[imagelist[doimage].left].status = 2;
      if (options.debug) {
	fprintf(stderr, "loaded L texture for frame %d\n", doimage);
      }
    }

    // right
    if (imagetexs[imagelist[doimage].right].status == 1) {
      imagetexs[imagelist[doimage].right].texid = 
	ss2ctt(imagetexs[imagelist[doimage].right].w,
	      imagetexs[imagelist[doimage].right].h);
      unsigned char *bits = ss2gt(imagetexs[imagelist[doimage].right].texid,
				  NULL, NULL);
      bcopy(imagetexs[imagelist[doimage].right].bits, bits, 
	    imagetexs[imagelist[doimage].right].size);
      ss2ptt(imagetexs[imagelist[doimage].right].texid);
      imagetexs[imagelist[doimage].right].status = 2;
      if (options.debug) {
	fprintf(stderr, "loaded R texture for frame %d\n", doimage);
      }
    }


    // remove this frame from wherever it is in list
    recentframes.remove(doimage);
    // and push it onto the front of list
    recentframes.push_front(doimage);

    imagelist[doimage].status = 2;
  }
  
  if (imagelist[doimage].status != 2) {
    fprintf(stderr, "Critical error: invalid status!\n");
    exit(-1);
  }

  float asscr = ss2qar();
  int sx, sy;
  ss2qsd(&sx, &sy);
  float viewscale = 1.0;
  if (options.edit) {
    viewscale = MIN(((float)(sx-4)/(float)sx), ((float)(sy-4)/(float)sy));
  }
  float dx, dy;

  if (options.debug) {
    fprintf(stderr, "showing frame %d\n", doimage);
  }
  // draw the current left image in screen coordinates...
  float asimg = imagetexs[imagelist[doimage].left].aspect;
  if (asimg >= asscr) {
    dx = 0.5;
    dy = 0.5 * asscr / asimg;
  } else {
    dx = 0.5 * asimg / asscr;
    dy = 0.5;
  }
  if (imagelist[doimage].flip) {
    dy *= -1.;
  }
  dx *= viewscale; /* this shrinks images so border is not overwritten */
  dy *= viewscale;
  P[0].x = P[3].x = 0.5 - dx + 0.5 * options.xoffset/(float)sx;
  P[1].x = P[2].x = 0.5 + dx + 0.5 * options.xoffset/(float)sx;
  P[0].y = P[1].y = 0.5 + dy + 0.5 * options.yoffset/(float)sy;
  P[2].y = P[3].y = 0.5 - dy + 0.5 * options.yoffset/(float)sy;
  ss2tsc((char *)"lc");
  ns2vf4x(P, col, imagetexs[imagelist[doimage].left].texid, 1., 'o');
  ss2tsc((char *)"");

  // draw the current right image in screen coordinates
  asimg = imagetexs[imagelist[doimage].right].aspect;
  if (asimg >= asscr) {
    dx = 0.5;
    dy = 0.5 * asscr / asimg;
  } else {
    dx = 0.5 * asimg / asscr;
    dy = 0.5;
  }
  if (imagelist[doimage].flip) {
    dy *= -1.;
  }
  dx *= viewscale; /* this shrinks images so border is not overwritten */
  dy *= viewscale;
  P[0].x = P[3].x = 0.5 - dx - 0.5 * options.xoffset/(float)sx;
  P[1].x = P[2].x = 0.5 + dx - 0.5 * options.xoffset/(float)sx;
  P[0].y = P[1].y = 0.5 + dy - 0.5 * options.yoffset/(float)sy;
  P[2].y = P[3].y = 0.5 - dy - 0.5 * options.yoffset/(float)sy;
  ss2tsc((char *)"r");
  ns2vf4x(P, col, imagetexs[imagelist[doimage].right].texid, 1., 'o');
  ss2tsc((char *)"");

  if (options.edit) {
    // number the slide 
    
  }

#if (0)
  // number the slide
  ss2tsc((char *)"lcr");
  char slidestr[80];
  sprintf(slidestr, "%2d/%d", currentimage+1, nimage);
  ns2text(0.004, 0.018, 0.01, 0.007,0.0,0.0, 0.0,0.007,0.0, 0.7,0.7,0.7, slidestr);

  //ns2disk(0.96, 0.96, 0.1, 0.,0.,-1., 0.005, 0.006, 0.8,0.8,0.5); 
  time_t thetime = time(NULL);
  struct tm *ltime = localtime(&thetime);
  int mins = ltime->tm_min + 15;
  fprintf(stderr, "mins = %d\n", mins);
  s2sci(S2_PG_RED);
  s2slw(3.0);
  ns2arc(0.5, 0.5, 0.01, 0.,0.,1., 0.6,0.6,0.01, (float)mins/60.0 * 360.0, mins+5);
  s2rectxy(0.9, 0.98, 0.96, 0.98, 0.01);

  ss2tsc((char *)"");
#endif

  if (!options.edit && imagelist[doimage].command && !execing) {
    execing = 1;
    spinner_idx = 0;
    if (pthread_create(&exec_thread, NULL, thread_exec_sub,
		       (void *)(imagelist[doimage].command))) {
      // failed - go to next slide
      execing = 0;
      currentimage = (currentimage + 1) % nimage;
    } else {
      // succeeded - do nothing here - state of "execing" is sufficient
      // to modify behaviour of this callback
    }
  }
  
  // drop any textures that haven't been used for a while:
  while (recentframes.size() > NTEXEDFRAMES) {
    int tg = recentframes.back();
    if (imagetexs[imagelist[tg].left].status == 2) {
      ss2dt(imagetexs[imagelist[tg].left].texid);
      imagetexs[imagelist[tg].left].status = 1;
    }
    if (imagetexs[imagelist[tg].right].status == 2) {
      ss2dt(imagetexs[imagelist[tg].right].texid);
      imagetexs[imagelist[tg].right].status = 1;
    }
    imagelist[tg].status = 1;
    if (options.debug) {
      fprintf(stderr, "dropped L+R textures for frame %d\n", tg);
    }
    recentframes.pop_back();
  }

  lastkc = *kc;
}

void ReadImagePair(void) {
  int index;
  
  /* Read the image pairs */
  index = (readimage);
  
  if (imagetexs.find(imagelist[index].left) == imagetexs.end()) {
    if (!ReadImage(imagelist[index].left,'l')) {
      fprintf(stderr,"Image read failed for image \"%s\"\n",imagelist[index].left);
      CreateDummy('l');
    } else {
      if (options.debug) {
	fprintf(stderr,"Successfully read file \"%s\"\n",imagelist[index].left);
      }
    }
  } else {
    //fprintf(stderr, "(already have this image)\n");
  }
  if (options.stereo) {
    index = (readimage);
    if (imagetexs.find(imagelist[index].right) == imagetexs.end()) {
      if (!ReadImage(imagelist[index].right,'r')) {
	fprintf(stderr,"Image read failed for image \"%s\"\n",imagelist[index].right);
	CreateDummy('r');
      } else {
	if (options.debug) {
	  fprintf(stderr,"Successfully read file \"%s\"\n",imagelist[index].right);
	}
      }
    } else {
      if (options.debug) {
	fprintf(stderr, "duplicate skipped (already have this image)\n");
      }
    }
  }
}
 

/* 
	Read a PPM, TGA, RLE, TIF, or RAW image 
	Swap the r and b for tga images
	Optionally flip the images vertically
*/
int ReadImage(char *filename,int eye) {
  int i,d,w,h,c;
  int n,depth;
  char id[32];
  FILE *fptr;
  int status = TRUE;
  int imagetype;
  
  if (options.debug) {
    fprintf(stderr,"Attempting to read file \"%s\"\n",filename);
  }
  
  if (strstr(filename,".ppm") != NULL || strstr(filename,".PPM") != NULL) {
    imagetype = PPM;
  } else if (strstr(filename,".raw") != NULL || strstr(filename,".RAW") != NULL) {
    imagetype = RAW;
  } else if (strstr(filename,".rle") != NULL || strstr(filename,".RLE") != NULL) {
    imagetype = RLE;
  } else if (strstr(filename,".tga") != NULL || strstr(filename,".TGA") != NULL) {
    imagetype = TGA;
  } else if (strstr(filename,".tif") != NULL || strstr(filename,".TIF") != NULL) {
    imagetype = TIF;
  } else {
    fprintf(stderr,"Unknown image \"%s\", expect (raw,ppm,tga,rle)\n",filename);
    return(FALSE);
  }
  
  if (options.debug) {
    fprintf(stderr,"Think I've found image type %d\n",imagetype);
  }
  
  if ((fptr = fopen(filename,"r")) == NULL)
    return(FALSE);
  
  /* -------------------------------------------------- RAW ----------------- */
  if (imagetype == RAW) {
    
    w = RAWWIDTH;	/* Fixed size for raw images */
    h = RAWHEIGHT;
    HandleMemory(eye,w,h);
    
  }
  
  /* -------------------------------------------------- PPM ----------------- */
  if (imagetype == PPM) {
    
    if (fscanf(fptr,"%s",id) != 1 || strcmp(id,"P6") != 0) {
      fprintf(stderr,"Unexpected file id of \"%s\", expected \"P6\"\n",id);
      status = FALSE;
      goto skip;
    }
    
    if (fscanf(fptr,"%d %d",&w,&h) != 2) {
      fprintf(stderr,"Failed to read width and height\n");
      status = FALSE;
      goto skip;
    }
    
    HandleMemory(eye,w,h);
    
    if (fscanf(fptr,"%d",&d) != 1 || d != 255) {
      fprintf(stderr,"Unexpected image depth of %d, expected 255\n",d);
      status = FALSE;
      goto skip;
    }
    
    while ((c = fgetc(fptr)) != EOF && c != '\n')
      ;
    
  }
  
  /* -------------------------------------------------- RLE ----------------- */
  if (imagetype == RLE) {
    
    if (fscanf(fptr,"%d %d",&w,&h) != 2) {
      fprintf(stderr,"Failed to read width and height\n");
      status = FALSE;
      goto skip;
    }
    HandleMemory(eye,w,h);
    
    while ((c = fgetc(fptr)) != EOF && c != '\n')
      ;
    
  }
  
  /* -------------------------------------------------- TIF ----------------- */
  if (imagetype == TIF) {
    
    for (i=0;i<8;i++)
      c = fgetc(fptr);
    
    w = L_WIDTH;   /* Fixed size for tif images */
    h = L_HEIGHT;
    HandleMemory(eye,w,h);
    
  }
  
  /* -------------------------------------------------- TGA ----------------- */
  if (imagetype == TGA) {
    
    TGA_Info(fptr,&w,&h,&depth);
    if (options.debug) {
      fprintf(stderr,"TGA_Info: %d %d %d\n",w,h,depth);
    }
    if (depth != 16 && depth != 24 && depth != 32) {
      fprintf(stderr,"Can only handle depth of 16, 24, or 32\n");
      exit(-1);
    }	
    HandleMemory(eye,w,h);
    
  }
  
  if (options.debug) {
    fprintf(stderr,"Successfully read the header\n");
  }
  
  if (eye == 'l') {
    if (imagetype == TGA) {
      n = TGA_Read(fptr,(BITMAP4 *)bm_left,&w,&h);
      if (options.debug) {
	fprintf(stderr,"TGA_Read returned %d (%d, %d)\n",n,w,h);
      }
    } else {
      if ((int)fread(bm_left,sizeof(BITMAP4),L_WIDTH*L_HEIGHT,fptr) != L_WIDTH*L_HEIGHT) {
	fprintf(stderr,"Error reading left image data\n");
	status = FALSE;
	goto skip;
      }
      if (options.debug) {
	fprintf(stderr,"Read left raw image data successfully\n");
      }
    }
  } else {	
    if (imagetype == TGA) {
      n = TGA_Read(fptr,(BITMAP4 *)bm_right,&w,&h); 
      if (options.debug) {
	fprintf(stderr,"TGA_Read returned %d (%d,%d)\n",n,w,h);
      }
    } else {
      if ((int)fread(bm_right,sizeof(BITMAP4),R_WIDTH*R_HEIGHT,fptr) != R_WIDTH*R_HEIGHT) {
	fprintf(stderr,"Error reading right image data\n");
	status = FALSE;
	goto skip;
      }
      if (options.debug) {
	fprintf(stderr,"Read right raw image data successfully\n");
      }
    }
  }
  
 skip:
  fclose(fptr);
  
  return(status);
}

/*
  Display the program usage information
*/
void lGiveUsage(char *cmd) {
  fprintf(stderr,"Usage: %s [options] slidelistfile\n",cmd);
  fprintf(stderr,"Options\n");          
  fprintf(stderr,"          -h    this text\n");
  fprintf(stderr,"          -s    active stereo mode\n");
  fprintf(stderr,"          -ss   dual screen stereo mode\n");
  fprintf(stderr,"          -si   interleave stereo mode\n");
  fprintf(stderr,"          -f    full screen mode\n");
  fprintf(stderr,"          -a    start in autoplay mode\n");
  fprintf(stderr,"          -e    exit at end of show, default is loop\n");
  fprintf(stderr,"Key Strokes and Menus:\n");
  fprintf(stderr," Shift-ESC,Q    quit\n");
  fprintf(stderr,"           g    autoplay\n");
  fprintf(stderr,"         h,6    go to first frame (start)\n");
  fprintf(stderr,"         <,,    prev frame\n");
  fprintf(stderr,"           7    prev frame (skip executables)\n");
  fprintf(stderr,"         >,.    next frame\n");
  fprintf(stderr,"           8    next frame (skip executables)\n");
  fprintf(stderr,"            9   go to last frame (end)\n");
  fprintf(stderr,"          i,I   stereo shift left image up\n");
  fprintf(stderr,"          k,K   stereo shift left image down\n");
  fprintf(stderr,"          j,J   stereo shift left image left\n");
  fprintf(stderr,"          l,L   stereo shift left image right\n");
}

/*
	Deal with memory for the buffers
	eye is either 'l', 'r', or 'b' for both
*/
void HandleMemory(int eye,int w,int h)
{
	if (eye == 'b' || eye == 'l') {
		if (L_WIDTH == w && L_HEIGHT == h && bm_left != NULL)
			return;
      L_WIDTH = w;
      L_HEIGHT = h;
		if (bm_left != NULL)
      	free(bm_left);
		if ((bm_left = (BITMAP4 *)malloc(L_WIDTH*L_HEIGHT*sizeof(BITMAP4))) == NULL) {
			fprintf(stderr,"Malloc of left image failed\n");
			exit(-1);
		}
	}

   if (eye == 'b' || eye == 'r') {
      if (R_WIDTH == w && R_HEIGHT == h && bm_right != NULL)
         return;
		R_WIDTH = w;
      R_HEIGHT = h;
		if (bm_right != NULL)
      	free(bm_right);
		if ((bm_right = (BITMAP4 *)malloc(R_WIDTH*R_HEIGHT*sizeof(BITMAP4))) == NULL) {
      	fprintf(stderr,"Malloc of right image failed\n");
      	exit(-1);
   	}
	}
}

/*
	Create dummy image for when we can recover from a read failure
*/
void CreateDummy(int eye)
{
	int i;
	BITMAP4 black = {0,0,0,255};

	if (eye == 'l') {
		for (i=0;i<L_WIDTH*L_HEIGHT;i++) 
			bm_left[i] = black;
	} else {
      for (i=0;i<R_WIDTH*R_HEIGHT;i++) 
         bm_right[i] = black;
	}
}



void *thread_sub(void *data) {  
  int i;

  // clear the map of images and textures
  imagetexs.clear();
  IMAGETEX newit;

  if (options.debug) {
    fprintf(stderr, ">>> Transient thread: reading %d images...\n", nimage);
  }

  // 1. add a black frame called "!" to be used for when we run an
  //    executable rather than show an image.
  newit.w = newit.h = 1;
  newit.aspect = 1.0;
  newit.size = newit.h * newit.w * sizeof(BITMAP4);
  newit.bits = (BITMAP4 *)malloc(newit.size);
  memset(newit.bits, 0, newit.size);
  newit.status = 1;
  newit.texid = 0;
  imagetexs["!"] = newit;
  
  // 2. now loop and add the images from the file
  for (i = 0; i < nimage; i++) {

    usleep(5000);

    if (imagelist[i].status > 0) {
      fprintf(stderr, "Thread: critical error - invalid status!\n");
      exit(-1);
    }
    readimage = i;

    // make sure this is a real image pair, not a command
    if (imagelist[i].left[0] != '!') {
      
      ReadImagePair();
      if (imagetexs.find(imagelist[i].left) == imagetexs.end()) {
	// add new IMAGETEX structure to imagetexs map for LEFT image
	newit.w = L_WIDTH;
	newit.h = L_HEIGHT;
	newit.aspect = (float)newit.w / (float)newit.h;
	newit.size = newit.w * newit.h * sizeof(BITMAP4);
	newit.bits = (BITMAP4 *)malloc(newit.size);
	bcopy(bm_left, newit.bits, newit.size);
	newit.status = 1;
	newit.texid = 0;
	imagetexs[imagelist[i].left] = newit;
      }
      if (imagetexs.find(imagelist[i].right) == imagetexs.end()) {
	// add new IMAGETEX structure to imagetexs map for RIGHT image
	newit.w = R_WIDTH;
	newit.h = R_HEIGHT;
	newit.aspect = (float)newit.w / (float)newit.h;
	newit.size = newit.w * newit.h * sizeof(BITMAP4);
	newit.bits = (BITMAP4 *)malloc(newit.size);
	bcopy(bm_right, newit.bits, newit.size);
	newit.status = 1;
	newit.texid = 0;
	imagetexs[imagelist[i].right] = newit;
      }
      imagelist[i].command = NULL;
      imagelist[i].status = 1;

    } else {
      // mark this as a command not an image
      imagelist[i].command = (char *)malloc((strlen(imagelist[i].left) + 
					     strlen(imagelist[i].right)+50) * 
					    sizeof(char));
      sprintf(imagelist[i].command, "cd %s; %s %s",
	      &(imagelist[i].left[1]), imagelist[i].right, flags.str().c_str());
      strcpy(imagelist[i].left, "!");
      strcpy(imagelist[i].right, "!");
      imagelist[i].status = 1;
    }

  }
  
  if (options.debug) {
    fprintf(stderr, ">>> Transient thread: finished!\n");
  }

  pthread_exit(NULL);
  return (NULL);
}

 void *thread_exec_sub(void *data) {
   usleep(5000);
   system((char *)data);
   currentimage = (currentimage + 1) % nimage;
   execing = 0;
   pthread_exit(NULL);
   return (NULL);
 }


int kbdcallback(unsigned char *key) {
  int consumed = 0;

  switch (*key) {
  case 'g':
  case 'G':
    options.autoplay = !options.autoplay;
    consumed = 1;
    break;

  case 'h':
  case 'H':
    /* go to start */
    currentimage = 0;
    consumed = 1;
    break;

  case 'd':
  case 'D':
    /* debug */
    options.debug = !options.debug;
    consumed = 1;
    break;

  case '1':
    options.framedelay -= 5;
    if (options.framedelay < 0) {
      options.framedelay = 0;
    }
    consumed = 1;
    break;

  case '2':
    options.framedelay += 5;
    consumed = 1;
    break;

  case '<':
  case ',':
    /* backward: do not skip executables */
    options.autoplay = FALSE;
    if (currentimage > 0) {
      currentimage--;
    }
    consumed = 1;
    break;

  case '>':
  case '.':
    /* forward: do not skip executables */
    options.autoplay = FALSE;
    if (currentimage < nimage-1) {
      currentimage++;
    }
    consumed = 1;
    break;

  case '6':
    /* to start */
    options.autoplay = FALSE;
    currentimage = 0;
    consumed = 1;
    break;

  case '7':
    /* backward (skip executable slides) */
    //options.autoplay = FALSE;
    if (currentimage > 0) {
      do {
	currentimage--;
      } while ((currentimage > 0) && 
	       (imagelist[currentimage].command));
    }
    consumed = 1;
    break;

  case '8':
    /* forward (skip executable slides) */
    //options.autoplay = FALSE;
    if (currentimage < nimage-1) {
      do {
	currentimage++;
      } while ((currentimage < nimage-1) &&
	       (imagelist[currentimage].command));
    }
    consumed = 1;
    break;
    
  case '9':
    /* to end */
    options.autoplay = FALSE;
    currentimage = nimage-1;
    consumed = 1;
    break;

  case 'i':
    /* left image up */
    options.yoffset -= 1.0;
    consumed = 1;
    break;
  case 'I':
    options.yoffset -= 5.0;
    consumed = 1;
    break;

  case 'k':
    /* left image down */
    options.yoffset += 1.0;
    consumed = 1;
    break;
  case 'K':
    options.yoffset += 5.0;
    consumed = 1;
    break;

  case 'j':
    /* left image left */
    options.xoffset -= 1.0;
    consumed = 1;
    break;
  case 'J':
    options.xoffset -= 5.0;
    consumed = 1;
    break;

  case 'l':
    /* left image right */
    options.xoffset += 1.0;
    consumed = 1;
    break;
  case 'L':
    options.xoffset += 5.0;
    consumed = 1;
    break;

  }

  return consumed;
}
