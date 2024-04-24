
/* png.h - header file for PNG reference library
 *
 * libpng version 1.2.16 - January 31, 2007
 * Copyright (c) 1998-2007 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * Authors and maintainers:
 *  libpng versions 0.71, May 1995, through 0.88, January 1996: Guy Schalnat
 *  libpng versions 0.89c, June 1996, through 0.96, May 1997: Andreas Dilger
 *  libpng versions 0.97, January 1998, through 1.2.16 - January 31, 2007: Glenn
 *  See also "Contributing Authors", below.
 *
 * Note about libpng version numbers:
 *
 *    Due to various miscommunications, unforeseen code incompatibilities
 *    and occasional factors outside the authors' control, version numbering
 *    on the library has not always been consistent and straightforward.
 *    The following table summarizes matters since version 0.89c, which was
 *    the first widely used release:
 *
 *    source                 png.h  png.h  shared-lib
 *    version                string   int  version
 *    -------                ------ -----  ----------
 *    0.89c "1.0 beta 3"     0.89      89  1.0.89
 *    0.90  "1.0 beta 4"     0.90      90  0.90  [should have been 2.0.90]
 *    0.95  "1.0 beta 5"     0.95      95  0.95  [should have been 2.0.95]
 *    0.96  "1.0 beta 6"     0.96      96  0.96  [should have been 2.0.96]
 *    0.97b "1.00.97 beta 7" 1.00.97   97  1.0.1 [should have been 2.0.97]
 *    0.97c                  0.97      97  2.0.97
 *    0.98                   0.98      98  2.0.98
 *    0.99                   0.99      98  2.0.99
 *    0.99a-m                0.99      99  2.0.99
 *    1.00                   1.00     100  2.1.0 [100 should be 10000]
 *    1.0.0      (from here on, the   100  2.1.0 [100 should be 10000]
 *    1.0.1       png.h string is   10001  2.1.0
 *    1.0.1a-e    identical to the  10002  from here on, the shared library
 *    1.0.2       source version)   10002  is 2.V where V is the source code
 *    1.0.2a-b                      10003  version, except as noted.
 *    1.0.3                         10003
 *    1.0.3a-d                      10004
 *    1.0.4                         10004
 *    1.0.4a-f                      10005
 *    1.0.5 (+ 2 patches)           10005
 *    1.0.5a-d                      10006
 *    1.0.5e-r                      10100 (not source compatible)
 *    1.0.5s-v                      10006 (not binary compatible)
 *    1.0.6 (+ 3 patches)           10006 (still binary incompatible)
 *    1.0.6d-f                      10007 (still binary incompatible)
 *    1.0.6g                        10007
 *    1.0.6h                        10007  10.6h (testing xy.z so-numbering)
 *    1.0.6i                        10007  10.6i
 *    1.0.6j                        10007  2.1.0.6j (incompatible with 1.0.0)
 *    1.0.7beta11-14        DLLNUM  10007  2.1.0.7beta11-14 (binary compatible)
 *    1.0.7beta15-18           1    10007  2.1.0.7beta15-18 (binary compatible)
 *    1.0.7rc1-2               1    10007  2.1.0.7rc1-2 (binary compatible)
 *    1.0.7                    1    10007  (still compatible)
 *    1.0.8beta1-4             1    10008  2.1.0.8beta1-4
 *    1.0.8rc1                 1    10008  2.1.0.8rc1
 *    1.0.8                    1    10008  2.1.0.8
 *    1.0.9beta1-6             1    10009  2.1.0.9beta1-6
 *    1.0.9rc1                 1    10009  2.1.0.9rc1
 *    1.0.9beta7-10            1    10009  2.1.0.9beta7-10
 *    1.0.9rc2                 1    10009  2.1.0.9rc2
 *    1.0.9                    1    10009  2.1.0.9
 *    1.0.10beta1              1    10010  2.1.0.10beta1
 *    1.0.10rc1                1    10010  2.1.0.10rc1
 *    1.0.10                   1    10010  2.1.0.10
 *    1.0.11beta1-3            1    10011  2.1.0.11beta1-3
 *    1.0.11rc1                1    10011  2.1.0.11rc1
 *    1.0.11                   1    10011  2.1.0.11
 *    1.0.12beta1-2            2    10012  2.1.0.12beta1-2
 *    1.0.12rc1                2    10012  2.1.0.12rc1
 *    1.0.12                   2    10012  2.1.0.12
 *    1.1.0a-f                 -    10100  2.1.1.0a-f (branch abandoned)
 *    1.2.0beta1-2             2    10200  2.1.2.0beta1-2
 *    1.2.0beta3-5             3    10200  3.1.2.0beta3-5
 *    1.2.0rc1                 3    10200  3.1.2.0rc1
 *    1.2.0                    3    10200  3.1.2.0
 *    1.2.1beta1-4             3    10201  3.1.2.1beta1-4
 *    1.2.1rc1-2               3    10201  3.1.2.1rc1-2
 *    1.2.1                    3    10201  3.1.2.1
 *    1.2.2beta1-6            12    10202  12.so.0.1.2.2beta1-6
 *    1.0.13beta1             10    10013  10.so.0.1.0.13beta1
 *    1.0.13rc1               10    10013  10.so.0.1.0.13rc1
 *    1.2.2rc1                12    10202  12.so.0.1.2.2rc1
 *    1.0.13                  10    10013  10.so.0.1.0.13
 *    1.2.2                   12    10202  12.so.0.1.2.2
 *    1.2.3rc1-6              12    10203  12.so.0.1.2.3rc1-6
 *    1.2.3                   12    10203  12.so.0.1.2.3
 *    1.2.4beta1-3            13    10204  12.so.0.1.2.4beta1-3
 *    1.0.14rc1               13    10014  10.so.0.1.0.14rc1
 *    1.2.4rc1                13    10204  12.so.0.1.2.4rc1
 *    1.0.14                  10    10014  10.so.0.1.0.14
 *    1.2.4                   13    10204  12.so.0.1.2.4
 *    1.2.5beta1-2            13    10205  12.so.0.1.2.5beta1-2
 *    1.0.15rc1-3             10    10015  10.so.0.1.0.15rc1-3
 *    1.2.5rc1-3              13    10205  12.so.0.1.2.5rc1-3
 *    1.0.15                  10    10015  10.so.0.1.0.15
 *    1.2.5                   13    10205  12.so.0.1.2.5
 *    1.2.6beta1-4            13    10206  12.so.0.1.2.6beta1-4
 *    1.0.16                  10    10016  10.so.0.1.0.16
 *    1.2.6                   13    10206  12.so.0.1.2.6
 *    1.2.7beta1-2            13    10207  12.so.0.1.2.7beta1-2
 *    1.0.17rc1               10    10017  10.so.0.1.0.17rc1
 *    1.2.7rc1                13    10207  12.so.0.1.2.7rc1
 *    1.0.17                  10    10017  10.so.0.1.0.17
 *    1.2.7                   13    10207  12.so.0.1.2.7
 *    1.2.8beta1-5            13    10208  12.so.0.1.2.8beta1-5
 *    1.0.18rc1-5             10    10018  10.so.0.1.0.18rc1-5
 *    1.2.8rc1-5              13    10208  12.so.0.1.2.8rc1-5
 *    1.0.18                  10    10018  10.so.0.1.0.18
 *    1.2.8                   13    10208  12.so.0.1.2.8
 *    1.2.9beta1-3            13    10209  12.so.0.1.2.9beta1-3
 *    1.2.9beta4-11           13    10209  12.so.0.9[.0]
 *    1.2.9rc1                13    10209  12.so.0.9[.0]
 *    1.2.9                   13    10209  12.so.0.9[.0]
 *    1.2.10beta1-8           13    10210  12.so.0.10[.0]
 *    1.2.10rc1-3             13    10210  12.so.0.10[.0]
 *    1.2.10                  13    10210  12.so.0.10[.0]
 *    1.2.11beta1-4           13    10211  12.so.0.11[.0]
 *    1.0.19rc1-5             10    10019  10.so.0.19[.0]
 *    1.2.11rc1-5             13    10211  12.so.0.11[.0]
 *    1.0.19                  10    10019  10.so.0.19[.0]
 *    1.2.11                  13    10211  12.so.0.11[.0]
 *    1.0.20                  10    10020  10.so.0.20[.0]
 *    1.2.12                  13    10212  12.so.0.12[.0]
 *    1.2.13beta1             13    10213  12.so.0.13[.0]
 *    1.0.21                  10    10021  10.so.0.21[.0]
 *    1.2.13                  13    10213  12.so.0.13[.0]
 *    1.2.14beta1-2           13    10214  12.so.0.14[.0]
 *    1.0.22rc1               10    10022  10.so.0.22[.0]
 *    1.2.14rc1               13    10214  12.so.0.14[.0]
 *    1.0.22                  10    10022  10.so.0.22[.0]
 *    1.2.14                  13    10214  12.so.0.14[.0]
 *    1.2.15beta1-6           13    10215  12.so.0.15[.0]
 *    1.0.23rc1-5             10    10023  10.so.0.23[.0]
 *    1.2.15rc1-5             13    10215  12.so.0.15[.0]
 *    1.0.23                  10    10023  10.so.0.23[.0]
 *    1.2.15                  13    10215  12.so.0.15[.0]
 *    1.2.16beta1-2           13    10216  12.so.0.16[.0]
 *    1.2.16rc1               13    10216  12.so.0.16[.0]
 *    1.0.24                  10    10024  10.so.0.24[.0]
 *    1.2.16                  13    10216  12.so.0.16[.0]
 *
 *    Henceforth the source version will match the shared-library major
 *    and minor numbers; the shared-library major version number will be
 *    used for changes in backward compatibility, as it is intended.  The
 *    PNG_LIBPNG_VER macro, which is not used within libpng but is available
 *    for applications, is an unsigned integer of the form xyyzz corresponding
 *    to the source version x.y.z (leading zeros in y and z).  Beta versions
 *    were given the previous public release number plus a letter, until
 *    version 1.0.6j; from then on they were given the upcoming public
 *    release number plus "betaNN" or "rcN".
 *
 *    Binary incompatibility exists only when applications make direct access
 *    to the info_ptr or png_ptr members through png.h, and the compiled
 *    application is loaded with a different version of the library.
 *
 *    DLLNUM will change each time there are forward or backward changes
 *    in binary compatibility (e.g., when a new feature is added).
 *
 * See libpng.txt or libpng.3 for more information.  The PNG specification
 * is available as a W3C Recommendation and as an ISO Specification,
 * <http://www.w3.org/TR/2003/REC-PNG-20031110/
 */

/*
 * COPYRIGHT NOTICE, DISCLAIMER, and LICENSE:
 *
 * If you modify libpng you may insert additional notices immediately following
 * this sentence.
 *
 * libpng versions 1.2.6, August 15, 2004, through 1.2.16, January 31, 2007, are
 * Copyright (c) 2004, 2007 Glenn Randers-Pehrson, and are
 * distributed according to the same disclaimer and license as libpng-1.2.5
 * with the following individual added to the list of Contributing Authors:
 *
 *    Cosmin Truta
 *
 * libpng versions 1.0.7, July 1, 2000, through 1.2.5, October 3, 2002, are
 * Copyright (c) 2000-2002 Glenn Randers-Pehrson, and are
 * distributed according to the same disclaimer and license as libpng-1.0.6
 * with the following individuals added to the list of Contributing Authors:
 *
 *    Simon-Pierre Cadieux
 *    Eric S. Raymond
 *    Gilles Vollant
 *
 * and with the following additions to the disclaimer:
 *
 *    There is no warranty against interference with your enjoyment of the
 *    library or against infringement.  There is no warranty that our
 *    efforts or the library will fulfill any of your particular purposes
 *    or needs.  This library is provided with all faults, and the entire
 *    risk of satisfactory quality, performance, accuracy, and effort is with
 *    the user.
 *
 * libpng versions 0.97, January 1998, through 1.0.6, March 20, 2000, are
 * Copyright (c) 1998, 1999, 2000 Glenn Randers-Pehrson, and are
 * distributed according to the same disclaimer and license as libpng-0.96,
 * with the following individuals added to the list of Contributing Authors:
 *
 *    Tom Lane
 *    Glenn Randers-Pehrson
 *    Willem van Schaik
 *
 * libpng versions 0.89, June 1996, through 0.96, May 1997, are
 * Copyright (c) 1996, 1997 Andreas Dilger
 * Distributed according to the same disclaimer and license as libpng-0.88,
 * with the following individuals added to the list of Contributing Authors:
 *
 *    John Bowler
 *    Kevin Bracey
 *    Sam Bushell
 *    Magnus Holmgren
 *    Greg Roelofs
 *    Tom Tanner
 *
 * libpng versions 0.5, May 1995, through 0.88, January 1996, are
 * Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.
 *
 * For the purposes of this copyright and license, "Contributing Authors"
 * is defined as the following set of individuals:
 *
 *    Andreas Dilger
 *    Dave Martindale
 *    Guy Eric Schalnat
 *    Paul Schmidt
 *    Tim Wegner
 *
 * The PNG Reference Library is supplied "AS IS".  The Contributing Authors
 * and Group 42, Inc. disclaim all warranties, expressed or implied,
 * including, without limitation, the warranties of merchantability and of
 * fitness for any purpose.  The Contributing Authors and Group 42, Inc.
 * assume no liability for direct, indirect, incidental, special, exemplary,
 * or consequential damages, which may result from the use of the PNG
 * Reference Library, even if advised of the possibility of such damage.
 *
 * Permission is hereby granted to use, copy, modify, and distribute this
 * source code, or portions hereof, for any purpose, without fee, subject
 * to the following restrictions:
 *
 * 1. The origin of this source code must not be misrepresented.
 *
 * 2. Altered versions must be plainly marked as such and
 * must not be misrepresented as being the original source.
 *
 * 3. This Copyright notice may not be removed or altered from
 *    any source or altered source distribution.
 *
 * The Contributing Authors and Group 42, Inc. specifically permit, without
 * fee, and encourage the use of this source code as a component to
 * supporting the PNG file format in commercial products.  If you use this
 * source code in a product, acknowledgment is not required but would be
 * appreciated.
 */

/*
 * A "png_get_copyright" function is available, for convenient use in "about"
 * boxes and the like:
 *
 * printf("%s",png_get_copyright(NULL));
 *
 * Also, the PNG logo (in PNG format, of course) is supplied in the
 * files "pngbar.png" and "pngbar.jpg (88x31) and "pngnow.png" (98x31).
 */

/*
 * Libpng is OSI Certified Open Source Software.  OSI Certified is a
 * certification mark of the Open Source Initiative.
 */

/*
 * The contributing authors would like to thank all those who helped
 * with testing, bug fixes, and patience.  This wouldn't have been
 * possible without all of you.
 *
 * Thanks to Frank J. T. Wojcik for helping with the documentation.
 */

