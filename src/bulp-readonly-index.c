#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include "bulp.h"
#include "bulp-indexer-blob-cache.h"

typedef struct Stats Stats;
typedef struct IndexLevel IndexLevel;

struct Stats
{
  uint64_t n_entries;
  uint64_t n_blobs;
  uint64_t total_uncompressed_blob_size;
  uint64_t total_compressed_blob_size;
};

struct IndexLevel
{
  // IndexEntry index + count 
  uint64_t start;
  uint64_t size;
};

struct BulpReadonlyIndex
{
  uint64_t n_entries;
  uint64_t n_blobs;

  int heap_fd, index_fd, keys_fd;
  unsigned n_levels;
  IndexLevel *levels;
  BulpComparator *comparator;
  BulpCompressionAlgorithm compression_algorithm;
  BulpIndexerBlobCache *blob_cache;
  BulpSlab key_slab;
  BulpSlab compressed_blob_slab;
  BulpSlab uncompressed_slab;
};

// a read-only index that can be opened from a file or obtained from BulpSortedIndexer

BulpReadonlyIndex *
bulp_readonly_index_new (const char *filename_prefix,
                         BulpComparator *comparator,
                         BulpCompressionOptions *compression_options,
                         BulpError **error)
{
  char *buf;
  int index_fd, keys_fd, heap_fd;
  unsigned fplen = strlen (filename_prefix);
  buf = alloca (fplen + 10);
  memcpy (buf, filename_prefix, fplen);
  strcpy (buf + fplen, ".stats");
  size_t stats_len;
  uint8_t *stats_data;
  if (!bulp_util_read_file (buf, &stats_len, &stats_data, error))
    return BULP_FALSE;
  strcpy (buf + fplen, ".index");
do_open_index:
  index_fd = open (buf, O_RDONLY);
  if (index_fd < 0)
    {
      if (errno == EINTR)
        goto do_open_index;
      *error = bulp_error_new_file_read (errno);
      return BULP_FALSE;
    }
  strcpy (buf + fplen, ".keys");
do_open_keys:
  keys_fd = open (buf, O_RDONLY);
  if (keys_fd < 0)
    {
      if (errno == EINTR)
        goto do_open_keys;
      *error = bulp_error_new_file_read (errno);
      return BULP_FALSE;
    }
  strcpy (buf + fplen, ".heap");
do_open_heap:
  heap_fd = open (buf, O_RDONLY);
  if (heap_fd < 0)
    {
      if (errno == EINTR)
        goto do_open_heap;
      *error = bulp_error_new_file_read (errno);
      return BULP_FALSE;
    }
  BulpReadonlyIndex *rv;
  rv = malloc (sizeof (BulpReadonlyIndex));
  bulp_uint64_unpack (8, stats_data + 0, &rv->n_entries, NULL);
  bulp_uint64_unpack (8, stats_data + 8, &rv->n_blobs, NULL);

  unsigned n_levels = 0;
  while ((1ULL << n_levels) <= (rv->n_blobs + 1))
    n_levels++;

  uint64_t offset = 0;
  IndexLevel *levels = malloc (sizeof (IndexLevel) * n_levels);
  for (unsigned i = 0; i < n_levels; i++)
    {
      levels[i].start = offset;
      levels[i].size = (rv->n_entries >> i) - (rv->n_entries >> (i+1));
      offset += levels[i].size;
    }

  rv->levels = levels;
  rv->n_levels = n_levels;
  rv->comparator = bulp_comparator_ref (comparator);
  rv->compression_algorithm = compression_options->algorithm;
  return rv;
}

static bulp_bool
read_index_entry (BulpReadonlyIndex *index,
                  uint64_t           offset,
                  BulpIndexEntry    *ie,
                  BulpError        **error)
{
  if (!bulp_util_pread (index->index_fd, ie, sizeof (BulpIndexEntry), offset * sizeof (BulpIndexEntry), error))
    return BULP_FALSE;
  bulp_index_entry_from_little_endian (ie);
  return BULP_TRUE;
}

