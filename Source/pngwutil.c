
/* pngwutil.c - utilities to write a PNG file
 *
 * Last changed in libpng 1.2.15 January 5, 2007
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2007 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 */

#include "png.h"
#ifdef PNG_WRITE_SUPPORTED

/* Place a 32-bit number into a buffer in PNG byte order.  We work
 * with unsigned numbers for convenience, although one supported
 * ancillary chunk uses signed (two's complement) numbers.
 */
void
png_save_uint_32(uint8_t* buf, uint32_t i)
{
   buf[0] = (uint8_t)((i >> 24) & 0xff);
   buf[1] = (uint8_t)((i >> 16) & 0xff);
   buf[2] = (uint8_t)((i >> 8) & 0xff);
   buf[3] = (uint8_t)(i & 0xff);
}

/* The png_save_int_32 function assumes integers are stored in two's
 * complement format.  If this isn't the case, then this routine needs to
 * be modified to write data in two's complement format.
 */
void
png_save_int_32(uint8_t* buf, int32_t i)
{
   buf[0] = (uint8_t)((i >> 24) & 0xff);
   buf[1] = (uint8_t)((i >> 16) & 0xff);
   buf[2] = (uint8_t)((i >> 8) & 0xff);
   buf[3] = (uint8_t)(i & 0xff);
}

/* Place a 16-bit number into a buffer in PNG byte order.
 * The parameter is declared unsigned int, not uint16_t,
 * just to avoid potential problems on pre-ANSI C compilers.
 */
void
png_save_uint_16(uint8_t* buf, unsigned int i)
{
   buf[0] = (uint8_t)((i >> 8) & 0xff);
   buf[1] = (uint8_t)(i & 0xff);
}

/* Write a PNG chunk all at once.  The type is an array of ASCII characters
 * representing the chunk name.  The array must be at least 4 bytes in
 * length, and does not need to be null terminated.  To be safe, pass the
 * pre-defined chunk names here, and if you need a new one, define it
 * where the others are defined.  The length is the length of the data.
 * All the data must be present.  If that is not possible, use the
 * png_write_chunk_start(), png_write_chunk_data(), and png_write_chunk_end()
 * functions instead.
 */
void
png_write_chunk(png_structp png_ptr, uint8_t* chunk_name,
   uint8_t* data, size_t length)
{
   if(png_ptr == NULL) return;
   png_write_chunk_start(png_ptr, chunk_name, (uint32_t)length);
   png_write_chunk_data(png_ptr, data, length);
   png_write_chunk_end(png_ptr);
}

/* Write the start of a PNG chunk.  The type is the chunk type.
 * The total_length is the sum of the lengths of all the data you will be
 * passing in png_write_chunk_data().
 */
void
png_write_chunk_start(png_structp png_ptr, uint8_t* chunk_name,
   uint32_t length)
{
   uint8_t buf[4];
   png_debug2(0, "Writing %s chunk (%lu bytes)\n", chunk_name, length);
   if(png_ptr == NULL) return;

   /* write the length */
   png_save_uint_32(buf, length);
   png_write_data(png_ptr, buf, (size_t)4);

   /* write the chunk name */
   png_write_data(png_ptr, chunk_name, (size_t)4);
   /* reset the crc and run it over the chunk name */
   png_reset_crc(png_ptr);
   png_calculate_crc(png_ptr, chunk_name, (size_t)4);
}

/* Write the data of a PNG chunk started with png_write_chunk_start().
 * Note that multiple calls to this function are allowed, and that the
 * sum of the lengths from these calls *must* add up to the total_length
 * given to png_write_chunk_start().
 */
void
png_write_chunk_data(png_structp png_ptr, uint8_t* data, size_t length)
{
   /* write the data, and run the CRC over it */
   if(png_ptr == NULL) return;
   if (data != NULL && length > 0)
   {
      png_calculate_crc(png_ptr, data, length);
      png_write_data(png_ptr, data, length);
   }
}

/* Finish a chunk started with png_write_chunk_start(). */
void
png_write_chunk_end(png_structp png_ptr)
{
   uint8_t buf[4];

   if(png_ptr == NULL) return;

   /* write the crc */
   png_save_uint_32(buf, png_ptr->crc);

   png_write_data(png_ptr, buf, (size_t)4);
}

/* Simple function to write the signature.  If we have already written
 * the magic bytes of the signature, or more likely, the PNG stream is
 * being embedded into another stream and doesn't need its own signature,
 * we should call png_set_sig_bytes() to tell libpng how many of the
 * bytes have already been written.
 */
void png_write_sig(png_structp png_ptr)
{
   uint8_t png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
   /* write the rest of the 8 byte signature */
   png_write_data(png_ptr, &png_signature[png_ptr->sig_bytes],
      (size_t)8 - png_ptr->sig_bytes);
   if(png_ptr->sig_bytes < 3)
      png_ptr->mode |= PNG_HAVE_PNG_SIGNATURE;
}


/* Write the IHDR chunk, and update the png_struct with the necessary
 * information.  Note that the rest of this code depends upon this
 * information being correct.
 */
