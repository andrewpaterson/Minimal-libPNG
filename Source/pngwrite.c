
/* pngwrite.c - general routines to write a PNG file
 *
 * Last changed in libpng 1.2.15 January 5, 2007
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2007 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 */

/* get internal access to png.h */
#include "png.h"
#ifdef PNG_WRITE_SUPPORTED

/* Writes all the PNG information.  This is the suggested way to use the
 * library.  If you have a new chunk to add, make a function to write it,
 * and put it in the correct location here.  If you want the chunk written
 * after the image data, put it in png_write_end().  I strongly encourage
 * you to supply a PNG_INFO_ flag, and check info_ptr->valid before writing
 * the chunk, as that will keep the code from breaking if you want to just
 * write a plain PNG file.  If you have long comments, I suggest writing
 * them in png_write_end(), and compressing them.
 */
void
png_write_info_before_PLTE(png_structp png_ptr, png_info* info_ptr)
{
   png_debug(1, "in png_write_info_before_PLTE\n");
   if (png_ptr == NULL || info_ptr == NULL)
      return;
   if (!(png_ptr->mode & PNG_WROTE_INFO_BEFORE_PLTE))
   {
   png_write_sig(png_ptr); /* write PNG signature */
   /* write IHDR information. */
   png_write_IHDR(png_ptr, info_ptr->width, info_ptr->height,
      info_ptr->bit_depth, info_ptr->color_type, info_ptr->compression_type,
      info_ptr->filter_type,
      0);
   /* the rest of these check to see if the valid field has the appropriate
      flag set, and if it does, writes the chunk. */
#if defined(PNG_WRITE_gAMA_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_gAMA)
   {
#  ifdef PNG_FLOATING_POINT_SUPPORTED
      png_write_gAMA(png_ptr, info_ptr->gamma);
#else
#ifdef PNG_FIXED_POINT_SUPPORTED
      png_write_gAMA_fixed(png_ptr, info_ptr->int_gamma);
#  endif
#endif
   }
#endif
#if defined(PNG_WRITE_sRGB_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_sRGB)
      png_write_sRGB(png_ptr, (int)info_ptr->srgb_intent);
#endif
#if defined(PNG_WRITE_sBIT_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_sBIT)
      png_write_sBIT(png_ptr, &(info_ptr->sig_bit), info_ptr->color_type);
#endif


#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   if (info_ptr->unknown_chunks_num)
   {
       png_unknown_chunk *up;

       png_debug(5, "writing extra chunks\n");

       for (up = info_ptr->unknown_chunks;
            up < info_ptr->unknown_chunks + info_ptr->unknown_chunks_num;
            up++)
       {
         int keep=png_handle_as_unknown(png_ptr, up->name);
         if (keep != PNG_HANDLE_CHUNK_NEVER &&
            up->location && !(up->location & PNG_HAVE_PLTE) &&
            !(up->location & PNG_HAVE_IDAT) &&
            ((up->name[3] & 0x20) || keep == PNG_HANDLE_CHUNK_ALWAYS ||
            (png_ptr->flags & PNG_FLAG_KEEP_UNSAFE_CHUNKS)))
         {
            png_write_chunk(png_ptr, up->name, up->data, up->size);
         }
       }
   }
#endif
      png_ptr->mode |= PNG_WROTE_INFO_BEFORE_PLTE;
   }
}

