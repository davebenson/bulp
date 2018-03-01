#include "bulp.h"
#include "bulp-internals.h"
#include <string.h>
#include <math.h>

#define DEFINE_BULP_FORMAT_INT_GENERIC(shortname, ucshortname, ctype, sizeof_bits, is_signed, is_b128) \
{                                                                \
  {                                                              \
    BULP_FORMAT_TYPE_INT,                                        \
    1,                    /* ref-count */                        \
    BULP_FORMAT_VFUNCS_DEFINE(shortname),                        \
    NULL, NULL,                   /* canonical name/ns */        \
    BULP_INT##sizeof_bits##_ALIGNOF,                             \
    sizeof_bits / 8,                                             \
    BULP_TRUE, /* copy_with_memcpy */                            \
    BULP_TRUE, /* is_zeroable */                                 \
    #ctype,                                                      \
    "bulp_"  #shortname,                                         \
    "BULP_"  #ucshortname,                                       \
    NULL,                                                        \
    NULL,                                                        \
  },                                                             \
  sizeof_bits / 8,                                               \
  is_signed,                                                     \
  is_b128                                                        \
}

/* --- uint8 implementation --- */
#define validate_native__uint8 NULL
static size_t
get_packed_size__uint8   (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 1;
}

static size_t
pack__uint8            (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  *packed_data_out = * (uint8_t *) native_data;
  return 1;
}

static void
pack_to__uint8         (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
  bulp_data_builder_append_byte (out, * (uint8_t *) native_data);
}

static size_t
unpack__uint8          (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  return bulp_uint8_unpack (packed_len, packed_data, native_data_out, error);
}

static void
destruct_format__uint8   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__uint8 = DEFINE_BULP_FORMAT_INT_GENERIC(
  uint8,
  UINT8,
  uint8_t,
  8,
  BULP_FALSE,
  BULP_FALSE
);

/* --- uint16 implementation --- */
#define validate_native__uint16 NULL
static size_t
get_packed_size__uint16  (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 2;
}

static size_t
pack__uint16           (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  return bulp_uint16_pack (* (uint16_t *) native_data, packed_data_out);
}

static void
pack_to__uint16        (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
  bulp_uint16_pack_to (* (uint16_t *) native_data, out);
}

static size_t
unpack__uint16         (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  return bulp_uint16_unpack (packed_len, packed_data, native_data_out, error);
}

static void
destruct_format__uint16   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__uint16 = DEFINE_BULP_FORMAT_INT_GENERIC(
  uint16,
  UINT16,
  uint16_t,
  16,
  BULP_FALSE,
  BULP_FALSE
);

/* --- uint32 implementation --- */
#define validate_native__uint32 NULL
static size_t
get_packed_size__uint32  (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 4;
}

static size_t
pack__uint32           (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  uint32_t v = * (uint32_t *) native_data;
  return bulp_uint32_pack (v, packed_data_out);
}

static void
pack_to__uint32        (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
  uint32_t v = * (uint32_t *) native_data;
  bulp_uint32_pack_to (v, out);
}

static size_t
unpack__uint32         (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  return bulp_uint32_unpack (packed_len, packed_data, native_data_out, error);
}

static void
destruct_format__uint32   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__uint32 = DEFINE_BULP_FORMAT_INT_GENERIC(
  uint32,
  UINT32,
  uint32_t,
  32,
  BULP_FALSE,
  BULP_FALSE
);

/* --- uint64 implementation --- */
#define validate_native__uint64 NULL
static size_t
get_packed_size__uint64  (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 8;
}

static size_t
pack__uint64           (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
#if BULP_IS_LITTLE_ENDIAN
  memcpy (packed_data_out, native_data, 8);
#else
  uint64_t v = * (uint64_t *) native_data;
  packed_data_out[0] = v;
  packed_data_out[1] = v >> 8;
  packed_data_out[2] = v >> 16;
  packed_data_out[3] = v >> 24;
  packed_data_out[4] = v >> 32;
  packed_data_out[5] = v >> 40;
  packed_data_out[6] = v >> 48;
  packed_data_out[7] = v >> 56;
#endif
  return 8;
}

