/* gzio.c -- IO on .gz files
 * Copyright (C) 1995-2005 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 *
 * Compile this file with -DNO_GZCOMPRESS to avoid the compression code.
 */

 /* @(#) $Id$ */

#include "stdio.h"
#include "stdarg.h"
#include "zutil.h"


#define Z_BUFSIZE 4096 /* minimize memory usage for 16-bit DOS */
#define Z_PRINTF_BUFSIZE 4096
#define TRYFREE(p) {if (p) free(p);}

static int const gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

typedef struct gz_stream
{
	z_stream stream;
	int			z_err;   /* error code for last stream operation */
	int			z_eof;   /* set if end of input file */
	FILE*		file;   /* .gz file */
	uint8_t*	inbuf;  /* input buffer */
	uint8_t*	outbuf; /* output buffer */
	uint32_t    crc;     /* crc32 of uncompressed data */
	char*		msg;    /* error message */
	char*		path;   /* path name for debugging only */
	int			transparent; /* 1 if input file is not a .gz file */
	char		mode;    /* 'w' or 'r' */
	int32_t		start;   /* start of compressed data in file (header skipped) */
	int32_t		in;      /* bytes into deflate or inflate */
	int32_t		out;     /* bytes out of deflate or inflate */
	int			back;    /* one character push-back */
	int			last;    /* true if push-back is last character */
} gz_stream;


static gzFile	gz_open(const char* path, const char* mode, int  fd);
static int		do_flush(gzFile file, int flush);
static int		get_byte(gz_stream* s);
static void		check_header(gz_stream* s);
static int		destroy(gz_stream* s);
static void		putLong(FILE* file, uint32_t x);
static uint32_t	getLong(gz_stream* s);

/* ===========================================================================
	 Opens a gzip (.gz) file for reading or writing. The mode parameter
   is as in fopen ("rb" or "wb"). The file is given either by file descriptor
   or path name (if fd == -1).
	 gz_open returns NULL if the file could not be opened or if there was
   insufficient memory to allocate the (de)compression state; errno
   can be checked to distinguish the two cases (if errno is zero, the
   zlib error is Z_MEM_ERROR).
*/
static gzFile gz_open(path, mode, fd)
const char* path;
const char* mode;
int  fd;
{
	int			err;
	int			level = Z_DEFAULT_COMPRESSION; /* compression level */
	int			strategy = Z_DEFAULT_STRATEGY; /* compression strategy */
	char*		p = (char*)mode;
	gz_stream*	s;
	char		fmode[80]; /* copy of mode, without the compression level */
	char*		m = fmode;

	if (!path || !mode) return Z_NULL;

	s = (gz_stream*)malloc(sizeof(gz_stream));
	if (!s) return Z_NULL;

	s->stream.zalloc = (alloc_func)0;
	s->stream.zfree = (free_func)0;
	s->stream.opaque = (void*)0;
	s->stream.next_in = s->inbuf = Z_NULL;
	s->stream.next_out = s->outbuf = Z_NULL;
	s->stream.avail_in = s->stream.avail_out = 0;
	s->file = NULL;
	s->z_err = Z_OK;
	s->z_eof = 0;
	s->in = 0;
	s->out = 0;
	s->back = EOF;
	s->crc = crc32(0L, Z_NULL, 0);
	s->msg = NULL;
	s->transparent = 0;

	s->path = (char*)malloc(strlen(path) + 1);
	if (s->path == NULL)
	{
		return destroy(s), (gzFile)Z_NULL;
	}
	strcpy(s->path, path); /* do this early for debugging */

	s->mode = '\0';
	do
	{
		if (*p == 'r')
		{
			s->mode = 'r';
		}
		if (*p == 'w' || *p == 'a')
		{
			s->mode = 'w';
		}
		if (*p >= '0' && *p <= '9')
		{
			level = *p - '0';
		}
		else if (*p == 'f')
		{
			strategy = Z_FILTERED;
		}
		else if (*p == 'h')
		{
			strategy = Z_HUFFMAN_ONLY;
		}
		else if (*p == 'R')
		{
			strategy = Z_RLE;
		}
		else
		{
			*m++ = *p; /* copy the mode */
		}
	}
	while (*p++ && m != fmode + sizeof(fmode));

	if (s->mode == '\0')
	{
		return destroy(s), (gzFile)Z_NULL;
	}

	if (s->mode == 'w')
	{
		err = deflateInit2(&(s->stream), level, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, strategy);
		/* windowBits is passed < 0 to suppress zlib header */

		s->stream.next_out = s->outbuf = (uint8_t*)malloc(Z_BUFSIZE);
		if (err != Z_OK || s->outbuf == Z_NULL)
		{
			return destroy(s), (gzFile)Z_NULL;
		}
	}
	else
	{
		s->stream.next_in = s->inbuf = (uint8_t*)malloc(Z_BUFSIZE);

		err = inflateInit2(&(s->stream), -MAX_WBITS);
		/* windowBits is passed < 0 to tell that there is no zlib header.
		 * Note that in this case inflate *requires* an extra "dummy" byte
		 * after the compressed stream in order to complete decompression and
		 * return Z_STREAM_END. Here the gzip CRC32 ensures that 4 bytes are
		 * present after the compressed stream.
		 */
		if (err != Z_OK || s->inbuf == Z_NULL)
		{
			return destroy(s), (gzFile)Z_NULL;
		}
	}
	s->stream.avail_out = Z_BUFSIZE;

	errno = 0;
	s->file = fd < 0 ? F_OPEN(path, fmode) : (FILE*)fdopen(fd, fmode);

	if (s->file == NULL)
	{
		return destroy(s), (gzFile)Z_NULL;
	}
	if (s->mode == 'w')
	{
		/* Write a very simple .gz header:
		 */
		fprintf(s->file, "%c%c%c%c%c%c%c%c%c%c", gz_magic[0], gz_magic[1],
				Z_DEFLATED, 0 /*flags*/, 0, 0, 0, 0 /*time*/, 0 /*xflags*/, OS_CODE);
		s->start = 10L;
		/* We use 10L instead of ftell(s->file) to because ftell causes an
		 * fflush on some systems. This version of the library doesn't use
		 * start anyway in write mode, so this initialization is not
		 * necessary.
		 */
	}
	else
	{
		check_header(s); /* skip the .gz header */
		s->start = ftell(s->file) - s->stream.avail_in;
	}

	return (gzFile)s;
}

