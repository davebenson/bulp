
#define FILENAME_EXTRA_SIZE       16

struct JournalHeader
{
  uint32_t magic;
  uint32_t n_pot_files;                 // index + key files

  uint64_t n_entries;
  uint64_t n_blobs;
  uint64_t total_key_bytes;
  uint64_t total_value_bytes;

  // length-prefixed journal data follows
};

struct BulpSortedIndexerIndexLevel
{
  int index_fd;
  int heap_fd;
};

struct BulpSortedIndexer
{
  uint64_t n_entries;
  uint64_t n_blobs;
  uint64_t total_blob_size;
  uint64_t total_key_bytes;
  uint64_t total_value_bytes;

  unsigned n_levels;
  BulpSortedIndexerIndexLevel *levels;

  /* basename + short (constant-length) suffix */
  char *filename_pad;
  unsigned base_filename_len;

  int heap_fd;
  BulpCompressionOptions compression_options;
  
  size_t pre_compressed_max_count;
  size_t pre_compressed_max_size;
  size_t pre_compressed_count;
  size_t pre_compressed_alloced;
  size_t pre_compressed_size;
  uint8_t *pre_compressed_data;

  BulpSlab *compressed_buffer;
};


BulpSortedIndexer *
bulp_sorted_indexer_new        (const char *base_filename,
                                BulpCompressionOptions *comp_options,
                                BulpError **error)
{
  BulpSortedIndexer *rv = malloc (sizeof (BulpSortedIndexer));
  rv->base_filename_len = strlen (base_filename);
  rv->filename_pad = malloc (rv->base_filename_len + FILENAME_EXTRA_SIZE);
  strcpy (rv->filename_pad, base_filename);


open_stats:
  int heap_fd = open (set_filename (rv, ".heap"), O_RDWR | O_EXIST | O_CREAT | O_TRUNC);
  if (heap_fd < 0)
    {
      if (errno == EINTR)
        goto create_heap;
      *error = bulp_error_new_file_open_error (rv->filename_pad, errno);
      free (rv->filename_pad);
      free (rv);
      return NULL;
    }

  rv->n_entries = 0;
  rv->n_blobs = 0;
  rv->total_key_bytes = 0;
  rv->total_value_bytes = 0;
      
  rv->compression_options = *comp_options;
  rv->pre_compressed_max_count = comp_options->records_per_chunk;
  rv->pre_compressed_max_size = 4096;
  rv->pre_compressed_count = 0;
  rv->pre_compressed_alloced = 2048;
  rv->pre_compressed_size = 0;
  rv->pre_compressed_data = malloc (sizeof (rv->pre_compressed_alloced);
  bulp_slab_init (&rv->compressed_buffer);

  return rv;
}

static bulp_bool
flush_precompressed_data (BulpSortedIndexer *indexer)
{
  bulp_compress (&indexer->compression_options,
                 indexer->pre_compressed_size, indexer->pre_compressed_data, 
                 indexer->compressed_buffer);

  // write compressed blob and note its location
  size_t n_written = 0;
  while (n_written < indexer->compressed_buffer.length)
    {
      ssize_t write_rv = write (indexer->heap_fd,
                                indexer->compressed_buffer.length - n_written,
                                indexer->compressed_buffer.data + n_written);
      if (write_rv < 0)
        {
          if (errno == EINTR)
            continue;
          *error = bulp_error_new_file_write (errno);
          return BULP_FALSE;
        }
      n_written += write_rv;
    }

  // write key + index according to the log2 of the largest power of two that divides index+1.
  unsigned which_level = n_least_signif_zeroes (rv->n_entries + 1);
  if (which_level >= indexer->n_levels)
    {
      ... create new pot files (index + heap)
    }

  // write to index file (key location and compressed-blob location)
  index_entry.key_offset = level->key_heap_size;
  index_entry.key_length = indexer->first_key_length;
  index_entry.compressed_blob_offset = indexer->total_blob_size;
  index_entry.compressed_blob_length = indexer->compressed_buffer->length;
  make_index_entry_big_endian (&index_entry);
  if (!writen (level->index_fd, &index_entry, sizeof (IndexEntry), &error))
    {
      bulp_error_append_suffix (*error, " (writing index-entry at level %u)", which_level);
      return BULP_FALSE;
    }
  if (!writen (level->key_fd, indexer->pre_compressed_data, indexer->first_key_length, &error))
    return BULP_FALSE;
  level->key_heap_size += indexer->first_key_length;
  level->n_blobs += 1;
  return BULP_TRUE;
}

bulp_bool
bulp_sorted_indexer_write      (BulpSortedIndexer *indexer,
                                size_t             key_len,
                                const uint8_t     *key,
                                size_t             value_len,
                                const uint8_t     *value,
                                BulpError        **error)
{
  uint8_t key_len_buf[10];
  unsigned key_len_len = bulp_uint_pack (key_len, key_len_buf);
  uint8_t value_len_buf[10];
  unsigned value_len_len = bulp_uint_pack (value_len, value_len_buf);
  size_t entry_size = key_len_len + key_len + value_len_len + value_len;
  size_t size = indexer->pre_compressed_size;
  if (size + entry_size > indexer->pre_compressed_alloced)
    {
      size_t new_alloced = indexer->pre_compressed_alloced * 2;
      while (size + entry_size > new_alloced)
        new_alloced += new_alloced;
      indexer->pre_compressed_data = realloc (indexer->pre_compressed_data, new_alloced);
      indexer->pre_compressed_alloced = new_alloced;
    }
  uint8_t *at = indexer->pre_compressed_data + size;
  memcpy (at, key_len_buf, key_len_len);
  at += key_len_len;
  memcpy (at, value_len_buf, value_len_len);
  at += value_len_len;
  memcpy (at, key, key_len);
  at += key_len;
  memcpy (at, value, value_len);

  if (indexer->pre_compressed_count == 0)
    indexer->first_key_length = key_len;
  indexer->pre_compressed_data += entry_size;
  indexer->pre_compressed_count += 1;

  if (indexer->pre_compressed_size > indexer->pre_compressed_max_size
   || indexer->pre_compressed_count > indexer->pre_compressed_max_count)
    {
      if (!flush_precompressed_data (indexer, error))
        return BULP_FALSE;
    }
  return BULP_TRUE;
}


void
bulp_sorted_indexer_finish (BulpSortedIndexer *indexer,
                            BulpSortedIndexerResult *out)
{
  ...
}---
