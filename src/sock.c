/* sock.c
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
 * original version of sock.c: Willem van Straten, c. 1996
 * - contributed to S2PLOT: 20080904
 *
 * $Id: sock.c 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#include "sock.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#ifdef LINUX
#include <sys/ioctl.h>
#endif

#include <netdb.h>

#ifdef _REENTRANT
#include <pthread.h>
static pthread_mutex_t sock_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

char* sock_herrstr (int h_error)
{
  switch (h_error) {
  case HOST_NOT_FOUND:
    return ("HOST_NOT_FOUND");
  case NO_ADDRESS:
    return ("NO_ADDRESS");
  case NO_RECOVERY:
    return ("NO_RECOVERY");
  case TRY_AGAIN:
    return ("TRY_AGAIN");
  default:
    return ("unknown error code");
  }
}


int sock_getname (char *self, int length, int alias)
{
  int ret;
  struct hostent *hp;
  struct in_addr in;
  
  ret = gethostname (self, length);
  if (ret < 0) {
    perror ("sock_getname: gethostname");
    return -1;
  }
  
  if (!alias) {
    
#ifdef _REENTRANT
    pthread_mutex_lock (&sock_mutex);
#endif
    
    hp = gethostbyname(self);
    
    if (hp) {
      memcpy (&in.s_addr, *(hp->h_addr_list), sizeof (in.s_addr));
      strncpy (self, inet_ntoa(in), length);
    }
    
#ifdef _REENTRANT
    pthread_mutex_unlock (&sock_mutex);
#endif
    
    if (!hp) {
      fprintf (stderr, "sock_getname: gethostbyname: %s\n",
	       sock_herrstr(h_errno));
      return -1;
    }
    
  }
  
  return 0;
}

int sock_create (int *port)
{
  struct sockaddr_in server;
  int fd;
  unsigned length;
  int on = 1; 
  
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  {
    perror ("sock_create: (err) socket");
    return -1;
  }
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(*port);
  
  /* redwards -- trying following to prevent the dreaded 
     bind() address already in use */
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); 
  
  if (bind(fd, (struct sockaddr*) &server, sizeof(server)))  {
    perror ("sock_create: (err) bind");
    return -1;
  }
  length = sizeof(struct sockaddr_in);
  if (getsockname(fd, (struct sockaddr*)&server, &length)) {
    perror ("sock_create: (err) getsockname");
    return -1;
  }
  *port = ntohs(server.sin_port);
  
  /* listen for up to ten queued connection requests */
  if (listen(fd, 10) < 0)  {
    perror ("sockCreate: (err) listen");
    return -1;
  }
  return fd;
}


int sock_accept (int fd)
{
  int new_fd;

#ifdef MSDOS
  struct sockaddr_in dummy;
  int dummy_size;
  new_fd = accept (fd, &dummy, &dummy_size);
#else
  new_fd = accept (fd, (struct sockaddr *)NULL, NULL);
#endif
  
  return new_fd;
}

/* opens an existing socket connection */
int sock_open (const char *host, int port)
{
  struct hostent *hp;
  struct sockaddr_in server;
  int fd;
  
#ifdef _REENTRANT
  pthread_mutex_lock (&sock_mutex);
#endif
  
  hp = gethostbyname(host);
  
  if (hp) {
    
    server.sin_family = AF_INET;
    
#ifdef MSDOS
    memset((char *)&server, 0, sizeof(server));
    memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
#else
    memcpy(&(server.sin_addr.s_addr), hp->h_addr, hp->h_length);
#endif
    server.sin_port = htons(port);
    
  }
  
#ifdef _REENTRANT
  pthread_mutex_unlock (&sock_mutex);
#endif
  
  if (hp == NULL)  {
    fprintf (stderr, "sock_open: host information for %s not found: %s\n",
	     host, sock_herrstr(h_errno));
    return -1;
  }
  
  /* create a socket and connect to the low level controller */
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  {
    perror ("sock_open: (err) socket");
    return -1;
  }
  if (connect (fd, (struct sockaddr*)&server, sizeof(struct sockaddr))<0)  {
    perror ("sock_open: (err) connect");
    close (fd);
    return -1;
  }
  
  return fd;
}

int sock_close (int fd)
{
  if (shutdown (fd, SHUT_RDWR) < 0) {
    perror ("sock_close: (err) shutdown");
    return -1;
  }
  
  if (close (fd) < 0) {
    perror ("sock_close: (err) close");
    return -1;
  }
  return 0;
}


