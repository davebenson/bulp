#include "bulp-common.h"
#include "bulp-mem-pool.h"
#include <string.h>

#define ALIGN(size)	        _BULP_MEM_POOL_ALIGN(size)
#define SLAB_GET_NEXT_PTR(slab) _BULP_MEM_POOL_SLAB_GET_NEXT_PTR(slab)
#define CHUNK_SIZE	        8192

#define ENABLE_FIXED_MEM_POOL   1

/* --- Allocate-only Memory Pool --- */
/**
 * bulp_mem_pool_init:
 * @pool: the mem-pool to initialize.
 *
 * Initialize the members of a mem-pool.
 * (The memory for the mem-pool structure itself
 * must be provided: either as a part of another
 * structure or on the stack.)
 */

/**
 * bulp_mem_pool_init_buf:
 * @pool: the mem-pool to initialize.
 * @buffer: the buffer to use.
 * @buffer_size: the number of bytes in buffer
 * to use as storage.
 *
 * Initialize the members of a mem-pool,
 * using a scratch-buffer that the user provides.
 * (The caller is responsible for ensuring that
 * the buffer exists long enough)
 */


/**
 * bulp_mem_pool_alloc:
 * @pool: area to allocate memory from.
 * @size: number of bytes to allocate.
 *
 * Allocate memory from a pool,
 * This function terminates the program if there is an
 * out-of-memory condition.
 *
 * returns: the slab of memory allocated from the pool.
 */


/**
 * bulp_mem_pool_alloc_unaligned:
 * @pool: area to allocate memory from.
 * @size: number of bytes to allocate.
 *
 * Allocate memory from a pool, without ensuring that
 * it is aligned.
 *
 * returns: the slab of memory allocated from the pool.
 */

/**
 * bulp_mem_pool_must_alloc:
 * @pool: area to allocate memory from.
 * @size: number of bytes to allocate.
 *
 * private
 *
 * returns: the slab of memory allocated from the pool.
 */

void *
bulp_mem_pool_must_alloc   (BulpMemPool     *pool,
			   size_t          size)
{
  char *rv;
  if (size < CHUNK_SIZE)
    {
      /* Allocate a new chunk. */
      void * carved = bulp_malloc (CHUNK_SIZE + sizeof (void *));
      SLAB_GET_NEXT_PTR (carved) = pool->all_chunk_list;
      pool->all_chunk_list = carved;
      rv = carved;
      rv += sizeof (void *);
      pool->chunk = rv + size;
      pool->chunk_left = CHUNK_SIZE - size;
    }
  else
    {
      /* Allocate a chunk of exactly the right size. */
      void * carved = bulp_malloc (size + sizeof (void *));
      if (pool->all_chunk_list)
	{
	  SLAB_GET_NEXT_PTR (carved) = SLAB_GET_NEXT_PTR (pool->all_chunk_list);
	  SLAB_GET_NEXT_PTR (pool->all_chunk_list) = carved;
	}
      else
	{
	  SLAB_GET_NEXT_PTR (carved) = NULL;
	  pool->all_chunk_list = carved;
	}
      rv = carved;
      rv += sizeof (void *);
    }
  return rv;
}

/**
 * bulp_mem_pool_alloc0:
 * @pool: area to allocate memory from.
 * @size: number of bytes to allocate.
 *
 * Allocate memory from a pool, and initializes it to 0.
 * This function terminates the program if there is an
 * out-of-memory condition.
 *
 * returns: the slab of memory allocated from the pool.
 */
void * bulp_mem_pool_alloc0           (BulpMemPool     *pool,
                                      size_t           size)
{
  return memset (bulp_mem_pool_alloc (pool, size), 0, size);
}

/**
 * bulp_mem_pool_strdup:
 * @pool: area to allocate memory from.
 * @str: a string to duplicate, or NULL.
 *
 * Allocated space from the mem-pool to store
 * the given string (including its terminal
 * NUL character) and copy the string onto that buffer.
 *
 * If @str is NULL, then NULL is returned.
 *
 * returns: a copy of @str, allocated from @pool.
 */
