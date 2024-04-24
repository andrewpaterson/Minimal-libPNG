
/* pngrutil.c - utilities to read a PNG file
 *
 * Last changed in libpng 1.2.15 January 5, 2007
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2007 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * This file contains routines that are only called from within
 * libpng itself during the course of reading an image.
 */

#include "png.h"

#if defined(PNG_READ_SUPPORTED)

#ifdef PNG_FLOATING_POINT_SUPPORTED
#    define png_strtod(p,a,b) strtod(a,b)
#endif

uint32_t
png_get_uint_31(png_structp png_ptr, uint8_t* buf)
{
   uint32_t i = png_get_uint_32(buf);
   if (i > PNG_UINT_31_MAX)
     png_error(png_ptr, "PNG unsigned integer out of range.");
   return (i);
}
#ifndef PNG_READ_BIG_ENDIAN_SUPPORTED
/* Grab an unsigned 32-bit integer from a buffer in big-endian format. */
uint32_t
png_get_uint_32(uint8_t* buf)
{
   uint32_t i = ((uint32_t)(*buf) << 24) +
      ((uint32_t)(*(buf + 1)) << 16) +
      ((uint32_t)(*(buf + 2)) << 8) +
      (uint32_t)(*(buf + 3));

   return (i);
}

/* Grab a signed 32-bit integer from a buffer in big-endian format.  The
 * data is stored in the PNG file in two's complement format, and it is
 * assumed that the machine format for signed integers is the same. */
int32_t
png_get_int_32(uint8_t* buf)
{
   int32_t i = ((int32_t)(*buf) << 24) +
      ((int32_t)(*(buf + 1)) << 16) +
      ((int32_t)(*(buf + 2)) << 8) +
      (int32_t)(*(buf + 3));

   return (i);
}

/* Grab an unsigned 16-bit integer from a buffer in big-endian format. */
uint16_t
png_get_uint_16(uint8_t* buf)
{
   uint16_t i = (uint16_t)(((uint16_t)(*buf) << 8) +
      (uint16_t)(*(buf + 1)));

   return (i);
}
#endif /* PNG_READ_BIG_ENDIAN_SUPPORTED */

/* Read data, and (optionally) run it through the CRC. */
void png_crc_read(png_structp png_ptr, uint8_t* buf, size_t length)
{
   if(png_ptr == NULL) return;
   png_read_data(png_ptr, buf, length);
   png_calculate_crc(png_ptr, buf, length);
}

/* Optionally skip data and then check the CRC.  Depending on whether we
   are reading a ancillary or critical chunk, and how the program has set
   things up, we may calculate the CRC on the data and print a message.
   Returns '1' if there was a CRC error, '0' otherwise. */
int png_crc_finish(png_structp png_ptr, uint32_t skip)
{
   size_t i;
   size_t istop = png_ptr->zbuf_size;

   for (i = (size_t)skip; i > istop; i -= istop)
   {
      png_crc_read(png_ptr, png_ptr->zbuf, png_ptr->zbuf_size);
   }
   if (i)
   {
      png_crc_read(png_ptr, png_ptr->zbuf, i);
   }

   if (png_crc_error(png_ptr))
   {
      if (((png_ptr->chunk_name[0] & 0x20) &&                /* Ancillary */
           !(png_ptr->flags & PNG_FLAG_CRC_ANCILLARY_NOWARN)) ||
          (!(png_ptr->chunk_name[0] & 0x20) &&             /* Critical  */
          (png_ptr->flags & PNG_FLAG_CRC_CRITICAL_USE)))
      {
         png_chunk_warning(png_ptr, "CRC error");
      }
      else
      {
         png_chunk_error(png_ptr, "CRC error");
      }
      return (1);
   }

   return (0);
}

/* Compare the CRC stored in the PNG file with that calculated by libpng from
   the data it has read thus far. */
int png_crc_error(png_structp png_ptr)
{
   uint8_t crc_bytes[4];
   uint32_t crc;
   int need_crc = 1;

   if (png_ptr->chunk_name[0] & 0x20)                     /* ancillary */
   {
      if ((png_ptr->flags & PNG_FLAG_CRC_ANCILLARY_MASK) ==
          (PNG_FLAG_CRC_ANCILLARY_USE | PNG_FLAG_CRC_ANCILLARY_NOWARN))
         need_crc = 0;
   }
   else                                                    /* critical */
   {
      if (png_ptr->flags & PNG_FLAG_CRC_CRITICAL_IGNORE)
         need_crc = 0;
   }

   png_read_data(png_ptr, crc_bytes, 4);

   if (need_crc)
   {
      crc = png_get_uint_32(crc_bytes);
      return ((int)(crc != png_ptr->crc));
   }
   else
      return (0);
}



/* read and check the IDHR chunk */
void png_handle_IHDR(png_structp png_ptr, png_info* info_ptr, uint32_t length)
{
   uint8_t buf[13];
   uint32_t width, height;
   int bit_depth, color_type, compression_type, filter_type;
   int interlace_type;

   png_debug(1, "in png_handle_IHDR\n");

   if (png_ptr->mode & PNG_HAVE_IHDR)
      png_error(png_ptr, "Out of place IHDR");

   /* check the length */
   if (length != 13)
      png_error(png_ptr, "Invalid IHDR chunk");

   png_ptr->mode |= PNG_HAVE_IHDR;

   png_crc_read(png_ptr, buf, 13);
   png_crc_finish(png_ptr, 0);

   width = png_get_uint_31(png_ptr, buf);
   height = png_get_uint_31(png_ptr, buf + 4);
   bit_depth = buf[8];
   color_type = buf[9];
   compression_type = buf[10];
   filter_type = buf[11];
   interlace_type = buf[12];

   /* set internal variables */
   png_ptr->width = width;
   png_ptr->height = height;
   png_ptr->bit_depth = (uint8_t)bit_depth;
   png_ptr->interlaced = (uint8_t)interlace_type;
   png_ptr->color_type = (uint8_t)color_type;
   png_ptr->compression_type = (uint8_t)compression_type;

   /* find number of channels */
   switch (png_ptr->color_type)
   {
      case PNG_COLOR_TYPE_GRAY:
      case PNG_COLOR_TYPE_PALETTE:
         png_ptr->channels = 1;
         break;
      case PNG_COLOR_TYPE_RGB:
         png_ptr->channels = 3;
         break;
      case PNG_COLOR_TYPE_GRAY_ALPHA:
         png_ptr->channels = 2;
         break;
      case PNG_COLOR_TYPE_RGB_ALPHA:
         png_ptr->channels = 4;
         break;
   }

   /* set up other useful info */
   png_ptr->pixel_depth = (uint8_t)(png_ptr->bit_depth *
   png_ptr->channels);
   png_ptr->rowbytes = PNG_ROWBYTES(png_ptr->pixel_depth,png_ptr->width);
   png_debug1(3,"bit_depth = %d\n", png_ptr->bit_depth);
   png_debug1(3,"channels = %d\n", png_ptr->channels);
   png_debug1(3,"rowbytes = %lu\n", png_ptr->rowbytes);
   png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
      color_type, interlace_type, compression_type, filter_type);
}

