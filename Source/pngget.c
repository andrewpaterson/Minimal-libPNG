
/* pngget.c - retrieval of values from info struct
 *
 * Last changed in libpng 1.2.15 January 5, 2007
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2007 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 */

#define PNG_INTERNAL
#include "png.h"

#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)

uint32_t PNGAPI
png_get_valid(png_structp png_ptr, png_infop info_ptr, uint32_t flag)
{
   if (png_ptr != NULL && info_ptr != NULL)
      return(info_ptr->valid & flag);
   else
      return(0);
}

uint32_t PNGAPI
png_get_rowbytes(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
      return(info_ptr->rowbytes);
   else
      return(0);
}

#if defined(PNG_INFO_IMAGE_SUPPORTED)
png_bytepp PNGAPI
png_get_rows(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
      return(info_ptr->row_pointers);
   else
      return(0);
}
#endif

#ifdef PNG_EASY_ACCESS_SUPPORTED
/* easy access to info, added in libpng-0.99 */
uint32_t PNGAPI
png_get_image_width(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
   {
      return info_ptr->width;
   }
   return (0);
}

uint32_t PNGAPI
png_get_image_height(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
   {
      return info_ptr->height;
   }
   return (0);
}

uint8_t PNGAPI
png_get_bit_depth(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
   {
      return info_ptr->bit_depth;
   }
   return (0);
}

uint8_t PNGAPI
png_get_color_type(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
   {
      return info_ptr->color_type;
   }
   return (0);
}

uint8_t PNGAPI
png_get_filter_type(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
   {
      return info_ptr->filter_type;
   }
   return (0);
}

uint8_t PNGAPI
png_get_interlace_type(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
   {
      return info_ptr->interlace_type;
   }
   return (0);
}

uint8_t PNGAPI
png_get_compression_type(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
   {
      return info_ptr->compression_type;
   }
   return (0);
}

uint32_t PNGAPI
png_get_x_pixels_per_meter(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
#if defined(PNG_pHYs_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_pHYs)
   {
      png_debug1(1, "in %s retrieval function\n", "png_get_x_pixels_per_meter");
      if(info_ptr->phys_unit_type != PNG_RESOLUTION_METER)
          return (0);
      else return (info_ptr->x_pixels_per_unit);
   }
#else
   return (0);
#endif
   return (0);
}

uint32_t PNGAPI
png_get_y_pixels_per_meter(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
#if defined(PNG_pHYs_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_pHYs)
   {
      png_debug1(1, "in %s retrieval function\n", "png_get_y_pixels_per_meter");
      if(info_ptr->phys_unit_type != PNG_RESOLUTION_METER)
          return (0);
      else return (info_ptr->y_pixels_per_unit);
   }
#else
   return (0);
#endif
   return (0);
}

uint32_t PNGAPI
png_get_pixels_per_meter(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
#if defined(PNG_pHYs_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_pHYs)
   {
      png_debug1(1, "in %s retrieval function\n", "png_get_pixels_per_meter");
      if(info_ptr->phys_unit_type != PNG_RESOLUTION_METER ||
         info_ptr->x_pixels_per_unit != info_ptr->y_pixels_per_unit)
          return (0);
      else return (info_ptr->x_pixels_per_unit);
   }
#else
   return (0);
#endif
   return (0);
}

#ifdef PNG_FLOATING_POINT_SUPPORTED
float PNGAPI
png_get_pixel_aspect_ratio(png_structp png_ptr, png_infop info_ptr)
   {
   if (png_ptr != NULL && info_ptr != NULL)
#if defined(PNG_pHYs_SUPPORTED)
   if (info_ptr->valid & PNG_INFO_pHYs)
   {
      png_debug1(1, "in %s retrieval function\n", "png_get_aspect_ratio");
      if (info_ptr->x_pixels_per_unit == 0)
         return ((float)0.0);
      else
         return ((float)((float)info_ptr->y_pixels_per_unit
            /(float)info_ptr->x_pixels_per_unit));
   }
#else
   return (0.0);
#endif
   return ((float)0.0);
}
#endif


/* png_get_channels really belongs in here, too, but it's been around longer */

#endif  /* PNG_EASY_ACCESS_SUPPORTED */

uint8_t PNGAPI
png_get_channels(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
      return(info_ptr->channels);
   else
      return (0);
}

png_bytep PNGAPI
png_get_signature(png_structp png_ptr, png_infop info_ptr)
{
   if (png_ptr != NULL && info_ptr != NULL)
      return(info_ptr->signature);
   else
      return (NULL);
}

#if defined(PNG_bKGD_SUPPORTED)
uint32_t PNGAPI
png_get_bKGD(png_structp png_ptr, png_infop info_ptr,
   png_color_16p *background)
{
   if (png_ptr != NULL && info_ptr != NULL && (info_ptr->valid & PNG_INFO_bKGD)
      && background != NULL)
   {
      png_debug1(1, "in %s retrieval function\n", "bKGD");
      *background = &(info_ptr->background);
      return (PNG_INFO_bKGD);
   }
   return (0);
}
#endif

