#include "bulp.h"
#include "bulp-internals.h"
#include "../generated/bulp-machdep-config.h"
#include <string.h>
#include <stdio.h>


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

BulpFormatUnionCase *
bulp_format_union_lookup_by_name   (BulpFormat *format,
                                    ssize_t     name_len,
                                    const char *name)
{
  assert (format->type == BULP_FORMAT_TYPE_UNION);
  unsigned start = 0, n = format->v_union.n_cases;
  while (n > 1)
    {
      unsigned mid = start + n / 2;
      int rv = part_str_compare (name_len, name, format->v_union.cases_by_name[mid]->name);
      if (rv == 0)
        return format->v_union.cases_by_name[mid];
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
  else if (part_str_compare (name_len, name, format->v_union.cases_by_name[start]->name) == 0)
    return format->v_union.cases_by_name[start];
  else
    return NULL;
}

BulpFormatUnionCase *
bulp_format_union_lookup_by_value   (BulpFormat *format,
                                     unsigned    value)
{
  assert (format->type == BULP_FORMAT_TYPE_UNION);
  unsigned start = 0, n = format->v_union.n_cases;
  while (n > 1)
    {
      unsigned mid = start + n / 2;
      unsigned mid_value = format->v_union.cases[mid].value;
      if (mid_value == value)
        return format->v_union.cases + mid;
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
  else if (format->v_union.cases[start].value == value)
    return format->v_union.cases + start;
  else
    return NULL;
}

#if BULP_SIZEOF_TINY_ENUM == 4 && BULP_SIZEOF_SHORT_ENUM == 4
static inline uint32_t get_native_case_value (BulpFormat *f, void *native_data)
{
  (void) f;
  return * (uint32_t *) native_data;
}
#else
static inline uint32_t get_native_case_value (BulpFormat *f, void *native_data)
{
  switch (format->v_union.native_type_size) {
    case 1: return * (uint8_t *) native_data;
    case 2: return * (uint16_t *) native_data;
    case 4: return * (uint32_t *) native_data;
    default: assert(0); return 0;
  }
}
#endif

/* --- union virtual functions --- */
static bulp_bool
validate_native__union   (BulpFormat *format,
                          void *native_data,
                          BulpError **error)
{
  uint32_t value = get_native_case_value (format, native_data);
  BulpFormatUnionCase *c = bulp_format_union_lookup_by_value (format, value);
  if (c == NULL)
    {
      *error = bulp_error_new_bad_case_value (value, format->base.canonical_name);
      return BULP_FALSE;
    }
  BulpFormat *sub = c->format;
  if (!sub->base.vfuncs.validate_native (sub, (char*) native_data + format->v_union.native_info_offset, error))
    return BULP_FALSE;
  return BULP_TRUE;
}

static size_t
get_packed_size__union   (BulpFormat *format,
                          void *native_data)
{
  uint32_t value = get_native_case_value (format, native_data);
  BulpFormatUnionCase *c = bulp_format_union_lookup_by_value (format, value);
  BulpFormat *sub = c->format;
  if (sub == NULL)
    return bulp_uint_get_packed_size (value);
  else
    return bulp_uint_get_packed_size (value)
         + sub->base.vfuncs.get_packed_size (sub, (char*) native_data + format->v_union.native_info_offset);
}

static size_t
pack__union              (BulpFormat *format,
                          void *native_data,
                          uint8_t *packed_data_out)
{
  uint32_t value = get_native_case_value (format, native_data);
  BulpFormatUnionCase *c = bulp_format_union_lookup_by_value (format, value);
  uint8_t *at = packed_data_out;
  at += bulp_uint_pack (value, at);
  BulpFormat *sub = c->format;
  if (sub != NULL)
    at += sub->base.vfuncs.pack (sub, (char*)native_data + format->v_union.native_info_offset, at);
  return at - packed_data_out;
}

static void
pack_to__union           (BulpFormat *format,
                          void *native_data,
                          BulpDataBuilder *builder)
{
  uint32_t value = get_native_case_value (format, native_data);
  BulpFormatUnionCase *c = bulp_format_union_lookup_by_value (format, value);
  bulp_uint_pack_to (value, builder);
  BulpFormat *sub = c->format;
  if (sub != NULL)
    sub->base.vfuncs.pack_to (sub, (char*)native_data + format->v_union.native_info_offset, builder);
}

static size_t
unpack__union            (BulpFormat *format,
                          size_t packed_len,
                          const uint8_t *packed_data,
                          void *native_data_out,
                          BulpMemPool *pool,
                          BulpError **error)
{
  uint32_t value;
  size_t at = bulp_uint_unpack (packed_len, packed_data, &value, error);
  BulpFormatUnionCase *c = bulp_format_union_lookup_by_value (format, value);
  if (c == NULL)
    {
      *error = bulp_error_new_bad_case_value (value, format->base.canonical_name);
      return 0;
    }
  if (c->format)
    {
      BulpFormat *sub = c->format;
      void *info = (char*) native_data_out + format->v_union.native_info_offset;
      size_t used = sub->base.vfuncs.unpack (sub, packed_len - at, packed_data - at, info, pool, error);
      if (used == 0)
        return 0;
      at += used;
    }
  return at;
}

static void
destruct_format__union (BulpFormat  *format)
{
  for (unsigned i = 0; i < format->v_union.n_cases; i++)
    {
      BulpFormatUnionCase *ucase = format->v_union.cases + i;
      bulp_format_unref (ucase->format);
      free ((char*) ucase->name);
    }
  free (format->v_union.cases);
  free (format->v_union.cases_by_name);
}

static BulpFormatVFuncs vfuncs__union = BULP_FORMAT_VFUNCS_DEFINE(union);


static int
compare_format_union_cases_by_value (const void *a, const void *b)
{
  const BulpFormatUnionCase *A = a;
  const BulpFormatUnionCase *B = b;
  return (A->value < B->value) ? -1 : (A->value > B->value) ? 1 : 0;
}

static int
compare_ptr_format_union_cases_by_name (const void *a, const void *b)
{
  const BulpFormatUnionCase *const*pA = a;
  const BulpFormatUnionCase *const*pB = b;
  const BulpFormatUnionCase *A = *pA;
  const BulpFormatUnionCase *B = *pB;
  return strcmp(A->name, B->name);
}

BulpFormat *
bulp_format_new_union           (unsigned n_cases,
                                 BulpUnionCase *cases)
{
  BulpFormatUnion *rv = malloc (sizeof (BulpFormatUnion));

  //  compute values
  rv->n_cases = n_cases;
  rv->cases = malloc (sizeof (BulpFormatUnionCase) * n_cases);
  unsigned next_value = 0;
  bulp_bool copy_with_memcpy = BULP_TRUE;
  bulp_bool is_zeroable = BULP_TRUE;
  for (unsigned i = 0; i < n_cases; i++)
    {
      unsigned v = (cases[i].set_value) ?  cases[i].value_if_set : next_value;
      rv->cases[i].value = v;
      rv->cases[i].name = strdup (cases[i].name);
      rv->cases[i].format = cases[i].format == NULL ? NULL : bulp_format_ref (cases[i].format);

      next_value = v + 1;
    }

  // sort values by value
  qsort (rv->cases, rv->n_cases, sizeof (BulpFormatUnionCase), compare_format_union_cases_by_value);

  uint32_t max_case_value = rv->n_cases == 0 ? 0 : rv->cases[rv->n_cases-1].value;
  size_t max_align;
  size_t sizeof_case_enum;
  if (max_case_value < 256)
    {
      sizeof_case_enum = BULP_SIZEOF_TINY_ENUM;
      max_align = BULP_ALIGNOF_TINY_ENUM;
    }
  else if (max_case_value < (1<<16))
    {
      sizeof_case_enum = BULP_SIZEOF_SHORT_ENUM;
      max_align = BULP_ALIGNOF_SHORT_ENUM;
    }
  else
    {
      sizeof_case_enum = BULP_SIZEOF_INT_ENUM;
      max_align = BULP_ALIGNOF_INT_ENUM;
    }
  size_t size = sizeof_case_enum;
  for (unsigned i = 0; i < n_cases; i++)
    {
      BulpFormat *format = cases[i].format;
      if (format != NULL)
        {
          if (!format->base.copy_with_memcpy)
            copy_with_memcpy = BULP_FALSE;
          if (!format->base.is_zeroable)
            is_zeroable = BULP_FALSE;
          size = BULP_MAX(size, sizeof_case_enum + format->base.c_sizeof);
          max_align = BULP_MAX (max_align, format->base.c_alignof);
        }
     }

  // search for dup values
  for (unsigned i = 1; i < n_cases; i++)
    if (rv->cases[i-1].value == rv->cases[i].value)
      {
        bulp_die ("duplicate value in union (%s v %s) (value=%u)",
                  rv->cases[i-1].name, rv->cases[i].name, rv->cases[i].value);
      }

  // compute values_by_name
  rv->cases_by_name = malloc (sizeof (BulpFormatUnionCase *) * n_cases);
  for (unsigned i = 0; i < n_cases; i++)
    rv->cases_by_name[i] = rv->cases + i;
  qsort (rv->cases_by_name, rv->n_cases, sizeof (BulpFormatUnionCase*), compare_ptr_format_union_cases_by_name);

  // search for dup names
  for (unsigned i = 1; i < n_cases; i++)
    if (strcmp (rv->cases_by_name[i-1]->name, rv->cases_by_name[i]->name) == 0)
      {
        bulp_die ("duplicate name in union case (name %s)",
                  rv->cases_by_name[i]->name);
      }

  // initialize rv->base
  rv->base.type = BULP_FORMAT_TYPE_UNION;
  rv->base.ref_count = 1;
  rv->base.canonical_ns = NULL;
  rv->base.canonical_name = NULL;
  /* native (ie equivalent to generated C code) representation */
  rv->base.c_sizeof = bulp_align (size, max_align);
  rv->base.c_alignof = max_align;
  rv->base.copy_with_memcpy = copy_with_memcpy;
  rv->base.is_zeroable = is_zeroable;
  rv->base.c_typename = NULL;
  rv->base.c_func_prefix = NULL;
  rv->base.c_macro_prefix = NULL;
  rv->base.vfuncs = vfuncs__union;

  return (BulpFormat *) rv;
}

