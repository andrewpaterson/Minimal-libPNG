
/* pngtrans.c - transforms the data in a row (used by both readers and writers)
 *
 * Last changed in libpng 1.2.13 November 13, 2006
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2006 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 */

#define PNG_INTERNAL
#include "png.h"

#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)
#if defined(PNG_READ_BGR_SUPPORTED) || defined(PNG_WRITE_BGR_SUPPORTED)
/* turn on BGR-to-RGB mapping */
void
png_set_bgr(png_structp png_ptr)
{
   png_debug(1, "in png_set_bgr\n");
   if(png_ptr == NULL) return;
   png_ptr->transformations |= PNG_BGR;
}
#endif

#if defined(PNG_READ_SWAP_SUPPORTED) || defined(PNG_WRITE_SWAP_SUPPORTED)
/* turn on 16 bit byte swapping */
void
png_set_swap(png_structp png_ptr)
{
   png_debug(1, "in png_set_swap\n");
   if(png_ptr == NULL) return;
   if (png_ptr->bit_depth == 16)
      png_ptr->transformations |= PNG_SWAP_BYTES;
}
#endif

#if defined(PNG_READ_PACK_SUPPORTED) || defined(PNG_WRITE_PACK_SUPPORTED)
/* turn on pixel packing */
void
png_set_packing(png_structp png_ptr)
{
   png_debug(1, "in png_set_packing\n");
   if(png_ptr == NULL) return;
   if (png_ptr->bit_depth < 8)
   {
      png_ptr->transformations |= PNG_PACK;
      png_ptr->usr_bit_depth = 8;
   }
}
#endif

#if defined(PNG_READ_SHIFT_SUPPORTED) || defined(PNG_WRITE_SHIFT_SUPPORTED)
void
png_set_shift(png_structp png_ptr, png_color_8p true_bits)
{
   png_debug(1, "in png_set_shift\n");
   if(png_ptr == NULL) return;
   png_ptr->transformations |= PNG_SHIFT;
   png_ptr->shift = *true_bits;
}
#endif

#if defined(PNG_READ_INTERLACING_SUPPORTED) || \
    defined(PNG_WRITE_INTERLACING_SUPPORTED)
int
png_set_interlace_handling(png_structp png_ptr)
{
   png_debug(1, "in png_set_interlace handling\n");
   if (png_ptr && png_ptr->interlaced)
   {
      png_ptr->transformations |= PNG_INTERLACE;
      return (7);
   }

   return (1);
}
#endif

#if defined(PNG_READ_FILLER_SUPPORTED) || defined(PNG_WRITE_FILLER_SUPPORTED)
/* Add a filler byte on read, or remove a filler or alpha byte on write.
 * The filler type has changed in v0.95 to allow future 2-byte fillers
 * for 48-bit input data, as well as to avoid problems with some compilers
 * that don't like bytes as parameters.
 */
void
png_set_filler(png_structp png_ptr, uint32_t filler, int filler_loc)
{
   png_debug(1, "in png_set_filler\n");
   if(png_ptr == NULL) return;
   png_ptr->transformations |= PNG_FILLER;
   png_ptr->filler = (uint8_t)filler;
   if (filler_loc == PNG_FILLER_AFTER)
      png_ptr->flags |= PNG_FLAG_FILLER_AFTER;
   else
      png_ptr->flags &= ~PNG_FLAG_FILLER_AFTER;

   /* This should probably go in the "do_read_filler" routine.
    * I attempted to do that in libpng-1.0.1a but that caused problems
    * so I restored it in libpng-1.0.2a
   */

   if (png_ptr->color_type == PNG_COLOR_TYPE_RGB)
   {
      png_ptr->usr_channels = 4;
   }

   /* Also I added this in libpng-1.0.2a (what happens when we expand
    * a less-than-8-bit grayscale to GA? */

   if (png_ptr->color_type == PNG_COLOR_TYPE_GRAY && png_ptr->bit_depth >= 8)
   {
      png_ptr->usr_channels = 2;
   }
}

