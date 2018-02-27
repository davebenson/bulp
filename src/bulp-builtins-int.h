/* === Fixed Length, Unsigned Integers (uint8, uint16, uint32, uint64) === */
BULP_INLINE size_t bulp_uint8_get_packed_size   (uint8_t v);
BULP_INLINE size_t bulp_uint8_pack              (uint8_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_uint8_pack_to           (uint8_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_uint8_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 uint8_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_uint16_get_packed_size  (uint16_t v);
BULP_INLINE size_t bulp_uint16_pack             (uint16_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_uint16_pack_to          (uint16_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_uint16_unpack           (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 uint16_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_uint32_get_packed_size  (uint32_t v);
BULP_INLINE size_t bulp_uint32_pack             (uint32_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_uint32_pack_to          (uint32_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_uint32_unpack           (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 uint32_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_uint64_get_packed_size  (uint64_t v);
BULP_INLINE size_t bulp_uint64_pack             (uint64_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_uint64_pack_to          (uint64_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_uint64_unpack           (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 uint64_t *out,
                                                 BulpError**error);

/* === Fixed Length, Signed Integers (int8, int16, int32, int64) === */
BULP_INLINE size_t bulp_int8_get_packed_size    (int8_t v);
BULP_INLINE size_t bulp_int8_pack               (int8_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_int8_pack_to            (int8_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_int8_unpack             (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int8_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_int16_get_packed_size   (int16_t v);
BULP_INLINE size_t bulp_int16_pack              (int16_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_int16_pack_to           (int16_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_int16_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int16_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_int32_get_packed_size   (int32_t v);
BULP_INLINE size_t bulp_int32_pack              (int32_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_int32_pack_to           (int32_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_int32_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int32_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_int64_get_packed_size   (int64_t v);
BULP_INLINE size_t bulp_int64_pack              (int64_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_int64_pack_to           (int64_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_int64_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int64_t *out,
                                                 BulpError**error);

/* === Variable Length, Unsigned Integers (ushort, uint, ulong) === */
BULP_INLINE size_t bulp_ushort_get_packed_size  (uint16_t v);
BULP_INLINE size_t bulp_ushort_pack             (uint16_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_ushort_pack_to          (uint16_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_ushort_unpack           (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 uint16_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_uint_get_packed_size    (uint32_t v);
BULP_INLINE size_t bulp_uint_pack               (uint32_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_uint_pack_to            (uint32_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_uint_unpack             (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 uint32_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_ulong_get_packed_size   (uint64_t v);
BULP_INLINE size_t bulp_ulong_pack              (uint64_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_ulong_pack_to           (uint64_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_ulong_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 uint64_t *out,
                                                 BulpError**error);

/* === Variable Length, Signed Integers (ushort, uint, ulong) === */
BULP_INLINE uint16_t bulp_zigzag16   (int16_t v);
BULP_INLINE int16_t  bulp_unzigzag16 (uint16_t v);
BULP_INLINE uint32_t bulp_zigzag32   (int32_t v);
BULP_INLINE int32_t  bulp_unzigzag32 (uint32_t v);
BULP_INLINE uint64_t bulp_zigzag64   (int64_t v);
BULP_INLINE int64_t  bulp_unzigzag64 (uint64_t v);

BULP_INLINE size_t bulp_short_get_packed_size   (int16_t v);
BULP_INLINE size_t bulp_short_pack              (int16_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_short_pack_to           (int16_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_short_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int16_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_int_get_packed_size     (int32_t v);
BULP_INLINE size_t bulp_int_pack                (int32_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_int_pack_to             (int32_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_int_unpack              (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int32_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_long_get_packed_size    (int64_t v);
BULP_INLINE size_t bulp_long_pack               (int64_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_long_pack_to            (int64_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_long_unpack             (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int64_t *out,
                                                 BulpError**error);

#if BULP_CAN_INLINE || defined(BULP_INTERNAL_IMPLEMENT_INLINE_FUNCTIONS)
BULP_INLINE size_t
bulp_uint8_get_packed_size   (uint8_t v)
{
  (void) v;
  return 1;
}
BULP_INLINE size_t
bulp_uint8_pack              (uint8_t v,
                              uint8_t *out)
{
  out[0] = v;
  return 1;
}
BULP_INLINE size_t
bulp_uint8_pack_to           (uint8_t v,
                              BulpBuffer *out)
{
  bulp_buffer_append_byte (out, v);
  return 1;
}
BULP_INLINE size_t
bulp_uint8_unpack            (size_t packed_len,
                              const uint8_t *packed_data,
                              uint8_t *out,
                              BulpError**error)
{
  if (packed_len < 1)
    {
      *error = bulp_error_new_too_short ("unpacking uint8");
      return 0;
    }
  * (uint8_t *) out = packed_data[0];
  return 1;
}

BULP_INLINE size_t
bulp_uint16_get_packed_size  (uint16_t v)
{
  (void) v; return 2;
}

BULP_INLINE size_t
bulp_uint16_pack             (uint16_t v,
                              uint8_t *out)
{
#if BULP_IS_LITTLE_ENDIAN
  memcpy (out, &v, 2);
#else
  out[0] = v;
  out[1] = v >> 8;
#endif
  return 2;
}
BULP_INLINE size_t
bulp_uint16_pack_to          (uint16_t v,
                              BulpBuffer *out)
{
#if BULP_IS_LITTLE_ENDIAN
  bulp_buffer_append_small (out, 2, &v);
#else
  uint8_t buf[2] = { v, v >> 8 };
  bulp_buffer_append_small (out, 2, buf);
#endif
  return 2;
}
BULP_INLINE size_t
bulp_uint16_unpack           (size_t packed_len,
                              const uint8_t *packed_data,
                              uint16_t *out,
                              BulpError**error)
{
  if (packed_len < 2)
    {
      *error = bulp_error_new_too_short ("unpacking uint16");
      return 0;
    }
#if BULP_IS_LITTLE_ENDIAN
  memcpy (out, packed_data, 2);
#else
  uint16_t v = ((uint16_t) packed_data[0])
             | ((uint16_t) packed_data[1] << 8);
  * out = v;
#endif
  return 2;
}
BULP_INLINE size_t
bulp_uint32_get_packed_size  (uint32_t v)
{
  (void) v;
  return 4;
}
BULP_INLINE size_t
bulp_uint32_pack             (uint32_t v,
                              uint8_t *out)
{
#if BULP_IS_LITTLE_ENDIAN
  memcpy (out, &v, 4);
#else
  out[0] = v;
  out[1] = v >> 8;
  out[2] = v >> 16;
  out[3] = v >> 24;
#endif
  return 4;
}
BULP_INLINE size_t
bulp_uint32_pack_to          (uint32_t v,
                              BulpBuffer *out)
{
#if BULP_IS_LITTLE_ENDIAN
  bulp_buffer_append_small (out, 4, &v);
#else
  uint8_t buf[4];
  buf[0] = v;
  buf[1] = v >> 8;
  buf[2] = v >> 16;
  buf[3] = v >> 24;
  bulp_buffer_append_small (out, 4, buf);
#endif
  return 4;
}

BULP_INLINE size_t
bulp_uint32_unpack           (size_t packed_len,
                              const uint8_t *packed_data,
                              uint32_t *out,
                              BulpError**error)
{
  if (packed_len < 4)
    {
      *error = bulp_error_new_too_short ("unpacking uint32");
      return 0;
    }
#if BULP_IS_LITTLE_ENDIAN
  memcpy (out, packed_data, 4);
#else
  uint32_t v = ((uint32_t) packed_data[0])
             | ((uint32_t) packed_data[1] << 8)
             | ((uint32_t) packed_data[2] << 16)
             | ((uint32_t) packed_data[3] << 24);
  * out = v;
#endif
  return 4;
}
BULP_INLINE size_t
bulp_uint64_get_packed_size  (uint64_t v)
{
  (void) v;
  return 8;
}
BULP_INLINE size_t
bulp_uint64_pack             (uint64_t v,
                              uint8_t *out)
{
#if BULP_IS_LITTLE_ENDIAN
  memcpy (out, &v, 8);
#else
  out[0] = v;
  out[1] = v >> 8;
  out[2] = v >> 16;
  out[3] = v >> 24;
  out[4] = v >> 32;
  out[5] = v >> 40;
  out[6] = v >> 48;
  out[7] = v >> 56;
#endif
  return 8;
}
BULP_INLINE size_t
bulp_uint64_pack_to          (uint64_t v,
                              BulpBuffer *out)
{
#if BULP_IS_LITTLE_ENDIAN
  bulp_buffer_append_small (out, 8, &v);
#else
  uint8_t buf[8];
  bulp_uint64_pack (v, buf);
  bulp_buffer_append_small (out, 8, buf);
#endif
  return 8;
}
BULP_INLINE size_t
bulp_uint64_unpack           (size_t packed_len,
                              const uint8_t *packed_data,
                              uint64_t *out,
                              BulpError**error)
{
  if (packed_len < 8)
    {
      *error = bulp_error_new_too_short ("unpacking uint64");
      return 0;
    }
#if BULP_IS_LITTLE_ENDIAN
  memcpy (out, packed_data, 8);
#else
  *out = ((uint64_t)packed_data[0] << 0)
       | ((uint64_t)packed_data[1] << 8)
       | ((uint64_t)packed_data[2] << 16)
       | ((uint64_t)packed_data[3] << 24)
       | ((uint64_t)packed_data[4] << 32)
       | ((uint64_t)packed_data[5] << 40)
       | ((uint64_t)packed_data[6] << 48)
       | ((uint64_t)packed_data[7] << 56);
#endif
  return 8;
}

/* === Fixed Length, Signed Integers (int8, int16, int32, int64) === */
BULP_INLINE size_t bulp_int8_get_packed_size    (int8_t v);
BULP_INLINE size_t bulp_int8_pack               (int8_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_int8_pack_to            (int8_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_int8_unpack             (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int8_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_int16_get_packed_size   (int16_t v);
BULP_INLINE size_t bulp_int16_pack              (int16_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_int16_pack_to           (int16_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_int16_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int16_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_int32_get_packed_size   (int32_t v);
BULP_INLINE size_t bulp_int32_pack              (int32_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_int32_pack_to           (int32_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_int32_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int32_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_int64_get_packed_size   (int64_t v);
BULP_INLINE size_t bulp_int64_pack              (int64_t v,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_int64_pack_to           (int64_t v,
                                                 BulpBuffer *out);
BULP_INLINE size_t bulp_int64_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int64_t *out,
                                                 BulpError**error);
BULP_INLINE size_t bulp_ushort_get_packed_size  (uint16_t v)
{
  return (v < (1<<7)) ? 1 : (v < (1<<14)) ? 2 : 3;
}
BULP_INLINE size_t bulp_ushort_pack             (uint16_t v,
                                                 uint8_t *out)
{
  if (v < (1<<7))
    {
      out[0] = v;
      return 1;
    }
  else if (v < (1<<14))
    {
      out[0] = v | 0x80;
      out[1] = v >> 7;
      return 2;
    }
  else
    {
      out[0] = v | 0x80;
      out[1] = (v >> 7) | 0x80;
      out[2] = (v >> 14);
      return 3;
    }
}
BULP_INLINE size_t bulp_ushort_pack_to          (uint16_t v,
                                                 BulpBuffer *out)
{
  uint8_t b[3];
  unsigned rv = bulp_ushort_pack (v, b);
  bulp_buffer_append_small (out, rv, b);
  return rv;
}

BULP_INLINE size_t
bulp_ushort_unpack          (size_t packed_len,
                             const uint8_t *packed_data,
                             uint16_t *out,
                             BulpError**error)
{
  if ((packed_data[0] & 0x80) == 0)
    {
      *out = packed_data[0];
      return 1;
    }
  if (BULP_UNLIKELY (packed_len < 2))
    {
      *error = bulp_error_new_too_short ("unpacking ushort");
      return 0;
    }
  if ((packed_data[1] & 0x80) == 0)
    {
      *out = ((uint16_t)packed_data[1] << 7) | (packed_data[0] & 0x7f);
      return 2;
    }
  if (BULP_UNLIKELY (packed_len < 3))
    {
      *error = bulp_error_new_too_short ("unpacking ushort");
      return 0;
    }
  if ((packed_data[2] & 0x80) != 0)
    {
      *error = bulp_error_new_bad_data (">3 byte ushort encoding");
      return 0;
    }
  *out = ((uint16_t)packed_data[2] << 14)
       | ((uint16_t)(packed_data[1] & 0x7f) << 7)
       | (packed_data[0] & 0x7f);
  return 3;
}

BULP_INLINE size_t
bulp_uint_get_packed_size    (uint32_t v)
{
  return (v < (1<<7)) ? 1 : (v < (1<<14)) ? 2 :
         (v < (1<<21)) ? 3 : (v < (1<<28)) ? 4 : 5;
}
BULP_INLINE size_t
bulp_uint_pack               (uint32_t v,
                              uint8_t *out)
{
  if (v < (1<<7))
    {
      out[0] = v;
      return 1;
    }
  else if (v < (1<<14))
    {
      out[0] = v | 0x80;
      out[1] = v >> 7;
      return 2;
    }
  else if (v < (1<<21))
    {
      out[0] = v | 0x80;
      out[1] = (v >> 7) | 0x80;
      out[2] = (v >> 14);
      return 3;
    }
  else if (v < (1<<28))
    {
      out[0] = v | 0x80;
      out[1] = (v >> 7) | 0x80;
      out[2] = (v >> 14) | 0x80;
      out[3] = (v >> 21);
      return 4;
    }
  else
    {
      out[0] = v | 0x80;
      out[1] = (v >> 7) | 0x80;
      out[2] = (v >> 14) | 0x80;
      out[3] = (v >> 21) | 0x80;
      out[4] = (v >> 28);
      return 5;
    }
}
BULP_INLINE size_t
bulp_uint_pack_to (uint32_t v, BulpBuffer *out)
{
  uint8_t buf[5];
  size_t rv = bulp_uint_pack (v, buf);
  bulp_buffer_append_small (out, rv, buf);
  return rv;
}
BULP_INLINE size_t
bulp_uint_unpack (size_t packed_len,
                 const uint8_t *packed_data,
                 uint32_t *out,
                 BulpError**error)
{
  if ((packed_data[0] & 0x80) == 0)
    {
      * out = packed_data[0];
      return 1;
    }
  if (BULP_UNLIKELY (packed_len < 2))
    {
      *error = bulp_error_new_too_short ("unpacking uint");
      return 0;
    }
  if ((packed_data[1] & 0x80) == 0)
    {
      *out = ((uint32_t)packed_data[1] << 7) | (packed_data[0] & 0x7f);
      return 2;
    }
  if (BULP_UNLIKELY (packed_len < 3))
    {
      *error = bulp_error_new_too_short ("unpacking uint");
      return 0;
    }
  if ((packed_data[2] & 0x80) == 0)
    {
      *out = ((uint32_t)packed_data[2] << 14)
           | ((uint32_t)(packed_data[1] & 0x7f) << 7)
           | (packed_data[0] & 0x7f);
      return 3;
    }
  if (BULP_UNLIKELY (packed_len < 4))
    {
      *error = bulp_error_new_too_short ("unpacking uint");
      return 0;
    }
  if ((packed_data[3] & 0x80) == 0)
    {
      *out = ((uint32_t)packed_data[3] << 21)
           | ((uint32_t)(packed_data[2] & 0x7f) << 14)
           | ((uint32_t)(packed_data[1] & 0x7f) << 7)
           | (packed_data[0] & 0x7f);
      return 4;
    }
  if (BULP_UNLIKELY (packed_len < 5))
    {
      *error = bulp_error_new_too_short ("unpacking uint");
      return 0;
    }
  if ((packed_data[4] & 0x80) != 0)
    {
      *error = bulp_error_new_bad_data (">5 byte uint encoding");
      return 0;
    }
  *out = ((uint32_t)packed_data[4] << 28)
       | ((uint32_t)(packed_data[3] & 0x7f) << 21)
       | ((uint32_t)(packed_data[2] & 0x7f) << 14)
       | ((uint32_t)(packed_data[1] & 0x7f) << 7)
       | (packed_data[0] & 0x7f);
  return 5;
}


/* ulong functions */
BULP_INLINE size_t
bulp_ulong_get_packed_size  (uint64_t v)
{
  return (v < (1ULL<<7)) ? 1 : (v < (1ULL<<14)) ? 2 :
         (v < (1ULL<<21)) ? 3 : (v < (1ULL<<28)) ? 4 :
         (v < (1ULL<<35)) ? 5 : (v < (1ULL<<42)) ? 6 :
         (v < (1ULL<<49)) ? 7 : (v < (1ULL<<56)) ? 8 :
         (v < (1ULL<<63)) ? 9 : 10;
}
BULP_INLINE size_t
bulp_ulong_pack             (uint64_t v,
                             uint8_t *out)
{
  unsigned rv = 0;
  while (v >= 128)
    {
      out[rv++] = 0x80 | v;
      v >>= 7;
    }
  out[rv++] = v;
  return rv;
}
BULP_INLINE size_t
bulp_ulong_pack_to          (uint64_t v,
                             BulpBuffer *out)
{
  uint8_t b[11];
  unsigned rv = bulp_ulong_pack (v, b);
  bulp_buffer_append_small (out, rv, b);
  return rv;
}

BULP_INLINE size_t
bulp_ulong_unpack           (size_t packed_len,
                             const uint8_t *packed_data,
                             uint64_t *out,
                             BulpError**error)
{
  unsigned n = 0;
  unsigned shift = 0;
  uint64_t rv = 0;
  do
    {
      if (BULP_UNLIKELY (n > 10))
        {
          *error = bulp_error_new_bad_data ("unpacking ulong");
          return 0;
        }
      if (BULP_UNLIKELY (n < packed_len))
        {
          *error = bulp_error_new_too_short ("unpacking ulong");
          return 0;
        }
      rv += ((uint64_t)(packed_data[n] & 0x7f) << shift);
      shift += 7;
    }
  while ((packed_data[n++] & 0x80) != 0);

  *out = rv;
  return n;
}

/* short functions */
BULP_INLINE int16_t
bulp_unzigzag16(uint16_t v)
{
  if (v & 1)
    return -(v >> 1) - 1;
  else
    return v >> 1;
}
BULP_INLINE uint16_t bulp_zigzag16 (int16_t v)
{
  if (v < 0)
    return (-(uint16_t)v) * 2 - 1;
  else
    return (uint16_t)(v) * 2;
}
BULP_INLINE size_t bulp_short_get_packed_size   (int16_t v)
{
  return bulp_ushort_get_packed_size (bulp_zigzag16 (v));
}
BULP_INLINE size_t bulp_short_pack              (int16_t v,
                                                 uint8_t *out)
{
  return bulp_ushort_pack (bulp_zigzag16 (v), out);
}
BULP_INLINE size_t bulp_short_pack_to           (int16_t v,
                                                 BulpBuffer *out)
{
  return bulp_ushort_pack_to (bulp_zigzag16 (v), out);
}
BULP_INLINE size_t bulp_short_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 int16_t *out,
                                                 BulpError**error)
{
  uint16_t uo;
  size_t rv = bulp_ushort_unpack (packed_len, packed_data, &uo, error);
  if (rv == 0)
    return 0;
  *out = bulp_unzigzag16 (uo);
  return rv;
}

/* int functions */
BULP_INLINE int32_t
bulp_unzigzag32(uint32_t v)
{
  if (v & 1)
    return -(v >> 1) - 1;
  else
    return v >> 1;
}
BULP_INLINE uint32_t bulp_zigzag32 (int32_t v)
{
  if (v < 0)
    return (-(uint32_t)v) * 2 - 1;
  else
    return (uint32_t)(v) * 2;
}
BULP_INLINE size_t
bulp_int_get_packed_size (int32_t value)
{
  return bulp_uint_get_packed_size (bulp_zigzag32 (value));
}

BULP_INLINE size_t
bulp_int_unpack (size_t packed_len,
                 const uint8_t *packed_data,
                 int32_t *out,
                 BulpError**error)
{
  uint32_t uo;
  size_t rv = bulp_uint_unpack (packed_len, packed_data, &uo, error);
  if (rv == 0)
    return 0;
  *out = bulp_unzigzag32 (uo);
  return rv;
}

BULP_INLINE size_t
bulp_int_pack (int32_t v, uint8_t *out)
{
  return bulp_uint_pack (bulp_zigzag32 (v), out);
}
BULP_INLINE size_t
bulp_int_pack_to (int32_t v, BulpBuffer *out)
{
  return bulp_uint_pack_to (bulp_zigzag32 (v), out);
}

/* long functions */
BULP_INLINE int64_t
bulp_unzigzag64(uint64_t v)
{
  if (v & 1)
    return -(v >> 1) - 1;
  else
    return v >> 1;
}
BULP_INLINE uint64_t bulp_zigzag64 (int64_t v)
{
  if (v < 0)
    return (-(uint64_t)v) * 2 - 1;
  else
    return (uint64_t)(v) * 2;
}

BULP_INLINE size_t
bulp_long_get_packed_size (int64_t value)
{
  return bulp_ulong_get_packed_size (bulp_zigzag64 (value));
}

BULP_INLINE size_t
bulp_long_unpack (size_t packed_len,
                 const uint8_t *packed_data,
                 int64_t *out,
                 BulpError**error)
{
  uint64_t uo;
  size_t rv = bulp_ulong_unpack (packed_len, packed_data, &uo, error);
  if (rv == 0)
    return 0;
  *out = bulp_unzigzag64 (uo);
  return rv;
}

BULP_INLINE size_t
bulp_long_pack (int64_t v, uint8_t *out)
{
  return bulp_ulong_pack (bulp_zigzag64 (v), out);
}
BULP_INLINE size_t
bulp_long_pack_to (int64_t v, BulpBuffer *out)
{
  return bulp_ulong_pack_to (bulp_zigzag64 (v), out);
}

#endif