/* ===========================================================================
	 Opens a gzip (.gz) file for reading or writing.
*/
gzFile gzopen(path, mode)
const char* path;
const char* mode;
{
	return gz_open(path, mode, -1);
}

/* ===========================================================================
	 Associate a gzFile with the file descriptor fd. fd is not dup'ed here
   to mimic the behavio(u)r of fdopen.
*/
gzFile gzdopen(fd, mode)
int fd;
const char* mode;
{
	char name[46];      /* allow for up to 128-bit integers */

	if (fd < 0)
	{
		return (gzFile)Z_NULL;
	}
	sprintf(name, "<fd:%d>", fd); /* for debugging */

	return gz_open(name, mode, fd);
}

/* ===========================================================================
 * Update the compression level and strategy
 */
int gzsetparams(file, level, strategy)
gzFile file;
int level;
int strategy;
{
	gz_stream* s = (gz_stream*)file;

	if (s == NULL || s->mode != 'w')
	{
		return Z_STREAM_ERROR;
	}

	/* Make room to allow flushing */
	if (s->stream.avail_out == 0)
	{

		s->stream.next_out = s->outbuf;
		if (fwrite(s->outbuf, 1, Z_BUFSIZE, s->file) != Z_BUFSIZE)
		{
			s->z_err = Z_ERRNO;
		}
		s->stream.avail_out = Z_BUFSIZE;
	}

	return deflateParams(&(s->stream), level, strategy);
}

/* ===========================================================================
	 Read a byte from a gz_stream; update next_in and avail_in. Return EOF
   for end of file.
   IN assertion: the stream s has been sucessfully opened for reading.
*/
static int get_byte(s)
gz_stream* s;
{
	if (s->z_eof)
	{
		return EOF;
	}

	if (s->stream.avail_in == 0)
	{
		errno = 0;
		s->stream.avail_in = (uint32_t)fread(s->inbuf, 1, Z_BUFSIZE, s->file);
		if (s->stream.avail_in == 0)
		{
			s->z_eof = 1;
			if (ferror(s->file)) s->z_err = Z_ERRNO;
			return EOF;
		}
		s->stream.next_in = s->inbuf;
	}
	s->stream.avail_in--;
	return *(s->stream.next_in)++;
}

