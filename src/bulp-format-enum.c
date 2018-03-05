#include "bulp.h"
#include "bulp-internals.h"
#include "../generated/bulp-machdep-config.h"
#include <string.h>

static int
part_str_compare (ssize_t name_len, const char *name, const char *b)
{
  if (name_len < 0)
    return strcmp (name, b);
  else
    {
      int rv = memcmp (name, b, name_len);
      if (rv == 0 && b[name_len])
        return -1;
      return rv;
    }
}

BulpFormatEnumValue *
bulp_format_enum_lookup_by_name  (BulpFormat *format,
                                  ssize_t     name_len,
                                  const char *name)
{
  unsigned start = 0, n = format->v_enum.n_values;
  while (n > 1)
    {
      unsigned mid = start + n / 2;
      int rv = part_str_compare (name_len, name, format->v_enum.values_by_name[mid]->name);
      if (rv == 0)
        return format->v_enum.values_by_name[mid];
      if (rv < 0)
        {
          n = mid - start;
        }
      else
        {
          unsigned old_end = n + start;
          start = mid + 1;
          n = old_end - start;
        }
    }
  if (n == 0)
    return NULL;
  else if (part_str_compare (name_len, name, format->v_enum.values_by_name[start]->name) == 0)
    return format->v_enum.values_by_name[start];
  else
    return NULL;
}

BulpFormatEnumValue *
bulp_format_enum_lookup_by_value (BulpFormat *format,
                                  unsigned    value)
{
  unsigned start = 0, n = format->v_enum.n_values;
  while (n > 1)
    {
      unsigned mid = start + n / 2;
      unsigned mid_value = format->v_enum.values[mid].value;
      if (mid_value == value)
        return format->v_enum.values + mid;
      if (value < mid_value)
        {
          n = mid - start;
        }
      else
        {
          unsigned old_end = n + start;
          start = mid + 1;
          n = old_end - start;
        }
    }
  if (n == 0)
    return NULL;
  else if (format->v_enum.values[start].value == value)
    return format->v_enum.values + start;
  else
    return NULL;
}

#if 1 || BULP_SIZEOF_TINY_ENUM == 1
static bulp_bool
validate_native__enum1 (BulpFormat *format,
                       void *native_data,
                       BulpError **error)
{
  uint8_t enum_value = * (const uint8_t *) native_data;
  if (bulp_format_enum_lookup_by_value (format, enum_value) == NULL)
    {
      *error = bulp_error_new_bad_data ("invalid value %u for enum %s",
                                        enum_value, format->base.canonical_name);
      return BULP_FALSE;
    }
  return BULP_TRUE;
}
static size_t
get_packed_size__enum1 (BulpFormat *format,
                       void *native_data)
{
  (void) format;
  return bulp_uint32_get_packed_size (* (uint8_t *) native_data);
}
static size_t
pack__enum1           (BulpFormat *format,
                       void *native_data,
                       uint8_t *packed_data_out)
{
  (void) format;
  return bulp_uint32_pack (* (uint8_t *) native_data, packed_data_out);
}
static void
pack_to__enum1        (BulpFormat *format,
                       void *native_data,
                       BulpDataBuilder *out)
{
  (void) format;
  bulp_uint32_pack_to (* (uint8_t *) native_data, out);
}
static size_t
unpack__enum1         (BulpFormat *format,
                       size_t packed_len,
                       const uint8_t *packed_data,
                       void *native_data_out,
                       BulpMemPool *pool,
                       BulpError **error)
{
  (void) pool;
  (void) error;
  uint32_t v;
  size_t rv = bulp_uint32_unpack (packed_len, packed_data, &v, error);
  if (rv == 0)
    return 0;
  if (bulp_format_enum_lookup_by_value (format, v) == NULL)
    {
      *error = bulp_error_new_bad_data ("bad enum value %u", v);
      return 0;
    }
  * (uint8_t *) native_data_out = v;
  return rv;
}

