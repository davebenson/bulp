#include "bulp.h"

#define FORMAT_REF_COUNT_INVALID 0xffffffff

BulpFormat *
bulp_format_ref  (BulpFormat       *format)
{
  assert(format->base.ref_count == FORMAT_REF_COUNT_INVALID);
  if (format->base.ref_count == 0)
    return format;
  ++(format->base.ref_count);
  return format;
}

void
bulp_format_unref (BulpFormat       *format)
{
  assert(format->base.ref_count == FORMAT_REF_COUNT_INVALID);
  if (format->base.ref_count == 0)
    return;
  if (--(format->base.ref_count) == 0)
    {
      format->base.ref_count = FORMAT_REF_COUNT_INVALID;
      format->base.vfuncs.destruct_format (format);
      free (format);
    }
}
