
/* pngconf.h - machine configurable file for libpng
 *
 * libpng version 1.2.16 - January 31, 2007
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2007 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 */

/* Any machine specific code is near the front of this file, so if you
 * are configuring libpng for a machine, you may want to read the section
 * starting here down to where it starts to typedef png_color, png_text,
 * and png_info.
 */

#ifndef PNGCONF_H
#define PNGCONF_H

#define PNG_1_2_X

/* End of material added to libpng-1.2.8 */

/* This is the size of the compression buffer, and thus the size of
 * an IDAT chunk.  Make this whatever size you feel is best for your
 * machine.  One of these will be allocated per png_struct.  When this
 * is full, it writes the data to the disk, and does some other
 * calculations.  Making this an extremely small size will slow
 * the library down, but you may want to experiment to determine
 * where it becomes significant, if you are concerned with memory
 * usage.  Note that zlib allocates at least 32Kb also.  For readers,
 * this describes the size of the buffer available to read the data in.
 * Unless this gets smaller than the size of a row (compressed),
 * it should not make much difference how big this is.
 */

#ifndef PNG_ZBUF_SIZE
#  define PNG_ZBUF_SIZE 8192
#endif

/* Enable if you want a write-only libpng */

#ifndef PNG_NO_READ_SUPPORTED
#  define PNG_READ_SUPPORTED
#endif

/* Enable if you want a read-only libpng */

#ifndef PNG_NO_WRITE_SUPPORTED
#  define PNG_WRITE_SUPPORTED
#endif


#ifndef PNG_NO_FLOATING_POINT_SUPPORTED
#  ifndef PNG_FLOATING_POINT_SUPPORTED
#    define PNG_FLOATING_POINT_SUPPORTED
#  endif
#endif

/* If you are running on a machine where you cannot allocate more
 * than 64K of memory at once, uncomment this.  While libpng will not
 * normally need that much memory in a chunk (unless you load up a very
 * large file), zlib needs to know how big of a chunk it can use, and
 * libpng thus makes sure to check any memory allocation to verify it
 * will fit into memory.
 */
#define PNG_MAX_MALLOC_64K

 /* This protects us against compilers that run on a windowing system
 * and thus don't have or would rather us not use the stdio types:
 * stdin, stdout, and stderr.  The only one currently used is stderr
 * in png_error() and png_warning().  #defining PNG_NO_CONSOLE_IO will
 * prevent these from being compiled and used. #defining PNG_NO_STDIO
 * will also prevent these, plus will prevent the entire set of stdio
 * macros and functions (FILE *, printf, etc.) from being compiled and used,
 * unless (PNG_DEBUG > 0) has been #defined.
 *
 * #define PNG_NO_CONSOLE_IO
 * #define PNG_NO_STDIO
 */

#  ifdef PNG_NO_STDIO
#    ifndef PNG_NO_CONSOLE_IO
#      define PNG_NO_CONSOLE_IO
#    endif
#    ifdef PNG_DEBUG
#      if (PNG_DEBUG > 0)
#        include <stdio.h>
#      endif
#    endif
#  else
/* "stdio.h" functions are not supported on WindowsCE */
#      include <stdio.h>
#  endif

/* This macro protects us against machines that don't have function
 * prototypes (ie K&R style headers).  If your compiler does not handle
 * function prototypes, define this macro and use the included ansi2knr.
 * I've always been able to use _NO_PROTO as the indicator, but you may
 * need to drag the empty declaration out in front of here, or change the
 * ifdef to suit your own needs.
 */


#if !defined(PNG_SETJMP_NOT_SUPPORTED) && !defined(PNG_NO_SETJMP_SUPPORTED)
#  define PNG_SETJMP_SUPPORTED
#endif

#ifdef PNG_SETJMP_SUPPORTED
/* This is an attempt to force a single setjmp behaviour on Linux.  If
 * the X config stuff didn't define _BSD_SOURCE we wouldn't need this.
 */


/* include setjmp.h for error handling */
#include <setjmp.h>
#include <string.h>
#endif

/* Other defines for things like memory and the like can go here.  */
#ifdef PNG_INTERNAL

#include <stdlib.h>

/* Other defines specific to compilers can go here.  Try to keep
 * them inside an appropriate ifdef/endif pair for portability.
 */

#if defined(PNG_FLOATING_POINT_SUPPORTED)
#    include <math.h>
#endif

