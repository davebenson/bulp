

typedef struct BulpMerger BulpMerger;
struct BulpMerger
{
  void (*merge) (BulpMerger *merger,
                 size_t a_length,
                 const uint8_t *a_data,
                 size_t b_length,
                 const uint8_t *b_data,
                 BulpSlab *out);
  void (*destroy) (BulpMerger *merger);
  BulpFormat *format;
  unsigned ref_count;
};

typedef struct BulpComparator BulpComparator;
struct BulpComparator
{
  int  (*compare) (BulpComparator *comparator,
                   size_t a_length,
                   const uint8_t *a_data,
                   size_t b_length,
                   const uint8_t *b_data);
  void (*destroy) (BulpMerger *merger);
  BulpFormat *format;
  unsigned ref_count;
};
BulpComparator *bulp_comparator_ref   (BulpComparator *);
void            bulp_comparator_unref (BulpComparator *);


typedef struct {
  uint64_t key_offset;
  uint64_t compressed_blob_offset;
  uint32_t key_length;
  uint32_t compressed_blob_length;
} BulpIndexEntry;

#if BULP_IS_LITTLE_ENDIAN
#define bulp_index_entry_from_little_endian(e)
#define bulp_index_entry_to_little_endian(e)
#else
static inline void
bulp_index_entry_to_little_endian (BulpIndexEntry *ie)
{
  ie->key_offset = bulp_uint64_to_little_endian (ie->key_offset);
  ie->compressed_blob_offset = bulp_uint64_to_little_endian (ie->compressed_blob_offset);
  ie->key_length = bulp_uint32_to_little_endian (ie->key_length);
  ie->compressed_blob_length = bulp_uint32_to_little_endian (ie->compressed_blob_length);
}
#define bulp_index_entry_from_little_endian bulp_index_entry_to_little_endian
#endif
