/* pngerror.c - stub functions for i/o and memory allocation
 *
 * Last changed in libpng 1.2.13 November 13, 2006
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2006 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * This file provides a location for all error handling.  Users who
 * need special error handling are expected to write replacement functions
 * and use png_set_error_fn() to use those functions.  See the instructions
 * at each function.
 */

#include "png.h"

#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)
static void png_default_error(png_structp png_ptr, const char* error_message);
static void png_default_warning(png_structp png_ptr, const char* warning_message);

/* This function is called whenever there is a fatal error.  This function
 * should not be changed.  If there is a need to handle errors differently,
 * you should supply a replacement error function and use png_set_error_fn()
 * to replace the error function at run-time.
 */
void
png_error(png_structp png_ptr, const char* error_message)
{
	if (png_ptr != NULL && png_ptr->error_fn != NULL)
	{
		(*(png_ptr->error_fn))(png_ptr, error_message);
	}

	/* If the custom handler doesn't exist, or if it returns,
	   use the default handler, which will not return. */
	png_default_error(png_ptr, error_message);
}

/* This function is called whenever there is a non-fatal error.  This function
 * should not be changed.  If there is a need to handle warnings differently,
 * you should supply a replacement warning function and use
 * png_set_error_fn() to replace the warning function at run-time.
 */
void
png_warning(png_structp png_ptr, const char* warning_message)
{
	int offset = 0;
	if (png_ptr != NULL)
	{
		if (*warning_message == '#')
		{
			for (offset = 1; offset < 15; offset++)
				if (*(warning_message + offset) == ' ')
					break;
		}
		if (png_ptr != NULL && png_ptr->warning_fn != NULL)
			(*(png_ptr->warning_fn))(png_ptr, warning_message + offset);
	}
	else
	{
		png_default_warning(png_ptr, warning_message + offset);
	}
}

/* These utilities are used internally to build an error message that relates
 * to the current chunk.  The chunk name comes from png_ptr->chunk_name,
 * this is used to prefix the message.  The message is limited in length
 * to 63 bytes, the name characters are output as hex digits wrapped in []
 * if the character is invalid.
 */
#define isnonalpha(c) ((c) < 65 || (c) > 122 || ((c) > 90 && (c) < 97))
static const char png_digit[16] = {
   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
   'A', 'B', 'C', 'D', 'E', 'F'
};

static void png_format_buffer(png_structp png_ptr, char* buffer, const char* error_message)
{
	int iout = 0, iin = 0;

	while (iin < 4)
	{
		int c = png_ptr->chunk_name[iin++];
		if (isnonalpha(c))
		{
			buffer[iout++] = '[';
			buffer[iout++] = png_digit[(c & 0xf0) >> 4];
			buffer[iout++] = png_digit[c & 0x0f];
			buffer[iout++] = ']';
		}
		else
		{
			buffer[iout++] = (uint8_t)c;
		}
	}

	if (error_message == NULL)
		buffer[iout] = 0;
	else
	{
		buffer[iout++] = ':';
		buffer[iout++] = ' ';
		strncpy(buffer + iout, error_message, 63);
		buffer[iout + 63] = 0;
	}
}

void
png_chunk_error(png_structp png_ptr, const char* error_message)
{
	char msg[18 + 64];
	if (png_ptr == NULL)
		png_error(png_ptr, error_message);
	else
	{
		png_format_buffer(png_ptr, msg, error_message);
		png_error(png_ptr, msg);
	}
}

void
png_chunk_warning(png_structp png_ptr, const char* warning_message)
{
	char msg[18 + 64];
	if (png_ptr == NULL)
		png_warning(png_ptr, warning_message);
	else
	{
		png_format_buffer(png_ptr, msg, warning_message);
		png_warning(png_ptr, msg);
	}
}

/* This is the default error handling function.  Note that replacements for
 * this function MUST NOT RETURN, or the program will likely crash.  This
 * function is used by default, or if the program supplies NULL for the
 * error function pointer in png_set_error_fn().
 */
static void png_default_error(png_structp png_ptr, const char* error_message)
{
#ifndef PNG_NO_CONSOLE_IO
#ifdef PNG_ERROR_NUMBERS_SUPPORTED
	if (*error_message == '#')
	{
		int offset;
		char error_number[16];
		for (offset = 0; offset < 15; offset++)
		{
			error_number[offset] = *(error_message + offset + 1);
			if (*(error_message + offset) == ' ')
				break;
		}
		if ((offset > 1) && (offset < 15))
		{
			error_number[offset - 1] = '\0';
			fprintf(stderr, "libpng error no. %s: %s\n", error_number,
				error_message + offset);
		}
		else
			fprintf(stderr, "libpng error: %s, offset=%d\n", error_message, offset);
	}
	else
#endif
		fprintf(stderr, "libpng error: %s\n", error_message);
#endif

	abort();
#ifdef PNG_NO_CONSOLE_IO
	/* make compiler happy */;
	if (&error_message != NULL)
		return;
#endif
}

/* This function is called when there is a warning, but the library thinks
 * it can continue anyway.  Replacement functions don't have to do anything
 * here if you don't want them to.  In the default configuration, png_ptr is
 * not used, but it is passed in case it may be useful.
 */
static void png_default_warning(png_structp png_ptr, const char* warning_message)
{
#ifndef PNG_NO_CONSOLE_IO
#  ifdef PNG_ERROR_NUMBERS_SUPPORTED
	if (*warning_message == '#')
	{
		int offset;
		char warning_number[16];
		for (offset = 0; offset < 15; offset++)
		{
			warning_number[offset] = *(warning_message + offset + 1);
			if (*(warning_message + offset) == ' ')
				break;
		}
		if ((offset > 1) && (offset < 15))
		{
			warning_number[offset - 1] = '\0';
			fprintf(stderr, "libpng warning no. %s: %s\n", warning_number,
				warning_message + offset);
		}
		else
			fprintf(stderr, "libpng warning: %s\n", warning_message);
	}
	else
#  endif
		fprintf(stderr, "libpng warning: %s\n", warning_message);
#else
	/* make compiler happy */;
	if (warning_message)
		return;
#endif
	/* make compiler happy */;
	if (png_ptr)
		return;
}

/* This function is called when the application wants to use another method
 * of handling errors and warnings.  Note that the error function MUST NOT
 * return to the calling routine or serious problems will occur.  The return
 * method used in the default routine calls longjmp(png_ptr->jmpbuf, 1)
 */
void
png_set_error_fn(png_structp png_ptr, void* error_ptr,
	png_error_ptr error_fn, png_error_ptr warning_fn)
{
	if (png_ptr == NULL)
		return;
	png_ptr->error_ptr = error_ptr;
	png_ptr->error_fn = error_fn;
	png_ptr->warning_fn = warning_fn;
}


/* This function returns a pointer to the error_ptr associated with the user
 * functions.  The application should free any memory associated with this
 * pointer before png_write_destroy and png_read_destroy are called.
 */
void*
png_get_error_ptr(png_structp png_ptr)
{
	if (png_ptr == NULL)
		return NULL;
	return ((void*)png_ptr->error_ptr);
}


#ifdef PNG_ERROR_NUMBERS_SUPPORTED
void
png_set_strip_error_numbers(png_structp png_ptr, uint32_t strip_mode)
{
	if (png_ptr != NULL)
	{
		png_ptr->flags &=
			((~(PNG_FLAG_STRIP_ERROR_NUMBERS | PNG_FLAG_STRIP_ERROR_TEXT)) & strip_mode);
	}
}
#endif
#endif /* PNG_READ_SUPPORTED || PNG_WRITE_SUPPORTED */