/* I have no idea why is this necessary... */
#if defined(_MSC_VER) && (defined(WIN32) || defined(_Windows) || \
    defined(_WINDOWS) || defined(_WIN32) || defined(__WIN32__))
#  include <malloc.h>
#endif

/* This controls how fine the dithering gets.  As this allocates
 * a largish chunk of memory (32K), those who are not as concerned
 * with dithering quality can decrease some or all of these.
 */
#ifndef PNG_DITHER_RED_BITS
#  define PNG_DITHER_RED_BITS 5
#endif
#ifndef PNG_DITHER_GREEN_BITS
#  define PNG_DITHER_GREEN_BITS 5
#endif
#ifndef PNG_DITHER_BLUE_BITS
#  define PNG_DITHER_BLUE_BITS 5
#endif

#endif /* PNG_INTERNAL */

/* The following uses const char * instead of char * for error
 * and warning message functions, so some compilers won't complain.
 * If you do not want to use const, define PNG_NO_CONST here.
 */

/* The following defines give you the ability to remove code from the
 * library that you will not be using.  I wish I could figure out how to
 * automate this, but I can't do that without making it seriously hard
 * on the users.  So if you are not using an ability, change the #define
 * to and #undef, and that part of the library will not be compiled.  If
 * your linker can't find a function, you may want to make sure the
 * ability is defined here.  Some of these depend upon some others being
 * defined.  I haven't figured out all the interactions here, so you may
 * have to experiment awhile to get everything to compile.  If you are
 * creating or using a shared library, you probably shouldn't touch this,
 * as it will affect the size of the structures, and this will cause bad
 * things to happen if the library and/or application ever change.
 */

/* The following support, added after version 1.0.0, can be turned off here en
 * masse by defining PNG_LEGACY_SUPPORTED in case you need binary compatibility
 * with old applications that require the length of png_struct and png_info
 * to remain unchanged.
 */

#ifdef PNG_LEGACY_SUPPORTED
#  define PNG_NO_FREE_ME
#  define PNG_NO_READ_UNKNOWN_CHUNKS
#  define PNG_NO_WRITE_UNKNOWN_CHUNKS
#  define PNG_NO_READ_USER_CHUNKS
#  define PNG_NO_READ_sCAL
#  define PNG_NO_WRITE_sCAL
#  define PNG_NO_READ_sPLT
#  define PNG_NO_WRITE_sPLT
#  define PNG_NO_INFO_IMAGE
#  define PNG_NO_READ_RGB_TO_GRAY
#  define PNG_NO_READ_USER_TRANSFORM
#  define PNG_NO_WRITE_USER_TRANSFORM
#  define PNG_NO_USER_MEM
#  define PNG_NO_READ_EMPTY_PLTE
#  define PNG_NO_MNG_FEATURES
#  define PNG_NO_FIXED_POINT_SUPPORTED
#endif

/* Ignore attempt to turn off both floating and fixed point support */
#if !defined(PNG_FLOATING_POINT_SUPPORTED) || \
    !defined(PNG_NO_FIXED_POINT_SUPPORTED)
#  define PNG_FIXED_POINT_SUPPORTED
#endif

#ifndef PNG_NO_FREE_ME
#  define PNG_FREE_ME_SUPPORTED
#endif

#if defined(PNG_READ_SUPPORTED)

#if !defined(PNG_READ_TRANSFORMS_NOT_SUPPORTED) && \
      !defined(PNG_NO_READ_TRANSFORMS)
#  define PNG_READ_TRANSFORMS_SUPPORTED
#endif

#ifdef PNG_READ_TRANSFORMS_SUPPORTED
#  ifndef PNG_NO_READ_EXPAND
#    define PNG_READ_EXPAND_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_SHIFT
#    define PNG_READ_SHIFT_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_PACK
#    define PNG_READ_PACK_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_BGR
#    define PNG_READ_BGR_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_SWAP
#    define PNG_READ_SWAP_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_INVERT
#    define PNG_READ_INVERT_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_DITHER
#    define PNG_READ_DITHER_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_16_TO_8
#    define PNG_READ_16_TO_8_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_FILLER
#    define PNG_READ_FILLER_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_GRAY_TO_RGB
#    define PNG_READ_GRAY_TO_RGB_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_SWAP_ALPHA
#    define PNG_READ_SWAP_ALPHA_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_INVERT_ALPHA
#    define PNG_READ_INVERT_ALPHA_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_STRIP_ALPHA
#    define PNG_READ_STRIP_ALPHA_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_USER_TRANSFORM
#    define PNG_READ_USER_TRANSFORM_SUPPORTED
#  endif
#  ifndef PNG_NO_READ_RGB_TO_GRAY
#    define PNG_READ_RGB_TO_GRAY_SUPPORTED
#  endif
#endif /* PNG_READ_TRANSFORMS_SUPPORTED */

