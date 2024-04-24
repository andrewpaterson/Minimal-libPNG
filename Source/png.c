
/* png.c - location for general purpose libpng functions
 *
 * Last changed in libpng 1.2.15 January 5, 2007
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2007 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 */

#define PNG_INTERNAL
#define PNG_NO_EXTERN
#include "png.h"

/* Generate a compiler error if there is an old png.h in the search path. */
typedef version_1_2_16 Your_png_h_is_not_version_1_2_16;

/* Version information for C files.  This had better match the version
 * string defined in png.h.  */

#ifdef PNG_USE_GLOBAL_ARRAYS
/* png_libpng_ver was changed to a function in version 1.0.5c */
const char png_libpng_ver[18] = PNG_LIBPNG_VER_STRING;

#ifdef PNG_READ_SUPPORTED

/* png_sig was changed to a function in version 1.0.5c */
/* Place to hold the signature string for a PNG file. */
const uint8_t png_sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
#endif /* PNG_READ_SUPPORTED */

/* Invoke global declarations for constant strings for known chunk types */
PNG_IHDR;
PNG_IDAT;
PNG_IEND;
PNG_PLTE;
PNG_bKGD;
PNG_cHRM;
PNG_gAMA;
PNG_hIST;
PNG_iCCP;
PNG_iTXt;
PNG_oFFs;
PNG_pCAL;
PNG_sCAL;
PNG_pHYs;
PNG_sBIT;
PNG_sPLT;
PNG_sRGB;
PNG_tEXt;
PNG_tIME;
PNG_tRNS;
PNG_zTXt;

#ifdef PNG_READ_SUPPORTED
/* arrays to facilitate easy interlacing - use pass (0 - 6) as index */

/* start of interlace block */
const int png_pass_start[] = {0, 4, 0, 2, 0, 1, 0};

/* offset to next interlace block */
const int png_pass_inc[] = {8, 8, 4, 4, 2, 2, 1};

/* start of interlace block in the y direction */
const int png_pass_ystart[] = {0, 0, 4, 0, 2, 0, 1};

/* offset to next interlace block in the y direction */
const int png_pass_yinc[] = {8, 8, 8, 4, 4, 2, 2};

/* Height of interlace block.  This is not currently used - if you need
 * it, uncomment it here and in png.h
const int png_pass_height[] = {8, 8, 4, 4, 2, 2, 1};
*/

/* Mask to determine which pixels are valid in a pass */
const int png_pass_mask[] = {0x80, 0x08, 0x88, 0x22, 0xaa, 0x55, 0xff};

/* Mask to determine which pixels to overwrite while displaying */
const int png_pass_dsp_mask[]
   = {0xff, 0x0f, 0xff, 0x33, 0xff, 0x55, 0xff};

#endif /* PNG_READ_SUPPORTED */
#endif /* PNG_USE_GLOBAL_ARRAYS */

/* Tells libpng that we have already handled the first "num_bytes" bytes
 * of the PNG file signature.  If the PNG data is embedded into another
 * stream we can set num_bytes = 8 so that libpng will not attempt to read
 * or write any of the magic bytes before it starts on the IHDR.
 */

#ifdef PNG_READ_SUPPORTED
void
png_set_sig_bytes(png_structp png_ptr, int num_bytes)
{
   if(png_ptr == NULL) return;
   png_debug(1, "in png_set_sig_bytes\n");
   if (num_bytes > 8)
      png_error(png_ptr, "Too many bytes for PNG signature.");

   png_ptr->sig_bytes = (uint8_t)(num_bytes < 0 ? 0 : num_bytes);
}

/* Checks whether the supplied bytes match the PNG signature.  We allow
 * checking less than the full 8-byte signature so that those apps that
 * already read the first few bytes of a file to determine the file type
 * can simply check the remaining bytes for extra assurance.  Returns
 * an integer less than, equal to, or greater than zero if sig is found,
 * respectively, to be less than, to match, or be greater than the correct
 * PNG signature (this is the same behaviour as strcmp, memcmp, etc).
 */
int
png_sig_cmp(uint8_t* sig, size_t start, size_t num_to_check)
{
   uint8_t png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
   if (num_to_check > 8)
      num_to_check = 8;
   else if (num_to_check < 1)
      return (-1);

   if (start > 7)
      return (-1);

   if (start + num_to_check > 8)
      num_to_check = 8 - start;

   return ((int)(png_memcmp(&sig[start], &png_signature[start], num_to_check)));
}

