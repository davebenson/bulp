#include "bulp.h"


// a read-only index that can be opened from a file or obtained from BulpSortedIndexer

BulpReadonlyIndex *
bulp_readonly_index_new (const char *filename_prefix,
                         BulpComparator *comparator,
                         BulpCompressionOptions *options)
{
  BulpFormat *format = ...;

  if (!bulp_util_read_file (...))
    {
      return BULP_FALSE;
    }
     

            
  ...
}

BulpReadonlyIndexLookupResult
bulp_readonly_index_lookup       (BulpReadonlyIndex *index,
                                  size_t                   key_len,
                                  const uint8_t           *key,
                                  BulpSlab                *out,
                                  BulpError              **error)
{
  ...
}

BulpReader *
bulp_readonly_index_create_reader (BulpReadonlyIndex *index)
{
  ...
}

BulpReader *
bulp_readonly_index_reader_new (const char *filename_prefix,
                                BulpCompressionOptions *options)
{
  ...
}

void
bulp_readonly_index_destroy       (BulpReadonlyIndex *index)
{
  ...
}
