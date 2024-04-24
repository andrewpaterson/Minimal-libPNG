#include "png.h"

/* Read a PNG file.  You may want to return an error code if the read
* fails (depending upon the failure).  There are two "prototypes" given
* here - one where we are given the filename, and we need to open the
* file, and the other where we are given an open file (possibly with
* some or all of the magic bytes read - see comments above).
*/
bool LoadPNG(char* file_name)
{
	png_structp			png_ptr;
	png_info*			info_ptr;
	unsigned int		sig_read = 0;
	FILE*				fp;
	uint8_t**			row_pointers;
	int					iWidth;
	int					iHeight;

	if ((fp = fopen(file_name, "rb")) == NULL)
	{
		return false;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose(fp);
		return false;
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_info**)NULL, (png_info**)NULL);
		return false;
	}

	/* Set up the input control if you are using standard C streams */
	png_init_io(png_ptr, fp);

	/* If we have already read some of the signature */
	png_set_sig_bytes(png_ptr, sig_read);

	/*
	* If you have enough memory to read in the entire image at once,
	* and you need to specify only transforms that can be controlled
	* with one of the PNG_TRANSFORM_* bits (this presently excludes
	* dithering, filling, setting background, and doing gamma
	* adjustment), then you can read the entire image (including
	* pixels) into the info structure with this call:
	*/


	png_read_png(png_ptr, info_ptr, 0, (void*)NULL);

	/* close the file */
	fclose(fp);

	row_pointers = png_get_rows(png_ptr, info_ptr);
	iWidth = info_ptr->width;
	iHeight = info_ptr->height;

	if ((iWidth == 0) || (iHeight == 0))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_info**)NULL);
		return false;
	}

	/* At this point you have read the entire image */

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_info**)NULL);
	return true;
}


int __cdecl main(int argc, char* argv[])
{
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\4x4.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\gray-1.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\gray-2.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\gray-4.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\gray-8.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\gray-16.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\gray-alpha-8.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\gray-alpha-16.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\hello.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\palette-1.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\palette-2.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\palette-4.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\palette-8.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\rgb-8.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\rgb-16.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\rgb-alpha-8.png");
	LoadPNG("D:\\Work\\SimplePNG\\testpngs\\rgb-alpha-16.png");
}