static bulp_bool
read_key (BulpReadonlyIndex *index,
          BulpIndexEntry    *ie,
          BulpError        **error)
{
  uint8_t *data = bulp_slab_set_size (&index->key_slab, ie->key_length);
  return bulp_util_pread (index->keys_fd, data, ie->key_length, ie->key_offset, error);
}

BulpReadonlyIndexLookupResult
bulp_readonly_index_lookup       (BulpReadonlyIndex       *index,
                                  size_t                   key_len,
                                  const uint8_t           *key,
                                  BulpSlab                *out,
                                  BulpError              **error)
{
  unsigned start = 0, count = index->n_blobs;
  unsigned level = index->n_levels - 1;
  BulpIndexEntry ie;
  while (count > 1)
    {
      uint64_t start_at_level = start >> level;
      uint64_t end_at_level = (start + count) >> level;
      uint64_t mid_at_level = (start_at_level + end_at_level) / 2;
      assert(start_at_level < end_at_level);
      if (!read_index_entry (index, index->levels[level].start + mid_at_level, &ie, error))
        {
          return BULP_READONLY_INDEX_LOOKUP_ERROR;
        }
      // read entry at mid_at_level
      if (!read_key (index, &ie, error))
        {
          return BULP_READONLY_INDEX_LOOKUP_ERROR;
        }
      //index->key_slab.length, index->key_slab.data
      int cmp_rv = index->comparator->compare(index->comparator, key_len, key, index->key_slab.length, index->key_slab.data);
      if (cmp_rv == 0)
        {
          start = mid_at_level;
          goto load_block_at_index_entry;
        }
      if (cmp_rv < 0)
        {
          uint64_t new_end = mid_at_level << level;
          count = new_end - start;
        }
      else
        {
          uint64_t new_start = (mid_at_level << level);
          uint64_t new_count = start + count - new_start;
          start = new_start;
          count = new_count;
        }
      if (level == 0)
        break;
      level--;
    }
  if (count == 0)
    return BULP_READONLY_INDEX_LOOKUP_NOT_FOUND;
  assert(count < 2);

  if (!read_index_entry (index,
                         index->levels[level].start + (start >> (level+1)),
                         &ie,
                         error))
    {
      return BULP_READONLY_INDEX_LOOKUP_ERROR;
    }

load_block_at_index_entry:

  {
    size_t uncomp_len;
    const uint8_t *uncomp_data;
    if (!bulp_indexer_blob_cache_get (index->blob_cache,
                                      ie.compressed_blob_offset, ie.compressed_blob_length,
                                      &uncomp_len, &uncomp_data))
      {
        // pread
        uint8_t *comp_data = bulp_slab_set_size (&index->compressed_blob_slab, ie.compressed_blob_length);
        if (!bulp_util_pread (index->heap_fd, comp_data, ie.compressed_blob_length, ie.compressed_blob_offset, error))
          return BULP_READONLY_INDEX_LOOKUP_ERROR;

        // uncompress
        if (!bulp_decompress (index->compression_algorithm,
                              ie.compressed_blob_length,
                              comp_data,
                              &index->uncompressed_slab,
                              error))
          return BULP_READONLY_INDEX_LOOKUP_ERROR;

        // add to cache
        uint8_t *copy = bulp_slab_memdup (&index->uncompressed_slab);
        bulp_indexer_blob_cache_set (index->blob_cache,
                                     ie.compressed_blob_offset, ie.compressed_blob_length,
                                     uncomp_len, copy);
        uncomp_data = copy;
      }
      
    // search through uncompressed blob 
    const uint8_t *uncomp_end = uncomp_data + uncomp_len;
    const uint8_t *at = uncomp_data;
    while (at < uncomp_end)
      {
        uint32_t entry_key_len, entry_value_len;
        unsigned len;
        
        len = bulp_uint_unpack (uncomp_end-at, at, &entry_key_len, error);
        if (len == 0)
          return BULP_READONLY_INDEX_LOOKUP_ERROR;
        at += len;

        len = bulp_uint_unpack (uncomp_end-at, at, &entry_value_len, error);
        if (len == 0)
          return BULP_READONLY_INDEX_LOOKUP_ERROR;
        at += len;

        const uint8_t *entry_key = at;
        at += entry_key_len + entry_value_len;

        int cmp_rv = index->comparator->compare(index->comparator, key_len, key, entry_key_len, entry_key);
        if (cmp_rv > 0)
          return BULP_READONLY_INDEX_LOOKUP_NOT_FOUND;
        if (cmp_rv == 0)
          {
            bulp_slab_set_data (out, entry_value_len, entry_key + entry_key_len);
            return BULP_READONLY_INDEX_LOOKUP_FOUND;
          }
      }
    return BULP_READONLY_INDEX_LOOKUP_NOT_FOUND;
  }
}

