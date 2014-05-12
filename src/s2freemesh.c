/* s2freemesh.c
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
 * $Id: s2freemesh.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "s2types.h"

MESHNODE **mesh;
int meshtype;
int meshnx;
int meshny;

/* BINARY mesh file handling code */

/* is this machine little endian?
 */
int isLittleEndian() {
  int i = 1;
  char *p = (char *) &i;
  if (p[0] == 1) {
    // Lowest address contains the least significant byte
    return 1;
  } else {
    return 0;
  }
}

/* Reverse the ordering of bytes in segments of length chunksize, and
 * return the number reordered.
 */
unsigned int ReverseBytes(unsigned char *buf, unsigned int len,
                          unsigned int chunksize) {
  // verify we have an integer number of segments
  if (len % chunksize) {
    return 0;
  }
  // verify we have an even chunksize
  if (chunksize % 2) {
    return 0;
  }

  // count how many we do, and go...
  unsigned int ndone = 0;
  unsigned int i;
  unsigned char tmp;
  unsigned char *ptr;
  for (ptr = buf; ptr < buf + len; ptr += chunksize) {
    for (i = 0; i < chunksize / 2; i++) {
      tmp = ptr[i];
      ptr[i] = ptr[chunksize-1-i];
      ptr[chunksize-1-i] = tmp;
    }
    ndone++;
  }
  return ndone;
}


/* write a mesh file in binary form.  Our binary files are stored
 * on disk in little-endian format (ie. x86).  Binary format files
 * have two ascii lines at the start, specifically as per ascii
 * format mesh file: line 1 contains the meshtype, line 2 contains
 * the x and y dimensions of the mesh.  Then the mesh is written as 
 * a contiguous binary chunk, 6 doubles per mesh coordinate, in the
 * same order as the ascii file.  We choose to use floats rather than
 * doubles.  Even though meshfn stores the values as doubles, this 
 * is overkill, and certainly doubles are not presently represented
 * properly in the ascii mesh files.
 */
#define BINTYPE float
int WriteBinMesh(char *fname) {
  FILE *fptr;
  BINTYPE *data;
  int i, j;
  int idx;
  unsigned char *cdata;

  if (strlen(fname) < 1) {
    return(FALSE);
  }

  if ((fptr = fopen(fname, "w")) == NULL) {
    fprintf(stderr, "Failed to open binary map file for writing\n");
    return(FALSE);
  }

  // write the mesh type
  fprintf(fptr, "%d\n", meshtype);

  // write the mesh dimensions
  fprintf(fptr, "%d %d\n", meshnx, meshny);

  // allocate mem to store the entire mesh in continguous BINTYPE form
  data = (BINTYPE *)calloc(meshnx * meshny * 5, sizeof(BINTYPE));
  idx = 0;
  for (j = 0; j < meshny; j++) {
    for (i = 0; i < meshnx; i++) {
      data[idx++] = mesh[i][j].u;
      data[idx++] = mesh[i][j].v;
      data[idx++] = mesh[i][j].x;
      data[idx++] = mesh[i][j].y;
      data[idx++] = mesh[i][j].i;
    }
  }
  
  cdata  = (unsigned char *)data;

  // switch byte ordering if necessary
  if (!isLittleEndian()) {
    int nrev = ReverseBytes(cdata, meshnx * meshny * 5 * sizeof(BINTYPE), 
			    sizeof(BINTYPE));
    if (nrev != meshnx * meshny * 5) {
      fprintf(stderr, "Did not reverse correct number of values!\n");
      fclose(fptr);
      free(data);
      return(FALSE);
    }
  }
  
  // write the chunk to the file
  fwrite(cdata, sizeof(unsigned char), meshnx * meshny * 5 * sizeof(BINTYPE),
	 fptr);
  
  fclose(fptr);
  free(data);
  return(TRUE);
}


