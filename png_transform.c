/*
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include <png.h>

typedef enum {
	transf_default = 0,
	transf_colors_weight,
	
	/* Add new transformation type below */
} e_transform_type;

void
abort_(const char *s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

int x, y;

struct decoded_image {
	int         w, h;
	png_byte    color_type;
	png_byte    bit_depth;
	png_structp png_ptr;
	png_infop   info_ptr;
	png_infop   end_info;
	int         number_of_passes;
	png_bytep   *row_pointers;
};

void
read_png_file(char *file_name, struct decoded_image *img)
{
	char header[8];        // 8 is the maximum size that can be checked
	size_t rsize;	// read size

	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
	if (!fp)
		abort_("[read_png_file] File %s could not be opened for reading", file_name);
		
	rsize = fread(header, 1, 8, fp);
	if (rsize < 8u)
		abort_("[read_png_file] Read file %s has filed (%d byte read)", file_name, rsize);
		
	if (png_sig_cmp((png_const_bytep)header, 0, 8))
		abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


	/* initialize stuff */
	img->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!img->png_ptr)
		abort_("[read_png_file] png_create_read_struct failed");

	img->info_ptr = png_create_info_struct(img->png_ptr);
	if (!img->info_ptr)
		abort_("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(img->png_ptr)))
		abort_("[read_png_file] Error during init_io");


    png_init_io(img->png_ptr, fp);
    png_set_sig_bytes(img->png_ptr, 8);

    png_read_info(img->png_ptr, img->info_ptr);

    img->w      = png_get_image_width(img->png_ptr, img->info_ptr);
    img->h     = png_get_image_height(img->png_ptr, img->info_ptr);
    img->color_type = png_get_color_type(img->png_ptr, img->info_ptr);
    img->bit_depth  = png_get_bit_depth(img->png_ptr, img->info_ptr);

    img->number_of_passes = png_set_interlace_handling(img->png_ptr);
    png_read_update_info(img->png_ptr, img->info_ptr);


	/* read file */
	if (setjmp(png_jmpbuf(img->png_ptr)))
		abort_("[read_png_file] Error during read_image");

	img->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * img->h);
	for (y = 0; y < img->h; y++)
		img->row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(img->png_ptr, img->info_ptr));

	png_read_image(img->png_ptr, img->row_pointers);
	fclose(fp);
}


static void
write_png_file(char *file_name, struct decoded_image *img)
{
  /* create file */
  FILE *fp = fopen(file_name, "wb");
  if (!fp)
    abort_("[write_png_file] File %s could not be opened for writing", file_name);


  /* initialize stuff */
  img->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!img->png_ptr)
    abort_("[write_png_file] png_create_write_struct failed");

  img->info_ptr = png_create_info_struct(img->png_ptr);
  if (!img->info_ptr)
    abort_("[write_png_file] png_create_info_struct failed");

  if (setjmp(png_jmpbuf(img->png_ptr)))
    abort_("[write_png_file] Error during init_io");

  png_init_io(img->png_ptr, fp);


	/* write header */
	if (setjmp(png_jmpbuf(img->png_ptr)))
		abort_("[write_png_file] Error during writing header");

	png_set_IHDR(img->png_ptr, img->info_ptr, img->w, img->h,
		img->bit_depth, img->color_type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(img->png_ptr, img->info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(img->png_ptr)))
		abort_("[write_png_file] Error during writing bytes");

	png_write_image(img->png_ptr, img->row_pointers);


	/* end write */
	if (setjmp(png_jmpbuf(img->png_ptr)))
		abort_("[write_png_file] Error during end of write");

	png_write_end(img->png_ptr, NULL);

	/* cleanup heap allocation */
	for (y = 0; y < img->h; y++)
		free(img->row_pointers[y]);
	free(img->row_pointers);
	png_destroy_write_struct(&img->png_ptr, &img->info_ptr);

	fclose(fp);
}


/* Apply default transformation */
static int process_default_transf(struct decoded_image *img)
{
	printf("Apply default transformation\n");
	for (x = 0; x < img->w; x++)
	{
	    for (y = 0; y < img->h; y++)
	    {
	      png_byte *row = img->row_pointers[y];
	      png_byte *ptr = &(row[x * 4]);
		  
		  /* set red value to 0 and green value to blue value*/
		  ptr[0]  = 0;
		  ptr[1] = ptr[2];
	    }
	}

	return 0;
}

/* Apply colors weight transformation */
static int process_colors_weight_tranf(struct decoded_image *img, float red_val, float green_val, float blue_val)
{
	printf("Apply colors weight transformation \n");
	for (x = 0; x < img->w; x++)
	{
	    for (y = 0; y < img->h; y++)
	    {
	      png_byte *row = img->row_pointers[y];
	      png_byte *ptr = &(row[x * 4]);
		  
		  /* Apply weight colours*/
		  ptr[0] *= red_val;
		  ptr[1] *= green_val;
		  ptr[2] *= blue_val;
	    }
	}

	return 0;
}

static int
process_file(struct decoded_image *img, e_transform_type transf_type, float red_val, float green_val, float blue_val)
{
	int res = 0;
	printf("Checking PNG format\n");

	if (png_get_color_type(img->png_ptr, img->info_ptr) != PNG_COLOR_TYPE_RGBA)
	{
		printf("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)", PNG_COLOR_TYPE_RGBA, png_get_color_type(img->png_ptr, img->info_ptr));
		return 1;
	}

	switch (transf_type)
	{
		case transf_colors_weight:
			res = process_colors_weight_tranf(img, red_val, green_val, blue_val);
			break;
		
        case transf_default:		
		default:
			res = process_default_transf(img);
			break;
	}

	printf("Processing done\n");

	png_destroy_read_struct(&img->png_ptr, &img->info_ptr, NULL);

	return res;
}


int
main(int argc, char **argv)
{
	float red_v = 0;
	float green_v = 0;
	float blue_v = 0;
	e_transform_type transf_type = transf_default;
	
	if (argc < 3)
		abort_("Usage: program_name <file_in> <file_out> [<transform_type> <transf_arg1> ...]");
		
	if (argc >= 4)
	{
	    transf_type = (e_transform_type) atoi(argv[3]);
		/* Check Arguments */
		if (transf_type == transf_colors_weight)
		{
			if (argc < 7)
			{
				abort_("Colors Weight Transformation selected: Missing Arguments! usage : program_name <file_in> <file_out> 1 <red_val> <green_val> <blue_val>");
			}
			else
			{
			    printf("Colors Weight Transformation\n");
				red_v = atof(argv[4]);
				green_v = atof(argv[5]);
				blue_v = atof(argv[6]);
			}
		}
		else
		{
			printf("Default Transformation\n");
		}
	}
	
	struct decoded_image *img = malloc(sizeof(struct decoded_image));

	printf("Reading input PNG\n");
	read_png_file(argv[1], img);
	
	/* process transformation */
	process_file(img, transf_type, red_v, green_v, blue_v);

	printf("Writing output PNG\n");
	write_png_file(argv[2], img);

	return 0;
}