void png_write_IHDR(png_structp png_ptr, uint32_t width, uint32_t height, int bit_depth, int color_type, int compression_type, int filter_type, int interlace_type)
{
   uint8_t buf[13]; /* buffer to store the IHDR info */

   png_debug(1, "in png_write_IHDR\n");
   /* Check that we have valid input data from the application info */
   switch (color_type)
   {
      case PNG_COLOR_TYPE_GRAY:
         switch (bit_depth)
         {
            case 1:
            case 2:
            case 4:
            case 8:
            case 16: png_ptr->channels = 1; break;
            default: png_error(png_ptr,"Invalid bit depth for grayscale image");
         }
         break;
      case PNG_COLOR_TYPE_RGB:
         if (bit_depth != 8 && bit_depth != 16)
            png_error(png_ptr, "Invalid bit depth for RGB image");
         png_ptr->channels = 3;
         break;
      case PNG_COLOR_TYPE_PALETTE:
         switch (bit_depth)
         {
            case 1:
            case 2:
            case 4:
            case 8: png_ptr->channels = 1; break;
            default: png_error(png_ptr, "Invalid bit depth for paletted image");
         }
         break;
      case PNG_COLOR_TYPE_GRAY_ALPHA:
         if (bit_depth != 8 && bit_depth != 16)
            png_error(png_ptr, "Invalid bit depth for grayscale+alpha image");
         png_ptr->channels = 2;
         break;
      case PNG_COLOR_TYPE_RGB_ALPHA:
         if (bit_depth != 8 && bit_depth != 16)
            png_error(png_ptr, "Invalid bit depth for RGBA image");
         png_ptr->channels = 4;
         break;
      default:
         png_error(png_ptr, "Invalid image color type specified");
   }

   if (compression_type != PNG_COMPRESSION_TYPE_BASE)
   {
      png_warning(png_ptr, "Invalid compression type specified");
      compression_type = PNG_COMPRESSION_TYPE_BASE;
   }

   if (filter_type != PNG_FILTER_TYPE_BASE)
   {
      png_warning(png_ptr, "Invalid filter type specified");
      filter_type = PNG_FILTER_TYPE_BASE;
   }

   interlace_type=PNG_INTERLACE_NONE;

   /* save off the relevent information */
   png_ptr->bit_depth = (uint8_t)bit_depth;
   png_ptr->color_type = (uint8_t)color_type;
   png_ptr->interlaced = (uint8_t)interlace_type;
   png_ptr->compression_type = (uint8_t)compression_type;
   png_ptr->width = width;
   png_ptr->height = height;

   png_ptr->pixel_depth = (uint8_t)(bit_depth * png_ptr->channels);
   png_ptr->rowbytes = PNG_ROWBYTES(png_ptr->pixel_depth, width);
   /* set the usr info, so any transformations can modify it */
   png_ptr->usr_width = png_ptr->width;
   png_ptr->usr_bit_depth = png_ptr->bit_depth;
   png_ptr->usr_channels = png_ptr->channels;

   /* pack the header information into the buffer */
   png_save_uint_32(buf, width);
   png_save_uint_32(buf + 4, height);
   buf[8] = (uint8_t)bit_depth;
   buf[9] = (uint8_t)color_type;
   buf[10] = (uint8_t)compression_type;
   buf[11] = (uint8_t)filter_type;
   buf[12] = (uint8_t)interlace_type;

   /* write the chunk */
   png_write_chunk(png_ptr, (uint8_t*)png_IHDR, buf, (size_t)13);

   /* initialize zlib with PNG info */
   png_ptr->zstream.zalloc = png_zalloc;
   png_ptr->zstream.zfree = png_zfree;
   png_ptr->zstream.opaque = (void *)png_ptr;
   if (!(png_ptr->do_filter))
   {
      if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE ||
         png_ptr->bit_depth < 8)
         png_ptr->do_filter = PNG_FILTER_NONE;
      else
         png_ptr->do_filter = PNG_ALL_FILTERS;
   }
   if (!(png_ptr->flags & PNG_FLAG_ZLIB_CUSTOM_STRATEGY))
   {
      if (png_ptr->do_filter != PNG_FILTER_NONE)
         png_ptr->zlib_strategy = Z_FILTERED;
      else
         png_ptr->zlib_strategy = Z_DEFAULT_STRATEGY;
   }
   if (!(png_ptr->flags & PNG_FLAG_ZLIB_CUSTOM_LEVEL))
      png_ptr->zlib_level = Z_DEFAULT_COMPRESSION;
   if (!(png_ptr->flags & PNG_FLAG_ZLIB_CUSTOM_MEM_LEVEL))
      png_ptr->zlib_mem_level = 8;
   if (!(png_ptr->flags & PNG_FLAG_ZLIB_CUSTOM_WINDOW_BITS))
      png_ptr->zlib_window_bits = 15;
   if (!(png_ptr->flags & PNG_FLAG_ZLIB_CUSTOM_METHOD))
      png_ptr->zlib_method = 8;
   deflateInit2(&png_ptr->zstream, png_ptr->zlib_level,
      png_ptr->zlib_method, png_ptr->zlib_window_bits,
      png_ptr->zlib_mem_level, png_ptr->zlib_strategy);
   png_ptr->zstream.next_out = png_ptr->zbuf;
   png_ptr->zstream.avail_out = (uint32_t)png_ptr->zbuf_size;
   /* libpng is not interested in zstream.data_type */
   /* set it to a predefined value, to avoid its evaluation inside zlib */
   png_ptr->zstream.data_type = Z_BINARY;

   png_ptr->mode = PNG_HAVE_IHDR;
}

/* write the palette.  We are careful not to trust png_color to be in the
 * correct order for PNG, so people can redefine it to any convenient
 * structure.
 */
