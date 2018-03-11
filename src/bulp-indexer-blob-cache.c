

BulpIndexerBlobCache *bulp_indexer_blob_cache_new (size_t                max_size);
bulp_bool             bulp_indexer_blob_cache_get (BulpIndexerBlobCache *cache,
                                                   uint64_t              blob_offset,
                                                   uint32_t              blob_length,
                                                   size_t               *uncomp_len_out,
                                                   const uint8_t       **uncomp_data_out);
    
/// takes ownership of uncomp_data, ie it'll free it
void                  bulp_indexer_blob_cache_set (BulpIndexerBlobCache *cache,
                                                   uint64_t              blob_offset,
                                                   uint32_t              blob_length,
                                                   size_t                uncomp_len,
                                                   uint8_t              *uncomp_data);
void                  bulp_indexer_blob_cache_destroy(BulpIndexerBlobCache *cache);
