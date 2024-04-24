
/* pngset.c - storage of image information into info struct
 *
 * Last changed in libpng 1.2.15 January 5, 2007
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2007 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * The functions here are used during reads to store data from the file
 * into the info struct, and during writes to store application data
 * into the info struct for writing into the file.  This abstracts the
 * info struct and allows us to change the structure in the future.
 */

#define PNG_INTERNAL
#include "png.h"

#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)

#if defined(PNG_bKGD_SUPPORTED)
void PNGAPI
png_set_bKGD(png_structp png_ptr, png_infop info_ptr, png_color_16p background)
{
   png_debug1(1, "in %s storage function\n", "bKGD");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   png_memcpy(&(info_ptr->background), background, png_sizeof(png_color_16));
   info_ptr->valid |= PNG_INFO_bKGD;
}
#endif

#if defined(PNG_gAMA_SUPPORTED)
#ifdef PNG_FLOATING_POINT_SUPPORTED
void PNGAPI
png_set_gAMA(png_structp png_ptr, png_infop info_ptr, double file_gamma)
{
   double gamma;
   png_debug1(1, "in %s storage function\n", "gAMA");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   /* Check for overflow */
   if (file_gamma > 21474.83)
   {
      png_warning(png_ptr, "Limiting gamma to 21474.83");
      gamma=21474.83;
   }
   else
      gamma=file_gamma;
   info_ptr->gamma = (float)gamma;
#ifdef PNG_FIXED_POINT_SUPPORTED
   info_ptr->int_gamma = (int)(gamma*100000.+.5);
#endif
   info_ptr->valid |= PNG_INFO_gAMA;
   if(gamma == 0.0)
      png_warning(png_ptr, "Setting gamma=0");
}
#endif
void PNGAPI
png_set_gAMA_fixed(png_structp png_ptr, png_infop info_ptr, png_fixed_point
   int_gamma)
{
   png_fixed_point gamma;

   png_debug1(1, "in %s storage function\n", "gAMA");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   if (int_gamma > (png_fixed_point) PNG_UINT_31_MAX)
   {
     png_warning(png_ptr, "Limiting gamma to 21474.83");
     gamma=PNG_UINT_31_MAX;
   }
   else
   {
     if (int_gamma < 0)
     {
       png_warning(png_ptr, "Setting negative gamma to zero");
       gamma=0;
     }
     else
       gamma=int_gamma;
   }
#ifdef PNG_FLOATING_POINT_SUPPORTED
   info_ptr->gamma = (float)(gamma/100000.);
#endif
#ifdef PNG_FIXED_POINT_SUPPORTED
   info_ptr->int_gamma = gamma;
#endif
   info_ptr->valid |= PNG_INFO_gAMA;
   if(gamma == 0)
      png_warning(png_ptr, "Setting gamma=0");
}
#endif

#if defined(PNG_hIST_SUPPORTED)
void PNGAPI
png_set_hIST(png_structp png_ptr, png_infop info_ptr, png_uint_16p hist)
{
   int i;

   png_debug1(1, "in %s storage function\n", "hIST");
   if (png_ptr == NULL || info_ptr == NULL)
      return;
   if (info_ptr->num_palette <= 0 || info_ptr->num_palette
       > PNG_MAX_PALETTE_LENGTH)
   {
       png_warning(png_ptr,
          "Invalid palette size, hIST allocation skipped.");
       return;
   }

#ifdef PNG_FREE_ME_SUPPORTED
   png_free_data(png_ptr, info_ptr, PNG_FREE_HIST, 0);
#endif
   /* Changed from info->num_palette to PNG_MAX_PALETTE_LENGTH in version
      1.2.1 */
   png_ptr->hist = (png_uint_16p)png_malloc_warn(png_ptr,
      (png_uint_32)(PNG_MAX_PALETTE_LENGTH * png_sizeof (png_uint_16)));
   if (png_ptr->hist == NULL)
     {
       png_warning(png_ptr, "Insufficient memory for hIST chunk data.");
       return;
     }

   for (i = 0; i < info_ptr->num_palette; i++)
       png_ptr->hist[i] = hist[i];
   info_ptr->hist = png_ptr->hist;
   info_ptr->valid |= PNG_INFO_hIST;

#ifdef PNG_FREE_ME_SUPPORTED
   info_ptr->free_me |= PNG_FREE_HIST;
#else
   png_ptr->flags |= PNG_FLAG_FREE_HIST;
#endif
}
#endif

