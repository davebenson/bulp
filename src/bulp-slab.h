
typedef struct BulpSlab BulpSlab;
struct BulpSlab
{
  size_t length;
  size_t alloced;
  uint8_t *data;
};

BULP_INLINE void     bulp_slab_init     (BulpSlab *slab);
BULP_INLINE uint8_t *bulp_slab_set_size (BulpSlab *slab,
                                         size_t    size);
BULP_INLINE void     bulp_slab_clear    (BulpSlab *slab);

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

BULP_INLINE void
bulp_slab_clear    (BulpSlab *slab)
{
  if (slab->data != NULL)
    slab->data = NULL;
}

#endif