#if defined(PNG_1_2_X)
/* (Obsolete) function to check signature bytes.  It does not allow one
 * to check a partial signature.  This function might be removed in the
 * future - use png_sig_cmp().  Returns true (nonzero) if the file is a PNG.
 */
int
png_check_sig(uint8_t* sig, int num)
{
  return ((int)!png_sig_cmp(sig, (size_t)0, (size_t)num));
}
#endif
#endif /* PNG_READ_SUPPORTED */

#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)
/* Function to allocate memory for zlib and clear it to 0. */
voidpf png_zalloc(voidpf png_ptr, uint32_t items, uint32_t size)
{
   void* ptr;
   png_structp p=(png_structp)png_ptr;
   uint32_t save_flags=p->flags;
   uint32_t num_bytes;

   if(png_ptr == NULL) return (NULL);
   if (items > PNG_UINT_32_MAX/size)
   {
     png_warning (p, "Potential overflow in png_zalloc()");
     return (NULL);
   }
   num_bytes = (uint32_t)items * size;

   p->flags|=PNG_FLAG_MALLOC_NULL_MEM_OK;
   ptr = (void*)png_malloc((png_structp)png_ptr, num_bytes);
   p->flags=save_flags;

   return ((voidpf)ptr);
}

/* function to free memory for zlib */
void png_zfree(voidpf png_ptr, voidpf ptr)
{
   png_free((png_structp)png_ptr, (void*)ptr);
}

/* Reset the CRC variable to 32 bits of 1's.  Care must be taken
 * in case CRC is > 32 bits to leave the top bits 0.
 */
void /* PRIVATE */
png_reset_crc(png_structp png_ptr)
{
   png_ptr->crc = crc32(0, Z_NULL, 0);
}

/* Calculate the CRC over a section of data.  We can only pass as
 * much data to this routine as the largest single buffer size.  We
 * also check that this data will actually be used before going to the
 * trouble of calculating it.
 */
void /* PRIVATE */
png_calculate_crc(png_structp png_ptr, uint8_t* ptr, size_t length)
{
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

   if (need_crc)
      png_ptr->crc = crc32(png_ptr->crc, ptr, (uint32_t)length);
}

/* Allocate the memory for an info_struct for the application.  We don't
 * really need the png_ptr, but it could potentially be useful in the
 * future.  This should be used in favour of malloc(sizeof(png_info))
 * and png_info_init() so that applications that want to use a shared
 * libpng don't have to be recompiled if png_info changes size.
 */
png_info*
png_create_info_struct(png_structp png_ptr)
{
   png_info* info_ptr;

   png_debug(1, "in png_create_info_struct\n");
   if(png_ptr == NULL) return (NULL);
#ifdef PNG_USER_MEM_SUPPORTED
   info_ptr = (png_info*)png_create_struct_2(PNG_STRUCT_INFO,
      png_ptr->malloc_fn, png_ptr->mem_ptr);
#else
   info_ptr = (png_info*)png_create_struct(PNG_STRUCT_INFO);
#endif
   if (info_ptr != NULL)
      png_info_init_3(&info_ptr, sizeof(png_info));

   return (info_ptr);
}

/* This function frees the memory associated with a single info struct.
 * Normally, one would use either png_destroy_read_struct() or
 * png_destroy_write_struct() to free an info struct, but this may be
 * useful for some applications.
 */
void
png_destroy_info_struct(png_structp png_ptr, png_info** info_ptr_ptr)
{
   png_info* info_ptr = NULL;
   if(png_ptr == NULL) return;

   png_debug(1, "in png_destroy_info_struct\n");
   if (info_ptr_ptr != NULL)
      info_ptr = *info_ptr_ptr;

   if (info_ptr != NULL)
   {
      png_info_destroy(png_ptr, info_ptr);

#ifdef PNG_USER_MEM_SUPPORTED
      png_destroy_struct_2((void*)info_ptr, png_ptr->free_fn,
          png_ptr->mem_ptr);
#else
      png_destroy_struct((void*)info_ptr);
#endif
      *info_ptr_ptr = NULL;
   }
}

/* Initialize the info structure.  This is now an internal function (0.89)
 * and applications using it are urged to use png_create_info_struct()
 * instead.
 */
