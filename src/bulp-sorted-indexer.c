#include "bulp.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "bulp-helper-process.h"

#if BULP_IS_LITTLE_ENDIAN
#define make_index_entry_little_endian(ie)
#else
static inline uint32_t bulp_swap_if_be_uint32(uint32_t v)
{
  union {
    uint8_t b[4];
    uint32_t rv;
  } u;
  bulp_uint32_pack (v, u.b);
  return u.rv;
}
static inline uint64_t bulp_swap_if_be_uint64(uint64_t v)
{
  union {
    uint8_t b[8];
    uint64_t rv;
  } u;
  bulp_uint64_pack (v, u.b);
  return u.rv;
}
static inline void make_index_entry_little_endian(IndexEntry *ie)
{
  ie->key_length = bulp_swap_if_be_uint32 (ie->key_length);
  ie->compressed_blob_length = bulp_swap_if_be_uint32 (ie->compressed_blob_length);
  ie->key_offset = bulp_swap_if_be_uint64 (ie->key_length);
  ie->compressed_blob_offset = bulp_swap_if_be_uint64 (ie->compressed_blob_length);
}
#endif

#define FILENAME_EXTRA_SIZE       16

typedef enum
{
  FINISH_NOT_STARTED,
  FINISH_RUNNING,
  FINISH_DONE,
  FINISH_FAILED,
  FINISH_DESTROYED_WHILE_RUNNING,
  FINISH_DESTROYED
} FinishStatus;

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

typedef struct BulpSortedIndexerIndexLevel
{
  int index_fd;
  int key_heap_fd;
  uint64_t key_heap_size;
} BulpSortedIndexerIndexLevel;

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
  
  size_t first_key_length;
  size_t pre_compressed_max_count;
  size_t pre_compressed_max_size;
  size_t pre_compressed_count;
  size_t pre_compressed_alloced;
  size_t pre_compressed_size;
  uint8_t *pre_compressed_data;

  BulpFormat *key_format;
  BulpFormat *value_format;

  BulpSlab *compressed_data;

  FinishStatus finish_status;
  unsigned finish_level;
  uint64_t finish_level_at;
  BulpError *finish_error;
};

static const char *set_filename (BulpSortedIndexer *indexer, const char *format, ...) BULP_PRINTF_LIKE(2,3);

static const char *set_filename (BulpSortedIndexer *indexer, const char *format, ...)
{
  va_list args;
  va_start (args, format);
  vsnprintf (indexer->filename_pad + indexer->base_filename_len,
             FILENAME_EXTRA_SIZE,
             format,
             args);
  va_end (args);
  return indexer->filename_pad;
}
       
static unsigned
n_least_signif_zeroes (uint32_t i)
{
  unsigned rv;
  for (rv = 0; rv < 32; rv++)
    if ((i & (1 << rv)) != 0)
      return rv;
  return rv;
}


static void
close_all_fds (BulpSortedIndexer *indexer)
{
  close (indexer->heap_fd);
  for (unsigned i = 0; i < indexer->n_levels; i++)
    {
      close (indexer->levels[i].index_fd);
      close (indexer->levels[i].key_heap_fd);
    }
}

