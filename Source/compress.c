/* compress.c -- compress a memory buffer
 * Copyright (C) 1995-2003 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#include "zlib.h"

/* ===========================================================================
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least 0.1% larger than sourceLen plus
   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

     compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/
int compress2 (dest, destLen, source, sourceLen, level)
    uint8_t *dest;
    uint32_t *destLen;
    const uint8_t *source;
    uint32_t sourceLen;
    int level;
{
    z_stream    stream;
    int         err;

    stream.next_in = (uint8_t*)source;
    stream.avail_in = (uint32_t)sourceLen;
    stream.next_out = dest;
    stream.avail_out = (uint32_t)*destLen;
    if ((uint32_t)stream.avail_out != *destLen)
    {
        return Z_BUF_ERROR;
    }

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (void *)0;

    err = deflateInit(&stream, level);
    if (err != Z_OK)
    {
        return err;
    }

    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) 
    {
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;

    err = deflateEnd(&stream);
    return err;
}

/* ===========================================================================
 */
int compress (dest, destLen, source, sourceLen)
    uint8_t *dest;
    uint32_t *destLen;
    const uint8_t *source;
    uint32_t sourceLen;
{
    return compress2(dest, destLen, source, sourceLen, Z_DEFAULT_COMPRESSION);
}

/* ===========================================================================
     If the default memLevel or windowBits for deflateInit() is changed, then
   this function needs to be updated.
 */
uint32_t compressBound (sourceLen)
    uint32_t sourceLen;
{
    return sourceLen + (sourceLen >> 12) + (sourceLen >> 14) + 11;
}