#if defined(PNG_1_2_X)
#undef png_info_init
void png_info_init(png_info* info_ptr)
{
   /* We only come here via pre-1.0.12-compiled applications */
   png_info_init_3(&info_ptr, 0);
}
#endif

void
png_info_init_3(png_info** ptr_ptr, size_t png_info_struct_size)
{
   png_info* info_ptr = *ptr_ptr;

   if(info_ptr == NULL) return;

   png_debug(1, "in png_info_init_3\n");

   if(sizeof(png_info) > png_info_struct_size)
     {
       png_destroy_struct(info_ptr);
       info_ptr = (png_info*)png_create_struct(PNG_STRUCT_INFO);
       *ptr_ptr = info_ptr;
     }

   /* set everything to 0 */
   png_memset(info_ptr, 0, sizeof (png_info));
}

#ifdef PNG_FREE_ME_SUPPORTED
void
png_data_freer(png_structp png_ptr, png_info* info_ptr,
   int freer, uint32_t mask)
{
   png_debug(1, "in png_data_freer\n");
   if (png_ptr == NULL || info_ptr == NULL)
      return;
   if(freer == PNG_DESTROY_WILL_FREE_DATA)
      info_ptr->free_me |= mask;
   else if(freer == PNG_USER_WILL_FREE_DATA)
      info_ptr->free_me &= ~mask;
   else
      png_warning(png_ptr,
         "Unknown freer parameter in png_data_freer.");
}
#endif

