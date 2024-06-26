
/* pngmem.c - stub functions for memory allocation
 *
 * Last changed in libpng 1.2.13 November 13, 2006
 * For conditions of distribution and use, see copyright notice in png.h
 * Copyright (c) 1998-2006 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * This file provides a location for all memory allocation.  Users who
 * need special memory handling are expected to supply replacement
 * functions for png_malloc() and png_free(), and to use
 * png_create_read_struct_2() and png_create_write_struct_2() to
 * identify the replacement functions.
 */

#include "png.h"

#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)

/* Allocate memory for a png_struct or a png_info.  The malloc and
   memset can be replaced by a single call to calloc() if this is thought
   to improve performance noticably. */
void* png_create_struct(int type)
{
#ifdef PNG_USER_MEM_SUPPORTED
   return (png_create_struct_2(type, (png_malloc_ptr)NULL, (void*)NULL));
}

/* Allocate memory for a png_struct or a png_info.  The malloc and
   memset can be replaced by a single call to calloc() if this is thought
   to improve performance noticably. */
void* png_create_struct_2(int type, png_malloc_ptr malloc_fn, void* mem_ptr)
{
#endif /* PNG_USER_MEM_SUPPORTED */
   size_t size;
   void* struct_ptr;

   if (type == PNG_STRUCT_INFO)
      size = sizeof(png_info);
   else if (type == PNG_STRUCT_PNG)
      size = sizeof(png_struct);
   else
      return (NULL);

#ifdef PNG_USER_MEM_SUPPORTED
   if(malloc_fn != NULL)
   {
      png_struct dummy_struct;
      png_structp png_ptr = &dummy_struct;
      png_ptr->mem_ptr=mem_ptr;
      struct_ptr = (*(malloc_fn))(png_ptr, size);
      if (struct_ptr != NULL)
         memset(struct_ptr, 0, size);
      return (struct_ptr);
   }
#endif /* PNG_USER_MEM_SUPPORTED */

   struct_ptr = (void*)malloc(size);
   if (struct_ptr != NULL)
      memset(struct_ptr, 0, size);

   return (struct_ptr);
}


/* Free memory allocated by a png_create_struct() call */
void png_destroy_struct(void* struct_ptr)
{
#ifdef PNG_USER_MEM_SUPPORTED
   png_destroy_struct_2(struct_ptr, (png_free_ptr)NULL, (void*)NULL);
}

/* Free memory allocated by a png_create_struct() call */
void png_destroy_struct_2(void* struct_ptr, png_free_ptr free_fn,
    void* mem_ptr)
{
#endif /* PNG_USER_MEM_SUPPORTED */
   if (struct_ptr != NULL)
   {
#ifdef PNG_USER_MEM_SUPPORTED
      if(free_fn != NULL)
      {
         png_struct dummy_struct;
         png_structp png_ptr = &dummy_struct;
         png_ptr->mem_ptr=mem_ptr;
         (*(free_fn))(png_ptr, struct_ptr);
         return;
      }
#endif /* PNG_USER_MEM_SUPPORTED */
      free(struct_ptr);
   }
}

/* Allocate memory.  For reasonable files, size should never exceed
   64K.  However, zlib may allocate more then 64K if you don't tell
   it not to.  See zconf.h and png.h for more information.  zlib does
   need to allocate exactly 64K, so whatever you call here must
   have the ability to do that. */

void*
png_malloc(png_structp png_ptr, uint32_t size)
{
   void* ret;

#ifdef PNG_USER_MEM_SUPPORTED
   if (png_ptr == NULL || size == 0)
      return (NULL);

   if(png_ptr->malloc_fn != NULL)
       ret = ((void*)(*(png_ptr->malloc_fn))(png_ptr, (size_t)size));
   else
       ret = (png_malloc_default(png_ptr, size));
   if (ret == NULL && (png_ptr->flags&PNG_FLAG_MALLOC_NULL_MEM_OK) == 0)
       png_error(png_ptr, "Out of Memory!");
   return (ret);
}