void png_write_PLTE(png_structp png_ptr, png_colorp palette, uint32_t num_pal)
{
   uint32_t i;
   png_colorp pal_ptr;
   uint8_t buf[3];

   png_debug(1, "in png_write_PLTE\n");
   if ((num_pal == 0) || num_pal > 256)
   {
     if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
     {
        png_error(png_ptr, "Invalid number of colors in palette");
     }
     else
     {
        png_warning(png_ptr, "Invalid number of colors in palette");
        return;
     }
   }

   if (!(png_ptr->color_type&PNG_COLOR_MASK_COLOR))
   {
      png_warning(png_ptr,
        "Ignoring request to write a PLTE chunk in grayscale PNG");
      return;
   }

   png_ptr->num_palette = (uint16_t)num_pal;
   png_debug1(3, "num_palette = %d\n", png_ptr->num_palette);

   png_write_chunk_start(png_ptr, (uint8_t*)png_PLTE, num_pal * 3);
#ifndef PNG_NO_POINTER_INDEXING
   for (i = 0, pal_ptr = palette; i < num_pal; i++, pal_ptr++)
   {
      buf[0] = pal_ptr->red;
      buf[1] = pal_ptr->green;
      buf[2] = pal_ptr->blue;
      png_write_chunk_data(png_ptr, buf, (size_t)3);
   }
#else
   /* This is a little slower but some buggy compilers need to do this instead */
   pal_ptr=palette;
   for (i = 0; i < num_pal; i++)
   {
      buf[0] = pal_ptr[i].red;
      buf[1] = pal_ptr[i].green;
      buf[2] = pal_ptr[i].blue;
      png_write_chunk_data(png_ptr, buf, (size_t)3);
   }
#endif
   png_write_chunk_end(png_ptr);
   png_ptr->mode |= PNG_HAVE_PLTE;
}

/* write an IDAT chunk */
void png_write_IDAT(png_structp png_ptr, uint8_t* data, size_t length)
{
   png_debug(1, "in png_write_IDAT\n");

   /* Optimize the CMF field in the zlib stream. */
   /* This hack of the zlib stream is compliant to the stream specification. */
   if (!(png_ptr->mode & PNG_HAVE_IDAT) &&
       png_ptr->compression_type == PNG_COMPRESSION_TYPE_BASE)
   {
      unsigned int z_cmf = data[0];  /* zlib compression method and flags */
      if ((z_cmf & 0x0f) == 8 && (z_cmf & 0xf0) <= 0x70)
      {
         /* Avoid memory underflows and multiplication overflows. */
         /* The conditions below are practically always satisfied;
            however, they still must be checked. */
         if (length >= 2 &&
             png_ptr->height < 16384 && png_ptr->width < 16384)
         {
            uint32_t uncompressed_idat_size = png_ptr->height *
               ((png_ptr->width *
               png_ptr->channels * png_ptr->bit_depth + 15) >> 3);
            unsigned int z_cinfo = z_cmf >> 4;
            unsigned int half_z_window_size = 1 << (z_cinfo + 7);
            while (uncompressed_idat_size <= half_z_window_size &&
                   half_z_window_size >= 256)
            {
               z_cinfo--;
               half_z_window_size >>= 1;
            }
            z_cmf = (z_cmf & 0x0f) | (z_cinfo << 4);
            if (data[0] != (uint8_t)z_cmf)
            {
               data[0] = (uint8_t)z_cmf;
               data[1] &= 0xe0;
               data[1] += (uint8_t)(0x1f - ((z_cmf << 8) + data[1]) % 0x1f);
            }
         }
      }
      else
         png_error(png_ptr,
            "Invalid zlib compression method or flags in IDAT");
   }

   png_write_chunk(png_ptr, (uint8_t*)png_IDAT, data, length);
   png_ptr->mode |= PNG_HAVE_IDAT;
}

/* write an IEND chunk */
void png_write_IEND(png_structp png_ptr)
{
   png_debug(1, "in png_write_IEND\n");
   png_write_chunk(png_ptr, (uint8_t*)png_IEND, (uint8_t*)NULL,
     (size_t)0);
   png_ptr->mode |= PNG_HAVE_IEND;
}

#if defined(PNG_WRITE_sRGB_SUPPORTED)
/* write a sRGB chunk */
void png_write_sRGB(png_structp png_ptr, int srgb_intent)
{
   uint8_t buf[1];

   png_debug(1, "in png_write_sRGB\n");
   if(srgb_intent >= PNG_sRGB_INTENT_LAST)
         png_warning(png_ptr,
            "Invalid sRGB rendering intent specified");
   buf[0]=(uint8_t)srgb_intent;
   png_write_chunk(png_ptr, (uint8_t*)png_sRGB, buf, (size_t)1);
}
#endif


#if defined(PNG_WRITE_sPLT_SUPPORTED)
/* write a sPLT chunk */
void png_write_sPLT(png_structp png_ptr, png_sPLT_tp spalette)
{
   size_t name_len;
   char* new_name;
   uint8_t entrybuf[10];
   int entry_size = (spalette->depth == 8 ? 6 : 10);
   int palette_size = entry_size * spalette->nentries;
   png_sPLT_entryp ep;
#ifdef PNG_NO_POINTER_INDEXING
   int i;
#endif

   png_debug(1, "in png_write_sPLT\n");
   if (spalette->name == NULL || (name_len = png_check_keyword(png_ptr,
      spalette->name, &new_name))==0)
   {
      png_warning(png_ptr, "Empty keyword in sPLT chunk");
      return;
   }

   /* make sure we include the NULL after the name */
   png_write_chunk_start(png_ptr, (uint8_t*)png_sPLT,
          (uint32_t)(name_len + 2 + palette_size));
   png_write_chunk_data(png_ptr, (uint8_t*)new_name, name_len + 1);
   png_write_chunk_data(png_ptr, (uint8_t*)&spalette->depth, 1);

   /* loop through each palette entry, writing appropriately */
#ifndef PNG_NO_POINTER_INDEXING
   for (ep = spalette->entries; ep<spalette->entries+spalette->nentries; ep++)
   {
       if (spalette->depth == 8)
       {
           entrybuf[0] = (uint8_t)ep->red;
           entrybuf[1] = (uint8_t)ep->green;
           entrybuf[2] = (uint8_t)ep->blue;
           entrybuf[3] = (uint8_t)ep->alpha;
           png_save_uint_16(entrybuf + 4, ep->frequency);
       }
       else
       {
           png_save_uint_16(entrybuf + 0, ep->red);
           png_save_uint_16(entrybuf + 2, ep->green);
           png_save_uint_16(entrybuf + 4, ep->blue);
           png_save_uint_16(entrybuf + 6, ep->alpha);
           png_save_uint_16(entrybuf + 8, ep->frequency);
       }
       png_write_chunk_data(png_ptr, entrybuf, (size_t)entry_size);
   }
#else
   ep=spalette->entries;
   for (i = 0; i>spalette->nentries; i++)
   {
       if (spalette->depth == 8)
       {
           entrybuf[0] = (uint8_t)ep[i].red;
           entrybuf[1] = (uint8_t)ep[i].green;
           entrybuf[2] = (uint8_t)ep[i].blue;
           entrybuf[3] = (uint8_t)ep[i].alpha;
           png_save_uint_16(entrybuf + 4, ep[i].frequency);
       }
       else
       {
           png_save_uint_16(entrybuf + 0, ep[i].red);
           png_save_uint_16(entrybuf + 2, ep[i].green);
           png_save_uint_16(entrybuf + 4, ep[i].blue);
           png_save_uint_16(entrybuf + 6, ep[i].alpha);
           png_save_uint_16(entrybuf + 8, ep[i].frequency);
       }
       png_write_chunk_data(png_ptr, entrybuf, entry_size);
   }
#endif

   png_write_chunk_end(png_ptr);
   png_free(png_ptr, new_name);
}
#endif

