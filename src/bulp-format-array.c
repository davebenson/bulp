#include "bulp.h"
#include "bulp-internals.h"
#include <string.h>

static bulp_bool
validate_native__array (BulpFormat *format,
                        void *native_data,
                        BulpError **error)
{
  BulpArray *array = native_data;
  char *d = array->data;
  BulpFormat *f = format->v_array.element_format;
  for (unsigned i = 0; i < array->length; i++)
    {
      if (!f->base.vfuncs.validate_native (f, d, error))
        {
          bulp_error_append_message (*error, "in array element %u", i);
          return BULP_FALSE;
        }
      d += f->base.c_sizeof;
    }
  return BULP_TRUE;
}

static size_t
get_packed_size__array (BulpFormat *format,
                        void *native_data)
{
  BulpArray *array = native_data;
  char *d = array->data;
  BulpFormat *f = format->v_array.element_format;
  size_t rv = bulp_uint_get_packed_size (array->length);
  for (unsigned i = 0; i < array->length; i++)
    {
      rv += f->base.vfuncs.get_packed_size (f, d);
      d += f->base.c_sizeof;
    }
  return rv;
}

static size_t
pack__array            (BulpFormat *format,
                        void *native_data,
                        uint8_t *out)
{
  BulpArray *array = native_data;
  char *d = array->data;
  BulpFormat *f = format->v_array.element_format;
  size_t rv = bulp_uint_pack (array->length, out);
  for (unsigned i = 0; i < array->length; i++)
    {
      rv += f->base.vfuncs.pack (f, d, out + rv);
      d += f->base.c_sizeof;
    }
  return rv;
}

static void
pack_to__array         (BulpFormat *format,
                        void *native_data,
                        BulpDataBuilder *builder)
{
  BulpArray *array = native_data;
  BulpFormat *f = format->v_array.element_format;
  bulp_uint_pack_to (array->length, builder);
  char *d = array->data;
  for (unsigned i = 0; i < array->length; i++)
    {
      f->base.vfuncs.pack_to (f, d, builder);
      d += f->base.c_sizeof;
    }
}

static size_t
unpack__array          (BulpFormat *format,
                        size_t packed_len,
                        const uint8_t *packed_data,
                        void *native_data_out,
                        BulpMemPool *pool,
                        BulpError **error)
{
  BulpArray *array = native_data_out;
  BulpFormat *f = format->v_array.element_format;
  uint32_t len;
  unsigned at = bulp_uint_unpack (packed_len, packed_data, &len, error);
  if (at == 0)
    return 0;
  array->length = len;
  if (len == 0)
    {
      array->data = NULL;
      return at;
    }
  array->data = bulp_mem_pool_alloc (pool, f->base.c_sizeof * len);
  char *elt = array->data;
  for (unsigned i = 0; i < len; i++)
    {
      unsigned subrv = f->base.vfuncs.unpack (f, packed_len-at, packed_data-at, elt, pool, error);
      if (subrv == 0)
        {
          bulp_error_append_message (*error, "unpacking element %u of array", i);
          return 0;
        }
      at += subrv;
      elt += f->base.c_sizeof;
    }
  return at;
}


static void
destruct_format__array (BulpFormat  *format)
{
  (void) format;
}

static BulpFormatVFuncs vfuncs__array = BULP_FORMAT_VFUNCS_DEFINE(array);

BulpFormat *
bulp_format_array_of (BulpFormat *format)
{
  if (format->base.array_of == NULL)
    {
      BulpFormatArray *aformat = malloc (sizeof (BulpFormatArray));
      memset (aformat, 0, sizeof (BulpFormatArray));
      aformat->base.type = BULP_FORMAT_TYPE_ARRAY;
      aformat->base.ref_count = 1;
      aformat->base.vfuncs = vfuncs__array;
      aformat->base.c_sizeof = sizeof (BulpArray);
      aformat->base.c_alignof = BULP_POINTER_ALIGNOF;           // XXX: improve
      aformat->base.is_zeroable = BULP_TRUE;
      aformat->element_format = format;
      format->base.array_of = (BulpFormat *) aformat;
    }
  return format->base.array_of;
}
