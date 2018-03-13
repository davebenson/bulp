
typedef struct BulpSortedIndexer BulpSortedIndexer;

BulpSortedIndexer *
bulp_sorted_indexer_new          (const char *base_filename,
                                  BulpCompressionOptions *comp_options,
                                  BulpSlab *shared_compression_buffer,
                                  BulpFormat             *key_format,
                                  BulpFormat             *value_format,
                                  BulpError             **error);

bulp_bool
bulp_sorted_indexer_write        (BulpSortedIndexer *indexer,
                                  size_t             key_len,
                                  const uint8_t     *key,
                                  size_t             value_len,
                                  const uint8_t     *value,
                                  BulpError        **error);

typedef enum BulpSortedIndexerResultType {
  /* the amount of work was small enough that we did it synchronously */
  BULP_SORTED_INDEXER_RESULT_GOT_INDEX,

  /* the amount of work was too large: we did some work, but you must call
   * bulp_sorted_indexer_finish() again. */
  BULP_SORTED_INDEXER_RESULT_RUNNING,

  /* something failed while finishing the indexer. */
  BULP_SORTED_INDEXER_RESULT_ERROR
} BulpSortedIndexerResultType;


BulpSortedIndexerResultType
bulp_sorted_indexer_finish       (BulpSortedIndexer *indexer);


/* This should be called UNLESS finish() returns CONVERTED_TO_INDEX. */
void
bulp_sorted_indexer_destroy      (BulpSortedIndexer *indexer,
                                  bulp_bool          delete_files);