/* ===========================================================================
	  Check the gzip header of a gz_stream opened for reading. Set the stream
	mode to transparent if the gzip magic header is not present; set s->err
	to Z_DATA_ERROR if the magic header is present but the rest of the header
	is incorrect.
	IN assertion: the stream s has already been created sucessfully;
	   s->stream.avail_in is zero for the first time, but may be non-zero
	   for concatenated .gz files.
*/
static void check_header(s)
gz_stream* s;
{
	int			method; /* method byte */
	int			flags;  /* flags byte */
	uint32_t	len;
	int			c;

	/* Assure two bytes in the buffer so we can peek ahead -- handle case
	   where first byte of header is at the end of the buffer after the last
	   gzip segment */
	len = s->stream.avail_in;
	if (len < 2)
	{
		if (len) s->inbuf[0] = s->stream.next_in[0];
		errno = 0;
		len = (uint32_t)fread(s->inbuf + len, 1, Z_BUFSIZE >> len, s->file);
		if (len == 0 && ferror(s->file))
		{
			s->z_err = Z_ERRNO;
		}

		s->stream.avail_in += len;
		s->stream.next_in = s->inbuf;
		if (s->stream.avail_in < 2)
		{
			s->transparent = s->stream.avail_in;
			return;
		}
	}

	/* Peek ahead to check the gzip magic header */
	if (s->stream.next_in[0] != gz_magic[0] ||
		s->stream.next_in[1] != gz_magic[1])
	{
		s->transparent = 1;
		return;
	}
	s->stream.avail_in -= 2;
	s->stream.next_in += 2;

	/* Check the rest of the gzip header */
	method = get_byte(s);
	flags = get_byte(s);
	if (method != Z_DEFLATED || (flags & RESERVED) != 0)
	{
		s->z_err = Z_DATA_ERROR;
		return;
	}

	/* Discard time, xflags and OS code: */
	for (len = 0; len < 6; len++)
	{
		(void)get_byte(s);
	}

	if ((flags & EXTRA_FIELD) != 0)
	{ /* skip the extra field */
		len = (uint32_t)get_byte(s);
		len += ((uint32_t)get_byte(s)) << 8;
		/* len is garbage if EOF but the loop below will quit anyway */
		while (len-- != 0 && get_byte(s) != EOF);
	}
	if ((flags & ORIG_NAME) != 0)
	{ /* skip the original file name */
		while ((c = get_byte(s)) != 0 && c != EOF);
	}
	if ((flags & COMMENT) != 0)
	{   /* skip the .gz file comment */
		while ((c = get_byte(s)) != 0 && c != EOF);
	}
	if ((flags & HEAD_CRC) != 0)
	{  /* skip the header crc */
		for (len = 0; len < 2; len++)
		{
			(void)get_byte(s);
		}
	}
	s->z_err = s->z_eof ? Z_DATA_ERROR : Z_OK;
}

/* ===========================================================================
* Cleanup then free the given gz_stream. Return a zlib error code.
  Try freeing in the reverse order of allocations.
*/
static int destroy(s)
gz_stream* s;
{
	int err = Z_OK;

	if (!s)
	{
		return Z_STREAM_ERROR;
	}

	TRYFREE(s->msg);

	if (s->stream.state != NULL)
	{
		if (s->mode == 'w')
		{
			err = deflateEnd(&(s->stream));
		}
		else if (s->mode == 'r')
		{
			err = inflateEnd(&(s->stream));
		}
	}
	if (s->file != NULL && fclose(s->file))
	{
		if (errno != ESPIPE)
		{
			err = Z_ERRNO;
		}
	}
	if (s->z_err < 0) err = s->z_err;

	TRYFREE(s->inbuf);
	TRYFREE(s->outbuf);
	TRYFREE(s->path);
	TRYFREE(s);
	return err;
}