/*
 * Y2K compliance in libpng:
 * =========================
 *
 *    January 31, 2007
 *
 *    Since the PNG Development group is an ad-hoc body, we can't make
 *    an official declaration.
 *
 *    This is your unofficial assurance that libpng from version 0.71 and
 *    upward through 1.2.16 are Y2K compliant.  It is my belief that earlier
 *    versions were also Y2K compliant.
 *
 *    Libpng only has three year fields.  One is a 2-byte unsigned integer
 *    that will hold years up to 65535.  The other two hold the date in text
 *    format, and will hold years up to 9999.
 *
 *    The integer is
 *        "uint16_t year" in png_time_struct.
 *
 *    The strings are
 *        "char* time_buffer" in png_struct and
 *        "near_time_buffer", which is a local character string in png.c.
 *
 *    There are seven time-related functions:
 *        png.c: png_convert_to_rfc_1123() in png.c
 *          (formerly png_convert_to_rfc_1152() in error)
 *        png_convert_from_struct_tm() in pngwrite.c, called in pngwrite.c
 *        png_convert_from_time_t() in pngwrite.c
 *        png_get_tIME() in pngget.c
 *        png_handle_tIME() in pngrutil.c, called in pngread.c
 *        png_set_tIME() in pngset.c
 *        png_write_tIME() in pngwutil.c, called in pngwrite.c
 *
 *    All handle dates properly in a Y2K environment.  The
 *    png_convert_from_time_t() function calls gmtime() to convert from system
 *    clock time, which returns (year - 1900), which we properly convert to
 *    the full 4-digit year.  There is a possibility that applications using
 *    libpng are not passing 4-digit years into the png_convert_to_rfc_1123()
 *    function, or that they are incorrectly passing only a 2-digit year
 *    instead of "year - 1900" into the png_convert_from_struct_tm() function,
 *    but this is not under our control.  The libpng documentation has always
 *    stated that it works with 4-digit years, and the APIs have been
 *    documented as such.
 *
 *    The tIME chunk itself is also Y2K compliant.  It uses a 2-byte unsigned
 *    integer to hold the year, and can hold years as large as 65535.
 *
 *    zlib, upon which libpng depends, is also Y2K compliant.  It contains
 *    no date-related code.
 *
 *       Glenn Randers-Pehrson
 *       libpng maintainer
 *       PNG Development Group
 */

#ifndef PNG_H
#define PNG_H

/* This is not the place to learn how to use libpng.  The file libpng.txt
 * describes how to use libpng, and the file example.c summarizes it
 * with some code on which to build.  This file is useful for looking
 * at the actual function definitions and structure components.
 */

/* Version information for png.h - this should match the version in png.c */
#define PNG_LIBPNG_VER_STRING "1.2.16"
#define PNG_HEADER_VERSION_STRING \
   " libpng version 1.2.16 - January 31, 2007 (header)\n"

#define PNG_LIBPNG_VER_SONUM   0
#define PNG_LIBPNG_VER_DLLNUM  13

/* These should match the first 3 components of PNG_LIBPNG_VER_STRING: */
#define PNG_LIBPNG_VER_MAJOR   1
#define PNG_LIBPNG_VER_MINOR   2
#define PNG_LIBPNG_VER_RELEASE 16
/* This should match the numeric part of the final component of
 * PNG_LIBPNG_VER_STRING, omitting any leading zero: */

#define PNG_LIBPNG_VER_BUILD  0

/* Release Status */
#define PNG_LIBPNG_BUILD_ALPHA    1
#define PNG_LIBPNG_BUILD_BETA     2
#define PNG_LIBPNG_BUILD_RC       3
#define PNG_LIBPNG_BUILD_STABLE   4
#define PNG_LIBPNG_BUILD_RELEASE_STATUS_MASK 7
  
/* Release-Specific Flags */
#define PNG_LIBPNG_BUILD_PATCH    8 /* Can be OR'ed with
                                       PNG_LIBPNG_BUILD_STABLE only */
#define PNG_LIBPNG_BUILD_PRIVATE 16 /* Cannot be OR'ed with
                                       PNG_LIBPNG_BUILD_SPECIAL */
#define PNG_LIBPNG_BUILD_SPECIAL 32 /* Cannot be OR'ed with
                                       PNG_LIBPNG_BUILD_PRIVATE */

#define PNG_LIBPNG_BUILD_BASE_TYPE PNG_LIBPNG_BUILD_STABLE

/* Careful here.  At one time, Guy wanted to use 082, but that would be octal.
 * We must not include leading zeros.
 * Versions 0.7 through 1.0.0 were in the range 0 to 100 here (only
 * version 1.0.0 was mis-numbered 100 instead of 10000).  From
 * version 1.0.1 it's    xxyyzz, where x=major, y=minor, z=release */
#define PNG_LIBPNG_VER 10216 /* 1.2.16 */

#include "zlib.h"

/* include all user configurable info, including optional assembler routines */
#include "pngconf.h"

/*
 * Added at libpng-1.2.8 */
/* Ref MSDN: Private as priority over Special
 * VS_FF_PRIVATEBUILD File *was not* built using standard release
 * procedures. If this value is given, the StringFileInfo block must
 * contain a PrivateBuild string. 
 *
 * VS_FF_SPECIALBUILD File *was* built by the original company using
 * standard release procedures but is a variation of the standard
 * file of the same version number. If this value is given, the
 * StringFileInfo block must contain a SpecialBuild string. 
 */

#define PNG_LIBPNG_BUILD_TYPE (PNG_LIBPNG_BUILD_BASE_TYPE)

/* Inhibit C++ name-mangling for libpng functions but not for system calls. */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* This file is arranged in several sections.  The first section contains
 * structure and type definitions.  The second section contains the external
 * library functions, while the third has the internal library functions,
 * which applications aren't expected to use directly.
 */

/* variables declared in png.c - only it needs to define PNG_NO_EXTERN */
#if !defined(PNG_NO_EXTERN) || defined(PNG_ALWAYS_EXTERN)
/* Version information for C files, stored in png.c.  This had better match
 * the version above.
 */
#ifdef PNG_USE_GLOBAL_ARRAYS
PNG_EXPORT_VAR (const char) png_libpng_ver[18];
  /* need room for 99.99.99beta99z */
#else
#define png_libpng_ver png_get_header_ver(NULL)
#endif

#ifdef PNG_USE_GLOBAL_ARRAYS
/* This was removed in version 1.0.5c */
/* Structures to facilitate easy interlacing.  See png.c for more details */
PNG_EXPORT_VAR (const int) png_pass_start[7];
PNG_EXPORT_VAR (const int) png_pass_inc[7];
PNG_EXPORT_VAR (const int) png_pass_ystart[7];
PNG_EXPORT_VAR (const int) png_pass_yinc[7];
PNG_EXPORT_VAR (const int) png_pass_mask[7];
PNG_EXPORT_VAR (const int) png_pass_dsp_mask[7];
/* This isn't currently used.  If you need it, see png.c for more details.
PNG_EXPORT_VAR (const int) png_pass_height[7];
*/
#endif

#endif /* PNG_NO_EXTERN */

/* Three color definitions.  The order of the red, green, and blue, (and the
 * exact size) is not important, although the size of the fields need to
 * be uint8_t or uint16_t (as defined below).
 */
typedef struct png_color_struct
{
   uint8_t red;
   uint8_t green;
   uint8_t blue;
} png_color;
typedef png_color *png_colorp;
typedef png_color **png_colorpp;

typedef struct png_color_16_struct
{
   uint8_t index;    /* used for palette files */
   uint16_t red;   /* for use in red green blue files */
   uint16_t green;
   uint16_t blue;
   uint16_t gray;  /* for use in grayscale files */
} png_color_16;
typedef png_color_16 *png_color_16p;
typedef png_color_16 **png_color_16pp;

typedef struct png_color_8_struct
{
   uint8_t red;   /* for use in red green blue files */
   uint8_t green;
   uint8_t blue;
   uint8_t gray;  /* for use in grayscale files */
   uint8_t alpha; /* for alpha channel files */
} png_color_8;
typedef png_color_8 *png_color_8p;
typedef png_color_8 **png_color_8pp;

/*
 * The following two structures are used for the in-core representation
 * of sPLT chunks.
 */
typedef struct png_sPLT_entry_struct
{
   uint16_t red;
   uint16_t green;
   uint16_t blue;
   uint16_t alpha;
   uint16_t frequency;
} png_sPLT_entry;
typedef png_sPLT_entry *png_sPLT_entryp;
typedef png_sPLT_entry **png_sPLT_entrypp;

/*  When the depth of the sPLT palette is 8 bits, the color and alpha samples
 *  occupy the LSB of their respective members, and the MSB of each member
 *  is zero-filled.  The frequency member always occupies the full 16 bits.
 */

typedef struct png_sPLT_struct
{
   char* name;           /* palette name */
   uint8_t depth;           /* depth of palette samples */
   png_sPLT_entryp entries;  /* palette entries */
   int32_t nentries;      /* number of palette entries */
} png_sPLT_t;
typedef png_sPLT_t *png_sPLT_tp;
typedef png_sPLT_t **png_sPLT_tpp;


/* Supported compression types for text in PNG files (tEXt, and zTXt).
 * The values of the PNG_TEXT_COMPRESSION_ defines should NOT be changed. */
#define PNG_TEXT_COMPRESSION_NONE_WR -3
#define PNG_TEXT_COMPRESSION_zTXt_WR -2
#define PNG_TEXT_COMPRESSION_NONE    -1
#define PNG_TEXT_COMPRESSION_zTXt     0
#define PNG_ITXT_COMPRESSION_NONE     1
#define PNG_ITXT_COMPRESSION_zTXt     2
#define PNG_TEXT_COMPRESSION_LAST     3  /* Not a valid value */

/* png_time is a way to hold the time in an machine independent way.
 * Two conversions are provided, both from time_t and struct tm.  There
 * is no portable way to convert to either of these structures, as far
 * as I know.  If you know of a portable way, send it to me.  As a side
 * note - PNG has always been Year 2000 compliant!
 */
typedef struct png_time_struct
{
   uint16_t year; /* full year, as in, 1995 */
   uint8_t month;   /* month of year, 1 - 12 */
   uint8_t day;     /* day of month, 1 - 31 */
   uint8_t hour;    /* hour of day, 0 - 23 */
   uint8_t minute;  /* minute of hour, 0 - 59 */
   uint8_t second;  /* second of minute, 0 - 60 (for leap seconds) */
} png_time;
typedef png_time *png_timep;
typedef png_time **png_timepp;

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
/* png_unknown_chunk is a structure to hold queued chunks for which there is
 * no specific support.  The idea is that we can use this to queue
 * up private chunks for output even though the library doesn't actually
 * know about their semantics.
 */
typedef struct png_unknown_chunk_t
{
    uint8_t name[5];
    uint8_t *data;
    size_t size;

    /* libpng-using applications should NOT directly modify this byte. */
    uint8_t location; /* mode of operation at read time */
}
png_unknown_chunk;
typedef png_unknown_chunk *png_unknown_chunkp;
typedef png_unknown_chunk **png_unknown_chunkpp;
#endif

/* png_info is a structure that holds the information in a PNG file so
 * that the application can find out the characteristics of the image.
 * If you are reading the file, this structure will tell you what is
 * in the PNG file.  If you are writing the file, fill in the information
 * you want to put into the PNG file, then call png_write_info().
 * The names chosen should be very close to the PNG specification, so
 * consult that document for information about the meaning of each field.
 *
 * With libpng < 0.95, it was only possible to directly set and read the
 * the values in the png_info_struct, which meant that the contents and
 * order of the values had to remain fixed.  With libpng 0.95 and later,
 * however, there are now functions that abstract the contents of
 * png_info_struct from the application, so this makes it easier to use
 * libpng with dynamic libraries, and even makes it possible to use
 * libraries that don't have all of the libpng ancillary chunk-handing
 * functionality.
 *
 * In any case, the order of the parameters in png_info_struct should NOT
 * be changed for as long as possible to keep compatibility with applications
 * that use the old direct-access method with png_info_struct.
 *
 * The following members may have allocated storage attached that should be
 * cleaned up before the structure is discarded: palette, trans, text,
 * pcal_purpose, pcal_units, pcal_params, hist, iccp_name, iccp_profile,
 * splt_palettes, scal_unit, row_pointers, and unknowns.   By default, these
 * are automatically freed when the info structure is deallocated, if they were
 * allocated internally by libpng.  This behavior can be changed by means
 * of the png_data_freer() function.
 *
 * More allocation details: all the chunk-reading functions that
 * change these members go through the corresponding png_set_*
 * functions.  A function to clear these members is available: see
 * png_free_data().  The png_set_* functions do not depend on being
 * able to point info structure members to any of the storage they are
 * passed (they make their own copies), EXCEPT that the png_set_text
 * functions use the same storage passed to them in the text_ptr or
 * itxt_ptr structure argument, and the png_set_rows and png_set_unknowns
 * functions do not make their own copies.
 */
