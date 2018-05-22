/* s2common.c
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
 * $Id: s2common.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

/* N.B. build this code via #include in primary source, NOT on its own! */

/*
	Set the camera to the "home" position (mode == 1)
	Focus the camera to the center of the geometry (mode = 2)
	Move to a "axis aligned" position (mode 3..8)
*/
void CameraHome(int mode) {
  
  double size;
  
  // Stop any forward movement 
  camera.speed = 0;
  
  // Find the bounds of the geometry 
  static int beenhere = 0;
  if (!beenhere) {
    CalcBounds();
    beenhere = 1;
  } else {
    fprintf(stderr, "NOT doing CalcBounds where I used to pre 20180522\n");
  }
  
  size = MAX((pmax.x-pmin.x),(pmax.y-pmin.y));
  size = MAX(size,(pmax.z-pmin.z));
#if defined(BUILDING_S2PLOT)
  /* addition dbarnes for S2PLOT: simply suggest the geometry is
   * larger than it really is! */
  size = sqrt(2.0) * size;
#endif
  
  // Set the camera attributes 
  camera.pr = pmid;
  switch (mode) {
  case HOME:
    camera.focallength = 1.75*size;
    camera.aperture = 45;
    camera.eyesep = size / 20.0;
    camera.vp = pmid;
    camera.vp.z += 1.75*size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  case FOCUS:
    break;
  case FRONT:
    camera.vp = pmid;
    camera.vp.z += 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  case BACK:
    camera.vp = pmid;
    camera.vp.z -= 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  case LEFT:
    camera.vp = pmid;
    camera.vp.x -= 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  case RIGHT:
    camera.vp = pmid;
    camera.vp.x += 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  case TOP:
    camera.vp = pmid;
    camera.vp.y += 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 0; camera.vu.z = 1;
    break;
  case BOTTOM:
    camera.vp = pmid;
    camera.vp.y -= 1.75 * size;
    camera.vu.x = 0; camera.vu.y = 0; camera.vu.z = 1;
    break;
  case AXONOMETRIC:
    camera.vp = pmid;
    camera.vp.x -= size;
    camera.vp.y += size;
    camera.vp.z -= size;
    camera.vu.x = 0; camera.vu.y = 1; camera.vu.z = 0;
    break;
  }
  
  // Point the camera toward the mid point of the model 
  camera.vd.x = pmid.x - camera.vp.x;
  camera.vd.y = pmid.y - camera.vp.y;
  camera.vd.z = pmid.z - camera.vp.z;
  
  // Update the focal point 
  Normalise(&camera.vd);
  camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
  camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
  camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
  
  if (0 || options.debug) {
    fprintf(stderr,"Bounds = (%g,%g,%g) -> (%g,%g,%g)\n",
	    pmin.x,pmin.y,pmin.z,pmax.x,pmax.y,pmax.z);
    fprintf(stderr,"Midpoint = (%g,%g,%g)\n",pmid.x,pmid.y,pmid.z);
  }
}