/* ===========================================================================
	 Reads the given number of uncompressed bytes from the compressed file.
   gzread returns the number of bytes actually read (0 for end of file).
*/
int gzread(file, buf, len)
gzFile file;
void* buf;
uint32_t len;
{
	gz_stream* s = (gz_stream*)file;
	uint8_t* start = (uint8_t*)buf; /* starting point for crc computation */
	uint8_t* next_out; /* == stream.next_out but not forced far (for MSDOS) */

	if (s == NULL || s->mode != 'r')
	{
		return Z_STREAM_ERROR;
	}

	if (s->z_err == Z_DATA_ERROR || s->z_err == Z_ERRNO)
	{
		return -1;
	}
	if (s->z_err == Z_STREAM_END)
	{
		return 0;  /* EOF */
	}

	next_out = (uint8_t*)buf;
	s->stream.next_out = (uint8_t*)buf;
	s->stream.avail_out = len;

	if (s->stream.avail_out && s->back != EOF)
	{
		*next_out++ = s->back;
		s->stream.next_out++;
		s->stream.avail_out--;
		s->back = EOF;
		s->out++;
		start++;
		if (s->last)
		{
			s->z_err = Z_STREAM_END;
			return 1;
		}
	}

	while (s->stream.avail_out != 0)
	{

		if (s->transparent)
		{
			/* Copy first the lookahead bytes: */
			uint32_t n = s->stream.avail_in;
			if (n > s->stream.avail_out) n = s->stream.avail_out;
			if (n > 0)
			{
				memcpy(s->stream.next_out, s->stream.next_in, n);
				next_out += n;
				s->stream.next_out = next_out;
				s->stream.next_in += n;
				s->stream.avail_out -= n;
				s->stream.avail_in -= n;
			}
			if (s->stream.avail_out > 0)
			{
				s->stream.avail_out -= (uint32_t)fread(next_out, 1, s->stream.avail_out, s->file);
			}
			len -= s->stream.avail_out;
			s->in += len;
			s->out += len;
			if (len == 0) s->z_eof = 1;
			return (int)len;
		}
		if (s->stream.avail_in == 0 && !s->z_eof)
		{

			errno = 0;
			s->stream.avail_in = (uint32_t)fread(s->inbuf, 1, Z_BUFSIZE, s->file);
			if (s->stream.avail_in == 0)
			{
				s->z_eof = 1;
				if (ferror(s->file))
				{
					s->z_err = Z_ERRNO;
					break;
				}
			}
			s->stream.next_in = s->inbuf;
		}
		s->in += s->stream.avail_in;
		s->out += s->stream.avail_out;
		s->z_err = inflate(&(s->stream), Z_NO_FLUSH);
		s->in -= s->stream.avail_in;
		s->out -= s->stream.avail_out;

		if (s->z_err == Z_STREAM_END)
		{
			/* Check CRC and original size */
			s->crc = crc32(s->crc, start, (uint32_t)(s->stream.next_out - start));
			start = s->stream.next_out;

			if (getLong(s) != s->crc)
			{
				s->z_err = Z_DATA_ERROR;
			}
			else
			{
				(void)getLong(s);
				/* The uncompressed length returned by above getlong() may be
				 * different from s->out in case of concatenated .gz files.
				 * Check for such files:
				 */
				check_header(s);
				if (s->z_err == Z_OK)
				{
					inflateReset(&(s->stream));
					s->crc = crc32(0L, Z_NULL, 0);
				}
			}
		}
		if (s->z_err != Z_OK || s->z_eof) break;
	}
	s->crc = crc32(s->crc, start, (uint32_t)(s->stream.next_out - start));

	if (len == s->stream.avail_out && (s->z_err == Z_DATA_ERROR || s->z_err == Z_ERRNO))
	{
		return -1;
	}
	return (int)(len - s->stream.avail_out);
}


/* ===========================================================================
	  Reads one byte from the compressed file. gzgetc returns this byte
   or -1 in case of end of file or error.
*/
int gzgetc(file)
gzFile file;
{
	uint8_t c;

	return gzread(file, &c, 1) == 1 ? c : -1;
}


/* ===========================================================================
	  Push one byte back onto the stream.
*/
int gzungetc(c, file)
int c;
gzFile file;
{
	gz_stream* s = (gz_stream*)file;

	if (s == NULL || s->mode != 'r' || c == EOF || s->back != EOF)
	{
		return EOF;
	}
	s->back = c;
	s->out--;
	s->last = (s->z_err == Z_STREAM_END);
	if (s->last)
	{
		s->z_err = Z_OK;
	}
	s->z_eof = 0;
	return c;
}


