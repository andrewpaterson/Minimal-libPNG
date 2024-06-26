
/* pngpread.c - read a png file in push mode
 *
 * Last changed in libpng 1.2.13 November 13, 2006
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2006 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 */

#include "png.h"

#ifdef PNG_PROGRESSIVE_READ_SUPPORTED

/* push model modes */
#define PNG_READ_SIG_MODE   0
#define PNG_READ_CHUNK_MODE 1
#define PNG_READ_IDAT_MODE  2
#define PNG_SKIP_MODE       3
#define PNG_READ_DONE_MODE  6
#define PNG_ERROR_MODE      8

void 
png_process_data(png_structp png_ptr, png_info* info_ptr,
   uint8_t* buffer, size_t buffer_size)
{
   if(png_ptr == NULL) return;
   png_push_restore_buffer(png_ptr, buffer, buffer_size);

   while (png_ptr->buffer_size)
   {
      png_process_some_data(png_ptr, info_ptr);
   }
}

/* What we do with the incoming data depends on what we were previously
 * doing before we ran out of data...
 */
void png_process_some_data(png_structp png_ptr, png_info* info_ptr)
{
   if(png_ptr == NULL) return;
   switch (png_ptr->process_mode)
   {
      case PNG_READ_SIG_MODE:
      {
         png_push_read_sig(png_ptr, info_ptr);
         break;
      }
      case PNG_READ_CHUNK_MODE:
      {
         png_push_read_chunk(png_ptr, info_ptr);
         break;
      }
      case PNG_READ_IDAT_MODE:
      {
         png_push_read_IDAT(png_ptr);
         break;
      }
      case PNG_SKIP_MODE:
      {
         png_push_crc_finish(png_ptr);
         break;
      }
      default:
      {
         png_ptr->buffer_size = 0;
         break;
      }
   }
}

/* Read any remaining signature bytes from the stream and compare them with
 * the correct PNG signature.  It is possible that this routine is called
 * with bytes already read from the signature, either because they have been
 * checked by the calling application, or because of multiple calls to this
 * routine.
 */
void png_push_read_sig(png_structp png_ptr, png_info* info_ptr)
{
   size_t num_checked = png_ptr->sig_bytes,
             num_to_check = 8 - num_checked;

   if (png_ptr->buffer_size < num_to_check)
   {
      num_to_check = png_ptr->buffer_size;
   }

   png_push_fill_buffer(png_ptr, &(info_ptr->signature[num_checked]),
      num_to_check);
   png_ptr->sig_bytes = (uint8_t)(png_ptr->sig_bytes+num_to_check);

   if (png_sig_cmp(info_ptr->signature, num_checked, num_to_check))
   {
      if (num_checked < 4 &&
          png_sig_cmp(info_ptr->signature, num_checked, num_to_check - 4))
         png_error(png_ptr, "Not a PNG file");
      else
         png_error(png_ptr, "PNG file corrupted by ASCII conversion");
   }
   else
   {
      if (png_ptr->sig_bytes >= 8)
      {
         png_ptr->process_mode = PNG_READ_CHUNK_MODE;
      }
   }
}