/*
	Rotate (ix,iy) or roll (iz) the camera.
	Depends on the interaction mode.
	Correctly update all camera attributes
*/
void RotateCamera(double ix,double iy,double iz,int source) {
  XYZ vp,vu,vd;
  XYZ right;
  XYZ newvp,newr;
  double delta,radius;
  double dx,dy,dz;
  
  vu = camera.vu;
  Normalise(&vu);
  vp = camera.vp;
  vd = camera.vd;
  Normalise(&vd);
  CROSSPROD(vd,vu,right);
  Normalise(&right);
  
  /* Determine the amount of change */
#if defined(BUILDING_S2PLOT)
  delta = ss2qcs() * DTOR;
#elif defined(BUILDING_VIEWER)
  delta = options.interactspeed * DTOR;
#endif
  if (source == MOUSECONTROL)
    delta /= 2;
  
  /* 
     Handle the roll 
     Just change the up vector
  */
  if (ABS(iz) > EPSILON) {
    if (options.interaction == OBJECT) {
      // rolls are rotations around the view direction
      double rx[16];
      // 1. rotate vd into object space:
      //XYZ vdo = apply4x4transformMatrix(_s2_object_rot, vd);
      computeRotationMatrix4x4(rx, vd, iz * delta);
      postmultiplyMatrix4x4(_s2_object_rot, rx);
      
    } else {
      camera.vu.x += iz * right.x * delta;
      camera.vu.y += iz * right.y * delta;
      camera.vu.z += iz * right.z * delta;
      Normalise(&camera.vu);
      return;
    }
  }

  /* Object mode, store the rotation in the _s2_object_rot matrix */
  if (options.interaction == OBJECT) {
    double rx[16];
    if (ABS(ix) > EPSILON) {
      // left/right are rotations around the up vector (Yaw)
      computeRotationMatrix4x4(rx, camera.vu, -ix * delta);
      postmultiplyMatrix4x4(_s2_object_rot, rx);
      
    } 
    if (ABS(iy) > EPSILON) {
      // up/down are rotations around the right vector (pitch)
      computeRotationMatrix4x4(rx, right, iy * delta);
      postmultiplyMatrix4x4(_s2_object_rot, rx);
    }
    return;
  }
  
  /*
    Fly mode
    Change the view direction vector along up or right vector
  */
  if (options.interaction == FLY) {
    camera.vd.x = vd.x + delta * ix * right.x + delta * iy * vu.x;
    camera.vd.y = vd.y + delta * ix * right.y + delta * iy * vu.y;
    camera.vd.z = vd.z + delta * ix * right.z + delta * iy * vu.z;
    Normalise(&camera.vd);
    
    /* Determine the new up vector */
    if (ABS(ix) > ABS(iy)) {
      CROSSPROD(camera.vd,camera.vu,right);
      Normalise(&camera.vu);
      CROSSPROD(right,camera.vd,camera.vu);
      Normalise(&camera.vu);
    } else if (ABS(ix) < ABS(iy)) {
      CROSSPROD(right,camera.vd,camera.vu);
      Normalise(&camera.vu);
    }
    
    camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
    camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
    camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
    
    return;
  }
  
  /*
    Walk mode
    ix changes the view direction
    iy changes the height along the up vector
  */
  if (options.interaction == WALK) {
    camera.vd.x = vd.x + delta * ix * right.x;
    camera.vd.y = vd.y + delta * ix * right.y;
    camera.vd.z = vd.z + delta * ix * right.z;
    Normalise(&camera.vd);
    
    camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
    camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
    camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
    
    return;
  }
  
  /* 
     Inspect mode 
     Handle special case when the camera coincident with the rotate point
  */
  dx = camera.vp.x - camera.pr.x;
  dy = camera.vp.y - camera.pr.y;
  dz = camera.vp.z - camera.pr.z;
  radius = sqrt(dx*dx + dy*dy + dz*dz);
  if (ABS(radius) > 0.0001) {
    
    /* Determine the new view point */
    delta *= radius;
    newvp.x = vp.x + delta * ix * right.x + delta * iy * vu.x - camera.pr.x; 
    newvp.y = vp.y + delta * ix * right.y + delta * iy * vu.y - camera.pr.y;
    newvp.z = vp.z + delta * ix * right.z + delta * iy * vu.z - camera.pr.z;
    Normalise(&newvp);
    camera.vp.x = camera.pr.x + radius * newvp.x;
    camera.vp.y = camera.pr.y + radius * newvp.y;
    camera.vp.z = camera.pr.z + radius * newvp.z;
    
    /* Determine the new right vector */
    newr.x = camera.vp.x + right.x - camera.pr.x;
    newr.y = camera.vp.y + right.y - camera.pr.y;
    newr.z = camera.vp.z + right.z - camera.pr.z;
    Normalise(&newr);
    newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
    newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
    newr.z = camera.pr.z + radius * newr.z - camera.vp.z;
    Normalise(&newr);
    
    camera.vd.x = camera.pr.x - camera.vp.x;
    camera.vd.y = camera.pr.y - camera.vp.y;
    camera.vd.z = camera.pr.z - camera.vp.z;
    Normalise(&camera.vd);
    
    /* Determine the new up vector */
    CROSSPROD(newr,camera.vd,camera.vu);
    Normalise(&camera.vu);
    
  } else {		/* Rotating about the camera position */
    
    // Calculate the new view direction 
    camera.vd.x += delta * ix * right.x + delta * iy * vu.x;
    camera.vd.y += delta * ix * right.y + delta * iy * vu.y;
    camera.vd.z += delta * ix * right.z + delta * iy * vu.z;
    Normalise(&camera.vd);
    
    // Determine the new up vector 
    CROSSPROD(right,camera.vd,camera.vu);
    Normalise(&camera.vu);
    
  }
  
  /* Update the focal point */
  Normalise(&camera.vd);
  camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
  camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
  camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;

}