/* ===========================================================================
	  Reads bytes from the compressed file until len-1 characters are
   read, or a newline character is read and transferred to buf, or an
   end-of-file condition is encountered.  The string is then terminated
   with a null character.
	  gzgets returns buf, or Z_NULL in case of error.

	  The current implementation is not optimized at all.
*/
char* gzgets(file, buf, len)
gzFile file;
char* buf;
int len;
{
	char* b = buf;
	if (buf == Z_NULL || len <= 0)
	{
		return Z_NULL;
	}

	while (--len > 0 && gzread(file, buf, 1) == 1 && *buf++ != '\n');
	*buf = '\0';
	return b == buf && len > 0 ? Z_NULL : b;
}


/* ===========================================================================
	 Writes the given number of uncompressed bytes into the compressed file.
   gzwrite returns the number of bytes actually written (0 in case of error).
*/
int gzwrite(file, buf, len)
gzFile file;
void const* buf;
uint32_t len;
{
	gz_stream* s = (gz_stream*)file;

	if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

	s->stream.next_in = (uint8_t*)buf;
	s->stream.avail_in = len;

	while (s->stream.avail_in != 0)
	{

		if (s->stream.avail_out == 0)
		{

			s->stream.next_out = s->outbuf;
			if (fwrite(s->outbuf, 1, Z_BUFSIZE, s->file) != Z_BUFSIZE)
			{
				s->z_err = Z_ERRNO;
				break;
			}
			s->stream.avail_out = Z_BUFSIZE;
		}
		s->in += s->stream.avail_in;
		s->out += s->stream.avail_out;
		s->z_err = deflate(&(s->stream), Z_NO_FLUSH);
		s->in -= s->stream.avail_in;
		s->out -= s->stream.avail_out;
		if (s->z_err != Z_OK) break;
	}
	s->crc = crc32(s->crc, (const uint8_t*)buf, len);

	return (int)(len - s->stream.avail_in);
}


/* ===========================================================================
	 Converts, formats, and writes the args to the compressed file under
   control of the format string, as in fprintf. gzprintf returns the number of
   uncompressed bytes actually written (0 in case of error).
*/

int gzprintf(gzFile file, const char* format, /* args */ ...)
{
	char buf[Z_PRINTF_BUFSIZE];
	va_list va;
	int len;

	buf[sizeof(buf) - 1] = 0;
	va_start(va, format);
	len = vsnprintf(buf, sizeof(buf), format, va);
	va_end(va);
	if (len <= 0 || len >= (int)sizeof(buf) || buf[sizeof(buf) - 1] != 0)
		return 0;
	return gzwrite(file, buf, (uint32_t)len);
}


/* ===========================================================================
	  Writes c, converted to an uint8_t, into the compressed file.
   gzputc returns the value that was written, or -1 in case of error.
*/
int gzputc(file, c)
gzFile file;
int c;
{
	uint8_t cc = (uint8_t)c; /* required for big endian systems */

	return gzwrite(file, &cc, 1) == 1 ? (int)cc : -1;
}


/* ===========================================================================
	  Writes the given null-terminated string to the compressed file, excluding
   the terminating null character.
	  gzputs returns the number of characters written, or -1 in case of error.
*/
int gzputs(file, s)
gzFile file;
const char* s;
{
	return gzwrite(file, (char*)s, (uint32_t)strlen(s));
}


/* ===========================================================================
	 Flushes all pending output into the compressed file. The parameter
   flush is as in the deflate() function.
*/
static int do_flush(file, flush)
gzFile file;
int flush;
{
	uint32_t len;
	int done = 0;
	gz_stream* s = (gz_stream*)file;

	if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

	s->stream.avail_in = 0; /* should be zero already anyway */

	for (;;)
	{
		len = Z_BUFSIZE - s->stream.avail_out;

		if (len != 0)
		{
			if ((uint32_t)fwrite(s->outbuf, 1, len, s->file) != len)
			{
				s->z_err = Z_ERRNO;
				return Z_ERRNO;
			}
			s->stream.next_out = s->outbuf;
			s->stream.avail_out = Z_BUFSIZE;
		}
		if (done) break;
		s->out += s->stream.avail_out;
		s->z_err = deflate(&(s->stream), flush);
		s->out -= s->stream.avail_out;

		/* Ignore the second of two consecutive flushes: */
		if (len == 0 && s->z_err == Z_BUF_ERROR) s->z_err = Z_OK;

		/* deflate has finished flushing only when it hasn't used up
		 * all the available space in the output buffer:
		 */
		done = (s->stream.avail_out != 0 || s->z_err == Z_STREAM_END);

		if (s->z_err != Z_OK && s->z_err != Z_STREAM_END) break;
	}
	return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
}