static void
pack_to__uint64        (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
#if BULP_IS_LITTLE_ENDIAN
  bulp_data_builder_append_nocopy (out, 8, native_data);
#else
  uint64_t v = * (uint64_t *) native_data;
  uint8_t tmp[8];
  tmp[0] = v;
  tmp[1] = v >> 8;
  tmp[2] = v >> 16;
  tmp[3] = v >> 24;
  tmp[4] = v >> 32;
  tmp[5] = v >> 40;
  tmp[6] = v >> 48;
  tmp[7] = v >> 56;
  bulp_data_builder_append (out, 8, tmp);
#endif
}

static size_t
unpack__uint64         (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  if (packed_len < 8)
    {
      *error = bulp_error_new_too_short ("unpacking uint64");
      return 0;
    }
#if BULP_IS_LITTLE_ENDIAN
  memcpy (native_data_out, packed_data, 8);
#else
  uint64_t v = ((uint64_t) packed_data[0])
             | ((uint64_t) packed_data[1] << 8)
             | ((uint64_t) packed_data[2] << 16)
             | ((uint64_t) packed_data[3] << 24)
             | ((uint64_t) packed_data[4] << 32)
             | ((uint64_t) packed_data[5] << 40)
             | ((uint64_t) packed_data[6] << 48)
             | ((uint64_t) packed_data[7] << 56);
  * (uint64_t *) native_data_out = v;
#endif
  return 8;
}

static void
destruct_format__uint64   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__uint64 = DEFINE_BULP_FORMAT_INT_GENERIC(
  uint64,
  UINT64,
  uint64_t,
  64,
  BULP_FALSE,
  BULP_FALSE
);

/* --- int8 implementation --- */
#define validate_native__int8 NULL
static size_t
get_packed_size__int8    (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 1;
}

static size_t
pack__int8             (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  *packed_data_out = * (uint8_t *) native_data;
  return 1;
}

static void
pack_to__int8          (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
  bulp_data_builder_append_byte (out, * (uint8_t *) native_data);
}

static size_t
unpack__int8           (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  if (packed_len < 1)
    {
      *error = bulp_error_new_too_short ("unpacking int8");
      return 0;
    }
  * (int8_t *) native_data_out = (int8_t) packed_data[0];
  return 1;
}

static void
destruct_format__int8   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__int8 = DEFINE_BULP_FORMAT_INT_GENERIC(
  int8,
  INT8,
  int8_t,
  8,
  BULP_TRUE,
  BULP_FALSE
);

/* --- int16 implementation --- */
#define validate_native__int16 NULL
static size_t
get_packed_size__int16   (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 2;
}

static size_t
pack__int16            (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
#if BULP_IS_LITTLE_ENDIAN
  memcpy (packed_data_out, native_data, 2);
#else
  uint16_t v = * (uint16_t *) native_data;
  packed_data_out[0] = v;
  packed_data_out[1] = v >> 8;
#endif
  return 2;
}

static void
pack_to__int16         (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
#if BULP_IS_LITTLE_ENDIAN
  bulp_data_builder_append_nocopy (out, 2, native_data);
#else
  uint16_t v = * (uint16_t *) native_data;
  uint8_t tmp[2];
  tmp[0] = v;
  tmp[1] = v >> 8;
  bulp_data_builder_append (out, 2, tmp);
#endif
}

static size_t
unpack__int16          (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  if (packed_len < 2)
    {
      *error = bulp_error_new_too_short ("unpacking uint16");
      return 0;
    }
#if BULP_IS_LITTLE_ENDIAN
  memcpy (native_data_out, packed_data, 2);
#else
  uint16_t v = ((uint16_t) packed_data[0])
             | ((uint16_t) packed_data[1] << 8);
  * (uint16_t *) native_data_out = v;
#endif
  return 2;
}

static void
destruct_format__int16   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__int16 = DEFINE_BULP_FORMAT_INT_GENERIC(
  int16,
  INT16,
  int16_t,
  16,
  BULP_TRUE,
  BULP_FALSE
);

/* --- int32 implementation --- */
#define validate_native__int32 NULL
static size_t
get_packed_size__int32   (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 4;
}

static size_t
pack__int32            (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
#if BULP_IS_LITTLE_ENDIAN
  memcpy (packed_data_out, native_data, 4);
#else
  uint32_t v = * (uint32_t *) native_data;
  packed_data_out[0] = v;
  packed_data_out[1] = v >> 8;
  packed_data_out[2] = v >> 16;
  packed_data_out[3] = v >> 24;
#endif
  return 4;
}