#if defined(PNG_WRITE_sBIT_SUPPORTED)
/* write the sBIT chunk */
void png_write_sBIT(png_structp png_ptr, png_color_8p sbit, int color_type)
{
   uint8_t buf[4];
   size_t size;

   png_debug(1, "in png_write_sBIT\n");
   /* make sure we don't depend upon the order of PNG_COLOR_8 */
   if (color_type & PNG_COLOR_MASK_COLOR)
   {
      uint8_t maxbits;

      maxbits = (uint8_t)(color_type==PNG_COLOR_TYPE_PALETTE ? 8 :
                png_ptr->usr_bit_depth);
      if (sbit->red == 0 || sbit->red > maxbits ||
          sbit->green == 0 || sbit->green > maxbits ||
          sbit->blue == 0 || sbit->blue > maxbits)
      {
         png_warning(png_ptr, "Invalid sBIT depth specified");
         return;
      }
      buf[0] = sbit->red;
      buf[1] = sbit->green;
      buf[2] = sbit->blue;
      size = 3;
   }
   else
   {
      if (sbit->gray == 0 || sbit->gray > png_ptr->usr_bit_depth)
      {
         png_warning(png_ptr, "Invalid sBIT depth specified");
         return;
      }
      buf[0] = sbit->gray;
      size = 1;
   }

   if (color_type & PNG_COLOR_MASK_ALPHA)
   {
      if (sbit->alpha == 0 || sbit->alpha > png_ptr->usr_bit_depth)
      {
         png_warning(png_ptr, "Invalid sBIT depth specified");
         return;
      }
      buf[size++] = sbit->alpha;
   }

   png_write_chunk(png_ptr, (uint8_t*)png_sBIT, buf, size);
}
#endif

#if defined(PNG_WRITE_tRNS_SUPPORTED)
/* write the tRNS chunk */
void png_write_tRNS(png_structp png_ptr, uint8_t* trans, png_color_16p tran,
   int num_trans, int color_type)
{
   uint8_t buf[6];

   png_debug(1, "in png_write_tRNS\n");
   if (color_type == PNG_COLOR_TYPE_PALETTE)
   {
      if (num_trans <= 0 || num_trans > (int)png_ptr->num_palette)
      {
         png_warning(png_ptr,"Invalid number of transparent colors specified");
         return;
      }
      /* write the chunk out as it is */
      png_write_chunk(png_ptr, (uint8_t*)png_tRNS, trans, (size_t)num_trans);
   }
   else if (color_type == PNG_COLOR_TYPE_GRAY)
   {
      /* one 16 bit value */
      if(tran->gray >= (1 << png_ptr->bit_depth))
      {
         png_warning(png_ptr,
           "Ignoring attempt to write tRNS chunk out-of-range for bit_depth");
         return;
      }
      png_save_uint_16(buf, tran->gray);
      png_write_chunk(png_ptr, (uint8_t*)png_tRNS, buf, (size_t)2);
   }
   else if (color_type == PNG_COLOR_TYPE_RGB)
   {
      /* three 16 bit values */
      png_save_uint_16(buf, tran->red);
      png_save_uint_16(buf + 2, tran->green);
      png_save_uint_16(buf + 4, tran->blue);
      if(png_ptr->bit_depth == 8 && (buf[0] | buf[2] | buf[4]))
         {
            png_warning(png_ptr,
              "Ignoring attempt to write 16-bit tRNS chunk when bit_depth is 8");
            return;
         }
      png_write_chunk(png_ptr, (uint8_t*)png_tRNS, buf, (size_t)6);
   }
   else
   {
      png_warning(png_ptr, "Can't write tRNS with an alpha channel");
   }
}
#endif


#if defined(PNG_WRITE_hIST_SUPPORTED)
/* write the histogram */
void png_write_hIST(png_structp png_ptr, uint16_t* hist, int num_hist)
{
   int i;
   uint8_t buf[3];

   png_debug(1, "in png_write_hIST\n");
   if (num_hist > (int)png_ptr->num_palette)
   {
      png_debug2(3, "num_hist = %d, num_palette = %d\n", num_hist,
         png_ptr->num_palette);
      png_warning(png_ptr, "Invalid number of histogram entries specified");
      return;
   }

   png_write_chunk_start(png_ptr, (uint8_t*)png_hIST, (uint32_t)(num_hist * 2));
   for (i = 0; i < num_hist; i++)
   {
      png_save_uint_16(buf, hist[i]);
      png_write_chunk_data(png_ptr, buf, (size_t)2);
   }
   png_write_chunk_end(png_ptr);
}
#endif

