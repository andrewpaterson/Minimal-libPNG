
/* pngwio.c - functions for data output
 *
 * Last changed in libpng 1.2.13 November 13, 2006
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2002 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * This file provides a location for all output.  Users who need
 * special handling are expected to write functions that have the same
 * arguments as these and perform similar functions, but that possibly
 * use different output methods.  Note that you shouldn't change these
 * functions, but rather write replacement functions and then change
 * them at run time with png_set_write_fn(...).
 */

#define PNG_INTERNAL
#include "png.h"
#ifdef PNG_WRITE_SUPPORTED

/* Write the data to whatever output you are using.  The default routine
   writes to a file pointer.  Note that this routine sometimes gets called
   with very small lengths, so you should implement some kind of simple
   buffering if you are using unbuffered writes.  This should never be asked
   to write more than 64K on a 16 bit machine.  */

void /* PRIVATE */
png_write_data(png_structp png_ptr, uint8_t* data, size_t length)
{
   if (png_ptr->write_data_fn != NULL )
      (*(png_ptr->write_data_fn))(png_ptr, data, length);
   else
      png_error(png_ptr, "Call to NULL write function");
}

#if !defined(PNG_NO_STDIO)
/* This is the function that does the actual writing of data.  If you are
   not writing to a standard C stream, you should create a replacement
   write_data function and use it at run time with png_set_write_fn(), rather
   than changing the library. */
void
png_default_write_data(png_structp png_ptr, uint8_t* data, size_t length)
{
   uint32_t check;

   if(png_ptr == NULL) return;
   check = (uint32_t)fwrite(data, 1, length, (FILE*)(png_ptr->io_ptr));
   if (check != length)
      png_error(png_ptr, "Write Error");
}
#endif

/* This function is called to output any data pending writing (normally
   to disk).  After png_flush is called, there should be no data pending
   writing in any buffers. */
#if defined(PNG_WRITE_FLUSH_SUPPORTED)
void /* PRIVATE */
png_flush(png_structp png_ptr)
{
   if (png_ptr->output_flush_fn != NULL)
      (*(png_ptr->output_flush_fn))(png_ptr);
}

#if !defined(PNG_NO_STDIO)
void
png_default_flush(png_structp png_ptr)
{
   FILE* io_ptr;
   if(png_ptr == NULL) return;
   io_ptr = (FILE*)CVT_PTR((png_ptr->io_ptr));
   if (io_ptr != NULL)
      fflush(io_ptr);
}
#endif
#endif

/* This function allows the application to supply new output functions for
   libpng if standard C streams aren't being used.

   This function takes as its arguments:
   png_ptr       - pointer to a png output data structure
   io_ptr        - pointer to user supplied structure containing info about
                   the output functions.  May be NULL.
   write_data_fn - pointer to a new output function that takes as its
                   arguments a pointer to a png_struct, a pointer to
                   data to be written, and a 32-bit unsigned int that is
                   the number of bytes to be written.  The new write
                   function should call png_error(png_ptr, "Error msg")
                   to exit and output any fatal error messages.
   flush_data_fn - pointer to a new flush function that takes as its
                   arguments a pointer to a png_struct.  After a call to
                   the flush function, there should be no data in any buffers
                   or pending transmission.  If the output method doesn't do
                   any buffering of ouput, a function prototype must still be
                   supplied although it doesn't have to do anything.  If
                   PNG_WRITE_FLUSH_SUPPORTED is not defined at libpng compile
                   time, output_flush_fn will be ignored, although it must be
                   supplied for compatibility. */
void
png_set_write_fn(png_structp png_ptr, void* io_ptr,
   png_rw_ptr write_data_fn, png_flush_ptr output_flush_fn)
{
   if(png_ptr == NULL) return;
   png_ptr->io_ptr = io_ptr;

#if !defined(PNG_NO_STDIO)
   if (write_data_fn != NULL)
      png_ptr->write_data_fn = write_data_fn;
   else
      png_ptr->write_data_fn = png_default_write_data;
#else
   png_ptr->write_data_fn = write_data_fn;
#endif

#if defined(PNG_WRITE_FLUSH_SUPPORTED)
#if !defined(PNG_NO_STDIO)
   if (output_flush_fn != NULL)
      png_ptr->output_flush_fn = output_flush_fn;
   else
      png_ptr->output_flush_fn = png_default_flush;
#else
   png_ptr->output_flush_fn = output_flush_fn;
#endif
#endif /* PNG_WRITE_FLUSH_SUPPORTED */

   /* It is an error to read while writing a png file */
   if (png_ptr->read_data_fn != NULL)
   {
      png_ptr->read_data_fn = NULL;
      png_warning(png_ptr,
         "Attempted to set both read_data_fn and write_data_fn in");
      png_warning(png_ptr,
         "the same structure.  Resetting read_data_fn to NULL.");
   }
}
#endif /* PNG_WRITE_SUPPORTED */
