
/* ascii string
   ascii0 string0 */

BULP_INLINE size_t bulp_ascii_validate          (BulpString str,
                                                 BulpError **error);
BULP_INLINE size_t bulp_ascii_get_packed_size   (BulpString str);
BULP_INLINE size_t bulp_ascii_pack              (BulpString str,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_ascii_pack_to           (uint8_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_ascii_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpMemPool *mem_pool,
                                                 BulpError**error);

BULP_INLINE size_t bulp_string_validate         (BulpString str,
                                                 BulpError **error);
BULP_INLINE size_t bulp_string_get_packed_size  (BulpString str);
BULP_INLINE size_t bulp_string_pack             (BulpString str,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_string_pack_to          (BulpString str,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_string_unpack           (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpMemPool *mem_pool,
                                                 BulpError**error);

BULP_INLINE size_t bulp_ascii0_validate         (BulpString str,
                                                 BulpError **error);
BULP_INLINE size_t bulp_ascii0_get_packed_size  (BulpString str);
BULP_INLINE size_t bulp_ascii0_pack             (BulpString str,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_ascii0_pack_to          (uint8_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_ascii0_unpack           (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpMemPool *mem_pool,
                                                 BulpError**error);

BULP_INLINE size_t bulp_string0_validate        (BulpString str,
                                                 BulpError **error);
BULP_INLINE size_t bulp_string0_get_packed_size (BulpString str);
BULP_INLINE size_t bulp_string0_pack            (BulpString str,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_string0_pack_to         (BulpString str,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_string0_unpack          (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpMemPool *mem_pool,
                                                 BulpError**error);
