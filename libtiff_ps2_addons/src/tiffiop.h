/*
 * Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef _TIFFIOP_
#define _TIFFIOP_

/* BORROWED THIS FOR PS2SDK. */

/*
 * ``Library-private'' definitions.
 */


#include <string.h>

#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>

#include "tiffio.h"

#include <limits.h>

#ifndef STRIP_SIZE_DEFAULT
#define STRIP_SIZE_DEFAULT 8192
#endif

#ifndef TIFF_MAX_DIR_COUNT
#define TIFF_MAX_DIR_COUNT 1048576
#endif

#define TIFF_NON_EXISTENT_DIR_NUMBER UINT_MAX

#define streq(a, b) (strcmp(a, b) == 0)
#define strneq(a, b, n) (strncmp(a, b, n) == 0)

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/** List element structure. */
typedef struct _TIFFList TIFFList;

/** List element structure. */
struct _TIFFList
{
    /*! Pointer to the data object. Should be allocated and freed by the
     * caller.
     * */
    void *pData;
    /*! Pointer to the next element in list. NULL, if current element is the
     * last one.
     */
    struct _TIFFList *psNext;
};

/** Opaque type for a hash set */
typedef struct _TIFFHashSet TIFFHashSet;

/** TIFFHashSetHashFunc */
typedef unsigned long (*TIFFHashSetHashFunc)(const void *elt);

/** TIFFHashSetEqualFunc */
typedef bool (*TIFFHashSetEqualFunc)(const void *elt1, const void *elt2);

/** TIFFHashSetFreeEltFunc */
typedef void (*TIFFHashSetFreeEltFunc)(void *elt);

struct _TIFFHashSet
{
    TIFFHashSetHashFunc fnHashFunc;
    TIFFHashSetEqualFunc fnEqualFunc;
    TIFFHashSetFreeEltFunc fnFreeEltFunc;
    TIFFList **tabList;
    int nSize;
    int nIndiceAllocatedSize;
    int nAllocatedSize;
    TIFFList *psRecyclingList;
    int nRecyclingListSize;
    bool bRehash;
#ifdef HASH_DEBUG
    int nCollisions;
#endif
};

typedef struct client_info
{
    struct client_info *next;
    void *data;
    char *name;
} TIFFClientInfoLink;

/*
 * Typedefs for ``method pointers'' used internally.
 * these are deprecated and provided only for backwards compatibility.
 */
typedef unsigned char tidataval_t; /* internal image data value type */
typedef tidataval_t *tidata_t;     /* reference to internal image data */

typedef void (*TIFFVoidMethod)(TIFF *);
typedef int (*TIFFBoolMethod)(TIFF *);
typedef int (*TIFFPreMethod)(TIFF *, uint16_t);
typedef int (*TIFFCodeMethod)(TIFF *tif, uint8_t *buf, tmsize_t size,
                              uint16_t sample);
typedef int (*TIFFSeekMethod)(TIFF *, uint32_t);
typedef void (*TIFFPostMethod)(TIFF *tif, uint8_t *buf, tmsize_t size);
typedef uint32_t (*TIFFStripMethod)(TIFF *, uint32_t);
typedef void (*TIFFTileMethod)(TIFF *, uint32_t *, uint32_t *);

struct TIFFOffsetAndDirNumber
{
    uint64_t offset;
    tdir_t dirNumber;
};
typedef struct TIFFOffsetAndDirNumber TIFFOffsetAndDirNumber;

typedef union
{
    TIFFHeaderCommon common;
    TIFFHeaderClassic classic;
    TIFFHeaderBig big;
} TIFFHeaderUnion;

/*
 * ``Library-private'' Directory-related Definitions.
 */

typedef struct
{
    const TIFFField *info;
    int count;
    void *value;
} TIFFTagValue;

/*
 * TIFF Image File Directories are comprised of a table of field
 * descriptors of the form shown below.  The table is sorted in
 * ascending order by tag.  The values associated with each entry are
 * disjoint and may appear anywhere in the file (so long as they are
 * placed on a word boundary).
 *
 * If the value is 4 bytes or less, in ClassicTIFF, or 8 bytes or less in
 * BigTIFF, then it is placed in the offset field to save space. If so,
 * it is left-justified in the offset field.
 */
