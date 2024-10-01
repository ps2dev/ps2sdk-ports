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
#include <sys/stat.h>
#include <fcntl.h>
#include <kernel.h>
#include <sifrpc.h>
#include "stdarg.h"
#include "iopheap.h"
#include "sys/ioctl.h"
#include "errno.h"
#include "string.h"
#include "libhdd.h"
#include "debug.h"
#include "sjpcm.h"
#include <sbv_patches.h>
#include "file.h"
#include <loadfile.h>
#include "iopcontrol.h"
#include <unistd.h>
#include <stdlib.h>

/*Store the current media being used, either Hard drive (MODE_HDD) or 
  CD drive (MODE_CD.)  Important in our directory functions.*/

char elfPath[255];

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
    elfPath[254] = '\0';

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
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();
//	EI;
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
		if((fd = open(name,1)) < 0) 
		{
			name[2]='1';
		}
	}
	if(fd < 0)
		if((fd = open(name,1)) < 0) 
		{
			return -1;
		}
		size = lseek(fd, 0, SEEK_END);
		if(!size) {
			close(fd);
			return -2;
		}

		lseek(fd, 0, 0);
		read(fd, eh, sizeof(elf_header_t)); // read the elf header

		// crazy code for crazy man :P
		boot_elf=(u8 *)0x1800000-size-256; 
		//if((eh->entry+size)>=boot_elf) boot_elf=(u8 *)eh->entry-size-256;
		boot_elf=(u8 *) (((unsigned)boot_elf) &0xfffffff0);

		// read rest file elf
		read(fd, boot_elf+sizeof(elf_header_t), size-sizeof(elf_header_t));
		close(fd);

		// mrbrown machine gun ;)
		eph = (elf_pheader_t *)(boot_elf + eh->phoff);
		// Scan through the ELF's program headers and copy them into RAM, then
		// zero out any non-loaded regions.
		for (i = 0; i < eh->phnum; i++) {
	   	   if (eph[i].type != ELF_PT_LOAD)
			   continue;
			   
		pdata = (void *)(boot_elf + eph[i].offset);
		memcpy((unsigned char *)eph[i].vaddr, (unsigned char *)pdata, (int)eph[i].filesz);

		if (eph[i].memsz > eph[i].filesz)
			memset((unsigned char *)eph[i].vaddr + eph[i].filesz, (unsigned char)0, (int)eph[i].memsz - eph[i].filesz);
	}

	// Let's go.
	argv[0] = name;

	Reset();
	FlushCache(0);
	FlushCache(2);
	ExecPS2((void *)eh->entry, 0, 1, argv);
	return 0;
}

