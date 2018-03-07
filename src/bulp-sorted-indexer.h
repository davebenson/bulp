
typedef struct BulpSortedIndexer BulpSortedIndexer;

BulpSortedIndexer *
bulp_sorted_indexer_new          (const char *base_filename,
                                  BulpCompressionOptions *comp_options);

bulp_bool
bulp_sorted_indexer_write        (BulpSortedIndexer *indexer,
                                  size_t             key_len,
                                  const uint8_t     *key,
                                  size_t             value_len,
                                  const uint8_t     *value,
                                  BulpError        **error);

typedef BulpSortedIndexerResultType {
  /* the amount of work was small enough that we did it synchronously */
  BULP_SORTED_INDEXER_RESULT_GOT_INDEX,

  /* the amount of work was too large: we did some work, but you must call
   * bulp_sorted_indexer_finish() again. */
  BULP_SORTED_INDEXER_RESULT_RETRY_FINISH,

  /* something failed while finishing the indexer. */
  BULP_SORTED_INDEXER_RESULT_ERROR
} BulpSortedIndexerResultType;

typedef struct {
  BulpSortedIndexerResultType result_type;
  union {
    struct {
      BulpSortedIndex *sorted_index;
    } got_index;
    struct {
      unsigned n_finishes       // number of times you have called "finish"
    } processing;
    BulpError *error;
  }
} BulpSortedIndexerResult

BulpSortedIndexerResult
bulp_sorted_indexer_finish       (BulpSortedIndexer *indexer);


void
bulp_sorted_indexer_destroy      (BulpSortedIndexer *indexer,
                                  bulp_bool          delete_files);

