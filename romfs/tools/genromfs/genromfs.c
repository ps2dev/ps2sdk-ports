/* Generate a ROMFS file system
 *
 * Copyright (C) 1997,1998	Janos Farkas <chexum@shadow.banki.hu>
 * Copyright (C) 1998		Jakub Jelinek <jj@ultra.linux.cz>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * Changes:
 *	2 Jan 1997				Initial release
 *      6 Aug 1997				Second release
 *     11 Sep 1998				Alignment support
 *     21 Feb 2001				Cygwin build fixes
 *						(Florian Schulze, Brian Peek)
 */

/*
 * Some sparse words about how to use the program
 *
 * `genromfs' is the `mkfs' equivalent of the other filesystems, but
 * you must tell it from which directory you want to build the
 * filesystem.  I.e. all files (and directories) in that directory
 * will be part of the newly created filesystem.  Imagine it like
 * building a cd image, or creating an archive (tar, zip) file.
 *
 * Basic usage:
 *
 * # genromfs -d rescue/ -f /dev/fd0
 *
 * All files in the rescue directory will be written to /dev/fd0 as a
 * new romfs filesystem image.  You can mount it (if you have the
 * romfs module loaded, or compiled into the kernel) via:
 *
 * # mount -t romfs /dev/fd0 /mnt
 *
 * You can also set the volume name of the filesystem (which is not
 * currently used by the kernel) with the -V option.  If you don't
 * specify one, genromfs will create a volume name of the form: 'rom
 * xxxxxxxx', where the x's represent the current time in a cryptic
 * form.
 *
 * All in all, it's as simple as:
 *
 * # genromfs -d rescue -f testimg.rom -V "Install disk"
 *
 * Other options:
 * -a N	 force all regular file data to be aligned on N bytes boundary
 * -A N,/name force named file(s) (shell globbing applied against the filenames)
 *       to be aligned on N bytes boundary
 * In both cases, N must be a power of two.
 */

/*
 * Warning!  Quite spaghetti code, it was born in a few hours.
 * Sorry about that.  Feel free to contact me if you have problems.
 */

#ifdef __CYGWIN__
#define _WIN32
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#ifdef _WIN32
#	include <getopt.h>
#else
#	include <fnmatch.h>
#endif /* _WIN32 */
#include <sys/time.h>
#include <netinet/in.h>
#if !defined(__FreeBSD__) && !defined(__NetBSD__)
#	include <sys/sysmacros.h>
#endif

#ifdef _WIN32
typedef unsigned short umode_t;
typedef signed char __s8;
typedef unsigned char __u8;
typedef signed short __s16;
typedef unsigned short __u16;
typedef signed int __s32;
typedef unsigned int __u32;
#else
typedef unsigned short umode_t;
typedef __signed__ char __s8;
typedef unsigned char __u8;
typedef __signed__ short __s16;
typedef unsigned short __u16;
typedef __signed__ int __s32;
typedef unsigned int __u32;
#endif

struct romfh {
	int nextfh;
	int spec;
	int size;
	int checksum;
};

#define ROMFS_MAXFN 128
#define ROMFH_HRD 0
#define ROMFH_DIR 1
#define ROMFH_REG 2
#define ROMFH_LNK 3
#define ROMFH_BLK 4
#define ROMFH_CHR 5
#define ROMFH_SCK 6
#define ROMFH_FIF 7
#define ROMFH_EXEC 8

struct filenode;

struct filehdr {
	struct filenode *head;
	struct filenode *tail;
	struct filenode *tailpred;
};

struct filenode {
	struct filenode *next;
	struct filenode *prev;
	struct filehdr dirlist;
	struct filenode *orig_link;
	char *name;
	char *realname;
	dev_t ondev;
	dev_t devnode;
	ino_t onino;
	mode_t modes;
	unsigned int offset;
	unsigned int size;
	unsigned int pad;
};

struct aligns {
	int align;
	struct aligns *next;
	char pattern[0];
};

void initlist(struct filehdr *fh)
{
	fh->head = (struct filenode *)&fh->tail;
	fh->tail = NULL;
	fh->tailpred = (struct filenode *)&fh->head;
}

int listisempty(struct filehdr *fh)
{
	return fh->head == (struct filenode *)&fh->tail;
}

