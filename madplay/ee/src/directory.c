/*
    -----------------------------------------------------------------------
    directory.c - PS2MP3. (c) Ryan Kegel, 2004
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
#include "cdvd_rpc.h"
#include "bstdfile.h"
#include "rmalloc.h"
#include "libpad.h"
#include "file.h"
#include "directory.h"

/****************************************************************************
 * Universal folder structure for CD and DVD directories					*
 ****************************************************************************/

struct folder folder;

/****************************************************************************
 * Helper function to update the current directory							*
 ****************************************************************************/
void resetDirectory(char *dir)
{
	strcpy(folder.directory, dir);
	//strcpy(folder.oldDirectory, dir);
}


/****************************************************************************
 * This function opens the directory in the folder struct.  It should only	*
 * be called 1 time.  It has no effect for CD directories.					* 
 ****************************************************************************/
int openDirectory(char *dir, int media)
{
	strcpy(folder.directory, dir);
	switch (media)
	{
	case 0:
		{
			folder.iDir = fileXioDopen(dir);
			if (folder.iDir < 0)
				return -1;
			return 0;
		}
	case 1:
		{
			return 0;
		}
	}
	return 0;
}

/****************************************************************************
 * This function closes the directory in the folder struct.					*
 * It has no effect for CD directories.										* 
 ****************************************************************************/
int closeDirectory(int media)
{
	switch (media)
	{
	case 0:
		{
			fileXioDclose(folder.iDir);
			folder.iDir = -1;
			return 0;
		}
	case 1:
		{
			return 0;
		}
	}
	return 0;
}

/****************************************************************************
 * This function changes the directory in the folder struct to a new		*
 * directory.  It has no effect for CD directories.							* 
 ****************************************************************************/
int changeDirectory(char *dir, int media)
{
	int len, i, k=0;
	if (strcmp(dir, "..") == 0) //prev folder
	{
		len = strlen(folder.directory);
		k = 0;
		for (i=len-2; i >= 0; i--)
		{
			if (folder.directory[i] == '/')
			{
				k = 1;
				break;
			}
		}
		if (k == 1)
		{
			folder.directory[i+1] = 0;
		}
	}
	else if (strcmp(dir, ".") == 0) //current folder
	{
		return 0;
	}
	else
	{
		k = dir[4];
		dir[4] = 0;
		if (strcmp(dir, "pfs0") == 0)
		{
			dir[4] = k;
			strcpy(folder.directory, dir);
		}
		else if (strcmp(dir, "cdfs") == 0)
		{
			dir[4] = k;
			strcpy(folder.directory, dir);
		}
		else
		{
			dir[4] = k;
			strcat(folder.directory, dir);
			strcat(folder.directory, "/");
		}
	}
	printf("Directory is %s\n", folder.directory);
	switch (media)
	{
	case 0:
		{
			fileXioDclose(folder.iDir);
			fileXioChdir(folder.directory);
			folder.iDir = fileXioDopen(folder.directory);
			return 1;
		}
	case 1:
		{
			return 1;
			fioDclose(folder.iDir);
			folder.iDir = fioDopen(folder.directory);
		}
	}
	return 0;
}

/****************************************************************************
 * This function will read the contents of a directory and store the		*
 * contents in the folder structure.  The ext parameter works as a filter	* 
 * to read in only certain file extension types.  After reading the			* 
 * directory, the function will sort the contents.							* 
 ****************************************************************************/