typedef struct
{
    uint16_t tdir_tag;   /* see below */
    uint16_t tdir_type;  /* data type; see below */
    uint64_t tdir_count; /* number of items; length in spec */
    union
    {
        uint16_t toff_short;
        uint32_t toff_long;
        uint64_t toff_long8;
    } tdir_offset;       /* either offset or the data itself if fits */
    uint8_t tdir_ignore; /* flag status to ignore tag when parsing tags in
                            tif_dirread.c */
} TIFFDirEntry;

/*
 * Internal format of a TIFF directory entry.
 */
typedef struct
{
#define FIELDSET_ITEMS 4
    /* bit vector of fields that are set */
    uint32_t td_fieldsset[FIELDSET_ITEMS];

    uint32_t td_imagewidth, td_imagelength, td_imagedepth;
    uint32_t td_tilewidth, td_tilelength, td_tiledepth;
    uint32_t td_subfiletype;
    uint16_t td_bitspersample;
    uint16_t td_sampleformat;
    uint16_t td_compression;
    uint16_t td_photometric;
    uint16_t td_threshholding;
    uint16_t td_fillorder;
    uint16_t td_orientation;
    uint16_t td_samplesperpixel;
    uint32_t td_rowsperstrip;
    uint16_t td_minsamplevalue, td_maxsamplevalue;
    double *td_sminsamplevalue;
    double *td_smaxsamplevalue;
    float td_xresolution, td_yresolution;
    uint16_t td_resolutionunit;
    uint16_t td_planarconfig;
    float td_xposition, td_yposition;
    uint16_t td_pagenumber[2];
    uint16_t *td_colormap[3];
    uint16_t td_halftonehints[2];
    uint16_t td_extrasamples;
    uint16_t *td_sampleinfo;
    /* even though the name is misleading, td_stripsperimage is the number
     * of striles (=strips or tiles) per plane, and td_nstrips the total
     * number of striles */
    uint32_t td_stripsperimage;
    uint32_t td_nstrips; /* size of offset & bytecount arrays */
    uint64_t
        *td_stripoffset_p; /* should be accessed with TIFFGetStrileOffset */
    uint64_t *td_stripbytecount_p; /* should be accessed with
                                      TIFFGetStrileByteCount */
    uint32_t
        td_stripoffsetbyteallocsize; /* number of elements currently allocated
                                        for td_stripoffset/td_stripbytecount.
                                        Only used if TIFF_LAZYSTRILELOAD is set
                                      */
#ifdef STRIPBYTECOUNTSORTED_UNUSED
    int td_stripbytecountsorted; /* is the bytecount array sorted ascending? */
#endif
    TIFFDirEntry td_stripoffset_entry;    /* for deferred loading */
    TIFFDirEntry td_stripbytecount_entry; /* for deferred loading */
    uint16_t td_nsubifd;
    uint64_t *td_subifd;
    /* YCbCr parameters */
    uint16_t td_ycbcrsubsampling[2];
    uint16_t td_ycbcrpositioning;
    /* Colorimetry parameters */
    uint16_t *td_transferfunction[3];
    float *td_refblackwhite;
    /* CMYK parameters */
    int td_inknameslen;
    char *td_inknames;
    uint16_t td_numberofinks; /* number of inks in InkNames string */

    int td_customValueCount;
    TIFFTagValue *td_customValues;

    unsigned char
        td_deferstrilearraywriting; /* see TIFFDeferStrileArrayWriting() */
} TIFFDirectory;