#if !defined(PNG_NO_PROGRESSIVE_READ) && \
 !defined(PNG_PROGRESSIVE_READ_NOT_SUPPORTED)  /* if you don't do progressive */
#  define PNG_PROGRESSIVE_READ_SUPPORTED     /* reading.  This is not talking */
#endif                               /* about interlacing capability!  You'll */
              /* still have interlacing unless you change the following line: */

#define PNG_READ_INTERLACING_SUPPORTED /* required for PNG-compliant decoders */

#ifndef PNG_NO_READ_COMPOSITE_NODIV
#  ifndef PNG_NO_READ_COMPOSITED_NODIV  /* libpng-1.0.x misspelling */
#    define PNG_READ_COMPOSITE_NODIV_SUPPORTED   /* well tested on Intel, SGI */
#  endif
#endif

#endif /* PNG_READ_SUPPORTED */

#if defined(PNG_WRITE_SUPPORTED)

# if !defined(PNG_WRITE_TRANSFORMS_NOT_SUPPORTED) && \
    !defined(PNG_NO_WRITE_TRANSFORMS)
#  define PNG_WRITE_TRANSFORMS_SUPPORTED
#endif

#ifdef PNG_WRITE_TRANSFORMS_SUPPORTED
#  ifndef PNG_NO_WRITE_SHIFT
#    define PNG_WRITE_SHIFT_SUPPORTED
#  endif
#  ifndef PNG_NO_WRITE_PACK
#    define PNG_WRITE_PACK_SUPPORTED
#  endif
#  ifndef PNG_NO_WRITE_BGR
#    define PNG_WRITE_BGR_SUPPORTED
#  endif
#  ifndef PNG_NO_WRITE_SWAP
#    define PNG_WRITE_SWAP_SUPPORTED
#  endif
#  ifndef PNG_NO_WRITE_INVERT
#    define PNG_WRITE_INVERT_SUPPORTED
#  endif
#  ifndef PNG_NO_WRITE_FILLER
#    define PNG_WRITE_FILLER_SUPPORTED   /* same as WRITE_STRIP_ALPHA */
#  endif
#  ifndef PNG_NO_WRITE_SWAP_ALPHA
#    define PNG_WRITE_SWAP_ALPHA_SUPPORTED
#  endif
#  ifndef PNG_NO_WRITE_INVERT_ALPHA
#    define PNG_WRITE_INVERT_ALPHA_SUPPORTED
#  endif
#  ifndef PNG_NO_WRITE_USER_TRANSFORM
#    define PNG_WRITE_USER_TRANSFORM_SUPPORTED
#  endif
#endif /* PNG_WRITE_TRANSFORMS_SUPPORTED */

#if !defined(PNG_NO_WRITE_INTERLACING_SUPPORTED) && \
    !defined(PNG_WRITE_INTERLACING_SUPPORTED)
#define PNG_WRITE_INTERLACING_SUPPORTED  /* not required for PNG-compliant
                                            encoders, but can cause trouble
                                            if left undefined */
#endif

#if !defined(PNG_NO_WRITE_WEIGHTED_FILTER) && \
    !defined(PNG_WRITE_WEIGHTED_FILTER) && \
     defined(PNG_FLOATING_POINT_SUPPORTED)
#  define PNG_WRITE_WEIGHTED_FILTER_SUPPORTED
#endif

#ifndef PNG_NO_WRITE_FLUSH
#  define PNG_WRITE_FLUSH_SUPPORTED
#endif

#endif /* PNG_WRITE_SUPPORTED */

#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED) || \
    defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
#  ifndef PNG_NO_USER_TRANSFORM_PTR
#    define PNG_USER_TRANSFORM_PTR_SUPPORTED
#  endif
#endif

/* This adds extra functions in pngget.c for accessing data from the
 * info pointer (added in version 0.99)
 * png_get_image_width()
 * png_get_image_height()
 * png_get_bit_depth()
 * png_get_color_type()
 * png_get_compression_type()
 * png_get_filter_type()
 * png_get_interlace_type()
 * png_get_pixel_aspect_ratio()
 * png_get_pixels_per_meter()
 * png_get_x_offset_pixels()
 * png_get_y_offset_pixels()
 * png_get_x_offset_microns()
 * png_get_y_offset_microns()
 */