void PNGAPI
png_set_IHDR(png_structp png_ptr, png_infop info_ptr,
   png_uint_32 width, png_uint_32 height, int bit_depth,
   int color_type, int interlace_type, int compression_type,
   int filter_type)
{
   png_debug1(1, "in %s storage function\n", "IHDR");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   /* check for width and height valid values */
   if (width == 0 || height == 0)
      png_error(png_ptr, "Image width or height is zero in IHDR");
#ifdef PNG_SET_USER_LIMITS_SUPPORTED
   if (width > png_ptr->user_width_max || height > png_ptr->user_height_max)
      png_error(png_ptr, "image size exceeds user limits in IHDR");
#else
   if (width > PNG_USER_WIDTH_MAX || height > PNG_USER_HEIGHT_MAX)
      png_error(png_ptr, "image size exceeds user limits in IHDR");
#endif
   if (width > PNG_UINT_31_MAX || height > PNG_UINT_31_MAX)
      png_error(png_ptr, "Invalid image size in IHDR");
   if ( width > (PNG_UINT_32_MAX
                 >> 3)      /* 8-byte RGBA pixels */
                 - 64       /* bigrowbuf hack */
                 - 1        /* filter byte */
                 - 7*8      /* rounding of width to multiple of 8 pixels */
                 - 8)       /* extra max_pixel_depth pad */
      png_warning(png_ptr, "Width is too large for libpng to process pixels");

   /* check other values */
   if (bit_depth != 1 && bit_depth != 2 && bit_depth != 4 &&
      bit_depth != 8 && bit_depth != 16)
      png_error(png_ptr, "Invalid bit depth in IHDR");

   if (color_type < 0 || color_type == 1 ||
      color_type == 5 || color_type > 6)
      png_error(png_ptr, "Invalid color type in IHDR");

   if (((color_type == PNG_COLOR_TYPE_PALETTE) && bit_depth > 8) ||
       ((color_type == PNG_COLOR_TYPE_RGB ||
         color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
         color_type == PNG_COLOR_TYPE_RGB_ALPHA) && bit_depth < 8))
      png_error(png_ptr, "Invalid color type/bit depth combination in IHDR");

   if (interlace_type >= PNG_INTERLACE_LAST)
      png_error(png_ptr, "Unknown interlace method in IHDR");

   if (compression_type != PNG_COMPRESSION_TYPE_BASE)
      png_error(png_ptr, "Unknown compression method in IHDR");

#if defined(PNG_MNG_FEATURES_SUPPORTED)
   /* Accept filter_method 64 (intrapixel differencing) only if
    * 1. Libpng was compiled with PNG_MNG_FEATURES_SUPPORTED and
    * 2. Libpng did not read a PNG signature (this filter_method is only
    *    used in PNG datastreams that are embedded in MNG datastreams) and
    * 3. The application called png_permit_mng_features with a mask that
    *    included PNG_FLAG_MNG_FILTER_64 and
    * 4. The filter_method is 64 and
    * 5. The color_type is RGB or RGBA
    */
   if((png_ptr->mode&PNG_HAVE_PNG_SIGNATURE)&&png_ptr->mng_features_permitted)
      png_warning(png_ptr,"MNG features are not allowed in a PNG datastream");
   if(filter_type != PNG_FILTER_TYPE_BASE)
   {
     if(!((png_ptr->mng_features_permitted & PNG_FLAG_MNG_FILTER_64) &&
        (filter_type == PNG_INTRAPIXEL_DIFFERENCING) &&
        ((png_ptr->mode&PNG_HAVE_PNG_SIGNATURE) == 0) &&
        (color_type == PNG_COLOR_TYPE_RGB ||
         color_type == PNG_COLOR_TYPE_RGB_ALPHA)))
        png_error(png_ptr, "Unknown filter method in IHDR");
     if(png_ptr->mode&PNG_HAVE_PNG_SIGNATURE)
        png_warning(png_ptr, "Invalid filter method in IHDR");
   }
#else
   if(filter_type != PNG_FILTER_TYPE_BASE)
      png_error(png_ptr, "Unknown filter method in IHDR");
#endif

   info_ptr->width = width;
   info_ptr->height = height;
   info_ptr->bit_depth = (png_byte)bit_depth;
   info_ptr->color_type =(png_byte) color_type;
   info_ptr->compression_type = (png_byte)compression_type;
   info_ptr->filter_type = (png_byte)filter_type;
   info_ptr->interlace_type = (png_byte)interlace_type;
   if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
      info_ptr->channels = 1;
   else if (info_ptr->color_type & PNG_COLOR_MASK_COLOR)
      info_ptr->channels = 3;
   else
      info_ptr->channels = 1;
   if (info_ptr->color_type & PNG_COLOR_MASK_ALPHA)
      info_ptr->channels++;
   info_ptr->pixel_depth = (png_byte)(info_ptr->channels * info_ptr->bit_depth);

   /* check for potential overflow */
   if ( width > (PNG_UINT_32_MAX
                 >> 3)      /* 8-byte RGBA pixels */
                 - 64       /* bigrowbuf hack */
                 - 1        /* filter byte */
                 - 7*8      /* rounding of width to multiple of 8 pixels */
                 - 8)       /* extra max_pixel_depth pad */
      info_ptr->rowbytes = (png_size_t)0;
   else
      info_ptr->rowbytes = PNG_ROWBYTES(info_ptr->pixel_depth,width);
}