typedef struct png_info_struct
{
   /* the following are necessary for every PNG file */
   uint32_t width;       /* width of image in pixels (from IHDR) */
   uint32_t height;      /* height of image in pixels (from IHDR) */
   uint32_t valid;       /* valid chunk data (see PNG_INFO_ below) */
   uint32_t rowbytes;    /* bytes needed to hold an untransformed row */
   png_colorp palette;      /* array of color values (valid & PNG_INFO_PLTE) */
   uint16_t num_palette; /* number of color entries in "palette" (PLTE) */
   uint16_t num_trans;   /* number of transparent palette color (tRNS) */
   uint8_t bit_depth;      /* 1, 2, 4, 8, or 16 bits/channel (from IHDR) */
   uint8_t color_type;     /* see PNG_COLOR_TYPE_ below (from IHDR) */
   /* The following three should have been named *_method not *_type */
   uint8_t compression_type; /* must be PNG_COMPRESSION_TYPE_BASE (IHDR) */
   uint8_t filter_type;    /* must be PNG_FILTER_TYPE_BASE (from IHDR) */
   uint8_t interlace_type; /* One of PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7 */

   /* The following is informational only on read, and not used on writes. */
   uint8_t channels;       /* number of data channels per pixel (1, 2, 3, 4) */
   uint8_t pixel_depth;    /* number of bits per pixel */
   uint8_t spare_byte;     /* to align the data, and for future use */
   uint8_t signature[8];   /* magic bytes read by libpng from start of file */

   /* The rest of the data is optional.  If you are reading, check the
    * valid field to see if the information in these are valid.  If you
    * are writing, set the valid field to those chunks you want written,
    * and initialize the appropriate fields below.
    */

#if defined(PNG_sRGB_SUPPORTED)
    /* GR-P, 0.96a */
    /* Data valid if (valid & PNG_INFO_sRGB) non-zero. */
   uint8_t srgb_intent; /* sRGB rendering intent [0, 1, 2, or 3] */
#endif

#if defined(PNG_sBIT_SUPPORTED)
   /* The sBIT chunk specifies the number of significant high-order bits
    * in the pixel data.  Values are in the range [1, bit_depth], and are
    * only specified for the channels in the pixel data.  The contents of
    * the low-order bits is not specified.  Data is valid if
    * (valid & PNG_INFO_sBIT) is non-zero.
    */
   png_color_8 sig_bit; /* significant bits in color channels */
#endif

#if defined(PNG_tRNS_SUPPORTED) || defined(PNG_READ_EXPAND_SUPPORTED)
   /* The tRNS chunk supplies transparency data for paletted images and
    * other image types that don't need a full alpha channel.  There are
    * "num_trans" transparency values for a paletted image, stored in the
    * same order as the palette colors, starting from index 0.  Values
    * for the data are in the range [0, 255], ranging from fully transparent
    * to fully opaque, respectively.  For non-paletted images, there is a
    * single color specified that should be treated as fully transparent.
    * Data is valid if (valid & PNG_INFO_tRNS) is non-zero.
    */
   uint8_t* trans; /* transparent values for paletted image */
   png_color_16 trans_values; /* transparent color for non-palette image */
#endif

#if defined(PNG_pHYs_SUPPORTED)
   /* The pHYs chunk gives the physical pixel density of the image for
    * display or printing in "phys_unit_type" units (see PNG_RESOLUTION_
    * defines below).  Data is valid if (valid & PNG_INFO_pHYs) is non-zero.
    */
   uint32_t x_pixels_per_unit; /* horizontal pixel density */
   uint32_t y_pixels_per_unit; /* vertical pixel density */
   uint8_t phys_unit_type; /* resolution type (see PNG_RESOLUTION_ below) */
#endif

#if defined(PNG_hIST_SUPPORTED)
   /* The hIST chunk contains the relative frequency or importance of the
    * various palette entries, so that a viewer can intelligently select a
    * reduced-color palette, if required.  Data is an array of "num_palette"
    * values in the range [0,65535]. Data valid if (valid & PNG_INFO_hIST)
    * is non-zero.
    */
   uint16_t* hist;
#endif

#if defined(PNG_pCAL_SUPPORTED)
   /* The pCAL chunk describes a transformation between the stored pixel
    * values and original physical data values used to create the image.
    * The integer range [0, 2^bit_depth - 1] maps to the floating-point
    * range given by [pcal_X0, pcal_X1], and are further transformed by a
    * (possibly non-linear) transformation function given by "pcal_type"
    * and "pcal_params" into "pcal_units".  Please see the PNG_EQUATION_
    * defines below, and the PNG-Group's PNG extensions document for a
    * complete description of the transformations and how they should be
    * implemented, and for a description of the ASCII parameter strings.
    * Data values are valid if (valid & PNG_INFO_pCAL) non-zero.
    */
   char* pcal_purpose;  /* pCAL chunk description string */
   int32_t pcal_X0;      /* minimum value */
   int32_t pcal_X1;      /* maximum value */
   char* pcal_units;    /* Latin-1 string giving physical units */
   png_charpp pcal_params;  /* ASCII strings containing parameter values */
   uint8_t pcal_type;      /* equation type (see PNG_EQUATION_ below) */
   uint8_t pcal_nparams;   /* number of parameters given in pcal_params */
#endif

/* New members added in libpng-1.0.6 */
#ifdef PNG_FREE_ME_SUPPORTED
   uint32_t free_me;     /* flags items libpng is responsible for freeing */
#endif

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
   /* storage for unknown chunks that the library doesn't recognize. */
   png_unknown_chunkp unknown_chunks;
   size_t unknown_chunks_num;
#endif

#if defined(PNG_sPLT_SUPPORTED)
   /* data on sPLT chunks (there may be more than one). */
   png_sPLT_tp splt_palettes;
   uint32_t splt_palettes_num;
#endif

#if defined(PNG_INFO_IMAGE_SUPPORTED)
   /* Memory has been allocated if (valid & PNG_ALLOCATED_INFO_ROWS) non-zero */
   /* Data valid if (valid & PNG_INFO_IDAT) non-zero */
   png_bytepp row_pointers;        /* the image bits */
#endif
} png_info;

typedef png_info *png_infop;
typedef png_info **png_infopp;

/* Maximum positive integer used in PNG is (2^31)-1 */
#define PNG_UINT_31_MAX ((uint32_t)0x7fffffffL)
#define PNG_UINT_32_MAX ((uint32_t)(-1))
#define PNG_SIZE_MAX ((size_t)(-1))
#if defined(PNG_1_0_X) || defined (PNG_1_2_X)
/* PNG_MAX_UINT is deprecated; use PNG_UINT_31_MAX instead. */
#define PNG_MAX_UINT PNG_UINT_31_MAX
#endif

/* These describe the color_type field in png_info. */
/* color type masks */
#define PNG_COLOR_MASK_PALETTE    1
#define PNG_COLOR_MASK_COLOR      2
#define PNG_COLOR_MASK_ALPHA      4

/* color types.  Note that not all combinations are legal */
#define PNG_COLOR_TYPE_GRAY 0
#define PNG_COLOR_TYPE_PALETTE  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
#define PNG_COLOR_TYPE_RGB        (PNG_COLOR_MASK_COLOR)
#define PNG_COLOR_TYPE_RGB_ALPHA  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_GRAY_ALPHA (PNG_COLOR_MASK_ALPHA)
/* aliases */
#define PNG_COLOR_TYPE_RGBA  PNG_COLOR_TYPE_RGB_ALPHA
#define PNG_COLOR_TYPE_GA  PNG_COLOR_TYPE_GRAY_ALPHA

/* This is for compression type. PNG 1.0-1.2 only define the single type. */
#define PNG_COMPRESSION_TYPE_BASE 0 /* Deflate method 8, 32K window */
#define PNG_COMPRESSION_TYPE_DEFAULT PNG_COMPRESSION_TYPE_BASE

/* This is for filter type. PNG 1.0-1.2 only define the single type. */
#define PNG_FILTER_TYPE_BASE      0 /* Single row per-byte filtering */
#define PNG_INTRAPIXEL_DIFFERENCING 64 /* Used only in MNG datastreams */
#define PNG_FILTER_TYPE_DEFAULT   PNG_FILTER_TYPE_BASE

/* These are for the interlacing type.  These values should NOT be changed. */
#define PNG_INTERLACE_NONE        0 /* Non-interlaced image */
#define PNG_INTERLACE_ADAM7       1 /* Adam7 interlacing */
#define PNG_INTERLACE_LAST        2 /* Not a valid value */

/* These are for the oFFs chunk.  These values should NOT be changed. */
#define PNG_OFFSET_PIXEL          0 /* Offset in pixels */
#define PNG_OFFSET_MICROMETER     1 /* Offset in micrometers (1/10^6 meter) */
#define PNG_OFFSET_LAST           2 /* Not a valid value */

/* These are for the pCAL chunk.  These values should NOT be changed. */
#define PNG_EQUATION_LINEAR       0 /* Linear transformation */
#define PNG_EQUATION_BASE_E       1 /* Exponential base e transform */
#define PNG_EQUATION_ARBITRARY    2 /* Arbitrary base exponential transform */
#define PNG_EQUATION_HYPERBOLIC   3 /* Hyperbolic sine transformation */
#define PNG_EQUATION_LAST         4 /* Not a valid value */

/* These are for the sCAL chunk.  These values should NOT be changed. */
#define PNG_SCALE_UNKNOWN         0 /* unknown unit (image scale) */
#define PNG_SCALE_METER           1 /* meters per pixel */
#define PNG_SCALE_RADIAN          2 /* radians per pixel */
#define PNG_SCALE_LAST            3 /* Not a valid value */

/* These are for the pHYs chunk.  These values should NOT be changed. */
#define PNG_RESOLUTION_UNKNOWN    0 /* pixels/unknown unit (aspect ratio) */
#define PNG_RESOLUTION_METER      1 /* pixels/meter */
#define PNG_RESOLUTION_LAST       2 /* Not a valid value */

/* These are for the sRGB chunk.  These values should NOT be changed. */
#define PNG_sRGB_INTENT_PERCEPTUAL 0
#define PNG_sRGB_INTENT_RELATIVE   1
#define PNG_sRGB_INTENT_SATURATION 2
#define PNG_sRGB_INTENT_ABSOLUTE   3
#define PNG_sRGB_INTENT_LAST       4 /* Not a valid value */

/* This is for text chunks */
#define PNG_KEYWORD_MAX_LENGTH     79

/* Maximum number of entries in PLTE/sPLT/tRNS arrays */
#define PNG_MAX_PALETTE_LENGTH    256

/* These determine if an ancillary chunk's data has been successfully read
 * from the PNG header, or if the application has filled in the corresponding
 * data in the info_struct to be written into the output file.  The values
 * of the PNG_INFO_<chunk> defines should NOT be changed.
 */
#define PNG_INFO_gAMA 0x0001
#define PNG_INFO_sBIT 0x0002
#define PNG_INFO_cHRM 0x0004
#define PNG_INFO_PLTE 0x0008
#define PNG_INFO_tRNS 0x0010
#define PNG_INFO_bKGD 0x0020
#define PNG_INFO_hIST 0x0040
#define PNG_INFO_pHYs 0x0080
#define PNG_INFO_oFFs 0x0100
#define PNG_INFO_tIME 0x0200
#define PNG_INFO_pCAL 0x0400
#define PNG_INFO_sRGB 0x0800   /* GR-P, 0.96a */
#define PNG_INFO_iCCP 0x1000   /* ESR, 1.0.6 */
#define PNG_INFO_sPLT 0x2000   /* ESR, 1.0.6 */
#define PNG_INFO_sCAL 0x4000   /* ESR, 1.0.6 */
#define PNG_INFO_IDAT 0x8000L  /* ESR, 1.0.6 */

/* This is used for the transformation routines, as some of them
 * change these values for the row.  It also should enable using
 * the routines for other purposes.
 */
typedef struct png_row_info_struct
{
   uint32_t width; /* width of row */
   uint32_t rowbytes; /* number of bytes in row */
   uint8_t color_type; /* color type of row */
   uint8_t bit_depth; /* bit depth of row */
   uint8_t channels; /* number of channels (1, 2, 3, or 4) */
   uint8_t pixel_depth; /* bits per pixel (depth * channels) */
} png_row_info;

typedef png_row_info *png_row_infop;
typedef png_row_info **png_row_infopp;

/* These are the function types for the I/O functions and for the functions
 * that allow the user to override the default I/O functions with his or her
 * own.  The png_error_ptr type should match that of user-supplied warning
 * and error functions, while the png_rw_ptr type should match that of the
 * user read/write data functions.
 */
typedef struct png_struct_def png_struct;
typedef png_struct *png_structp;

typedef void (*png_error_ptr) (png_structp, const char*);
typedef void (*png_rw_ptr) (png_structp, uint8_t*, size_t);
typedef void (* png_flush_ptr) (png_structp);
typedef void (*png_read_status_ptr) (png_structp, uint32_t, int);
typedef void (*png_write_status_ptr) (png_structp, uint32_t, int);

#ifdef PNG_PROGRESSIVE_READ_SUPPORTED
typedef void (*png_progressive_info_ptr) (png_structp, png_infop);
typedef void (*png_progressive_end_ptr) (png_structp, png_infop);
typedef void (*png_progressive_row_ptr) (png_structp, uint8_t*, uint32_t, int);
#endif

#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED) || \
    defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED) || \
    defined(PNG_LEGACY_SUPPORTED)
typedef void (*png_user_transform_ptr) (png_structp, png_row_infop, uint8_t*);
#endif

#if defined(PNG_USER_CHUNKS_SUPPORTED)
typedef int (*png_user_chunk_ptr) (png_structp, png_unknown_chunkp);
#endif
#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
typedef void (*png_unknown_chunk_ptr) (png_structp);
#endif

/* Transform masks for the high-level interface */
#define PNG_TRANSFORM_IDENTITY       0x0000    /* read and write */
#define PNG_TRANSFORM_STRIP_16       0x0001    /* read only */
#define PNG_TRANSFORM_STRIP_ALPHA    0x0002    /* read only */
#define PNG_TRANSFORM_PACKING        0x0004    /* read and write */
#define PNG_TRANSFORM_PACKSWAP       0x0008    /* read and write */
#define PNG_TRANSFORM_EXPAND         0x0010    /* read only */
#define PNG_TRANSFORM_INVERT_MONO    0x0020    /* read and write */
#define PNG_TRANSFORM_SHIFT          0x0040    /* read and write */
#define PNG_TRANSFORM_BGR            0x0080    /* read and write */
#define PNG_TRANSFORM_SWAP_ALPHA     0x0100    /* read and write */
#define PNG_TRANSFORM_SWAP_ENDIAN    0x0200    /* read and write */
#define PNG_TRANSFORM_INVERT_ALPHA   0x0400    /* read and write */
#define PNG_TRANSFORM_STRIP_FILLER   0x0800    /* WRITE only */

/* Flags for MNG supported features */
#define PNG_FLAG_MNG_EMPTY_PLTE     0x01
#define PNG_FLAG_MNG_FILTER_64      0x04
#define PNG_ALL_MNG_FEATURES        0x05

typedef void* (*png_malloc_ptr) (png_structp, size_t);
typedef void (*png_free_ptr) (png_structp, void*);

/* The structure that holds the information to read and write PNG files.
 * The only people who need to care about what is inside of this are the
 * people who will be modifying the library for their own special needs.
 * It should NOT be accessed directly by an application, except to store
 * the jmp_buf.
 */

struct png_struct_def
{
#ifdef PNG_SETJMP_SUPPORTED
   jmp_buf jmpbuf;            /* used in png_error */
#endif
   png_error_ptr error_fn;    /* function for printing errors and aborting */
   png_error_ptr warning_fn;  /* function for printing warnings */
   void* error_ptr;       /* user supplied struct for error functions */
   png_rw_ptr write_data_fn;  /* function for writing output data */
   png_rw_ptr read_data_fn;   /* function for reading input data */
   void* io_ptr;          /* ptr to application struct for I/O functions */

#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED)
   png_user_transform_ptr read_user_transform_fn; /* user read transform */
#endif

#if defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
   png_user_transform_ptr write_user_transform_fn; /* user write transform */
#endif

/* These were added in libpng-1.0.2 */
#if defined(PNG_USER_TRANSFORM_PTR_SUPPORTED)
#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED) || \
    defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
   void* user_transform_ptr; /* user supplied struct for user transform */
   uint8_t user_transform_depth;    /* bit depth of user transformed pixels */
   uint8_t user_transform_channels; /* channels in user transformed pixels */
#endif
#endif

   uint32_t mode;          /* tells us where we are in the PNG file */
   uint32_t flags;         /* flags indicating various things to libpng */
   uint32_t transformations; /* which transformations to perform */

   z_stream zstream;          /* pointer to decompression structure (below) */
   uint8_t* zbuf;            /* buffer for zlib */
   size_t zbuf_size;      /* size of zbuf */
   int zlib_level;            /* holds zlib compression level */
   int zlib_method;           /* holds zlib compression method */
   int zlib_window_bits;      /* holds zlib compression window bits */
   int zlib_mem_level;        /* holds zlib compression memory level */
   int zlib_strategy;         /* holds zlib compression strategy */

