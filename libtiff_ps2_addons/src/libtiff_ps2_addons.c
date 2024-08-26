/*
* Some PS2 utils for libtiff
*/

#include <stdint.h>
#include <tiffio.h>
#include <malloc.h>
#include "tiffiop.h"

#include "libtiff_ps2_addons.h"

/*
 * Read the specified image into PS2-format raster 
 */
int
TIFFReadPS2Image(TIFF* tif,
		 uint32_t rwidth, uint32_t rheight, uint32_t* raster,
		 int stop)
{
    char emsg[1024] = "";
    TIFFRGBAImage img;
    int ok;
    int y, cy, x;

    int orientation = ORIENTATION_BOTLEFT;

    uint32_t* nraster = malloc(rwidth * rheight * 4);

    if (TIFFRGBAImageOK(tif, emsg) &&
        TIFFRGBAImageBegin(&img, tif, stop, emsg)) {
        img.req_orientation = orientation;
        /* XXX verify rwidth and rheight against width and height */
        ok = TIFFRGBAImageGet(&img, nraster+(rheight-img.height)*rwidth,
            rwidth, img.height);
        TIFFRGBAImageEnd(&img);
    } else {
        TIFFErrorExt(tif->tif_clientdata, TIFFFileName(tif), emsg);
        ok = 0;
    }

    int tmp;
    for (y = (img.height - 1), cy = 0; y >= 0; y--, cy++)
    {
        for (x = 0; x < img.width; x++)
        {
            ((uint8_t*)raster)[(y*img.width+x)*4+0] = ((uint8_t*)nraster)[(cy*img.width+x)*4+0];
            ((uint8_t*)raster)[(y*img.width+x)*4+1] = ((uint8_t*)nraster)[(cy*img.width+x)*4+1];
            ((uint8_t*)raster)[(y*img.width+x)*4+2] = ((uint8_t*)nraster)[(cy*img.width+x)*4+2];
            tmp = (((uint8_t*)nraster)[(cy*img.width+x)*4+3] * 128);
            ((uint8_t*)raster)[(y*img.width+x)*4+3] = tmp / 255;
        }
    }
    free(nraster);

    return (ok);
}