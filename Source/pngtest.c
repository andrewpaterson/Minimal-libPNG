
/* pngtest.c - a simple test program to test libpng
 *
 * Last changed in libpng 1.2.6 - August 15, 2004
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2004 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * This program reads in a PNG image, writes it out again, and then
 * compares the two files.  If the files are identical, this shows that
 * the basic chunk handling, filtering, and (de)compression code is working
 * properly.  It does not currently test all of the transforms, although
 * it probably should.
 *
 * The program will report "FAIL" in certain legitimate cases:
 * 1) when the compression level or filter selection method is changed.
 * 2) when the maximum IDAT size (PNG_ZBUF_SIZE in pngconf.h) is not 8192.
 * 3) unknown unsafe-to-copy ancillary chunks or unknown critical chunks
 *    exist in the input file.
 * 4) others not listed here...
 * In these cases, it is best to check with another tool such as "pngcheck"
 * to see what the differences between the two files are.
 *
 * If a filename is given on the command-line, then this file is used
 * for the input, rather than the default "pngtest.png".  This allows
 * testing a wide variety of files easily.  You can also test a number
 * of files at once by typing "pngtest -m file1.png file2.png ..."
 */

#include "png.h"

#  include <stdio.h>
#  include <stdlib.h>
#  define READFILE(file, data, length, check) \
     check=(size_t)fread(data,(size_t)1,length,file)
#  define WRITEFILE(file, data, length, check) \
     check=(size_t)fwrite(data,(size_t)1, length, file)
#  define FCLOSE(file) fclose(file)

/* Makes pngtest verbose so we can find problems (needs to be before png.h) */
#ifndef PNG_DEBUG
#  define PNG_DEBUG 0
#endif

#if !PNG_DEBUG
#  define SINGLE_ROWBUF_ALLOC  /* makes buffer overruns easier to nail */
#endif

/* Turn on CPU timing
#define PNGTEST_TIMING
*/

#ifdef PNG_NO_FLOATING_POINT_SUPPORTED
#undef PNGTEST_TIMING
#endif

#ifdef PNGTEST_TIMING
static float t_start, t_stop, t_decode, t_encode, t_misc;
#include <time.h>
#endif

static int verbose = 0;

int test_one_file(const char *inname, const char *outname);

/* defined so I can write to a file on gui/windowing platforms */
/*  #define STDERR stderr  */
#define STDERR stdout   /* for DOS */

/* example of using row callbacks to make a simple progress meter */
static int status_pass=1;
static int status_dots_requested=0;
static int status_dots=1;

void read_row_callback(png_structp png_ptr, uint32_t row_number, int pass);
void read_row_callback(png_structp png_ptr, uint32_t row_number, int pass)
{
    if(png_ptr == NULL || row_number > PNG_UINT_31_MAX) return;
    if(status_pass != pass)
    {
       fprintf(stdout,"\n Pass %d: ",pass);
       status_pass = pass;
       status_dots = 31;
    }
    status_dots--;
    if(status_dots == 0)
    {
       fprintf(stdout, "\n         ");
       status_dots=30;
    }
    fprintf(stdout, "r");
}

void write_row_callback(png_structp png_ptr, uint32_t row_number, int pass);
void write_row_callback(png_structp png_ptr, uint32_t row_number, int pass)
{
    if(png_ptr == NULL || row_number > PNG_UINT_31_MAX || pass > 7) return;
    fprintf(stdout, "w");
}


#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED)
/* Example of using user transform callback (we don't transform anything,
   but merely examine the row filters.  We set this to 256 rather than
   5 in case illegal filter values are present.) */
static uint32_t filters_used[256];
void count_filters(png_structp png_ptr, png_row_infop row_info, uint8_t* data);
void count_filters(png_structp png_ptr, png_row_infop row_info, uint8_t* data)
{
    if(png_ptr != NULL && row_info != NULL)
      ++filters_used[*(data-1)];
}
#endif

#if defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
/* example of using user transform callback (we don't transform anything,
   but merely count the zero samples) */

static uint32_t zero_samples;

