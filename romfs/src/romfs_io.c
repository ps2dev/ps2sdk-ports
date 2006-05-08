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



#include <stdio.h>
#include <string.h>
#include <io_common.h>

// handle romdisk 
#include "romfs.h"

/*---	 Private declaration	 ---*/

FILE 		__riob[_NFILE];
int 		__sz[_NFILE];                 //size  of  file from romfs
int 		__offset[_NFILE];             //offset  ... 
unsigned char *(__dataptr[_NFILE]);  	      //datptr ... 



static int rio_initialized = 0;

/*---	 romfs functions	 ---*/

void rioInit()
{
	if (!rio_initialized)
	{
		int i;
		// init all descriptors 
		// except stdout, stdin and stderr (even if not relevent here)
		for(i=3;i<_NFILE;i++)
		{
			__riob[i].fd = -1;
			__sz[i] = 0;
			__offset[i] = 0;
			__dataptr[i] = 0;
		}
		// mount the rom image
		romdisk_mount(romdisk_start);
		printf ("romdisk initialized & mounted!\n");
		rio_initialized =1;
	}	
}

int rioOpen(char* path, int mode)
{
	int i, fd;

	if (!rio_initialized)
	{
	  printf("ROMFS: file sytem not initialized!");
	  return -1;
	}
	
	if ((mode & O_RDONLY) != O_RDONLY)
	{
	  printf("ROMFS: Warning, ROMFS is a Read-Only Device !");
	}

	fd = -1;
	for(i=3;i<_NFILE;i++)
	{
		if(__riob[i].fd == -1)
		{
			fd = i;
			break;
		}
	}

	if(fd==-1)
	  return -1;

	printf("romfs opening: %s as %i\n",path, fd);
	
	if (romdisk_find(path, (void **)&(__dataptr[fd]), &(__sz[fd])) == 0)
	{
		// set the new fd
		 __riob[i].fd = fd;
		return fd;
	}
	else
	{
		printf("failed to open %s\n", path);
		return -1;
	}
}

int 	rioRead(int fd, void *buff, size_t buff_size)
{
	// check if we read beyond the array bounce
	if ((__offset[fd] + buff_size ) > __sz[fd])
	{
		// max readable bytes..
		buff_size = __sz[fd] - __offset[fd] + 1;
	}
	if (buff_size > 0)
 	   memcpy(buff, (__dataptr[fd]) + __offset[fd], buff_size);
	
	__offset[fd] += buff_size;
		
	return buff_size;
}

int 	rioWrite(int fd, const void *buff, size_t buff_size)
{
	printf("ROMFS: Write Operation not possible !");
	return 0;	
}	

int 	rioClose(int fd)
{	
	__riob[fd].fd = -1;
	__sz[fd] = 0;
	__offset[fd] = 0;
	__dataptr[fd] = 0;
	
	return 0;
}

int	rioSize(int fd) {
	return __sz[fd];
}

int	rioLseek( int fd, int off, int whence)
{
	switch (whence) 
	{
	   case SEEK_CUR :
	   	break;
	   
	   case SEEK_SET : 
	   	__offset[fd] = 0;
	   	break;
	   
	   case SEEK_END :
	   	__offset[fd] = __sz[fd];
		break;	   	
	   default : 
	   	return -EINVAL;
	   	break;
	}
		
	__offset[fd] += off;
	return 0;
}


int	rioGetc(int fd) 
{
	int c;
	
	if ((__offset[fd]) >= __sz[fd])
  	  return EOF;
	
	c = *(__dataptr[fd] + __offset[fd]);
	__offset[fd]++;
	return c;
	

}


int	rioGets(int fd, char* buff, int n)
{
	// Rather than doing a slow byte-at-a-time read
	// read upto the max langth, then re-position file afterwards
	int read;
	int i;

	read = rioRead(fd, &buff[0], n);

    	for (i=0; i<(read-1); i++)
	{
		
		switch (buff[i])
		{
		  case '\n':
			rioLseek(fd, (i + 1) - read, SEEK_CUR);
			buff[i+1]=0;	// terminate after newline
			return i;

		  case 0:
			rioLseek(fd, i-read, SEEK_CUR);
            	  return i;
		}
	}

	
	// if we reached here, then we havent found the end of a string
	return i;
}

/**************************************************************/

FILE	*ropen(const char * fname, const char * mode)
{
	int fd;
	
	// check mode for some useful warning...
	while (*mode)
	{
		if (*mode == '+' || *mode == 'a' || *mode == 'w') {
			printf("ROMFS: Warning, ROMFS is a Read-Only Device !");
			break;
		}
		mode++;
	}
	
	fd = rioOpen ((char *)fname, O_RDONLY);
	
	if (fd == -1)	
	  return NULL;
	  
	// return the corresponding FILE* 
	return &__riob[fd];
}

int    	rclose(FILE * stream)
{
	rioClose (stream->fd);
	return 0;
}

size_t  rread(void *buf, size_t r, size_t n, FILE *stream)
{
	return (size_t)((rioRead(stream->fd, buf, (int)r * (int)n)) / r);
}
size_t 	rwrite(const void *buf, size_t r, size_t n, FILE *stream)
{
	printf("ROMFS: Write Operation not possible !");
	return 0;
}
int    	rseek(FILE *stream, long offset, int origin)
{
	return rioLseek( stream->fd, offset, origin);
}
long 	rtell(FILE *stream)
{
	return (long)__offset[stream->fd];
}
int 	rsize(FILE *stream)
{
	return rioSize(stream->fd);
}
int     rgetc(FILE *stream)
{
	return rioGetc(stream->fd);
}
char    *rgets(char *buf, int n, FILE *stream)
{
	rioGets(stream->fd, buf, n);
	return (buf);
}

/**************************************************************/

void *rmmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
	// we ignore parameters : addr, len, prot & flags
	if (fildes ==-1)
	   return ((void *)-1);
	
	// if off beyond file size
	if (off > __sz[fildes])
	   return ((void *)-1);

	// we just return the memory address at off offset
	return (__dataptr[fildes] + (int)off);
}