#if !defined(PNG_1_0_X)
/* Added to libpng-1.2.7 */
void
png_set_add_alpha(png_structp png_ptr, uint32_t filler, int filler_loc)
{
   png_debug(1, "in png_set_add_alpha\n");
   if(png_ptr == NULL) return;
   png_set_filler(png_ptr, filler, filler_loc);
   png_ptr->transformations |= PNG_ADD_ALPHA;
}
#endif

#endif

#if defined(PNG_READ_SWAP_ALPHA_SUPPORTED) || \
    defined(PNG_WRITE_SWAP_ALPHA_SUPPORTED)
void
png_set_swap_alpha(png_structp png_ptr)
{
   png_debug(1, "in png_set_swap_alpha\n");
   if(png_ptr == NULL) return;
   png_ptr->transformations |= PNG_SWAP_ALPHA;
}
#endif

#if defined(PNG_READ_INVERT_ALPHA_SUPPORTED) || \
    defined(PNG_WRITE_INVERT_ALPHA_SUPPORTED)
void
png_set_invert_alpha(png_structp png_ptr)
{
   png_debug(1, "in png_set_invert_alpha\n");
   if(png_ptr == NULL) return;
   png_ptr->transformations |= PNG_INVERT_ALPHA;
}
#endif

#if defined(PNG_READ_INVERT_SUPPORTED) || defined(PNG_WRITE_INVERT_SUPPORTED)
void
png_set_invert_mono(png_structp png_ptr)
{
   png_debug(1, "in png_set_invert_mono\n");
   if(png_ptr == NULL) return;
   png_ptr->transformations |= PNG_INVERT_MONO;
}

/* invert monochrome grayscale data */
void png_do_invert(png_row_infop row_info, uint8_t* row)
{
   png_debug(1, "in png_do_invert\n");
  /* This test removed from libpng version 1.0.13 and 1.2.0:
   *   if (row_info->bit_depth == 1 &&
   */
#if defined(PNG_USELESS_TESTS_SUPPORTED)
   if (row == NULL || row_info == NULL)
     return;
#endif
   if (row_info->color_type == PNG_COLOR_TYPE_GRAY)
   {
      uint8_t* rp = row;
      uint32_t i;
      uint32_t istop = row_info->rowbytes;

      for (i = 0; i < istop; i++)
      {
         *rp = (uint8_t)(~(*rp));
         rp++;
      }
   }
   else if (row_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA &&
      row_info->bit_depth == 8)
   {
      uint8_t* rp = row;
      uint32_t i;
      uint32_t istop = row_info->rowbytes;

      for (i = 0; i < istop; i+=2)
      {
         *rp = (uint8_t)(~(*rp));
         rp+=2;
      }
   }
   else if (row_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA &&
      row_info->bit_depth == 16)
   {
      uint8_t* rp = row;
      uint32_t i;
      uint32_t istop = row_info->rowbytes;

      for (i = 0; i < istop; i+=4)
      {
         *rp = (uint8_t)(~(*rp));
         *(rp+1) = (uint8_t)(~(*(rp+1)));
         rp+=4;
      }
   }
}
#endif

#if defined(PNG_READ_SWAP_SUPPORTED) || defined(PNG_WRITE_SWAP_SUPPORTED)
/* swaps byte order on 16 bit depth images */
void png_do_swap(png_row_infop row_info, uint8_t* row)
{
   png_debug(1, "in png_do_swap\n");
   if (
#if defined(PNG_USELESS_TESTS_SUPPORTED)
       row != NULL && row_info != NULL &&
#endif
       row_info->bit_depth == 16)
   {
      uint8_t* rp = row;
      uint32_t i;
      uint32_t istop= row_info->width * row_info->channels;

      for (i = 0; i < istop; i++, rp += 2)
      {
         uint8_t t = *rp;
         *rp = *(rp + 1);
         *(rp + 1) = t;
      }
   }
}
#endif