int readDirectory(char *ext, int media)
{
	int ret = 1;
	iox_dirent_t directory;
	int size;
	char folderName[255];
	unsigned int numToc, index;
	char *extcmp;
	folder.fIndex = 0;
	struct TocEntry cdDirectory[255];
	size = strlen(ext);

	switch (media)
	{
	case 0:
		{
			while (ret > 0)
			{
				ret = fileXioDread(folder.iDir, &directory);
				if (ret > 0)
				{
					if (FIO_S_ISDIR(directory.stat.mode))  //is a directory
					{
						strcpy(folder.object[folder.fIndex].name, "");
						strcat(folder.object[folder.fIndex].name, "/");
						strcat(folder.object[folder.fIndex].name, directory.name);
						folder.object[folder.fIndex].type = 0;
						folder.object[folder.fIndex].count = folder.fIndex;
						folder.fIndex++;
					}
					else if (FIO_S_ISREG(directory.stat.mode)) //is a file
					{
						if (size > ret)
							size = 0;
						extcmp = &directory.name[ret-size];
						if (strcasecmp(extcmp, ext) == 0)
						{
							strcpy(folder.object[folder.fIndex].name, directory.name);
							folder.object[folder.fIndex].type = 1;
							folder.object[folder.fIndex].count = folder.fIndex;
							folder.fIndex++;
						}
					}
				}
			}
			folder.fMax = folder.fIndex;
			folder.fIndex = 0;
			sortFolder();
			for (ret = 0; ret < folder.fMax; ret++)
			{
				if (folder.object[ret].type == 0)
				{
					strcpy(folderName, &folder.object[ret].name[1]);
					strcpy(folder.object[ret].name, folderName);
				}
			}
			closeDirectory(MODE_HDD);
			return folder.fMax;
		}
	case 1:
		{
//			numToc = CDVD_GetDir(&folder.directory[5], NULL, CDVD_GET_FILES_AND_DIRS, cdDirectory, 254, NULL);
// fixme: update to ps2sdk
//			CDVD_Stop();
			cdStop();

#define CD_S_ISDIR(x) x & 2
#define CD_S_ISFILE(x) !CD_S_ISDIR(x)

			for (index=0; index<numToc; index++)
			{
				if (CD_S_ISDIR(cdDirectory[index].fileProperties)) //is a folder
				{
					strcpy(folder.object[folder.fIndex].name, "");
					strcat(folder.object[folder.fIndex].name, "/");
					strcat(folder.object[folder.fIndex].name, cdDirectory[index].filename);
					folder.object[folder.fIndex].type = 0;
					folder.object[folder.fIndex].count = folder.fIndex;
					folder.fIndex++;
				}
				else
				{
					ret = strlen(cdDirectory[index].filename);
					if (size > ret)
						size = 0;
					extcmp = &cdDirectory[index].filename[ret-size];
					if (strcasecmp(extcmp, ext) == 0)
					{
						strcpy(folder.object[folder.fIndex].name, cdDirectory[index].filename);
						folder.object[folder.fIndex].type = 1;
						folder.object[folder.fIndex].count = folder.fIndex;
						folder.fIndex++;
					}
				}
			}
			folder.fMax = folder.fIndex;
			folder.fIndex = 0;
			sortFolder();
			for (ret = 0; ret < folder.fMax; ret++)
			{
				if (folder.object[ret].type == 0)
				{
					strcpy(folderName, &folder.object[ret].name[1]);
					strcpy(folder.object[ret].name, folderName);
				}
			}
			return folder.fMax;
		}
	}
	return 0;
}

/****************************************************************************
 * Returns the current directory.											*
 ****************************************************************************/

char *getCurrentDirectory()
{
	return folder.directory;
}


/****************************************************************************
 * Not used																	*
 ****************************************************************************/
int currentIndex()
{
	return folder.fIndex;
}

/****************************************************************************
 * Not used																	*
 ****************************************************************************/
int currentType()
{
	return folder.object[folder.fIndex].type;
}

/****************************************************************************
 * Not used																	*
 ****************************************************************************/
char *currentName()
{
	return folder.object[folder.fIndex].name;
}

/****************************************************************************
 * Not used																	*
 ****************************************************************************/
int incrementDirectory()
{
	folder.fIndex++;
	if (folder.fIndex == folder.fMax)
	{
		folder.fIndex--;
		return 0;
	}
	return 1;
}

/****************************************************************************
 * Not used																	*
 ****************************************************************************/
int decrementDirectory()
{
	folder.fIndex--;
	if (folder.fIndex < 0)
	{
		folder.fIndex = 0;
		return 0;
	}
	return 1;
}

/****************************************************************************
 * Not used																	*
 ****************************************************************************/
char *currentFullName()
{
	strcpy(folder.full, folder.directory);
	strcat(folder.full, "/");
	strcat(folder.full, folder.object[folder.fIndex].name);
	return folder.full;
}

/****************************************************************************
 * Sorts directory contents in alphabetical order							*
 ****************************************************************************/
void sortFolder()
{
	char swapName[255];
	char swapType;
	char swapFlag;

	int size = folder.fMax;
	int i, j;
	for (i=0; i<size; i++)
	{
		for (j=1; j<size-i; j++)
		{
			if (strcasecmp(folder.object[j-1].name, folder.object[j].name) > 0) //s2 comes first
			{
				strcpy(swapName, folder.object[j-1].name);
				swapType  = folder.object[j-1].type;
				swapFlag  = folder.object[j-1].flag;
				//swapCount = folder.object[j-1].count;
				
				strcpy(folder.object[j-1].name, folder.object[j].name);
				folder.object[j-1].type  = folder.object[j].type;
				folder.object[j-1].flag  = folder.object[j].flag;
				//folder.object[j-1].count = folder.object[j].count;
				
				strcpy(folder.object[j].name, swapName);
				folder.object[j].type  = swapType;
				folder.object[j].flag  = swapFlag;
				//folder.object[j].count = swapCount;
			}
		}
	}
}


/****************************************************************************
 * This will fill an array of max object structures with information on		*
 * the type of object such as file or folder.								*
 * Used for displaying a portion of the contents inside a directory			*
 ****************************************************************************/
int fillObjectInfo(int current, int max, struct object *obj[])
{
	int i;
	if (current >= folder.fMax)
		return 0;
	if (current < 0)
		return 0;

	for (i=0; i<max; i++)
	{
		if (i+current == folder.fMax)
			break;
		obj[i] = &folder.object[i+current];
	}
	return i;
}


/****************************************************************************
 * Returns the amount of files and folders are in the current directory		*
 ****************************************************************************/
int maxObjects()
{
	return folder.fMax;
}