BulpSortedIndexer *
bulp_sorted_indexer_new        (const char *base_filename,
                                BulpCompressionOptions *comp_options,
                                BulpSlab *shared_compression_buffer,
                                BulpFormat             *key_format,
                                BulpFormat             *value_format,
                                BulpError **error)
{
  int heap_fd;
  BulpSortedIndexer *rv = malloc (sizeof (BulpSortedIndexer));
  rv->base_filename_len = strlen (base_filename);
  rv->filename_pad = malloc (rv->base_filename_len + FILENAME_EXTRA_SIZE);
  strcpy (rv->filename_pad, base_filename);
  rv->finish_status = FINISH_NOT_STARTED;

open_stats:
  heap_fd = open (set_filename (rv, ".heap"), O_RDWR | O_CREAT | O_TRUNC);
  if (heap_fd < 0)
    {
      if (errno == EINTR)
        goto open_stats;
      *error = bulp_error_new_file_open_error (rv->filename_pad, errno);
      free (rv->filename_pad);
      free (rv);
      return NULL;
    }

  rv->n_entries = 0;
  rv->n_blobs = 0;
  rv->total_key_bytes = 0;
  rv->total_value_bytes = 0;
  rv->key_format = bulp_format_ref (key_format);
  rv->value_format = bulp_format_ref (value_format);
      
  rv->compression_options = *comp_options;
  rv->pre_compressed_max_count = comp_options->records_per_chunk;
  rv->pre_compressed_max_size = 4096;
  rv->pre_compressed_count = 0;
  rv->pre_compressed_alloced = 2048;
  rv->pre_compressed_size = 0;
  rv->pre_compressed_data = malloc (sizeof (rv->pre_compressed_alloced));
  rv->compressed_data = shared_compression_buffer;

  return rv;
}

