#ifdef USE_ATI
#include <stdio.h>
#include <vha.h>
#include <r128hap.h>
#include <ovly.h>
#include "vhar128.h"

/* #define  DEBUG_R128VHA */

#define I_FRAME 1
#define P_FRAME 2
#define B_FRAME 3
#define D_FRAME 4

struct vhar128_image {
  OVDESCRIPTION * overlay;
};

struct private_yuvhwdata {
  OVSURFACE * surface;
};

/* Translate error code to something readable */
static void vhar128_perror(char * string, int error_code)
{
  switch(error_code)
  {
    case VHAERR_INVALIDPARAMS:
      fprintf(stderr, "%s: (ATI) Invalid parameters\n", string);
      break; 
    case VHAERR_UNSUPPORTED:
      fprintf(stderr, "%s: (ATI) Unsupported feature\n", string);
      break; 
    case VHAERR_INVALIDHANDLE:
      fprintf(stderr, "%s: (ATI) Invalid handle\n", string);
      break; 
    case VHAERR_OUTOFMEMORY:
      fprintf(stderr, "%s: (ATI) Out of memory\n", string);
      break; 
    case VHA_OK:
      fprintf(stderr, "%s: (ATI) Success\n", string);
      break; 
    default:
      fprintf(stderr, "%s: (ATI) Undocumented error\n", string);
      break;
  }
}

/* Check and initialize hardware */
unsigned int vhar128_new()
{
    VHA_HARDWAREQUERY vhaQuery;

    /* Initialize hardware access */
    ATIHAP_InitHWAccess();

    /* Query hardware information */
    memset(&vhaQuery, 0, sizeof(VHA_HARDWAREQUERY));
    vhaQuery.uSize = sizeof(VHA_HARDWAREQUERY);
    vhaQuery.uCards = 0;
    vhaQuery.ulCodedWidth = 0;
    vhaQuery.ulCodedHeight = 0;
    VHA_HardwareQuery(&vhaQuery);

#ifdef DEBUG_R128VHA
    printf("VHA_HardwareQuery :\n");
    printf("uHandle = %d\n", vhaQuery.uHandle);
    if(vhaQuery.ulHWCaps & VHA_HWCAPS_IDCTMC)
      printf("Hardware supports iScan, iDCT, and MC.\n");
    if(vhaQuery.ulHWCaps & VHA_HWCAPS_SUBPIC)
      printf("Hardware supports Sub-picture.\n");
    printf("ulMinOvlyBuffer = %d\n", vhaQuery.ulMinOvlyBuffer);
    printf("ulMaxOvlyBuffer = %d\n", vhaQuery.ulMaxOvlyBuffer);
#endif

    return(vhaQuery.uHandle);
}

/* Create a new overlay */
struct vhar128_image * vhar128_newimage(unsigned int handle, unsigned long width, unsigned long height)
{
  struct vhar128_image * image;

  image = (struct vhar128_image *) malloc(sizeof *image);
  
  image->overlay = (OVDESCRIPTION *) malloc(sizeof *image->overlay);
  memset(image->overlay, 0, sizeof(OVDESCRIPTION));
  image->overlay->uSize = sizeof(OVDESCRIPTION);
  image->overlay->ulOVFormat = OV_FORMAT_YUV12;
  image->overlay->uWidth = width;
  image->overlay->uHeight = height;
  CreateOVSurface(handle, image->overlay);

  return(image);
}

/* Lock overlay */
void vhar128_lockimage(unsigned int handle, struct vhar128_image * image, SDL_Overlay * ov)
{
    struct private_yuvhwdata * hwdata;

#ifdef DEBUG_R128VHA
    /* show locked overlay */
    SetOVSurface(handle, image->overlay);
#endif

    hwdata = (struct private_yuvhwdata *) malloc(sizeof *hwdata);
    hwdata->surface = (OVSURFACE *) malloc(sizeof *hwdata->surface);
    hwdata->surface->uSize = sizeof(OVSURFACE);

    LockOVSurface(handle, image->overlay, hwdata->surface);

    /* create an SDL_Overlay from the information in the ATI overlay */
    ov->format = SDL_YV12_OVERLAY;
    ov->w = image->overlay->uWidth;
    ov->h = image->overlay->uHeight;
    ov->planes = 3;
    ov->pitches = (Uint16 *) malloc(ov->planes * sizeof(Uint16));
    ov->pitches[0] = hwdata->surface->uPitchPlane1;
    ov->pitches[1] = hwdata->surface->uPitchPlane3;
    ov->pitches[2] = hwdata->surface->uPitchPlane2;
    ov->pixels = (Uint8 **) malloc(ov->planes * sizeof(void *));
    ov->pixels[0] = (Uint8 *) hwdata->surface->pSurfPlane1;
    ov->pixels[1] = (Uint8 *) hwdata->surface->pSurfPlane3;
    ov->pixels[2] = (Uint8 *) hwdata->surface->pSurfPlane2;
    ov->hwdata = hwdata;
}

/* Unlock the overlay */
void vhar128_unlockimage(unsigned int handle, struct vhar128_image * image, SDL_Overlay * ov)
{
  UnlockOVSurface(handle, image->overlay, ov->hwdata->surface);
  free(ov->pitches);
  free(ov->pixels);
  free(ov->hwdata->surface);
  free(ov->hwdata);
}