int gzflush(file, flush)
gzFile file;
int flush;
{
	gz_stream* s = (gz_stream*)file;
	int err = do_flush(file, flush);

	if (err) return err;
	fflush(s->file);
	return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
}


/* ===========================================================================
	  Sets the starting position for the next gzread or gzwrite on the given
   compressed file. The offset represents a number of bytes in the
	  gzseek returns the resulting offset location as measured in bytes from
   the beginning of the uncompressed stream, or -1 in case of error.
	  SEEK_END is not implemented, returns error.
	  In this version of the library, gzseek can be extremely slow.
*/
int32_t gzseek(file, offset, whence)
gzFile file;
int32_t offset;
int whence;
{
	gz_stream* s = (gz_stream*)file;

	if (s == NULL || whence == SEEK_END ||
		s->z_err == Z_ERRNO || s->z_err == Z_DATA_ERROR)
	{
		return -1L;
	}

	if (s->mode == 'w')
	{
		if (whence == SEEK_SET)
		{
			offset -= s->in;
		}
		if (offset < 0) return -1L;

		/* At this point, offset is the number of zero bytes to write. */
		if (s->inbuf == Z_NULL)
		{
			s->inbuf = (uint8_t*)malloc(Z_BUFSIZE); /* for seeking */
			if (s->inbuf == Z_NULL) return -1L;
			memzero(s->inbuf, Z_BUFSIZE);
		}
		while (offset > 0)
		{
			uint32_t size = Z_BUFSIZE;
			if (offset < Z_BUFSIZE) size = (uint32_t)offset;

			size = gzwrite(file, s->inbuf, size);
			if (size == 0) return -1L;

			offset -= size;
		}
		return s->in;
	}
	/* Rest of function is for reading only */

	/* compute absolute position */
	if (whence == SEEK_CUR)
	{
		offset += s->out;
	}
	if (offset < 0) return -1L;

	if (s->transparent)
	{
		/* map to fseek */
		s->back = EOF;
		s->stream.avail_in = 0;
		s->stream.next_in = s->inbuf;
		if (fseek(s->file, offset, SEEK_SET) < 0) return -1L;

		s->in = s->out = offset;
		return offset;
	}

	/* For a negative seek, rewind and use positive seek */
	if (offset >= s->out)
	{
		offset -= s->out;
	}
	else if (gzrewind(file) < 0)
	{
		return -1L;
	}
	/* offset is now the number of bytes to skip. */

	if (offset != 0 && s->outbuf == Z_NULL)
	{
		s->outbuf = (uint8_t*)malloc(Z_BUFSIZE);
		if (s->outbuf == Z_NULL) return -1L;
	}
	if (offset && s->back != EOF)
	{
		s->back = EOF;
		s->out++;
		offset--;
		if (s->last) s->z_err = Z_STREAM_END;
	}
	while (offset > 0)
	{
		int size = Z_BUFSIZE;
		if (offset < Z_BUFSIZE) size = (int)offset;

		size = gzread(file, s->outbuf, (uint32_t)size);
		if (size <= 0) return -1L;
		offset -= size;
	}
	return s->out;
}

/* ===========================================================================
	 Rewinds input file.
*/
int gzrewind(file)
gzFile file;
{
	gz_stream* s = (gz_stream*)file;

	if (s == NULL || s->mode != 'r') return -1;

	s->z_err = Z_OK;
	s->z_eof = 0;
	s->back = EOF;
	s->stream.avail_in = 0;
	s->stream.next_in = s->inbuf;
	s->crc = crc32(0L, Z_NULL, 0);
	if (!s->transparent) (void)inflateReset(&s->stream);
	s->in = 0;
	s->out = 0;
	return fseek(s->file, s->start, SEEK_SET);
}