#if defined(PNG_WRITE_pCAL_SUPPORTED) || defined(PNG_WRITE_sPLT_SUPPORTED)
/* Check that the tEXt or zTXt keyword is valid per PNG 1.0 specification,
 * and if invalid, correct the keyword rather than discarding the entire
 * chunk.  The PNG 1.0 specification requires keywords 1-79 characters in
 * length, forbids leading or trailing whitespace, multiple internal spaces,
 * and the non-break space (0x80) from ISO 8859-1.  Returns keyword length.
 *
 * The new_key is allocated to hold the corrected keyword and must be freed
 * by the calling routine.  This avoids problems with trying to write to
 * static keywords without having to have duplicate copies of the strings.
 */
size_t png_check_keyword(png_structp png_ptr, char* key, png_charpp new_key)
{
   size_t key_len;
   char* kp;
   char* dp;
   int kflag;
   int kwarn=0;

   png_debug(1, "in png_check_keyword\n");
   *new_key = NULL;

   if (key == NULL || (key_len = strlen(key)) == 0)
   {
      png_warning(png_ptr, "zero length keyword");
      return ((size_t)0);
   }

   png_debug1(2, "Keyword to be checked is '%s'\n", key);

   *new_key = (char*)png_malloc_warn(png_ptr, (uint32_t)(key_len + 2));
   if (*new_key == NULL)
   {
      png_warning(png_ptr, "Out of memory while procesing keyword");
      return ((size_t)0);
   }

   /* Replace non-printing characters with a blank and print a warning */
   for (kp = key, dp = *new_key; *kp != '\0'; kp++, dp++)
   {
      if (*kp < 0x20 || (*kp > 0x7E && (uint8_t)*kp < 0xA1))
      {
         char msg[40];

         sprintf(msg, "invalid keyword character 0x%02X", *kp);
         png_warning(png_ptr, msg);
         *dp = ' ';
      }
      else
      {
         *dp = *kp;
      }
   }
   *dp = '\0';

   /* Remove any trailing white space. */
   kp = *new_key + key_len - 1;
   if (*kp == ' ')
   {
      png_warning(png_ptr, "trailing spaces removed from keyword");

      while (*kp == ' ')
      {
        *(kp--) = '\0';
        key_len--;
      }
   }

   /* Remove any leading white space. */
   kp = *new_key;
   if (*kp == ' ')
   {
      png_warning(png_ptr, "leading spaces removed from keyword");

      while (*kp == ' ')
      {
        kp++;
        key_len--;
      }
   }

   png_debug1(2, "Checking for multiple internal spaces in '%s'\n", kp);

   /* Remove multiple internal spaces. */
   for (kflag = 0, dp = *new_key; *kp != '\0'; kp++)
   {
      if (*kp == ' ' && kflag == 0)
      {
         *(dp++) = *kp;
         kflag = 1;
      }
      else if (*kp == ' ')
      {
         key_len--;
         kwarn=1;
      }
      else
      {
         *(dp++) = *kp;
         kflag = 0;
      }
   }
   *dp = '\0';
   if(kwarn)
      png_warning(png_ptr, "extra interior spaces removed from keyword");

   if (key_len == 0)
   {
      png_free(png_ptr, *new_key);
      *new_key=NULL;
      png_warning(png_ptr, "Zero length keyword");
   }

   if (key_len > 79)
   {
      png_warning(png_ptr, "keyword length must be 1 - 79 characters");
      new_key[79] = '\0';
      key_len = 79;
   }

   return (key_len);
}
#endif

#if defined(PNG_WRITE_pCAL_SUPPORTED)
/* write the pCAL chunk (described in the PNG extensions document) */
void png_write_pCAL(png_structp png_ptr, char* purpose, int32_t X0,
   int32_t X1, int type, int nparams, char* units, png_charpp params)
{
   size_t purpose_len, units_len, total_len;
   uint32_t* params_len;
   uint8_t buf[10];
   char* new_purpose;
   int i;

   png_debug1(1, "in png_write_pCAL (%d parameters)\n", nparams);
   if (type >= PNG_EQUATION_LAST)
      png_warning(png_ptr, "Unrecognized equation type for pCAL chunk");

   purpose_len = png_check_keyword(png_ptr, purpose, &new_purpose) + 1;
   png_debug1(3, "pCAL purpose length = %d\n", (int)purpose_len);
   units_len = strlen(units) + (nparams == 0 ? 0 : 1);
   png_debug1(3, "pCAL units length = %d\n", (int)units_len);
   total_len = purpose_len + units_len + 10;

   params_len = (uint32_t*)png_malloc(png_ptr, (uint32_t)(nparams
      *sizeof(uint32_t)));

   /* Find the length of each parameter, making sure we don't count the
      null terminator for the last parameter. */
   for (i = 0; i < nparams; i++)
   {
      params_len[i] = (int)strlen(params[i]) + (i == nparams - 1 ? 0 : 1);
      png_debug2(3, "pCAL parameter %d length = %lu\n", i, params_len[i]);
      total_len += (size_t)params_len[i];
   }

   png_debug1(3, "pCAL total length = %d\n", (int)total_len);
   png_write_chunk_start(png_ptr, (uint8_t*)png_pCAL, (uint32_t)total_len);
   png_write_chunk_data(png_ptr, (uint8_t*)new_purpose, purpose_len);
   png_save_int_32(buf, X0);
   png_save_int_32(buf + 4, X1);
   buf[8] = (uint8_t)type;
   buf[9] = (uint8_t)nparams;
   png_write_chunk_data(png_ptr, buf, (size_t)10);
   png_write_chunk_data(png_ptr, (uint8_t*)units, (size_t)units_len);

   png_free(png_ptr, new_purpose);

   for (i = 0; i < nparams; i++)
   {
      png_write_chunk_data(png_ptr, (uint8_t*)params[i],
         (size_t)params_len[i]);
   }

   png_free(png_ptr, params_len);
   png_write_chunk_end(png_ptr);
}
#endif
#if defined(PNG_WRITE_pHYs_SUPPORTED)
/* write the pHYs chunk */
void png_write_pHYs(png_structp png_ptr, uint32_t x_pixels_per_unit,
   uint32_t y_pixels_per_unit,
   int unit_type)
{
   uint8_t buf[9];

   png_debug(1, "in png_write_pHYs\n");
   if (unit_type >= PNG_RESOLUTION_LAST)
      png_warning(png_ptr, "Unrecognized unit type for pHYs chunk");

   png_save_uint_32(buf, x_pixels_per_unit);
   png_save_uint_32(buf + 4, y_pixels_per_unit);
   buf[8] = (uint8_t)unit_type;

   png_write_chunk(png_ptr, (uint8_t*)png_pHYs, buf, (size_t)9);
}
#endif

