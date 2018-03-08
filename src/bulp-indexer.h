
struct BulpIndexerOptions
{
  BulpCompressionOptions compression_options;

  BulpComparator *comparator;
  BulpMerger *merger;
  BulpFormat *key_value_format;                 // if not set, it'll be an anonymous struct of key+value

  const char *prefix;
};


BulpIndexer *
bulp_indexer_new (const char *filename_prefix,
                  BulpIndexerOptions *options);

/* note: adding to an indexer does NOT guarantee
 * that the data will persist in the event of
 * a power/machine failure.
 */
bulp_bool
bulp_indexer_add   (BulpIndexer   *indexer,
                    size_t         key_length,
                    const uint8_t *key_data,
                    size_t         value_length,
                    const uint8_t *value_data,
                    BulpError    **error);

bulp_bool
bulp_indexer_add_kv(BulpIndexer   *indexer,
                    size_t         length,
                    const uint8_t *data,
                    BulpError    **error);

BulpIndexerQueryResult
bulp_indexer_query (BulpIndexer   *indexer,
                    size_t         key_length,
                    const uint8_t *key_data,
                    BulpSlab      *value_out,
                    BulpError    **error);



BulpReader *
bulp_indexer_create_reader (BulpIndexer   *indexer,
                            BulpError    **error);


void bulp_indexer_destroy (BulpIndexer *indexer);
void bulp_indexer_destroy_erase (BulpIndexer *indexer);


/* rollback_millis is the number of milliseconds to allow
 * before merging the transaction.
 *
 * rollback requirements can severely inhibit normal database
 * operations, so they should be minimally.  (The intended use-case
 * is distributed transactions, where any system failing should
 * lead to a rollback of all other systems)
 */
BulpIndexerTransaction *
bulp_indexer_create_transaction (BulpIndexer   *indexer,
                                 BulpError    **error);

BulpWriter *
bulp_indexer_transaction_get_writer (BulpIndexerTransaction *transaction);

/* After committing, returns from the indexer will include the transaction.
 * But the transaction can be rolled-back until either of the destroy
 * functions is called. */
void
bulp_indexer_transaction_commit (BulpIndexerTransaction *);

void
bulp_indexer_transaction_rollback_destroy (BulpIndexerTransaction *);

void
bulp_indexer_transaction_destroy (BulpIndexerTransaction *transaction);