void append(struct filehdr *fh, struct filenode *n)
{
	struct filenode *tail = (struct filenode *)&fh->tail;

	n->next = tail; n->prev = tail->prev;
	tail->prev = n; n->prev->next =n;
}

void shownode(int level, struct filenode *node, FILE *f)
{
	struct filenode *p;
	fprintf(f, "%-4d %-20s [0x%-8x, 0x%-8x] %07o, sz %5u, at 0x%-6x", level, node->name,
		(int)node->ondev, (int)node->onino, node->modes, node->size, node->offset);
	if (node->orig_link)
		fprintf(f, " [link to 0x%-6x]", node->orig_link->offset);
	fprintf(f, "\n");
	p = node->dirlist.head;
	while (p->next) {
		shownode(level+1, p, f);
		p = p->next;
	}
}

/* Dumping functions */

static char bigbuf[4096];
static char fixbuf[512];
static int atoffs = 0;
static int align = 16;
struct aligns *alignlist = NULL;
int realbase;

int findalign(struct filenode *node)
{
	struct aligns *pa;
	int i;
	
	if (S_ISREG(node->modes))
		i = align;
	else
		i = 16;
	for (pa = alignlist; pa; pa = pa->next) {
		if (pa->align > i) {
			if (pa->pattern[0] == '/') {
#ifndef _WIN32
				if (!fnmatch(pa->pattern, node->realname + realbase, FNM_PATHNAME|FNM_PERIOD))
					i = pa->align;
#else
				printf("pa->pattern: %s\n", pa->pattern);
				printf("node->realname + realbase: %s\n", node->realname + realbase);
#endif
			} else {
#ifndef _WIN32
				if (!fnmatch(pa->pattern, node->name, FNM_PATHNAME|FNM_PERIOD))
					i = pa->align;
#else
				printf("pa->pattern: %s\n", pa->pattern);
				printf("node->name: %s\n", node->name);
#endif
			}
		}
	}
	return i;
}

int romfs_checksum(void *data, int size)
{
        __s32 sum, *ptr;

        sum = 0; ptr = data;
        size>>=2;
        while (size>0) {
                sum += htonl(*ptr++);
                size--;
        }
        return sum;
}

void fixsum(struct romfh *ri, int size)
{
	ri->checksum = 0;
	ri->checksum = ntohl(-romfs_checksum(ri, size));
}

void dumpdata(void *addr, int len, FILE *f)
{
	int tocopy;
	struct romfh *ri;

	if (!len)
		return;
	if (atoffs >= 512) {
		fwrite(addr, len, 1, f);
		atoffs+=len;
		return;
	}

	tocopy = len < 512-atoffs ? len : 512-atoffs;
	memcpy(fixbuf+atoffs, addr, tocopy);
	atoffs+=tocopy;
	addr=(char*)addr+tocopy;
	len-=tocopy;

	if (atoffs==512) {
		ri = (struct romfh *)&fixbuf;
		fixsum(ri, atoffs<ntohl(ri->size)?atoffs:ntohl(ri->size));
		fwrite(fixbuf, atoffs, 1, f);
	}
	if (len) {
		fwrite(addr, len, 1, f);
		atoffs+=len;
	}
}

void dumpzero(int len, FILE *f)
{
	memset(bigbuf, 0, len);
	dumpdata(bigbuf, len, f);
}

void dumpdataa(void *addr, int len, FILE *f)
{
	dumpdata(addr, len, f);
	if ((len & 15) != 0)
		dumpzero(16-(len&15), f);
}

void dumpstring(char *str, FILE *f)
{
	dumpdataa(str, strlen(str)+1, f);
}

void dumpri(struct romfh *ri, struct filenode *n, FILE *f)
{
	int len;

	len = strlen(n->name)+1;
	memcpy(bigbuf, ri, sizeof(*ri));
	memcpy(bigbuf+16, n->name, len);
	if (len&15) {
		memset(bigbuf+16+len, 0, 16-(len&15));
		len += 16-(len&15);
	}
	len+=16;
	ri=(struct romfh *)bigbuf;
	if (n->offset)
		fixsum(ri, len);
	dumpdata(bigbuf, len, f);
#if 0
	fprintf(stderr, "RI: [at %06x] %08lx, %08lx, %08lx, %08lx [%s]\n",
		n->offset,
		ntohl(ri->nextfh), ntohl(ri->spec),
		ntohl(ri->size), ntohl(ri->checksum),
		n->name);
#endif
}