/* ===========================================================================
	 Returns the starting position for the next gzread or gzwrite on the
   given compressed file. This position represents a number of bytes in the
   uncompressed data stream.
*/
int32_t gztell(file)
gzFile file;
{
	return gzseek(file, 0L, SEEK_CUR);
}

/* ===========================================================================
	 Returns 1 when EOF has previously been detected reading the given
   input stream, otherwise zero.
*/
int gzeof(file)
gzFile file;
{
	gz_stream* s = (gz_stream*)file;

	/* With concatenated compressed files that can have embedded
	 * crc trailers, z_eof is no longer the only/best indicator of EOF
	 * on a gz_stream. Handle end-of-stream error explicitly here.
	 */
	if (s == NULL || s->mode != 'r') return 0;
	if (s->z_eof) return 1;
	return s->z_err == Z_STREAM_END;
}

/* ===========================================================================
	 Returns 1 if reading and doing so transparently, otherwise zero.
*/
int gzdirect(file)
gzFile file;
{
	gz_stream* s = (gz_stream*)file;

	if (s == NULL || s->mode != 'r') return 0;
	return s->transparent;
}

/* ===========================================================================
   Outputs a long in LSB order to the given file
*/
static void putLong(file, x)
FILE* file;
uint32_t x;
{
	int n;
	for (n = 0; n < 4; n++)
	{
		fputc((int)(x & 0xff), file);
		x >>= 8;
	}
}

/* ===========================================================================
   Reads a long in LSB order from the given gz_stream. Sets z_err in case
   of error.
*/
static uint32_t getLong(s)
gz_stream* s;
{
	uint32_t x = (uint32_t)get_byte(s);
	int c;

	x += ((uint32_t)get_byte(s)) << 8;
	x += ((uint32_t)get_byte(s)) << 16;
	c = get_byte(s);
	if (c == EOF) s->z_err = Z_DATA_ERROR;
	x += ((uint32_t)c) << 24;
	return x;
}

/* ===========================================================================
	 Flushes all pending output if necessary, closes the compressed file
   and deallocates all the (de)compression state.
*/
int gzclose(file)
gzFile file;
{
	gz_stream* s = (gz_stream*)file;

	if (s == NULL) return Z_STREAM_ERROR;

	if (s->mode == 'w')
	{
		if (do_flush(file, Z_FINISH) != Z_OK)
			return destroy((gz_stream*)file);

		putLong(s->file, s->crc);
		putLong(s->file, (uint32_t)(s->in & 0xffffffff));
	}
	return destroy((gz_stream*)file);
}


/* ===========================================================================
	 Returns the error message for the last error which occurred on the
   given compressed file. errnum is set to zlib error number. If an
   error occurred in the file system and not in the compression library,
   errnum is set to Z_ERRNO and the application may consult errno
   to get the exact error code.
*/
const char* gzerror(file, errnum)
gzFile file;
int* errnum;
{
	char* m;
	gz_stream* s = (gz_stream*)file;

	if (s == NULL)
	{
		*errnum = Z_STREAM_ERROR;
		return (const char*)ERR_MSG(Z_STREAM_ERROR);
	}
	*errnum = s->z_err;
	if (*errnum == Z_OK) return (const char*)"";

	m = (char*)(*errnum == Z_ERRNO ? strerror(errno) : s->stream.msg);

	if (m == NULL || *m == '\0') m = (char*)ERR_MSG(s->z_err);

	TRYFREE(s->msg);
	s->msg = (char*)malloc(strlen(s->path) + strlen(m) + 3);
	if (s->msg == Z_NULL) return (const char*)ERR_MSG(Z_MEM_ERROR);
	strcpy(s->msg, s->path);
	strcat(s->msg, ": ");
	strcat(s->msg, m);
	return (const char*)s->msg;
}

/* ===========================================================================
	 Clear the error and end-of-file flags, and do the same for the real file.
*/
void gzclearerr(file)
gzFile file;
{
	gz_stream* s = (gz_stream*)file;

	if (s == NULL) return;
	if (s->z_err != Z_STREAM_END) s->z_err = Z_OK;
	s->z_eof = 0;
	clearerr(s->file);
}
