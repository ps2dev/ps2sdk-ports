/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2005, ps2dev - http://www.ps2dev.org
# Licenced under GNU Library General Public License version 2
# Review ps2sdk README & LICENSE files for further details.
#
# ROMFS.
*/

#ifndef _ROMFS_IO_
#define _ROMFS_IO_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void 	rioInit();


int 	rioOpen(char* fname, int mode);
int 	rioClose(int fd);

int 	rioRead(int fd, void *buff, size_t buff_size);
int 	rioWrite(int fd, const void *buff, size_t buff_size);
int 	rioLseek( int fd, int off, int whence);
int 	rioSize(int file);

int 	rioGetc(int fd);
int 	rioGets(int fd, char* buff, int n);

/**************************************************************/

FILE	*ropen(const char * fname, const char * mode);
int    	rclose(FILE *);

size_t  rread(void *buf, size_t r, size_t n, FILE *stream);
size_t 	rwrite(const void *buf, size_t r, size_t n, FILE *stream);
int    	rseek(FILE *stream, long offset, int origin);
long 	rtell(FILE *stream);
int 	rsize(FILE *stream);
int     rgetc(FILE *stream);
char    *rgets(char *buf, int n, FILE *stream);


/**************************************************************/

 
void *rmmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off); 
  


#ifdef __cplusplus
}
#endif


#endif

