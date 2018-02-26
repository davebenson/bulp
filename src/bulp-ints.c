#include "bulp.h"
#include "bulp-internals.h"

#define DEFINE_BULP_FORMAT_INT_GENERIC(shortname, ucshortname, ctype, sizeof_bits) \
{                                                              \
  BULP_FORMAT_TYPE_INT,                                        \
  1,                    /* ref-count */                        \
  BULP_FORMAT_VFUNCS_DEFINE(shortname),                        \
  NULL, NULL,                   /* canonical name/ns */        \
  BULP_INT##sizeof_bits##_ALIGNOF, \                           \
  sizeof_bits / 8, \                                           \
  BULP_TRUE, /* copy_with_memcpy */                            \
  BULP_TRUE, /* is_zeroable */                                 \
  ctype,                                                       \
  "bulp_" ## #shortname,                                       \
  "BULP_" ## #ucshortname                                      \
}

/* --- uint8 implementation --- */

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

static size_t
pack_to__uint8         (BulpFormat *format,
                        void *native_data,
                        BulpBuffer *out)
{
  (void) format;
  bulp_buffer_append_byte (out, * (uint8_t *) native_data);
  return 1;
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
  if (packed_len < 1)
    {
      *error = bulp_error_new_too_short ("unpacking uint8");
      return 0;
    }
  * (uint8_t *) native_data_out = packed_data[0];
  return 1;
}

static void
destruct_format__uint8   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__uint8 = DEFINE_BULP_FORMAT_INT_GENERIC(uint8, UINT8, uint8_t, 8);

/* --- uint16 implementation --- */
static size_t
get_packed_size__uint16  (BulpFormat *format,
                          void *native_data)
{
  (void) format;
  (void) native_data;
  return 1;
}

static size_t
pack__uint16           (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  (void) format;
  uint16_t v = * (uint16_t *) native_data;
  packed_data_out[0] = v;
  packed_data_out[1] = v >> 8;
  return 2;
}

static size_t
pack_to__uint16        (BulpFormat *format,
                        void *native_data,
                        BulpBuffer *out)
{
  (void) format;
  uint16_t v = * (uint16_t *) native_data;
  uint8_t tmp[2];
  tmp[0] = v;
  tmp[1] = v >> 8;
  bulp_buffer_append_small (out, 2, tmp);
  return 2;
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
  if (packed_len < 2)
    {
      *error = bulp_error_new_too_short ("unpacking uint16");
      return 0;
    }
  uint16_t v = ((uint16_t) packed_data[0])
             | ((uint16_t) packed_data[1] << 8);
  * (uint16_t *) native_data_out = v;
  return 1;
}

static void
packed_to_json__uint16  (BulpFormat  *format,
                        ...)
{
...
}

static void
json_to_packed__uint16   (BulpFormat  *format,
                         ...)
{
...
}

static void
destruct_format__uint16   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__uint16 = DEFINE_BULP_FORMAT_INT_GENERIC(uint16, UINT16, uint16_t, 16);

/* --- uint32 implementation --- */
...

/* --- uint64 implementation --- */
...

static BulpFormat *
dup_int_format (BulpFormatInt *format_int)
{
  BulpFormatInt *rv = malloc (sizeof (BulpFormatInt));
  memcpy (rv, &int_format__uint8, sizeof (BulpFormatInt));
  return &rv->base;
}

void
_bulp_namespace_add_int_types (BulpNamespaceToplevel *ns)
{
  BulpFormat *tmp;

#define ADD_INT_TYPE_TO_NS(shortname)                                 \
  do{                                                                   \
    BulpFormat *tmp = dup_int_format(&int_format__##shortname);         \
    bulp_namespace_add_format (&ns->base, #shortname, tmp, BULP_TRUE);  \
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
}