#if !defined(PNG_NO_EASY_ACCESS) && !defined(PNG_EASY_ACCESS_SUPPORTED)
#  define PNG_EASY_ACCESS_SUPPORTED
#endif

/* If you are sure that you don't need thread safety and you are compiling
   with PNG_USE_PNGCCRD for an MMX application, you can define this for
   faster execution.  See pnggccrd.c.
#define PNG_THREAD_UNSAFE_OK
*/

#if !defined(PNG_NO_USER_MEM) && !defined(PNG_USER_MEM_SUPPORTED)
#  define PNG_USER_MEM_SUPPORTED
#endif

/* Added at libpng-1.2.6 */
#ifndef PNG_SET_USER_LIMITS_SUPPORTED
#if !defined(PNG_NO_SET_USER_LIMITS) && !defined(PNG_SET_USER_LIMITS_SUPPORTED)
#  define PNG_SET_USER_LIMITS_SUPPORTED
#endif
#endif

/* Added at libpng-1.0.16 and 1.2.6.  To accept all valid PNGS no matter
 * how large, set these limits to 0x7fffffffL
 */
#ifndef PNG_USER_WIDTH_MAX
#  define PNG_USER_WIDTH_MAX 1000000L
#endif
#ifndef PNG_USER_HEIGHT_MAX
#  define PNG_USER_HEIGHT_MAX 1000000L
#endif

/* These are currently experimental features, define them if you want */

/* very little testing */
/*
#ifdef PNG_READ_SUPPORTED
#  ifndef PNG_READ_16_TO_8_ACCURATE_SCALE_SUPPORTED
#    define PNG_READ_16_TO_8_ACCURATE_SCALE_SUPPORTED
#  endif
#endif
*/

/* This is only for PowerPC big-endian and 680x0 systems */
/* some testing */
/*
#ifndef PNG_READ_BIG_ENDIAN_SUPPORTED
#  define PNG_READ_BIG_ENDIAN_SUPPORTED
#endif
*/

/* Buggy compilers (e.g., gcc 2.7.2.2) need this */
/*
#define PNG_NO_POINTER_INDEXING
*/

/* These functions are turned off by default, as they will be phased out. */
/*
#define  PNG_USELESS_TESTS_SUPPORTED
#define  PNG_CORRECT_PALETTE_SUPPORTED
*/

/* Any chunks you are not interested in, you can undef here.  The
 * ones that allocate memory may be expecially important (hIST,
 * tEXt, zTXt, tRNS, pCAL).  Others will just save time and make png_info
 * a bit smaller.
 */

#if defined(PNG_READ_SUPPORTED) && \
    !defined(PNG_READ_ANCILLARY_CHUNKS_NOT_SUPPORTED) && \
    !defined(PNG_NO_READ_ANCILLARY_CHUNKS)
#  define PNG_READ_ANCILLARY_CHUNKS_SUPPORTED
#endif

#if defined(PNG_WRITE_SUPPORTED) && \
    !defined(PNG_WRITE_ANCILLARY_CHUNKS_NOT_SUPPORTED) && \
    !defined(PNG_NO_WRITE_ANCILLARY_CHUNKS)
#  define PNG_WRITE_ANCILLARY_CHUNKS_SUPPORTED
#endif

#ifdef PNG_READ_ANCILLARY_CHUNKS_SUPPORTED

#ifndef PNG_NO_READ_hIST
#  define PNG_READ_hIST_SUPPORTED
#  define PNG_hIST_SUPPORTED
#endif
#ifndef PNG_NO_READ_pCAL
#  define PNG_READ_pCAL_SUPPORTED
#  define PNG_pCAL_SUPPORTED
#endif
#ifndef PNG_NO_READ_pHYs
#  define PNG_READ_pHYs_SUPPORTED
#  define PNG_pHYs_SUPPORTED
#endif
#ifndef PNG_NO_READ_sBIT
#  define PNG_READ_sBIT_SUPPORTED
#  define PNG_sBIT_SUPPORTED
#endif
#ifndef PNG_NO_READ_sPLT
#  define PNG_READ_sPLT_SUPPORTED
#  define PNG_sPLT_SUPPORTED
#endif
#ifndef PNG_NO_READ_sRGB
#  define PNG_READ_sRGB_SUPPORTED
#  define PNG_sRGB_SUPPORTED
#endif
#ifndef PNG_NO_READ_tRNS
#  define PNG_READ_tRNS_SUPPORTED
#  define PNG_tRNS_SUPPORTED
#endif
#ifndef PNG_NO_READ_UNKNOWN_CHUNKS
#  define PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
#  ifndef PNG_UNKNOWN_CHUNKS_SUPPORTED
#    define PNG_UNKNOWN_CHUNKS_SUPPORTED
#  endif
#  ifndef PNG_NO_HANDLE_AS_UNKNOWN
#    define PNG_HANDLE_AS_UNKNOWN_SUPPORTED
#  endif
#endif
#if !defined(PNG_NO_READ_USER_CHUNKS) && \
     defined(PNG_READ_UNKNOWN_CHUNKS_SUPPORTED)