/* initializes the row writing capability of libpng */
void png_write_start_row(png_structp png_ptr)
{
   size_t buf_size;

   png_debug(1, "in png_write_start_row\n");
   buf_size = (size_t)(PNG_ROWBYTES(
      png_ptr->usr_channels*png_ptr->usr_bit_depth,png_ptr->width)+1);

   /* set up row buffer */
   png_ptr->row_buf = (uint8_t*)png_malloc(png_ptr, (uint32_t)buf_size);
   png_ptr->row_buf[0] = PNG_FILTER_VALUE_NONE;

   /* set up filtering buffer, if using this filter */
   if (png_ptr->do_filter & PNG_FILTER_SUB)
   {
      png_ptr->sub_row = (uint8_t*)png_malloc(png_ptr,
         (png_ptr->rowbytes + 1));
      png_ptr->sub_row[0] = PNG_FILTER_VALUE_SUB;
   }

   /* We only need to keep the previous row if we are using one of these. */
   if (png_ptr->do_filter & (PNG_FILTER_AVG | PNG_FILTER_UP | PNG_FILTER_PAETH))
   {
     /* set up previous row buffer */
      png_ptr->prev_row = (uint8_t*)png_malloc(png_ptr, (uint32_t)buf_size);
      memset(png_ptr->prev_row, 0, buf_size);

      if (png_ptr->do_filter & PNG_FILTER_UP)
      {
         png_ptr->up_row = (uint8_t* )png_malloc(png_ptr,
            (png_ptr->rowbytes + 1));
         png_ptr->up_row[0] = PNG_FILTER_VALUE_UP;
      }

      if (png_ptr->do_filter & PNG_FILTER_AVG)
      {
         png_ptr->avg_row = (uint8_t*)png_malloc(png_ptr,
            (png_ptr->rowbytes + 1));
         png_ptr->avg_row[0] = PNG_FILTER_VALUE_AVG;
      }

      if (png_ptr->do_filter & PNG_FILTER_PAETH)
      {
         png_ptr->paeth_row = (uint8_t* )png_malloc(png_ptr,
            (png_ptr->rowbytes + 1));
         png_ptr->paeth_row[0] = PNG_FILTER_VALUE_PAETH;
      }
   }

   png_ptr->num_rows = png_ptr->height;
   png_ptr->usr_width = png_ptr->width;
   png_ptr->zstream.avail_out = (uint32_t)png_ptr->zbuf_size;
   png_ptr->zstream.next_out = png_ptr->zbuf;
}

/* Internal use only.  Called when finished processing a row of data. */
void png_write_finish_row(png_structp png_ptr)
{
   int ret;

   png_debug(1, "in png_write_finish_row\n");
   /* next row */
   png_ptr->row_number++;

   /* see if we are done */
   if (png_ptr->row_number < png_ptr->num_rows)
      return;

   /* if we get here, we've just written the last row, so we need
      to flush the compressor */
   do
   {
      /* tell the compressor we are done */
      ret = deflate(&png_ptr->zstream, Z_FINISH);
      /* check for an error */
      if (ret == Z_OK)
      {
         /* check to see if we need more room */
         if (!(png_ptr->zstream.avail_out))
         {
            png_write_IDAT(png_ptr, png_ptr->zbuf, png_ptr->zbuf_size);
            png_ptr->zstream.next_out = png_ptr->zbuf;
            png_ptr->zstream.avail_out = (uint32_t)png_ptr->zbuf_size;
         }
      }
      else if (ret != Z_STREAM_END)
      {
         if (png_ptr->zstream.msg != NULL)
            png_error(png_ptr, png_ptr->zstream.msg);
         else
            png_error(png_ptr, "zlib error");
      }
   } while (ret != Z_STREAM_END);

   /* write any extra space */
   if (png_ptr->zstream.avail_out < png_ptr->zbuf_size)
   {
      png_write_IDAT(png_ptr, png_ptr->zbuf, png_ptr->zbuf_size -
         png_ptr->zstream.avail_out);
   }

   deflateReset(&png_ptr->zstream);
   png_ptr->zstream.data_type = Z_BINARY;
}

/* This filters the row, chooses which filter to use, if it has not already
 * been specified by the application, and then writes the row out with the
 * chosen filter.
 */