struct tiff
{
    char *tif_name; /* name of open file */
    int tif_fd;     /* open file descriptor */
    int tif_mode;   /* open mode (O_*) */
    uint32_t tif_flags;
#define TIFF_FILLORDER 0x00003U   /* natural bit fill order for machine */
#define TIFF_DIRTYHEADER 0x00004U /* header must be written on close */
#define TIFF_DIRTYDIRECT 0x00008U /* current directory must be written */
#define TIFF_BUFFERSETUP 0x00010U /* data buffers setup */
#define TIFF_CODERSETUP 0x00020U  /* encoder/decoder setup done */
#define TIFF_BEENWRITING 0x00040U /* written 1+ scanlines to file */
#define TIFF_SWAB 0x00080U        /* byte swap file information */
#define TIFF_NOBITREV 0x00100U    /* inhibit bit reversal logic */
#define TIFF_MYBUFFER 0x00200U    /* my raw data buffer; free on close */
#define TIFF_ISTILED 0x00400U     /* file is tile, not strip- based */
#define TIFF_MAPPED 0x00800U      /* file is mapped into memory */
#define TIFF_POSTENCODE 0x01000U  /* need call to postencode routine */
#define TIFF_INSUBIFD 0x02000U    /* currently writing a subifd */
#define TIFF_UPSAMPLED 0x04000U   /* library is doing data up-sampling */
#define TIFF_STRIPCHOP 0x08000U   /* enable strip chopping support */
#define TIFF_HEADERONLY                                                        \
    0x10000U /* read header only, do not process the first directory */
#define TIFF_NOREADRAW                                                         \
    0x20000U /* skip reading of raw uncompressed image data */
#define TIFF_INCUSTOMIFD 0x40000U /* currently writing a custom IFD */
#define TIFF_BIGTIFF 0x80000U     /* read/write bigtiff */
#define TIFF_BUF4WRITE 0x100000U  /* rawcc bytes are for writing */
#define TIFF_DIRTYSTRIP 0x200000U /* stripoffsets/stripbytecount dirty*/
#define TIFF_PERSAMPLE 0x400000U  /* get/set per sample tags as arrays */
#define TIFF_BUFFERMMAP                                                        \
    0x800000U /* read buffer (tif_rawdata) points into mmap() memory */
#define TIFF_DEFERSTRILELOAD                                                   \
    0x1000000U /* defer strip/tile offset/bytecount array loading. */
#define TIFF_LAZYSTRILELOAD                                                    \
    0x2000000U /* lazy/ondemand loading of strip/tile offset/bytecount values. \
                  Only used if TIFF_DEFERSTRILELOAD is set and in read-only    \
                  mode */
#define TIFF_CHOPPEDUPARRAYS                                                   \
    0x4000000U /* set when allocChoppedUpStripArrays() has modified strip      \
                  array */
    uint64_t tif_diroff;     /* file offset of current directory */
    uint64_t tif_nextdiroff; /* file offset of following directory */
    uint64_t tif_lastdiroff; /* file offset of last directory written so far */
    TIFFHashSet *tif_map_dir_offset_to_number;
    TIFFHashSet *tif_map_dir_number_to_offset;
    int tif_setdirectory_force_absolute; /* switch between relative and absolute
                                            stepping in TIFFSetDirectory() */
    TIFFDirectory tif_dir;               /* internal rep of current directory */
    TIFFDirectory
        tif_customdir; /* custom IFDs are separated from the main ones */
    TIFFHeaderUnion tif_header; /* file's header block Classic/BigTIFF union */
    uint16_t tif_header_size;   /* file's header block and its length */
    uint32_t tif_row;           /* current scanline */