static void
destruct_format__enum1 (BulpFormat  *format)
{
  (void) format;
}
BulpFormatVFuncs vfuncs__enum1 = BULP_FORMAT_VFUNCS_DEFINE(enum1);
#endif
#if 1 || BULP_SIZEOF_TINY_ENUM == 2 || BULP_SIZEOF_SHORT_ENUM == 2
static bulp_bool
validate_native__enum2 (BulpFormat *format,
                       void *native_data,
                       BulpError **error)
{
  uint16_t enum_value = * (const uint16_t *) native_data;
  if (bulp_format_enum_lookup_by_value (format, enum_value) == NULL)
    {
      *error = bulp_error_new_bad_data ("invalid value %u for enum %s",
                                        enum_value, format->base.canonical_name);
      return BULP_FALSE;
    }
  return BULP_TRUE;
}
static size_t
get_packed_size__enum2 (BulpFormat *format,
                       void *native_data)
{
  (void) format;
  return bulp_uint32_get_packed_size (* (uint16_t *) native_data);
}
static size_t
pack__enum2           (BulpFormat *format,
                       void *native_data,
                       uint8_t *packed_data_out)
{
  (void) format;
  return bulp_uint32_pack (* (uint16_t *) native_data, packed_data_out);
}
static void
pack_to__enum2        (BulpFormat *format,
                       void *native_data,
                       BulpDataBuilder *out)
{
  (void) format;
  bulp_uint32_pack_to (* (uint16_t *) native_data, out);
}
static size_t
unpack__enum2         (BulpFormat *format,
                       size_t packed_len,
                       const uint8_t *packed_data,
                       void *native_data_out,
                       BulpMemPool *pool,
                       BulpError **error)
{
  (void) pool;
  (void) error;
  uint32_t v;
  size_t rv = bulp_uint32_unpack (packed_len, packed_data, &v, error);
  if (rv == 0)
    return 0;
  if (bulp_format_enum_lookup_by_value (format, v) == NULL)
    {
      *error = bulp_error_new_bad_data ("bad enum value %u", v);
      return 0;
    }
  * (uint16_t *) native_data_out = v;
  return rv;
}

static void
destruct_format__enum2 (BulpFormat  *format)
{
  (void) format;
}
BulpFormatVFuncs vfuncs__enum2 = BULP_FORMAT_VFUNCS_DEFINE(enum2);
#endif

#if 1 || BULP_SIZEOF_TINY_ENUM == 4 || BULP_SIZEOF_SHORT_ENUM == 4 || BULP_SIZEOF_INT_ENUM == 4
static bulp_bool
validate_native__enum4 (BulpFormat *format,
                       void *native_data,
                       BulpError **error)
{
  uint32_t enum_value = * (const uint32_t *) native_data;
  if (bulp_format_enum_lookup_by_value (format, enum_value) == NULL)
    {
      *error = bulp_error_new_bad_data ("invalid value %u for enum %s",
                                        enum_value, format->base.canonical_name);
      return BULP_FALSE;
    }
  return BULP_TRUE;
}
static size_t
get_packed_size__enum4 (BulpFormat *format,
                       void *native_data)
{
  (void) format;
  return bulp_uint32_get_packed_size (* (uint32_t *) native_data);
}
static size_t
pack__enum4           (BulpFormat *format,
                       void *native_data,
                       uint8_t *packed_data_out)
{
  (void) format;
  return bulp_uint32_pack (* (uint32_t *) native_data, packed_data_out);
}
static void
pack_to__enum4        (BulpFormat *format,
                       void *native_data,
                       BulpDataBuilder *out)
{
  (void) format;
  bulp_uint32_pack_to (* (uint32_t *) native_data, out);
}
static size_t
unpack__enum4         (BulpFormat *format,
                       size_t packed_len,
                       const uint8_t *packed_data,
                       void *native_data_out,
                       BulpMemPool *pool,
                       BulpError **error)
{
  (void) format;
  (void) pool;
  uint32_t v;
  size_t rv = bulp_uint32_unpack (packed_len, packed_data, &v, error);
  if (rv == 0)
    return 0;
  if (bulp_format_enum_lookup_by_value (format, v) == NULL)
    {
      *error = bulp_error_new_bad_data ("bad enum value %u", v);
      return 0;
    }
  * (uint32_t *) native_data_out = v;
  return rv;
}

static void
destruct_format__enum4 (BulpFormat  *format)
{
  (void) format;
}
BulpFormatVFuncs vfuncs__enum4 = BULP_FORMAT_VFUNCS_DEFINE(enum4);
#endif