   uint32_t width;         /* width of image in pixels */
   uint32_t height;        /* height of image in pixels */
   uint32_t num_rows;      /* number of rows in current pass */
   uint32_t usr_width;     /* width of row at start of write */
   uint32_t rowbytes;      /* size of row in bytes */
   uint32_t irowbytes;     /* size of current interlaced row in bytes */
   uint32_t iwidth;        /* width of current interlaced row in pixels */
   uint32_t row_number;    /* current row in interlace pass */
   uint8_t* prev_row;        /* buffer to save previous (unfiltered) row */
   uint8_t* row_buf;         /* buffer to save current (unfiltered) row */
   uint8_t* sub_row;         /* buffer to save "sub" row when filtering */
   uint8_t* up_row;          /* buffer to save "up" row when filtering */
   uint8_t* avg_row;         /* buffer to save "avg" row when filtering */
   uint8_t* paeth_row;       /* buffer to save "Paeth" row when filtering */
   png_row_info row_info;     /* used for transformation routines */

   uint32_t idat_size;     /* current IDAT size for read */
   uint32_t crc;           /* current chunk CRC value */
   png_colorp palette;        /* palette from the input file */
   uint16_t num_palette;   /* number of color entries in palette */
   uint16_t num_trans;     /* number of transparency values */
   uint8_t chunk_name[5];    /* null-terminated name of current chunk */
   uint8_t compression;      /* file compression type (always 0) */
   uint8_t filter;           /* file filter type (always 0) */
   uint8_t interlaced;       /* PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7 */
   uint8_t pass;             /* current interlace pass (0 - 6) */
   uint8_t do_filter;        /* row filter flags (see PNG_FILTER_ below ) */
   uint8_t color_type;       /* color type of file */
   uint8_t bit_depth;        /* bit depth of file */
   uint8_t usr_bit_depth;    /* bit depth of users row */
   uint8_t pixel_depth;      /* number of bits per pixel */
   uint8_t channels;         /* number of channels in file */
   uint8_t usr_channels;     /* channels at start of write */
   uint8_t sig_bytes;        /* magic bytes read/written from start of file */

#if defined(PNG_READ_FILLER_SUPPORTED) || defined(PNG_WRITE_FILLER_SUPPORTED)
#ifdef PNG_LEGACY_SUPPORTED
   uint8_t filler;           /* filler byte for pixel expansion */
#else
   uint16_t filler;           /* filler bytes for pixel expansion */
#endif
#endif

#if defined(PNG_WRITE_FLUSH_SUPPORTED)
   png_flush_ptr output_flush_fn;/* Function for flushing output */
   uint32_t flush_dist;    /* how many rows apart to flush, 0 - no flush */
   uint32_t flush_rows;    /* number of rows written since last flush */
#endif

#if defined(PNG_sBIT_SUPPORTED)
   png_color_8 sig_bit;       /* significant bits in each available channel */
#endif

#if defined(PNG_READ_SHIFT_SUPPORTED) || defined(PNG_WRITE_SHIFT_SUPPORTED)
   png_color_8 shift;         /* shift for significant bit tranformation */
#endif

#if defined(PNG_tRNS_SUPPORTED) || defined(PNG_READ_EXPAND_SUPPORTED)
   uint8_t* trans;           /* transparency values for paletted files */
   png_color_16 trans_values; /* transparency values for non-paletted files */
#endif

   png_read_status_ptr read_row_fn;   /* called after each row is decoded */
   png_write_status_ptr write_row_fn; /* called after each row is encoded */
#ifdef PNG_PROGRESSIVE_READ_SUPPORTED
   png_progressive_info_ptr info_fn; /* called after header data fully read */
   png_progressive_row_ptr row_fn;   /* called after each prog. row is decoded */
   png_progressive_end_ptr end_fn;   /* called after image is complete */
   uint8_t* save_buffer_ptr;        /* current location in save_buffer */
   uint8_t* save_buffer;            /* buffer for previously read data */
   uint8_t* current_buffer_ptr;     /* current location in current_buffer */
   uint8_t* current_buffer;         /* buffer for recently used data */
   uint32_t push_length;          /* size of current input chunk */
   uint32_t skip_length;          /* bytes to skip in input data */
   size_t save_buffer_size;      /* amount of data now in save_buffer */
   size_t save_buffer_max;       /* total size of save_buffer */
   size_t buffer_size;           /* total amount of available input data */
   size_t current_buffer_size;   /* amount of data now in current_buffer */
   int process_mode;                 /* what push library is currently doing */
   int cur_palette;                  /* current push library palette index */

#endif /* PNG_PROGRESSIVE_READ_SUPPORTED */

#if defined(PNG_READ_DITHER_SUPPORTED)
   uint8_t* palette_lookup;         /* lookup table for dithering */
   uint8_t* dither_index;           /* index translation for palette files */
#endif

#if defined(PNG_READ_DITHER_SUPPORTED) || defined(PNG_hIST_SUPPORTED)
   uint16_t* hist;                /* histogram */
#endif

#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED)
   uint8_t heuristic_method;        /* heuristic for row filter selection */
   uint8_t num_prev_filters;        /* number of weights for previous rows */
   uint8_t* prev_filters;           /* filter type(s) of previous row(s) */
   uint16_t* filter_weights;      /* weight(s) for previous line(s) */
   uint16_t* inv_filter_weights;  /* 1/weight(s) for previous line(s) */
   uint16_t* filter_costs;        /* relative filter calculation cost */
   uint16_t* inv_filter_costs;    /* 1/relative filter calculation cost */
#endif

/* New members added in libpng-1.0.6 */

#ifdef PNG_FREE_ME_SUPPORTED
   uint32_t free_me;       /* flags items libpng is responsible for freeing */
#endif

#if defined(PNG_USER_CHUNKS_SUPPORTED)
   void* user_chunk_ptr;
   png_user_chunk_ptr read_user_chunk_fn; /* user read chunk handler */
#endif

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
   int num_chunk_list;
   uint8_t* chunk_list;
#endif

/* New members added in libpng-1.0.3 */
#if defined(PNG_READ_RGB_TO_GRAY_SUPPORTED)
   uint8_t rgb_to_gray_status;
   /* These were changed from uint8_t in libpng-1.0.6 */
   uint16_t rgb_to_gray_red_coeff;
   uint16_t rgb_to_gray_green_coeff;
   uint16_t rgb_to_gray_blue_coeff;
#endif
   uint32_t mng_features_permitted;

/* New members added in libpng-1.0.2 but first enabled by default in 1.2.0 */
#ifdef PNG_USER_MEM_SUPPORTED
   void* mem_ptr;                /* user supplied struct for mem functions */
   png_malloc_ptr malloc_fn;         /* function for allocating memory */
   png_free_ptr free_fn;             /* function for freeing memory */
#endif

/* New member added in libpng-1.0.13 and 1.2.0 */
   uint8_t* big_row_buf;         /* buffer to save current (unfiltered) row */

#if defined(PNG_READ_DITHER_SUPPORTED)
/* The following three members were added at version 1.0.14 and 1.2.4 */
   uint8_t* dither_sort;            /* working sort array */
   uint8_t* index_to_palette;       /* where the original index currently is */
                                     /* in the palette */
   uint8_t* palette_to_index;       /* which original index points to this */
                                     /* palette color */
#endif

/* New members added in libpng-1.0.16 and 1.2.6 */
   uint8_t compression_type;

#ifdef PNG_SET_USER_LIMITS_SUPPORTED
   uint32_t user_width_max;
   uint32_t user_height_max;
#endif

};


/* This triggers a compiler error in png.c, if png.c and png.h
 * do not agree upon the version number.
 */
typedef png_structp version_1_2_16;

typedef png_struct **png_structpp;

/* Here are the function definitions most commonly used.  This is not
 * the place to find out how to use libpng.  See libpng.txt for the
 * full explanation, see example.c for the summary.  This just provides
 * a simple one line description of the use of each function.
 */

/* Returns the version number of the library */
extern uint32_t png_access_version_number(void);

/* Tell lib we have already handled the first <num_bytes> magic bytes.
 * Handling more than 8 bytes from the beginning of the file is an error.
 */
extern void png_set_sig_bytes(png_structp png_ptr, int num_bytes);

/* Check sig[start] through sig[start + num_to_check - 1] to see if it's a
 * PNG file.  Returns zero if the supplied bytes match the 8-byte PNG
 * signature, and non-zero otherwise.  Having num_to_check == 0 or
 * start > 7 will always fail (ie return non-zero).
 */
extern int  png_sig_cmp(uint8_t* sig, size_t start, size_t num_to_check);

/* Simple signature checking function.  This is the same as calling
 * png_check_sig(sig, n) := !png_sig_cmp(sig, 0, n).
 */
extern int png_check_sig(uint8_t* sig, int num);

/* Allocate and initialize png_ptr struct for reading, and any other memory. */
extern png_structp png_create_read_struct(const char* user_png_ver, void* error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);

/* Allocate and initialize png_ptr struct for writing, and any other memory */
extern png_structp png_create_write_struct(const char* user_png_ver, void* error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);

#ifdef PNG_WRITE_SUPPORTED
extern uint32_t png_get_compression_buffer_size(png_structp png_ptr);
#endif

#ifdef PNG_WRITE_SUPPORTED
extern void png_set_compression_buffer_size(png_structp png_ptr, uint32_t size);
#endif

/* Reset the compression stream */
extern int png_reset_zstream(png_structp png_ptr);

/* New functions added in libpng-1.0.2 (not enabled by default until 1.2.0) */
#ifdef PNG_USER_MEM_SUPPORTED
extern png_structp png_create_read_struct_2(const char* user_png_ver, void* error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn, void* mem_ptr, png_malloc_ptr malloc_fn, png_free_ptr free_fn);
extern png_structp png_create_write_struct_2(const char* user_png_ver, void* error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn, void* mem_ptr, png_malloc_ptr malloc_fn, png_free_ptr free_fn);
#endif

/* Write a PNG chunk - size, type, (optional) data, CRC. */
extern void png_write_chunk(png_structp png_ptr, uint8_t* chunk_name, uint8_t* data, size_t length);

/* Write the start of a PNG chunk - length and chunk name. */
extern void png_write_chunk_start(png_structp png_ptr, uint8_t* chunk_name, uint32_t length);

/* Write the data of a PNG chunk started with png_write_chunk_start(). */
extern void png_write_chunk_data(png_structp png_ptr, uint8_t* data, size_t length);

/* Finish a chunk started with png_write_chunk_start() (includes CRC). */
extern void png_write_chunk_end(png_structp png_ptr);

/* Allocate and initialize the info structure */
extern png_infop png_create_info_struct(png_structp png_ptr);

#if defined(PNG_1_0_X) || defined (PNG_1_2_X)
/* Initialize the info structure (old interface - DEPRECATED) */
extern void png_info_init(png_infop info_ptr);
#undef png_info_init
#define png_info_init(info_ptr) png_info_init_3(&info_ptr, sizeof(png_info));
#endif

extern void png_info_init_3(png_infopp info_ptr, size_t png_info_struct_size);

/* Writes all the PNG information before the image. */
extern void png_write_info_before_PLTE(png_structp png_ptr, png_infop info_ptr);
extern void png_write_info(png_structp png_ptr, png_infop info_ptr);

#ifndef PNG_NO_SEQUENTIAL_READ_SUPPORTED
/* read the information before the actual image data. */
extern void png_read_info(png_structp png_ptr, png_infop info_ptr);
#endif

#if defined(PNG_READ_EXPAND_SUPPORTED)
/* Expand data to 24-bit RGB, or 8-bit grayscale, with alpha if available. */
extern void png_set_expand(png_structp png_ptr);
#if !defined(PNG_1_0_X)
extern void png_set_expand_gray_1_2_4_to_8(png_structp png_ptr);
#endif
extern void png_set_palette_to_rgb(png_structp png_ptr);
extern void png_set_tRNS_to_alpha(png_structp png_ptr);
#if defined(PNG_1_0_X) || defined (PNG_1_2_X)
/* Deprecated */
extern void png_set_gray_1_2_4_to_8(png_structp png_ptr);
#endif
#endif

#if defined(PNG_READ_BGR_SUPPORTED) || defined(PNG_WRITE_BGR_SUPPORTED)
/* Use blue, green, red order for pixels. */
extern void png_set_bgr(png_structp png_ptr);
#endif

#if defined(PNG_READ_GRAY_TO_RGB_SUPPORTED)
/* Expand the grayscale to 24-bit RGB if necessary. */
extern void png_set_gray_to_rgb(png_structp png_ptr);
#endif

#if defined(PNG_READ_RGB_TO_GRAY_SUPPORTED)
/* Reduce RGB to grayscale. */
#ifdef PNG_FLOATING_POINT_SUPPORTED
extern void png_set_rgb_to_gray(png_structp png_ptr, int error_action, double red, double green);
#endif
extern void png_set_rgb_to_gray_fixed(png_structp png_ptr, int error_action, png_fixed_point red, png_fixed_point green);
extern uint8_t png_get_rgb_to_gray_status(png_structp png_ptr);
#endif

extern void png_build_grayscale_palette(int bit_depth, png_colorp palette);

#if defined(PNG_READ_STRIP_ALPHA_SUPPORTED)
extern void png_set_strip_alpha(png_structp png_ptr);
#endif

#if defined(PNG_READ_SWAP_ALPHA_SUPPORTED) || defined(PNG_WRITE_SWAP_ALPHA_SUPPORTED)
extern void png_set_swap_alpha(png_structp png_ptr);
#endif

#if defined(PNG_READ_INVERT_ALPHA_SUPPORTED) || defined(PNG_WRITE_INVERT_ALPHA_SUPPORTED)
extern void png_set_invert_alpha(png_structp png_ptr);
#endif

#if defined(PNG_READ_FILLER_SUPPORTED) || defined(PNG_WRITE_FILLER_SUPPORTED)
/* Add a filler byte to 8-bit Gray or 24-bit RGB images. */
extern void png_set_filler(png_structp png_ptr, uint32_t filler, int flags);
/* The values of the PNG_FILLER_ defines should NOT be changed */
#define PNG_FILLER_BEFORE 0
#define PNG_FILLER_AFTER 1
/* Add an alpha byte to 8-bit Gray or 24-bit RGB images. */
#if !defined(PNG_1_0_X)
extern void png_set_add_alpha(png_structp png_ptr, uint32_t filler, int flags);
#endif
#endif /* PNG_READ_FILLER_SUPPORTED || PNG_WRITE_FILLER_SUPPORTED */

#if defined(PNG_READ_SWAP_SUPPORTED) || defined(PNG_WRITE_SWAP_SUPPORTED)
/* Swap bytes in 16-bit depth files. */
extern void png_set_swap(png_structp png_ptr);
#endif