#if defined(PNG_sRGB_SUPPORTED)
uint32_t PNGAPI
png_get_sRGB(png_structp png_ptr, png_infop info_ptr, int *file_srgb_intent)
{
   if (png_ptr != NULL && info_ptr != NULL && (info_ptr->valid & PNG_INFO_sRGB)
      && file_srgb_intent != NULL)
   {
      png_debug1(1, "in %s retrieval function\n", "sRGB");
      *file_srgb_intent = (int)info_ptr->srgb_intent;
      return (PNG_INFO_sRGB);
   }
   return (0);
}
#endif


#if defined(PNG_sPLT_SUPPORTED)
uint32_t PNGAPI
png_get_sPLT(png_structp png_ptr, png_infop info_ptr,
             png_sPLT_tpp spalettes)
{
   if (png_ptr != NULL && info_ptr != NULL && spalettes != NULL)
     *spalettes = info_ptr->splt_palettes;
   return ((uint32_t)info_ptr->splt_palettes_num);
}
#endif

#if defined(PNG_hIST_SUPPORTED)
uint32_t PNGAPI
png_get_hIST(png_structp png_ptr, png_infop info_ptr, png_uint_16p *hist)
{
   if (png_ptr != NULL && info_ptr != NULL && (info_ptr->valid & PNG_INFO_hIST)
      && hist != NULL)
   {
      png_debug1(1, "in %s retrieval function\n", "hIST");
      *hist = info_ptr->hist;
      return (PNG_INFO_hIST);
   }
   return (0);
}
#endif

uint32_t PNGAPI
png_get_IHDR(png_structp png_ptr, png_infop info_ptr,
   uint32_t *width, uint32_t *height, int *bit_depth,
   int *color_type, int *interlace_type, int *compression_type,
   int *filter_type)

{
   if (png_ptr != NULL && info_ptr != NULL && width != NULL && height != NULL &&
      bit_depth != NULL && color_type != NULL)
   {
      png_debug1(1, "in %s retrieval function\n", "IHDR");
      *width = info_ptr->width;
      *height = info_ptr->height;
      *bit_depth = info_ptr->bit_depth;
      if (info_ptr->bit_depth < 1 || info_ptr->bit_depth > 16)
        png_error(png_ptr, "Invalid bit depth");
      *color_type = info_ptr->color_type;
      if (info_ptr->color_type > 6)
        png_error(png_ptr, "Invalid color type");
      if (compression_type != NULL)
         *compression_type = info_ptr->compression_type;
      if (filter_type != NULL)
         *filter_type = info_ptr->filter_type;
      if (interlace_type != NULL)
         *interlace_type = info_ptr->interlace_type;

      /* check for potential overflow of rowbytes */
      if (*width == 0 || *width > PNG_UINT_31_MAX)
        png_error(png_ptr, "Invalid image width");
      if (*height == 0 || *height > PNG_UINT_31_MAX)
        png_error(png_ptr, "Invalid image height");
      if (info_ptr->width > (PNG_UINT_32_MAX
                 >> 3)      /* 8-byte RGBA pixels */
                 - 64       /* bigrowbuf hack */
                 - 1        /* filter byte */
                 - 7*8      /* rounding of width to multiple of 8 pixels */
                 - 8)       /* extra max_pixel_depth pad */
      {
         png_warning(png_ptr,
            "Width too large for libpng to process image data.");
      }
      return (1);
   }
   return (0);
}


#if defined(PNG_pCAL_SUPPORTED)
uint32_t PNGAPI
png_get_pCAL(png_structp png_ptr, png_infop info_ptr,
   png_charp *purpose, int32_t *X0, int32_t *X1, int *type, int *nparams,
   png_charp *units, png_charpp *params)
{
   if (png_ptr != NULL && info_ptr != NULL && (info_ptr->valid & PNG_INFO_pCAL)
      && purpose != NULL && X0 != NULL && X1 != NULL && type != NULL &&
      nparams != NULL && units != NULL && params != NULL)
   {
      png_debug1(1, "in %s retrieval function\n", "pCAL");
      *purpose = info_ptr->pcal_purpose;
      *X0 = info_ptr->pcal_X0;
      *X1 = info_ptr->pcal_X1;
      *type = (int)info_ptr->pcal_type;
      *nparams = (int)info_ptr->pcal_nparams;
      *units = info_ptr->pcal_units;
      *params = info_ptr->pcal_params;
      return (PNG_INFO_pCAL);
   }
   return (0);
}
#endif