#define PNG_MAXSUM (((uint32_t)(-1)) >> 1)
#define PNG_HISHIFT 10
#define PNG_LOMASK ((uint32_t)0xffffL)
#define PNG_HIMASK ((uint32_t)(~PNG_LOMASK >> PNG_HISHIFT))
void png_write_find_filter(png_structp png_ptr, png_row_infop row_info)
{
    uint8_t* prev_row;
    uint8_t* best_row;
    uint8_t* row_buf;
    uint32_t mins, bpp;
    uint8_t filter_to_do = png_ptr->do_filter;
    uint32_t row_bytes = row_info->rowbytes;

   png_debug(1, "in png_write_find_filter\n");
   /* find out how many bytes offset each pixel is */
   bpp = (row_info->pixel_depth + 7) >> 3;

   prev_row = png_ptr->prev_row;
   best_row = row_buf = png_ptr->row_buf;
   mins = PNG_MAXSUM;

   /* The prediction method we use is to find which method provides the
    * smallest value when summing the absolute values of the distances
    * from zero, using anything >= 128 as negative numbers.  This is known
    * as the "minimum sum of absolute differences" heuristic.  Other
    * heuristics are the "weighted minimum sum of absolute differences"
    * (experimental and can in theory improve compression), and the "zlib
    * predictive" method (not implemented yet), which does test compressions
    * of lines using different filter methods, and then chooses the
    * (series of) filter(s) that give minimum compressed data size (VERY
    * computationally expensive).
    *
    * GRR 980525:  consider also
    *   (1) minimum sum of absolute differences from running average (i.e.,
    *       keep running sum of non-absolute differences & count of bytes)
    *       [track dispersion, too?  restart average if dispersion too large?]
    *  (1b) minimum sum of absolute differences from sliding average, probably
    *       with window size <= deflate window (usually 32K)
    *   (2) minimum sum of squared differences from zero or running average
    *       (i.e., ~ root-mean-square approach)
    */


   /* We don't need to test the 'no filter' case if this is the only filter
    * that has been chosen, as it doesn't actually do anything to the data.
    */
   if ((filter_to_do & PNG_FILTER_NONE) &&
       filter_to_do != PNG_FILTER_NONE)
   {
      uint8_t* rp;
      uint32_t sum = 0;
      uint32_t i;
      int v;

      for (i = 0, rp = row_buf + 1; i < row_bytes; i++, rp++)
      {
         v = *rp;
         sum += (v < 128) ? v : 256 - v;
      }
      mins = sum;
   }

   /* sub filter */
   if (filter_to_do == PNG_FILTER_SUB)
   /* it's the only filter so no testing is needed */
   {
      uint8_t* rp;
      uint8_t* lp;
      uint8_t* dp;
      uint32_t i;
      for (i = 0, rp = row_buf + 1, dp = png_ptr->sub_row + 1; i < bpp;
           i++, rp++, dp++)
      {
         *dp = *rp;
      }
      for (lp = row_buf + 1; i < row_bytes;
         i++, rp++, lp++, dp++)
      {
         *dp = (uint8_t)(((int)*rp - (int)*lp) & 0xff);
      }
      best_row = png_ptr->sub_row;
   }

   else if (filter_to_do & PNG_FILTER_SUB)
   {
       uint8_t* rp;
       uint8_t* dp;
       uint8_t* lp;
       uint32_t sum = 0;
       uint32_t lmins = mins;
      uint32_t i;
      int v;

      for (i = 0, rp = row_buf + 1, dp = png_ptr->sub_row + 1; i < bpp;
           i++, rp++, dp++)
      {
         v = *dp = *rp;

         sum += (v < 128) ? v : 256 - v;
      }
      for (lp = row_buf + 1; i < row_bytes;
         i++, rp++, lp++, dp++)
      {
         v = *dp = (uint8_t)(((int)*rp - (int)*lp) & 0xff);

         sum += (v < 128) ? v : 256 - v;

         if (sum > lmins)  /* We are already worse, don't continue. */
            break;
      }

      if (sum < mins)
      {
         mins = sum;
         best_row = png_ptr->sub_row;
      }
   }

   /* up filter */
   if (filter_to_do == PNG_FILTER_UP)
   {
       uint8_t* rp;
       uint8_t* dp;
       uint8_t* pp;
       uint32_t i;

      for (i = 0, rp = row_buf + 1, dp = png_ptr->up_row + 1,
           pp = prev_row + 1; i < row_bytes;
           i++, rp++, pp++, dp++)
      {
         *dp = (uint8_t)(((int)*rp - (int)*pp) & 0xff);
      }
      best_row = png_ptr->up_row;
   }

   else if (filter_to_do & PNG_FILTER_UP)
   {
       uint8_t* rp;
       uint8_t* dp;
       uint8_t* pp;
       uint32_t sum = 0;
       uint32_t lmins = mins;
       uint32_t i;
       int v;

        for (i = 0, rp = row_buf + 1, dp = png_ptr->up_row + 1, pp = prev_row + 1; i < row_bytes; i++)
        {
            v = *dp++ = (uint8_t)(((int)*rp++ - (int)*pp++) & 0xff);

            sum += (v < 128) ? v : 256 - v;

            if (sum > lmins)  /* We are already worse, don't continue. */
            break;
        }

        if (sum < mins)
        {
            mins = sum;
            best_row = png_ptr->up_row;
        }
   }

   /* avg filter */
   if (filter_to_do == PNG_FILTER_AVG)
   {
       uint8_t* rp;
       uint8_t* dp;
       uint8_t* pp;
       uint8_t* lp;
       uint32_t i;
      for (i = 0, rp = row_buf + 1, dp = png_ptr->avg_row + 1,
           pp = prev_row + 1; i < bpp; i++)
      {
         *dp++ = (uint8_t)(((int)*rp++ - ((int)*pp++ / 2)) & 0xff);
      }
      for (lp = row_buf + 1; i < row_bytes; i++)
      {
         *dp++ = (uint8_t)(((int)*rp++ - (((int)*pp++ + (int)*lp++) / 2))
                 & 0xff);
      }
      best_row = png_ptr->avg_row;
   }

   else if (filter_to_do & PNG_FILTER_AVG)
   {
      uint8_t* rp;
      uint8_t* dp;
      uint8_t* pp;
      uint8_t* lp;
      uint32_t sum = 0, lmins = mins;
      uint32_t i;
      int v;

      for (i = 0, rp = row_buf + 1, dp = png_ptr->avg_row + 1,
           pp = prev_row + 1; i < bpp; i++)
      {
         v = *dp++ = (uint8_t)(((int)*rp++ - ((int)*pp++ / 2)) & 0xff);

         sum += (v < 128) ? v : 256 - v;
      }
      for (lp = row_buf + 1; i < row_bytes; i++)
      {
         v = *dp++ =
          (uint8_t)(((int)*rp++ - (((int)*pp++ + (int)*lp++) / 2)) & 0xff);

         sum += (v < 128) ? v : 256 - v;

         if (sum > lmins)  /* We are already worse, don't continue. */
            break;
      }

      if (sum < mins)
      {
         mins = sum;
         best_row = png_ptr->avg_row;
      }
   }

   /* Paeth filter */
   if (filter_to_do == PNG_FILTER_PAETH)
   {
      uint8_t* rp;
      uint8_t* dp;
      uint8_t* pp;
      uint8_t* cp;
      uint8_t* lp;

      uint32_t i;
      for (i = 0, rp = row_buf + 1, dp = png_ptr->paeth_row + 1,
           pp = prev_row + 1; i < bpp; i++)
      {
         *dp++ = (uint8_t)(((int)*rp++ - (int)*pp++) & 0xff);
      }

      for (lp = row_buf + 1, cp = prev_row + 1; i < row_bytes; i++)
      {
         int a, b, c, pa, pb, pc, p;

         b = *pp++;
         c = *cp++;
         a = *lp++;

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

         p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;

         *dp++ = (uint8_t)(((int)*rp++ - p) & 0xff);
      }
      best_row = png_ptr->paeth_row;
   }

   else if (filter_to_do & PNG_FILTER_PAETH)
   {
       uint8_t* rp;
       uint8_t* dp;
       uint8_t* pp;
       uint8_t* cp;
       uint8_t* lp;
       uint32_t sum = 0;
       uint32_t lmins = mins;
       uint32_t i;
       int v;

      for (i = 0, rp = row_buf + 1, dp = png_ptr->paeth_row + 1,
           pp = prev_row + 1; i < bpp; i++)
      {
         v = *dp++ = (uint8_t)(((int)*rp++ - (int)*pp++) & 0xff);

         sum += (v < 128) ? v : 256 - v;
      }

      for (lp = row_buf + 1, cp = prev_row + 1; i < row_bytes; i++)
      {
         int a, b, c, pa, pb, pc, p;

         b = *pp++;
         c = *cp++;
         a = *lp++;

#ifndef PNG_SLOW_PAETH
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
         p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;
#else /* PNG_SLOW_PAETH */
         p = a + b - c;
         pa = abs(p - a);
         pb = abs(p - b);
         pc = abs(p - c);
         if (pa <= pb && pa <= pc)
            p = a;
         else if (pb <= pc)
            p = b;
         else
            p = c;
#endif /* PNG_SLOW_PAETH */

         v = *dp++ = (uint8_t)(((int)*rp++ - p) & 0xff);

         sum += (v < 128) ? v : 256 - v;

         if (sum > lmins)  /* We are already worse, don't continue. */
            break;
      }

      if (sum < mins)
      {
         best_row = png_ptr->paeth_row;
      }
   }

   /* Do the actual writing of the filtered row data from the chosen filter. */

   png_write_filtered_row(png_ptr, best_row);
}