void
png_free_data(png_structp png_ptr, png_info* info_ptr, uint32_t mask,
   int num)
{
   png_debug(1, "in png_free_data\n");
   if (png_ptr == NULL || info_ptr == NULL)
      return;

#if defined(PNG_tRNS_SUPPORTED)
/* free any tRNS entry */
#ifdef PNG_FREE_ME_SUPPORTED
if ((mask & PNG_FREE_TRNS) & info_ptr->free_me)
#else
if ((mask & PNG_FREE_TRNS) && (png_ptr->flags & PNG_FLAG_FREE_TRNS))
#endif
{
    png_free(png_ptr, info_ptr->trans);
    info_ptr->valid &= ~PNG_INFO_tRNS;
#ifndef PNG_FREE_ME_SUPPORTED
    png_ptr->flags &= ~PNG_FLAG_FREE_TRNS;
#endif
    info_ptr->trans = NULL;
}
#endif

#if defined(PNG_pCAL_SUPPORTED)
/* free any pCAL entry */
#ifdef PNG_FREE_ME_SUPPORTED
if ((mask & PNG_FREE_PCAL) & info_ptr->free_me)
#else
if (mask & PNG_FREE_PCAL)
#endif
{
    png_free(png_ptr, info_ptr->pcal_purpose);
    png_free(png_ptr, info_ptr->pcal_units);
    info_ptr->pcal_purpose = NULL;
    info_ptr->pcal_units = NULL;
    if (info_ptr->pcal_params != NULL)
    {
        int i;
        for (i = 0; i < (int)info_ptr->pcal_nparams; i++)
        {
          png_free(png_ptr, info_ptr->pcal_params[i]);
          info_ptr->pcal_params[i]=NULL;
        }
        png_free(png_ptr, info_ptr->pcal_params);
        info_ptr->pcal_params = NULL;
    }
    info_ptr->valid &= ~PNG_INFO_pCAL;
}
#endif


#if defined(PNG_sPLT_SUPPORTED)
/* free a given sPLT entry, or (if num == -1) all sPLT entries */
#ifdef PNG_FREE_ME_SUPPORTED
if ((mask & PNG_FREE_SPLT) & info_ptr->free_me)
#else
if (mask & PNG_FREE_SPLT)
#endif
{
   if (num != -1)
   {
      if(info_ptr->splt_palettes)
      {
          png_free(png_ptr, info_ptr->splt_palettes[num].name);
          png_free(png_ptr, info_ptr->splt_palettes[num].entries);
          info_ptr->splt_palettes[num].name = NULL;
          info_ptr->splt_palettes[num].entries = NULL;
      }
   }
   else
   {
       if(info_ptr->splt_palettes_num)
       {
         int i;
         for (i = 0; i < (int)info_ptr->splt_palettes_num; i++)
            png_free_data(png_ptr, info_ptr, PNG_FREE_SPLT, i);

         png_free(png_ptr, info_ptr->splt_palettes);
         info_ptr->splt_palettes = NULL;
         info_ptr->splt_palettes_num = 0;
       }
       info_ptr->valid &= ~PNG_INFO_sPLT;
   }
}
#endif

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
#ifdef PNG_FREE_ME_SUPPORTED
if ((mask & PNG_FREE_UNKN) & info_ptr->free_me)
#else
if (mask & PNG_FREE_UNKN)
#endif
{
   if (num != -1)
   {
       if(info_ptr->unknown_chunks)
       {
          png_free(png_ptr, info_ptr->unknown_chunks[num].data);
          info_ptr->unknown_chunks[num].data = NULL;
       }
   }
   else
   {
       int i;

       if(info_ptr->unknown_chunks_num)
       {
         for (i = 0; i < (int)info_ptr->unknown_chunks_num; i++)
            png_free_data(png_ptr, info_ptr, PNG_FREE_UNKN, i);

         png_free(png_ptr, info_ptr->unknown_chunks);
         info_ptr->unknown_chunks = NULL;
         info_ptr->unknown_chunks_num = 0;
       }
   }
}
#endif

#if defined(PNG_hIST_SUPPORTED)
/* free any hIST entry */
#ifdef PNG_FREE_ME_SUPPORTED
if ((mask & PNG_FREE_HIST)  & info_ptr->free_me)
#else
if ((mask & PNG_FREE_HIST) && (png_ptr->flags & PNG_FLAG_FREE_HIST))
#endif
{
    png_free(png_ptr, info_ptr->hist);
    info_ptr->hist = NULL;
    info_ptr->valid &= ~PNG_INFO_hIST;
#ifndef PNG_FREE_ME_SUPPORTED
    png_ptr->flags &= ~PNG_FLAG_FREE_HIST;
#endif
}
#endif

/* free any PLTE entry that was internally allocated */
#ifdef PNG_FREE_ME_SUPPORTED
if ((mask & PNG_FREE_PLTE) & info_ptr->free_me)
#else
if ((mask & PNG_FREE_PLTE) && (png_ptr->flags & PNG_FLAG_FREE_PLTE))
#endif
{
    png_zfree(png_ptr, info_ptr->palette);
    info_ptr->palette = NULL;
    info_ptr->valid &= ~PNG_INFO_PLTE;
#ifndef PNG_FREE_ME_SUPPORTED
    png_ptr->flags &= ~PNG_FLAG_FREE_PLTE;
#endif
    info_ptr->num_palette = 0;
}

#if defined(PNG_INFO_IMAGE_SUPPORTED)
/* free any image bits attached to the info structure */
#ifdef PNG_FREE_ME_SUPPORTED
if ((mask & PNG_FREE_ROWS) & info_ptr->free_me)
#else
if (mask & PNG_FREE_ROWS)
#endif
{
    if(info_ptr->row_pointers)
    {
       int row;
       for (row = 0; row < (int)info_ptr->height; row++)
       {
          png_free(png_ptr, info_ptr->row_pointers[row]);
          info_ptr->row_pointers[row]=NULL;
       }
       png_free(png_ptr, info_ptr->row_pointers);
       info_ptr->row_pointers=NULL;
    }
    info_ptr->valid &= ~PNG_INFO_IDAT;
}
#endif

#ifdef PNG_FREE_ME_SUPPORTED
   if(num == -1)
     info_ptr->free_me &= ~mask;
   else
     info_ptr->free_me &= ~(mask & ~PNG_FREE_MUL);
#endif
}

/* This is an internal routine to free any memory that the info struct is
 * pointing to before re-using it or freeing the struct itself.  Recall
 * that png_free() checks for NULL pointers for us.
 */
void /* PRIVATE */
png_info_destroy(png_structp png_ptr, png_info* info_ptr)
{
   png_debug(1, "in png_info_destroy\n");

   png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
   if (png_ptr->num_chunk_list)
   {
       png_free(png_ptr, png_ptr->chunk_list);
       png_ptr->chunk_list=NULL;
       png_ptr->num_chunk_list = 0;
   }
#endif

   png_info_init_3(&info_ptr, sizeof(png_info));
}
#endif /* defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED) */

/* This function returns a pointer to the io_ptr associated with the user
 * functions.  The application should free any memory associated with this
 * pointer before png_write_destroy() or png_read_destroy() are called.
 */