    /* There are IFDs in the file and an "active" IFD in memory,
     * from which fields are "set" and "get".
     * tif_curdir is set to:
     *   a) TIFF_NON_EXISTENT_DIR_NUMBER if there is no IFD in the file
     *      or the state is unknown,
     *      or the last read (i.e. TIFFFetchDirectory()) failed,
     *      or a custom directory was written.
     *   b) IFD index of last IFD written in the file. In this case the
     *      active IFD is a new (empty) one and tif_diroff is zero.
     *      If writing fails, tif_curdir is not changed.
     *   c) IFD index of IFD read from file into memory (=active IFD),
     *      even if IFD is corrupt and TIFFReadDirectory() returns 0.
     *      Then tif_diroff contains the offset of the IFD in the file.
     *   d) IFD index 0, whenever a custom directory or an unchained SubIFD
     *      was read. */
    tdir_t tif_curdir; /* current directory (index) */
    /* tif_curdircount: number of directories (main-IFDs) in file:
     * - TIFF_NON_EXISTENT_DIR_NUMBER means 'dont know number of IFDs'.
     * - 0 means 'empty file opened for writing, but no IFD written yet' */
    tdir_t tif_curdircount;
    uint32_t tif_curstrip;     /* current strip for read/write */
    uint64_t tif_curoff;       /* current offset for read/write */
    uint64_t tif_lastvalidoff; /* last valid offset allowed for rewrite in
                                  place. Used only by TIFFAppendToStrip() */
    uint64_t tif_dataoff;      /* current offset for writing dir (IFD) */
    /* SubIFD support */
    uint16_t tif_nsubifd;   /* remaining subifds to write */
    uint64_t tif_subifdoff; /* offset for patching SubIFD link */
    /* tiling support */
    uint32_t tif_col;      /* current column (offset by row too) */
    uint32_t tif_curtile;  /* current tile for read/write */
    tmsize_t tif_tilesize; /* # of bytes in a tile */
    /* compression scheme hooks */
    int tif_decodestatus;
    TIFFBoolMethod tif_fixuptags;   /* called in TIFFReadDirectory */
    TIFFBoolMethod tif_setupdecode; /* called once before predecode */
    TIFFPreMethod tif_predecode;    /* pre- row/strip/tile decoding */
    TIFFBoolMethod tif_setupencode; /* called once before preencode */
    int tif_encodestatus;
    TIFFPreMethod tif_preencode;      /* pre- row/strip/tile encoding */
    TIFFBoolMethod tif_postencode;    /* post- row/strip/tile encoding */
    TIFFCodeMethod tif_decoderow;     /* scanline decoding routine */
    TIFFCodeMethod tif_encoderow;     /* scanline encoding routine */
    TIFFCodeMethod tif_decodestrip;   /* strip decoding routine */
    TIFFCodeMethod tif_encodestrip;   /* strip encoding routine */
    TIFFCodeMethod tif_decodetile;    /* tile decoding routine */
    TIFFCodeMethod tif_encodetile;    /* tile encoding routine */
    TIFFVoidMethod tif_close;         /* cleanup-on-close routine */
    TIFFSeekMethod tif_seek;          /* position within a strip routine */
    TIFFVoidMethod tif_cleanup;       /* cleanup state routine */
    TIFFStripMethod tif_defstripsize; /* calculate/constrain strip size */
    TIFFTileMethod tif_deftilesize;   /* calculate/constrain tile size */
    uint8_t *tif_data;                /* compression scheme private data */
    /* input/output buffering */
    tmsize_t tif_scanlinesize;  /* # of bytes in a scanline */
    tmsize_t tif_scanlineskew;  /* scanline skew for reading strips */
    uint8_t *tif_rawdata;       /* raw data buffer */
    tmsize_t tif_rawdatasize;   /* # of bytes in raw data buffer */
    tmsize_t tif_rawdataoff;    /* rawdata offset within strip */
    tmsize_t tif_rawdataloaded; /* amount of data in rawdata */
    uint8_t *tif_rawcp;         /* current spot in raw buffer */
    tmsize_t tif_rawcc;         /* bytes unread from raw buffer */
    /* memory-mapped file support */
    uint8_t *tif_base; /* base of mapped file */
    tmsize_t tif_size; /* size of mapped file region (bytes, thus tmsize_t) */
    TIFFMapFileProc tif_mapproc;     /* map file method */
    TIFFUnmapFileProc tif_unmapproc; /* unmap file method */
    /* input/output callback methods */
    thandle_t tif_clientdata;        /* callback parameter */
    TIFFReadWriteProc tif_readproc;  /* read method */
    TIFFReadWriteProc tif_writeproc; /* write method */
    TIFFSeekProc tif_seekproc;       /* lseek method */
    TIFFCloseProc tif_closeproc;     /* close method */
    TIFFSizeProc tif_sizeproc;       /* filesize method */
    /* post-decoding support */
    TIFFPostMethod tif_postdecode; /* post decoding routine */
    /* tag support */
    TIFFField **tif_fields;          /* sorted table of registered tags */
    size_t tif_nfields;              /* # entries in registered tag table */
    const TIFFField *tif_foundfield; /* cached pointer to already found tag */
    TIFFTagMethods tif_tagmethods;   /* tag get/set/print routines */
    TIFFClientInfoLink *tif_clientinfo; /* extra client information. */
    /* Backward compatibility stuff. We need these two fields for
     * setting up an old tag extension scheme. */
    TIFFFieldArray *tif_fieldscompat;
    size_t tif_nfieldscompat;
    /* Error handler support */
    TIFFErrorHandlerExtR tif_errorhandler;
    void *tif_errorhandler_user_data;
    TIFFErrorHandlerExtR tif_warnhandler;
    void *tif_warnhandler_user_data;
    tmsize_t tif_max_single_mem_alloc;    /* in bytes. 0 for unlimited */
    tmsize_t tif_max_cumulated_mem_alloc; /* in bytes. 0 for unlimited */
    tmsize_t tif_cur_cumulated_mem_alloc; /* in bytes */
};

struct TIFFOpenOptions
{
    TIFFErrorHandlerExtR errorhandler; /* may be NULL */
    void *errorhandler_user_data;      /* may be NULL */
    TIFFErrorHandlerExtR warnhandler;  /* may be NULL */
    void *warnhandler_user_data;       /* may be NULL */
    tmsize_t max_single_mem_alloc;     /* in bytes. 0 for unlimited */
    tmsize_t max_cumulated_mem_alloc;  /* in bytes. 0 for unlimited */
};



#endif