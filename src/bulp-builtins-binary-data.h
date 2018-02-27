
BULP_INLINE size_t bulp_binary_data_get_packed_size (BulpBinaryData data);
BULP_INLINE size_t bulp_binary_data_pack            (BulpBinaryData data,
                                                     uint8_t *out);
BULP_INLINE size_t bulp_binary_data_pack_to         (uint8_t v,
                                                     BulpBuffer *out);
BULP_INLINE size_t bulp_binary_data_unpack          (size_t packed_len,
                                                     const uint8_t *packed_data,
                                                     BulpString *out,
                                                     BulpMemPool *mem_pool,
                                                     BulpError**error);