#if defined(PNG_pCAL_SUPPORTED)
void PNGAPI
png_set_pCAL(png_structp png_ptr, png_infop info_ptr,
   png_charp purpose, png_int_32 X0, png_int_32 X1, int type, int nparams,
   png_charp units, png_charpp params)
{
   png_uint_32 length;
   int i;

   png_debug1(1, "in %s storage function\n", "pCAL");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   length = (png_uint_32)png_strlen(purpose) + 1;
   png_debug1(3, "allocating purpose for info (%lu bytes)\n", length);
   info_ptr->pcal_purpose = (png_charp)png_malloc_warn(png_ptr, length);
   if (info_ptr->pcal_purpose == NULL)
     {
       png_warning(png_ptr, "Insufficient memory for pCAL purpose.");
       return;
     }
   png_memcpy(info_ptr->pcal_purpose, purpose, (png_size_t)length);

   png_debug(3, "storing X0, X1, type, and nparams in info\n");
   info_ptr->pcal_X0 = X0;
   info_ptr->pcal_X1 = X1;
   info_ptr->pcal_type = (png_byte)type;
   info_ptr->pcal_nparams = (png_byte)nparams;

   length = (png_uint_32)png_strlen(units) + 1;
   png_debug1(3, "allocating units for info (%lu bytes)\n", length);
   info_ptr->pcal_units = (png_charp)png_malloc_warn(png_ptr, length);
   if (info_ptr->pcal_units == NULL)
     {
       png_warning(png_ptr, "Insufficient memory for pCAL units.");
       return;
     }
   png_memcpy(info_ptr->pcal_units, units, (png_size_t)length);

   info_ptr->pcal_params = (png_charpp)png_malloc_warn(png_ptr,
      (png_uint_32)((nparams + 1) * png_sizeof(png_charp)));
   if (info_ptr->pcal_params == NULL)
     {
       png_warning(png_ptr, "Insufficient memory for pCAL params.");
       return;
     }

   info_ptr->pcal_params[nparams] = NULL;

   for (i = 0; i < nparams; i++)
   {
      length = (png_uint_32)png_strlen(params[i]) + 1;
      png_debug2(3, "allocating parameter %d for info (%lu bytes)\n", i, length);
      info_ptr->pcal_params[i] = (png_charp)png_malloc_warn(png_ptr, length);
      if (info_ptr->pcal_params[i] == NULL)
        {
          png_warning(png_ptr, "Insufficient memory for pCAL parameter.");
          return;
        }
      png_memcpy(info_ptr->pcal_params[i], params[i], (png_size_t)length);
   }

   info_ptr->valid |= PNG_INFO_pCAL;
#ifdef PNG_FREE_ME_SUPPORTED
   info_ptr->free_me |= PNG_FREE_PCAL;
#endif
}
#endif

