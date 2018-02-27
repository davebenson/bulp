#include "bulp.h"
#include "bulp-internals.h"
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
      if (format->v_enum.values[mid] == value)
        return format->v_enum.values + mid;
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
  else if (format->v_enum.values[start].value == value)
    return format->v_enum.values + start;
  else
    return NULL;
}

BulpFormat *
bulp_format_new_enum         (unsigned n_values,
                              BulpEnumValue *values)
{
  BulpFormatEnum *rv = malloc (sizeof (BulpFormatEnum));

  //  compute values
  rv->values = malloc (sizeof (BulpFormatEnumValue) * n_values);

  // sort values by value
  ...

  // search for dup values
  ...

  // compute values_by_name
  ...

  // search for dup names
  ...

  // initialize rv->base
  ...

  return (BulpFormat *) rv;
}