#if defined(PNG_READ_PACK_SUPPORTED) || defined(PNG_WRITE_PACK_SUPPORTED)
/* Use 1 byte per pixel in 1, 2, or 4-bit depth files. */
extern void png_set_packing(png_structp png_ptr);
#endif

#if defined(PNG_READ_SHIFT_SUPPORTED) || defined(PNG_WRITE_SHIFT_SUPPORTED)
/* Converts files to legal bit depths. */
extern void png_set_shift(png_structp png_ptr, png_color_8p true_bits);
#endif

#if defined(PNG_READ_INTERLACING_SUPPORTED) || defined(PNG_WRITE_INTERLACING_SUPPORTED)
/* Have the code handle the interlacing.  Returns the number of passes. */
extern int png_set_interlace_handling(png_structp png_ptr);
#endif

#if defined(PNG_READ_INVERT_SUPPORTED) || defined(PNG_WRITE_INVERT_SUPPORTED)
/* Invert monochrome files */
extern void png_set_invert_mono(png_structp png_ptr);
#endif

#if defined(PNG_READ_16_TO_8_SUPPORTED)
/* strip the second byte of information from a 16-bit depth file. */
extern void png_set_strip_16(png_structp png_ptr);
#endif

#if defined(PNG_READ_DITHER_SUPPORTED)
/* Turn on dithering, and reduce the palette to the number of colors available. */
extern void png_set_dither(png_structp png_ptr, png_colorp palette, int num_palette, int maximum_colors, uint16_t* histogram, int full_dither);
#endif

extern void png_permit_empty_plte(png_structp png_ptr, int empty_plte_permitted);

#if defined(PNG_WRITE_FLUSH_SUPPORTED)
/* Set how many lines between output flushes - 0 for no flushing */
extern void png_set_flush(png_structp png_ptr, int nrows);
/* Flush the current PNG output buffer */
extern void png_write_flush(png_structp png_ptr);
#endif

/* optional update palette with requested transformations */
extern void png_start_read_image(png_structp png_ptr);

/* optional call to update the users info structure */
extern void png_read_update_info(png_structp png_ptr, png_infop info_ptr);

#ifndef PNG_NO_SEQUENTIAL_READ_SUPPORTED
/* read one or more rows of image data. */
extern void png_read_rows(png_structp png_ptr, png_bytepp row, png_bytepp display_row, uint32_t num_rows);
#endif

#ifndef PNG_NO_SEQUENTIAL_READ_SUPPORTED
/* read a row of data. */
extern void png_read_row(png_structp png_ptr, uint8_t* row, uint8_t* display_row);
#endif

#ifndef PNG_NO_SEQUENTIAL_READ_SUPPORTED
/* read the whole image into memory at once. */
extern void png_read_image(png_structp png_ptr, png_bytepp image);
#endif

/* write a row of image data */
extern void png_write_row(png_structp png_ptr, uint8_t* row);

/* write a few rows of image data */
extern void png_write_rows(png_structp png_ptr, png_bytepp row, uint32_t num_rows);

/* write the image data */
extern void png_write_image(png_structp png_ptr, png_bytepp image);

/* writes the end of the PNG file. */
extern void png_write_end(png_structp png_ptr, png_infop info_ptr);

#ifndef PNG_NO_SEQUENTIAL_READ_SUPPORTED
/* read the end of the PNG file. */
extern void png_read_end(png_structp png_ptr, png_infop info_ptr);
#endif

/* free any memory associated with the png_info_struct */
extern void png_destroy_info_struct(png_structp png_ptr, png_infopp info_ptr_ptr);

/* free any memory associated with the png_struct and the png_info_structs */
extern void png_destroy_read_struct(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);

/* free all memory used by the read (old method - NOT DLL EXPORTED) */
extern void png_read_destroy(png_structp png_ptr, png_infop info_ptr, png_infop end_info_ptr);

/* free any memory associated with the png_struct and the png_info_structs */
extern void png_destroy_write_struct(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr);

/* free any memory used in png_ptr struct (old method - NOT DLL EXPORTED) */
extern void png_write_destroy(png_structp png_ptr);

/* set the libpng method of handling chunk CRC errors */
extern void png_set_crc_action(png_structp png_ptr, int crit_action, int ancil_action);

/* Values for png_set_crc_action() to say how to handle CRC errors in
 * ancillary and critical chunks, and whether to use the data contained
 * therein.  Note that it is impossible to "discard" data in a critical
 * chunk.  For versions prior to 0.90, the action was always error/quit,
 * whereas in version 0.90 and later, the action for CRC errors in ancillary
 * chunks is warn/discard.  These values should NOT be changed.
 *
 *      value                       action:critical     action:ancillary
 */
#define PNG_CRC_DEFAULT       0  /* error/quit          warn/discard data */
#define PNG_CRC_ERROR_QUIT    1  /* error/quit          error/quit        */
#define PNG_CRC_WARN_DISCARD  2  /* (INVALID)           warn/discard data */
#define PNG_CRC_WARN_USE      3  /* warn/use data       warn/use data     */
#define PNG_CRC_QUIET_USE     4  /* quiet/use data      quiet/use data    */
#define PNG_CRC_NO_CHANGE     5  /* use current value   use current value */

/* These functions give the user control over the scan-line filtering in
 * libpng and the compression methods used by zlib.  These functions are
 * mainly useful for testing, as the defaults should work with most users.
 * Those users who are tight on memory or want faster performance at the
 * expense of compression can modify them.  See the compression library
 * header file (zlib.h) for an explination of the compression functions.
 */

/* set the filtering method(s) used by libpng.  Currently, the only valid
 * value for "method" is 0.
 */
extern void png_set_filter(png_structp png_ptr, int method, int filters);

/* Flags for png_set_filter() to say which filters to use.  The flags
 * are chosen so that they don't conflict with real filter types
 * below, in case they are supplied instead of the #defined constants.
 * These values should NOT be changed.
 */
#define PNG_NO_FILTERS     0x00
#define PNG_FILTER_NONE    0x08
#define PNG_FILTER_SUB     0x10
#define PNG_FILTER_UP      0x20
#define PNG_FILTER_AVG     0x40
#define PNG_FILTER_PAETH   0x80
#define PNG_ALL_FILTERS (PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_UP | \
                         PNG_FILTER_AVG | PNG_FILTER_PAETH)

/* Filter values (not flags) - used in pngwrite.c, pngwutil.c for now.
 * These defines should NOT be changed.
 */
#define PNG_FILTER_VALUE_NONE  0
#define PNG_FILTER_VALUE_SUB   1
#define PNG_FILTER_VALUE_UP    2
#define PNG_FILTER_VALUE_AVG   3
#define PNG_FILTER_VALUE_PAETH 4
#define PNG_FILTER_VALUE_LAST  5

#if defined(PNG_WRITE_WEIGHTED_FILTER_SUPPORTED) /* EXPERIMENTAL */
/* The "heuristic_method" is given by one of the PNG_FILTER_HEURISTIC_
 * defines, either the default (minimum-sum-of-absolute-differences), or
 * the experimental method (weighted-minimum-sum-of-absolute-differences).
 *
 * Weights are factors >= 1.0, indicating how important it is to keep the
 * filter type consistent between rows.  Larger numbers mean the current
 * filter is that many times as likely to be the same as the "num_weights"
 * previous filters.  This is cumulative for each previous row with a weight.
 * There needs to be "num_weights" values in "filter_weights", or it can be
 * NULL if the weights aren't being specified.  Weights have no influence on
 * the selection of the first row filter.  Well chosen weights can (in theory)
 * improve the compression for a given image.
 *
 * Costs are factors >= 1.0 indicating the relative decoding costs of a
 * filter type.  Higher costs indicate more decoding expense, and are
 * therefore less likely to be selected over a filter with lower computational
 * costs.  There needs to be a value in "filter_costs" for each valid filter
 * type (given by PNG_FILTER_VALUE_LAST), or it can be NULL if you aren't
 * setting the costs.  Costs try to improve the speed of decompression without
 * unduly increasing the compressed image size.
 *
 * A negative weight or cost indicates the default value is to be used, and
 * values in the range [0.0, 1.0) indicate the value is to remain unchanged.
 * The default values for both weights and costs are currently 1.0, but may
 * change if good general weighting/cost heuristics can be found.  If both
 * the weights and costs are set to 1.0, this degenerates the WEIGHTED method
 * to the UNWEIGHTED method, but with added encoding time/computation.
 */
#ifdef PNG_FLOATING_POINT_SUPPORTED
extern void png_set_filter_heuristics(png_structp png_ptr, int heuristic_method, int num_weights, png_doublep filter_weights, png_doublep filter_costs);
#endif
#endif /*  PNG_WRITE_WEIGHTED_FILTER_SUPPORTED */

/* Heuristic used for row filter selection.  These defines should NOT be
 * changed.
 */
#define PNG_FILTER_HEURISTIC_DEFAULT    0  /* Currently "UNWEIGHTED" */
#define PNG_FILTER_HEURISTIC_UNWEIGHTED 1  /* Used by libpng < 0.95 */
#define PNG_FILTER_HEURISTIC_WEIGHTED   2  /* Experimental feature */
#define PNG_FILTER_HEURISTIC_LAST       3  /* Not a valid value */

/* Set the library compression level.  Currently, valid values range from
 * 0 - 9, corresponding directly to the zlib compression levels 0 - 9
 * (0 - no compression, 9 - "maximal" compression).  Note that tests have
 * shown that zlib compression levels 3-6 usually perform as well as level 9
 * for PNG images, and do considerably fewer caclulations.  In the future,
 * these values may not correspond directly to the zlib compression levels.
 */
extern void png_set_compression_level(png_structp png_ptr, int level);

extern void png_set_compression_mem_level(png_structp png_ptr, int mem_level);

extern void png_set_compression_strategy(png_structp png_ptr, int strategy);

extern void png_set_compression_window_bits(png_structp png_ptr, int window_bits);

extern void png_set_compression_method(png_structp png_ptr, int method);

/* These next functions are called for input/output, memory, and error
 * handling.  They are in the file pngrio.c, pngwio.c, and pngerror.c,
 * and call standard C I/O routines such as fread(), fwrite(), and
 * fprintf().  These functions can be made to use other I/O routines
 * at run time for those applications that need to handle I/O in a
 * different manner by calling png_set_???_fn().  See libpng.txt for
 * more information.
 */

#if !defined(PNG_NO_STDIO)
/* Initialize the input/output for the PNG file to the default functions. */
extern void png_init_io(png_structp png_ptr, FILE* fp);
#endif

/* Replace the (error and abort), and warning functions with user
 * supplied functions.  If no messages are to be printed you must still
 * write and use replacement functions. The replacement error_fn should
 * still do a longjmp to the last setjmp location if you are using this
 * method of error handling.  If error_fn or warning_fn is NULL, the
 * default function will be used.
 */

extern void png_set_error_fn(png_structp png_ptr, void* error_ptr, png_error_ptr error_fn, png_error_ptr warning_fn);

/* Return the user pointer associated with the error functions */
extern void* png_get_error_ptr(png_structp png_ptr);

/* Replace the default data output functions with a user supplied one(s).
 * If buffered output is not used, then output_flush_fn can be set to NULL.
 * If PNG_WRITE_FLUSH_SUPPORTED is not defined at libpng compile time
 * output_flush_fn will be ignored (and thus can be NULL).
 */
extern void png_set_write_fn(png_structp png_ptr, void* io_ptr, png_rw_ptr write_data_fn, png_flush_ptr output_flush_fn);

/* Replace the default data input function with a user supplied one. */
extern void png_set_read_fn(png_structp png_ptr, void* io_ptr, png_rw_ptr read_data_fn);

/* Return the user pointer associated with the I/O functions */
extern void* png_get_io_ptr(png_structp png_ptr);

extern void png_set_read_status_fn(png_structp png_ptr, png_read_status_ptr read_row_fn);

extern void png_set_write_status_fn(png_structp png_ptr, png_write_status_ptr write_row_fn);

#ifdef PNG_USER_MEM_SUPPORTED
/* Replace the default memory allocation functions with user supplied one(s). */
extern void png_set_mem_fn(png_structp png_ptr, void* mem_ptr, png_malloc_ptr malloc_fn, png_free_ptr free_fn);
/* Return the user pointer associated with the memory functions */
extern void* png_get_mem_ptr(png_structp png_ptr);
#endif

#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED) || \
    defined(PNG_LEGACY_SUPPORTED)
extern void png_set_read_user_transform_fn(png_structp png_ptr, png_user_transform_ptr read_user_transform_fn);
#endif

#if defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED) || \
    defined(PNG_LEGACY_SUPPORTED)
extern void png_set_write_user_transform_fn(png_structp png_ptr, png_user_transform_ptr write_user_transform_fn);
#endif

#if defined(PNG_READ_USER_TRANSFORM_SUPPORTED) || \
    defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED) || \
    defined(PNG_LEGACY_SUPPORTED)
extern void png_set_user_transform_info(png_structp png_ptr, void* user_transform_ptr, int user_transform_depth, int user_transform_channels);
/* Return the user pointer associated with the user transform functions */
extern void* png_get_user_transform_ptr(png_structp png_ptr);
#endif

#ifdef PNG_USER_CHUNKS_SUPPORTED
extern void png_set_read_user_chunk_fn(png_structp png_ptr, void* user_chunk_ptr, png_user_chunk_ptr read_user_chunk_fn);
extern void* png_get_user_chunk_ptr(png_structp png_ptr);
#endif

#ifdef PNG_PROGRESSIVE_READ_SUPPORTED
/* Sets the function callbacks for the push reader, and a pointer to a
 * user-defined structure available to the callback functions.
 */
extern void png_set_progressive_read_fn(png_structp png_ptr, void* progressive_ptr, png_progressive_info_ptr info_fn, png_progressive_row_ptr row_fn, png_progressive_end_ptr end_fn);

/* returns the user pointer associated with the push read functions */
extern void* png_get_progressive_ptr(png_structp png_ptr);

/* function to be called when data becomes available */
extern void png_process_data(png_structp png_ptr, png_infop info_ptr, uint8_t* buffer, size_t buffer_size);

/* function that combines rows.  Not very much different than the
 * png_combine_row() call.  Is this even used?????
 */
extern void png_progressive_combine_row(png_structp png_ptr, uint8_t* old_row, uint8_t* new_row);
#endif /* PNG_PROGRESSIVE_READ_SUPPORTED */

extern void* png_malloc(png_structp png_ptr, uint32_t size);

#if defined(PNG_1_0_X)
#  define png_malloc_warn png_malloc
#else
/* Added at libpng version 1.2.4 */
extern void* png_malloc_warn(png_structp png_ptr, uint32_t size);
#endif

/* frees a pointer allocated by png_malloc() */
extern void png_free(png_structp png_ptr, void* ptr);

#if defined(PNG_1_0_X)
/* Function to allocate memory for zlib. */
extern voidpf png_zalloc(voidpf png_ptr, uint32_t items, uint32_t size);

/* Function to free memory for zlib */
extern void png_zfree(voidpf png_ptr, voidpf ptr);
#endif