#if defined(PNG_pHYs_SUPPORTED)
void PNGAPI
png_set_pHYs(png_structp png_ptr, png_infop info_ptr,
   png_uint_32 res_x, png_uint_32 res_y, int unit_type)
{
   png_debug1(1, "in %s storage function\n", "pHYs");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   info_ptr->x_pixels_per_unit = res_x;
   info_ptr->y_pixels_per_unit = res_y;
   info_ptr->phys_unit_type = (png_byte)unit_type;
   info_ptr->valid |= PNG_INFO_pHYs;
}
#endif

void PNGAPI
png_set_PLTE(png_structp png_ptr, png_infop info_ptr,
   png_colorp palette, int num_palette)
{

   png_debug1(1, "in %s storage function\n", "PLTE");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   if (num_palette < 0 || num_palette > PNG_MAX_PALETTE_LENGTH)
     {
       if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
         png_error(png_ptr, "Invalid palette length");
       else
       {
         png_warning(png_ptr, "Invalid palette length");
         return;
       }
     }

   /*
    * It may not actually be necessary to set png_ptr->palette here;
    * we do it for backward compatibility with the way the png_handle_tRNS
    * function used to do the allocation.
    */
#ifdef PNG_FREE_ME_SUPPORTED
   png_free_data(png_ptr, info_ptr, PNG_FREE_PLTE, 0);
#endif

   /* Changed in libpng-1.2.1 to allocate PNG_MAX_PALETTE_LENGTH instead
      of num_palette entries,
      in case of an invalid PNG file that has too-large sample values. */
   png_ptr->palette = (png_colorp)png_malloc(png_ptr,
      PNG_MAX_PALETTE_LENGTH * png_sizeof(png_color));
   png_memset(png_ptr->palette, 0, PNG_MAX_PALETTE_LENGTH *
      png_sizeof(png_color));
   png_memcpy(png_ptr->palette, palette, num_palette * png_sizeof (png_color));
   info_ptr->palette = png_ptr->palette;
   info_ptr->num_palette = png_ptr->num_palette = (png_uint_16)num_palette;

#ifdef PNG_FREE_ME_SUPPORTED
   info_ptr->free_me |= PNG_FREE_PLTE;
#else
   png_ptr->flags |= PNG_FLAG_FREE_PLTE;
#endif

   info_ptr->valid |= PNG_INFO_PLTE;
}

#if defined(PNG_sBIT_SUPPORTED)
void PNGAPI
png_set_sBIT(png_structp png_ptr, png_infop info_ptr,
   png_color_8p sig_bit)
{
   png_debug1(1, "in %s storage function\n", "sBIT");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   png_memcpy(&(info_ptr->sig_bit), sig_bit, png_sizeof (png_color_8));
   info_ptr->valid |= PNG_INFO_sBIT;
}
#endif

#if defined(PNG_sRGB_SUPPORTED)
void PNGAPI
png_set_sRGB(png_structp png_ptr, png_infop info_ptr, int intent)
{
   png_debug1(1, "in %s storage function\n", "sRGB");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   info_ptr->srgb_intent = (png_byte)intent;
   info_ptr->valid |= PNG_INFO_sRGB;
}

void PNGAPI
png_set_sRGB_gAMA_and_cHRM(png_structp png_ptr, png_infop info_ptr,
   int intent)
{
#if defined(PNG_gAMA_SUPPORTED)
#ifdef PNG_FLOATING_POINT_SUPPORTED
   float file_gamma;
#endif
#ifdef PNG_FIXED_POINT_SUPPORTED
   png_fixed_point int_file_gamma;
#endif
#endif
   png_debug1(1, "in %s storage function\n", "sRGB_gAMA_and_cHRM");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   png_set_sRGB(png_ptr, info_ptr, intent);

#if defined(PNG_gAMA_SUPPORTED)
#ifdef PNG_FLOATING_POINT_SUPPORTED
   file_gamma = (float).45455;
   png_set_gAMA(png_ptr, info_ptr, file_gamma);
#endif
#ifdef PNG_FIXED_POINT_SUPPORTED
   int_file_gamma = 45455L;
   png_set_gAMA_fixed(png_ptr, info_ptr, int_file_gamma);
#endif
#endif
}
#endif


