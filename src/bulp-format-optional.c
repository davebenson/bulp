#include "bulp.h"
#include "bulp-internals.h"
#include <string.h>


static bulp_bool
validate_native__optional (BulpFormat *format,
                           void *native_data,
                           BulpError **error)
{
  if (* (void **) native_data == NULL)
    return BULP_TRUE;
  BulpFormat *sub = format->v_optional.subformat;
  if (sub->base.vfuncs.validate_native == NULL)
    return BULP_TRUE;
  return sub->base.vfuncs.validate_native (sub, * (void **) native_data, error);
}

static size_t
get_packed_size__optional (BulpFormat *format,
                           void *native_data)
{
  if (* (void **) native_data == NULL)
    return 1;
  else
    {
      BulpFormat *sub = format->v_optional.subformat;
      return 1 + sub->base.vfuncs.get_packed_size (sub, * (void **) native_data);
    }
}

static size_t
pack__optional            (BulpFormat *format,
                           void *native_data,
                           uint8_t *out)
{
  if (* (void **) native_data == NULL)
    {
      out[0] = 0;
      return 1;
    }
  else
    {
      BulpFormat *sub = format->v_optional.subformat;
      out[0] = 1;
      return 1 + sub->base.vfuncs.pack (sub, * (void **) native_data, out + 1);
    }
}

static void
pack_to__optional         (BulpFormat *format,
                           void *native_data,
                           BulpDataBuilder *builder)
{
  if (* (void **) native_data == NULL)
    {
      bulp_data_builder_append_byte (builder, 0);
    }
  else
    {
      BulpFormat *sub = format->v_optional.subformat;
      bulp_data_builder_append_byte (builder, 1);
      sub->base.vfuncs.pack_to (sub, * (void **) native_data, builder);
    }
}

static size_t
unpack__optional          (BulpFormat *format,
                           size_t packed_len,
                           const uint8_t *packed_data,
                           void *native_data_out,
                           BulpMemPool *pool,
                           BulpError **error)
{
  assert(packed_len > 0);
  if (packed_data[0] == 0)
    {
      * (void **) native_data_out = NULL;
      return 1;
    }
  else if (packed_data[0] != 1)
    {
      *error = bulp_error_new_bad_data ("expected 0 or 1 as first byte of packed optional, got %u", packed_data[0]);
      return 0;
    }
  else
    {
      BulpFormat *sub = format->v_optional.subformat;
      void *subdata = bulp_mem_pool_alloc (pool, sub->base.c_sizeof);
      size_t subrv = sub->base.vfuncs.unpack (sub, packed_len - 1, packed_data + 1, subdata, pool, error);
      if (subrv == 0)
        return 0;
      * (void **) native_data_out = subdata;
      return 1 + subrv;
    }
}
 

static void
destruct_format__optional (BulpFormat  *format)
{
  assert(format->v_optional.subformat == NULL);
}

static BulpFormatVFuncs vfuncs__optional = BULP_FORMAT_VFUNCS_DEFINE(optional);

BulpFormat *bulp_format_optional_of      (BulpFormat *subtype)
{
  if (subtype->base.optional_of == NULL)
    {
      BulpFormatOptional *opt = malloc (sizeof (BulpFormatOptional));
      memset (opt, 0, sizeof (BulpFormatOptional));
      opt->base.type = BULP_FORMAT_TYPE_OPTIONAL;
      opt->base.ref_count = 1;
      opt->base.vfuncs = vfuncs__optional;
      opt->base.c_alignof = BULP_POINTER_ALIGNOF;
      opt->base.c_sizeof = sizeof (void*);
      opt->base.is_zeroable = BULP_TRUE;
      opt->subformat = subtype;
      subtype->base.optional_of = (BulpFormat *) opt;
    }
  return subtype->base.optional_of;
}