/* Destroy the overlay */
void vhar128_destroyimage(unsigned int handle, struct vhar128_image * image)
{
  DestroyOVSurface(handle, image->overlay);
  free(image->overlay);
  free(image);
}

/* Setup hardware decoding */
int vhar128_init(unsigned int handle, unsigned long width, unsigned long height, struct vhar128_image *ring[], int ring_size)
{
    VHA_INIT vhaInit;
    register int i;

    memset(&vhaInit, 0, sizeof(VHA_INIT));

    /* obtain yv12 offset and send it to vha */
    for(i = 0; i < ring_size; i++)
    {
      vhaInit.yv12[i].ulOffsetY = ring[i]->overlay->OVOffset.ulOfsPlane1;
      vhaInit.yv12[i].ulOffsetU = ring[i]->overlay->OVOffset.ulOfsPlane2;
      vhaInit.yv12[i].ulOffsetV = ring[i]->overlay->OVOffset.ulOfsPlane3;
      vhaInit.yv12[i].ulPitchY  = ring[i]->overlay->OVOffset.uPitchPlane1;
      vhaInit.yv12[i].ulPitchUV = ring[i]->overlay->OVOffset.uPitchPlane2;
    }
    vhaInit.ulNumYV12Buffer = ring_size;
    vhaInit.uSize = sizeof(VHA_INIT);
    vhaInit.ulHWSupports = VHA_HWCAPS_IDCTMC;
    vhaInit.ulCodedWidth = width;
    vhaInit.ulCodedHeight = height;

#ifdef DEBUG_R128VHA
    /* set region for showing ATI overlay on the screen */
    {
      RCTL rSrc, rDst, rView;

      rView.left = rView.top = 0;
      rView.right = width;
      rView.bottom = height;
      rSrc.left = rSrc.top = 0;
      rSrc.right = width;
      rSrc.bottom = height;
      rDst.left = 0;
      rDst.top = 0;
      rDst.right = width;
      rDst.bottom = height;
      
      UpdateOVPosition(handle, &rSrc, &rDst, &rView, OV_SHOW);

      SetOVSurface(handle, ring[0]->overlay);
    }
#endif

    return(VHA_Init(handle, &vhaInit));
}

/* Setup decoding of a new picture */
int vhar128_newdecode(unsigned int handle, int back, int forw, int current)
{
  VHA_NEWDECODE vhaND;
  int retval;

  memset(&vhaND, 0, sizeof(VHA_NEWDECODE));
  vhaND.uSize = sizeof(VHA_NEWDECODE);
  vhaND.BackwardRefFrame[0] = vhaND.BackwardRefFrame[1] = back;
  vhaND.ForwardRefFrame[0] = vhaND.ForwardRefFrame[1] = forw;
  vhaND.DecodeFrame = current;
  vhaND.PictureStructure = VHA_PS_FRAME_PICTURE;

  if((retval = VHA_NewDecode(handle, &vhaND)) != VHA_OK)
    vhar128_perror("vhar128_newdecode", retval);

  return(retval);
}

/* Send a macroblock to the hardware */
int vhar128_macroblock(unsigned int handle, int mb_x, int mb_y, int intra, int back, int forw, int mv_back_x, int mv_back_y, int mv_forw_x, int mv_forw_y, long runlevel[6][130])
{
  VHA_MACROBLOCK vhaMB;
  int retval;

  memset(&vhaMB, 0, sizeof(VHA_MACROBLOCK));
  vhaMB.ulSize = sizeof(VHA_MACROBLOCK);

  memcpy(vhaMB.RunLevel, runlevel, 6*130*sizeof(long));

  vhaMB.mb_x = mb_x;
  vhaMB.mb_y = mb_y;
  vhaMB.mbType = (intra)?VHA_MBT_INTRA:0;
  if(forw) vhaMB.mbType |= VHA_MBT_MOTION_FORWARD;
  if(back) vhaMB.mbType |= VHA_MBT_MOTION_BACKWARD;
  vhaMB.PredictionType = VHA_PT_FRAME_BASED;
  vhaMB.ScanType = SCAN_ZIG_ZAG;
  vhaMB.dct_type = 0;
  vhaMB.vector[0][0][0] = mv_forw_x;
  vhaMB.vector[0][0][1] = mv_forw_y;
  vhaMB.vector[0][1][0] = mv_back_x;
  vhaMB.vector[0][1][1] = mv_back_y;

  if((retval = VHA_Macroblock(handle, &vhaMB)) != VHA_OK)
    vhar128_perror("vhar128_macroblock", retval);

  return(retval);
}

/* Flush all macroblocks */
int vhar128_flush(unsigned int handle)
{
  VHA_DECODECOMMAND vhaDC;
  int retval;

  memset(&vhaDC, 0, sizeof(VHA_DECODECOMMAND));
  vhaDC.uSize = sizeof(VHA_DECODECOMMAND);
  vhaDC.ulCommand = VHA_CMD_FLUSH;

  if((retval = VHA_DecodeCommand(handle, &vhaDC)) != VHA_OK)
    vhar128_perror("vhar128_flush", retval);

  return(retval);
}

/* Close hardware decoding */
void vhar128_close(unsigned int handle)
{
    VHA_Close(handle);
}

/* Close hardware access */
void vhar128_delete()
{
    ATIHAP_CloseHWAccess();    
}

#endif