// premultiply 4x4 matrix: mx -> mx*my
void premultiplyMatrix4x4(double mx[16], double my[16]) {
  double tx[16];
  int c, r; // col, row
  int i; // idx
  double f;
  for (c = 0; c < 4; c++) {
    for (r = 0; r < 4; r++) {
      f = 0.0;
      for (i = 0; i < 4; i++) {
	f += mx[i*4 + r] * my[c*4+i];
      }
      tx[c*4+r] = f;
    }
  }

  for (i = 0; i < 16; i++) {
    mx[i] = tx[i];
  }
}

// postmultiply 4x4 matrix: mx -> my*mx
void postmultiplyMatrix4x4(double mx[16], double my[16]) {
  double tx[16];
  int c, r; // col, row
  int i; // idx
  double f;
  for (c = 0; c < 4; c++) {
    for (r = 0; r < 4; r++) {
      f = 0.0;
      for (i = 0; i < 4; i++) {
	f += my[i*4 + r] * mx[c*4+i];
      }
      tx[c*4+r] = f;
    }
  }

  for (i = 0; i < 16; i++) {
    mx[i] = tx[i];
  }
}

// apply a 4x4 transformation matrix to a vector
XYZ apply4x4transformMatrix(double mx[16], XYZ a) {
  XYZ b;
  b.x = mx[0] * a.x + mx[4] * a.y + mx[8] * a.z;
  b.y = mx[1] * a.x + mx[5] * a.y + mx[9] * a.z;
  b.z = mx[2] * a.x + mx[6] * a.y + mx[10]* a.z;
  return b;
}


// compute the rotation matrix of an angle radians around the normalised
// axis axis, store in 4x4 mx.
void computeRotationMatrix4x4(double mx[16], XYZ axis, double radians) {
  Normalise(&axis);
  double cost = cos(radians);
  double sint = sin(radians);
  double u = axis.x;
  double v = axis.y;
  double w = axis.z;
  // see inside.mines.edu/fs_home/gmurray/ArbitraryAxisRotate 5.2
  mx[0] = u*u+(1-u*u)*cost;
  mx[4] = u*v*(1-cost) - w*sint;
  mx[8] = u*w*(1-cost) + v*sint;
  mx[12] = 0.0;
  
  mx[1] = u*v*(1-cost) + w*sint;
  mx[5] = v*v+(1-v*v)*cost;
  mx[9] = v*w*(1-cost) - u*sint;
  mx[13] = 0.0;

  mx[2] = u*w*(1-cost) - v*sint;
  mx[6] = v*w*(1-cost) + u*sint;
  mx[10]= w*w+(1-w*w)*cost;
  mx[14] = 0.0;

  mx[3] = mx[7] = mx[11] = 0.0;
  mx[15] = 1.0;
}


