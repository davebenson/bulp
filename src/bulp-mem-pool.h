
typedef struct _BulpMemPool BulpMemPool;
typedef struct _BulpMemPoolFixed BulpMemPoolFixed;

/* --- Allocate-only Memory Pool --- */
struct _BulpMemPool
{
  /*< private >*/
  void * all_chunk_list;
  char *chunk;
  unsigned chunk_left;
};

#define BULP_MEM_POOL_STATIC_INIT                        { NULL, NULL, 0 }


BULP_INLINE void     bulp_mem_pool_init     (BulpMemPool     *pool);
BULP_INLINE void     bulp_mem_pool_init_buf (BulpMemPool     *pool,
                                                size_t          buffer_size,
                                                void           *buffer);
BULP_INLINE void    *bulp_mem_pool_alloc    (BulpMemPool     *pool,
                                                size_t          size);
                void    *bulp_mem_pool_alloc0   (BulpMemPool     *pool,
                                                size_t          size);
BULP_INLINE void    *bulp_mem_pool_alloc_unaligned(BulpMemPool *pool,
                                                size_t          size);
                char    *bulp_mem_pool_strdup   (BulpMemPool      *pool,
                                                const char      *str);
                char    *bulp_mem_pool_strcut   (BulpMemPool      *pool,
                                                const char      *start,
                                                const char      *past_end);
BULP_INLINE void     bulp_mem_pool_clear    (BulpMemPool     *pool);

/* --- Allocate and free Memory Pool --- */
struct _BulpMemPoolFixed
{
  /*< private >*/
  void * slab_list;
  char *chunk;
  unsigned pieces_left;
  unsigned piece_size;
  void * free_list;
};

#define BULP_MEM_POOL_FIXED_STATIC_INIT(size) \
                          { NULL, NULL, 0, size, NULL } 

BULP_INLINE void     bulp_mem_pool_fixed_init_buf
                                                 (BulpMemPoolFixed *pool,
                                                  size_t            elt_size,
                                                  size_t            buffer_n_elements,
                                                  void    *         buffer);
void     bulp_mem_pool_fixed_init (BulpMemPoolFixed  *pool,
                                       unsigned           size);

/* (for technical discussion about inlining these functions
   see inlining-notes.txt) */
void   * bulp_mem_pool_fixed_alloc     (BulpMemPoolFixed  *pool);
void   * bulp_mem_pool_fixed_alloc0    (BulpMemPoolFixed  *pool);
void     bulp_mem_pool_fixed_free      (BulpMemPoolFixed  *pool,
                                       void             *from_pool);
void     bulp_mem_pool_fixed_clear     (BulpMemPoolFixed  *pool);



/* private */
void * bulp_mem_pool_must_alloc (BulpMemPool *pool,
                                  size_t     size);

/* ------------------------------*/
/* -- Inline Implementations --- */

#define _BULP_MEM_POOL_ALIGN(size)	\
  (((size) + sizeof(void *) - 1) / sizeof (void *) * sizeof (void *))
#define _BULP_MEM_POOL_SLAB_GET_NEXT_PTR(slab) \
  (* (void **) (slab))

#if defined(BULP_CAN_INLINE) || defined(BULP_IMPLEMENT_INLINES)
BULP_INLINE void     bulp_mem_pool_init    (BulpMemPool     *pool)
{
  pool->all_chunk_list = NULL;
  pool->chunk = NULL;
  pool->chunk_left = 0;
}
BULP_INLINE void     bulp_mem_pool_init_buf   (BulpMemPool     *pool,
                                                  size_t           buffer_size,
                                                  void *        buffer)
{
  pool->all_chunk_list = NULL;
  pool->chunk = buffer;
  pool->chunk_left = buffer_size;
}

BULP_INLINE void     bulp_mem_pool_align        (BulpMemPool     *pool)
{
  unsigned mask = ((size_t) (pool->chunk)) & (sizeof(void *)-1);
  if (mask)
    {
      /* need to align chunk */
      unsigned align = sizeof (void *) - mask;
      pool->chunk_left -= align;
      pool->chunk = (char*)pool->chunk + align;
    }
}

BULP_INLINE void * bulp_mem_pool_alloc_unaligned   (BulpMemPool     *pool,
                                                       size_t           size)
{
  char *rv;
  if (BULP_LIKELY (pool->chunk_left >= size))
    {
      rv = pool->chunk;
      pool->chunk_left -= size;
      pool->chunk = rv + size;
      return rv;
    }
  else
    /* fall through to non-inline version for
       slow malloc-using case */
    return bulp_mem_pool_must_alloc (pool, size);
}

BULP_INLINE void * bulp_mem_pool_alloc            (BulpMemPool     *pool,
                                                      size_t           size)
{
  bulp_mem_pool_align (pool);
  return bulp_mem_pool_alloc_unaligned (pool, size);
}

BULP_INLINE void     bulp_mem_pool_clear     (BulpMemPool     *pool)
{
  void * slab = pool->all_chunk_list;
  while (slab)
    {
      void * new_slab = _BULP_MEM_POOL_SLAB_GET_NEXT_PTR (slab);
      bulp_free (slab);
      slab = new_slab;
    }
}
BULP_INLINE void     bulp_mem_pool_fixed_init_buf
                                                 (BulpMemPoolFixed *pool,
                                                  size_t           elt_size,
                                                  size_t           buffer_n_elements,
                                                  void *           buffer)
{
  pool->slab_list = NULL;
  pool->chunk = buffer;
  pool->pieces_left = buffer_n_elements;
  pool->piece_size = elt_size;
  pool->free_list = NULL;
}

#endif /* BULP_CAN_INLINE */

