
typedef struct BulpReadonlyIndex BulpReadonlyIndex;

// a read-only index that can be opened from a file or obtained from BulpSortedIndexer

typedef enum {
  BULP_READONLY_INDEX_LOOKUP_FOUND,
  BULP_READONLY_INDEX_LOOKUP_NOT_FOUND,
  BULP_READONLY_INDEX_LOOKUP_ERROR
} BulpReadonlyIndexLookupResult;

BulpReadonlyIndex *
bulp_readonly_index_new          (const char *filename_prefix,
                                  BulpComparator *comparator,
                                  BulpCompressionOptions *options);

BulpReadonlyIndexLookupResult
bulp_readonly_index_lookup       (BulpReadonlyIndex    *index,
                                  size_t                key_len,
                                  const uint8_t        *key,
                                  BulpSlab             *out,
                                  BulpError           **error);

BulpReader *
bulp_readonly_index_create_reader(BulpReadonlyIndex    *index);

BulpReader *
bulp_readonly_index_reader_new   (const char           *filename_prefix,
                                  BulpCompressionOptions *options);

void
bulp_readonly_index_destroy      (BulpReadonlyIndex    *index);