#if defined(PNG_tIME_SUPPORTED)
void PNGAPI
png_set_tIME(png_structp png_ptr, png_infop info_ptr, png_timep mod_time)
{
   png_debug1(1, "in %s storage function\n", "tIME");
   if (png_ptr == NULL || info_ptr == NULL ||
       (png_ptr->mode & PNG_WROTE_tIME))
      return;

   png_memcpy(&(info_ptr->mod_time), mod_time, png_sizeof (png_time));
   info_ptr->valid |= PNG_INFO_tIME;
}
#endif

#if defined(PNG_tRNS_SUPPORTED)
void PNGAPI
png_set_tRNS(png_structp png_ptr, png_infop info_ptr,
   png_bytep trans, int num_trans, png_color_16p trans_values)
{
   png_debug1(1, "in %s storage function\n", "tRNS");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

   if (trans != NULL)
   {
       /*
        * It may not actually be necessary to set png_ptr->trans here;
        * we do it for backward compatibility with the way the png_handle_tRNS
        * function used to do the allocation.
        */
#ifdef PNG_FREE_ME_SUPPORTED
       png_free_data(png_ptr, info_ptr, PNG_FREE_TRNS, 0);
#endif
       /* Changed from num_trans to PNG_MAX_PALETTE_LENGTH in version 1.2.1 */
       png_ptr->trans = info_ptr->trans = (png_bytep)png_malloc(png_ptr,
           (png_uint_32)PNG_MAX_PALETTE_LENGTH);
       if (num_trans <= PNG_MAX_PALETTE_LENGTH)
         png_memcpy(info_ptr->trans, trans, (png_size_t)num_trans);
#ifdef PNG_FREE_ME_SUPPORTED
       info_ptr->free_me |= PNG_FREE_TRNS;
#else
       png_ptr->flags |= PNG_FLAG_FREE_TRNS;
#endif
   }

   if (trans_values != NULL)
   {
      png_memcpy(&(info_ptr->trans_values), trans_values,
         png_sizeof(png_color_16));
      if (num_trans == 0)
        num_trans = 1;
   }
   info_ptr->num_trans = (png_uint_16)num_trans;
   info_ptr->valid |= PNG_INFO_tRNS;
}
#endif