/* translate scene in camera frame */
void TranslateInCameraFrame(double sx, double sy, double sz) {
#if defined(BUILDING_S2PLOT) 
  if (!_s2_transcam_enable) {
    return;
  }
#endif
  double delta;
  
  /* Determine the amount to move */
  if (options.deltamove <= 0) {
    delta = VectorLength(pmin,pmax) / 2000;
  } else {
    delta = options.deltamove;
  }

  XYZ vd = camera.vd;
  Normalise(&vd);
  XYZ vu = camera.vu;
  Normalise(&vu);
  XYZ vr = CrossProduct(vd, vu);
  Normalise(&vr);
  
  // sx is movement along right vector
  _s2_object_trans = VectorAdd(_s2_object_trans, VectorMul(vr, sx * delta));
  
  // sy is movement along up vector
  _s2_object_trans = VectorAdd(_s2_object_trans, VectorMul(vu, sy * delta));

  // sz is -movement along viewdir
  _s2_object_trans = VectorAdd(_s2_object_trans, VectorMul(vd, sz * delta));
}


/*
	Fly the camera forward or backward at a given speed
	In INSPECT mode we just get shifted in increments.
	In WALK and FLY mode it is continuous
*/
void FlyCamera(double speed)
{
#if defined(BUILDING_S2PLOT) 
  if (!_s2_transcam_enable) {
    return;
  }
#endif
  double delta;
  XYZ vp;
  
  /* Determine the amount to move */
  if (options.deltamove <= 0)
    delta = speed * VectorLength(pmin,pmax) / 2000;
  else
    delta = speed * options.deltamove;

  if (options.interaction == OBJECT) {
    // in future this might be e.g. linked to a tracker view direction
    //_s2_object_trans.z -= delta;
    //camera.vp.z -= delta;
    TranslateInCameraFrame(0., 0., -speed);
    return;
  } 
  
#if defined(BUILDING_S2PLOT)
  // check we don't move through "the origin" = focus point
  XYZ newvp;
  newvp.x = camera.vp.x + delta * camera.vd.x;
  newvp.y = camera.vp.y + delta * camera.vd.y;
  newvp.z = camera.vp.z + delta * camera.vd.z;
  if (DotProduct(VectorSub(newvp, camera.pr), 
		 VectorSub(camera.vp, camera.pr)) <= 0) {
    // don't allow this to happen
    return;
  }
#endif
  
  /* Move the camera */
  vp.x = camera.vp.x + delta * camera.vd.x;
  vp.y = camera.vp.y + delta * camera.vd.y;
  vp.z = camera.vp.z + delta * camera.vd.z;
  
  /* Update the permanent settings */
  camera.vp = vp;
  if (options.projectiontype == PERSPECTIVE) {
    camera.pr.x += delta * camera.vd.x;
    camera.pr.y += delta * camera.vd.y;
    camera.pr.z += delta * camera.vd.z;
  }
  
  /* Update the focal distance */
  camera.focus.x = camera.vp.x + camera.focallength * camera.vd.x;
  camera.focus.y = camera.vp.y + camera.focallength * camera.vd.y;
  camera.focus.z = camera.vp.z + camera.focallength * camera.vd.z;
  
  if (options.interaction == INSPECT) {
    CameraHome(FOCUS);
  }
}