void*
png_malloc_default(png_structp png_ptr, uint32_t size)
{
   void* ret;
#endif /* PNG_USER_MEM_SUPPORTED */

   if (png_ptr == NULL || size == 0)
      return (NULL);

#ifdef PNG_MAX_MALLOC_64K
   if (size > (uint32_t)65536L)
   {
#ifndef PNG_USER_MEM_SUPPORTED
      if(png_ptr->flags&PNG_FLAG_MALLOC_NULL_MEM_OK) == 0)
         png_error(png_ptr, "Cannot Allocate > 64K");
      else
#endif
         return NULL;
   }
#endif

 /* Check for overflow */
 if (size != (size_t)size)
   ret = NULL;
 else
   ret = malloc((size_t)size);

#ifndef PNG_USER_MEM_SUPPORTED
   if (ret == NULL && (png_ptr->flags&PNG_FLAG_MALLOC_NULL_MEM_OK) == 0)
      png_error(png_ptr, "Out of Memory");
#endif

   return (ret);
}

/* Free a pointer allocated by png_malloc().  If ptr is NULL, return
   without taking any action. */
void
png_free(png_structp png_ptr, void* ptr)
{
   if (png_ptr == NULL || ptr == NULL)
      return;

#ifdef PNG_USER_MEM_SUPPORTED
   if (png_ptr->free_fn != NULL)
   {
      (*(png_ptr->free_fn))(png_ptr, ptr);
      return;
   }
   else png_free_default(png_ptr, ptr);
}
void
png_free_default(png_structp png_ptr, void* ptr)
{
   if (png_ptr == NULL || ptr == NULL)
      return;

#endif /* PNG_USER_MEM_SUPPORTED */

   free(ptr);
}


/* This function was added at libpng version 1.2.3.  The png_malloc_warn()
 * function will set up png_malloc() to issue a png_warning and return NULL
 * instead of issuing a png_error, if it fails to allocate the requested
 * memory.
 */
void*
png_malloc_warn(png_structp png_ptr, uint32_t size)
{
   void* ptr;
   uint32_t save_flags;
   if(png_ptr == NULL) return (NULL);

   save_flags=png_ptr->flags;
   png_ptr->flags|=PNG_FLAG_MALLOC_NULL_MEM_OK;
   ptr = (void*)png_malloc((png_structp)png_ptr, size);
   png_ptr->flags=save_flags;
   return(ptr);
}

void*
png_memcpy_check (png_structp png_ptr, void* s1, void* s2,
   uint32_t length)
{
   size_t size;

   size = (size_t)length;
   if ((uint32_t)size != length)
      png_error(png_ptr,"Overflow in png_memcpy_check.");

   return(memcpy (s1, s2, size));
}

void*
png_memset_check (png_structp png_ptr, void* s1, int value,
   uint32_t length)
{
   size_t size;

   size = (size_t)length;
   if ((uint32_t)size != length)
      png_error(png_ptr,"Overflow in png_memset_check.");

   return (memset (s1, value, size));

}

#ifdef PNG_USER_MEM_SUPPORTED
/* This function is called when the application wants to use another method
 * of allocating and freeing memory.
 */
void
png_set_mem_fn(png_structp png_ptr, void* mem_ptr, png_malloc_ptr
  malloc_fn, png_free_ptr free_fn)
{
   if(png_ptr != NULL) {
   png_ptr->mem_ptr = mem_ptr;
   png_ptr->malloc_fn = malloc_fn;
   png_ptr->free_fn = free_fn;
   }
}

/* This function returns a pointer to the mem_ptr associated with the user
 * functions.  The application should free any memory associated with this
 * pointer before png_write_destroy and png_read_destroy are called.
 */
void*
png_get_mem_ptr(png_structp png_ptr)
{
   if(png_ptr == NULL) return (NULL);
   return ((void*)png_ptr->mem_ptr);
}
#endif /* PNG_USER_MEM_SUPPORTED */
#endif /* PNG_READ_SUPPORTED || PNG_WRITE_SUPPORTED */