/* Free data that was allocated internally */
extern void png_free_data(png_structp png_ptr, png_infop info_ptr, uint32_t free_me, int num);
#ifdef PNG_FREE_ME_SUPPORTED
/* Reassign responsibility for freeing existing data, whether allocated
 * by libpng or by the application */
extern void png_data_freer(png_structp png_ptr, png_infop info_ptr, int freer, uint32_t mask);
#endif
/* assignments for png_data_freer */
#define PNG_DESTROY_WILL_FREE_DATA 1
#define PNG_SET_WILL_FREE_DATA 1
#define PNG_USER_WILL_FREE_DATA 2
/* Flags for png_ptr->free_me and info_ptr->free_me */
#define PNG_FREE_HIST 0x0008
#define PNG_FREE_ICCP 0x0010
#define PNG_FREE_SPLT 0x0020
#define PNG_FREE_ROWS 0x0040
#define PNG_FREE_PCAL 0x0080
#define PNG_FREE_SCAL 0x0100
#define PNG_FREE_UNKN 0x0200
#define PNG_FREE_LIST 0x0400
#define PNG_FREE_PLTE 0x1000
#define PNG_FREE_TRNS 0x2000
#define PNG_FREE_TEXT 0x4000
#define PNG_FREE_ALL  0x7fff
#define PNG_FREE_MUL  0x4220 /* PNG_FREE_SPLT|PNG_FREE_TEXT|PNG_FREE_UNKN */

#ifdef PNG_USER_MEM_SUPPORTED
extern void* png_malloc_default(png_structp png_ptr, uint32_t size);
extern void png_free_default(png_structp png_ptr, void* ptr);
#endif

extern void* png_memcpy_check(png_structp png_ptr, void* s1, void* s2, uint32_t size);

extern void* png_memset_check(png_structp png_ptr, void* s1, int value, uint32_t size);

/* Fatal error in PNG image of libpng - can't continue */
extern void png_error(png_structp png_ptr, const char* error_message);

/* The same, but the chunk name is prepended to the error string. */
extern void png_chunk_error(png_structp png_ptr, const char* error_message);

/* Non-fatal error in libpng.  Can continue, but may have a problem. */
extern void png_warning(png_structp png_ptr, const char* warning_message);

/* Non-fatal error in libpng, chunk name is prepended to message. */
extern void png_chunk_warning(png_structp png_ptr, const char* warning_message);

/* The png_set_<chunk> functions are for storing values in the png_info_struct.
 * Similarly, the png_get_<chunk> calls are used to read values from the
 * png_info_struct, either storing the parameters in the passed variables, or
 * setting pointers into the png_info_struct where the data is stored.  The
 * png_get_<chunk> functions return a non-zero value if the data was available
 * in info_ptr, or return zero and do not change any of the parameters if the
 * data was not available.
 *
 * These functions should be used instead of directly accessing png_info
 * to avoid problems with future changes in the size and internal layout of
 * png_info_struct.
 */
/* Returns "flag" if chunk data is valid in info_ptr. */
extern uint32_t png_get_valid(png_structp png_ptr, png_infop info_ptr, uint32_t flag);

/* Returns number of bytes needed to hold a transformed row. */
extern uint32_t png_get_rowbytes(png_structp png_ptr, png_infop info_ptr);

#if defined(PNG_INFO_IMAGE_SUPPORTED)
/* Returns row_pointers, which is an array of pointers to scanlines that was
returned from png_read_png(). */
extern png_bytepp png_get_rows(png_structp png_ptr, png_infop info_ptr);
/* Set row_pointers, which is an array of pointers to scanlines for use
by png_write_png(). */
extern void png_set_rows(png_structp png_ptr, png_infop info_ptr, png_bytepp row_pointers);
#endif

/* Returns number of color channels in image. */
extern uint8_t png_get_channels(png_structp png_ptr, png_infop info_ptr);

#ifdef PNG_EASY_ACCESS_SUPPORTED
/* Returns image width in pixels. */
extern uint32_t png_get_image_width(png_structp png_ptr, png_infop info_ptr);

/* Returns image height in pixels. */
extern uint32_t png_get_image_height(png_structp png_ptr, png_infop info_ptr);

/* Returns image bit_depth. */
extern uint8_t png_get_bit_depth(png_structp png_ptr, png_infop info_ptr);

/* Returns image color_type. */
extern uint8_t png_get_color_type(png_structp png_ptr, png_infop info_ptr);

/* Returns image filter_type. */
extern uint8_t png_get_filter_type(png_structp png_ptr, png_infop info_ptr);

/* Returns image interlace_type. */
extern uint8_t png_get_interlace_type(png_structp png_ptr, png_infop info_ptr);

/* Returns image compression_type. */
extern uint8_t png_get_compression_type(png_structp png_ptr, png_infop info_ptr);

/* Returns image resolution in pixels per meter, from pHYs chunk data. */
extern uint32_t png_get_pixels_per_meter(png_structp png_ptr, png_infop info_ptr);
extern uint32_t png_get_x_pixels_per_meter(png_structp png_ptr, png_infop info_ptr);
extern uint32_t png_get_y_pixels_per_meter(png_structp png_ptr, png_infop info_ptr);

/* Returns pixel aspect ratio, computed from pHYs chunk data.  */
#ifdef PNG_FLOATING_POINT_SUPPORTED
extern float png_get_pixel_aspect_ratio(png_structp png_ptr, png_infop info_ptr);
#endif

#endif /* PNG_EASY_ACCESS_SUPPORTED */

/* Returns pointer to signature string read from PNG header */
extern uint8_t* png_get_signature(png_structp png_ptr, png_infop info_ptr);

#if defined(PNG_hIST_SUPPORTED)
extern uint32_t png_get_hIST(png_structp png_ptr, png_infop info_ptr, uint16_t** hist);
#endif

#if defined(PNG_hIST_SUPPORTED)
extern void png_set_hIST(png_structp png_ptr, png_infop info_ptr, uint16_t* hist);
#endif

extern uint32_t png_get_IHDR(png_structp png_ptr, png_infop info_ptr, uint32_t* width, uint32_t* height, int* bit_depth, int* color_type, int* interlace_method, int* compression_method, int* filter_method);

extern void png_set_IHDR(png_structp png_ptr, png_infop info_ptr, uint32_t width, uint32_t height, int bit_depth, int color_type, int interlace_method, int compression_method, int filter_method);

#if defined(PNG_pCAL_SUPPORTED)
extern uint32_t png_get_pCAL(png_structp png_ptr, png_infop info_ptr, char** purpose, int32_t* X0, int32_t* X1, int* type, int* nparams, char** units, png_charpp* params);
#endif

#if defined(PNG_pCAL_SUPPORTED)
extern void png_set_pCAL(png_structp png_ptr, png_infop info_ptr, char* purpose, int32_t X0, int32_t X1, int type, int nparams, char* units, png_charpp params);
#endif

#if defined(PNG_pHYs_SUPPORTED)
extern uint32_t png_get_pHYs(png_structp png_ptr, png_infop info_ptr, uint32_t* res_x, uint32_t* res_y, int* unit_type);
#endif

#if defined(PNG_pHYs_SUPPORTED)
extern void png_set_pHYs(png_structp png_ptr, png_infop info_ptr, uint32_t res_x, uint32_t res_y, int unit_type);
#endif

extern uint32_t png_get_PLTE(png_structp png_ptr, png_infop info_ptr, png_colorp* palette, int* num_palette);

extern void png_set_PLTE(png_structp png_ptr, png_infop info_ptr, png_colorp palette, int num_palette);

#if defined(PNG_sBIT_SUPPORTED)
extern uint32_t png_get_sBIT(png_structp png_ptr, png_infop info_ptr, png_color_8p* sig_bit);
#endif

#if defined(PNG_sBIT_SUPPORTED)
extern void png_set_sBIT(png_structp png_ptr, png_infop info_ptr, png_color_8p sig_bit);
#endif

#if defined(PNG_sRGB_SUPPORTED)
extern uint32_t png_get_sRGB(png_structp png_ptr, png_infop info_ptr, int* intent);
#endif

#if defined(PNG_sRGB_SUPPORTED)
extern void png_set_sRGB(png_structp png_ptr, png_infop info_ptr, int intent);
extern void png_set_sRGB_gAMA_and_cHRM(png_structp png_ptr, png_infop info_ptr, int intent);
#endif

#if defined(PNG_sPLT_SUPPORTED)
extern uint32_t png_get_sPLT(png_structp png_ptr, png_infop info_ptr, png_sPLT_tpp entries);
#endif

#if defined(PNG_sPLT_SUPPORTED)
extern void png_set_sPLT(png_structp png_ptr, png_infop info_ptr, png_sPLT_tp entries, int nentries);
#endif

#if defined(PNG_tRNS_SUPPORTED)
extern uint32_t png_get_tRNS(png_structp png_ptr, png_infop info_ptr, uint8_t** trans, int* num_trans, png_color_16p* trans_values);
#endif

#if defined(PNG_tRNS_SUPPORTED)
extern void png_set_tRNS(png_structp png_ptr, png_infop info_ptr, uint8_t* trans, int num_trans, png_color_16p trans_values);
#endif

#if defined(PNG_tRNS_SUPPORTED)
#endif

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
/* provide a list of chunks and how they are to be handled, if the built-in
   handling or default unknown chunk handling is not desired.  Any chunks not
   listed will be handled in the default manner.  The IHDR and IEND chunks
   must not be listed.
      keep = 0: follow default behavour
           = 1: do not keep
           = 2: keep only if safe-to-copy
           = 3: keep even if unsafe-to-copy
*/
extern void png_set_keep_unknown_chunks(png_structp png_ptr, int keep, uint8_t* chunk_list, int num_chunks);
extern void png_set_unknown_chunks(png_structp png_ptr, png_infop info_ptr, png_unknown_chunkp unknowns, int num_unknowns);
extern void png_set_unknown_chunk_location(png_structp png_ptr, png_infop info_ptr, int chunk, int location);
extern uint32_t png_get_unknown_chunks(png_structp png_ptr, png_infop info_ptr, png_unknown_chunkpp entries);
#endif
#ifdef PNG_HANDLE_AS_UNKNOWN_SUPPORTED
int png_handle_as_unknown(png_structp png_ptr, uint8_t* chunk_name);
#endif

/* Png_free_data() will turn off the "valid" flag for anything it frees.
   If you need to turn it off for a chunk that your application has freed,
   you can use png_set_invalid(png_ptr, info_ptr, PNG_INFO_CHNK); */
extern void png_set_invalid(png_structp png_ptr, png_infop info_ptr, int mask);

#if defined(PNG_INFO_IMAGE_SUPPORTED)
/* The "params" pointer is currently not used and is for future expansion. */
extern void png_read_png(png_structp png_ptr, png_infop info_ptr, int transforms, void* params);
extern void png_write_png(png_structp png_ptr, png_infop info_ptr, int transforms, void* params);
#endif

/* Define PNG_DEBUG at compile time for debugging information.  Higher
 * numbers for PNG_DEBUG mean more debugging information.  This has
 * only been added since version 0.95 so it is not implemented throughout
 * libpng yet, but more support will be added as needed.
 */
#ifdef PNG_DEBUG
#if (PNG_DEBUG > 0)
#if !defined(PNG_DEBUG_FILE) && defined(_MSC_VER)
#include <crtdbg.h>
#if (PNG_DEBUG > 1)
#define png_debug(l,m)  _RPT0(_CRT_WARN,m)
#define png_debug1(l,m,p1)  _RPT1(_CRT_WARN,m,p1)
#define png_debug2(l,m,p1,p2) _RPT2(_CRT_WARN,m,p1,p2)
#endif
#else /* PNG_DEBUG_FILE || !_MSC_VER */
#ifndef PNG_DEBUG_FILE
#define PNG_DEBUG_FILE stderr
#endif /* PNG_DEBUG_FILE */
#if (PNG_DEBUG > 1)
#define png_debug(l,m) \
{ \
     int num_tabs=l; \
     fprintf(PNG_DEBUG_FILE,"%s"m,(num_tabs==1 ? "\t" : \
       (num_tabs==2 ? "\t\t":(num_tabs>2 ? "\t\t\t":"")))); \
}
#define png_debug1(l,m,p1) \
{ \
     int num_tabs=l; \
     fprintf(PNG_DEBUG_FILE,"%s"m,(num_tabs==1 ? "\t" : \
       (num_tabs==2 ? "\t\t":(num_tabs>2 ? "\t\t\t":""))),p1); \
}
#define png_debug2(l,m,p1,p2) \
{ \
     int num_tabs=l; \
     fprintf(PNG_DEBUG_FILE,"%s"m,(num_tabs==1 ? "\t" : \
       (num_tabs==2 ? "\t\t":(num_tabs>2 ? "\t\t\t":""))),p1,p2); \
}
#endif /* (PNG_DEBUG > 1) */
#endif /* _MSC_VER */
#endif /* (PNG_DEBUG > 0) */
#endif /* PNG_DEBUG */
#ifndef png_debug
#define png_debug(l, m)
#endif
#ifndef png_debug1
#define png_debug1(l, m, p1)
#endif
#ifndef png_debug2
#define png_debug2(l, m, p1, p2)
#endif

extern char* png_get_copyright(png_structp png_ptr);
extern char* png_get_header_ver(png_structp png_ptr);
extern char* png_get_header_version(png_structp png_ptr);
extern char* png_get_libpng_ver(png_structp png_ptr);

/* For use in png_set_keep_unknown, added to version 1.2.6 */
#define PNG_HANDLE_CHUNK_AS_DEFAULT   0
#define PNG_HANDLE_CHUNK_NEVER        1
#define PNG_HANDLE_CHUNK_IF_SAFE      2
#define PNG_HANDLE_CHUNK_ALWAYS       3

/* Added at libpng-1.2.6 */
#ifdef PNG_SET_USER_LIMITS_SUPPORTED
extern void png_set_user_limits(png_structp   png_ptr, uint32_t user_width_max, uint32_t user_height_max);
extern uint32_t png_get_user_width_max(png_structp png_ptr);
extern uint32_t png_get_user_height_max(png_structp png_ptr);
#endif

#ifdef PNG_READ_COMPOSITE_NODIV_SUPPORTED
/* With these routines we avoid an integer divide, which will be slower on
 * most machines.  However, it does take more operations than the corresponding
 * divide method, so it may be slower on a few RISC systems.  There are two
 * shifts (by 8 or 16 bits) and an addition, versus a single integer divide.
 *
 * Note that the rounding factors are NOT supposed to be the same!  128 and
 * 32768 are correct for the NODIV code; 127 and 32767 are correct for the
 * standard method.
 *
 * [Optimized code by Greg Roelofs and Mark Adler...blame us for bugs. :-) ]
 */

 /* fg and bg should be in `gamma 1.0' space; alpha is the opacity          */

#  define png_composite(composite, fg, alpha, bg)                            \
     { uint16_t temp = (uint16_t)((uint16_t)(fg) * (uint16_t)(alpha) \
                        +        (uint16_t)(bg)*(uint16_t)(255 -       \
                        (uint16_t)(alpha)) + (uint16_t)128);           \
       (composite) = (uint8_t)((temp + (temp >> 8)) >> 8); }

