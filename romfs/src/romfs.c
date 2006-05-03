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
#include "sys.h"

typedef struct 
{
	char	magic[8];		/* Should be "-rom1fs-" */
	uint32	full_size;		/* Full size of the file system */
	uint32	checksum;		/* Checksum */
	char	volume_name[16];	/* Volume name (zero-terminated) */
} romdisk_hdr_t;

typedef struct {
	uint32	next_header;		/* Offset of next header */
	uint32	spec_info;		/* Spec info */
	uint32	size;			/* Data size */
	uint32	checksum;		/* File checksum */
	char	filename[16];		/* File name (zero-terminated) */
} romdisk_file_t;

static uint8 const *mounted_img = 0;

int romdisk_mount(const void *img)
{
	const romdisk_hdr_t *hdr;

	/* make sure romfs is sane */
	hdr = (romdisk_hdr_t *)img;
	if (strncmp(hdr->magic, "-rom1fs-", 8) != 0) {
		return -1;
	}

	mounted_img = (const uint8 *)img;
	return 0;
}

int romdisk_umount()
{
	mounted_img = 0;
	return 0;
}

int romdisk_find(const char *path, void **ptr, int *size)
{
	const romdisk_hdr_t *hdr;
	const romdisk_file_t *fhdr;
	const char *subdir;
	int path_len;
	int type;
	uint32 offset;

	if (mounted_img == NULL || ptr == NULL) {
		return -1;
	}

	hdr = (romdisk_hdr_t *)mounted_img;

	/* look for the directory containing the file */
	offset = sizeof(romdisk_hdr_t) + (strlen(hdr->volume_name) & ~0x0f);
	//printf("starting at offset 0x%x\n", offset);

	if (path[0] == '/')
	{
		/* skip leading '/' */
		path++;
	}

	subdir = strchr(path, '/');
	while (subdir != 0)
	{
		path_len = (subdir - path);
		//printf("looking for directory '%s' len=%d\n", path, path_len);

		while (offset != 0)
		{
			fhdr = (const romdisk_file_t *)(mounted_img + offset);
			type = READ_BE_UINT32(&fhdr->next_header) & 0x0f;
			
			//printf("comparing against %s\n", fhdr->filename);
			if (strncasecmp(fhdr->filename, path, path_len) == 0)
			{
				/* found our directory */
				offset = READ_BE_UINT32(&fhdr->spec_info);
				break;
			}

			offset = READ_BE_UINT32(&fhdr->next_header) & ~0x0f;
		}

		if (offset == 0)
		{
			/* directory not found */
			printf("directory not found\n");
			return 2;
		}

		//printf("directory found at offset 0x%x\n", offset);

		path = subdir + 1;
		subdir = strchr(path, '/'); 
	}

	/* now look for the file */
	if (strlen(path) == 0)
	{
		printf("empty file name\n");
		return 2;
	}

	while (offset != 0)
	{
		fhdr = (const romdisk_file_t *)(mounted_img + offset);
		type = READ_BE_UINT32(&fhdr->next_header) & 0x0f;
			
		//printf("looking for file %s at offset 0x%x (%s)\n", path, offset, fhdr->filename);
		if (strcasecmp(fhdr->filename, path) == 0)
		{
			/* found our file */
			*ptr = (void *)(mounted_img + offset + sizeof(romdisk_file_t) + (strlen(fhdr->filename) & ~0x0f));
			if (size != 0)
			{
				*size = READ_BE_UINT32(&fhdr->size);
			}

			break;
		}

		offset = READ_BE_UINT32(&fhdr->next_header) & ~0x0f;
	}

	if (offset == 0)
	{
		printf("file not found\n");
		return 2;
	}

	return 0;
}
