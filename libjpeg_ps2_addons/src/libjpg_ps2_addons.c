/*
* Some PS2 utils for libjpeg-turbo
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <screenshot.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <gs_psm.h>

#include "../include/libjpg_ps2_addons.h"

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr)cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

static jpgData *_load_JPEG(struct jpeg_decompress_struct *cinfo, int mode)
{
	jpgData *jpg;

	int textureSize = 0;

	jpg = malloc(sizeof(jpgData));
	if(jpg == NULL)
		return NULL;

	jpeg_start_decompress(cinfo);


	textureSize = cinfo->output_width*cinfo->output_height*cinfo->out_color_components;

	if( (mode == JPG_WIDTH_FIX) && (cinfo->output_width % 4) )
		jpg->width = cinfo->output_width - (cinfo->output_width % 4);
	else
		jpg->width  = cinfo->output_width;

	jpg->height = cinfo->output_height;
	jpg->bpp    = cinfo->out_color_components * 8;
	jpg->buffer = malloc(textureSize);

	unsigned int row_stride = textureSize/cinfo->output_height;
	unsigned char *row_pointer = (unsigned char *)jpg->buffer;
	while (cinfo->output_scanline < cinfo->output_height) {
		jpeg_read_scanlines(cinfo, (JSAMPARRAY)&row_pointer, 1);
		row_pointer += row_stride;
	}

	jpeg_finish_decompress(cinfo);

	return jpg;
}

jpgData *jpgFromRAW(void *data, int size, int mode)
{
	jpgData *ret;
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		printf("jpgFromRAW: error during processing file\n");
		return NULL;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, (void *)data, size);
	jpeg_read_header(&cinfo, TRUE);

	ret = _load_JPEG(&cinfo, mode);
	
	jpeg_destroy_decompress(&cinfo);

	return ret;
}

jpgData *jpgFromFilename(const char *filename, int mode)
{
	void *data;
	int size;
	int fd;
	ssize_t ret;

	fd = open(filename, O_RDONLY, 0);
	if(fd == -1) {
		printf("jpgOpen: error opening '%s'\n", filename);
		return NULL;
	}

	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	data = malloc(size);
	if(data == NULL) {
		printf("jpgOpen: NOT enought memory\n");
		close(fd);
		return NULL;
	}
	
	ret = read(fd, data, size);
	if (ret != size) {
		printf("jpgOpen: Error reading content\n");
		free(data);
		close(fd);
		return NULL;
	}

	close(fd);

	return jpgFromRAW(data, size, mode);
}

jpgData *jpgFromFILE(FILE *in_file, int mode)
{
	jpgData *jpg;
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		printf("jpgOpenFILE: error during processing file\n");
		return NULL;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, in_file);
	jpeg_read_header(&cinfo, TRUE);

	jpg = _load_JPEG(&cinfo, mode);
	
	jpeg_destroy_decompress(&cinfo);

	return jpg;
}

void jpgFileFromJpgData(const char *filename, int quality, jpgData *jpg) {
	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;
	FILE *outfile;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_compress(&cinfo);
		printf("jpgOpenFILE: error during processing file\n");
		return;
	}
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(filename, "wb")) == NULL) {
    	fprintf(stderr, "can't open %s\n", filename);
    	return;
  	}

  	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = jpg->width;
  	cinfo.image_height = jpg->height;
  	cinfo.input_components = jpg->bpp/8;
  	cinfo.in_color_space = JCS_RGB;
  
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);


	jpeg_start_compress(&cinfo, TRUE);

	unsigned int row_stride = jpg->width * 3;
	unsigned char *row_pointer = (unsigned char *)jpg->buffer;
	while (cinfo.next_scanline < cinfo.image_height) {
		jpeg_write_scanlines(&cinfo, (JSAMPARRAY)&row_pointer, 1);
		row_pointer += row_stride;
	}

	jpeg_finish_compress(&cinfo);
  	fclose(outfile);

	jpeg_destroy_compress(&cinfo);
}

int jpgScreenshot(const char* filename,unsigned int vramAdress, unsigned int width, unsigned int height, unsigned int psm)
{
	int y;
	static uint32_t in_buffer[1024*4];  // max 1024*32bit for a line, should be ok
	uint8_t *p_out;
	jpgData *jpg;

	jpg = malloc(sizeof(jpgData));
	if(jpg == NULL)
		return -1;

	jpg->buffer = malloc(width * height * 3);
	if (jpg->buffer == NULL) {
		free(jpg);
		return -1;
	}

	p_out = jpg->buffer;

	// Check if we have a tempbuffer, if we do we use it 
	for (y = 0; y < height; y++)
	{
		ps2_screenshot(in_buffer, vramAdress, 0, y, width, 1, psm);

		if (psm == GS_PSM_16)
		{
			uint32_t x;
			uint16_t* p_in  = (uint16_t*)&in_buffer;

			for (x = 0; x < width; x++)
			{
				uint32_t r = (p_in[x] & 31) << 3;
				uint32_t g = ((p_in[x] >> 5) & 31) << 3;
				uint32_t b = ((p_in[x] >> 10) & 31) << 3;

				p_out[x*3+0] = r;
				p_out[x*3+1] = g;
				p_out[x*3+2] = b;
			}
		}
		else
		{
			if (psm == GS_PSM_24)
			{
				uint32_t x;
				uint8_t* p_in  = (uint8_t*)&in_buffer;

				for (x = 0; x < width; x++)
				{
					uint8_t r =  *p_in++;
					uint8_t g =  *p_in++;
					uint8_t b =  *p_in++;

					p_out[x*3+0] = r;
					p_out[x*3+1] = g;
					p_out[x*3+2] = b;
				}
			}
			else
			{
				uint8_t *p_in = (uint8_t *) &in_buffer;
				uint32_t x;

				for(x = 0; x < width; x++)
				{
					uint8_t r = *p_in++;
					uint8_t g = *p_in++;
					uint8_t b = *p_in++;

					p_in++;

					p_out[x*3+0] = r;
					p_out[x*3+1] = g;
					p_out[x*3+2] = b;
				}
			}

			p_out+= width*3;
		}
	}

	jpg->width = width;
	jpg->height = height;
	jpg->bpp = 24;

	jpgFileFromJpgData(filename, 100, jpg);
	
	free(jpg->buffer);
	free(jpg);

	return 0;
}