#if defined(PNG_sPLT_SUPPORTED)
void PNGAPI
png_set_sPLT(png_structp png_ptr,
             png_infop info_ptr, png_sPLT_tp entries, int nentries)
{
    png_sPLT_tp np;
    int i;

    if (png_ptr == NULL || info_ptr == NULL)
       return;

    np = (png_sPLT_tp)png_malloc_warn(png_ptr,
        (info_ptr->splt_palettes_num + nentries) * png_sizeof(png_sPLT_t));
    if (np == NULL)
    {
      png_warning(png_ptr, "No memory for sPLT palettes.");
      return;
    }

    png_memcpy(np, info_ptr->splt_palettes,
           info_ptr->splt_palettes_num * png_sizeof(png_sPLT_t));
    png_free(png_ptr, info_ptr->splt_palettes);
    info_ptr->splt_palettes=NULL;

    for (i = 0; i < nentries; i++)
    {
        png_sPLT_tp to = np + info_ptr->splt_palettes_num + i;
        png_sPLT_tp from = entries + i;

        to->name = (png_charp)png_malloc(png_ptr, (png_uint_32)png_strlen(from->name) + 1);
        /* TODO: use png_malloc_warn */
        png_strcpy(to->name, from->name);
        to->entries = (png_sPLT_entryp)png_malloc(png_ptr,
            from->nentries * png_sizeof(png_sPLT_entry));
        /* TODO: use png_malloc_warn */
        png_memcpy(to->entries, from->entries,
            from->nentries * png_sizeof(png_sPLT_entry));
        to->nentries = from->nentries;
        to->depth = from->depth;
    }

    info_ptr->splt_palettes = np;
    info_ptr->splt_palettes_num += nentries;
    info_ptr->valid |= PNG_INFO_sPLT;
#ifdef PNG_FREE_ME_SUPPORTED
    info_ptr->free_me |= PNG_FREE_SPLT;
#endif
}
#endif /* PNG_sPLT_SUPPORTED */

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
void PNGAPI
png_set_unknown_chunks(png_structp png_ptr,
   png_infop info_ptr, png_unknown_chunkp unknowns, int num_unknowns)
{
    png_unknown_chunkp np;
    int i;

    if (png_ptr == NULL || info_ptr == NULL || num_unknowns == 0)
        return;

    np = (png_unknown_chunkp)png_malloc_warn(png_ptr, (png_uint_32)((info_ptr->unknown_chunks_num + num_unknowns) * png_sizeof(png_unknown_chunk)));
    if (np == NULL)
    {
       png_warning(png_ptr, "Out of memory while processing unknown chunk.");
       return;
    }

    png_memcpy(np, info_ptr->unknown_chunks,
           info_ptr->unknown_chunks_num * png_sizeof(png_unknown_chunk));
    png_free(png_ptr, info_ptr->unknown_chunks);
    info_ptr->unknown_chunks=NULL;

    for (i = 0; i < num_unknowns; i++)
    {
        png_unknown_chunkp to = np + info_ptr->unknown_chunks_num + i;
        png_unknown_chunkp from = unknowns + i;

        png_strncpy((png_charp)to->name, (png_charp)from->name, 5);
        to->data = (png_bytep)png_malloc_warn(png_ptr, (png_uint_32)from->size);
        if (to->data == NULL)
        {
           png_warning(png_ptr, "Out of memory processing unknown chunk.");
        }
        else
        {
           png_memcpy(to->data, from->data, from->size);
           to->size = from->size;

           /* note our location in the read or write sequence */
           to->location = (png_byte)(png_ptr->mode & 0xff);
        }
    }

    info_ptr->unknown_chunks = np;
    info_ptr->unknown_chunks_num += num_unknowns;
#ifdef PNG_FREE_ME_SUPPORTED
    info_ptr->free_me |= PNG_FREE_UNKN;
#endif
}
void PNGAPI
png_set_unknown_chunk_location(png_structp png_ptr, png_infop info_ptr,
   int chunk, int location)
{
   if(png_ptr != NULL && info_ptr != NULL && chunk >= 0 && chunk <
         (int)info_ptr->unknown_chunks_num)
      info_ptr->unknown_chunks[chunk].location = (png_byte)location;
}
#endif

#if defined(PNG_1_0_X) || defined(PNG_1_2_X)
#if defined(PNG_READ_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED)
void PNGAPI
png_permit_empty_plte (png_structp png_ptr, int empty_plte_permitted)
{
   /* This function is deprecated in favor of png_permit_mng_features()
      and will be removed from libpng-1.3.0 */
   png_debug(1, "in png_permit_empty_plte, DEPRECATED.\n");
   if (png_ptr == NULL)
      return;
   png_ptr->mng_features_permitted = (png_byte)
     ((png_ptr->mng_features_permitted & (~(PNG_FLAG_MNG_EMPTY_PLTE))) |
     ((empty_plte_permitted & PNG_FLAG_MNG_EMPTY_PLTE)));
}
#endif
#endif

