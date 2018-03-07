
BULP_INLINE size_t bulp_binary_data_get_packed_size (BulpBinaryData data);
BULP_INLINE size_t bulp_binary_data_pack            (BulpBinaryData data,
                                                     uint8_t *out);
BULP_INLINE size_t bulp_binary_data_pack_to         (BulpBinaryData data,
                                                     BulpDataBuilder *out);
BULP_INLINE size_t bulp_binary_data_unpack          (size_t packed_len,
                                                     const uint8_t *packed_data,
                                                     BulpBinaryData *out,
                                                     BulpError**error);

#if BULP_CAN_INLINE || defined(BULP_INTERNAL_IMPLEMENT_INLINE_FUNCTIONS)
BULP_INLINE size_t bulp_binary_data_get_packed_size (BulpBinaryData data)
{
  return bulp_uint_get_packed_size (data.length) + data.length;
}
BULP_INLINE size_t bulp_binary_data_pack            (BulpBinaryData data,
                                                     uint8_t *out)
{ 
  unsigned lenlen = bulp_uint_pack (data.length, out);
  memcpy (out + lenlen, data.data, data.length);
  return lenlen + data.length;
}

BULP_INLINE size_t bulp_binary_data_pack_to         (BulpBinaryData data,
                                                     BulpDataBuilder *out)
{
  unsigned lenlen = bulp_uint_pack_to (data.length, out);
  bulp_data_builder_append_nocopy (out, data.length, data.data);
  return lenlen + data.length;
}
BULP_INLINE size_t bulp_binary_data_unpack          (size_t packed_len,
                                                     const uint8_t *packed_data,
                                                     BulpBinaryData *out,
                                                     BulpError**error)
{
  uint32_t len;
  uint32_t lenlen = bulp_uint_unpack (packed_len, packed_data, &len, error);
  if (lenlen == 0)
    return 0;
  if (lenlen + len > packed_len)
    {
      *error = bulp_error_new_too_short ("unpacking binary-data");
      return 0;
    }
  out->length = len;
  out->data = (uint8_t*) packed_data + lenlen;
  return len + lenlen;
}
  
#endif