typedef struct BulpReadonlyIndexReaderClass BulpReadonlyIndexReaderClass;
struct BulpReadonlyIndexReaderClass
{
  BulpReaderClass base_class;
};
typedef struct BulpReadonlyIndexReader BulpReadonlyIndexReader;
struct BulpReadonlyIndexReader
{
  BulpReader base_instance;
  int index_fd;
  int heap_fd;
  uint64_t heap_offset;
  BulpCompressionAlgorithm compression_algorithm;
  BulpSlab compressed_blob_slab;
  BulpSlab uncompressed_slab;
  const uint8_t *uncompressed_data_at;
  size_t latest_length;
};

static void
bulp_readonly_index_reader_destroy (BulpObject *object)
{
  BulpReadonlyIndexReader *ireader = (BulpReadonlyIndexReader *) object;
  if (ireader->index_fd >= 0)
    close (ireader->index_fd);
  if (ireader->heap_fd >= 0)
    close (ireader->heap_fd);
  free (ireader);
}

static BulpReadResult
bulp_readonly_index_reader_peek (BulpReader *reader,
                                 size_t *data_length_out,
                                 const uint8_t **data_out,
                                 BulpError **error)
{
  BulpReadonlyIndexReader *ireader = (BulpReadonlyIndexReader *) reader;
  if (ireader->latest_length > 0)
    {
      *data_length_out = ireader->latest_length;
      *data_out = ireader->uncompressed_data_at - ireader->latest_length;
      return BULP_READ_RESULT_OK;
    }
  if (ireader->uncompressed_data_at == bulp_slab_get_end (&ireader->uncompressed_slab))
    {
      // read next blob
      BulpIndexEntry ie;
      switch (bulp_util_readn (ireader->index_fd, sizeof (ie), &ie, error))
        {
          case BULP_READ_RESULT_OK:
            bulp_index_entry_from_little_endian (&ie);
            break;

          case BULP_READ_RESULT_EOF:
            return BULP_READ_RESULT_EOF;

          case BULP_READ_RESULT_ERROR:
            return BULP_READ_RESULT_ERROR;
        }
            
      // assert positions align
      assert(ireader->heap_offset == ie.compressed_blob_offset);
      ireader->heap_offset += ie.compressed_blob_length;

      // read blob data
      uint8_t *comp_data = bulp_slab_set_size (&ireader->compressed_blob_slab, ie.compressed_blob_length);
      switch (bulp_util_readn (ireader->heap_fd, ie.compressed_blob_length, comp_data, error))
        {
          case BULP_READ_RESULT_OK:
            break;

          case BULP_READ_RESULT_EOF:
            *error = bulp_error_new_premature_eof (".heap file", "got eof despite .index entry");
            return BULP_READ_RESULT_ERROR;

          case BULP_READ_RESULT_ERROR:
            return BULP_READ_RESULT_ERROR;
        }

      // decompress
      if (!bulp_decompress (ireader->compression_algorithm, 
                            ie.compressed_blob_length, comp_data,
                            &ireader->uncompressed_slab, error))
       {
         return BULP_READ_RESULT_ERROR;
       }

      // reset iterator
      ireader->uncompressed_data_at = ireader->uncompressed_slab.data;
    }

  // key / value lengths
  uint32_t key_len, value_len;
  unsigned lenlen;
  const uint8_t *end = bulp_slab_get_end (&ireader->uncompressed_slab);
  const uint8_t *at = ireader->uncompressed_data_at;
  lenlen = bulp_uint_unpack (end - at, at, &key_len, error);
  if (lenlen == 0)
    return BULP_READ_RESULT_ERROR;
  at += lenlen;
  lenlen = bulp_uint_unpack (end - at, at, &value_len, error);
  if (lenlen == 0)
    return BULP_READ_RESULT_ERROR;
  at += lenlen;

  *data_length_out = ireader->latest_length = key_len + value_len;
  *data_out = at;
  ireader->uncompressed_data_at = at + key_len + value_len;
  return BULP_READ_RESULT_OK;
}