#  define png_composite_16(composite, fg, alpha, bg)                         \
     { uint32_t temp = (uint32_t)((uint32_t)(fg) * (uint32_t)(alpha) \
                        + (uint32_t)(bg)*(uint32_t)(65535L -           \
                        (uint32_t)(alpha)) + (uint32_t)32768L);        \
       (composite) = (uint16_t)((temp + (temp >> 16)) >> 16); }

#else  /* standard method using integer division */

#  define png_composite(composite, fg, alpha, bg)                            \
     (composite) = (uint8_t)(((uint16_t)(fg) * (uint16_t)(alpha) +    \
       (uint16_t)(bg) * (uint16_t)(255 - (uint16_t)(alpha)) +       \
       (uint16_t)127) / 255)

#  define png_composite_16(composite, fg, alpha, bg)                         \
     (composite) = (uint16_t)(((uint32_t)(fg) * (uint32_t)(alpha) + \
       (uint32_t)(bg)*(uint32_t)(65535L - (uint32_t)(alpha)) +      \
       (uint32_t)32767) / (uint32_t)65535L)

#endif /* PNG_READ_COMPOSITE_NODIV_SUPPORTED */

/* Inline macros to do direct reads of bytes from the input buffer.  These
 * require that you are using an architecture that uses PNG byte ordering
 * (MSB first) and supports unaligned data storage.  I think that PowerPC
 * in big-endian mode and 680x0 are the only ones that will support this.
 * The x86 line of processors definitely do not.  The png_get_int_32()
 * routine also assumes we are using two's complement format for negative
 * values, which is almost certainly true.
 */
#if defined(PNG_READ_BIG_ENDIAN_SUPPORTED)
#  define png_get_uint_32(buf) ( *((uint32_t*) (buf)))
#  define png_get_uint_16(buf) ( *((uint16_t*) (buf)))
#  define png_get_int_32(buf)  ( *((int32_t*)  (buf)))
#else
extern uint32_t png_get_uint_32(uint8_t* buf);
extern uint16_t png_get_uint_16(uint8_t* buf);
extern int32_t png_get_int_32(uint8_t* buf);
#endif /* !PNG_READ_BIG_ENDIAN_SUPPORTED */
extern uint32_t png_get_uint_31(png_structp png_ptr, uint8_t* buf);
/* No png_get_int_16 -- may be added if there's a real need for it. */

/* Place a 32-bit number into a buffer in PNG byte order (big-endian).
 */
extern void png_save_uint_32(uint8_t* buf, uint32_t i);
extern void png_save_int_32(uint8_t* buf, int32_t i);

/* Place a 16-bit number into a buffer in PNG byte order.
 * The parameter is declared unsigned int, not uint16_t,
 * just to avoid potential problems on pre-ANSI C compilers.
 */
extern void png_save_uint_16(uint8_t* buf, unsigned int i);
/* No png_save_int_16 -- may be added if there's a real need for it. */

/* ************************************************************************* */

/* These next functions are used internally in the code.  They generally
 * shouldn't be used unless you are writing code to add or replace some
 * functionality in libpng.  More information about most functions can
 * be found in the files where the functions are located.
 */

#if defined(PNG_INTERNAL)

/* Various modes of operation.  Note that after an init, mode is set to
 * zero automatically when the structure is created.
 */
#define PNG_HAVE_IHDR               0x01
#define PNG_HAVE_PLTE               0x02
#define PNG_HAVE_IDAT               0x04
#define PNG_AFTER_IDAT              0x08 /* Have complete zlib datastream */
#define PNG_HAVE_IEND               0x10
#define PNG_HAVE_gAMA               0x20
#define PNG_HAVE_cHRM               0x40
#define PNG_HAVE_sRGB               0x80
#define PNG_HAVE_CHUNK_HEADER      0x100
#define PNG_WROTE_tIME             0x200
#define PNG_WROTE_INFO_BEFORE_PLTE 0x400
#define PNG_BACKGROUND_IS_GRAY     0x800
#define PNG_HAVE_PNG_SIGNATURE    0x1000
#define PNG_HAVE_CHUNK_AFTER_IDAT 0x2000 /* Have another chunk after IDAT */

/* flags for the transformations the PNG library does on the image data */
#define PNG_BGR                0x0001
#define PNG_INTERLACE          0x0002
#define PNG_PACK               0x0004
#define PNG_SHIFT              0x0008
#define PNG_SWAP_BYTES         0x0010
#define PNG_INVERT_MONO        0x0020
#define PNG_DITHER             0x0040
#define PNG_BACKGROUND         0x0080
#define PNG_BACKGROUND_EXPAND  0x0100
                          /*   0x0200 unused */
#define PNG_16_TO_8            0x0400
#define PNG_RGBA               0x0800
#define PNG_EXPAND             0x1000
#define PNG_GAMMA              0x2000
#define PNG_GRAY_TO_RGB        0x4000
#define PNG_FILLER             0x8000L
#define PNG_PACKSWAP          0x10000L
#define PNG_SWAP_ALPHA        0x20000L
#define PNG_STRIP_ALPHA       0x40000L
#define PNG_INVERT_ALPHA      0x80000L
#define PNG_USER_TRANSFORM   0x100000L
#define PNG_RGB_TO_GRAY_ERR  0x200000L
#define PNG_RGB_TO_GRAY_WARN 0x400000L
#define PNG_RGB_TO_GRAY      0x600000L  /* two bits, RGB_TO_GRAY_ERR|WARN */
                       /*    0x800000L     Unused */
#define PNG_ADD_ALPHA       0x1000000L  /* Added to libpng-1.2.7 */
#define PNG_EXPAND_tRNS     0x2000000L  /* Added to libpng-1.2.9 */
                       /*   0x4000000L  unused */
                       /*   0x8000000L  unused */
                       /*  0x10000000L  unused */
                       /*  0x20000000L  unused */
                       /*  0x40000000L  unused */

/* flags for png_create_struct */
#define PNG_STRUCT_PNG   0x0001
#define PNG_STRUCT_INFO  0x0002

/* Scaling factor for filter heuristic weighting calculations */
#define PNG_WEIGHT_SHIFT 8
#define PNG_WEIGHT_FACTOR (1<<(PNG_WEIGHT_SHIFT))
#define PNG_COST_SHIFT 3
#define PNG_COST_FACTOR (1<<(PNG_COST_SHIFT))

/* flags for the png_ptr->flags rather than declaring a byte for each one */
#define PNG_FLAG_ZLIB_CUSTOM_STRATEGY     0x0001
#define PNG_FLAG_ZLIB_CUSTOM_LEVEL        0x0002
#define PNG_FLAG_ZLIB_CUSTOM_MEM_LEVEL    0x0004
#define PNG_FLAG_ZLIB_CUSTOM_WINDOW_BITS  0x0008
#define PNG_FLAG_ZLIB_CUSTOM_METHOD       0x0010
#define PNG_FLAG_ZLIB_FINISHED            0x0020
#define PNG_FLAG_ROW_INIT                 0x0040
#define PNG_FLAG_FILLER_AFTER             0x0080
#define PNG_FLAG_CRC_ANCILLARY_USE        0x0100
#define PNG_FLAG_CRC_ANCILLARY_NOWARN     0x0200
#define PNG_FLAG_CRC_CRITICAL_USE         0x0400
#define PNG_FLAG_CRC_CRITICAL_IGNORE      0x0800
#define PNG_FLAG_FREE_PLTE                0x1000
#define PNG_FLAG_FREE_TRNS                0x2000
#define PNG_FLAG_FREE_HIST                0x4000
#define PNG_FLAG_KEEP_UNKNOWN_CHUNKS      0x8000L
#define PNG_FLAG_KEEP_UNSAFE_CHUNKS       0x10000L
#define PNG_FLAG_LIBRARY_MISMATCH         0x20000L
#define PNG_FLAG_STRIP_ERROR_NUMBERS      0x40000L
#define PNG_FLAG_STRIP_ERROR_TEXT         0x80000L
#define PNG_FLAG_MALLOC_NULL_MEM_OK       0x100000L
#define PNG_FLAG_ADD_ALPHA                0x200000L  /* Added to libpng-1.2.8 */
#define PNG_FLAG_STRIP_ALPHA              0x400000L  /* Added to libpng-1.2.8 */
                                  /*      0x800000L  unused */
                                  /*     0x1000000L  unused */
                                  /*     0x2000000L  unused */
                                  /*     0x4000000L  unused */
                                  /*     0x8000000L  unused */
                                  /*    0x10000000L  unused */
                                  /*    0x20000000L  unused */
                                  /*    0x40000000L  unused */

#define PNG_FLAG_CRC_ANCILLARY_MASK (PNG_FLAG_CRC_ANCILLARY_USE | \
                                     PNG_FLAG_CRC_ANCILLARY_NOWARN)

#define PNG_FLAG_CRC_CRITICAL_MASK  (PNG_FLAG_CRC_CRITICAL_USE | \
                                     PNG_FLAG_CRC_CRITICAL_IGNORE)

#define PNG_FLAG_CRC_MASK           (PNG_FLAG_CRC_ANCILLARY_MASK | \
                                     PNG_FLAG_CRC_CRITICAL_MASK)

/* save typing and make code easier to understand */

#define PNG_COLOR_DIST(c1, c2) (abs((int)((c1).red) - (int)((c2).red)) + \
   abs((int)((c1).green) - (int)((c2).green)) + \
   abs((int)((c1).blue) - (int)((c2).blue)))

/* Added to libpng-1.2.6 JB */
#define PNG_ROWBYTES(pixel_bits, width) \
    ((pixel_bits) >= 8 ? \
    ((width) * (((uint32_t)(pixel_bits)) >> 3)) : \
    (( ((width) * ((uint32_t)(pixel_bits))) + 7) >> 3) )

/* PNG_OUT_OF_RANGE returns true if value is outside the range
   ideal-delta..ideal+delta.  Each argument is evaluated twice.
   "ideal" and "delta" should be constants, normally simple
   integers, "value" a variable. Added to libpng-1.2.6 JB */
#define PNG_OUT_OF_RANGE(value, ideal, delta) \
        ( (value) < (ideal)-(delta) || (value) > (ideal)+(delta) )

/* variables declared in png.c - only it needs to define PNG_NO_EXTERN */
#if !defined(PNG_NO_EXTERN) || defined(PNG_ALWAYS_EXTERN)
/* place to hold the signature string for a PNG file. */
#ifdef PNG_USE_GLOBAL_ARRAYS
   PNG_EXPORT_VAR (const uint8_t) png_sig[8];
#else
#if 0
#define png_sig png_sig_bytes(NULL)
#endif
#endif
#endif /* PNG_NO_EXTERN */

/* Constant strings for known chunk types.  If you need to add a chunk,
 * define the name here, and add an invocation of the macro in png.c and
 * wherever it's needed.
 */
#define PNG_IHDR const uint8_t png_IHDR[5] = { 73,  72,  68,  82, '\0'}
#define PNG_IDAT const uint8_t png_IDAT[5] = { 73,  68,  65,  84, '\0'}
#define PNG_IEND const uint8_t png_IEND[5] = { 73,  69,  78,  68, '\0'}
#define PNG_PLTE const uint8_t png_PLTE[5] = { 80,  76,  84,  69, '\0'}
#define PNG_bKGD const uint8_t png_bKGD[5] = { 98,  75,  71,  68, '\0'}
#define PNG_cHRM const uint8_t png_cHRM[5] = { 99,  72,  82,  77, '\0'}
#define PNG_gAMA const uint8_t png_gAMA[5] = {103,  65,  77,  65, '\0'}
#define PNG_hIST const uint8_t png_hIST[5] = {104,  73,  83,  84, '\0'}
#define PNG_iCCP const uint8_t png_iCCP[5] = {105,  67,  67,  80, '\0'}
#define PNG_iTXt const uint8_t png_iTXt[5] = {105,  84,  88, 116, '\0'}
#define PNG_oFFs const uint8_t png_oFFs[5] = {111,  70,  70, 115, '\0'}
#define PNG_pCAL const uint8_t png_pCAL[5] = {112,  67,  65,  76, '\0'}
#define PNG_sCAL const uint8_t png_sCAL[5] = {115,  67,  65,  76, '\0'}
#define PNG_pHYs const uint8_t png_pHYs[5] = {112,  72,  89, 115, '\0'}
#define PNG_sBIT const uint8_t png_sBIT[5] = {115,  66,  73,  84, '\0'}
#define PNG_sPLT const uint8_t png_sPLT[5] = {115,  80,  76,  84, '\0'}
#define PNG_sRGB const uint8_t png_sRGB[5] = {115,  82,  71,  66, '\0'}
#define PNG_tEXt const uint8_t png_tEXt[5] = {116,  69,  88, 116, '\0'}
#define PNG_tIME const uint8_t png_tIME[5] = {116,  73,  77,  69, '\0'}
#define PNG_tRNS const uint8_t png_tRNS[5] = {116,  82,  78,  83, '\0'}
#define PNG_zTXt const uint8_t png_zTXt[5] = {122,  84,  88, 116, '\0'}

#ifdef PNG_USE_GLOBAL_ARRAYS
PNG_EXPORT_VAR (const uint8_t) png_IHDR[5];
PNG_EXPORT_VAR (const uint8_t) png_IDAT[5];
PNG_EXPORT_VAR (const uint8_t) png_IEND[5];
PNG_EXPORT_VAR (const uint8_t) png_PLTE[5];
PNG_EXPORT_VAR (const uint8_t) png_bKGD[5];
PNG_EXPORT_VAR (const uint8_t) png_cHRM[5];
PNG_EXPORT_VAR (const uint8_t) png_gAMA[5];
PNG_EXPORT_VAR (const uint8_t) png_hIST[5];
PNG_EXPORT_VAR (const uint8_t) png_iCCP[5];
PNG_EXPORT_VAR (const uint8_t) png_iTXt[5];
PNG_EXPORT_VAR (const uint8_t) png_oFFs[5];
PNG_EXPORT_VAR (const uint8_t) png_pCAL[5];
PNG_EXPORT_VAR (const uint8_t) png_sCAL[5];
PNG_EXPORT_VAR (const uint8_t) png_pHYs[5];
PNG_EXPORT_VAR (const uint8_t) png_sBIT[5];
PNG_EXPORT_VAR (const uint8_t) png_sPLT[5];
PNG_EXPORT_VAR (const uint8_t) png_sRGB[5];
PNG_EXPORT_VAR (const uint8_t) png_tEXt[5];
PNG_EXPORT_VAR (const uint8_t) png_tIME[5];
PNG_EXPORT_VAR (const uint8_t) png_tRNS[5];
PNG_EXPORT_VAR (const uint8_t) png_zTXt[5];
#endif /* PNG_USE_GLOBAL_ARRAYS */