#if defined(PNG_pHYs_SUPPORTED)
uint32_t PNGAPI
png_get_pHYs(png_structp png_ptr, png_infop info_ptr,
   uint32_t *res_x, uint32_t *res_y, int *unit_type)
{
   uint32_t retval = 0;

   if (png_ptr != NULL && info_ptr != NULL &&
      (info_ptr->valid & PNG_INFO_pHYs))
   {
      png_debug1(1, "in %s retrieval function\n", "pHYs");
      if (res_x != NULL)
      {
         *res_x = info_ptr->x_pixels_per_unit;
         retval |= PNG_INFO_pHYs;
      }
      if (res_y != NULL)
      {
         *res_y = info_ptr->y_pixels_per_unit;
         retval |= PNG_INFO_pHYs;
      }
      if (unit_type != NULL)
      {
         *unit_type = (int)info_ptr->phys_unit_type;
         retval |= PNG_INFO_pHYs;
      }
   }
   return (retval);
}
#endif

uint32_t PNGAPI
png_get_PLTE(png_structp png_ptr, png_infop info_ptr, png_colorp *palette,
   int *num_palette)
{
   if (png_ptr != NULL && info_ptr != NULL && (info_ptr->valid & PNG_INFO_PLTE)
       && palette != NULL)
   {
      png_debug1(1, "in %s retrieval function\n", "PLTE");
      *palette = info_ptr->palette;
      *num_palette = info_ptr->num_palette;
      png_debug1(3, "num_palette = %d\n", *num_palette);
      return (PNG_INFO_PLTE);
   }
   return (0);
}

#if defined(PNG_sBIT_SUPPORTED)
uint32_t PNGAPI
png_get_sBIT(png_structp png_ptr, png_infop info_ptr, png_color_8p *sig_bit)
{
   if (png_ptr != NULL && info_ptr != NULL && (info_ptr->valid & PNG_INFO_sBIT)
      && sig_bit != NULL)
   {
      png_debug1(1, "in %s retrieval function\n", "sBIT");
      *sig_bit = &(info_ptr->sig_bit);
      return (PNG_INFO_sBIT);
   }
   return (0);
}
#endif

#if defined(PNG_tRNS_SUPPORTED)
uint32_t PNGAPI
png_get_tRNS(png_structp png_ptr, png_infop info_ptr,
   png_bytep *trans, int *num_trans, png_color_16p *trans_values)
{
   uint32_t retval = 0;
   if (png_ptr != NULL && info_ptr != NULL && (info_ptr->valid & PNG_INFO_tRNS))
   {
      png_debug1(1, "in %s retrieval function\n", "tRNS");
      if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
      {
          if (trans != NULL)
          {
             *trans = info_ptr->trans;
             retval |= PNG_INFO_tRNS;
          }
          if (trans_values != NULL)
             *trans_values = &(info_ptr->trans_values);
      }
      else /* if (info_ptr->color_type != PNG_COLOR_TYPE_PALETTE) */
      {
          if (trans_values != NULL)
          {
             *trans_values = &(info_ptr->trans_values);
             retval |= PNG_INFO_tRNS;
          }
          if(trans != NULL)
             *trans = NULL;
      }
      if(num_trans != NULL)
      {
         *num_trans = info_ptr->num_trans;
         retval |= PNG_INFO_tRNS;
      }
   }
   return (retval);
}
#endif

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
uint32_t PNGAPI
png_get_unknown_chunks(png_structp png_ptr, png_infop info_ptr,
             png_unknown_chunkpp unknowns)
{
   if (png_ptr != NULL && info_ptr != NULL && unknowns != NULL)
     *unknowns = info_ptr->unknown_chunks;
   return ((uint32_t)info_ptr->unknown_chunks_num);
}
#endif

#if defined(PNG_READ_RGB_TO_GRAY_SUPPORTED)
uint8_t PNGAPI
png_get_rgb_to_gray_status (png_structp png_ptr)
{
   return (uint8_t)(png_ptr? png_ptr->rgb_to_gray_status : 0);
}
#endif

#if defined(PNG_USER_CHUNKS_SUPPORTED)
png_voidp PNGAPI
png_get_user_chunk_ptr(png_structp png_ptr)
{
   return (png_ptr? png_ptr->user_chunk_ptr : NULL);
}
#endif

#ifdef PNG_WRITE_SUPPORTED
uint32_t PNGAPI
png_get_compression_buffer_size(png_structp png_ptr)
{
   return (uint32_t)(png_ptr? png_ptr->zbuf_size : 0L);
}
#endif

#ifdef PNG_SET_USER_LIMITS_SUPPORTED
/* these functions were added to libpng 1.2.6 */
uint32_t PNGAPI
png_get_user_width_max (png_structp png_ptr)
{
    return (png_ptr? png_ptr->user_width_max : 0);
}
uint32_t PNGAPI
png_get_user_height_max (png_structp png_ptr)
{
    return (png_ptr? png_ptr->user_height_max : 0);
}
#endif /* ?PNG_SET_USER_LIMITS_SUPPORTED */

#endif /* PNG_READ_SUPPORTED || PNG_WRITE_SUPPORTED */