void count_zero_samples(png_structp png_ptr, png_row_infop row_info, uint8_t* data);
void count_zero_samples(png_structp png_ptr, png_row_infop row_info, uint8_t* data)
{
   uint8_t* dp = data;
   if(png_ptr == NULL)return;

   /* contents of row_info:
    *  uint32_t width      width of row
    *  uint32_t rowbytes   number of bytes in row
    *  uint8_t color_type    color type of pixels
    *  uint8_t bit_depth     bit depth of samples
    *  uint8_t channels      number of channels (1-4)
    *  uint8_t pixel_depth   bits per pixel (depth*channels)
    */


    /* counts the number of zero samples (or zero pixels if color_type is 3 */

    if(row_info->color_type == 0 || row_info->color_type == 3)
    {
       int pos=0;
       uint32_t n, nstop;
       for (n=0, nstop=row_info->width; n<nstop; n++)
       {
          if(row_info->bit_depth == 1)
          {
             if(((*dp << pos++ ) & 0x80) == 0) zero_samples++;
             if(pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if(row_info->bit_depth == 2)
          {
             if(((*dp << (pos+=2)) & 0xc0) == 0) zero_samples++;
             if(pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if(row_info->bit_depth == 4)
          {
             if(((*dp << (pos+=4)) & 0xf0) == 0) zero_samples++;
             if(pos == 8)
             {
                pos = 0;
                dp++;
             }
          }
          if(row_info->bit_depth == 8)
             if(*dp++ == 0) zero_samples++;
          if(row_info->bit_depth == 16)
          {
             if((*dp | *(dp+1)) == 0) zero_samples++;
             dp+=2;
          }
       }
    }
    else /* other color types */
    {
       uint32_t n, nstop;
       int channel;
       int color_channels = row_info->channels;
       if(row_info->color_type > 3)color_channels--;

       for (n=0, nstop=row_info->width; n<nstop; n++)
       {
          for (channel = 0; channel < color_channels; channel++)
          {
             if(row_info->bit_depth == 8)
                if(*dp++ == 0) zero_samples++;
             if(row_info->bit_depth == 16)
             {
                if((*dp | *(dp+1)) == 0) zero_samples++;
                dp+=2;
             }
          }
          if(row_info->color_type > 3)
          {
             dp++;
             if(row_info->bit_depth == 16)dp++;
          }
       }
    }
}
#endif /* PNG_WRITE_USER_TRANSFORM_SUPPORTED */

static int wrote_question = 0;

/* START of code to validate memory allocation and deallocation */
#if defined(PNG_USER_MEM_SUPPORTED) && PNG_DEBUG

/* Allocate memory.  For reasonable files, size should never exceed
   64K.  However, zlib may allocate more then 64K if you don't tell
   it not to.  See zconf.h and png.h for more information.  zlib does
   need to allocate exactly 64K, so whatever you call here must
   have the ability to do that.

   This piece of code can be compiled to validate max 64K allocations
   by setting MAXSEG_64K in zlib zconf.h *or* PNG_MAX_MALLOC_64K. */
typedef struct memory_information
{
   uint32_t               size;
   void*                 pointer;
   struct memory_information *next;
} memory_information;
typedef memory_information *memory_infop;

static memory_infop pinformation = NULL;
static int current_allocation = 0;
static int maximum_allocation = 0;
static int total_allocation = 0;
static int num_allocations = 0;

void* png_debug_malloc ((png_structp png_ptr, uint32_t size));
void png_debug_free ((png_structp png_ptr, void* ptr));

void*
png_debug_malloc(png_structp png_ptr, uint32_t size)
{

   /* png_malloc has already tested for NULL; png_create_struct calls
      png_debug_malloc directly, with png_ptr == NULL which is OK */

   if (size == 0)
      return (NULL);

   /* This calls the library allocator twice, once to get the requested
      buffer and once to get a new free list entry. */
   {
      /* Disable malloc_fn and free_fn */
      memory_infop pinfo;
      png_set_mem_fn(png_ptr, NULL, NULL, NULL);
      pinfo = (memory_infop)png_malloc(png_ptr,
         (uint32_t)sizeof (*pinfo));
      pinfo->size = size;
      current_allocation += size;
      total_allocation += size;
      num_allocations ++;
      if (current_allocation > maximum_allocation)
         maximum_allocation = current_allocation;
      pinfo->pointer = (void*)png_malloc(png_ptr, size);
      /* Restore malloc_fn and free_fn */
      png_set_mem_fn(png_ptr, (void*)NULL, (png_malloc_ptr)png_debug_malloc,
         (png_free_ptr)png_debug_free);
      if (size != 0 && pinfo->pointer == NULL)
      {
         current_allocation -= size;
         total_allocation -= size;
         png_error(png_ptr,
           "out of memory in pngtest->png_debug_malloc.");
      }
      pinfo->next = pinformation;
      pinformation = pinfo;
      /* Make sure the caller isn't assuming zeroed memory. */
      memset(pinfo->pointer, 0xdd, pinfo->size);
      if(verbose)
         printf("png_malloc %lu bytes at %x\n",(uint32_t)size,
          pinfo->pointer);
      return (void*)(pinfo->pointer);
   }
}

/* Free a pointer.  It is removed from the list at the same time. */
void
png_debug_free(png_structp png_ptr, void* ptr)
{
   if (png_ptr == NULL)
      fprintf(STDERR, "NULL pointer to png_debug_free.\n");
   if (ptr == 0)
   {
      return;
   }

   /* Unlink the element from the list. */
   {
      memory_infop *ppinfo = &pinformation;
      for (;;)
      {
         memory_infop pinfo = *ppinfo;
         if (pinfo->pointer == ptr)
         {
            *ppinfo = pinfo->next;
            current_allocation -= pinfo->size;
            if (current_allocation < 0)
               fprintf(STDERR, "Duplicate free of memory\n");
            /* We must free the list element too, but first kill
               the memory that is to be freed. */
            memset(ptr, 0x55, pinfo->size);
            png_free_default(png_ptr, pinfo);
            pinfo=NULL;
            break;
         }
         if (pinfo->next == NULL)
         {
            fprintf(STDERR, "Pointer %x not found\n", (uint32_t)ptr);
            break;
         }
         ppinfo = &pinfo->next;
      }
   }

   /* Finally free the data. */
   if(verbose)
      printf("Freeing %x\n",ptr);
   png_free_default(png_ptr, ptr);
   ptr=NULL;
}
#endif /* PNG_USER_MEM_SUPPORTED && PNG_DEBUG */
/* END of code to test memory allocation/deallocation */

/* Test one file */
int
test_one_file(const char *inname, const char *outname)
{
   static FILE* fpin;
   static FILE* fpout;  /* "static" prevents setjmp corruption */
   png_structp read_ptr;
   png_infop read_info_ptr, end_info_ptr;
#ifdef PNG_WRITE_SUPPORTED
   png_structp write_ptr;
   png_infop write_info_ptr;
   png_infop write_end_info_ptr;
#else
   png_structp write_ptr = NULL;
   png_infop write_info_ptr = NULL;
   png_infop write_end_info_ptr = NULL;
#endif
   uint8_t* row_buf;
   uint32_t y;
   uint32_t width, height;
   int num_pass, pass;
   int bit_depth, color_type;
   char inbuf[256], outbuf[256];

   row_buf = NULL;

   if ((fpin = fopen(inname, "rb")) == NULL)
   {
      fprintf(STDERR, "Could not find input file %s\n", inname);
      return (1);
   }

   if ((fpout = fopen(outname, "wb")) == NULL)
   {
      fprintf(STDERR, "Could not open output file %s\n", outname);
      FCLOSE(fpin);
      return (1);
   }

   png_debug(0, "Allocating read and write structures\n");
#if defined(PNG_USER_MEM_SUPPORTED) && PNG_DEBUG
   read_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, (void*)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL, (void*)NULL,
      (png_malloc_ptr)png_debug_malloc, (png_free_ptr)png_debug_free);
#else
   read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (void*)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL);
#endif
#ifdef PNG_WRITE_SUPPORTED
#if defined(PNG_USER_MEM_SUPPORTED) && PNG_DEBUG
   write_ptr = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, (void*)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL, (void*)NULL,
      (png_malloc_ptr)png_debug_malloc, (png_free_ptr)png_debug_free);
#else
   write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (void*)NULL,
      (png_error_ptr)NULL, (png_error_ptr)NULL);
#endif
#endif
   png_debug(0, "Allocating read_info, write_info and end_info structures\n");
   read_info_ptr = png_create_info_struct(read_ptr);
   end_info_ptr = png_create_info_struct(read_ptr);
#ifdef PNG_WRITE_SUPPORTED
   write_info_ptr = png_create_info_struct(write_ptr);
   write_end_info_ptr = png_create_info_struct(write_ptr);
#endif

   png_debug(0, "Initializing input and output streams\n");
   png_init_io(read_ptr, fpin);
#  ifdef PNG_WRITE_SUPPORTED
   png_init_io(write_ptr, fpout);
#  endif
   if(status_dots_requested == 1)
   {
#ifdef PNG_WRITE_SUPPORTED
      png_set_write_status_fn(write_ptr, write_row_callback);
#endif
      png_set_read_status_fn(read_ptr, read_row_callback);
   }
   else
   {
#ifdef PNG_WRITE_SUPPORTED
      png_set_write_status_fn(write_ptr, (png_write_status_ptr)NULL);
#endif
      png_set_read_status_fn(read_ptr, (png_read_status_ptr)NULL);
   }

#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED)
   {
     int i;
     for(i = 0; i<256; i++)
        filters_used[i]=0;
     png_set_read_user_transform_fn(read_ptr, count_filters);
   }
#endif
#if defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
   zero_samples=0;
   png_set_write_user_transform_fn(write_ptr, count_zero_samples);
#endif

#if defined(PNG_READ_UNKNOWN_CHUNKS_SUPPORTED)
#  ifndef PNG_HANDLE_CHUNK_ALWAYS
#    define PNG_HANDLE_CHUNK_ALWAYS       3
#  endif
   png_set_keep_unknown_chunks(read_ptr, PNG_HANDLE_CHUNK_ALWAYS,
      (uint8_t*)NULL, 0);
#endif
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
#  ifndef PNG_HANDLE_CHUNK_IF_SAFE
#    define PNG_HANDLE_CHUNK_IF_SAFE      2
#  endif
   png_set_keep_unknown_chunks(write_ptr, PNG_HANDLE_CHUNK_IF_SAFE,
      (uint8_t*)NULL, 0);
#endif

   png_debug(0, "Reading info struct\n");
   png_read_info(read_ptr, read_info_ptr);

   png_debug(0, "Transferring info struct\n");
   {
      int interlace_type, compression_type, filter_type;

      if (png_get_IHDR(read_ptr, read_info_ptr, &width, &height, &bit_depth,
          &color_type, &interlace_type, &compression_type, &filter_type))
      {
         png_set_IHDR(write_ptr, write_info_ptr, width, height, bit_depth,
         color_type, PNG_INTERLACE_NONE, compression_type, filter_type);
      }
   }
    png_colorp palette;
    int num_palette;

    if (png_get_PLTE(read_ptr, read_info_ptr, &palette, &num_palette))
    {
        png_set_PLTE(write_ptr, write_info_ptr, palette, num_palette);
    }
#if defined(PNG_hIST_SUPPORTED)
   {
      uint16_t* hist;

      if (png_get_hIST(read_ptr, read_info_ptr, &hist))
      {
         png_set_hIST(write_ptr, write_info_ptr, hist);
      }
   }
#endif
#if defined(PNG_pCAL_SUPPORTED)
   {
      char* purpose;
      char* units;
      png_charpp params;
      int32_t X0, X1;
      int type, nparams;

      if (png_get_pCAL(read_ptr, read_info_ptr, &purpose, &X0, &X1, &type,
         &nparams, &units, &params))
      {
         png_set_pCAL(write_ptr, write_info_ptr, purpose, X0, X1, type,
            nparams, units, params);
      }
   }
#endif
#if defined(PNG_pHYs_SUPPORTED)
   {
      uint32_t res_x, res_y;
      int unit_type;

      if (png_get_pHYs(read_ptr, read_info_ptr, &res_x, &res_y, &unit_type))
      {
         png_set_pHYs(write_ptr, write_info_ptr, res_x, res_y, unit_type);
      }
   }
#endif
#if defined(PNG_sBIT_SUPPORTED)
   {
      png_color_8p sig_bit;

      if (png_get_sBIT(read_ptr, read_info_ptr, &sig_bit))
      {
         png_set_sBIT(write_ptr, write_info_ptr, sig_bit);
      }
   }
#endif

#if defined(PNG_tRNS_SUPPORTED)
   {
      uint8_t* trans;
      int num_trans;
      png_color_16p trans_values;

      if (png_get_tRNS(read_ptr, read_info_ptr, &trans, &num_trans,
         &trans_values))
      {
         png_set_tRNS(write_ptr, write_info_ptr, trans, num_trans,
            trans_values);
      }
   }
#endif
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   {
      png_unknown_chunkp unknowns;
      int num_unknowns = (int)png_get_unknown_chunks(read_ptr, read_info_ptr,
         &unknowns);
      if (num_unknowns)
      {
         size_t i;
         png_set_unknown_chunks(write_ptr, write_info_ptr, unknowns,
           num_unknowns);
         /* copy the locations from the read_info_ptr.  The automatically
            generated locations in write_info_ptr are wrong because we
            haven't written anything yet */
         for (i = 0; i < (size_t)num_unknowns; i++)
		 {
           png_set_unknown_chunk_location(write_ptr, write_info_ptr, (int)i, (int)unknowns[i].location);
		 }
      }
   }
#endif

#ifdef PNG_WRITE_SUPPORTED
   png_debug(0, "\nWriting info struct\n");

/* If we wanted, we could write info in two steps:
   png_write_info_before_PLTE(write_ptr, write_info_ptr);
 */
   png_write_info(write_ptr, write_info_ptr);
#endif

#ifdef SINGLE_ROWBUF_ALLOC
   png_debug(0, "\nAllocating row buffer...");
   row_buf = (uint8_t*)png_malloc(read_ptr,
      png_get_rowbytes(read_ptr, read_info_ptr));
   png_debug1(0, "0x%08lx\n\n", (uint32_t)row_buf);
#endif /* SINGLE_ROWBUF_ALLOC */
   png_debug(0, "Writing row data\n");

   num_pass=1;

#ifdef PNGTEST_TIMING
   t_stop = (float)clock();
   t_misc += (t_stop - t_start);
   t_start = t_stop;
#endif
   for (pass = 0; pass < num_pass; pass++)
   {
      png_debug1(0, "Writing row data for pass %d\n",pass);
      for (y = 0; y < height; y++)
      {
#ifndef SINGLE_ROWBUF_ALLOC
         png_debug2(0, "\nAllocating row buffer (pass %d, y = %ld)...", pass,y);
         row_buf = (uint8_t*)png_malloc(read_ptr,
            png_get_rowbytes(read_ptr, read_info_ptr));
         png_debug2(0, "0x%08lx (%ld bytes)\n", (uint32_t)row_buf,
            png_get_rowbytes(read_ptr, read_info_ptr));
#endif /* !SINGLE_ROWBUF_ALLOC */
         png_read_rows(read_ptr, (png_bytepp)&row_buf, (png_bytepp)NULL, 1);

#ifdef PNG_WRITE_SUPPORTED
#ifdef PNGTEST_TIMING
         t_stop = (float)clock();
         t_decode += (t_stop - t_start);
         t_start = t_stop;
#endif
         png_write_rows(write_ptr, (png_bytepp)&row_buf, 1);
#ifdef PNGTEST_TIMING
         t_stop = (float)clock();
         t_encode += (t_stop - t_start);
         t_start = t_stop;
#endif
#endif /* PNG_WRITE_SUPPORTED */

#ifndef SINGLE_ROWBUF_ALLOC
         png_debug2(0, "Freeing row buffer (pass %d, y = %ld)\n\n", pass, y);
         png_free(read_ptr, row_buf);
#endif /* !SINGLE_ROWBUF_ALLOC */
      }
   }

#if defined(PNG_READ_UNKNOWN_CHUNKS_SUPPORTED)
   png_free_data(read_ptr, read_info_ptr, PNG_FREE_UNKN, -1);
#endif
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   png_free_data(write_ptr, write_info_ptr, PNG_FREE_UNKN, -1);
#endif

   png_debug(0, "Reading and writing end_info data\n");

   png_read_end(read_ptr, end_info_ptr);
#if defined(PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED)
   {
      png_unknown_chunkp unknowns;
      int num_unknowns;
      num_unknowns = (int)png_get_unknown_chunks(read_ptr, end_info_ptr,
         &unknowns);
      if (num_unknowns)
      {
         size_t i;
         png_set_unknown_chunks(write_ptr, write_end_info_ptr, unknowns,
           num_unknowns);
         /* copy the locations from the read_info_ptr.  The automatically
            generated locations in write_end_info_ptr are wrong because we
            haven't written the end_info yet */
         for (i = 0; i < (size_t)num_unknowns; i++)
		 {
           png_set_unknown_chunk_location(write_ptr, write_end_info_ptr, (int)i, (int)unknowns[i].location);
		 }
      }
   }
#endif
#ifdef PNG_WRITE_SUPPORTED
   png_write_end(write_ptr, write_end_info_ptr);
#endif

#ifdef PNG_EASY_ACCESS_SUPPORTED
   if(verbose)
   {
      uint32_t iwidth, iheight;
      iwidth = png_get_image_width(write_ptr, write_info_ptr);
      iheight = png_get_image_height(write_ptr, write_info_ptr);
      fprintf(STDERR, "Image width = %lu, height = %lu\n",
         (uint32_t)iwidth, (uint32_t)iheight);
   }
#endif

   png_debug(0, "Destroying data structs\n");
#ifdef SINGLE_ROWBUF_ALLOC
   png_debug(1, "destroying row_buf for read_ptr\n");
   png_free(read_ptr, row_buf);
   row_buf=NULL;
#endif /* SINGLE_ROWBUF_ALLOC */
   png_debug(1, "destroying read_ptr, read_info_ptr, end_info_ptr\n");
   png_destroy_read_struct(&read_ptr, &read_info_ptr, &end_info_ptr);
#ifdef PNG_WRITE_SUPPORTED
   png_debug(1, "destroying write_end_info_ptr\n");
   png_destroy_info_struct(write_ptr, &write_end_info_ptr);
   png_debug(1, "destroying write_ptr, write_info_ptr\n");
   png_destroy_write_struct(&write_ptr, &write_info_ptr);
#endif
   png_debug(0, "Destruction complete.\n");

   FCLOSE(fpin);
   FCLOSE(fpout);

   png_debug(0, "Opening files for comparison\n");
   if ((fpin = fopen(inname, "rb")) == NULL)
   {
      fprintf(STDERR, "Could not find file %s\n", inname);
      return (1);
   }

   if ((fpout = fopen(outname, "rb")) == NULL)
   {
      fprintf(STDERR, "Could not find file %s\n", outname);
      FCLOSE(fpin);
      return (1);
   }

   for(;;)
   {
      size_t num_in, num_out;

      READFILE(fpin, inbuf, 1, num_in);
      READFILE(fpout, outbuf, 1, num_out);

      if (num_in != num_out)
      {
         fprintf(STDERR, "\nFiles %s and %s are of a different size\n",
                 inname, outname);
         if(wrote_question == 0)
         {
            fprintf(STDERR,
         "   Was %s written with the same maximum IDAT chunk size (%d bytes),",
              inname,PNG_ZBUF_SIZE);
            fprintf(STDERR,
              "\n   filtering heuristic (libpng default), compression");
            fprintf(STDERR,
              " level (zlib default),\n   and zlib version (%s)?\n\n",
              ZLIB_VERSION);
            wrote_question=1;
         }
         FCLOSE(fpin);
         FCLOSE(fpout);
         return (0);
      }

      if (!num_in)
         break;

      if (memcmp(inbuf, outbuf, num_in))
      {
         fprintf(STDERR, "\nFiles %s and %s are different\n", inname, outname);
         if(wrote_question == 0)
         {
            fprintf(STDERR,
         "   Was %s written with the same maximum IDAT chunk size (%d bytes),",
                 inname,PNG_ZBUF_SIZE);
            fprintf(STDERR,
              "\n   filtering heuristic (libpng default), compression");
            fprintf(STDERR,
              " level (zlib default),\n   and zlib version (%s)?\n\n",
              ZLIB_VERSION);
            wrote_question=1;
         }
         FCLOSE(fpin);
         FCLOSE(fpout);
         return (0);
      }
   }

   FCLOSE(fpin);
   FCLOSE(fpout);

   return (0);
}