int sock_ready (int fd, int* to_read, int* to_write, float timeout)
{
  int ret=0;
  
  struct timeval* tmoutp = NULL;
  struct timeval tmout;

  fd_set *rdsp = NULL;
  fd_set readset;
  
  fd_set *wrsp = NULL;
  fd_set writeset;
  
  if (to_read) {
    FD_ZERO (&readset);
    FD_SET (fd, &readset);
    rdsp = &readset;
  }
  if (to_write) {
    FD_ZERO (&writeset);
    FD_SET (fd, &writeset);
    wrsp = &writeset;
  }
  if (timeout >= 0.0) {
    tmout.tv_sec =  (long) timeout;
    tmout.tv_usec = (long) ( (timeout - (float) tmout.tv_sec) * 1e6 );
    tmoutp = &tmout;
  }
  
  ret = select (fd+1, rdsp, wrsp, NULL, tmoutp);
  if (ret < 0)  {
    perror ("sock_ready: (err) select");
    return -1;
  }
  if (!ret)
    return 0;
  
  if (to_read && FD_ISSET(fd,&readset)) {
    *to_read = 1;
    ret = 1;
  }
  if (to_write && FD_ISSET(fd,&writeset)) {
    *to_write = 1;
    ret = 1;
  }
  
  return ret;
}



/* returns a value of zero if no bytes were read before timeout seconds
// have passed, -1 on error. */
int sock_tm_read (int fd, void* buf, size_t size, float timeout)
				 /*(fd, buf, size, timeout)*/
				 /*     int   fd;
					char* buf;
					size_t   size;
					float timeout;*/
{
  int rd;
  int retval = sock_ready (fd, &rd, NULL, timeout);
  if (retval < 0)
    return -1;
  else if (retval > 0)
    return sock_read (fd, buf, size);
  return 0;
}


int sock_tm_write (int fd, void* buf, size_t size, float timeout)
  
/* (fd, buf, size, timeout)
   int    fd;
   void*  buf;
   size_t size;
   float  timeout; */
{
  int wt;
  int retval = sock_ready (fd, NULL, &wt, timeout);
  if (retval < 0)
    return -1;
  else if (retval > 0)
    return sock_write (fd, buf, size);
  return 0;
}


int sock_read (int fd, void *buf, size_t size)
{
  int ret;
  
  ret = read (fd, buf, size);
  if (ret == -1)
    {
#ifdef __alpha
      if ( errno == EAGAIN || errno == EWOULDBLOCK )
#endif
#ifdef sgi
        if ( errno == EAGAIN || errno == EWOULDBLOCK )
#endif
#ifdef sun
	  if ( errno == EAGAIN )
#endif
#ifdef _OSK
	    if ( errno == EWOULDBLOCK )
#endif
	      ret = 0;
    }
  
  return ret;
}

int sock_write (int fd, const void *buf, size_t size)
{
  int ret;
  
  ret = write (fd, buf, size);
  if (ret == -1)
    {
#ifdef __alpha
      if ( errno == EAGAIN || errno == EWOULDBLOCK )
#endif
#ifdef sgi
        if ( errno == EAGAIN || errno == EWOULDBLOCK )
#endif
#ifdef sun
	  if ( errno == EAGAIN )
#endif
#ifdef _OSK
	    if ( errno == EWOULDBLOCK )
#endif
	      ret = 0;
    }
  
  return ret;
}

#ifdef _OSK
int sock_block (fd)
     int fd;
{
  struct sgbuf buf;
  _gs_opt(fd,&buf);
  buf.sg_noblock = 0;
  _ss_opt(fd,&buf);
}

int sock_nonblock (fd)
     int fd;
{
  struct sgbuf buf;
  _gs_opt(fd,&buf);
  buf.sg_noblock = 1;
  _ss_opt(fd,&buf);
}


#else

int sock_block (int fd)
{
  int flags;
  flags = fcntl(fd,F_GETFL);
  flags &= ~(O_NONBLOCK);
  return fcntl(fd,F_SETFL,flags);
}

int sock_nonblock (int fd)
{
  int flags;
  flags = fcntl(fd,F_GETFL);
  flags |= O_NONBLOCK;
  return fcntl(fd,F_SETFL,flags);
}
  
#endif