/* read and check the palette */
void png_handle_PLTE(png_structp png_ptr, png_info* info_ptr, uint32_t length)
{
   png_color palette[PNG_MAX_PALETTE_LENGTH];
   int num, i;
#ifndef PNG_NO_POINTER_INDEXING
   png_colorp pal_ptr;
#endif

   png_debug(1, "in png_handle_PLTE\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before PLTE");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid PLTE after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->mode & PNG_HAVE_PLTE)
      png_error(png_ptr, "Duplicate PLTE chunk");

   png_ptr->mode |= PNG_HAVE_PLTE;

   if (!(png_ptr->color_type&PNG_COLOR_MASK_COLOR))
   {
      png_warning(png_ptr,
        "Ignoring PLTE chunk in grayscale PNG");
      png_crc_finish(png_ptr, length);
      return;
   }
#if !defined(PNG_READ_OPT_PLTE_SUPPORTED)
   if (png_ptr->color_type != PNG_COLOR_TYPE_PALETTE)
   {
      png_crc_finish(png_ptr, length);
      return;
   }
#endif

   if (length > 3*PNG_MAX_PALETTE_LENGTH || length % 3)
   {
      if (png_ptr->color_type != PNG_COLOR_TYPE_PALETTE)
      {
         png_warning(png_ptr, "Invalid palette chunk");
         png_crc_finish(png_ptr, length);
         return;
      }
      else
      {
         png_error(png_ptr, "Invalid palette chunk");
      }
   }

   num = (int)length / 3;

#ifndef PNG_NO_POINTER_INDEXING
   for (i = 0, pal_ptr = palette; i < num; i++, pal_ptr++)
   {
      uint8_t buf[3];

      png_crc_read(png_ptr, buf, 3);
      pal_ptr->red = buf[0];
      pal_ptr->green = buf[1];
      pal_ptr->blue = buf[2];
   }
#else
   for (i = 0; i < num; i++)
   {
      uint8_t buf[3];

      png_crc_read(png_ptr, buf, 3);
      /* don't depend upon png_color being any order */
      palette[i].red = buf[0];
      palette[i].green = buf[1];
      palette[i].blue = buf[2];
   }
#endif

   /* If we actually NEED the PLTE chunk (ie for a paletted image), we do
      whatever the normal CRC configuration tells us.  However, if we
      have an RGB image, the PLTE can be considered ancillary, so
      we will act as though it is. */
#if !defined(PNG_READ_OPT_PLTE_SUPPORTED)
   if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
#endif
   {
      png_crc_finish(png_ptr, 0);
   }
#if !defined(PNG_READ_OPT_PLTE_SUPPORTED)
   else if (png_crc_error(png_ptr))  /* Only if we have a CRC error */
   {
      /* If we don't want to use the data from an ancillary chunk,
         we have two options: an error abort, or a warning and we
         ignore the data in this chunk (which should be OK, since
         it's considered ancillary for a RGB or RGBA image). */
      if (!(png_ptr->flags & PNG_FLAG_CRC_ANCILLARY_USE))
      {
         if (png_ptr->flags & PNG_FLAG_CRC_ANCILLARY_NOWARN)
         {
            png_chunk_error(png_ptr, "CRC error");
         }
         else
         {
            png_chunk_warning(png_ptr, "CRC error");
            return;
         }
      }
      /* Otherwise, we (optionally) emit a warning and use the chunk. */
      else if (!(png_ptr->flags & PNG_FLAG_CRC_ANCILLARY_NOWARN))
      {
         png_chunk_warning(png_ptr, "CRC error");
      }
   }
#endif

   png_set_PLTE(png_ptr, info_ptr, palette, num);

#if defined(PNG_READ_tRNS_SUPPORTED)
   if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
   {
      if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_tRNS))
      {
         if (png_ptr->num_trans > (uint16_t)num)
         {
            png_warning(png_ptr, "Truncating incorrect tRNS chunk length");
            png_ptr->num_trans = (uint16_t)num;
         }
         if (info_ptr->num_trans > (uint16_t)num)
         {
            png_warning(png_ptr, "Truncating incorrect info tRNS chunk length");
            info_ptr->num_trans = (uint16_t)num;
         }
      }
   }
#endif

}

