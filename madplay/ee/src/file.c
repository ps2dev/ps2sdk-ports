/*
    -----------------------------------------------------------------------
    file.c - PS2MP3. (c) Ryan Kegel, 2004
	-----------------------------------------------------------------------

    This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

*/
#include "tamtypes.h"
#include <stdio.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include "sys/stat.h"
#include "sys/fcntl.h"
#include "kernel.h"
#include "sifrpc.h"
#include "stdarg.h"
#include "iopheap.h"
#include "sys/ioctl.h"
#include "fileXio_rpc.h"
#include "errno.h"
#include "string.h"
#include "libhdd.h"
#include "fileio.h"
#include "debug.h"
#include "sjpcm.h"
#include "sbv_patches.h"
#include "rmalloc.h"
#include "file.h"
#include "cdvd_rpc.h"
#include "loadfile.h"
#include "iopcontrol.h"

/*Store the current media being used, either Hard drive (MODE_HDD) or 
  CD drive (MODE_CD.)  Important in our directory functions.*/

int mediaMode = MODE_HOST; 
char elfPath[256];

void
setPathInfo(int argc, char **argv)
{
	char *ptr;
	char *bootPath;

    // argc == 0 usually means naplink..
    if (argc == 0) {
        bootPath = "host:";
    }
    // reload1 usually gives an argc > 60000 (yea, this is kinda a hack..)
    else if (argc > 60000) {
        bootPath = "mc0:/BWLINUX/";
    }
    else {
        bootPath = argv[0];
    }

    strncpy(elfPath, bootPath, 255);
    elfPath[255] = '\0';

    ptr = strrchr(elfPath, '/');
    if (ptr == NULL) {
        ptr = strrchr(elfPath, '\\');
        if (ptr == NULL) {
            ptr = strrchr(elfPath, ':');
            if (ptr == NULL) {
                scr_printf("Did not find path (%s)!\n", bootPath);
                SleepThread();
            }
        }
    }
    
	ptr++;
	*ptr = '\0';

    printf("path is %s\n", elfPath);
}


/****************************************************************************
 * Mounts a hard disk partition.											*
 ****************************************************************************/
int OpenPartition(char *part)
{
	int fd;
	fd = fileXioMount("pfs0:", part, FIO_MT_RDWR);
	if (fd < 0) { printf("Cannot mount partition\n"); return -1; }
	return 0;
}

/****************************************************************************
 * Unmounts a hard disk partition.											*
 ****************************************************************************/
int ClosePartition()
{
	fileXioUmount("pfs0:");
	return 0;
}

/****************************************************************************
 * Universal file opening function.  Returns the handle to the file.		*
 ****************************************************************************/
int OpenFile(char *filename, int mode, int media)
{
	int fd = 0;
	switch (media)
	{
	case 0:  //hdd
		{
			fd = fileXioOpen(filename, mode, 0);
			break;
		}
	case 1:  //cdfs
		{
//			CDVD_FlushCache();
// fixme: update to ps2sdk
			fd = fioOpen(filename, mode);
			break;
		}
	case 2:
		{
			//fd = fioOpen(filename, mode);
			fd = fileXioOpen(filename, mode, 0);
			break;
		}
	case 3:
		{
			fd = fileXioOpen(filename, mode, 0);
			break;	
		}
	case 4:
		{
			fd = fioOpen(filename, mode);
			break;	
		}
	case 5:
		{
			fd = fileXioOpen(filename, mode, 0);
			break;	
		}
	}
	return fd;
}

/****************************************************************************
 * Universal file closing function.											*
 ****************************************************************************/
void CloseFile(int handle, int media)
{
	switch (media)
	{
	case 0:  //hdd
		{
			fileXioClose(handle);
			break;
		}
	case 1:  //cdfs
		{
			fioClose(handle);
			//CDVD_Stop();
//			cdStop();
			break;
		}
	case 2: //mc0
		{
			//fioClose(handle);
			fileXioClose(handle);
			break;
		}	
	case 3:
		{
			//fioClose(handle);
			fileXioClose(handle);
			break;
		}		
	case 4:
		{
			fioClose(handle);
			break;
		}	
	case 5:
		{
			fileXioClose(handle);
			break;
		}			
	}
}


/****************************************************************************
 * Universal file reading function.  Returns the amount of bytes read.		*
 ****************************************************************************/
int ReadFile(int handle, unsigned char *buffer, int size, int media)
{
	int sr =0;
//	printf ("ReadFile(%d, %p, %d, %d)\n", handle, buffer, size, media);
	switch (media)
	{
	case 0:
		{
			sr = fileXioRead(handle, buffer, size);
			break;
		}
	case 1:
		{
			sr = fioRead(handle, buffer, size);
			break;
		}
	case 2:
		{
			//sr = fioRead(handle, buffer, size);
			sr = fileXioRead(handle, buffer, size);
			break;
		}
	case 3:
		{
			//sr = fioRead(handle, buffer, size);
			sr = fileXioRead(handle, buffer, size);
			break;
		}
	case 4:
		{
			sr = fioRead(handle, buffer, size);
			break;
		}
	case 5:
		{
			sr = fileXioRead(handle, buffer, size);
			break;
		}
	}
	return sr;
}