#if defined(PNG_WRITE_FILLER_SUPPORTED) || \
    defined(PNG_READ_STRIP_ALPHA_SUPPORTED)
/* remove filler or alpha byte(s) */
void png_do_strip_filler(png_row_infop row_info, uint8_t* row, uint32_t flags)
{
   png_debug(1, "in png_do_strip_filler\n");
#if defined(PNG_USELESS_TESTS_SUPPORTED)
   if (row != NULL && row_info != NULL)
#endif
   {
      uint8_t* sp=row;
      uint8_t* dp=row;
      uint32_t row_width=row_info->width;
      uint32_t i;

      if ((row_info->color_type == PNG_COLOR_TYPE_RGB ||
         (row_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA &&
         (flags & PNG_FLAG_STRIP_ALPHA))) &&
         row_info->channels == 4)
      {
         if (row_info->bit_depth == 8)
         {
            /* This converts from RGBX or RGBA to RGB */
            if (flags & PNG_FLAG_FILLER_AFTER)
            {
               dp+=3; sp+=4;
               for (i = 1; i < row_width; i++)
               {
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  sp++;
               }
            }
            /* This converts from XRGB or ARGB to RGB */
            else
            {
               for (i = 0; i < row_width; i++)
               {
                  sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
               }
            }
            row_info->pixel_depth = 24;
            row_info->rowbytes = row_width * 3;
         }
         else /* if (row_info->bit_depth == 16) */
         {
            if (flags & PNG_FLAG_FILLER_AFTER)
            {
               /* This converts from RRGGBBXX or RRGGBBAA to RRGGBB */
               sp += 8; dp += 6;
               for (i = 1; i < row_width; i++)
               {
                  /* This could be (although png_memcpy is probably slower):
                  png_memcpy(dp, sp, 6);
                  sp += 8;
                  dp += 6;
                  */

                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  sp += 2;
               }
            }
            else
            {
               /* This converts from XXRRGGBB or AARRGGBB to RRGGBB */
               for (i = 0; i < row_width; i++)
               {
                  /* This could be (although png_memcpy is probably slower):
                  png_memcpy(dp, sp, 6);
                  sp += 8;
                  dp += 6;
                  */

                  sp+=2;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
               }
            }
            row_info->pixel_depth = 48;
            row_info->rowbytes = row_width * 6;
         }
         row_info->channels = 3;
      }
      else if ((row_info->color_type == PNG_COLOR_TYPE_GRAY ||
         (row_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA &&
         (flags & PNG_FLAG_STRIP_ALPHA))) &&
          row_info->channels == 2)
      {
         if (row_info->bit_depth == 8)
         {
            /* This converts from GX or GA to G */
            if (flags & PNG_FLAG_FILLER_AFTER)
            {
               for (i = 0; i < row_width; i++)
               {
                  *dp++ = *sp++;
                  sp++;
               }
            }
            /* This converts from XG or AG to G */
            else
            {
               for (i = 0; i < row_width; i++)
               {
                  sp++;
                  *dp++ = *sp++;
               }
            }
            row_info->pixel_depth = 8;
            row_info->rowbytes = row_width;
         }
         else /* if (row_info->bit_depth == 16) */
         {
            if (flags & PNG_FLAG_FILLER_AFTER)
            {
               /* This converts from GGXX or GGAA to GG */
               sp += 4; dp += 2;
               for (i = 1; i < row_width; i++)
               {
                  *dp++ = *sp++;
                  *dp++ = *sp++;
                  sp += 2;
               }
            }
            else
            {
               /* This converts from XXGG or AAGG to GG */
               for (i = 0; i < row_width; i++)
               {
                  sp += 2;
                  *dp++ = *sp++;
                  *dp++ = *sp++;
               }
            }
            row_info->pixel_depth = 16;
            row_info->rowbytes = row_width * 2;
         }
         row_info->channels = 1;
      }
      if (flags & PNG_FLAG_STRIP_ALPHA)
        row_info->color_type &= ~PNG_COLOR_MASK_ALPHA;
   }
}
#endif