/* Do the actual writing of a previously filtered row. */
void png_write_filtered_row(png_structp png_ptr, uint8_t* filtered_row)
{
   png_debug(1, "in png_write_filtered_row\n");
   png_debug1(2, "filter = %d\n", filtered_row[0]);
   /* set up the zlib input buffer */

   png_ptr->zstream.next_in = filtered_row;
   png_ptr->zstream.avail_in = (uint32_t)png_ptr->row_info.rowbytes + 1;
   /* repeat until we have compressed all the data */
   do
   {
      int ret; /* return of zlib */

      /* compress the data */
      ret = deflate(&png_ptr->zstream, Z_NO_FLUSH);
      /* check for compression errors */
      if (ret != Z_OK)
      {
         if (png_ptr->zstream.msg != NULL)
            png_error(png_ptr, png_ptr->zstream.msg);
         else
            png_error(png_ptr, "zlib error");
      }

      /* see if it is time to write another IDAT */
      if (!(png_ptr->zstream.avail_out))
      {
         /* write the IDAT and reset the zlib output buffer */
         png_write_IDAT(png_ptr, png_ptr->zbuf, png_ptr->zbuf_size);
         png_ptr->zstream.next_out = png_ptr->zbuf;
         png_ptr->zstream.avail_out = (uint32_t)png_ptr->zbuf_size;
      }
   /* repeat until all data has been compressed */
   } while (png_ptr->zstream.avail_in);

   /* swap the current and previous rows */
   if (png_ptr->prev_row != NULL)
   {
      uint8_t* tptr;

      tptr = png_ptr->prev_row;
      png_ptr->prev_row = png_ptr->row_buf;
      png_ptr->row_buf = tptr;
   }

   /* finish row - updates counters and flushes zlib if last row */
   png_write_finish_row(png_ptr);

#if defined(PNG_WRITE_FLUSH_SUPPORTED)
   png_ptr->flush_rows++;

   if (png_ptr->flush_dist > 0 &&
       png_ptr->flush_rows >= png_ptr->flush_dist)
   {
      png_write_flush(png_ptr);
   }
#endif
}
#endif /* PNG_WRITE_SUPPORTED */