/****************************************************************************
 * Universal file seek function.											*
 ****************************************************************************/
int SeekFile(int handle, int pos, int rel, int media)
{
	int sr=0;
	switch (media)
	{
	case 0:
		{
			sr = fileXioLseek(handle, pos, rel);
			break;
		}
	case 1:
		{
			sr = fioLseek(handle, pos, rel);
			break;
		}
	case 2:
		{
			//sr = fioLseek(handle, pos, rel);
			sr = fileXioLseek(handle, pos, rel);
			break;
		}
	case 3:
		{
			//sr = fioLseek(handle, pos, rel);
			sr = fileXioLseek(handle, pos, rel);
			break;
		}
	case 4:
		{
			sr = fioLseek(handle, pos, rel);
			break;
		}
	case 5:
		{
			sr = fileXioLseek(handle, pos, rel);
			break;
		}
	}
	return sr;
}
	


/****************************************************************************
 * Helper function.  Not used anymore.						                  				*
 ****************************************************************************/
void closeShop(int handle)
{
	fileXioClose(handle);
	fileXioUmount("pfs0:");
}

//from now elf loading functions

/***************************************************************
 * Carica .ELF
 * LoadElf (original source by mrbrown in payload.c)
 ***************************************************************/
 typedef struct {
	u8	ident[16];
	u16	type;
	u16	machine;
	u32	version;
	u32	entry;
	u32	phoff;
	u32	shoff;
	u32	flags;
	u16	ehsize;
	u16	phentsize;
	u16	phnum;
	u16	shentsize;
	u16	shnum;
	u16	shstrndx;
} elf_header_t;

typedef struct {
	u32	type;
	u32	offset;
	void	*vaddr;
	u32	paddr;
	u32	filesz;
	u32	memsz;
	u32	flags;
	u32	align;
} elf_pheader_t;
 
 elf_header_t elfh;
#define ELF_MAGIC	0x464c457f
#define ELF_PT_LOAD	1

void Reset()
{
	SifIopReset("rom0:UDNL rom0:EELOADCNF",0);
	while (SifIopSync()) ;
	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();
	EI;
	SifInitRpc(0);
	FlushCache(0);
	FlushCache(2);
}

int RunElf(char *name)
{
	int fd,size,i;
	u8 *boot_elf = (u8 *) 0;//&_end;
	elf_header_t *eh = &elfh;
	elf_pheader_t *eph;
	char *argv[1];
	void *pdata;
	fd=-1;
	if(name[0]=='m' && name[1]=='c') // if mc, test mc0 and mc1
	{
		if((fd = fioOpen(name,1)) < 0) 
		{
			name[2]='1';
		}
	}
	if(fd < 0)
		if((fd = fioOpen(name,1)) < 0) 
		{
			return -1;
		}
		size = fioLseek(fd, 0, SEEK_END);
		if(!size) {
			fioClose(fd);
			return -2;
		}

		fioLseek(fd, 0, 0);
		fioRead(fd, eh, sizeof(elf_header_t)); // read the elf header

		// crazy code for crazy man :P
		boot_elf=(u8 *)0x1800000-size-256; 
		//if((eh->entry+size)>=boot_elf) boot_elf=(u8 *)eh->entry-size-256;
		boot_elf=(u8 *) (((unsigned)boot_elf) &0xfffffff0);

		// read rest file elf
		fioRead(fd, boot_elf+sizeof(elf_header_t), size-sizeof(elf_header_t));
		fioClose(fd);

		// mrbrown machine gun ;)
		eph = (elf_pheader_t *)(boot_elf + eh->phoff);
		// Scan through the ELF's program headers and copy them into RAM, then
		// zero out any non-loaded regions.
		for (i = 0; i < eh->phnum; i++) {
	   	   if (eph[i].type != ELF_PT_LOAD)
			   continue;
			   
		pdata = (void *)(boot_elf + eph[i].offset);
		memcpy2((unsigned char *)eph[i].vaddr, (unsigned char *)pdata, (int)eph[i].filesz);

		if (eph[i].memsz > eph[i].filesz)
			memset2((unsigned char *)eph[i].vaddr + eph[i].filesz, (unsigned char)0, (int)eph[i].memsz - eph[i].filesz);
	}

	// Let's go.
	argv[0] = name;

	fioExit();
	Reset();
	FlushCache(0);
	FlushCache(2);
	ExecPS2((void *)eh->entry, 0, 1, argv);
	return 0;
}

void memcpy2(unsigned char *dest,unsigned char *org,int ndata)
{
	int n;
	for(n=0;n<ndata;n++)
	{
		dest[n]=org[n];
	}
}

void memset2(unsigned char *dest,unsigned char val,int ndata)
{
	int n;
	for(n=0;n<ndata;n++)
	{
		dest[n]=val;
	}
}