void*
png_get_io_ptr(png_structp png_ptr)
{
   if(png_ptr == NULL) return (NULL);
   return (png_ptr->io_ptr);
}

#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)
#if !defined(PNG_NO_STDIO)
/* Initialize the default input/output functions for the PNG file.  If you
 * use your own read or write routines, you can call either png_set_read_fn()
 * or png_set_write_fn() instead of png_init_io().  If you have defined
 * PNG_NO_STDIO, you must use a function of your own because "FILE *" isn't
 * necessarily available.
 */
void
png_init_io(png_structp png_ptr, FILE* fp)
{
   png_debug(1, "in png_init_io\n");
   if(png_ptr == NULL) return;
   png_ptr->io_ptr = (void*)fp;
}
#endif


#if 0
/* Signature string for a PNG file. */
uint8_t*
png_sig_bytes(void)
{
   return ((uint8_t*)"\211\120\116\107\015\012\032\012");
}
#endif
#endif /* defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED) */

char*
png_get_copyright(png_structp png_ptr)
{
   if (&png_ptr != NULL)  /* silence compiler warning about unused png_ptr */
   return ((char*) "\n libpng version 1.2.16 - January 31, 2007\n\
   Copyright (c) 1998-2007 Glenn Randers-Pehrson\n\
   Copyright (c) 1996-1997 Andreas Dilger\n\
   Copyright (c) 1995-1996 Guy Eric Schalnat, Group 42, Inc.\n");
   return ((char*) "");
}

/* The following return the library version as a short string in the
 * format 1.0.0 through 99.99.99zz.  To get the version of *.h files
 * used with your application, print out PNG_LIBPNG_VER_STRING, which
 * is defined in png.h.
 * Note: now there is no difference between png_get_libpng_ver() and
 * png_get_header_ver().  Due to the version_nn_nn_nn typedef guard,
 * it is guaranteed that png.c uses the correct version of png.h.
 */
char*
png_get_libpng_ver(png_structp png_ptr)
{
   /* Version of *.c files used when building libpng */
   if (&png_ptr != NULL)  /* silence compiler warning about unused png_ptr */
      return ((char*) PNG_LIBPNG_VER_STRING);
   return ((char*) "");
}

char*
png_get_header_ver(png_structp png_ptr)
{
   /* Version of *.h files used when building libpng */
   if (&png_ptr != NULL)  /* silence compiler warning about unused png_ptr */
      return ((char*) PNG_LIBPNG_VER_STRING);
   return ((char*) "");
}

char*
png_get_header_version(png_structp png_ptr)
{
   /* Returns longer string containing both version and date */
   if (&png_ptr != NULL)  /* silence compiler warning about unused png_ptr */
      return ((char*) PNG_HEADER_VERSION_STRING);
   return ((char*) "");
}

#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)
#ifdef PNG_HANDLE_AS_UNKNOWN_SUPPORTED
int
png_handle_as_unknown(png_structp png_ptr, uint8_t* chunk_name)
{
   /* check chunk_name and return "keep" value if it's on the list, else 0 */
   int i;
   uint8_t* p;
   if((png_ptr == NULL && chunk_name == NULL) || png_ptr->num_chunk_list <= 0)
      return 0;
   p=png_ptr->chunk_list+png_ptr->num_chunk_list*5-5;
   for (i = png_ptr->num_chunk_list; i; i--, p-=5)
      if (!png_memcmp(chunk_name, p, 4))
        return ((int)*(p+4));
   return 0;
}
#endif

/* This function, added to libpng-1.0.6g, is untested. */
int
png_reset_zstream(png_structp png_ptr)
{
   if (png_ptr == NULL) return Z_STREAM_ERROR;
   return (inflateReset(&png_ptr->zstream));
}
#endif /* defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED) */

/* This function was added to libpng-1.0.7 */
uint32_t
png_access_version_number(void)
{
   /* Version of *.c files used when building libpng */
   return((uint32_t) PNG_LIBPNG_VER);
}

#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)
#ifdef PNG_SIZE_T
/* Added at libpng version 1.2.6 */
   size_t png_convert_size ((size_t size));
size_t
png_convert_size(size_t size)
{
  if (size > (size_t)-1)
     PNG_ABORT();  /* We haven't got access to png_ptr, so no png_error() */
  return ((size_t)size);
}
#endif /* PNG_SIZE_T */
#endif /* defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED) */