void dumpnode(struct filenode *node, FILE *f)
{
	struct romfh ri;
	struct filenode *p;

	ri.nextfh = 0;
	ri.spec = 0;
	ri.size = htonl(node->size);
	ri.checksum = htonl(0x55555555);
	if (node->pad)
		dumpzero(node->pad, f);
	if (node->next && node->next->next)
		ri.nextfh = ntohl(node->next->offset);
	if ((node->modes & 0111) &&
	    (S_ISDIR(node->modes) || S_ISREG(node->modes)))
		ri.nextfh |= ntohl(ROMFH_EXEC);

	if (node->orig_link) {
		ri.nextfh |= ntohl(ROMFH_HRD);
		/* Don't allow hardlinks to convey attributes */
		ri.nextfh &= ~ntohl(ROMFH_EXEC);
		ri.spec = ntohl(node->orig_link->offset);
		dumpri(&ri, node, f);
	} else if (S_ISDIR(node->modes)) {
		ri.nextfh |= ntohl(ROMFH_DIR);
		if (listisempty(&node->dirlist)) {
			ri.spec = ntohl(node->offset);
		} else {
			ri.spec = ntohl(node->dirlist.head->offset);
		}
		dumpri(&ri, node, f);
	} else if (S_ISLNK(node->modes)) {
		ri.nextfh |= ntohl(ROMFH_LNK);
		dumpri(&ri, node, f);
		memset(bigbuf, 0, sizeof(bigbuf));
		readlink(node->realname, bigbuf, node->size);
		dumpdataa(bigbuf, node->size, f);
	} else if (S_ISREG(node->modes)) {
		int offset, len, fd, max, avail;
		ri.nextfh |= ntohl(ROMFH_REG);
		dumpri(&ri, node, f);
		offset = 0;
		max = node->size;
		/* XXX warn about size mismatch */
#ifdef _WIN32
		fd = open(node->realname, O_RDONLY | O_BINARY);
#else
		fd = open(node->realname, O_RDONLY);
#endif
		if (fd) {
			while(offset < max) {
				avail = max-offset < sizeof(bigbuf) ? max-offset : sizeof(bigbuf);
				len = read(fd, bigbuf, avail);
				if (len <= 0)
					break;
				dumpdata(bigbuf, len, f);
				offset+=len;
			}
			close(fd);
		}
		max = (max+15)&~15;
		while (offset < max) {
			avail = max-offset < sizeof(bigbuf) ? max-offset : sizeof(bigbuf);
			dumpdata(bigbuf, avail, f);
			offset+=avail;
		}
	} else if (S_ISCHR(node->modes)) {
		ri.nextfh |= ntohl(ROMFH_CHR);
		ri.spec = ntohl(major(node->devnode)<<16|minor(node->devnode));
		dumpri(&ri, node, f);
	} else if (S_ISBLK(node->modes)) {
		ri.nextfh |= ntohl(ROMFH_BLK);
		ri.spec = ntohl(major(node->devnode)<<16|minor(node->devnode));
		dumpri(&ri, node, f);
	} else if (S_ISFIFO(node->modes)) {
		ri.nextfh |= ntohl(ROMFH_FIF);
		dumpri(&ri, node, f);
	} else if (S_ISSOCK(node->modes)) {
		ri.nextfh |= ntohl(ROMFH_SCK);
		dumpri(&ri, node, f);
	}

	p = node->dirlist.head;
	while (p->next) {
		dumpnode(p, f);
		p = p->next;
	}
}

void dumpall(struct filenode *node, int lastoff, FILE *f)
{
	struct romfh ri;
	struct filenode *p;

	ri.nextfh = htonl(0x2d726f6d);
	ri.spec = htonl(0x3166732d);
	ri.size = htonl(lastoff);
	ri.checksum = htonl(0x55555555);
	dumpri(&ri, node, f);
	p = node->dirlist.head;
	while (p->next) {
		dumpnode(p, f);
		p = p->next;
	}
	/* Align the whole bunch to ROMBSIZE boundary */
	if (lastoff&1023)
		dumpzero(1024-(lastoff&1023), f);
}