char *
bulp_mem_pool_strdup       (BulpMemPool     *pool,
			   const char     *str)
{
  unsigned L;
  if (str == NULL)
    return NULL;
  L = strlen (str) + 1;
  return memcpy (bulp_mem_pool_alloc_unaligned (pool, L), str, L);
}
char    *bulp_mem_pool_strcut   (BulpMemPool      *pool,
                                const char      *start,
                                const char      *past_end)
{
  unsigned L = past_end - start;
  char *rv;
  if (start == NULL)
    return NULL;
  rv = bulp_mem_pool_alloc_unaligned (pool, L + 1);
  memcpy (rv, start, L);
  rv[L] = '\0';
  return rv;
}

/**
 * bulp_mem_pool_destruct:
 * @pool: the pool to destroy.
 *
 * Destroy all chunk associated with the given mem-pool.
 */
#if 0           /* inlined */
void
bulp_mem_pool_destruct     (BulpMemPool     *pool)
{
  void * slab = pool->all_chunk_list;
  while (slab)
    {
      void * new_slab = SLAB_GET_NEXT_PTR (slab);
      bulp_free (slab);
      slab = new_slab;
    }
#ifdef BULP_DEBUG
  memset (pool, 0xea, sizeof (BulpMemPool));
#endif
}
#endif

/* === Fixed-Size MemPool's === */
/**
 * bulp_mem_pool_fixed_construct:
 * @pool: the fixed-size memory pool to construct.
 * @size: size of the allocation to take from the mem-pool.
 *
 * Set up a fixed-size memory allocator for use.
 */
void
bulp_mem_pool_fixed_construct (BulpMemPoolFixed   *pool,
			      size_t             size)
{
  pool->slab_list = NULL;
  pool->chunk = NULL;
  pool->pieces_left = 0;
  size = ALIGN (size);
  if (size < sizeof (void *))
    size = sizeof (void *);
  pool->piece_size = size;
  pool->free_list = NULL;
}

/**
 * bulp_mem_pool_fixed_alloc:
 * @pool: the pool to allocate a block from.
 *
 * Allocate a block of the Fixed-Pool's size.
 *
 * returns: the allocated memory.
 */
void *
bulp_mem_pool_fixed_alloc     (BulpMemPoolFixed     *pool)
{
#if ENABLE_FIXED_MEM_POOL
  if (pool->free_list)
    {
      void * rv = pool->free_list;
      pool->free_list = SLAB_GET_NEXT_PTR (rv);
      return rv;
    }
  if (pool->pieces_left == 0)
    {
      void * slab = bulp_malloc (256 * pool->piece_size + sizeof (void *));
      SLAB_GET_NEXT_PTR (slab) = pool->slab_list;
      pool->slab_list = slab;
      pool->chunk = slab;
      pool->chunk += sizeof (void *);
      pool->pieces_left = 256;
    }
  {
    void * rv = pool->chunk;
    pool->chunk += pool->piece_size;
    pool->pieces_left--;
    return rv;
  }
#else
  return bulp_malloc (pool->piece_size);
#endif
}

/**
 * bulp_mem_pool_fixed_alloc0:
 * @pool: the pool to allocate a block from.
 *
 * Allocate a block of the Fixed-Pool's size.
 * Set its contents to 0.
 *
 * returns: the allocated, zeroed memory.
 */
void * bulp_mem_pool_fixed_alloc0    (BulpMemPoolFixed     *pool)
{
  return memset (bulp_mem_pool_fixed_alloc (pool), 0, pool->piece_size);
}

/**
 * bulp_mem_pool_fixed_free:
 * @pool: the pool to return memory to.
 * @from_pool: the memory to return to this pool.
 * It must have been allocated from this pool.
 *
 * Recycle some of the pool's memory back to it.
 */
void     bulp_mem_pool_fixed_free      (BulpMemPoolFixed     *pool,
                                       void *        from_pool)
{
#if ENABLE_FIXED_MEM_POOL
  SLAB_GET_NEXT_PTR (from_pool) = pool->free_list;
  pool->free_list = from_pool;
#else
  bulp_free (from_pool);
#endif
}

/**
 * bulp_mem_pool_fixed_destruct:
 * @pool: the pool to destroy.
 *
 * Free all memory associated with this pool.
 */
void     bulp_mem_pool_fixed_destruct  (BulpMemPoolFixed     *pool)
{
  while (pool->slab_list)
    {
      void * kill = pool->slab_list;
      pool->slab_list = SLAB_GET_NEXT_PTR (kill);
      bulp_free (kill);
    }
}