void
png_write_info(png_structp png_ptr, png_info* info_ptr)
{
#if defined(PNG_WRITE_TEXT_SUPPORTED) || defined(PNG_WRITE_sPLT_SUPPORTED)
   int i;
#endif

   png_debug(1, "in png_write_info\n");

   if (png_ptr == NULL || info_ptr == NULL)
      return;

   png_write_info_before_PLTE(png_ptr, info_ptr);

   if (info_ptr->valid & PNG_INFO_PLTE)
      png_write_PLTE(png_ptr, info_ptr->palette,
         (uint32_t)info_ptr->num_palette);
   else if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
      png_error(png_ptr, "Valid palette required for paletted images");

#if defined(PNG_WRITE_tRNS_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_tRNS)
      {
#if defined(PNG_WRITE_INVERT_ALPHA_SUPPORTED)
         /* invert the alpha channel (in tRNS) */
         if ((png_ptr->transformations & PNG_INVERT_ALPHA) &&
            info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
         {
            int j;
            for (j = 0; j<(int)info_ptr->num_trans; j++)
               info_ptr->trans[j] = (uint8_t)(255 - info_ptr->trans[j]);
         }
#endif
      png_write_tRNS(png_ptr, info_ptr->trans, &(info_ptr->trans_values),
         info_ptr->num_trans, info_ptr->color_type);
      }
#endif
#if defined(PNG_WRITE_hIST_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_hIST)
      png_write_hIST(png_ptr, info_ptr->hist, info_ptr->num_palette);
#endif
#if defined(PNG_WRITE_pCAL_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_pCAL)
      png_write_pCAL(png_ptr, info_ptr->pcal_purpose, info_ptr->pcal_X0,
         info_ptr->pcal_X1, info_ptr->pcal_type, info_ptr->pcal_nparams,
         info_ptr->pcal_units, info_ptr->pcal_params);
#endif
#if defined(PNG_WRITE_pHYs_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_pHYs)
      png_write_pHYs(png_ptr, info_ptr->x_pixels_per_unit,
         info_ptr->y_pixels_per_unit, info_ptr->phys_unit_type);
#endif
#if defined(PNG_WRITE_sPLT_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_sPLT)
     for (i = 0; i < (int)info_ptr->splt_palettes_num; i++)
       png_write_sPLT(png_ptr, info_ptr->splt_palettes + i);
#endif

#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   if (info_ptr->unknown_chunks_num)
   {
       png_unknown_chunk *up;

       png_debug(5, "writing extra chunks\n");

       for (up = info_ptr->unknown_chunks;
            up < info_ptr->unknown_chunks + info_ptr->unknown_chunks_num;
            up++)
       {
         int keep=png_handle_as_unknown(png_ptr, up->name);
         if (keep != PNG_HANDLE_CHUNK_NEVER &&
            up->location && (up->location & PNG_HAVE_PLTE) &&
            !(up->location & PNG_HAVE_IDAT) &&
            ((up->name[3] & 0x20) || keep == PNG_HANDLE_CHUNK_ALWAYS ||
            (png_ptr->flags & PNG_FLAG_KEEP_UNSAFE_CHUNKS)))
         {
            png_write_chunk(png_ptr, up->name, up->data, up->size);
         }
       }
   }
#endif
}

/* Writes the end of the PNG file.  If you don't want to write comments or
 * time information, you can pass NULL for info.  If you already wrote these
 * in png_write_info(), do not write them again here.  If you have long
 * comments, I suggest writing them here, and compressing them.
 */
void
png_write_end(png_structp png_ptr, png_info* info_ptr)
{
   png_debug(1, "in png_write_end\n");
   if (png_ptr == NULL)
      return;
   if (!(png_ptr->mode & PNG_HAVE_IDAT))
      png_error(png_ptr, "No IDATs written into file");

   /* see if user wants us to write information chunks */
   if (info_ptr != NULL)
   {
#if defined(PNG_WRITE_TEXT_SUPPORTED)
      int i; /* local index variable */
#endif

#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   if (info_ptr->unknown_chunks_num)
   {
       png_unknown_chunk *up;

       png_debug(5, "writing extra chunks\n");

       for (up = info_ptr->unknown_chunks;
            up < info_ptr->unknown_chunks + info_ptr->unknown_chunks_num;
            up++)
       {
         int keep=png_handle_as_unknown(png_ptr, up->name);
         if (keep != PNG_HANDLE_CHUNK_NEVER &&
            up->location && (up->location & PNG_AFTER_IDAT) &&
            ((up->name[3] & 0x20) || keep == PNG_HANDLE_CHUNK_ALWAYS ||
            (png_ptr->flags & PNG_FLAG_KEEP_UNSAFE_CHUNKS)))
         {
            png_write_chunk(png_ptr, up->name, up->data, up->size);
         }
       }
   }
#endif
   }

   png_ptr->mode |= PNG_AFTER_IDAT;

   /* write end of PNG file */
   png_write_IEND(png_ptr);
#if 0
/* This flush, added in libpng-1.0.8,  causes some applications to crash
   because they do not set png_ptr->output_flush_fn */
   png_flush(png_ptr);
#endif
}


/* Initialize png_ptr structure, and allocate any memory needed */
png_structp
png_create_write_struct(const char* user_png_ver, void* error_ptr,
   png_error_ptr error_fn, png_error_ptr warn_fn)
{
#ifdef PNG_USER_MEM_SUPPORTED
   return (png_create_write_struct_2(user_png_ver, error_ptr, error_fn,
      warn_fn, (void*)NULL, (png_malloc_ptr)NULL, (png_free_ptr)NULL));
}

/* Alternate initialize png_ptr structure, and allocate any memory needed */
png_structp
png_create_write_struct_2(const char* user_png_ver, void* error_ptr,
   png_error_ptr error_fn, png_error_ptr warn_fn, void* mem_ptr,
   png_malloc_ptr malloc_fn, png_free_ptr free_fn)
{
#endif /* PNG_USER_MEM_SUPPORTED */
   png_structp png_ptr;
   int i;
   png_debug(1, "in png_create_write_struct\n");
#ifdef PNG_USER_MEM_SUPPORTED
   png_ptr = (png_structp)png_create_struct_2(PNG_STRUCT_PNG,
      (png_malloc_ptr)malloc_fn, (void*)mem_ptr);
#else
   png_ptr = (png_structp)png_create_struct(PNG_STRUCT_PNG);
#endif /* PNG_USER_MEM_SUPPORTED */
   if (png_ptr == NULL)
      return (NULL);

   /* added at libpng-1.2.6 */
#ifdef PNG_SET_USER_LIMITS_SUPPORTED
   png_ptr->user_width_max=PNG_USER_WIDTH_MAX;
   png_ptr->user_height_max=PNG_USER_HEIGHT_MAX;
#endif

#ifdef PNG_USER_MEM_SUPPORTED
   png_set_mem_fn(png_ptr, mem_ptr, malloc_fn, free_fn);
#endif /* PNG_USER_MEM_SUPPORTED */
   png_set_error_fn(png_ptr, error_ptr, error_fn, warn_fn);

   i = 0;
   do
   {
     if(user_png_ver[i] != png_libpng_ver[i])
        png_ptr->flags |= PNG_FLAG_LIBRARY_MISMATCH;
   } while (png_libpng_ver[i++]);

   if (png_ptr->flags & PNG_FLAG_LIBRARY_MISMATCH)
   {
     /* Libpng 0.90 and later are binary incompatible with libpng 0.89, so
      * we must recompile any applications that use any older library version.
      * For versions after libpng 1.0, we will be compatible, so we need
      * only check the first digit.
      */
     if (user_png_ver == NULL || user_png_ver[0] != png_libpng_ver[0] ||
         (user_png_ver[0] == '1' && user_png_ver[2] != png_libpng_ver[2]) ||
         (user_png_ver[0] == '0' && user_png_ver[2] < '9'))
     {
#if !defined(PNG_NO_STDIO)
        char msg[80];
        if (user_png_ver)
        {
          sprintf(msg, "Application was compiled with png.h from libpng-%.20s",
             user_png_ver);
          png_warning(png_ptr, msg);
        }
        sprintf(msg, "Application  is  running with png.c from libpng-%.20s",
           png_libpng_ver);
        png_warning(png_ptr, msg);
#endif
#ifdef PNG_ERROR_NUMBERS_SUPPORTED
        png_ptr->flags=0;
#endif
        png_error(png_ptr,
           "Incompatible libpng version in application and library");
     }
   }

   /* initialize zbuf - compression buffer */
   png_ptr->zbuf_size = PNG_ZBUF_SIZE;
   png_ptr->zbuf = (uint8_t*)png_malloc(png_ptr,
      (uint32_t)png_ptr->zbuf_size);

   png_set_write_fn(png_ptr, (void*)NULL, (png_rw_ptr)NULL,
      (png_flush_ptr)NULL);

   return (png_ptr);
}

/* Initialize png_ptr structure, and allocate any memory needed */
#if defined(PNG_1_0_X) || defined(PNG_1_2_X)
/* Deprecated. */
#undef png_write_init
void
png_write_init(png_structp png_ptr)
{
   /* We only come here via pre-1.0.7-compiled applications */
   png_write_init_2(png_ptr, "1.0.6 or earlier", 0, 0);
}

void
png_write_init_2(png_structp png_ptr, const char* user_png_ver,
   size_t png_struct_size, size_t png_info_size)
{
   /* We only come here via pre-1.0.12-compiled applications */
   if(png_ptr == NULL) return;
#if !defined(PNG_NO_STDIO)
   if(sizeof(png_struct) > png_struct_size ||
      sizeof(png_info) > png_info_size)
   {
      char msg[80];
      png_ptr->warning_fn=NULL;
      if (user_png_ver)
      {
        sprintf(msg, "Application was compiled with png.h from libpng-%.20s",
           user_png_ver);
        png_warning(png_ptr, msg);
      }
      sprintf(msg, "Application  is  running with png.c from libpng-%.20s",
         png_libpng_ver);
      png_warning(png_ptr, msg);
   }
#endif
   if(sizeof(png_struct) > png_struct_size)
     {
       png_ptr->error_fn=NULL;
#ifdef PNG_ERROR_NUMBERS_SUPPORTED
       png_ptr->flags=0;
#endif
       png_error(png_ptr,
       "The png struct allocated by the application for writing is too small.");
     }
   if(sizeof(png_info) > png_info_size)
     {
       png_ptr->error_fn=NULL;
#ifdef PNG_ERROR_NUMBERS_SUPPORTED
       png_ptr->flags=0;
#endif
       png_error(png_ptr,
       "The info struct allocated by the application for writing is too small.");
     }
   png_write_init_3(&png_ptr, user_png_ver, png_struct_size);
}
#endif /* PNG_1_0_X || PNG_1_2_X */


void
png_write_init_3(png_structpp ptr_ptr, const char* user_png_ver,
   size_t png_struct_size)
{
   png_structp png_ptr=*ptr_ptr;

   int i = 0;

   if (png_ptr == NULL)
      return;

   do
   {
     if (user_png_ver[i] != png_libpng_ver[i])
     {
#ifdef PNG_LEGACY_SUPPORTED
       png_ptr->flags |= PNG_FLAG_LIBRARY_MISMATCH;
#else
       png_ptr->warning_fn=NULL;
       png_warning(png_ptr,
     "Application uses deprecated png_write_init() and should be recompiled.");
       break;
#endif
     }
   } while (png_libpng_ver[i++]);

   png_debug(1, "in png_write_init_3\n");

   if (sizeof(png_struct) > png_struct_size)
     {
       png_destroy_struct(png_ptr);
       png_ptr = (png_structp)png_create_struct(PNG_STRUCT_PNG);
       *ptr_ptr = png_ptr;
     }

   /* reset all variables to 0 */
   memset(png_ptr, 0, sizeof (png_struct));

   /* added at libpng-1.2.6 */
#ifdef PNG_SET_USER_LIMITS_SUPPORTED
   png_ptr->user_width_max=PNG_USER_WIDTH_MAX;
   png_ptr->user_height_max=PNG_USER_HEIGHT_MAX;
#endif

   png_set_write_fn(png_ptr, (void*)NULL, (png_rw_ptr)NULL,
      (png_flush_ptr)NULL);

   /* initialize zbuf - compression buffer */
   png_ptr->zbuf_size = PNG_ZBUF_SIZE;
   png_ptr->zbuf = (uint8_t*)png_malloc(png_ptr,
      (uint32_t)png_ptr->zbuf_size);
}

/* Write a few rows of image data.  If the image is interlaced,
 * either you will have to write the 7 sub images, or, if you
 * have called png_set_interlace_handling(), you will have to
 * "write" the image seven times.
 */
void
png_write_rows(png_structp png_ptr, png_bytepp row,
   uint32_t num_rows)
{
   uint32_t i; /* row counter */
   png_bytepp rp; /* row pointer */

   png_debug(1, "in png_write_rows\n");

   if (png_ptr == NULL)
      return;

   /* loop through the rows */
   for (i = 0, rp = row; i < num_rows; i++, rp++)
   {
      png_write_row(png_ptr, *rp);
   }
}

/* Write the image.  You only need to call this function once, even
 * if you are writing an interlaced image.
 */
void
png_write_image(png_structp png_ptr, png_bytepp image)
{
   uint32_t i; /* row index */
   int pass, num_pass; /* pass variables */
   png_bytepp rp; /* points to current row */

   if (png_ptr == NULL)
      return;

   png_debug(1, "in png_write_image\n");
   num_pass = 1;
   /* loop through passes */
   for (pass = 0; pass < num_pass; pass++)
   {
      /* loop through image */
      for (i = 0, rp = image; i < png_ptr->height; i++, rp++)
      {
         png_write_row(png_ptr, *rp);
      }
   }
}

/* called by user to write a row of image data */
void
png_write_row(png_structp png_ptr, uint8_t* row)
{
   if (png_ptr == NULL)
      return;
   png_debug2(1, "in png_write_row (row %ld, pass %d)\n",
      png_ptr->row_number, png_ptr->pass);

   /* initialize transformations and other stuff if first time */
   if (png_ptr->row_number == 0 && png_ptr->pass == 0)
   {
   /* make sure we wrote the header info */
   if (!(png_ptr->mode & PNG_WROTE_INFO_BEFORE_PLTE))
      png_error(png_ptr,
         "png_write_info was never called before png_write_row.");

   /* check for transforms that have been set but were defined out */
#if !defined(PNG_WRITE_INVERT_SUPPORTED) && defined(PNG_READ_INVERT_SUPPORTED)
   if (png_ptr->transformations & PNG_INVERT_MONO)
      png_warning(png_ptr, "PNG_WRITE_INVERT_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_FILLER_SUPPORTED) && defined(PNG_READ_FILLER_SUPPORTED)
   if (png_ptr->transformations & PNG_FILLER)
      png_warning(png_ptr, "PNG_WRITE_FILLER_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_PACKSWAP_SUPPORTED) && defined(PNG_READ_PACKSWAP_SUPPORTED)
   if (png_ptr->transformations & PNG_PACKSWAP)
      png_warning(png_ptr, "PNG_WRITE_PACKSWAP_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_PACK_SUPPORTED) && defined(PNG_READ_PACK_SUPPORTED)
   if (png_ptr->transformations & PNG_PACK)
      png_warning(png_ptr, "PNG_WRITE_PACK_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_SHIFT_SUPPORTED) && defined(PNG_READ_SHIFT_SUPPORTED)
   if (png_ptr->transformations & PNG_SHIFT)
      png_warning(png_ptr, "PNG_WRITE_SHIFT_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_BGR_SUPPORTED) && defined(PNG_READ_BGR_SUPPORTED)
   if (png_ptr->transformations & PNG_BGR)
      png_warning(png_ptr, "PNG_WRITE_BGR_SUPPORTED is not defined.");
#endif
#if !defined(PNG_WRITE_SWAP_SUPPORTED) && defined(PNG_READ_SWAP_SUPPORTED)
   if (png_ptr->transformations & PNG_SWAP_BYTES)
      png_warning(png_ptr, "PNG_WRITE_SWAP_SUPPORTED is not defined.");
#endif

      png_write_start_row(png_ptr);
   }

   /* set up row info for transformations */
   png_ptr->row_info.color_type = png_ptr->color_type;
   png_ptr->row_info.width = png_ptr->usr_width;
   png_ptr->row_info.channels = png_ptr->usr_channels;
   png_ptr->row_info.bit_depth = png_ptr->usr_bit_depth;
   png_ptr->row_info.pixel_depth = (uint8_t)(png_ptr->row_info.bit_depth *
      png_ptr->row_info.channels);

   png_ptr->row_info.rowbytes = PNG_ROWBYTES(png_ptr->row_info.pixel_depth,
      png_ptr->row_info.width);

   png_debug1(3, "row_info->color_type = %d\n", png_ptr->row_info.color_type);
   png_debug1(3, "row_info->width = %lu\n", png_ptr->row_info.width);
   png_debug1(3, "row_info->channels = %d\n", png_ptr->row_info.channels);
   png_debug1(3, "row_info->bit_depth = %d\n", png_ptr->row_info.bit_depth);
   png_debug1(3, "row_info->pixel_depth = %d\n", png_ptr->row_info.pixel_depth);
   png_debug1(3, "row_info->rowbytes = %lu\n", png_ptr->row_info.rowbytes);

   /* Copy user's row into buffer, leaving room for filter byte. */
   png_memcpy_check(png_ptr, png_ptr->row_buf + 1, row,
      png_ptr->row_info.rowbytes);

   /* handle other transformations */
   if (png_ptr->transformations)
      png_do_write_transformations(png_ptr);

   /* Find a filter if necessary, filter the row and write it out. */
   png_write_find_filter(png_ptr, &(png_ptr->row_info));

   if (png_ptr->write_row_fn != NULL)
      (*(png_ptr->write_row_fn))(png_ptr, png_ptr->row_number, png_ptr->pass);
}

#if defined(PNG_WRITE_FLUSH_SUPPORTED)
/* Set the automatic flush interval or 0 to turn flushing off */
void
png_set_flush(png_structp png_ptr, int nrows)
{
   png_debug(1, "in png_set_flush\n");
   if (png_ptr == NULL)
      return;
   png_ptr->flush_dist = (nrows < 0 ? 0 : nrows);
}

/* flush the current output buffers now */
void
png_write_flush(png_structp png_ptr)
{
   int wrote_IDAT;

   png_debug(1, "in png_write_flush\n");
   if (png_ptr == NULL)
      return;
   /* We have already written out all of the data */
   if (png_ptr->row_number >= png_ptr->num_rows)
     return;

   do
   {
      int ret;

      /* compress the data */
      ret = deflate(&png_ptr->zstream, Z_SYNC_FLUSH);
      wrote_IDAT = 0;

      /* check for compression errors */
      if (ret != Z_OK)
      {
         if (png_ptr->zstream.msg != NULL)
            png_error(png_ptr, png_ptr->zstream.msg);
         else
            png_error(png_ptr, "zlib error");
      }

      if (!(png_ptr->zstream.avail_out))
      {
         /* write the IDAT and reset the zlib output buffer */
         png_write_IDAT(png_ptr, png_ptr->zbuf,
                        png_ptr->zbuf_size);
         png_ptr->zstream.next_out = png_ptr->zbuf;
         png_ptr->zstream.avail_out = (uint32_t)png_ptr->zbuf_size;
         wrote_IDAT = 1;
      }
   } while(wrote_IDAT == 1);

   /* If there is any data left to be output, write it into a new IDAT */
   if (png_ptr->zbuf_size != png_ptr->zstream.avail_out)
   {
      /* write the IDAT and reset the zlib output buffer */
      png_write_IDAT(png_ptr, png_ptr->zbuf,
                     png_ptr->zbuf_size - png_ptr->zstream.avail_out);
      png_ptr->zstream.next_out = png_ptr->zbuf;
      png_ptr->zstream.avail_out = (uint32_t)png_ptr->zbuf_size;
   }
   png_ptr->flush_rows = 0;
   png_flush(png_ptr);
}
#endif /* PNG_WRITE_FLUSH_SUPPORTED */

/* free all memory used by the write */
void
png_destroy_write_struct(png_structpp png_ptr_ptr, png_info** info_ptr_ptr)
{
   png_structp png_ptr = NULL;
   png_info* info_ptr = NULL;
#ifdef PNG_USER_MEM_SUPPORTED
   png_free_ptr free_fn = NULL;
   void* mem_ptr = NULL;
#endif

   png_debug(1, "in png_destroy_write_struct\n");
   if (png_ptr_ptr != NULL)
   {
      png_ptr = *png_ptr_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
      free_fn = png_ptr->free_fn;
      mem_ptr = png_ptr->mem_ptr;
#endif
   }

   if (info_ptr_ptr != NULL)
      info_ptr = *info_ptr_ptr;

   if (info_ptr != NULL)
   {
      png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
      if (png_ptr->num_chunk_list)
      {
         png_free(png_ptr, png_ptr->chunk_list);
         png_ptr->chunk_list=NULL;
         png_ptr->num_chunk_list=0;
      }
#endif

#ifdef PNG_USER_MEM_SUPPORTED
      png_destroy_struct_2((void*)info_ptr, (png_free_ptr)free_fn,
         (void*)mem_ptr);
#else
      png_destroy_struct((void*)info_ptr);
#endif
      *info_ptr_ptr = NULL;
   }

   if (png_ptr != NULL)
   {
      png_write_destroy(png_ptr);
#ifdef PNG_USER_MEM_SUPPORTED
      png_destroy_struct_2((void*)png_ptr, (png_free_ptr)free_fn,
         (void*)mem_ptr);
#else
      png_destroy_struct((void*)png_ptr);
#endif
      *png_ptr_ptr = NULL;
   }
}


/* Free any memory used in png_ptr struct (old method) */
void png_write_destroy(png_structp png_ptr)
{
   png_error_ptr error_fn;
   png_error_ptr warning_fn;
   void* error_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
   png_free_ptr free_fn;
#endif

   png_debug(1, "in png_write_destroy\n");
   /* free any memory zlib uses */
   deflateEnd(&png_ptr->zstream);

   /* free our memory.  png_free checks NULL for us. */
   png_free(png_ptr, png_ptr->zbuf);
   png_free(png_ptr, png_ptr->row_buf);
   png_free(png_ptr, png_ptr->prev_row);
   png_free(png_ptr, png_ptr->sub_row);
   png_free(png_ptr, png_ptr->up_row);
   png_free(png_ptr, png_ptr->avg_row);
   png_free(png_ptr, png_ptr->paeth_row);

   error_fn = png_ptr->error_fn;
   warning_fn = png_ptr->warning_fn;
   error_ptr = png_ptr->error_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
   free_fn = png_ptr->free_fn;
#endif

   memset(png_ptr, 0, sizeof (png_struct));

   png_ptr->error_fn = error_fn;
   png_ptr->warning_fn = warning_fn;
   png_ptr->error_ptr = error_ptr;
#ifdef PNG_USER_MEM_SUPPORTED
   png_ptr->free_fn = free_fn;
#endif
}

/* Allow the application to select one or more row filters to use. */
void
png_set_filter(png_structp png_ptr, int method, int filters)
{
   png_debug(1, "in png_set_filter\n");
   if (png_ptr == NULL)
      return;
   if (method == PNG_FILTER_TYPE_BASE)
   {
      switch (filters & (PNG_ALL_FILTERS | 0x07))
      {
         case 5:
         case 6:
         case 7: png_warning(png_ptr, "Unknown row filter for method 0");
         case PNG_FILTER_VALUE_NONE:  png_ptr->do_filter=PNG_FILTER_NONE; break;
         case PNG_FILTER_VALUE_SUB:   png_ptr->do_filter=PNG_FILTER_SUB;  break;
         case PNG_FILTER_VALUE_UP:    png_ptr->do_filter=PNG_FILTER_UP;   break;
         case PNG_FILTER_VALUE_AVG:   png_ptr->do_filter=PNG_FILTER_AVG;  break;
         case PNG_FILTER_VALUE_PAETH: png_ptr->do_filter=PNG_FILTER_PAETH;break;
         default: png_ptr->do_filter = (uint8_t)filters; break;
      }

      /* If we have allocated the row_buf, this means we have already started
       * with the image and we should have allocated all of the filter buffers
       * that have been selected.  If prev_row isn't already allocated, then
       * it is too late to start using the filters that need it, since we
       * will be missing the data in the previous row.  If an application
       * wants to start and stop using particular filters during compression,
       * it should start out with all of the filters, and then add and
       * remove them after the start of compression.
       */
      if (png_ptr->row_buf != NULL)
      {
         if ((png_ptr->do_filter & PNG_FILTER_SUB) && png_ptr->sub_row == NULL)
         {
            png_ptr->sub_row = (uint8_t*)png_malloc(png_ptr,
              (png_ptr->rowbytes + 1));
            png_ptr->sub_row[0] = PNG_FILTER_VALUE_SUB;
         }

         if ((png_ptr->do_filter & PNG_FILTER_UP) && png_ptr->up_row == NULL)
         {
            if (png_ptr->prev_row == NULL)
            {
               png_warning(png_ptr, "Can't add Up filter after starting");
               png_ptr->do_filter &= ~PNG_FILTER_UP;
            }
            else
            {
               png_ptr->up_row = (uint8_t*)png_malloc(png_ptr,
                  (png_ptr->rowbytes + 1));
               png_ptr->up_row[0] = PNG_FILTER_VALUE_UP;
            }
         }

         if ((png_ptr->do_filter & PNG_FILTER_AVG) && png_ptr->avg_row == NULL)
         {
            if (png_ptr->prev_row == NULL)
            {
               png_warning(png_ptr, "Can't add Average filter after starting");
               png_ptr->do_filter &= ~PNG_FILTER_AVG;
            }
            else
            {
               png_ptr->avg_row = (uint8_t*)png_malloc(png_ptr,
                  (png_ptr->rowbytes + 1));
               png_ptr->avg_row[0] = PNG_FILTER_VALUE_AVG;
            }
         }

         if ((png_ptr->do_filter & PNG_FILTER_PAETH) &&
             png_ptr->paeth_row == NULL)
         {
            if (png_ptr->prev_row == NULL)
            {
               png_warning(png_ptr, "Can't add Paeth filter after starting");
               png_ptr->do_filter &= (uint8_t)(~PNG_FILTER_PAETH);
            }
            else
            {
               png_ptr->paeth_row = (uint8_t*)png_malloc(png_ptr,
                  (png_ptr->rowbytes + 1));
               png_ptr->paeth_row[0] = PNG_FILTER_VALUE_PAETH;
            }
         }

         if (png_ptr->do_filter == PNG_NO_FILTERS)
            png_ptr->do_filter = PNG_FILTER_NONE;
      }
   }
   else
      png_error(png_ptr, "Unknown custom filter method");
}

void png_set_compression_level(png_structp png_ptr, int level)
{
   png_debug(1, "in png_set_compression_level\n");
   if (png_ptr == NULL)
      return;
   png_ptr->flags |= PNG_FLAG_ZLIB_CUSTOM_LEVEL;
   png_ptr->zlib_level = level;
}

void png_set_compression_mem_level(png_structp png_ptr, int mem_level)
{
   png_debug(1, "in png_set_compression_mem_level\n");
   if (png_ptr == NULL)
      return;
   png_ptr->flags |= PNG_FLAG_ZLIB_CUSTOM_MEM_LEVEL;
   png_ptr->zlib_mem_level = mem_level;
}

void png_set_compression_strategy(png_structp png_ptr, int strategy)
{
   png_debug(1, "in png_set_compression_strategy\n");
   if (png_ptr == NULL)
      return;
   png_ptr->flags |= PNG_FLAG_ZLIB_CUSTOM_STRATEGY;
   png_ptr->zlib_strategy = strategy;
}

void png_set_compression_window_bits(png_structp png_ptr, int window_bits)
{
   if (png_ptr == NULL)
      return;
   if (window_bits > 15)
      png_warning(png_ptr, "Only compression windows <= 32k supported by PNG");
   else if (window_bits < 8)
      png_warning(png_ptr, "Only compression windows >= 256 supported by PNG");
#ifndef WBITS_8_OK
   /* avoid libpng bug with 256-byte windows */
   if (window_bits == 8)
     {
       png_warning(png_ptr, "Compression window is being reset to 512");
       window_bits=9;
     }
#endif
   png_ptr->flags |= PNG_FLAG_ZLIB_CUSTOM_WINDOW_BITS;
   png_ptr->zlib_window_bits = window_bits;
}

void
png_set_compression_method(png_structp png_ptr, int method)
{
   png_debug(1, "in png_set_compression_method\n");
   if (png_ptr == NULL)
      return;
   if (method != 8)
      png_warning(png_ptr, "Only compression method 8 is supported by PNG");
   png_ptr->flags |= PNG_FLAG_ZLIB_CUSTOM_METHOD;
   png_ptr->zlib_method = method;
}

void
png_set_write_status_fn(png_structp png_ptr, png_write_status_ptr write_row_fn)
{
   if (png_ptr == NULL)
      return;
   png_ptr->write_row_fn = write_row_fn;
}

#if defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
void
png_set_write_user_transform_fn(png_structp png_ptr, png_user_transform_ptr
   write_user_transform_fn)
{
   png_debug(1, "in png_set_write_user_transform_fn\n");
   if (png_ptr == NULL)
      return;
   png_ptr->transformations |= PNG_USER_TRANSFORM;
   png_ptr->write_user_transform_fn = write_user_transform_fn;
}
#endif


#if defined(PNG_INFO_IMAGE_SUPPORTED)
void
png_write_png(png_structp png_ptr, png_info* info_ptr,
              int transforms, void * params)
{
   if (png_ptr == NULL || info_ptr == NULL)
      return;
#if defined(PNG_WRITE_INVERT_ALPHA_SUPPORTED)
   /* invert the alpha channel from opacity to transparency */
   if (transforms & PNG_TRANSFORM_INVERT_ALPHA)
       png_set_invert_alpha(png_ptr);
#endif

   /* Write the file header information. */
   png_write_info(png_ptr, info_ptr);

   /* ------ these transformations don't touch the info structure ------- */

#if defined(PNG_WRITE_INVERT_SUPPORTED)
   /* invert monochrome pixels */
   if (transforms & PNG_TRANSFORM_INVERT_MONO)
       png_set_invert_mono(png_ptr);
#endif

#if defined(PNG_WRITE_SHIFT_SUPPORTED)
   /* Shift the pixels up to a legal bit depth and fill in
    * as appropriate to correctly scale the image.
    */
   if ((transforms & PNG_TRANSFORM_SHIFT)
               && (info_ptr->valid & PNG_INFO_sBIT))
       png_set_shift(png_ptr, &info_ptr->sig_bit);
#endif

#if defined(PNG_WRITE_PACK_SUPPORTED)
   /* pack pixels into bytes */
   if (transforms & PNG_TRANSFORM_PACKING)
       png_set_packing(png_ptr);
#endif

#if defined(PNG_WRITE_SWAP_ALPHA_SUPPORTED)
   /* swap location of alpha bytes from ARGB to RGBA */
   if (transforms & PNG_TRANSFORM_SWAP_ALPHA)
       png_set_swap_alpha(png_ptr);
#endif

#if defined(PNG_WRITE_FILLER_SUPPORTED)
   /* Get rid of filler (OR ALPHA) bytes, pack XRGB/RGBX/ARGB/RGBA into
    * RGB (4 channels -> 3 channels). The second parameter is not used.
    */
   if (transforms & PNG_TRANSFORM_STRIP_FILLER)
       png_set_filler(png_ptr, 0, PNG_FILLER_BEFORE);
#endif

#if defined(PNG_WRITE_BGR_SUPPORTED)
   /* flip BGR pixels to RGB */
   if (transforms & PNG_TRANSFORM_BGR)
       png_set_bgr(png_ptr);
#endif

#if defined(PNG_WRITE_SWAP_SUPPORTED)
   /* swap bytes of 16-bit files to most significant byte first */
   if (transforms & PNG_TRANSFORM_SWAP_ENDIAN)
       png_set_swap(png_ptr);
#endif

#if defined(PNG_WRITE_PACKSWAP_SUPPORTED)
   /* swap bits of 1, 2, 4 bit packed pixel formats */
   if (transforms & PNG_TRANSFORM_PACKSWAP)
       png_set_packswap(png_ptr);
#endif

   /* ----------------------- end of transformations ------------------- */

   /* write the bits */
   if (info_ptr->valid & PNG_INFO_IDAT)
       png_write_image(png_ptr, info_ptr->row_pointers);

   /* It is REQUIRED to call this to finish writing the rest of the file */
   png_write_end(png_ptr, info_ptr);

   if(transforms == 0 || params == NULL)
      /* quiet compiler warnings */ return;
}
#endif
#endif /* PNG_WRITE_SUPPORTED */