static void
pack_to__int32         (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
#if BULP_IS_LITTLE_ENDIAN
  bulp_data_builder_append_nocopy (out, 4, native_data);
#else
  uint32_t v = * (uint32_t *) native_data;
  uint8_t tmp[4];
  tmp[0] = v;
  tmp[1] = v >> 8;
  tmp[2] = v >> 16;
  tmp[3] = v >> 24;
  bulp_data_builder_append (out, 4, tmp);
#endif
}

static size_t
unpack__int32          (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  if (packed_len < 4)
    {
      *error = bulp_error_new_too_short ("unpacking uint32");
      return 0;
    }
#if BULP_IS_LITTLE_ENDIAN
  memcpy (native_data_out, packed_data, 4);
#else
  uint32_t v = ((uint32_t) packed_data[0])
             | ((uint32_t) packed_data[1] << 8)
             | ((uint32_t) packed_data[2] << 16)
             | ((uint32_t) packed_data[3] << 24);
  * (uint32_t *) native_data_out = v;
#endif
  return 4;
}

static void
destruct_format__int32   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__int32 = DEFINE_BULP_FORMAT_INT_GENERIC(
  int32,
  INT32,
  int32_t,
  32,
  BULP_TRUE,
  BULP_FALSE
);

/* --- int64 implementation --- */
#define validate_native__int64 NULL
static size_t
get_packed_size__int64   (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 8;
}

static size_t
pack__int64            (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
#if BULP_IS_LITTLE_ENDIAN
  memcpy (packed_data_out, native_data, 8);
#else
  uint64_t v = * (uint64_t *) native_data;
  packed_data_out[0] = v;
  packed_data_out[1] = v >> 8;
  packed_data_out[2] = v >> 16;
  packed_data_out[3] = v >> 24;
  packed_data_out[4] = v >> 32;
  packed_data_out[5] = v >> 40;
  packed_data_out[6] = v >> 48;
  packed_data_out[7] = v >> 56;
#endif
  return 8;
}

static void
pack_to__int64         (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
#if BULP_IS_LITTLE_ENDIAN
  bulp_data_builder_append_nocopy (out, 8, native_data);
#else
  uint64_t v = * (uint64_t *) native_data;
  uint8_t tmp[8];
  tmp[0] = v;
  tmp[1] = v >> 8;
  tmp[2] = v >> 16;
  tmp[3] = v >> 24;
  tmp[4] = v >> 32;
  tmp[5] = v >> 40;
  tmp[6] = v >> 48;
  tmp[7] = v >> 56;
  bulp_data_builder_append (out, 8, tmp);
#endif
}

static size_t
unpack__int64          (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  if (packed_len < 8)
    {
      *error = bulp_error_new_too_short ("unpacking int64");
      return 0;
    }
#if BULP_IS_LITTLE_ENDIAN
  memcpy (native_data_out, packed_data, 8);
#else
  uint64_t v = ((uint64_t) packed_data[0])
             | ((uint64_t) packed_data[1] << 8)
             | ((uint64_t) packed_data[2] << 16)
             | ((uint64_t) packed_data[3] << 24)
             | ((uint64_t) packed_data[4] << 32)
             | ((uint64_t) packed_data[5] << 40)
             | ((uint64_t) packed_data[6] << 48)
             | ((uint64_t) packed_data[7] << 56);
  * (uint64_t *) native_data_out = v;
#endif
  return 8;
}

static void
destruct_format__int64   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__int64 = DEFINE_BULP_FORMAT_INT_GENERIC(
  int64,
  INT64,
  int64_t,
  64,
  BULP_TRUE,
  BULP_FALSE
);

/* --- ushort implementation --- */
#define validate_native__ushort NULL
static size_t
get_packed_size__ushort  (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  return bulp_ushort_get_packed_size (* (uint16_t *) native_data);
}

static size_t
pack__ushort           (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  return bulp_ushort_pack (* (uint16_t *) native_data, packed_data_out);
}

static void
pack_to__ushort        (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
  bulp_ushort_pack_to (* (uint16_t *) native_data, out);
}

static size_t
unpack__ushort         (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  return bulp_ushort_unpack (packed_len, packed_data, native_data_out, error);
} 