#if defined(PNG_READ_BGR_SUPPORTED) || defined(PNG_WRITE_BGR_SUPPORTED)
/* swaps red and blue bytes within a pixel */
void png_do_bgr(png_row_infop row_info, uint8_t* row)
{
   png_debug(1, "in png_do_bgr\n");
   if (
#if defined(PNG_USELESS_TESTS_SUPPORTED)
       row != NULL && row_info != NULL &&
#endif
       (row_info->color_type & PNG_COLOR_MASK_COLOR))
   {
      uint32_t row_width = row_info->width;
      if (row_info->bit_depth == 8)
      {
         if (row_info->color_type == PNG_COLOR_TYPE_RGB)
         {
            uint8_t* rp;
            uint32_t i;

            for (i = 0, rp = row; i < row_width; i++, rp += 3)
            {
               uint8_t save = *rp;
               *rp = *(rp + 2);
               *(rp + 2) = save;
            }
         }
         else if (row_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
         {
            uint8_t* rp;
            uint32_t i;

            for (i = 0, rp = row; i < row_width; i++, rp += 4)
            {
               uint8_t save = *rp;
               *rp = *(rp + 2);
               *(rp + 2) = save;
            }
         }
      }
      else if (row_info->bit_depth == 16)
      {
         if (row_info->color_type == PNG_COLOR_TYPE_RGB)
         {
            uint8_t* rp;
            uint32_t i;

            for (i = 0, rp = row; i < row_width; i++, rp += 6)
            {
               uint8_t save = *rp;
               *rp = *(rp + 4);
               *(rp + 4) = save;
               save = *(rp + 1);
               *(rp + 1) = *(rp + 5);
               *(rp + 5) = save;
            }
         }
         else if (row_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
         {
            uint8_t* rp;
            uint32_t i;

            for (i = 0, rp = row; i < row_width; i++, rp += 8)
            {
               uint8_t save = *rp;
               *rp = *(rp + 4);
               *(rp + 4) = save;
               save = *(rp + 1);
               *(rp + 1) = *(rp + 5);
               *(rp + 5) = save;
            }
         }
      }
   }
}
#endif /* PNG_READ_BGR_SUPPORTED or PNG_WRITE_BGR_SUPPORTED */

#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED) || \
    defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED) || \
    defined(PNG_LEGACY_SUPPORTED)
void
png_set_user_transform_info(png_structp png_ptr, void*
   user_transform_ptr, int user_transform_depth, int user_transform_channels)
{
   png_debug(1, "in png_set_user_transform_info\n");
   if(png_ptr == NULL) return;
#if defined(PNG_USER_TRANSFORM_PTR_SUPPORTED)
   png_ptr->user_transform_ptr = user_transform_ptr;
   png_ptr->user_transform_depth = (uint8_t)user_transform_depth;
   png_ptr->user_transform_channels = (uint8_t)user_transform_channels;
#else
   if(user_transform_ptr || user_transform_depth || user_transform_channels)
      png_warning(png_ptr,
        "This version of libpng does not support user transform info");
#endif
}
#endif

/* This function returns a pointer to the user_transform_ptr associated with
 * the user transform functions.  The application should free any memory
 * associated with this pointer before png_write_destroy and png_read_destroy
 * are called.
 */
void*
png_get_user_transform_ptr(png_structp png_ptr)
{
   if (png_ptr == NULL) return (NULL);
#if defined(PNG_USER_TRANSFORM_PTR_SUPPORTED)
   return ((void*)png_ptr->user_transform_ptr);
#endif
   return (NULL);
}
#endif /* PNG_READ_SUPPORTED || PNG_WRITE_SUPPORTED */