#  define PNG_READ_USER_CHUNKS_SUPPORTED
#  define PNG_USER_CHUNKS_SUPPORTED
#  ifdef PNG_NO_READ_UNKNOWN_CHUNKS
#    undef PNG_NO_READ_UNKNOWN_CHUNKS
#  endif
#  ifdef PNG_NO_HANDLE_AS_UNKNOWN
#    undef PNG_NO_HANDLE_AS_UNKNOWN
#  endif
#endif
#ifndef PNG_NO_READ_OPT_PLTE
#  define PNG_READ_OPT_PLTE_SUPPORTED /* only affects support of the */
#endif                      /* optional PLTE chunk in RGB and RGBA images */
#endif /* PNG_READ_ANCILLARY_CHUNKS_SUPPORTED */

#ifdef PNG_WRITE_ANCILLARY_CHUNKS_SUPPORTED

#ifndef PNG_NO_WRITE_hIST
#  define PNG_WRITE_hIST_SUPPORTED
#  ifndef PNG_hIST_SUPPORTED
#    define PNG_hIST_SUPPORTED
#  endif
#endif
#ifndef PNG_NO_WRITE_pCAL
#  define PNG_WRITE_pCAL_SUPPORTED
#  ifndef PNG_pCAL_SUPPORTED
#    define PNG_pCAL_SUPPORTED
#  endif
#endif
#ifndef PNG_NO_WRITE_pHYs
#  define PNG_WRITE_pHYs_SUPPORTED
#  ifndef PNG_pHYs_SUPPORTED
#    define PNG_pHYs_SUPPORTED
#  endif
#endif
#ifndef PNG_NO_WRITE_sBIT
#  define PNG_WRITE_sBIT_SUPPORTED
#  ifndef PNG_sBIT_SUPPORTED
#    define PNG_sBIT_SUPPORTED
#  endif
#endif
#ifndef PNG_NO_WRITE_sPLT
#  define PNG_WRITE_sPLT_SUPPORTED
#  ifndef PNG_sPLT_SUPPORTED
#    define PNG_sPLT_SUPPORTED
#  endif
#endif
#ifndef PNG_NO_WRITE_sRGB
#  define PNG_WRITE_sRGB_SUPPORTED
#  ifndef PNG_sRGB_SUPPORTED
#    define PNG_sRGB_SUPPORTED
#  endif
#endif
#ifndef PNG_NO_WRITE_tRNS
#  define PNG_WRITE_tRNS_SUPPORTED
#  ifndef PNG_tRNS_SUPPORTED
#    define PNG_tRNS_SUPPORTED
#  endif
#endif
#ifndef PNG_NO_WRITE_UNKNOWN_CHUNKS
#  define PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED
#  ifndef PNG_UNKNOWN_CHUNKS_SUPPORTED
#    define PNG_UNKNOWN_CHUNKS_SUPPORTED
#  endif
#  ifndef PNG_NO_HANDLE_AS_UNKNOWN
#     ifndef PNG_HANDLE_AS_UNKNOWN_SUPPORTED
#       define PNG_HANDLE_AS_UNKNOWN_SUPPORTED
#     endif
#  endif
#endif

#endif /* PNG_WRITE_ANCILLARY_CHUNKS_SUPPORTED */

/* Turn this off to disable png_read_png() and
 * png_write_png() and leave the row_pointers member
 * out of the info structure.
 */
#ifndef PNG_NO_INFO_IMAGE
#  define PNG_INFO_IMAGE_SUPPORTED
#endif

