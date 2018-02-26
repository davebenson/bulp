
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
  return 1;
}

static size_t
pack__uint8            (BulpFormat *format,
                        void *native_data,
                        uint8_t *packed_data_out)
{
  *packed_data_out = * (uint8_t *) native_data;
  return 1;
}

static size_t
pack_to__uint8         (BulpFormat *format,
                        void *native_data,
                        BulpBuffer *out)
{
  bulp_buffer_append_byte (out, * (uint8_t *) native_data;
  return 1;
}

static bulp_bool
unpack__uint8          (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
...
}

static void
packed_to_json__uint8  (BulpFormat  *format,
                        ...)
{
...
}

static void
json_to_packed__uint8   (BulpFormat  *format,
                         ...)
{
...
}

static void
destruct_format__uint8   (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatInt int_format__uint8 = DEFINE_BULP_FORMAT_INT_GENERIC(uint8, UINT8, uint8_t, 8);

void
_bulp_namespace_add_int_types (BulpNamespace *ns)
{
}