static bulp_bool
flush_precompressed_data (BulpSortedIndexer *indexer,
                          BulpError        **error)
{
  bulp_compress (&indexer->compression_options,
                 indexer->pre_compressed_size, indexer->pre_compressed_data, 
                 indexer->compressed_data);

  // write compressed blob and note its location
  size_t n_written = 0;
  BulpSlab *compressed_data = indexer->compressed_data;
  while (n_written < compressed_data->length)
    {
      ssize_t write_rv = write (indexer->heap_fd,
                                compressed_data->data + n_written,
                                compressed_data->length - n_written);
      if (write_rv < 0)
        {
          if (errno == EINTR)
            continue;
          *error = bulp_error_new_file_write (errno);
          return BULP_FALSE;
        }
      n_written += write_rv;
    }
  compressed_data->length = 0;

  // write key + index according to the log2 of the largest power of two that divides index+1.
  unsigned which_level = n_least_signif_zeroes (indexer->n_entries + 1);
  if (which_level >= indexer->n_levels)
    {
      BulpSortedIndexerIndexLevel level;
      level.key_heap_size = 0;
open_index_file:
      level.index_fd = open(set_filename(indexer, ".index.%02u", which_level),
                            O_TRUNC | O_CREAT | O_WRONLY, 0666);
      if (level.index_fd < 0)
        {
          if (errno == EINTR)
            goto open_index_file;
          int e = errno;
          *error = bulp_error_new_file_write (e);
          bulp_error_append_message (*error,
                                    " (creating %s)",
                                    indexer->filename_pad);
          return BULP_FALSE;
        }
open_key_heap_file:
      level.key_heap_fd = open(set_filename(indexer, ".keys.%02u", which_level),
                            O_TRUNC | O_CREAT | O_WRONLY, 0666);
      if (level.key_heap_fd < 0)
        {
          if (errno == EINTR)
            goto open_key_heap_file;
          int e = errno;
          close (level.index_fd);
          *error = bulp_error_new_file_write (e);
          bulp_error_append_message (*error,
                                    " (creating %s)",
                                    indexer->filename_pad);
          return BULP_FALSE;
        }

      indexer->levels = realloc (indexer->levels, sizeof (BulpSortedIndexerIndexLevel) * (indexer->n_levels + 1));
      indexer->levels[indexer->n_levels] = level;
      indexer->n_levels++;
    }

  BulpSortedIndexerIndexLevel *level = indexer->levels + which_level;

  // write to index file (key location and compressed-blob location)
  BulpIndexEntry index_entry;
  index_entry.key_offset = level->key_heap_size;
  index_entry.key_length = indexer->first_key_length;
  index_entry.compressed_blob_offset = indexer->total_blob_size;
  index_entry.compressed_blob_length = indexer->compressed_data->length;
  make_index_entry_little_endian (&index_entry);
  if (!bulp_util_writen (level->index_fd, sizeof (BulpIndexEntry), &index_entry, error))
    {
      bulp_error_append_message (*error, " (writing index-entry at level %u)", which_level);
      return BULP_FALSE;
    }
  if (!bulp_util_writen (level->key_heap_fd, indexer->first_key_length, indexer->pre_compressed_data, error))
    return BULP_FALSE;
  level->key_heap_size += indexer->first_key_length;
  indexer->n_blobs += 1;
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

static void
handle_concat_done    (BulpHelperProcessResult result,
                       BulpError              *error,
                       void                   *callback_data)
{
  BulpSortedIndexer *indexer = callback_data;
  switch (result)
    {
    case BULP_HELPER_PROCESS_SUCCESS:
      if (indexer->finish_status == FINISH_DESTROYED_WHILE_RUNNING)
        {
          close_all_fds (indexer);
          indexer->finish_status = FINISH_DESTROYED;
          free (indexer->levels);
          free (indexer->pre_compressed_data);
          free (indexer);
        }
      else
        {
          indexer->finish_status = FINISH_DONE;
        }
      return;
    case BULP_HELPER_PROCESS_ERROR:
      indexer->finish_status = FINISH_FAILED;
      indexer->finish_error = bulp_error_ref (error);
      return;
    case BULP_HELPER_PROCESS_CANCELLED:
      return;
    }
}

BulpSortedIndexerResult
bulp_sorted_indexer_finish (BulpSortedIndexer *indexer)
{
  BulpSortedIndexerResult rv;

  assert(indexer->finish_status == FINISH_NOT_STARTED);

  /* close all file-descriptors */
  close_all_fds (indexer);

  /* is the total amount of data manageable?
   * if so, write a placeholder, write the data, delete placeholder.
   * if otherwise allocate a Job to do the work piecemeal.
   */
  assert(indexer->finish_status == FINISH_RUNNING);
  if (indexer->n_levels < 4)
    {
      BulpError *error = NULL;
      if (!bulp_helper_foreground_concat_sorted_levels
                      (NULL,
                       indexer->filename_pad,
                       indexer->n_levels,
                       &error))
        {
          rv.result_type = BULP_SORTED_INDEXER_RESULT_ERROR;
          rv.info.error = error;
          indexer->finish_status = FINISH_FAILED;
        }
      else
        {
          rv.result_type = BULP_SORTED_INDEXER_RESULT_GOT_INDEX;
          indexer->finish_status = FINISH_DONE;
        }
    }
  else
    {
      indexer->filename_pad[indexer->base_filename_len] = 0;
      bulp_helper_process_concat_sorted_levels (NULL,
          indexer->filename_pad, indexer->n_levels,
          handle_concat_done,
          indexer);
      rv.result_type = BULP_SORTED_INDEXER_RESULT_RUNNING;
    }
  return rv;
}

static void
handle_deletion_done (BulpHelperProcessResult result,
                      BulpError              *error,       // usually NULL
                      void                   *callback_data)
{
  BulpSortedIndexer *indexer = callback_data;
  (void) result;
  if (error != NULL)
    {
      indexer->finish_status = FINISH_FAILED;
    }
  else
    {
      indexer->finish_status = FINISH_DONE;
    }
  free (indexer->levels);
  free (indexer->pre_compressed_data);
  free (indexer);
}

void
bulp_sorted_indexer_destroy (BulpSortedIndexer *indexer,
                             bulp_bool          delete_files)
{
  (void) delete_files;  // XXX todo
  if (indexer->finish_status == FINISH_RUNNING)
    {
      indexer->finish_status = FINISH_DESTROYED_WHILE_RUNNING;
      return;
    }
  else if (indexer->finish_status == FINISH_NOT_STARTED)
    {
      close_all_fds (indexer);
      indexer->filename_pad[indexer->base_filename_len] = 0;
      bulp_helper_process_delete_sorted_index (NULL, indexer->filename_pad, indexer->n_levels,
                                               handle_deletion_done, indexer);
    }
  else
    {
      indexer->finish_status = FINISH_DESTROYED;
      free (indexer->levels);
      free (indexer->pre_compressed_data);
      free (indexer);
    }
}