void png_push_read_chunk(png_structp png_ptr, png_info* info_ptr)
{
   /* First we make sure we have enough data for the 4 byte chunk name
    * and the 4 byte chunk length before proceeding with decoding the
    * chunk data.  To fully decode each of these chunks, we also make
    * sure we have enough data in the buffer for the 4 byte CRC at the
    * end of every chunk (except IDAT, which is handled separately).
    */
   if (!(png_ptr->mode & PNG_HAVE_CHUNK_HEADER))
   {
      uint8_t chunk_length[4];

      if (png_ptr->buffer_size < 8)
      {
         png_push_save_buffer(png_ptr);
         return;
      }

      png_push_fill_buffer(png_ptr, chunk_length, 4);
      png_ptr->push_length = png_get_uint_31(png_ptr,chunk_length);
      png_reset_crc(png_ptr);
      png_crc_read(png_ptr, png_ptr->chunk_name, 4);
      png_ptr->mode |= PNG_HAVE_CHUNK_HEADER;
   }

   if (!memcmp(png_ptr->chunk_name, (uint8_t*)png_IDAT, 4))
     if(png_ptr->mode & PNG_AFTER_IDAT)
        png_ptr->mode |= PNG_HAVE_CHUNK_AFTER_IDAT;

   if (!memcmp(png_ptr->chunk_name, png_IHDR, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_handle_IHDR(png_ptr, info_ptr, png_ptr->push_length);
   }
   else if (!memcmp(png_ptr->chunk_name, png_IEND, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_handle_IEND(png_ptr, info_ptr, png_ptr->push_length);

      png_ptr->process_mode = PNG_READ_DONE_MODE;
      png_push_have_end(png_ptr, info_ptr);
   }
#ifdef PNG_HANDLE_AS_UNKNOWN_SUPPORTED
   else if (png_handle_as_unknown(png_ptr, png_ptr->chunk_name))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      if (!memcmp(png_ptr->chunk_name, png_IDAT, 4))
         png_ptr->mode |= PNG_HAVE_IDAT;
      png_handle_unknown(png_ptr, info_ptr, png_ptr->push_length);
      if (!memcmp(png_ptr->chunk_name, png_PLTE, 4))
         png_ptr->mode |= PNG_HAVE_PLTE;
      else if (!memcmp(png_ptr->chunk_name, png_IDAT, 4))
      {
         if (!(png_ptr->mode & PNG_HAVE_IHDR))
            png_error(png_ptr, "Missing IHDR before IDAT");
         else if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE &&
                  !(png_ptr->mode & PNG_HAVE_PLTE))
            png_error(png_ptr, "Missing PLTE before IDAT");
      }
   }
#endif
   else if (!memcmp(png_ptr->chunk_name, png_PLTE, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_handle_PLTE(png_ptr, info_ptr, png_ptr->push_length);
   }
   else if (!memcmp(png_ptr->chunk_name, (uint8_t*)png_IDAT, 4))
   {
      /* If we reach an IDAT chunk, this means we have read all of the
       * header chunks, and we can start reading the image (or if this
       * is called after the image has been read - we have an error).
       */
     if (!(png_ptr->mode & PNG_HAVE_IHDR))
       png_error(png_ptr, "Missing IHDR before IDAT");
     else if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE &&
         !(png_ptr->mode & PNG_HAVE_PLTE))
       png_error(png_ptr, "Missing PLTE before IDAT");

      if (png_ptr->mode & PNG_HAVE_IDAT)
      {
         if (!(png_ptr->mode & PNG_HAVE_CHUNK_AFTER_IDAT))
           if (png_ptr->push_length == 0)
              return;

         if (png_ptr->mode & PNG_AFTER_IDAT)
            png_error(png_ptr, "Too many IDAT's found");
      }

      png_ptr->idat_size = png_ptr->push_length;
      png_ptr->mode |= PNG_HAVE_IDAT;
      png_ptr->process_mode = PNG_READ_IDAT_MODE;
      png_push_have_info(png_ptr, info_ptr);
      png_ptr->zstream.avail_out = (uint32_t)png_ptr->irowbytes;
      png_ptr->zstream.next_out = png_ptr->row_buf;
      return;
   }
#if defined(PNG_READ_sBIT_SUPPORTED)
   else if (!memcmp(png_ptr->chunk_name, png_sBIT, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_handle_sBIT(png_ptr, info_ptr, png_ptr->push_length);
   }
#endif
#if defined(PNG_READ_sRGB_SUPPORTED)
   else if (!memcmp(png_ptr->chunk_name, png_sRGB, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_handle_sRGB(png_ptr, info_ptr, png_ptr->push_length);
   }
#endif
#if defined(PNG_READ_sPLT_SUPPORTED)
   else if (!memcmp(png_ptr->chunk_name, png_sPLT, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_handle_sPLT(png_ptr, info_ptr, png_ptr->push_length);
   }
#endif
#if defined(PNG_READ_tRNS_SUPPORTED)
   else if (!memcmp(png_ptr->chunk_name, png_tRNS, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_handle_tRNS(png_ptr, info_ptr, png_ptr->push_length);
   }
#endif
#if defined(PNG_READ_hIST_SUPPORTED)
   else if (!memcmp(png_ptr->chunk_name, png_hIST, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_handle_hIST(png_ptr, info_ptr, png_ptr->push_length);
   }
#endif
#if defined(PNG_READ_pHYs_SUPPORTED)
   else if (!memcmp(png_ptr->chunk_name, png_pHYs, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_handle_pHYs(png_ptr, info_ptr, png_ptr->push_length);
   }
#endif
#if defined(PNG_READ_pCAL_SUPPORTED)
   else if (!memcmp(png_ptr->chunk_name, png_pCAL, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_handle_pCAL(png_ptr, info_ptr, png_ptr->push_length);
   }
#endif
   else
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }
      png_push_handle_unknown(png_ptr, info_ptr, png_ptr->push_length);
   }

   png_ptr->mode &= ~PNG_HAVE_CHUNK_HEADER;
}

void png_push_crc_skip(png_structp png_ptr, uint32_t skip)
{
   png_ptr->process_mode = PNG_SKIP_MODE;
   png_ptr->skip_length = skip;
}

void png_push_crc_finish(png_structp png_ptr)
{
   if (png_ptr->skip_length && png_ptr->save_buffer_size)
   {
      size_t save_size;

      if (png_ptr->skip_length < (uint32_t)png_ptr->save_buffer_size)
         save_size = (size_t)png_ptr->skip_length;
      else
         save_size = png_ptr->save_buffer_size;

      png_calculate_crc(png_ptr, png_ptr->save_buffer_ptr, save_size);

      png_ptr->skip_length -= (int)save_size;
      png_ptr->buffer_size -= (int)save_size;
      png_ptr->save_buffer_size -= (int)save_size;
      png_ptr->save_buffer_ptr += (int)save_size;
   }
   if (png_ptr->skip_length && png_ptr->current_buffer_size)
   {
      size_t save_size;

      if (png_ptr->skip_length < (uint32_t)png_ptr->current_buffer_size)
         save_size = (size_t)png_ptr->skip_length;
      else
         save_size = png_ptr->current_buffer_size;

      png_calculate_crc(png_ptr, png_ptr->current_buffer_ptr, save_size);

      png_ptr->skip_length -= (int)save_size;
      png_ptr->buffer_size -= (int)save_size;
      png_ptr->current_buffer_size -= (int)save_size;
      png_ptr->current_buffer_ptr += (int)save_size;
   }
   if (!png_ptr->skip_length)
   {
      if (png_ptr->buffer_size < 4)
      {
         png_push_save_buffer(png_ptr);
         return;
      }

      png_crc_finish(png_ptr, 0);
      png_ptr->process_mode = PNG_READ_CHUNK_MODE;
   }
}

void 
png_push_fill_buffer(png_structp png_ptr, uint8_t* buffer, size_t length)
{
   uint8_t* ptr;

   if(png_ptr == NULL) return;
   ptr = buffer;
   if (png_ptr->save_buffer_size)
   {
      size_t save_size;

      if (length < png_ptr->save_buffer_size)
         save_size = length;
      else
         save_size = png_ptr->save_buffer_size;

      memcpy(ptr, png_ptr->save_buffer_ptr, save_size);
      length -= save_size;
      ptr += save_size;
      png_ptr->buffer_size -= save_size;
      png_ptr->save_buffer_size -= save_size;
      png_ptr->save_buffer_ptr += save_size;
   }
   if (length && png_ptr->current_buffer_size)
   {
      size_t save_size;

      if (length < png_ptr->current_buffer_size)
         save_size = length;
      else
         save_size = png_ptr->current_buffer_size;

      memcpy(ptr, png_ptr->current_buffer_ptr, save_size);
      png_ptr->buffer_size -= save_size;
      png_ptr->current_buffer_size -= save_size;
      png_ptr->current_buffer_ptr += save_size;
   }
}

void png_push_save_buffer(png_structp png_ptr)
{
   if (png_ptr->save_buffer_size)
   {
      if (png_ptr->save_buffer_ptr != png_ptr->save_buffer)
      {
         size_t i,istop;
         uint8_t* sp;
         uint8_t* dp;

         istop = png_ptr->save_buffer_size;
         for (i = 0, sp = png_ptr->save_buffer_ptr, dp = png_ptr->save_buffer;
            i < istop; i++, sp++, dp++)
         {
            *dp = *sp;
         }
      }
   }
   if (png_ptr->save_buffer_size + png_ptr->current_buffer_size >
      png_ptr->save_buffer_max)
   {
      size_t new_max;
      uint8_t* old_buffer;

      if (png_ptr->save_buffer_size > PNG_SIZE_MAX -
         (png_ptr->current_buffer_size + 256))
      {
        png_error(png_ptr, "Potential overflow of save_buffer");
      }
      new_max = png_ptr->save_buffer_size + png_ptr->current_buffer_size + 256;
      old_buffer = png_ptr->save_buffer;
      png_ptr->save_buffer = (uint8_t*)png_malloc(png_ptr,
         (uint32_t)new_max);
      memcpy(png_ptr->save_buffer, old_buffer, png_ptr->save_buffer_size);
      png_free(png_ptr, old_buffer);
      png_ptr->save_buffer_max = new_max;
   }
   if (png_ptr->current_buffer_size)
   {
      memcpy(png_ptr->save_buffer + png_ptr->save_buffer_size,
         png_ptr->current_buffer_ptr, png_ptr->current_buffer_size);
      png_ptr->save_buffer_size += png_ptr->current_buffer_size;
      png_ptr->current_buffer_size = 0;
   }
   png_ptr->save_buffer_ptr = png_ptr->save_buffer;
   png_ptr->buffer_size = 0;
}

void png_push_restore_buffer(png_structp png_ptr, uint8_t* buffer,
   size_t buffer_length)
{
   png_ptr->current_buffer = buffer;
   png_ptr->current_buffer_size = buffer_length;
   png_ptr->buffer_size = buffer_length + png_ptr->save_buffer_size;
   png_ptr->current_buffer_ptr = png_ptr->current_buffer;
}

void png_push_read_IDAT(png_structp png_ptr)
{
   if (!(png_ptr->mode & PNG_HAVE_CHUNK_HEADER))
   {
      uint8_t chunk_length[4];

      if (png_ptr->buffer_size < 8)
      {
         png_push_save_buffer(png_ptr);
         return;
      }

      png_push_fill_buffer(png_ptr, chunk_length, 4);
      png_ptr->push_length = png_get_uint_31(png_ptr,chunk_length);
      png_reset_crc(png_ptr);
      png_crc_read(png_ptr, png_ptr->chunk_name, 4);
      png_ptr->mode |= PNG_HAVE_CHUNK_HEADER;

      if (memcmp(png_ptr->chunk_name, (uint8_t*)png_IDAT, 4))
      {
         png_ptr->process_mode = PNG_READ_CHUNK_MODE;
         if (!(png_ptr->flags & PNG_FLAG_ZLIB_FINISHED))
            png_error(png_ptr, "Not enough compressed data");
         return;
      }

      png_ptr->idat_size = png_ptr->push_length;
   }
   if (png_ptr->idat_size && png_ptr->save_buffer_size)
   {
      size_t save_size;

      if (png_ptr->idat_size < (uint32_t)png_ptr->save_buffer_size)
      {
         save_size = (size_t)png_ptr->idat_size;
         /* check for overflow */
         if((uint32_t)save_size != png_ptr->idat_size)
            png_error(png_ptr, "save_size overflowed in pngpread");
      }
      else
         save_size = png_ptr->save_buffer_size;

      png_calculate_crc(png_ptr, png_ptr->save_buffer_ptr, save_size);
      if (!(png_ptr->flags & PNG_FLAG_ZLIB_FINISHED))
         png_process_IDAT_data(png_ptr, png_ptr->save_buffer_ptr, save_size);
      png_ptr->idat_size -= (int)save_size;
      png_ptr->buffer_size -= (int)save_size;
      png_ptr->save_buffer_size -= (int)save_size;
      png_ptr->save_buffer_ptr += (int)save_size;
   }
   if (png_ptr->idat_size && png_ptr->current_buffer_size)
   {
      size_t save_size;

      if (png_ptr->idat_size < (uint32_t)png_ptr->current_buffer_size)
      {
         save_size = (size_t)png_ptr->idat_size;
         /* check for overflow */
         if((uint32_t)save_size != png_ptr->idat_size)
            png_error(png_ptr, "save_size overflowed in pngpread");
      }
      else
         save_size = png_ptr->current_buffer_size;

      png_calculate_crc(png_ptr, png_ptr->current_buffer_ptr, save_size);
      if (!(png_ptr->flags & PNG_FLAG_ZLIB_FINISHED))
        png_process_IDAT_data(png_ptr, png_ptr->current_buffer_ptr, save_size);

      png_ptr->idat_size -= (int)save_size;
      png_ptr->buffer_size -= (int)save_size;
      png_ptr->current_buffer_size -= (int)save_size;
      png_ptr->current_buffer_ptr += (int)save_size;
   }
   if (!png_ptr->idat_size)
   {
      if (png_ptr->buffer_size < 4)
      {
         png_push_save_buffer(png_ptr);
         return;
      }

      png_crc_finish(png_ptr, 0);
      png_ptr->mode &= ~PNG_HAVE_CHUNK_HEADER;
      png_ptr->mode |= PNG_AFTER_IDAT;
   }
}

void png_process_IDAT_data(png_structp png_ptr, uint8_t* buffer,
   size_t buffer_length)
{
   int ret;

   if ((png_ptr->flags & PNG_FLAG_ZLIB_FINISHED) && buffer_length)
      png_error(png_ptr, "Extra compression data");

   png_ptr->zstream.next_in = buffer;
   png_ptr->zstream.avail_in = (uint32_t)buffer_length;
   for(;;)
   {
      ret = inflate(&png_ptr->zstream, Z_PARTIAL_FLUSH);
      if (ret != Z_OK)
      {
         if (ret == Z_STREAM_END)
         {
            if (png_ptr->zstream.avail_in)
               png_error(png_ptr, "Extra compressed data");
            if (!(png_ptr->zstream.avail_out))
            {
               png_push_process_row(png_ptr);
            }

            png_ptr->mode |= PNG_AFTER_IDAT;
            png_ptr->flags |= PNG_FLAG_ZLIB_FINISHED;
            break;
         }
         else if (ret == Z_BUF_ERROR)
            break;
         else
            png_error(png_ptr, "Decompression Error");
      }
      if (!(png_ptr->zstream.avail_out))
      {
         if ((png_ptr->row_number == png_ptr->num_rows))
         {
           if (png_ptr->zstream.avail_in)
             png_warning(png_ptr, "Too much data in IDAT chunks");
           png_ptr->flags |= PNG_FLAG_ZLIB_FINISHED;
           break;
         }
         png_push_process_row(png_ptr);
         png_ptr->zstream.avail_out = (uint32_t)png_ptr->irowbytes;
         png_ptr->zstream.next_out = png_ptr->row_buf;
      }
      else
         break;
   }
}

void png_push_process_row(png_structp png_ptr)
{
   png_ptr->row_info.color_type = png_ptr->color_type;
   png_ptr->row_info.width = png_ptr->iwidth;
   png_ptr->row_info.channels = png_ptr->channels;
   png_ptr->row_info.bit_depth = png_ptr->bit_depth;
   png_ptr->row_info.pixel_depth = png_ptr->pixel_depth;

   png_ptr->row_info.rowbytes = PNG_ROWBYTES(png_ptr->row_info.pixel_depth,
       png_ptr->row_info.width);

   png_read_filter_row(png_ptr, &(png_ptr->row_info),
      png_ptr->row_buf + 1, png_ptr->prev_row + 1,
      (int)(png_ptr->row_buf[0]));

   png_memcpy_check(png_ptr, png_ptr->prev_row, png_ptr->row_buf,
      png_ptr->rowbytes + 1);

   if (png_ptr->transformations || (png_ptr->flags&PNG_FLAG_STRIP_ALPHA))
      png_do_read_transformations(png_ptr);

   png_push_have_row(png_ptr, png_ptr->row_buf + 1);
   png_read_push_finish_row(png_ptr);
}

void png_read_push_finish_row(png_structp png_ptr)
{
   png_ptr->row_number++;
   if (png_ptr->row_number < png_ptr->num_rows)
      return;

   if (png_ptr->interlaced)
   {
      png_ptr->row_number = 0;
      png_memset_check(png_ptr, png_ptr->prev_row, 0,
         png_ptr->rowbytes + 1);
      do
      {
         png_ptr->pass++;
         if ((png_ptr->pass == 1 && png_ptr->width < 5) ||
             (png_ptr->pass == 3 && png_ptr->width < 3) ||
             (png_ptr->pass == 5 && png_ptr->width < 2))
           png_ptr->pass++;

         if (png_ptr->pass > 7)
            png_ptr->pass--;
         if (png_ptr->pass >= 7)
            break;

         png_ptr->iwidth = (png_ptr->width +
            png_pass_inc[png_ptr->pass] - 1 -
            png_pass_start[png_ptr->pass]) /
            png_pass_inc[png_ptr->pass];

         png_ptr->irowbytes = PNG_ROWBYTES(png_ptr->pixel_depth,
            png_ptr->iwidth) + 1;

         if (png_ptr->transformations & PNG_INTERLACE)
            break;

         png_ptr->num_rows = (png_ptr->height +
            png_pass_yinc[png_ptr->pass] - 1 -
            png_pass_ystart[png_ptr->pass]) /
            png_pass_yinc[png_ptr->pass];

      } while (png_ptr->iwidth == 0 || png_ptr->num_rows == 0);
   }
}


/* This function is called when we haven't found a handler for this
 * chunk.  If there isn't a problem with the chunk itself (ie a bad chunk
 * name or a critical chunk), the chunk is (currently) silently ignored.
 */
void png_push_handle_unknown(png_structp png_ptr, png_info* info_ptr, uint32_t
   length)
{
   uint32_t skip=0;
   png_check_chunk_name(png_ptr, png_ptr->chunk_name);

   if (!(png_ptr->chunk_name[0] & 0x20))
   {
#if defined(PNG_READ_UNKNOWN_CHUNKS_SUPPORTED)
      if(png_handle_as_unknown(png_ptr, png_ptr->chunk_name) !=
           PNG_HANDLE_CHUNK_ALWAYS
#if defined(PNG_READ_USER_CHUNKS_SUPPORTED)
           && png_ptr->read_user_chunk_fn == NULL
#endif
         )
#endif
         png_chunk_error(png_ptr, "unknown critical chunk");

      /* to quiet compiler warnings about unused info_ptr */
      if (info_ptr == NULL)
         return;
   }

#if defined(PNG_READ_UNKNOWN_CHUNKS_SUPPORTED)
   if (png_ptr->flags & PNG_FLAG_KEEP_UNKNOWN_CHUNKS)
   {
       png_unknown_chunk chunk;

#ifdef PNG_MAX_MALLOC_64K
       if (length > (uint32_t)65535L)
       {
           png_warning(png_ptr, "unknown chunk too large to fit in memory");
           skip = length - (uint32_t)65535L;
           length = (uint32_t)65535L;
       }
#endif

       strcpy((char*)chunk.name, (char*)png_ptr->chunk_name);
       chunk.data = (uint8_t*)png_malloc(png_ptr, length);
       png_crc_read(png_ptr, chunk.data, length);
       chunk.size = length;
#if defined(PNG_READ_USER_CHUNKS_SUPPORTED)
       if(png_ptr->read_user_chunk_fn != NULL)
       {
          /* callback to user unknown chunk handler */
          if ((*(png_ptr->read_user_chunk_fn)) (png_ptr, &chunk) <= 0)
          {
             if (!(png_ptr->chunk_name[0] & 0x20))
                if(png_handle_as_unknown(png_ptr, png_ptr->chunk_name) !=
                     PNG_HANDLE_CHUNK_ALWAYS)
                   png_chunk_error(png_ptr, "unknown critical chunk");
          }
             png_set_unknown_chunks(png_ptr, info_ptr, &chunk, 1);
       }
       else
#endif
          png_set_unknown_chunks(png_ptr, info_ptr, &chunk, 1);
       png_free(png_ptr, chunk.data);
   }
   else
#endif
      skip=length;
   png_push_crc_skip(png_ptr, skip);
}

void png_push_have_info(png_structp png_ptr, png_info* info_ptr)
{
   if (png_ptr->info_fn != NULL)
      (*(png_ptr->info_fn))(png_ptr, info_ptr);
}

void png_push_have_end(png_structp png_ptr, png_info* info_ptr)
{
   if (png_ptr->end_fn != NULL)
      (*(png_ptr->end_fn))(png_ptr, info_ptr);
}

void png_push_have_row(png_structp png_ptr, uint8_t* row)
{
   if (png_ptr->row_fn != NULL)
      (*(png_ptr->row_fn))(png_ptr, row, png_ptr->row_number,
         (int)png_ptr->pass);
}

void 
png_progressive_combine_row (png_structp png_ptr,
   uint8_t* old_row, uint8_t* new_row)
{
   if(png_ptr == NULL) return;
   if (new_row != NULL)    /* new_row must == png_ptr->row_buf here. */
      png_combine_row(png_ptr, old_row, png_pass_dsp_mask[png_ptr->pass]);
}

void 
png_set_progressive_read_fn(png_structp png_ptr, void* progressive_ptr,
   png_progressive_info_ptr info_fn, png_progressive_row_ptr row_fn,
   png_progressive_end_ptr end_fn)
{
   if(png_ptr == NULL) return;
   png_ptr->info_fn = info_fn;
   png_ptr->row_fn = row_fn;
   png_ptr->end_fn = end_fn;

   png_set_read_fn(png_ptr, progressive_ptr, png_push_fill_buffer);
}

void* 
png_get_progressive_ptr(png_structp png_ptr)
{
   if(png_ptr == NULL) return (NULL);
   return png_ptr->io_ptr;
}
#endif /* PNG_PROGRESSIVE_READ_SUPPORTED */