#if defined(PNG_1_0_X) || defined (PNG_1_2_X)
/* Initialize png_ptr struct for reading, and allocate any other memory.
 * (old interface - DEPRECATED - use png_create_read_struct instead).
 */
extern void png_read_init(png_structp png_ptr);
#undef png_read_init
#define png_read_init(png_ptr) png_read_init_3(&png_ptr, PNG_LIBPNG_VER_STRING,  sizeof(png_struct));
#endif

extern void png_read_init_3(png_structpp ptr_ptr, const char* user_png_ver, size_t png_struct_size);
#if defined(PNG_1_0_X) || defined (PNG_1_2_X)
extern void png_read_init_2(png_structp png_ptr, const char* user_png_ver, size_t png_struct_size, size_t png_info_size);
#endif

#if defined(PNG_1_0_X) || defined (PNG_1_2_X)
/* Initialize png_ptr struct for writing, and allocate any other memory.
 * (old interface - DEPRECATED - use png_create_write_struct instead).
 */
extern void png_write_init(png_structp png_ptr);
#undef png_write_init
#define png_write_init(png_ptr) png_write_init_3(&png_ptr, PNG_LIBPNG_VER_STRING, sizeof(png_struct));
#endif

extern void png_write_init_3(png_structpp ptr_ptr, const char* user_png_ver, size_t png_struct_size);
extern void png_write_init_2(png_structp png_ptr, const char* user_png_ver, size_t png_struct_size, size_t png_info_size);

/* Allocate memory for an internal libpng struct */
void* png_create_struct (int type);

/* Free memory from internal libpng struct */
void png_destroy_struct(void* struct_ptr);

void* png_create_struct_2(int type, png_malloc_ptr malloc_fn, void* mem_ptr);
void png_destroy_struct_2(void* struct_ptr, png_free_ptr free_fn, void* mem_ptr);

/* Free any memory that info_ptr points to and reset struct. */
void png_info_destroy(png_structp png_ptr, png_infop info_ptr);

#ifndef PNG_1_0_X
/* Function to allocate memory for zlib. */
voidpf png_zalloc(voidpf png_ptr, uint32_t items, uint32_t size);

/* Function to free memory for zlib */
void png_zfree(voidpf png_ptr, voidpf ptr);

#ifdef PNG_SIZE_T
/* Function to convert a sizeof an item to sizeof item */
size_t png_convert_size(size_t size);
#endif

void png_default_read_data(png_structp png_ptr, uint8_t* data, size_t length);

#ifdef PNG_PROGRESSIVE_READ_SUPPORTED
void png_push_fill_buffer(png_structp png_ptr, uint8_t* buffer, size_t length);
#endif

void png_default_write_data(png_structp png_ptr, uint8_t* data, size_t length);

#if defined(PNG_WRITE_FLUSH_SUPPORTED)
#if !defined(PNG_NO_STDIO)
void png_default_flush(png_structp png_ptr);
#endif
#endif
#else /* PNG_1_0_X */
#ifdef PNG_PROGRESSIVE_READ_SUPPORTED
void png_push_fill_buffer(png_structp png_ptr, uint8_t* buffer, size_t length);
#endif
#endif /* PNG_1_0_X */

/* Reset the CRC variable */
void png_reset_crc(png_structp png_ptr);

/* Write the "data" buffer to whatever output you are using. */
void png_write_data(png_structp png_ptr, uint8_t* data, size_t length);

/* Read data from whatever input you are using into the "data" buffer */
void png_read_data(png_structp png_ptr, uint8_t* data, size_t length);

/* Read bytes into buf, and update png_ptr->crc */
void png_crc_read(png_structp png_ptr, uint8_t* buf, size_t length);

/* Read "skip" bytes, read the file crc, and (optionally) verify png_ptr->crc */
int png_crc_finish(png_structp png_ptr, uint32_t skip);

/* Read the CRC from the file and compare it to the libpng calculated CRC */
int png_crc_error(png_structp png_ptr);

/* Calculate the CRC over a section of data.  Note that we are only
 * passing a maximum of 64K on systems that have this as a memory limit,
 * since this is the maximum buffer size we can specify.
 */
void png_calculate_crc(png_structp png_ptr, uint8_t* ptr, size_t length);

#if defined(PNG_WRITE_FLUSH_SUPPORTED)
void png_flush(png_structp png_ptr);
#endif

/* simple function to write the signature */
void png_write_sig(png_structp png_ptr);

/* write various chunks */

/* Write the IHDR chunk, and update the png_struct with the necessary
 * information.
 */
void png_write_IHDR(png_structp png_ptr, uint32_t width, uint32_t height, int bit_depth, int color_type, int compression_method, int filter_method, int interlace_method);

void png_write_PLTE(png_structp png_ptr, png_colorp palette, uint32_t num_pal);

void png_write_IDAT(png_structp png_ptr, uint8_t* data, size_t length);

void png_write_IEND(png_structp png_ptr);

#if defined(PNG_WRITE_sBIT_SUPPORTED)
void png_write_sBIT(png_structp png_ptr, png_color_8p sbit, int color_type);
#endif

#if defined(PNG_WRITE_sRGB_SUPPORTED)
void png_write_sRGB(png_structp png_ptr, int intent);
#endif

#if defined(PNG_WRITE_sPLT_SUPPORTED)
void png_write_sPLT(png_structp png_ptr, png_sPLT_tp palette);
#endif

#if defined(PNG_WRITE_tRNS_SUPPORTED)
void png_write_tRNS(png_structp png_ptr, uint8_t* trans, png_color_16p values, int number, int color_type);
#endif

#if defined(PNG_WRITE_hIST_SUPPORTED)
void png_write_hIST(png_structp png_ptr, uint16_t* hist, int num_hist);
#endif

#if defined(PNG_WRITE_pCAL_SUPPORTED) || defined(PNG_WRITE_sPLT_SUPPORTED)
size_t png_check_keyword(png_structp png_ptr, char* key, png_charpp new_key);
#endif

#if defined(PNG_WRITE_pCAL_SUPPORTED)
void png_write_pCAL(png_structp png_ptr, char* purpose, int32_t X0, int32_t X1, int type, int nparams, char* units, png_charpp params);
#endif

#if defined(PNG_WRITE_pHYs_SUPPORTED)
void png_write_pHYs(png_structp png_ptr, uint32_t x_pixels_per_unit, uint32_t y_pixels_per_unit, int unit_type);
#endif

/* Called when finished processing a row of data */
void png_write_finish_row(png_structp png_ptr);

/* Internal use only.   Called before first row of data */
void png_write_start_row(png_structp png_ptr);

/* combine a row of data, dealing with alpha, etc. if requested */
void png_combine_row(png_structp png_ptr, uint8_t* row, int mask);

#if defined(PNG_READ_INTERLACING_SUPPORTED)
void png_do_read_interlace(png_structp png_ptr);
#endif

/* GRR TO DO (2.0 or whenever):  simplify other internal calling interfaces */

#if defined(PNG_WRITE_INTERLACING_SUPPORTED)
/* grab pixels out of a row for an interlaced pass */
void png_do_write_interlace(png_row_infop row_info, uint8_t* row, int pass);
#endif

/* unfilter a row */
void png_read_filter_row(png_structp png_ptr, png_row_infop row_info, uint8_t* row, uint8_t* prev_row, int filter);

/* Choose the best filter to use and filter the row data */
void png_write_find_filter(png_structp png_ptr, png_row_infop row_info);

/* Write out the filtered row. */
void png_write_filtered_row(png_structp png_ptr, uint8_t* filtered_row);
/* finish a row while reading, dealing with interlacing passes, etc. */
void png_read_finish_row(png_structp png_ptr);

/* initialize the row buffers, etc. */
void png_read_start_row(png_structp png_ptr);
/* optional call to update the users info structure */
void png_read_transform_info(png_structp png_ptr, png_infop info_ptr);

/* these are the functions that do the transformations */
#if defined(PNG_READ_FILLER_SUPPORTED)
void png_do_read_filler(png_row_infop row_info, uint8_t* row, uint32_t filler, uint32_t flags);
#endif

#if defined(PNG_READ_SWAP_ALPHA_SUPPORTED)
void png_do_read_swap_alpha(png_row_infop row_info, uint8_t* row);
#endif

#if defined(PNG_WRITE_SWAP_ALPHA_SUPPORTED)
void png_do_write_swap_alpha(png_row_infop row_info, uint8_t* row);
#endif

#if defined(PNG_READ_INVERT_ALPHA_SUPPORTED)
void png_do_read_invert_alpha(png_row_infop row_info, uint8_t* row);
#endif

#if defined(PNG_WRITE_INVERT_ALPHA_SUPPORTED)
void png_do_write_invert_alpha(png_row_infop row_info, uint8_t* row);
#endif

#if defined(PNG_WRITE_FILLER_SUPPORTED) || \
    defined(PNG_READ_STRIP_ALPHA_SUPPORTED)
void png_do_strip_filler(png_row_infop row_info, uint8_t* row, uint32_t flags);
#endif

#if defined(PNG_READ_SWAP_SUPPORTED) || defined(PNG_WRITE_SWAP_SUPPORTED)
void png_do_swap(png_row_infop row_info, uint8_t* row);
#endif

#if defined(PNG_READ_RGB_TO_GRAY_SUPPORTED)
int png_do_rgb_to_gray(png_structp png_ptr, png_row_infop   row_info, uint8_t* row);
#endif

#if defined(PNG_READ_GRAY_TO_RGB_SUPPORTED)
void png_do_gray_to_rgb(png_row_infop row_info, uint8_t* row);
#endif

#if defined(PNG_READ_PACK_SUPPORTED)
void png_do_unpack(png_row_infop row_info, uint8_t* row);
#endif

#if defined(PNG_READ_SHIFT_SUPPORTED)
void png_do_unshift(png_row_infop row_info, uint8_t* row, png_color_8p sig_bits);
#endif

#if defined(PNG_READ_INVERT_SUPPORTED) || defined(PNG_WRITE_INVERT_SUPPORTED)
void png_do_invert(png_row_infop row_info, uint8_t* row);
#endif

#if defined(PNG_READ_16_TO_8_SUPPORTED)
void png_do_chop(png_row_infop row_info, uint8_t* row);
#endif

#if defined(PNG_READ_DITHER_SUPPORTED)
void png_do_dither(png_row_infop row_info, uint8_t* row, uint8_t* palette_lookup, uint8_t* dither_lookup);

#  if defined(PNG_CORRECT_PALETTE_SUPPORTED)
void png_correct_palette(png_structp png_ptr, png_colorp palette, int num_palette);
#  endif
#endif

#if defined(PNG_READ_BGR_SUPPORTED) || defined(PNG_WRITE_BGR_SUPPORTED)
void png_do_bgr(png_row_infop row_info, uint8_t* row);
#endif

#if defined(PNG_WRITE_PACK_SUPPORTED)
void png_do_pack(png_row_infop row_info, uint8_t* row, uint32_t bit_depth);
#endif

#if defined(PNG_WRITE_SHIFT_SUPPORTED)
void png_do_shift(png_row_infop row_info, uint8_t* row, png_color_8p bit_depth);
#endif

#if defined(PNG_READ_EXPAND_SUPPORTED)
void png_do_expand_palette(png_row_infop row_info, uint8_t* row, png_colorp palette, uint8_t* trans, int num_trans);
void png_do_expand(png_row_infop row_info, uint8_t* row, png_color_16p trans_value);
#endif

/* The following decodes the appropriate chunks, and does error correction,
 * then calls the appropriate callback for the chunk if it is valid.
 */

/* decode the IHDR chunk */
void png_handle_IHDR(png_structp png_ptr, png_infop info_ptr, uint32_t length);
void png_handle_PLTE(png_structp png_ptr, png_infop info_ptr, uint32_t length);
void png_handle_IEND(png_structp png_ptr, png_infop info_ptr, uint32_t length);

#if defined(PNG_READ_hIST_SUPPORTED)
void png_handle_hIST(png_structp png_ptr, png_infop info_ptr, uint32_t length);
#endif

#if defined(PNG_READ_pCAL_SUPPORTED)
void png_handle_pCAL(png_structp png_ptr, png_infop info_ptr, uint32_t length);
#endif

#if defined(PNG_READ_pHYs_SUPPORTED)
void png_handle_pHYs(png_structp png_ptr, png_infop info_ptr, uint32_t length);
#endif

#if defined(PNG_READ_sBIT_SUPPORTED)
void png_handle_sBIT(png_structp png_ptr, png_infop info_ptr, uint32_t length);
#endif

#if defined(PNG_READ_sPLT_SUPPORTED)
extern void png_handle_sPLT(png_structp png_ptr, png_infop info_ptr, uint32_t length);
#endif /* PNG_READ_sPLT_SUPPORTED */

#if defined(PNG_READ_sRGB_SUPPORTED)
void png_handle_sRGB(png_structp png_ptr, png_infop info_ptr, uint32_t length);
#endif

#if defined(PNG_READ_tRNS_SUPPORTED)
void png_handle_tRNS(png_structp png_ptr, png_infop info_ptr, uint32_t length);
#endif

void png_handle_unknown(png_structp png_ptr, png_infop info_ptr, uint32_t length);

void png_check_chunk_name(png_structp png_ptr, uint8_t* chunk_name);

/* handle the transformations for reading and writing */
void png_do_read_transformations(png_structp png_ptr);
void png_do_write_transformations(png_structp png_ptr);
 
void png_init_read_transformations(png_structp png_ptr);

#ifdef PNG_PROGRESSIVE_READ_SUPPORTED
void png_push_read_chunk(png_structp png_ptr, png_infop info_ptr);
void png_push_read_sig(png_structp png_ptr, png_infop info_ptr);
void png_push_check_crc(png_structp png_ptr);
void png_push_crc_skip(png_structp png_ptr, uint32_t length);
void png_push_crc_finish(png_structp png_ptr);
void png_push_save_buffer(png_structp png_ptr);
void png_push_restore_buffer(png_structp png_ptr, uint8_t* buffer, size_t buffer_length);
void png_push_read_IDAT(png_structp png_ptr);
void png_process_IDAT_data(png_structp png_ptr, uint8_t* buffer, size_t buffer_length);
void png_push_process_row(png_structp png_ptr);
void png_push_handle_unknown(png_structp png_ptr, png_infop info_ptr, uint32_t length);
void png_push_have_info(png_structp png_ptr, png_infop info_ptr);
void png_push_have_end(png_structp png_ptr, png_infop info_ptr);
void png_push_have_row(png_structp png_ptr, uint8_t* row);
void png_push_read_end(png_structp png_ptr, png_infop info_ptr);
void png_process_some_data(png_structp png_ptr, png_infop info_ptr);
void png_read_push_finish_row(png_structp png_ptr);

#endif /* PNG_PROGRESSIVE_READ_SUPPORTED */


/* Maintainer: Put new private prototypes here ^ and in libpngpf.3 */

#endif /* PNG_INTERNAL */

#ifdef __cplusplus
}
#endif

/* do not put anything past this line */
#endif /* PNG_H */