/*
	Calculate the model bounds
	Update pmin,pmax,pmid
	Optionally update rangemin,rangemax
*/
void CalcBounds(void) {
  int i;
  XYZ p;

#if defined(BUILDING_S2PLOT)
  pmin.x = _s2devicemin[_S2XAX];
  pmax.x = _s2devicemax[_S2XAX];
  pmin.y = _s2devicemin[_S2YAX];
  pmax.y = _s2devicemax[_S2YAX];
  pmin.z = _s2devicemin[_S2ZAX];
  pmax.z = _s2devicemax[_S2ZAX];
#else
  pmin.x = 1e32;  pmin.y = 1e32;  pmin.z = 1e32;
  pmax.x = -1e32; pmax.y = -1e32; pmax.z = -1e32;
#endif

  rangemin = 1e32;
  rangemax = -1e32;
  
  for (i=0;i<ndot;i++) {
#if defined(BUILDING_S2PLOT) 
    if (!strlen(dot[i].whichscreen))
#endif
      UpdateBounds(dot[i].p);
  }
  for (i=0;i<nball;i++) {
    p.x = ball[i].p.x + ball[i].r;
    p.y = ball[i].p.y + ball[i].r;
    p.z = ball[i].p.z + ball[i].r;
#if defined(BUILDING_S2PLOT)
    if (!strlen(ball[i].whichscreen))
#endif
      UpdateBounds(p);
    p.x = ball[i].p.x - ball[i].r;
    p.y = ball[i].p.y - ball[i].r;
    p.z = ball[i].p.z - ball[i].r;
#if defined(BUILDING_S2PLOT)
    if (!strlen(ball[i].whichscreen))
#endif
      UpdateBounds(p);
  }
  for (i=0;i<nballt;i++) {
    p.x = ballt[i].p.x + ballt[i].r;
    p.y = ballt[i].p.y + ballt[i].r;
    p.z = ballt[i].p.z + ballt[i].r;
#if defined(BUILDING_S2PLOT)
    if (!strlen(ballt[i].whichscreen))
#endif
      UpdateBounds(p);
    p.x = ballt[i].p.x - ballt[i].r;
    p.y = ballt[i].p.y - ballt[i].r;
    p.z = ballt[i].p.z - ballt[i].r;
#if defined(BUILDING_S2PLOT)
    if (!strlen(ballt[i].whichscreen))
#endif
      UpdateBounds(p);
  }
  for (i=0;i<ndisk;i++) {
    p.x = disk[i].p.x + disk[i].r2;
    p.y = disk[i].p.y + disk[i].r2;
    p.z = disk[i].p.z + disk[i].r2;
#if defined(BUILDING_S2PLOT)
    if (!strlen(disk[i].whichscreen))
#endif
      UpdateBounds(p);
    p.x = disk[i].p.x - disk[i].r2;
    p.y = disk[i].p.y - disk[i].r2;
    p.z = disk[i].p.z - disk[i].r2;
#if defined(BUILDING_S2PLOT)
    if (!strlen(disk[i].whichscreen))
#endif
      UpdateBounds(p);
  }
  for (i=0;i<ncone;i++) {
    p.x = cone[i].p1.x + cone[i].r1;
    p.y = cone[i].p1.y + cone[i].r1;
    p.z = cone[i].p1.z + cone[i].r1;
#if defined(BUILDING_S2PLOT)
    if (!strlen(cone[i].whichscreen))
#endif
      UpdateBounds(p);
    p.x = cone[i].p1.x - cone[i].r1;
    p.y = cone[i].p1.y - cone[i].r1;
    p.z = cone[i].p1.z - cone[i].r1;
#if defined(BUILDING_S2PLOT)
    if (!strlen(cone[i].whichscreen))
#endif
      UpdateBounds(p);
    
    p.x = cone[i].p2.x + cone[i].r2;
    p.y = cone[i].p2.y + cone[i].r2;
    p.z = cone[i].p2.z + cone[i].r2;
#if defined(BUILDING_S2PLOT)
    if (!strlen(cone[i].whichscreen))
#endif
      UpdateBounds(p);
    p.x = cone[i].p2.x - cone[i].r2;
    p.y = cone[i].p2.y - cone[i].r2;
    p.z = cone[i].p2.z - cone[i].r2;
#if defined(BUILDING_S2PLOT)
    if (!strlen(cone[i].whichscreen))
#endif
      UpdateBounds(p);
  }
  for (i=0;i<nline;i++) {
#if defined(BUILDING_S2PLOT)
    if (!strlen(line[i].whichscreen)) {
#endif
      UpdateBounds(line[i].p[0]);
      UpdateBounds(line[i].p[1]);
#if defined(BUILDING_S2PLOT)
    }
#endif
  }
  for (i=0;i<nface3;i++) {
#if defined(BUILDING_S2PLOT)
    if (!strlen(face3[i].whichscreen)) {
#endif
      UpdateBounds(face3[i].p[0]);
      UpdateBounds(face3[i].p[1]);
      UpdateBounds(face3[i].p[2]);
#if defined(BUILDING_S2PLOT)
    }
#endif
  }
#if defined(BUILDING_S2PLOT)
  for (i=0;i<nface3a;i++) {
    if (!strlen(face3a[i].whichscreen)) {
      UpdateBounds(face3a[i].p[0]);
      UpdateBounds(face3a[i].p[1]);
      UpdateBounds(face3a[i].p[2]);
    }
  }
#if defined(S2_3D_TEXTURES)
  for (i=0;i<ntexpoly3d;i++) {
    if (!strlen(texpoly3d[i].whichscreen)) {
      int j;
      for (j=0; j<texpoly3d[i].nverts;j++) {
	UpdateBounds(texpoly3d[i].verts[j]);
      }
    }
  }
#endif
  for (i = 0; i < ntexmesh; i++) {
    if (!strlen(texmesh[i].whichscreen)) {
      int j;
      for (j = 0; j < texmesh[i].nverts; j++) {
	UpdateBounds(texmesh[i].verts[j]);
      }
    }
  }
#endif
  for (i=0;i<nface4;i++) {
#if defined(BUILDING_S2PLOT)
    if (!strlen(face4[i].whichscreen)) {
#endif
      UpdateBounds(face4[i].p[0]);
      UpdateBounds(face4[i].p[1]);
      UpdateBounds(face4[i].p[2]);
      UpdateBounds(face4[i].p[3]);
#if defined(BUILDING_S2PLOT)
    }
#endif
  }
  for (i=0;i<nface4t;i++) {
#if defined(BUILDING_S2PLOT)
    if (!strlen(face4t[i].whichscreen)) {
#endif
      UpdateBounds(face4t[i].p[0]);
      UpdateBounds(face4t[i].p[1]);
      UpdateBounds(face4t[i].p[2]);
      UpdateBounds(face4t[i].p[3]);
#if defined(BUILDING_S2PLOT)
    }
#endif
  }

#if defined(BUILDING_S2PLOT)
  for (i = 0; i < ntrdot; i++) {
    if (!strlen(trdot[i].whichscreen)) {
      UpdateBounds(trdot[i].p);
    }
  }
#endif

  for (i=0;i<nlabel;i++)
#if defined(BUILDING_S2PLOT)
    if (!strlen(label[i].whichscreen)) 
#endif
      UpdateBounds(label[i].p);
#if defined(BUILDING_S2PLOT)
  for (i = 0; i < nlabel; i++) {
    if (!strlen(label[i].whichscreen)) {
      UpdateBounds(VectorAdd(VectorAdd(label[i].p, label[i].up), 
			     VectorMul(label[i].right, 
				       (double)strlen(label[i].s))));
    }
  }
#endif
  
  if (pmax.x <= pmin.x) { pmax.x = pmin.x; pmax.x += 0.01; pmin.x -= 0.01; }
  if (pmax.y <= pmin.y) { pmax.y = pmin.y; pmax.y += 0.01; pmin.y -= 0.01; }
  if (pmax.z <= pmin.z) { pmax.z = pmin.z; pmax.z += 0.01; pmin.z -= 0.01; }
  if (rangemin >= rangemax) { 
    rangemax = rangemin; 
    rangemin -= 0.01; 
    rangemax += 0.01; 
  }
  
  /* The center of the model */
#if defined(BUILDING_S2PLOT)
  if (!_s2_camexfocus) {
#endif
    pmid.x = (pmin.x + pmax.x) / 2;
    pmid.y = (pmin.y + pmax.y) / 2;
    pmid.z = (pmin.z + pmax.z) / 2;
#if defined(BUILDING_S2PLOT)
  } else {
    pmid = _s2_camfocus;
  }
#endif
  
  //fprintf(stderr, "in CalcBounds: pmin,pmax = (%f,%f,%f), (%f,%f,%f)\n", 
  //	  pmin.x, pmin.y, pmin.z, pmax.x, pmax.y, pmax.z);



  if (0 && options.debug) {
    fprintf(stderr,"Range: %g to %g\n",rangemin,rangemax);
  }
}

