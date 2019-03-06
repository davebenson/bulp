// a grow-only slab.
//
// this is intended for long-running processes where the max-length will
// typically do all its resizing early on, and will typically reach
// its max size in few reallocation.
//
// but on the other hand, it never allocates any more than what it needs,
// so it might waste less than exponential-resizers.
//

typedef struct BulpSlab BulpSlab;
struct BulpSlab
{
  size_t length;
  size_t alloced;
  uint8_t *data;
};
#define BULP_SLAB_INIT {0,0,NULL}
BULP_INLINE void     bulp_slab_init     (BulpSlab      *slab);
BULP_INLINE uint8_t *bulp_slab_set_size (BulpSlab      *slab,
                                         size_t         size);
BULP_INLINE void     bulp_slab_truncate (BulpSlab      *slab,
                                         size_t         size);
BULP_INLINE uint8_t *bulp_slab_set_data (BulpSlab      *slab,
                                         size_t         size,
                                         const uint8_t *data);
BULP_INLINE uint8_t *bulp_slab_memdup   (BulpSlab      *slab);
BULP_INLINE uint8_t *bulp_slab_get_end  (BulpSlab      *slab);
BULP_INLINE void     bulp_slab_clear    (BulpSlab      *slab);

BULP_INLINE void     bulp_slabs_swap    (BulpSlab      *a,
                                         BulpSlab      *b);
BULP_INLINE void     bulp_slabs_swap_pair(BulpSlab      *pair);

#if BULP_CAN_INLINE || defined(BULP_INTERNAL_IMPLEMENT_INLINE_FUNCTIONS)
BULP_INLINE void     bulp_slab_init     (BulpSlab *slab)
{
  slab->length = 0;
  slab->alloced = 0;
  slab->data = NULL;
}
BULP_INLINE uint8_t *
bulp_slab_set_size (BulpSlab *slab,
                    size_t    size)
{
  if (slab->alloced < size)
    {
       size_t new_alloced = slab->alloced == 0 ? 16 : slab->alloced*2;
      while (new_alloced < size)
        new_alloced *= 2;
      slab->data = realloc (slab->data, new_alloced);
      slab->alloced = new_alloced;
    }
  slab->length = size;
  return slab->data;
}
BULP_INLINE void     bulp_slab_truncate (BulpSlab      *slab,
                                         size_t         size)
{
  BULP_INLINE_ASSERT(slab->length >= size);
  slab->length = size;
}
BULP_INLINE uint8_t *
bulp_slab_set_data (BulpSlab      *slab,
                    size_t         size,
                    const uint8_t *data)
{
  return memcpy (bulp_slab_set_size (slab, size), data, size);
}
BULP_INLINE uint8_t *bulp_slab_memdup   (BulpSlab      *slab)
{
  uint8_t *rv = malloc (slab->length);
  memcpy (rv, slab->data, slab->length);
  return rv;
}
BULP_INLINE uint8_t *bulp_slab_get_end  (BulpSlab      *slab)
{
  return slab->data + slab->length;
}

BULP_INLINE void
bulp_slab_clear    (BulpSlab *slab)
{
  if (slab->data != NULL)
    {
      free (slab->data);
      slab->data = NULL;
    }
}
BULP_INLINE void     bulp_slabs_swap    (BulpSlab      *a,
                                         BulpSlab      *b)
{
  BulpSlab tmp = *a;
  *a = *b;
  *b = tmp;
}
BULP_INLINE void     bulp_slabs_swap_pair(BulpSlab      *pair)
{
  BulpSlab tmp = pair[0];
  pair[0] = pair[1];
  pair[1] = tmp;
}

#endif