static BulpReadResult
bulp_readonly_index_reader_advance (BulpReader *reader,
                                    BulpError **error)
{
  BulpReadonlyIndexReader *ireader = (BulpReadonlyIndexReader *) reader;
  ireader->latest_length = 0;
  const uint8_t *data;
  size_t len;
  return bulp_readonly_index_reader_peek (reader, &len, &data, error);
}

static BulpReaderCloseResult
bulp_readonly_index_reader_close (BulpReader *reader,
                                  BulpError **error)
{
  BulpReadonlyIndexReader *ireader = (BulpReadonlyIndexReader *) reader;
  (void) error;
  if (ireader->heap_fd >= 0)
    {
      close (ireader->heap_fd);
      ireader->heap_fd = -1;
    }
  if (ireader->index_fd >= 0)
    {
      close (ireader->index_fd);
      ireader->index_fd = -1;
    }
  return BULP_READER_CLOSE_OK;
}

static BulpReadonlyIndexReaderClass index_reader_class = {
  { /* BulpReaderClass */
    { /* BulpObject */
      "BulpReadonlyIndexReader",
      (BulpClass *) &bulp_reader_class,
      BULP_CLASS_TYPE_READER,
      sizeof (BulpReadonlyIndexReaderClass),
      sizeof (BulpReadonlyIndexReader),
      bulp_readonly_index_reader_destroy
    },
    bulp_readonly_index_reader_peek,
    bulp_readonly_index_reader_advance,
    bulp_readonly_index_reader_close
  }
};

BulpReader *
bulp_readonly_index_reader_new (const char *filename_prefix,
                                BulpCompressionAlgorithm algo,
                                BulpFormat *format,
                                BulpError **error)
{
  unsigned fname_len = strlen (filename_prefix) + 10;
  char *filename = malloc (fname_len);
  int heap_fd, index_fd;

  snprintf(filename, fname_len, "%s.heap", filename_prefix);
open_heap_fd:
  heap_fd = open (filename, O_RDONLY);
  if (heap_fd < 0)
    { 
      if (errno == EINTR)
        goto open_heap_fd;
      *error = bulp_error_new_file_read (errno);
      return BULP_FALSE;
    }

  snprintf(filename, fname_len, "%s.index", filename_prefix);
open_index_fd:
  index_fd = open (filename, O_RDONLY);
  if (index_fd < 0)
    { 
      if (errno == EINTR)
        goto open_index_fd;
      *error = bulp_error_new_file_read (errno);
      return BULP_FALSE;
    }
  
  BulpReadonlyIndexReader *ireader = (BulpReadonlyIndexReader *) bulp_reader_new_protected ((BulpReaderClass *) &index_reader_class, format);
  ireader->heap_fd = heap_fd;
  ireader->index_fd = index_fd;
  ireader->compression_algorithm = algo;
  // These commented initializers are unnecessary with calloc()
  //ireader->heap_offset = 0;
  //bulp_slab_init (&ireader->compressed_blob_slab);
  //bulp_slab_init (&ireader->uncompressed_slab);
  //ireader->uncompressed_data_at = NULL;
  //ireader->latest_length = 0;
  //&ireader->base_instance.base_instance.bulp_class = (BulpClass *) &index_reader_class);
  return (BulpReader *) ireader;
}

void
bulp_readonly_index_destroy       (BulpReadonlyIndex *index)
{
  close (index->keys_fd);
  close (index->heap_fd);
  close (index->index_fd);
  free (index->levels);
  bulp_comparator_unref (index->comparator);
  bulp_indexer_blob_cache_destroy (index->blob_cache);
  bulp_slab_clear (&index->key_slab);
  bulp_slab_clear (&index->compressed_blob_slab);
  bulp_slab_clear (&index->uncompressed_slab);
  free (index);
}
