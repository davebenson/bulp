BULP_INLINE size_t bulp_float32_get_packed_size (float v);
BULP_INLINE size_t bulp_float32_pack            (float v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_float32_pack_to         (float v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_float32_unpack          (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 float *out,
                                                 BulpError**error);

BULP_INLINE size_t bulp_float64_get_packed_size (double v);
BULP_INLINE size_t bulp_float64_pack            (double v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_float64_pack_to         (double v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_float64_unpack          (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 double *out,
                                                 BulpError**error);

#if BULP_CAN_INLINE || defined(BULP_INTERNAL_IMPLEMENT_INLINE_FUNCTIONS)
BULP_INLINE size_t
bulp_float32_get_packed_size (float v)
{
  (void) v;
  return 4;
}
BULP_INLINE size_t
bulp_float32_pack (float v, uint8_t *out)
{
#if BULP_IS_LITTLE_ENDIAN
  memcpy (out, native_data, 4);
#else
  union { uint32_t i; float f; } u;
  u.f = v;
  out[0] = u.i;
  out[1] = u.i >> 8;
  out[2] = u.i >> 16;
  out[3] = u.i >> 24;
#endif
  return 4;
}
BULP_INLINE size_t
bulp_float32_pack_to (float v, BulpBuffer *b)
{
#if BULP_IS_LITTLE_ENDIAN
  bulp_buffer_append_small (b, 4, &v);
#else
  uint8_t out[4];
  bulp_float32_pack (v, out);
  bulp_buffer_append_small (b, 4, out);
#endif
  return 4;
}
BULP_INLINE size_t
bulp_float32_unpack           (size_t packed_len,
                               const uint8_t *packed_data,
                               float *out,
                               BulpError**error)
{
  if (packed_len < 4)
    {
      *error = bulp_error_new_too_short ("packed float32 needs 4 bytes, got %u", (unsigned) packed_len);
      return 0;
    }
#if BULP_IS_LITTLE_ENDIAN
  memcpy (out, packed_data, 4);
#else
  union { uint32_t i; float f; } u;
  if (bulp_uint32_unpack (packed_len, packed_data, &u.i, error) == 0)
    return 0;
  *out = u.f;
#endif
  return 4;
}



BULP_INLINE size_t
bulp_float64_get_packed_size (double v)
{
  (void) v;
  return 8;
}
BULP_INLINE size_t
bulp_float64_pack (double v, uint8_t *out)
{
#if BULP_IS_LITTLE_ENDIAN
  memcpy (out, &v, 8);
#else
  union { uint64_t i; double f; } u;
  u.f = v;
  bulp_uint64_pack (u.i, out);
#endif
  return 8;
}
BULP_INLINE size_t
bulp_float64_pack_to (double v, BulpBuffer *b)
{
#if BULP_IS_LITTLE_ENDIAN
  bulp_buffer_append_small (b, 8, &v);
#else
  uint8_t out[8];
  bulp_float64_pack (v, out);
  bulp_buffer_append_small (b, 8, out);
#endif
  return 8;
}
BULP_INLINE size_t
bulp_float64_unpack           (size_t packed_len,
                               const uint8_t *packed_data,
                               double *out,
                               BulpError**error)
{
  if (packed_len < 8)
    {
      *error = bulp_error_new_too_short ("packed float64 needs 4 bytes, got %u", (unsigned) packed_len);
      return 0;
    }
#if BULP_IS_LITTLE_ENDIAN
  memcpy (out, packed_data, 8);
#else
  union { uint64_t i; double f; } u;
  if (bulp_uint64_unpack (packed_len, packed_data, &u.i, error) == 0)
    return 0;
  *out = u.f;
#endif
  return 8;
}
#endif