/* Node manipulating functions */

void freenode(struct filenode *n)
{
	/* Rare, not yet */
}

void setnode(struct filenode *n, dev_t dev, ino_t ino, mode_t um)
{
	n->ondev = dev;
	n->onino = ino;
	n->modes = um;
}

struct filenode *newnode(const char *base, const char *name, int curroffset)
{
	struct filenode *node;
	int len;
	char *str;

	node = malloc(sizeof (*node));
	if (!node) {
		fprintf(stderr,"out of memory\n");
		exit(1);
	}

	len = strlen(name);
	str = malloc(len+1);
	if (!str) {
		fprintf(stderr,"out of memory\n");
		exit(1);
	}
	strcpy(str, name);
	node->name = str;

	if (!curroffset) {
		len = 1;
		name = ".";
	}
	if (strlen(base))
		len++;
	str = malloc(strlen(base)+len+1);
	if (!str) {
		fprintf(stderr,"out of memory\n");
		exit(1);
	}
	if (strlen(base)) {
		sprintf(str, "%s/%s", base, name);
	} else {
		strcpy(str, name);
	}

	node->realname = str;
	node->next = node->prev = NULL;
	initlist(&node->dirlist);

	node->ondev = -1;
	node->onino = -1;
	node->modes = -1;
	node->size = 0;
	node->devnode = 0;
	node->orig_link = NULL;
	node->offset = curroffset;
	node->pad = 0;

	return node;
}

struct filenode *findnode(struct filenode *node, dev_t dev, ino_t ino)
{
	struct filenode *found, *p;

	/* scan the whole tree */
	if (node->ondev == dev && node->onino == ino)
		return node;
	p = node->dirlist.head;
	while (p->next) {
		found = findnode(p, dev, ino);
		if (found)
			return found;
		p = p->next;
	}
	return NULL;
}

#define ALIGNUP16(x) (((x)+15)&~15)

int spaceneeded(struct filenode *node)
{
	return 16 + ALIGNUP16(strlen(node->name)+1) + ALIGNUP16(node->size);
}

int alignnode(struct filenode *node, int curroffset, int extraspace)
{
	int align = findalign(node), d;
			
	d = ((curroffset + extraspace) & (align - 1));
	if (d) {
		align -= d;
		curroffset += align;
		node->offset += align;
		node->pad = align;
	}
	return curroffset;
}

int processdir(int level, const char *base, const char *dirname, struct stat *sb,
	struct filenode *dir, struct filenode *root, int curroffset)
{
	DIR *dirfd;
	struct dirent *dp;
	struct filenode *n, *link;

	if (level <= 1) {
		/* Ok, to make sure . and .. are handled correctly
		 * we add them first.  Note also that we alloc them
		 * first to get to know the real name
		 */
		link = newnode(base, ".", curroffset);
		if (!lstat(link->realname, sb)) {
			setnode(link, sb->st_dev, sb->st_ino, sb->st_mode);
			append(&dir->dirlist, link);
			curroffset = alignnode(link, curroffset, 0) + spaceneeded(link);
			n = newnode(base, "..", curroffset);
			if (!lstat(n->realname, sb)) {
				setnode(n, sb->st_dev, sb->st_ino, sb->st_mode);
				append(&dir->dirlist, n);
				n->orig_link = link;
				curroffset = alignnode(n, curroffset, 0) + spaceneeded(n);
			}
		}
	}

	dirfd = opendir(dir->realname);
	while((dp = readdir(dirfd))) {
		/* don't process main . and .. twice */
		if (level <= 1 &&
		    (strcmp(dp->d_name, ".") == 0
		     || strcmp(dp->d_name, "..") == 0))
			continue;
		n = newnode(base, dp->d_name, curroffset);
		if (lstat(n->realname, sb)) {
			fprintf(stderr, "ignoring '%s' (lstat failed)\n", n->realname);
			freenode(n); continue;
		}
		setnode(n, sb->st_dev, sb->st_ino, sb->st_mode);
		/* Skip unreadable files/dirs */
		if (!S_ISLNK(n->modes) && access(n->realname, R_OK)) {
			fprintf(stderr, "ignoring '%s' (access failed)\n", n->realname);
			freenode(n); continue;
		}
		/* Look up old links */
		link = findnode(root, n->ondev, n->onino);
		append(&dir->dirlist, n);
		if (link) {
			n->orig_link = link;
			curroffset = alignnode(n, curroffset, 0) + spaceneeded(n);
			continue;
		}
		if (S_ISREG(sb->st_mode)) {
			curroffset = alignnode(n, curroffset, spaceneeded(n));
			n->size = sb->st_size;
		} else
			curroffset = alignnode(n, curroffset, 0);
		if (S_ISLNK(sb->st_mode)) {
			n->size = sb->st_size;
		}
		curroffset += spaceneeded(n);
		if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode)) {
			n->devnode = sb->st_rdev;
		}
		if (S_ISDIR(sb->st_mode)) {
			curroffset = processdir(level+1, n->realname, dp->d_name, sb, n, root, curroffset);
		}
	}
	closedir(dirfd);
	return curroffset;
}