static void
destruct_format__ushort  (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__ushort = DEFINE_BULP_FORMAT_INT_GENERIC(
  ushort,
  USHORT,
  uint16_t,
  16,
  BULP_FALSE,
  BULP_TRUE
);

/* --- uint implementation --- */
#define validate_native__uint NULL
static size_t
get_packed_size__uint    (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  uint32_t v = * (uint32_t *) native_data;
  return bulp_uint_get_packed_size (v);
}

static size_t
pack__uint             (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  return bulp_uint_pack (* (uint32_t *) native_data, packed_data_out);
}

static void
pack_to__uint          (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
  uint8_t b[5];
  unsigned rv = pack__uint (format, native_data, b);
  bulp_data_builder_append (out, rv, b);
}

static size_t
unpack__uint           (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  return bulp_uint_unpack (packed_len, packed_data, native_data_out, error);
} 

static void
destruct_format__uint    (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__uint   = DEFINE_BULP_FORMAT_INT_GENERIC(
  uint,
  UINT,
  uint32_t,
  32,
  BULP_FALSE,
  BULP_TRUE
);

/* --- ulong implementation --- */
#define validate_native__ulong NULL
static size_t
get_packed_size__ulong   (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  return bulp_ulong_get_packed_size (* (uint64_t *) native_data);
}

static size_t
pack__ulong            (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  uint64_t v = * (uint64_t *) native_data;
  unsigned rv = 0;
  while (v >= 128)
    {
      packed_data_out[rv++] = 0x80 | v;
      v >>= 7;
    }
  packed_data_out[rv++] = v;
  return rv;
}

static void
pack_to__ulong         (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
  uint8_t b[11];
  unsigned rv = pack__ulong (format, native_data, b);
  bulp_data_builder_append (out, rv, b);
}

static size_t
unpack__ulong          (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  unsigned n = 0;
  unsigned shift = 0;
  uint64_t rv = 0;
  (void) format;
  (void) pool;
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

  * (uint64_t *) native_data_out = rv;
  return n;
} 

static void
destruct_format__ulong    (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__ulong = DEFINE_BULP_FORMAT_INT_GENERIC(
  ulong,
  ULONG,
  uint64_t,
  64,
  BULP_FALSE,
  BULP_TRUE
);

/* --- short implementation --- */
/* --- int implementation --- */
/* --- long implementation --- */
static inline uint16_t zigzag16 (int16_t sv)
{
  if (sv >= 0)
    return (sv) * 2;
  else
    return 1 + (-sv) * 2;
}
#define validate_native__short NULL
static size_t
get_packed_size__short  (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  int16_t sv = * (int16_t *) native_data;
  uint16_t v = zigzag16 (sv);
  return (v < (1<<7)) ? 1 : (v < (1<<14)) ? 2 : 3;
}

static size_t
pack__short           (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  int16_t sv = * (int16_t *) native_data;
  uint16_t v = zigzag16 (sv);
  if (v < (1<<7))
    {
      packed_data_out[0] = v;
      return 1;
    }
  else if (v < (1<<14))
    {
      packed_data_out[0] = v | 0x80;
      packed_data_out[1] = v >> 7;
      return 2;
    }
  else
    {
      packed_data_out[0] = v | 0x80;
      packed_data_out[1] = (v >> 7) | 0x80;
      packed_data_out[2] = (v >> 14);
      return 3;
    }
}

static void
pack_to__short         (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
  uint8_t b[3];
  unsigned rv = pack__short (format, native_data, b);
  bulp_data_builder_append (out, rv, b);
}

static size_t
unpack__short         (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  if ((packed_data[0] & 0x80) == 0)
    {
      * (uint16_t *) native_data_out = packed_data[0];
      return 1;
    }
  if (BULP_UNLIKELY (packed_len < 2))
    {
      *error = bulp_error_new_too_short ("unpacking short");
      return 0;
    }
  if ((packed_data[1] & 0x80) == 0)
    {
      * (uint16_t *) native_data_out = ((uint16_t)packed_data[1] << 7) | (packed_data[0] & 0x7f);
      return 2;
    }
  if (BULP_UNLIKELY (packed_len < 3))
    {
      *error = bulp_error_new_too_short ("unpacking short");
      return 0;
    }
  if ((packed_data[2] & 0x80) != 0)
    {
      *error = bulp_error_new_bad_data (">3 byte short encoding");
      return 0;
    }
  * (uint16_t *) native_data_out = ((uint16_t)packed_data[2] << 14)
                                 | ((uint16_t)(packed_data[1] & 0x7f) << 7)
                                 | (packed_data[0] & 0x7f);
  return 3;
} 

static void
destruct_format__short  (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__short = DEFINE_BULP_FORMAT_INT_GENERIC(
  short,
  SHORT,
  int16_t,
  16,
  BULP_TRUE,
  BULP_TRUE
);

/* --- int implementation --- */
#define validate_native__int NULL
static size_t
get_packed_size__int    (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  uint32_t v = * (uint32_t *) native_data;
  return bulp_uint_get_packed_size (v);
}

static size_t
pack__int             (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  return bulp_int_pack (* (int32_t *) native_data, packed_data_out);
}

static void
pack_to__int          (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
  bulp_int_pack_to (* (int32_t *) native_data, out);
}

static size_t
unpack__int           (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  return bulp_int_unpack (packed_len, packed_data, native_data_out, error);
} 

static void
destruct_format__int    (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__int   = DEFINE_BULP_FORMAT_INT_GENERIC(
  int,
  INT,
  int32_t,
  32,
  BULP_TRUE,
  BULP_TRUE
);

/* --- long implementation --- */
#define validate_native__long NULL
static size_t
get_packed_size__long   (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  return bulp_long_get_packed_size (* (int64_t *) native_data);
}

static size_t
pack__long            (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  return bulp_long_pack (* (int64_t *) native_data, packed_data_out);
}

static void
pack_to__long         (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *out)
{
  (void) format;
  bulp_long_pack_to (* (int64_t *) native_data, out);
}

static size_t
unpack__long          (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  (void) format;
  (void) pool;
  return bulp_long_unpack (packed_len, packed_data, native_data_out, error);
} 

static void
destruct_format__long    (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__long = DEFINE_BULP_FORMAT_INT_GENERIC(
  long,
  LONG,
  int64_t,
  64,
  BULP_FALSE,
  BULP_TRUE
);


static BulpFormat *
dup_int_format (BulpFormatInt *format_int)
{
  BulpFormatInt *rv = malloc (sizeof (BulpFormatInt));
  memcpy (rv, format_int, sizeof (BulpFormatInt));
  return (BulpFormat *) rv;
}

#define DEFINE_BULP_FORMAT_FLOAT_GENERIC(shortname, ucshortname, ctype, sizeof_bits) \
{                                                                \
  {                                                              \
    BULP_FORMAT_TYPE_FLOAT,                                      \
    1,                    /* ref-count */                        \
    BULP_FORMAT_VFUNCS_DEFINE(shortname),                        \
    NULL, NULL,                   /* canonical name/ns */        \
    BULP_FLOAT##sizeof_bits##_ALIGNOF,                           \
    sizeof_bits / 8,                                             \
    BULP_TRUE, /* copy_with_memcpy */                            \
    BULP_TRUE, /* is_zeroable */                                 \
    #ctype,                                                      \
    "bulp_"  #shortname,                                         \
    "BULP_"  #ucshortname,                                       \
    NULL,       /* optional_of */                                \
    NULL        /* array_of */                                   \
  },                                                             \
  BULP_FLOAT_TYPE_##ucshortname,                                 \
}

static bulp_bool
validate_native__float32 (BulpFormat *format,
                          void       *native_data,
                          BulpError **error)
{
  (void) format;
  switch (fpclassify (* (float *) native_data))
    {
    case FP_INFINITE:
      *error = bulp_error_new_infinity_not_allowed ();
      return BULP_FALSE;
    case FP_NAN:
    case FP_SUBNORMAL:
      *error = bulp_error_new_not_a_number ();
      return BULP_FALSE;
    default:
      return BULP_TRUE;
    }
}

static size_t
get_packed_size__float32 (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 4;
}

static size_t
pack__float32            (BulpFormat *format,
                          void *native_data,
                          uint8_t *packed_data_out)
{
  (void) format;
  return bulp_float32_pack (* (float *) native_data, packed_data_out);
}

static void
pack_to__float32         (BulpFormat *format,
                          void *native_data,
                          BulpDataBuilder *out)
{
  (void) format;
  bulp_float32_pack_to (* (float *) native_data, out);
}

static size_t
unpack__float32          (BulpFormat *format,
                          size_t packed_len,
                          const uint8_t *packed_data,
                          void *native_data_out,
                          BulpMemPool *pool,
                          BulpError **error)
{
  (void) format;
  (void) pool;
  return bulp_float32_unpack (packed_len, packed_data, native_data_out, error);
}

static void
destruct_format__float32 (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatFloat float_format__float32 = DEFINE_BULP_FORMAT_FLOAT_GENERIC(
  float32,
  FLOAT32,
  float,
  32
);


static bulp_bool
validate_native__float64 (BulpFormat *format,
                          void       *native_data,
                          BulpError **error)
{
  (void) format;
  switch (fpclassify (* (double *) native_data))
    {
    case FP_INFINITE:
      *error = bulp_error_new_infinity_not_allowed ();
      return BULP_FALSE;
    case FP_NAN:
    case FP_SUBNORMAL:
      *error = bulp_error_new_not_a_number ();
      return BULP_FALSE;
    default:
      return BULP_TRUE;
    }
}

static size_t
get_packed_size__float64 (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 4;
}

static size_t
pack__float64            (BulpFormat *format,
                          void *native_data,
                          uint8_t *packed_data_out)
{
  (void) format;
  return bulp_float64_pack (* (double *) native_data, packed_data_out);
}

static void
pack_to__float64         (BulpFormat *format,
                          void *native_data,
                          BulpDataBuilder *out)
{
  (void) format;
  bulp_float64_pack_to (* (double *) native_data, out);
}

static size_t
unpack__float64          (BulpFormat *format,
                          size_t packed_len,
                          const uint8_t *packed_data,
                          void *native_data_out,
                          BulpMemPool *pool,
                          BulpError **error)
{
  (void) format;
  (void) pool;
  return bulp_float64_unpack (packed_len, packed_data, native_data_out, error);
}

static void
destruct_format__float64 (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatFloat float_format__float64 = DEFINE_BULP_FORMAT_FLOAT_GENERIC(
  float64,
  FLOAT64,
  double,
  64
);
static BulpFormat *
dup_float_format (BulpFormatFloat *format_float)
{
  BulpFormatFloat *rv = malloc (sizeof (BulpFormatFloat));
  memcpy (rv, format_float, sizeof (BulpFormatFloat));
  return (BulpFormat *) rv;
}

void
_bulp_namespace_toplevel_add_builtins (BulpNamespaceToplevel *ns)
{
#define ADD_INT_TYPE_TO_NS(shortname)                                 \
  do{                                                                   \
    BulpFormat *tmp = dup_int_format(&int_format__##shortname);         \
    bulp_namespace_add_format (&ns->base, -1, #shortname, tmp, BULP_TRUE);  \
    ns->format_##shortname = tmp;                                       \
    bulp_format_unref (tmp);                                            \
  }while(0)

  ADD_INT_TYPE_TO_NS(uint8);
  ADD_INT_TYPE_TO_NS(uint16);
  ADD_INT_TYPE_TO_NS(uint32);
  ADD_INT_TYPE_TO_NS(uint64);
  ADD_INT_TYPE_TO_NS(int8);
  ADD_INT_TYPE_TO_NS(int16);
  ADD_INT_TYPE_TO_NS(int32);
  ADD_INT_TYPE_TO_NS(int64);

  ADD_INT_TYPE_TO_NS(ushort);
  ADD_INT_TYPE_TO_NS(uint);
  ADD_INT_TYPE_TO_NS(ulong);
  ADD_INT_TYPE_TO_NS(short);
  ADD_INT_TYPE_TO_NS(int);
  ADD_INT_TYPE_TO_NS(long);

#undef ADD_INT_TYPE_TO_NS

#define ADD_FLOAT_TYPE_TO_NS(shortname)                                 \
  do{                                                                   \
    BulpFormat *tmp = dup_float_format(&float_format__##shortname);     \
    bulp_namespace_add_format (&ns->base, -1, #shortname, tmp, BULP_TRUE);  \
    ns->format_##shortname = tmp;                                       \
    bulp_format_unref (tmp);                                            \
  }while(0)

  ADD_FLOAT_TYPE_TO_NS(float32);
  ADD_FLOAT_TYPE_TO_NS(float64);

#undef ADD_FLOAT_TYPE_TO_NS
}
