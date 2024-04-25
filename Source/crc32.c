/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-2005 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 *
 * Thanks to Rodney Brown <rbrown64@csc.com.au> for his contribution of faster
 * CRC methods: exclusive-oring 32 bits of data at a time, and pre-computing
 * tables for updating the shift register in one step with three exclusive-ors
 * instead of four steps with four exclusive-ors.  This results in about a
 * factor of two increase in speed on a Power PC G4 (PPC7455) using gcc -O3.
 */

#include "zutil.h"

#define TBLS 1
#include "crc32.h"

#define GF2_DIM 32      /* dimension of GF(2) vectors (length of CRC) */

/* Local functions for crc concatenation */
static uint32_t gf2_matrix_times (uint32_t *mat, uint32_t vec);
static void gf2_matrix_square (uint32_t *square, uint32_t *mat);


/* =========================================================================
 * This function can be used by asm versions of crc32()
 */
const uint32_t *get_crc_table()
{
    return (const uint32_t *)crc_table;
}

/* ========================================================================= */
#define DO1 crc = crc_table[0][((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8)
#define DO8 DO1; DO1; DO1; DO1; DO1; DO1; DO1; DO1

/* ========================================================================= */
uint32_t crc32(crc, buf, len)
    uint32_t crc;
    const uint8_t *buf;
    uint32_t len;
{
    if (buf == Z_NULL)
    {
        return 0UL;
    }

    crc = crc ^ 0xffffffffUL;
    while (len >= 8) 
    {
        DO8;
        len -= 8;
    }
    if (len)
    {
        do
        {
            DO1;
        }
        while (--len);
    }
    return crc ^ 0xffffffffUL;
}


/* ========================================================================= */
static uint32_t gf2_matrix_times(mat, vec)
    uint32_t *mat;
    uint32_t vec;
{
    uint32_t sum;

    sum = 0;
    while (vec) {
        if (vec & 1)
            sum ^= *mat;
        vec >>= 1;
        mat++;
    }
    return sum;
}

/* ========================================================================= */
static void gf2_matrix_square(square, mat)
    uint32_t *square;
    uint32_t *mat;
{
    int n;

    for (n = 0; n < GF2_DIM; n++)
        square[n] = gf2_matrix_times(mat, mat[n]);
}