/* Suggest testing for specific compiler first before testing for
 * .  The Watcom compiler defines both __MEDIUM__ and M_I86MM,
 * making reliance oncertain keywords suspect. (SJT)
 */


/* Typedef for floating-point numbers that are converted
   to fixed-point with a multiple of 100,000, e.g., int_gamma */
typedef int32_t png_fixed_point;

#ifdef PNG_FLOATING_POINT_SUPPORTED
typedef double          *png_doublep;
#endif

/* Pointers to pointers; i.e. arrays */
typedef uint8_t        **png_bytepp;
typedef uint32_t     **png_uint_32pp;
typedef int32_t      **png_int_32pp;
typedef uint16_t     **png_uint_16pp;
typedef int16_t      **png_int_16pp;
typedef const char  **png_const_charpp;
typedef char            **png_charpp;
typedef png_fixed_point **png_fixed_point_pp;
#ifdef PNG_FLOATING_POINT_SUPPORTED
typedef double          **png_doublepp;
#endif

/* Pointers to pointers to pointers; i.e., pointer to array */
typedef char            ***png_charppp;

#if defined(PNG_1_2_X)
/* SPC -  Is this stuff deprecated? */
/* It'll be removed as of libpng-1.3.0 - GR-P */
/* libpng typedefs for types in zlib. If zlib changes
 * or another compression library is used, then change these.
 * Eliminates need to change all the source files.
 */
typedef char*      png_zcharp;
typedef char**     png_zcharpp;
typedef z_stream*   png_zstreamp;
#endif /* defined(PNG_1_2_X) */

/* Do not use global arrays (helps with building DLL's)
 * They are no longer used in libpng itself, since version 1.0.5c,
 * but might be required for some pre-1.0.5c applications.
 */
#if !defined(PNG_USE_LOCAL_ARRAYS) && !defined(PNG_USE_GLOBAL_ARRAYS)
#  if defined(PNG_NO_GLOBAL_ARRAYS)
#    define PNG_USE_LOCAL_ARRAYS
#  else
#    define PNG_USE_GLOBAL_ARRAYS
#  endif
#endif

#ifdef PNG_USE_GLOBAL_ARRAYS
#  ifndef PNG_EXPORT_VAR
#    define PNG_EXPORT_VAR(type) extern  type
#  endif
#endif

/* User may want to use these so they are not in PNG_INTERNAL. Any library
 * functions that are passed far data must be model independent.
 */

#ifndef PNG_ABORT
#  define PNG_ABORT() abort()
#endif

#ifdef PNG_SETJMP_SUPPORTED
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#else
#  define png_jmpbuf(png_ptr) \
   (LIBPNG_WAS_COMPILED_WITH__PNG_SETJMP_NOT_SUPPORTED)
#endif

#  define CVT_PTR(ptr)         (ptr)
#  define CVT_PTR_NOCHECK(ptr) (ptr)
#  define png_strcpy  strcpy
#  define png_strncpy strncpy     /* Added to v 1.2.6 */
#  define png_strlen  strlen
#  define png_memcmp  memcmp      /* SJT: added */
#  define png_memcpy  memcpy
#  define png_memset  memset
/* End of memory model independent support */

/* Just a little check that someone hasn't tried to define something
 * contradictory.
 */
#if (PNG_ZBUF_SIZE > 65536L) && defined(PNG_MAX_MALLOC_64K)
#  undef PNG_ZBUF_SIZE
#  define PNG_ZBUF_SIZE 65536L
#endif

#ifdef PNG_READ_SUPPORTED
/* Prior to libpng-1.0.9, this block was in pngasmrd.h */
#if defined(PNG_INTERNAL)

/* These are the default thresholds before the MMX code kicks in; if either
 * rowbytes or bitdepth is below the threshold, plain C code is used.  These
 * can be overridden at runtime via the png_set_mmx_thresholds() call in
 * libpng 1.2.0 and later.  The values below were chosen by Intel.
 */

#ifndef PNG_MMX_ROWBYTES_THRESHOLD_DEFAULT
#  define PNG_MMX_ROWBYTES_THRESHOLD_DEFAULT  128  /*  >=  */
#endif
#ifndef PNG_MMX_BITDEPTH_THRESHOLD_DEFAULT
#  define PNG_MMX_BITDEPTH_THRESHOLD_DEFAULT  9    /*  >=  */   
#endif

#endif /* PNG_INTERNAL */
#endif /* PNG_READ_SUPPORTED */

#endif /* PNGCONF_H */