void UpdateBounds(XYZ p) {
  double dist;
  
  pmin.x = MIN(pmin.x,p.x);
  pmin.y = MIN(pmin.y,p.y);
  pmin.z = MIN(pmin.z,p.z);
  pmax.x = MAX(pmax.x,p.x);
  pmax.y = MAX(pmax.y,p.y);
  pmax.z = MAX(pmax.z,p.z);
  
  dist = VectorLength(p,camera.vp);
  rangemin = MIN(rangemin,dist);
  rangemax = MAX(rangemax,dist);
}

/*
	Add a line segment to the line database
*/
void AddLine2Database(XYZ p1,XYZ p2,COLOUR c1,COLOUR c2,double w) {
  if ((line = (LINE *)realloc(line,(nline+1)*sizeof(LINE))) == NULL) {
    _s2error("(internal)", "memory allocation for line failed");
  }
  w = ABS(w);
  line[nline].width = w;
  line[nline].p[0] = p1;
  line[nline].p[1] = p2;
  line[nline].colour[0] = c1;
  line[nline].colour[1] = c2;
#if defined(BUILDING_S2PLOT)
  strcpy(line[nline].whichscreen, _s2_whichscreen);
  strncpy(line[nline].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN-1);
  line[nline].VRMLname[MAXVRMLLEN-1] = '\0';
  line[nline].stipple_factor = 0;
  line[nline].stipple_pattern = 0;
#endif
  nline++;
}

