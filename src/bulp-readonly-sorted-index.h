
// a read-only index that can be opened from a file or obtained from BulpSortedIndexer

typedef enum {
  BULP_READONLY_SORTED_INDEX_LOOKUP_FOUND,
  BULP_READONLY_SORTED_INDEX_LOOKUP_NOT_FOUND,
  BULP_READONLY_SORTED_INDEX_LOOKUP_ERROR
} BulpReadonlySortedIndexLookupResult;

BulpReadonlySortedIndexLookupResult
bulp_readonly_sorted_index_lookup        (BulpReadonlySortedIndex *index,
                                          size_t                   key_len,
                                          const uint8_t           *key,
                                          BulpComparatorFunc       comparator_func,
                                          void                    *comparator_data,
                                          BulpSlab                *out,
                                          BulpError              **error);

BulpReader *
bulp_readonly_sorted_index_create_reader (BulpReadonlySortedIndex *index);

void
bulp_readonly_sorted_index_destroy       (BulpReadonlySortedIndex *index);