static int
compare_format_enum_values_by_value (const void *a, const void *b)
{
  const BulpFormatEnumValue *A = a;
  const BulpFormatEnumValue *B = b;
  return (A->value < B->value) ? -1 : (A->value > B->value) ? 1 : 0;
}

static int
compare_ptr_format_enum_values_by_name (const void *a, const void *b)
{
  const BulpFormatEnumValue *const*Ap = a;
  const BulpFormatEnumValue *const*Bp = b;
  const BulpFormatEnumValue *A = *Ap;
  const BulpFormatEnumValue *B = *Bp;
  return strcmp (A->name, B->name);
}

BulpFormat *
bulp_format_new_enum         (unsigned n_values,
                              BulpEnumValue *values)
{
  BulpFormatEnum *rv = malloc (sizeof (BulpFormatEnum));
  memset (rv, 0, sizeof (BulpFormatEnum));

  //  compute values
  rv->n_values = n_values;
  rv->values = malloc (sizeof (BulpFormatEnumValue) * n_values);
  unsigned next_value = 0;
  for (unsigned i = 0; i < n_values; i++)
    {
      unsigned v = (values[i].set_value) ?  values[i].value_if_set : next_value;
      rv->values[i].value = v;
      rv->values[i].name = strdup (values[i].name);

      next_value = v + 1;
    }

  // sort values by value
  qsort (rv->values, rv->n_values, sizeof (BulpFormatEnumValue), compare_format_enum_values_by_value);

  // search for dup values
  for (unsigned i = 1; i < n_values; i++)
    if (rv->values[i-1].value == rv->values[i].value)
      {
        bulp_die ("duplicate value in enum (%s v %s) (value=%u)",
                  rv->values[i-1].name, rv->values[i].name, rv->values[i].value);
      }

  // compute values_by_name
  rv->values_by_name = malloc (sizeof (BulpFormatEnumValue *) * n_values);
  for (unsigned i = 0; i < n_values; i++)
    rv->values_by_name[i] = rv->values + i;
  qsort (rv->values_by_name, rv->n_values, sizeof (BulpFormatEnumValue*), compare_ptr_format_enum_values_by_name);

  // search for dup names
  for (unsigned i = 1; i < n_values; i++)
    if (strcmp (rv->values_by_name[i-1]->name, rv->values_by_name[i]->name) == 0)
      {
        bulp_die ("duplicate name in enum (name %s)", rv->values_by_name[i]->name);
      }

  // initialize rv->base
  rv->base.type = BULP_FORMAT_TYPE_ENUM;
  rv->base.ref_count = 1;
  /* native (ie equivalent to generated C code) representation */
  uint32_t maxv = rv->values[rv->n_values - 1].value;
  rv->base.c_sizeof = maxv < 256 ? BULP_SIZEOF_TINY_ENUM
                    : maxv < (1<<16) ? BULP_SIZEOF_SHORT_ENUM
                    : BULP_SIZEOF_INT_ENUM;
  rv->base.c_alignof = rv->base.c_sizeof == 1 ? 1
                     : rv->base.c_sizeof == 2 ? BULP_INT16_ALIGNOF
                     : rv->base.c_sizeof == 4 ? BULP_INT32_ALIGNOF
                     : BULP_INT64_ALIGNOF;
  switch (rv->base.c_sizeof)
    {
    case 1:
      rv->base.c_alignof = 1;
      rv->base.vfuncs = vfuncs__enum1;
      break;
    case 2:
      rv->base.c_alignof = BULP_INT16_ALIGNOF;
      rv->base.vfuncs = vfuncs__enum2;
      break;
    case 4:
      rv->base.c_alignof = BULP_INT32_ALIGNOF;
      rv->base.vfuncs = vfuncs__enum4;
      break;
    case 8:
      rv->base.c_alignof = BULP_INT64_ALIGNOF;
      //rv->base.vfuncs = vfuncs__enum4; TODO
      break;
    default:
      assert(0);
    }
  rv->base.copy_with_memcpy = BULP_TRUE;
  rv->base.is_zeroable = rv->values[0].value == 0;

  return (BulpFormat *) rv;
}