#if defined(PNG_MNG_FEATURES_SUPPORTED)
png_uint_32 PNGAPI
png_permit_mng_features (png_structp png_ptr, png_uint_32 mng_features)
{
   png_debug(1, "in png_permit_mng_features\n");
   if (png_ptr == NULL)
      return (png_uint_32)0;
   png_ptr->mng_features_permitted =
     (png_byte)(mng_features & PNG_ALL_MNG_FEATURES);
   return (png_uint_32)png_ptr->mng_features_permitted;
}
#endif

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
void PNGAPI
png_set_keep_unknown_chunks(png_structp png_ptr, int keep, png_bytep
   chunk_list, int num_chunks)
{
    png_bytep new_list, p;
    int i, old_num_chunks;
    if (png_ptr == NULL)
       return;
    if (num_chunks == 0)
    {
      if(keep == PNG_HANDLE_CHUNK_ALWAYS || keep == PNG_HANDLE_CHUNK_IF_SAFE)
        png_ptr->flags |= PNG_FLAG_KEEP_UNKNOWN_CHUNKS;
      else
        png_ptr->flags &= ~PNG_FLAG_KEEP_UNKNOWN_CHUNKS;

      if(keep == PNG_HANDLE_CHUNK_ALWAYS)
        png_ptr->flags |= PNG_FLAG_KEEP_UNSAFE_CHUNKS;
      else
        png_ptr->flags &= ~PNG_FLAG_KEEP_UNSAFE_CHUNKS;
      return;
    }
    if (chunk_list == NULL)
      return;
    old_num_chunks=png_ptr->num_chunk_list;
    new_list=(png_bytep)png_malloc(png_ptr,
       (png_uint_32)(5*(num_chunks+old_num_chunks)));
    if(png_ptr->chunk_list != NULL)
    {
       png_memcpy(new_list, png_ptr->chunk_list,
          (png_size_t)(5*old_num_chunks));
       png_free(png_ptr, png_ptr->chunk_list);
       png_ptr->chunk_list=NULL;
    }
    png_memcpy(new_list+5*old_num_chunks, chunk_list,
       (png_size_t)(5*num_chunks));
    for (p=new_list+5*old_num_chunks+4, i = 0; i<num_chunks; i++, p+=5)
       *p=(png_byte)keep;
    png_ptr->num_chunk_list=old_num_chunks+num_chunks;
    png_ptr->chunk_list=new_list;
#ifdef PNG_FREE_ME_SUPPORTED
    png_ptr->free_me |= PNG_FREE_LIST;
#endif
}
#endif

#if defined(PNG_READ_USER_CHUNKS_SUPPORTED)
void PNGAPI
png_set_read_user_chunk_fn(png_structp png_ptr, png_voidp user_chunk_ptr,
   png_user_chunk_ptr read_user_chunk_fn)
{
   png_debug(1, "in png_set_read_user_chunk_fn\n");
   if (png_ptr == NULL)
      return;
   png_ptr->read_user_chunk_fn = read_user_chunk_fn;
   png_ptr->user_chunk_ptr = user_chunk_ptr;
}
#endif

#if defined(PNG_INFO_IMAGE_SUPPORTED)
void PNGAPI
png_set_rows(png_structp png_ptr, png_infop info_ptr, png_bytepp row_pointers)
{
   png_debug1(1, "in %s storage function\n", "rows");

   if (png_ptr == NULL || info_ptr == NULL)
      return;

   if(info_ptr->row_pointers && (info_ptr->row_pointers != row_pointers))
      png_free_data(png_ptr, info_ptr, PNG_FREE_ROWS, 0);
   info_ptr->row_pointers = row_pointers;
   if(row_pointers)
      info_ptr->valid |= PNG_INFO_IDAT;
}
#endif

#ifdef PNG_WRITE_SUPPORTED
void PNGAPI
png_set_compression_buffer_size(png_structp png_ptr, png_uint_32 size)
{
    if (png_ptr == NULL)
       return;
    if(png_ptr->zbuf)
       png_free(png_ptr, png_ptr->zbuf);
    png_ptr->zbuf_size = (png_size_t)size;
    png_ptr->zbuf = (png_bytep)png_malloc(png_ptr, size);
    png_ptr->zstream.next_out = png_ptr->zbuf;
    png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
}
#endif

void PNGAPI
png_set_invalid(png_structp png_ptr, png_infop info_ptr, int mask)
{
   if (png_ptr && info_ptr)
      info_ptr->valid &= ~(mask);
}


#ifndef PNG_1_0_X
#ifdef PNG_SET_USER_LIMITS_SUPPORTED
/* this function was added to libpng 1.2.6 */
void PNGAPI
png_set_user_limits (png_structp png_ptr, png_uint_32 user_width_max,
    png_uint_32 user_height_max)
{
    /* Images with dimensions larger than these limits will be
     * rejected by png_set_IHDR().  To accept any PNG datastream
     * regardless of dimensions, set both limits to 0x7ffffffL.
     */
    if(png_ptr == NULL) return;
    png_ptr->user_width_max = user_width_max;
    png_ptr->user_height_max = user_height_max;
}
#endif /* ?PNG_SET_USER_LIMITS_SUPPORTED */

#endif /* ?PNG_1_0_X */
#endif /* PNG_READ_SUPPORTED || PNG_WRITE_SUPPORTED */
