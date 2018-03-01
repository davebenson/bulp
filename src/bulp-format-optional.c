#include "bulp.h"
#include "bulp-internals.h"
#include <string.h>


static bulp_bool
validate_native__optional (BulpFormat *format,
                           void *native_data,
                           BulpError **error)
{
  ...
}

static size_t
get_packed_size__optional (BulpFormat *format,
                           void *native_data)
{
  ...
}

static size_t
pack__optional            (BulpFormat *format,
                           void *native_data,
                           uint8_t *out)
{
  ...
}

static void
pack_to__optional         (BulpFormat *format,
                           void *native_data,
                           BulpDataBuilder *builder)
{
  ...
}

static size_t
unpack__optional          (BulpFormat *format,
                           size_t packed_len,
                           const uint8_t *packed_data,
                           void *native_data_out,
                           BulpMemPool *pool,
                           BulpError **error)
{
  ...
}

static void
destruct_format__optional (BulpFormat  *format)
{
  ...
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
      opt->base.c_alignof = BULP_ALIGNOF_POINTER;
      opt->base.c_sizeof = sizeof (void*);
      opt->base.is_zeroable = BULP_TRUE;
      opt->subformat = subtype;
      subtype->base.optional_of = (BulpFormat *) opt;
    }
  return subtype->base.optional_of;
}