/*
	Add a simple face to the database
	Scale and then translate it
*/
void AddFace2Database(XYZ *p,int n,COLOUR c,double scale,XYZ shift) {
  int i;
  
  if (n == 3) {
    if ((face3 = (FACE3 *)realloc(face3,(nface3+1)*sizeof(FACE3))) == NULL) {
      _s2error("(internal)", "memory allocation failed for face3");
    }
    for (i=0;i<3;i++) {
      face3[nface3].p[i].x = shift.x + scale*p[i].x;
      face3[nface3].p[i].y = shift.y + scale*p[i].y;
      face3[nface3].p[i].z = shift.z + scale*p[i].z;
      face3[nface3].n[i] = CalcNormal(p[0],p[1],p[2]);
      face3[nface3].colour[i] = c;
#if defined(BUILDING_S2PLOT)
      strcpy(face3[nface3].whichscreen, _s2_whichscreen);
      strncpy(face3[nface3].VRMLname, _s2_VRMLnames[_s2_currVRMLidx], MAXVRMLLEN);
      face3[nface3].VRMLname[MAXVRMLLEN-1] = '\0';
#endif
    }
    nface3++;
  } else if (n == 4) {
    if ((face4 = (FACE4 *)realloc(face4,(nface4+1)*sizeof(FACE4))) == NULL) {
      _s2error("(internal)", "memory allocation failed for face4");
    }
    for (i=0;i<4;i++) {
      face4[nface4].p[i].x = shift.x + scale*p[i].x;
      face4[nface4].p[i].y = shift.y + scale*p[i].y;
      face4[nface4].p[i].z = shift.z + scale*p[i].z;
      face4[nface4].n[i] = CalcNormal(p[(i-1+4)%4],p[i],p[(i+1)%4]);
      face4[nface4].colour[i] = c;
#if defined(BUILDING_S2PLOT)
      strcpy(face4[nface4].whichscreen, _s2_whichscreen);
      strncpy(face4[nface4].VRMLname, _s2_VRMLnames[_s2_currVRMLidx],
	      MAXVRMLLEN);
      face4[nface4].VRMLname[MAXVRMLLEN-1] = '\0';
#endif
    }
    nface4++;
  }
}