int ReadMesh(char *fname) {
  FILE *fptr;
  char string[255];

  if (strlen(fname) < 1) {
    return(FALSE);
  }

  if ((fptr = fopen(fname, "r")) == NULL) {
    fprintf(stderr, "Failed to open binary map file for reading\n");
    return(FALSE);
  }
  fprintf(stderr, "opened file %s\n", fname);

  // Get the mesh type
  fgets(string, 254, fptr);
  if (sscanf(string,"%d",&meshtype) != 1) {
    fprintf(stderr,"Failed to read the mesh type\n");
    fclose(fptr);
    return(FALSE);
  }
  if (meshtype != POLAR && meshtype != RECTANGULAR) {
    fprintf(stderr,"Failed to get a recognised map type (%d)\n",meshtype);
    fclose(fptr);
    return(FALSE);
  }
  if (meshtype != RECTANGULAR) {
    fprintf(stderr,"Currently only support a rectangular\n");
    fclose(fptr);
    return(FALSE);
  }
  
  // Get the dimensions
  fgets(string, 254, fptr);
  if (sscanf(string ,"%d %d",&meshnx,&meshny) != 2) {
    fprintf(stderr,"Failed to read the mesh dimensions\n");
    fclose(fptr);
    return(FALSE);
  }
  if (meshnx < 4 || meshny < 4 || meshnx > 100000 || meshny > 100000) {
    fprintf(stderr,"Didn't read acceptable mesh resolution (%d,%d)\n",
	    meshnx,meshny);
    fclose(fptr);
    return(FALSE);
  }

  int i, j;

  // Create new mesh
  mesh = (MESHNODE **)malloc(meshnx*sizeof(MESHNODE *));
  for (i=0;i<meshnx;i++) {
    mesh[i] = (MESHNODE *)malloc(meshny*sizeof(MESHNODE));
  }

  for (j = 0; j < meshny; j++) {
    for (i = 0; i < meshnx; i++) {

      fgets(string, 254, fptr);
      if (sscanf(string ,"%lf %lf %lf %lf %lf",&mesh[i][j].u, &mesh[i][j].v, 
		 &mesh[i][j].x, &mesh[i][j].y, &mesh[i][j].i) != 5) {
	fprintf(stderr,"Failed to read the mesh dimensions\n");
	fclose(fptr);
	return(FALSE);
      }
    }
  }

  fclose(fptr);
  return TRUE;
}
  

int ReadBinMesh(char *fname) {
  FILE *fptr;
  BINTYPE *data;
  int i, j;
  int idx;
  unsigned char *cdata;
  unsigned int nread;
  char string[255];
  
  if (strlen(fname) < 1) {
    return(FALSE);
  }

  if ((fptr = fopen(fname, "r")) == NULL) {
    fprintf(stderr, "Failed to open binary map file for reading\n");
    return(FALSE);
  }
  fprintf(stderr, "opened file %s\n", fname);

  // Get the mesh type
  fgets(string, 254, fptr);
  if (sscanf(string,"%d",&meshtype) != 1) {
    fprintf(stderr,"Failed to read the mesh type\n");
    fclose(fptr);
    return(FALSE);
  }
  if (meshtype != POLAR && meshtype != RECTANGULAR) {
    fprintf(stderr,"Failed to get a recognised map type (%d)\n",meshtype);
    fclose(fptr);
    return(FALSE);
  }
  if (meshtype != RECTANGULAR) {
    fprintf(stderr,"Currently only support a rectangular\n");
    fclose(fptr);
    return(FALSE);
  }
  
  // Get the dimensions
  fgets(string, 254, fptr);
  if (sscanf(string ,"%d %d",&meshnx,&meshny) != 2) {
    fprintf(stderr,"Failed to read the mesh dimensions\n");
    fclose(fptr);
    return(FALSE);
  }
  if (meshnx < 4 || meshny < 4 || meshnx > 100000 || meshny > 100000) {
    fprintf(stderr,"Didn't read acceptable mesh resolution (%d,%d)\n",
	    meshnx,meshny);
    fclose(fptr);
    return(FALSE);
  }
  
  // Create new mesh
  mesh = (MESHNODE **)malloc(meshnx*sizeof(MESHNODE *));
  for (i=0;i<meshnx;i++) {
    mesh[i] = (MESHNODE *)malloc(meshny*sizeof(MESHNODE));
  }

  cdata = (unsigned char *)calloc(meshnx * meshny * 5 * sizeof(BINTYPE),
				  sizeof(unsigned char));
  nread = fread((void *)cdata, sizeof(unsigned char), 
		meshnx * meshny * 5 * sizeof(BINTYPE), fptr);
  if (nread != meshnx * meshny * 5 * sizeof(BINTYPE)) {
    fprintf(stderr, "Did not read correct number of points\n");
    fclose(fptr);
    free(cdata);
    return(FALSE);
  }

  if (!isLittleEndian()) {
    fprintf(stderr, "Swapping bytes...\n");
    int nrev = ReverseBytes(cdata, meshnx * meshny * 5 * sizeof(BINTYPE),
			    sizeof(BINTYPE));
    if (nrev != meshnx * meshny * 5) {
      fprintf(stderr, "Did not reverse correct number of values!\n");
      fclose(fptr);
      free(cdata);
    }
  }

  // stuff values into struct array
  data = (BINTYPE *)cdata;
  idx = 0;
  for (j = 0; j < meshny; j++) {
    for (i = 0; i < meshnx; i++) {
      mesh[i][j].u = data[idx++];
      mesh[i][j].v = data[idx++];
      mesh[i][j].x = data[idx++];
      mesh[i][j].y = data[idx++];
      mesh[i][j].i = data[idx++];
    }
  }

  free(cdata);
  fclose(fptr);
  return(TRUE);
}

/* here is a code segment that will create a binary mesh file from
 * an ascii version ...

  if (ReadMesh("uvxy.map")) {
    if (WriteBinMesh("uvxy.bin.map")) {
      fprintf(stderr, "ok!\n");
    }
  }

*/

