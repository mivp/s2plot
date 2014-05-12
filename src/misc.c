/* misc.c
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
 * $Id: misc.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

/*
   Create a simple sphere
	twidth and theight allow us to deal with power of 2 (or not) textures
*/       
void CreateASphere(XYZ c,double r,int n,int twidth,int theight)
{        
   int i,j,nn;
   double theta1,theta2,theta3;
   XYZ e,p;

   if (r < 0)
      r = -r;
   if (n < 0)
      n = -n;
   if (n < 4 || r <= 0) {
      glBegin(GL_POINTS);
      glVertex3f(c.x,c.y,c.z);
      glEnd();
      return;
   }     

   /* Make n even */
   n /= 2;
   n *= 2;
   nn = n/2;

   for (j=0;j<nn;j++) {
      theta1 = j * TWOPI / n - PID2; 
      theta2 = (j + 1) * TWOPI / n - PID2;

      glBegin(GL_QUAD_STRIP);

      for (i=0;i<=n;i++) {
	theta3 = i * TWOPI / n;

         e.x = cos(theta2) * cos(theta3);
         e.y = sin(theta2);
         e.z = cos(theta2) * sin(theta3);
         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;

         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(twidth*i/(double)n,2*theight*(j+1)/(double)n);
         glVertex3f(p.x,p.y,p.z);

         e.x = cos(theta1) * cos(theta3);
         e.y = sin(theta1);
         e.z = cos(theta1) * sin(theta3);
         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;
         
         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(twidth*i/(double)n,2*theight*j/(double)n);
         glVertex3f(p.x,p.y,p.z);
      }
      glEnd();
   }
}

/*
   Create a planet, which is a sphere, but with control over 
   placement of texture, and then rotation of sphere.
*/       
void CreateAPlanet(XYZ c,double r,int n,int twidth,int theight,
		   float texture_phase, XYZ axis, float rotation)
{        
   int i,j,nn;
   double theta1,theta2,theta3;
   XYZ e,p;

   if (r < 0)
      r = -r;
   if (n < 0)
      n = -n;
   if (n < 4 || r <= 0) {
      glBegin(GL_POINTS);
      glVertex3f(c.x,c.y,c.z);
      glEnd();
      return;
   }     

   /* Make n even */
   n /= 2;
   n *= 2;
   nn = n/2;

   for (j=0;j<nn;j++) {
      theta1 = j * TWOPI / n - PID2; 
      theta2 = (j + 1) * TWOPI / n - PID2;

      glBegin(GL_QUAD_STRIP);

      for (i=0;i<=n;i++) {
	//theta3 = i * TWOPI / n;
	theta3 = ((float)i / (float)n + texture_phase) * TWOPI;

         e.x = cos(theta2) * cos(theta3);
         e.y = sin(theta2);
         e.z = cos(theta2) * sin(theta3);
	 e = ArbitraryRotate(e, rotation*DTOR, axis);

         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;

         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(twidth*i/(double)n,2*theight*(j+1)/(double)n);
         glVertex3f(p.x,p.y,p.z);

         e.x = cos(theta1) * cos(theta3);
         e.y = sin(theta1);
         e.z = cos(theta1) * sin(theta3);
	 e = ArbitraryRotate(e, rotation*DTOR, axis);

         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;
         
         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(twidth*i/(double)n,2*theight*j/(double)n);
         glVertex3f(p.x,p.y,p.z);
      }
      glEnd();
   }
}

/*
   Display the program usage information
*/
void GiveUsage(char *cmd)
{
static char *_s2x_helpstring = 
"Command line options    -h         this text\n\
                        -s         active stereo mode\n\
                        -ss        dual screen (passive) stereo\n\
                        -si        interleave stereo\n\
                        -sa        anaglyph (red-cyan) stereo\n\
                        -f         full screen mode\n\
                        -fps n     target frame rate\n\
                        -r [wfds]  render mode\n\
                        -as        start in autospin mode\n\
                        -ap fn     start in autopilot mode\n\
                        -og        open geom file\n\
                        -oo        open off file\n\
                        -vf        open view file\n\
                        -bg r g b  background colour (default=black)\n\
                        -sh n      change default material shininess\n\
                        -dm n      delta movement\n\
                        -bb        show bounding box (default=off)\n\
                        -cw        clockwise polygons (default=CCW)\n\
                        -cf        back face culling (default=off)\n\
                        -fly       start off in fly mode\n\
                        -walk      start in walking mode\n\
                        -m         make mouse visible (default: invisible)\n\
                        -sr n      sphere resolution (default: 8)\n\
                        -sc x y z  scale factors for three coordinates\n\
";
   fprintf(stderr,"Usage: %s [command line options]\n",cmd);
   fprintf(stderr,"%s\n",_s2x_helpstring);
   exit(-1);
}

int ReadVector(FILE *fptr,XYZ *p)
{
   if (fscanf(fptr,"%lf %lf %lf",&(p->x),&(p->y),&(p->z)) != 3)
      return(FALSE);
   return(TRUE);
}

int ReadColour(FILE *fptr,COLOUR *c)
{
   if (fscanf(fptr,"%lf %lf %lf",&(c->r),&(c->g),&(c->b)) != 3)
      return(FALSE);
   ClipColour(c);
   return(TRUE);
}