void showhelp(const char *argv0)
{
	printf("Usage: %s [OPTIONS] -f IMAGE\n",argv0);
	printf("Create a romfs filesystem image from a directory\n");
	printf("\n");
	printf("  -f IMAGE               Output the image into this file\n");
	printf("  -d DIRECTORY           Use this directory as source\n");
	printf("  -v                     (Too) verbose operation\n");
	printf("  -V VOLUME              Use the specified volume name\n");
	printf("  -a ALIGN               Align regular file data to ALIGN bytes\n");
	printf("  -A ALIGN,PATTERN	 Align all objects matching pattern to at least ALIGN bytes\n");
	printf("  -h                     Show this help\n");
	printf("\n");
	printf("Report bugs to chexum@shadow.banki.hu\n");
}

int main(int argc, char *argv[])
{
	int c;
	char *dir = ".";
	char *outf = NULL;
	char *volname = NULL;
	int verbose=0;
	char buf[256];
	struct filenode *root;
	struct stat sb;
	int lastoff;
	int i;
	char *p;
	struct aligns *pa, *pa2;
	FILE *f;

	while ((c = getopt(argc, argv, "V:vd:f:ha:A:")) != EOF) {
		switch(c) {
		case 'd':
			dir = optarg;
			break;
		case 'f':
			outf = optarg;
			break;
		case 'V':
			volname = optarg;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'h':
			showhelp(argv[0]);
			exit(0);
		case 'a':
			align = strtoul(optarg, NULL, 0);
			if (align < 16 || (align & (align - 1))) {
				fprintf(stderr, "Align has to be at least 16 bytes nad a power of two\n");
				exit(1);
			}
			break;
		case 'A':
			i = strtoul(optarg, &p, 0);
			if (i < 16 || (i & (i - 1))) {
				fprintf(stderr, "Align has to be at least 16 bytes nad a power of two\n");
				exit(1);
			}
			if (*p != ',' || !p[1]) {
				fprintf(stderr, "-A takes N,PATTERN format of argument, where N is a number\n");
				exit(1);
			}
			pa = (struct aligns *)malloc(sizeof(*pa) + strlen(p));
			pa->align = i;
			pa->next = NULL;
			strcpy(pa->pattern, p + 1);
			if (!alignlist)
				alignlist = pa;
			else {
				for (pa2 = alignlist; pa2->next; pa2 = pa2->next);
				pa2->next = pa;
			}
			break;
		default:
			exit(1);
		}
	}

	if (!volname) {
		sprintf(buf, "rom %08lx", time(NULL));
		volname = buf;
	}
	if (!outf) {
		fprintf(stderr, "%s: you must specify the destination file\n", argv[0]);
		fprintf(stderr, "Try `%s -h' for more information\n",argv[0]);
		exit(1);
	}
	if (strcmp(outf, "-") == 0) {
#ifdef _WIN32
		f = fdopen(1, "wb");
#else
		f = fdopen(1,"w");
#endif
	} else
#ifdef _WIN32
		f = fopen(outf, "wb");
#else
		f = fopen(outf, "w");
#endif

	if (!f) {
		perror(outf);
		exit(1);
	}
	
	realbase = strlen(dir);
	root = newnode(dir, volname, 0);
	lastoff = processdir (1, dir, dir, &sb, root, root, spaceneeded(root));
	if (verbose)
		shownode(0, root, stderr);
	dumpall(root, lastoff, f);

	exit(0);
}