void png_handle_IEND(png_structp png_ptr, png_info* info_ptr, uint32_t length)
{
   png_debug(1, "in png_handle_IEND\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR) || !(png_ptr->mode & PNG_HAVE_IDAT))
   {
      png_error(png_ptr, "No image in file");
   }

   png_ptr->mode |= (PNG_AFTER_IDAT | PNG_HAVE_IEND);

   if (length != 0)
   {
      png_warning(png_ptr, "Incorrect IEND chunk length");
   }
   png_crc_finish(png_ptr, length);

   if (&info_ptr == NULL) /* quiet compiler warnings about unused info_ptr */
      return;
}


#if defined(PNG_READ_sBIT_SUPPORTED)
void png_handle_sBIT(png_structp png_ptr, png_info* info_ptr, uint32_t length)
{
   size_t truelen;
   uint8_t buf[4];

   png_debug(1, "in png_handle_sBIT\n");

   buf[0] = buf[1] = buf[2] = buf[3] = 0;

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before sBIT");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid sBIT after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->mode & PNG_HAVE_PLTE)
   {
      /* Should be an error, but we can cope with it */
      png_warning(png_ptr, "Out of place sBIT chunk");
   }
   if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_sBIT))
   {
      png_warning(png_ptr, "Duplicate sBIT chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
      truelen = 3;
   else
      truelen = (size_t)png_ptr->channels;

   if (length != truelen || length > 4)
   {
      png_warning(png_ptr, "Incorrect sBIT chunk length");
      png_crc_finish(png_ptr, length);
      return;
   }

   png_crc_read(png_ptr, buf, truelen);
   if (png_crc_finish(png_ptr, 0))
      return;

   if (png_ptr->color_type & PNG_COLOR_MASK_COLOR)
   {
      png_ptr->sig_bit.red = buf[0];
      png_ptr->sig_bit.green = buf[1];
      png_ptr->sig_bit.blue = buf[2];
      png_ptr->sig_bit.alpha = buf[3];
   }
   else
   {
      png_ptr->sig_bit.gray = buf[0];
      png_ptr->sig_bit.red = buf[0];
      png_ptr->sig_bit.green = buf[0];
      png_ptr->sig_bit.blue = buf[0];
      png_ptr->sig_bit.alpha = buf[1];
   }
   png_set_sBIT(png_ptr, info_ptr, &(png_ptr->sig_bit));
}
#endif

#if defined(PNG_READ_sRGB_SUPPORTED)
void png_handle_sRGB(png_structp png_ptr, png_info* info_ptr, uint32_t length)
{
   int intent;
   uint8_t buf[1];

   png_debug(1, "in png_handle_sRGB\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before sRGB");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid sRGB after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->mode & PNG_HAVE_PLTE)
      /* Should be an error, but we can cope with it */
      png_warning(png_ptr, "Out of place sRGB chunk");

   if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_sRGB))
   {
      png_warning(png_ptr, "Duplicate sRGB chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (length != 1)
   {
      png_warning(png_ptr, "Incorrect sRGB chunk length");
      png_crc_finish(png_ptr, length);
      return;
   }

   png_crc_read(png_ptr, buf, 1);
   if (png_crc_finish(png_ptr, 0))
      return;

   intent = buf[0];
   /* check for bad intent */
   if (intent >= PNG_sRGB_INTENT_LAST)
   {
      png_warning(png_ptr, "Unknown sRGB intent");
      return;
   }
   png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, intent);
}
#endif /* PNG_READ_sRGB_SUPPORTED */


#if defined(PNG_READ_sPLT_SUPPORTED)
void png_handle_sPLT(png_structp png_ptr, png_info* info_ptr, uint32_t length)
/* Note: this does not properly handle chunks that are > 64K under DOS */
{
   uint8_t* chunkdata;
   uint8_t* entry_start;
   png_sPLT_t new_palette;
#ifdef PNG_NO_POINTER_INDEXING
   png_sPLT_entryp pp;
#endif
   int data_length, entry_size, i;
   uint32_t skip = 0;
   size_t slength;

   png_debug(1, "in png_handle_sPLT\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before sPLT");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid sPLT after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }

#ifdef PNG_MAX_MALLOC_64K
   if (length > (uint32_t)65535L)
   {
      png_warning(png_ptr, "sPLT chunk too large to fit in memory");
      skip = length - (uint32_t)65535L;
      length = (uint32_t)65535L;
   }
#endif

   chunkdata = (uint8_t*)png_malloc(png_ptr, length + 1);
   slength = (size_t)length;
   png_crc_read(png_ptr, (uint8_t*)chunkdata, slength);

   if (png_crc_finish(png_ptr, skip))
   {
      png_free(png_ptr, chunkdata);
      return;
   }

   chunkdata[slength] = 0x00;

   for (entry_start = chunkdata; *entry_start; entry_start++)
      /* empty loop to find end of name */ ;
   ++entry_start;

   /* a sample depth should follow the separator, and we should be on it  */
   if (entry_start > chunkdata + slength)
   {
      png_free(png_ptr, chunkdata);
      png_warning(png_ptr, "malformed sPLT chunk");
      return;
   }

   new_palette.depth = *entry_start++;
   entry_size = (new_palette.depth == 8 ? 6 : 10);
   data_length = (uint32_t)(slength - (entry_start - chunkdata));

   /* integrity-check the data length */
   if (data_length % entry_size)
   {
      png_free(png_ptr, chunkdata);
      png_warning(png_ptr, "sPLT chunk has bad length");
      return;
   }

   new_palette.nentries = (int32_t) ( data_length / entry_size);
   if ((uint32_t) new_palette.nentries > (uint32_t) (PNG_SIZE_MAX /
       sizeof(png_sPLT_entry)))
   {
       png_warning(png_ptr, "sPLT chunk too long");
       return;
   }
   new_palette.entries = (png_sPLT_entryp)png_malloc_warn(
       png_ptr, new_palette.nentries * sizeof(png_sPLT_entry));
   if (new_palette.entries == NULL)
   {
       png_warning(png_ptr, "sPLT chunk requires too much memory");
       return;
   }

#ifndef PNG_NO_POINTER_INDEXING
   for (i = 0; i < new_palette.nentries; i++)
   {
      png_sPLT_entryp pp = new_palette.entries + i;

      if (new_palette.depth == 8)
      {
          pp->red = *entry_start++;
          pp->green = *entry_start++;
          pp->blue = *entry_start++;
          pp->alpha = *entry_start++;
      }
      else
      {
          pp->red   = png_get_uint_16(entry_start); entry_start += 2;
          pp->green = png_get_uint_16(entry_start); entry_start += 2;
          pp->blue  = png_get_uint_16(entry_start); entry_start += 2;
          pp->alpha = png_get_uint_16(entry_start); entry_start += 2;
      }
      pp->frequency = png_get_uint_16(entry_start); entry_start += 2;
   }
