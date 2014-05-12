/* sock.h
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
 * original version: Willem van Straten, c. 1996
 * - contributed to S2PLOT: 20080904
 *
 * $Id: sock.h 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#ifndef __DADA_SOCK_H
#define __DADA_SOCK_H

#include <sys/types.h>

#if defined(__cplusplus) && !defined(S2_CPPBUILD)
extern "C" {
#endif
  
  int sock_getname (char* self, int length, int alias);
  
  int sock_create (int* port);
  int sock_accept (int fd);
  
  int sock_open (const char* host, int port);
  int sock_close (int fd);
  
  int sock_read (int fd, void* buf, size_t size);
  int sock_write (int fd, const void* buf, size_t size);
  
  int sock_ready (int fd, int* to_read, int* to_write, float timeout);
  int sock_tm_read (int fd, void* buf, size_t size, float timeout);
  int sock_tm_write (int fd, void* buf, size_t size, float timeout);
  
  int sock_block (int fd);
  int sock_nonblock (int fd);
  
#if defined(__cplusplus) && !defined(S2_CPPBUILD)
}
#endif

#endif /* SOCK_H */