#else
   pp = new_palette.entries;
   for (i = 0; i < new_palette.nentries; i++)
   {

      if (new_palette.depth == 8)
      {
          pp[i].red   = *entry_start++;
          pp[i].green = *entry_start++;
          pp[i].blue  = *entry_start++;
          pp[i].alpha = *entry_start++;
      }
      else
      {
          pp[i].red   = png_get_uint_16(entry_start); entry_start += 2;
          pp[i].green = png_get_uint_16(entry_start); entry_start += 2;
          pp[i].blue  = png_get_uint_16(entry_start); entry_start += 2;
          pp[i].alpha = png_get_uint_16(entry_start); entry_start += 2;
      }
      pp->frequency = png_get_uint_16(entry_start); entry_start += 2;
   }
#endif

   /* discard all chunk data except the name and stash that */
   new_palette.name = (char*)chunkdata;

   png_set_sPLT(png_ptr, info_ptr, &new_palette, 1);

   png_free(png_ptr, chunkdata);
   png_free(png_ptr, new_palette.entries);
}
#endif /* PNG_READ_sPLT_SUPPORTED */

#if defined(PNG_READ_tRNS_SUPPORTED)
void png_handle_tRNS(png_structp png_ptr, png_info* info_ptr, uint32_t length)
{
   uint8_t readbuf[PNG_MAX_PALETTE_LENGTH];

   png_debug(1, "in png_handle_tRNS\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before tRNS");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid tRNS after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_tRNS))
   {
      png_warning(png_ptr, "Duplicate tRNS chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_ptr->color_type == PNG_COLOR_TYPE_GRAY)
   {
      uint8_t buf[2];

      if (length != 2)
      {
         png_warning(png_ptr, "Incorrect tRNS chunk length");
         png_crc_finish(png_ptr, length);
         return;
      }

      png_crc_read(png_ptr, buf, 2);
      png_ptr->num_trans = 1;
      png_ptr->trans_values.gray = png_get_uint_16(buf);
   }
   else if (png_ptr->color_type == PNG_COLOR_TYPE_RGB)
   {
      uint8_t buf[6];

      if (length != 6)
      {
         png_warning(png_ptr, "Incorrect tRNS chunk length");
         png_crc_finish(png_ptr, length);
         return;
      }
      png_crc_read(png_ptr, buf, (size_t)length);
      png_ptr->num_trans = 1;
      png_ptr->trans_values.red = png_get_uint_16(buf);
      png_ptr->trans_values.green = png_get_uint_16(buf + 2);
      png_ptr->trans_values.blue = png_get_uint_16(buf + 4);
   }
   else if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
   {
      if (!(png_ptr->mode & PNG_HAVE_PLTE))
      {
         /* Should be an error, but we can cope with it. */
         png_warning(png_ptr, "Missing PLTE before tRNS");
      }
      if (length > (uint32_t)png_ptr->num_palette ||
          length > PNG_MAX_PALETTE_LENGTH)
      {
         png_warning(png_ptr, "Incorrect tRNS chunk length");
         png_crc_finish(png_ptr, length);
         return;
      }
      if (length == 0)
      {
         png_warning(png_ptr, "Zero length tRNS chunk");
         png_crc_finish(png_ptr, length);
         return;
      }
      png_crc_read(png_ptr, readbuf, (size_t)length);
      png_ptr->num_trans = (uint16_t)length;
   }
   else
   {
      png_warning(png_ptr, "tRNS chunk not allowed with alpha channel");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (png_crc_finish(png_ptr, 0))
      return;

   png_set_tRNS(png_ptr, info_ptr, readbuf, png_ptr->num_trans,
      &(png_ptr->trans_values));
}
#endif

#if defined(PNG_READ_hIST_SUPPORTED)
void png_handle_hIST(png_structp png_ptr, png_info* info_ptr, uint32_t length)
{
   unsigned int num, i;
   uint16_t readbuf[PNG_MAX_PALETTE_LENGTH];

   png_debug(1, "in png_handle_hIST\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before hIST");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid hIST after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (!(png_ptr->mode & PNG_HAVE_PLTE))
   {
      png_warning(png_ptr, "Missing PLTE before hIST");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_hIST))
   {
      png_warning(png_ptr, "Duplicate hIST chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   num = length / 2 ;
   if (num != (unsigned int) png_ptr->num_palette || num >
      (unsigned int) PNG_MAX_PALETTE_LENGTH)
   {
      png_warning(png_ptr, "Incorrect hIST chunk length");
      png_crc_finish(png_ptr, length);
      return;
   }

   for (i = 0; i < num; i++)
   {
      uint8_t buf[2];

      png_crc_read(png_ptr, buf, 2);
      readbuf[i] = png_get_uint_16(buf);
   }

   if (png_crc_finish(png_ptr, 0))
      return;

   png_set_hIST(png_ptr, info_ptr, readbuf);
}
#endif

#if defined(PNG_READ_pHYs_SUPPORTED)
void png_handle_pHYs(png_structp png_ptr, png_info* info_ptr, uint32_t length)
{
   uint8_t buf[9];
   uint32_t res_x, res_y;
   int unit_type;

   png_debug(1, "in png_handle_pHYs\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before pHYs");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid pHYs after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_pHYs))
   {
      png_warning(png_ptr, "Duplicate pHYs chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   if (length != 9)
   {
      png_warning(png_ptr, "Incorrect pHYs chunk length");
      png_crc_finish(png_ptr, length);
      return;
   }

   png_crc_read(png_ptr, buf, 9);
   if (png_crc_finish(png_ptr, 0))
      return;

   res_x = png_get_uint_32(buf);
   res_y = png_get_uint_32(buf + 4);
   unit_type = buf[8];
   png_set_pHYs(png_ptr, info_ptr, res_x, res_y, unit_type);
}
#endif


#if defined(PNG_READ_pCAL_SUPPORTED)
/* read the pCAL chunk (described in the PNG Extensions document) */
void png_handle_pCAL(png_structp png_ptr, png_info* info_ptr, uint32_t length)
{
   char* purpose;
   int32_t X0, X1;
   uint8_t type, nparams;
   char* buf;
   char* units;
   char* endptr;
   png_charpp params;
   size_t slength;
   int i;

   png_debug(1, "in png_handle_pCAL\n");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before pCAL");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid pCAL after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_pCAL))
   {
      png_warning(png_ptr, "Duplicate pCAL chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

   png_debug1(2, "Allocating and reading pCAL chunk data (%lu bytes)\n",
      length + 1);
   purpose = (char*)png_malloc_warn(png_ptr, length + 1);
   if (purpose == NULL)
     {
       png_warning(png_ptr, "No memory for pCAL purpose.");
       return;
     }
   slength = (size_t)length;
   png_crc_read(png_ptr, (uint8_t*)purpose, slength);

   if (png_crc_finish(png_ptr, 0))
   {
      png_free(png_ptr, purpose);
      return;
   }

   purpose[slength] = 0x00; /* null terminate the last string */

   png_debug(3, "Finding end of pCAL purpose string\n");
   for (buf = purpose; *buf; buf++)
      /* empty loop */ ;

   endptr = purpose + slength;

   /* We need to have at least 12 bytes after the purpose string
      in order to get the parameter information. */
   if (endptr <= buf + 12)
   {
      png_warning(png_ptr, "Invalid pCAL data");
      png_free(png_ptr, purpose);
      return;
   }

   png_debug(3, "Reading pCAL X0, X1, type, nparams, and units\n");
   X0 = png_get_int_32((uint8_t*)buf+1);
   X1 = png_get_int_32((uint8_t*)buf+5);
   type = buf[9];
   nparams = buf[10];
   units = buf + 11;

   png_debug(3, "Checking pCAL equation type and number of parameters\n");
   /* Check that we have the right number of parameters for known
      equation types. */
   if ((type == PNG_EQUATION_LINEAR && nparams != 2) ||
       (type == PNG_EQUATION_BASE_E && nparams != 3) ||
       (type == PNG_EQUATION_ARBITRARY && nparams != 3) ||
       (type == PNG_EQUATION_HYPERBOLIC && nparams != 4))
   {
      png_warning(png_ptr, "Invalid pCAL parameters for equation type");
      png_free(png_ptr, purpose);
      return;
   }
   else if (type >= PNG_EQUATION_LAST)
   {
      png_warning(png_ptr, "Unrecognized equation type for pCAL chunk");
   }

   for (buf = units; *buf; buf++)
      /* Empty loop to move past the units string. */ ;

   png_debug(3, "Allocating pCAL parameters array\n");
   params = (png_charpp)png_malloc_warn(png_ptr, (uint32_t)(nparams
      *sizeof(char*))) ;
   if (params == NULL)
     {
       png_free(png_ptr, purpose);
       png_warning(png_ptr, "No memory for pCAL params.");
       return;
     }

   /* Get pointers to the start of each parameter string. */
   for (i = 0; i < (int)nparams; i++)
   {
      buf++; /* Skip the null string terminator from previous parameter. */

      png_debug1(3, "Reading pCAL parameter %d\n", i);
      for (params[i] = buf; *buf != 0x00 && buf <= endptr; buf++)
         /* Empty loop to move past each parameter string */ ;

      /* Make sure we haven't run out of data yet */
      if (buf > endptr)
      {
         png_warning(png_ptr, "Invalid pCAL data");
         png_free(png_ptr, purpose);
         png_free(png_ptr, params);
         return;
      }
   }

   png_set_pCAL(png_ptr, info_ptr, purpose, X0, X1, type, nparams,
      units, params);

   png_free(png_ptr, purpose);
   png_free(png_ptr, params);
}
#endif

/* This function is called when we haven't found a handler for a
   chunk.  If there isn't a problem with the chunk itself (ie bad
   chunk name, CRC, or a critical chunk), the chunk is silently ignored
   -- unless the PNG_FLAG_UNKNOWN_CHUNKS_SUPPORTED flag is on in which
   case it will be saved away to be written out later. */
void png_handle_unknown(png_structp png_ptr, png_info* info_ptr, uint32_t length)
{
   uint32_t skip = 0;

   png_debug(1, "in png_handle_unknown\n");

   if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      if (memcmp(png_ptr->chunk_name, png_IDAT, 4))  /* not an IDAT */
         png_ptr->mode |= PNG_AFTER_IDAT;
   }

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
   }

#if defined(PNG_READ_UNKNOWN_CHUNKS_SUPPORTED)
   if ((png_ptr->flags & PNG_FLAG_KEEP_UNKNOWN_CHUNKS) ||
       (png_ptr->read_user_chunk_fn != NULL))
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
       chunk.size = (size_t)length;
       png_crc_read(png_ptr, (uint8_t*)chunk.data, length);
#if defined(PNG_READ_USER_CHUNKS_SUPPORTED)
       if(png_ptr->read_user_chunk_fn != NULL)
       {
          /* callback to user unknown chunk handler */
          if ((*(png_ptr->read_user_chunk_fn)) (png_ptr, &chunk) <= 0)
          {
             if (!(png_ptr->chunk_name[0] & 0x20))
                if(png_handle_as_unknown(png_ptr, png_ptr->chunk_name) !=
                     PNG_HANDLE_CHUNK_ALWAYS)
                 {
                   png_free(png_ptr, chunk.data);
                   png_chunk_error(png_ptr, "unknown critical chunk");
                 }
             png_set_unknown_chunks(png_ptr, info_ptr, &chunk, 1);
          }
       }
       else
#endif
          png_set_unknown_chunks(png_ptr, info_ptr, &chunk, 1);
       png_free(png_ptr, chunk.data);
   }
   else
#endif
      skip = length;

   png_crc_finish(png_ptr, skip);

#if !defined(PNG_READ_USER_CHUNKS_SUPPORTED)
   if (&info_ptr == NULL) /* quiet compiler warnings about unused info_ptr */
      return;
#endif
}

/* This function is called to verify that a chunk name is valid.
   This function can't have the "critical chunk check" incorporated
   into it, since in the future we will need to be able to call user
   functions to handle unknown critical chunks after we check that
   the chunk name itself is valid. */

#define isnonalpha(c) ((c) < 65 || (c) > 122 || ((c) > 90 && (c) < 97))

void png_check_chunk_name(png_structp png_ptr, uint8_t* chunk_name)
{
   png_debug(1, "in png_check_chunk_name\n");
   if (isnonalpha(chunk_name[0]) || isnonalpha(chunk_name[1]) ||
       isnonalpha(chunk_name[2]) || isnonalpha(chunk_name[3]))
   {
      png_chunk_error(png_ptr, "invalid chunk type");
   }
}

/* Combines the row recently read in with the existing pixels in the
   row.  This routine takes care of alpha and transparency if requested.
   This routine also handles the two methods of progressive display
   of interlaced images, depending on the mask value.
   The mask value describes which pixels are to be combined with
   the row.  The pattern always repeats every 8 pixels, so just 8
   bits are needed.  A one indicates the pixel is to be combined,
   a zero indicates the pixel is to be skipped.  This is in addition
   to any alpha or transparency value associated with the pixel.  If
   you want all pixels to be combined, pass 0xff (255) in mask.  */

void png_combine_row(png_structp png_ptr, uint8_t* row, int mask)
{
   png_debug(1,"in png_combine_row\n");
   if (mask == 0xff)
   {
      memcpy(row, png_ptr->row_buf + 1,
         PNG_ROWBYTES(png_ptr->row_info.pixel_depth, png_ptr->width));
   }
   else
   {
      switch (png_ptr->row_info.pixel_depth)
      {
         case 1:
         {
            uint8_t* sp = png_ptr->row_buf + 1;
            uint8_t* dp = row;
            int s_inc, s_start, s_end;
            int m = 0x80;
            int shift;
            uint32_t i;
            uint32_t row_width = png_ptr->width;

            s_start = 7;
            s_end = 0;
            s_inc = -1;

            shift = s_start;

            for (i = 0; i < row_width; i++)
            {
               if (m & mask)
               {
                  int value;

                  value = (*sp >> shift) & 0x01;
                  *dp &= (uint8_t)((0x7f7f >> (7 - shift)) & 0xff);
                  *dp |= (uint8_t)(value << shift);
               }

               if (shift == s_end)
               {
                  shift = s_start;
                  sp++;
                  dp++;
               }
               else
                  shift += s_inc;

               if (m == 1)
                  m = 0x80;
               else
                  m >>= 1;
            }
            break;
         }
         case 2:
         {
            uint8_t* sp = png_ptr->row_buf + 1;
            uint8_t* dp = row;
            int s_start, s_end, s_inc;
            int m = 0x80;
            int shift;
            uint32_t i;
            uint32_t row_width = png_ptr->width;
            int value;

            s_start = 6;
            s_end = 0;
            s_inc = -2;

            shift = s_start;

            for (i = 0; i < row_width; i++)
            {
               if (m & mask)
               {
                  value = (*sp >> shift) & 0x03;
                  *dp &= (uint8_t)((0x3f3f >> (6 - shift)) & 0xff);
                  *dp |= (uint8_t)(value << shift);
               }

               if (shift == s_end)
               {
                  shift = s_start;
                  sp++;
                  dp++;
               }
               else
                  shift += s_inc;
               if (m == 1)
                  m = 0x80;
               else
                  m >>= 1;
            }
            break;
         }
         case 4:
         {
            uint8_t* sp = png_ptr->row_buf + 1;
            uint8_t* dp = row;
            int s_start, s_end, s_inc;
            int m = 0x80;
            int shift;
            uint32_t i;
            uint32_t row_width = png_ptr->width;
            int value;

            s_start = 4;
            s_end = 0;
            s_inc = -4;
            shift = s_start;

            for (i = 0; i < row_width; i++)
            {
               if (m & mask)
               {
                  value = (*sp >> shift) & 0xf;
                  *dp &= (uint8_t)((0xf0f >> (4 - shift)) & 0xff);
                  *dp |= (uint8_t)(value << shift);
               }

               if (shift == s_end)
               {
                  shift = s_start;
                  sp++;
                  dp++;
               }
               else
                  shift += s_inc;
               if (m == 1)
                  m = 0x80;
               else
                  m >>= 1;
            }
            break;
         }
         default:
         {
            uint8_t* sp = png_ptr->row_buf + 1;
            uint8_t* dp = row;
            size_t pixel_bytes = (png_ptr->row_info.pixel_depth >> 3);
            uint32_t i;
            uint32_t row_width = png_ptr->width;
            uint8_t m = 0x80;


            for (i = 0; i < row_width; i++)
            {
               if (m & mask)
               {
                  memcpy(dp, sp, pixel_bytes);
               }

               sp += pixel_bytes;
               dp += pixel_bytes;

               if (m == 1)
                  m = 0x80;
               else
                  m >>= 1;
            }
            break;
         }
      }
   }
}

void png_read_filter_row(png_structp png_ptr, png_row_infop row_info, uint8_t* row,
   uint8_t* prev_row, int filter)
{
   png_debug(1, "in png_read_filter_row\n");
   png_debug2(2,"row = %lu, filter = %d\n", png_ptr->row_number, filter);
   switch (filter)
   {
      case PNG_FILTER_VALUE_NONE:
         break;
      case PNG_FILTER_VALUE_SUB:
      {
         uint32_t i;
         uint32_t istop = row_info->rowbytes;
         uint32_t bpp = (row_info->pixel_depth + 7) >> 3;
         uint8_t* rp = row + bpp;
         uint8_t* lp = row;

         for (i = bpp; i < istop; i++)
         {
            *rp = (uint8_t)(((int)(*rp) + (int)(*lp++)) & 0xff);
            rp++;
         }
         break;
      }
      case PNG_FILTER_VALUE_UP:
      {
         uint32_t i;
         uint32_t istop = row_info->rowbytes;
         uint8_t* rp = row;
         uint8_t* pp = prev_row;

         for (i = 0; i < istop; i++)
         {
            *rp = (uint8_t)(((int)(*rp) + (int)(*pp++)) & 0xff);
            rp++;
         }
         break;
      }
      case PNG_FILTER_VALUE_AVG:
      {
         uint32_t i;
         uint8_t* rp = row;
         uint8_t* pp = prev_row;
         uint8_t* lp = row;
         uint32_t bpp = (row_info->pixel_depth + 7) >> 3;
         uint32_t istop = row_info->rowbytes - bpp;

         for (i = 0; i < bpp; i++)
         {
            *rp = (uint8_t)(((int)(*rp) +
               ((int)(*pp++) / 2 )) & 0xff);
            rp++;
         }

         for (i = 0; i < istop; i++)
         {
            *rp = (uint8_t)(((int)(*rp) +
               (int)(*pp++ + *lp++) / 2 ) & 0xff);
            rp++;
         }
         break;
      }
      case PNG_FILTER_VALUE_PAETH:
      {
         uint32_t i;
         uint8_t* rp = row;
         uint8_t* pp = prev_row;
         uint8_t* lp = row;
         uint8_t* cp = prev_row;
         uint32_t bpp = (row_info->pixel_depth + 7) >> 3;
         uint32_t istop=row_info->rowbytes - bpp;

         for (i = 0; i < bpp; i++)
         {
            *rp = (uint8_t)(((int)(*rp) + (int)(*pp++)) & 0xff);
            rp++;
         }

         for (i = 0; i < istop; i++)   /* use leftover rp,pp */
         {
            int a, b, c, pa, pb, pc, p;

            a = *lp++;
            b = *pp++;
            c = *cp++;

            p = b - c;
            pc = a - c;

#ifdef PNG_USE_ABS
            pa = abs(p);
            pb = abs(pc);
            pc = abs(p + pc);
#else
            pa = p < 0 ? -p : p;
            pb = pc < 0 ? -pc : pc;
            pc = (p + pc) < 0 ? -(p + pc) : p + pc;
#endif

            /*
               if (pa <= pb && pa <= pc)
                  p = a;
               else if (pb <= pc)
                  p = b;
               else
                  p = c;
             */

            p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;

            *rp = (uint8_t)(((int)(*rp) + p) & 0xff);
            rp++;
         }
         break;
      }
      default:
         png_warning(png_ptr, "Ignoring bad adaptive filter type");
         *row=0;
         break;
   }
}

void png_read_finish_row(png_structp png_ptr)
{
   png_debug(1, "in png_read_finish_row\n");
   png_ptr->row_number++;
   if (png_ptr->row_number < png_ptr->num_rows)
      return;

   if (png_ptr->interlaced)
   {
      png_ptr->row_number = 0;
      png_memset_check(png_ptr, png_ptr->prev_row, 0, png_ptr->rowbytes + 1);
      do
      {
         png_ptr->pass++;
         if (png_ptr->pass >= 7)
            break;
         png_ptr->iwidth = (png_ptr->width +
            png_pass_inc[png_ptr->pass] - 1 -
            png_pass_start[png_ptr->pass]) /
            png_pass_inc[png_ptr->pass];

         png_ptr->irowbytes = PNG_ROWBYTES(png_ptr->pixel_depth,
            png_ptr->iwidth) + 1;

         if (!(png_ptr->transformations & PNG_INTERLACE))
         {
            png_ptr->num_rows = (png_ptr->height +
               png_pass_yinc[png_ptr->pass] - 1 -
               png_pass_ystart[png_ptr->pass]) /
               png_pass_yinc[png_ptr->pass];
            if (!(png_ptr->num_rows))
               continue;
         }
         else  /* if (png_ptr->transformations & PNG_INTERLACE) */
            break;
      } while (png_ptr->iwidth == 0);

      if (png_ptr->pass < 7)
         return;
   }

   if (!(png_ptr->flags & PNG_FLAG_ZLIB_FINISHED))
   {
      char extra;
      int ret;

      png_ptr->zstream.next_out = (uint8_t *)&extra;
      png_ptr->zstream.avail_out = (uint32_t)1;
      for(;;)
      {
         if (!(png_ptr->zstream.avail_in))
         {
            while (!png_ptr->idat_size)
            {
               uint8_t chunk_length[4];

               png_crc_finish(png_ptr, 0);

               png_read_data(png_ptr, chunk_length, 4);
               png_ptr->idat_size = png_get_uint_31(png_ptr, chunk_length);
               png_reset_crc(png_ptr);
               png_crc_read(png_ptr, png_ptr->chunk_name, 4);
               if (memcmp(png_ptr->chunk_name, (uint8_t*)png_IDAT, 4))
                  png_error(png_ptr, "Not enough image data");

            }
            png_ptr->zstream.avail_in = (uint32_t)png_ptr->zbuf_size;
            png_ptr->zstream.next_in = png_ptr->zbuf;
            if (png_ptr->zbuf_size > png_ptr->idat_size)
               png_ptr->zstream.avail_in = (uint32_t)png_ptr->idat_size;
            png_crc_read(png_ptr, png_ptr->zbuf, png_ptr->zstream.avail_in);
            png_ptr->idat_size -= png_ptr->zstream.avail_in;
         }
         ret = inflate(&png_ptr->zstream, Z_PARTIAL_FLUSH);
         if (ret == Z_STREAM_END)
         {
            if (!(png_ptr->zstream.avail_out) || png_ptr->zstream.avail_in ||
               png_ptr->idat_size)
               png_warning(png_ptr, "Extra compressed data");
            png_ptr->mode |= PNG_AFTER_IDAT;
            png_ptr->flags |= PNG_FLAG_ZLIB_FINISHED;
            break;
         }
         if (ret != Z_OK)
            png_error(png_ptr, png_ptr->zstream.msg ? png_ptr->zstream.msg :
                      "Decompression Error");

         if (!(png_ptr->zstream.avail_out))
         {
            png_warning(png_ptr, "Extra compressed data.");
            png_ptr->mode |= PNG_AFTER_IDAT;
            png_ptr->flags |= PNG_FLAG_ZLIB_FINISHED;
            break;
         }

      }
      png_ptr->zstream.avail_out = 0;
   }

   if (png_ptr->idat_size || png_ptr->zstream.avail_in)
      png_warning(png_ptr, "Extra compression data");

   inflateReset(&png_ptr->zstream);

   png_ptr->mode |= PNG_AFTER_IDAT;
}

void png_read_start_row(png_structp png_ptr)
{
   int max_pixel_depth;
   uint32_t row_bytes;

   png_debug(1, "in png_read_start_row\n");
   png_ptr->zstream.avail_in = 0;
   png_init_read_transformations(png_ptr);
   if (png_ptr->interlaced)
   {
      if (!(png_ptr->transformations & PNG_INTERLACE))
         png_ptr->num_rows = (png_ptr->height + png_pass_yinc[0] - 1 -
            png_pass_ystart[0]) / png_pass_yinc[0];
      else
         png_ptr->num_rows = png_ptr->height;

      png_ptr->iwidth = (png_ptr->width +
         png_pass_inc[png_ptr->pass] - 1 -
         png_pass_start[png_ptr->pass]) /
         png_pass_inc[png_ptr->pass];

         row_bytes = PNG_ROWBYTES(png_ptr->pixel_depth,png_ptr->iwidth) + 1;

         png_ptr->irowbytes = row_bytes;
         if((uint32_t)png_ptr->irowbytes != row_bytes)
            png_error(png_ptr, "Rowbytes overflow in png_read_start_row");
   }
   else
   {
      png_ptr->num_rows = png_ptr->height;
      png_ptr->iwidth = png_ptr->width;
      png_ptr->irowbytes = png_ptr->rowbytes + 1;
   }
   max_pixel_depth = png_ptr->pixel_depth;

#if defined(PNG_READ_PACK_SUPPORTED)
   if ((png_ptr->transformations & PNG_PACK) && png_ptr->bit_depth < 8)
      max_pixel_depth = 8;
#endif

#if defined(PNG_READ_EXPAND_SUPPORTED)
   if (png_ptr->transformations & PNG_EXPAND)
   {
      if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
      {
         if (png_ptr->num_trans)
            max_pixel_depth = 32;
         else
            max_pixel_depth = 24;
      }
      else if (png_ptr->color_type == PNG_COLOR_TYPE_GRAY)
      {
         if (max_pixel_depth < 8)
            max_pixel_depth = 8;
         if (png_ptr->num_trans)
            max_pixel_depth *= 2;
      }
      else if (png_ptr->color_type == PNG_COLOR_TYPE_RGB)
      {
         if (png_ptr->num_trans)
         {
            max_pixel_depth *= 4;
            max_pixel_depth /= 3;
         }
      }
   }
#endif

#if defined(PNG_READ_FILLER_SUPPORTED)
   if (png_ptr->transformations & (PNG_FILLER))
   {
      if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
         max_pixel_depth = 32;
      else if (png_ptr->color_type == PNG_COLOR_TYPE_GRAY)
      {
         if (max_pixel_depth <= 8)
            max_pixel_depth = 16;
         else
            max_pixel_depth = 32;
      }
      else if (png_ptr->color_type == PNG_COLOR_TYPE_RGB)
      {
         if (max_pixel_depth <= 32)
            max_pixel_depth = 32;
         else
            max_pixel_depth = 64;
      }
   }
#endif

#if defined(PNG_READ_GRAY_TO_RGB_SUPPORTED)
   if (png_ptr->transformations & PNG_GRAY_TO_RGB)
   {
      if (
#if defined(PNG_READ_EXPAND_SUPPORTED)
        (png_ptr->num_trans && (png_ptr->transformations & PNG_EXPAND)) ||
#endif
#if defined(PNG_READ_FILLER_SUPPORTED)
        (png_ptr->transformations & (PNG_FILLER)) ||
#endif
        png_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      {
         if (max_pixel_depth <= 16)
            max_pixel_depth = 32;
         else
            max_pixel_depth = 64;
      }
      else
      {
         if (max_pixel_depth <= 8)
           {
             if (png_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
               max_pixel_depth = 32;
             else
               max_pixel_depth = 24;
           }
         else if (png_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
            max_pixel_depth = 64;
         else
            max_pixel_depth = 48;
      }
   }
#endif

#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED) && \
defined(PNG_USER_TRANSFORM_PTR_SUPPORTED)
   if(png_ptr->transformations & PNG_USER_TRANSFORM)
     {
       int user_pixel_depth=png_ptr->user_transform_depth*
         png_ptr->user_transform_channels;
       if(user_pixel_depth > max_pixel_depth)
         max_pixel_depth=user_pixel_depth;
     }
#endif

   /* align the width on the next larger 8 pixels.  Mainly used
      for interlacing */
   row_bytes = ((png_ptr->width + 7) & ~((uint32_t)7));
   /* calculate the maximum bytes needed, adding a byte and a pixel
      for safety's sake */
   row_bytes = PNG_ROWBYTES(max_pixel_depth,row_bytes) +
      1 + ((max_pixel_depth + 7) >> 3);
#ifdef PNG_MAX_MALLOC_64K
   if (row_bytes > (uint32_t)65536L)
      png_error(png_ptr, "This image requires a row greater than 64KB");
#endif
   png_ptr->big_row_buf = (uint8_t*)png_malloc(png_ptr, row_bytes+64);
   png_ptr->row_buf = png_ptr->big_row_buf+32;

#ifdef PNG_MAX_MALLOC_64K
   if ((uint32_t)png_ptr->rowbytes + 1 > (uint32_t)65536L)
      png_error(png_ptr, "This image requires a row greater than 64KB");
#endif
   if ((uint32_t)png_ptr->rowbytes > (uint32_t)(PNG_SIZE_MAX - 1))
      png_error(png_ptr, "Row has too many bytes to allocate in memory.");
   png_ptr->prev_row = (uint8_t*)png_malloc(png_ptr, (uint32_t)(
      png_ptr->rowbytes + 1));

   png_memset_check(png_ptr, png_ptr->prev_row, 0, png_ptr->rowbytes + 1);

   png_debug1(3, "width = %lu,\n", png_ptr->width);
   png_debug1(3, "height = %lu,\n", png_ptr->height);
   png_debug1(3, "iwidth = %lu,\n", png_ptr->iwidth);
   png_debug1(3, "num_rows = %lu\n", png_ptr->num_rows);
   png_debug1(3, "rowbytes = %lu,\n", png_ptr->rowbytes);
   png_debug1(3, "irowbytes = %lu,\n", png_ptr->irowbytes);

   png_ptr->flags |= PNG_FLAG_ROW_INIT;
}
#endif /* PNG_READ_SUPPORTED */
