#include "bulp.h"

#define COMPARE_RETURN_PM1(a, b)               \
  do{ if ((a) < (b)) return -1;                \
      if ((a) > (b)) return +1; } while (0)

/* These functions actually work for length-prefixed strings just as effectively */
static int
compare__binary_data (BulpComparator *comparator,
                      const uint8_t *a_data,
                      const uint8_t *b_data)
{
  (void) comparator;

  uint32_t a_len, b_len;
  size_t a_lenlen = bulp_uint_unpack_unsafe (a_data, &a_len);
  size_t b_lenlen = bulp_uint_unpack_unsafe (b_data, &b_len);
  size_t min_len = BULP_MIN (a_len, b_len);
  int memcmp_rv = memcmp (a_data + a_lenlen, b_data + b_lenlen, min_len);
  if (memcmp_rv == 0)
    return (a_len < b_len) ? -1 : (a_len > b_len) ? 1 :
           (a_lenlen < b_lenlen) ? -1 : (a_lenlen > b_lenlen) ? 1 : 0;
  else
    return memcmp_rv;
}
static int
compare2__binary_data (BulpComparator *comparator,
                       const uint8_t *a_data,
                       const uint8_t *b_data,
                       size_t        *len_out)
{
  (void) comparator;

  uint32_t a_len, b_len;
  size_t a_lenlen = bulp_uint_unpack_unsafe (a_data, &a_len);
  size_t b_lenlen = bulp_uint_unpack_unsafe (b_data, &b_len);
  size_t min_len = BULP_MIN (a_len, b_len);
  int memcmp_rv = memcmp (a_data + a_lenlen, b_data + b_lenlen, min_len);
  if (memcmp_rv == 0)
    {
      COMPARE_RETURN_PM1(a_len, b_len);
      COMPARE_RETURN_PM1(a_lenlen, b_lenlen);
      *len_out = a_lenlen + a_len;
      return 0;
    }
  else
    return memcmp_rv;
}

BulpComparator *
bulp_comparator_new_protected (size_t                 sizeof_comparator,
                               BulpFormat *format,
                               BulpComparatorCompare func,
                               BulpComparatorCompare2 func2,
                               BulpComparatorDestroy destroy)
{
  assert (sizeof_comparator >= sizeof (BulpComparator));
  assert (destroy != NULL);
  BulpComparator *rv = malloc (sizeof_comparator);
  rv->ref_count = 1;
  rv->format = bulp_format_ref (format);
  rv->compare = func;
  rv->compare2 = func2;
  rv->destroy = destroy;
  return rv;
}

static int
compare__string0     (BulpComparator *comparator,
                      const uint8_t *a_data,
                      const uint8_t *b_data)
{
  (void) comparator;
  return strcmp ((char*) a_data, (char *) b_data);
}
static int
compare2__string0     (BulpComparator *comparator,
                       const uint8_t *a_data,
                       const uint8_t *b_data,
                       size_t        *len_out)
{
  (void) comparator;
  size_t len = 0;
  while (*a_data && *b_data)
    {
      int rv = (int) *a_data++ - (int) *b_data++;
      if (rv != 0)
        return rv;
      len++;
    }
  int rv = (int) *a_data - (int) *b_data;
  if (rv == 0)
    *len_out = len + 1;         // length includes terminating NUL
  return rv;
}

static BulpComparatorCompare compare__string__funcs[2] = {
  compare__string0,
  compare__binary_data
};
static BulpComparatorCompare2 compare2__string__funcs[2] = {
  compare2__string0,
  compare2__binary_data
};

/* Variable-length integers */
static int compare__unsigned_varint (BulpComparator *comparator,
                                     const uint8_t  *a,
                                     const uint8_t  *b)
{
  (void) comparator;
  uint64_t aa, bb;
  bulp_ulong_unpack_unsafe (a, &aa);
  bulp_ulong_unpack_unsafe (b, &bb);
  return (aa < bb) ? -1 : (aa > bb) ? 1 : 0;
}
static int compare2__unsigned_varint (BulpComparator *comparator,
                                     const uint8_t  *a,
                                     const uint8_t  *b,
                                     size_t         *len_out)
{
  (void) comparator;
  uint64_t aa, bb;
  unsigned all = bulp_ulong_unpack_unsafe (a, &aa);
  unsigned bll = bulp_ulong_unpack_unsafe (b, &bb);
  int rv = (aa < bb) ? -1 : (aa > bb) ? 1 :
           (all < bll) ? -1 : (all > bll) ? 1 : 0;
  if (rv == 0)
    *len_out = all;
  return rv;
}
static int compare__signed_varint (BulpComparator *comparator,
                                   const uint8_t  *a,
                                   const uint8_t  *b)
{
  (void) comparator;
  int64_t aa, bb;
  bulp_long_unpack_unsafe (a, &aa);
  bulp_long_unpack_unsafe (b, &bb);
  return (aa < bb) ? -1 : (aa > bb) ? 1 : 0;
}
static int compare2__signed_varint (BulpComparator *comparator,
                                    const uint8_t  *a,
                                    const uint8_t  *b,
                                    size_t         *len_out)
{
  (void) comparator;
  int64_t aa, bb;
  unsigned all = bulp_long_unpack_unsafe (a, &aa);
  unsigned bll = bulp_long_unpack_unsafe (b, &bb);
  int rv = (aa < bb) ? -1 : (aa > bb) ? 1 :
           (all < bll) ? -1 : (all > bll) ? 1 : 0;
  if (rv == 0)
    *len_out = all;
  return rv;
}
static BulpComparatorCompare compare__varint__funcs[2] = {
  compare__unsigned_varint,
  compare__signed_varint,
};
static BulpComparatorCompare2 compare2__varint__funcs[2] = {
  compare2__unsigned_varint,
  compare2__signed_varint,
};

static int compare__uint8 (BulpComparator *comparator,
                           const uint8_t  *a,
                           const uint8_t  *b)
{
  (void) comparator;
  COMPARE_RETURN_PM1(*a, *b);
  return 0;
}
static int compare2__uint8 (BulpComparator *comparator,
                            const uint8_t  *a,
                            const uint8_t  *b,
                            size_t         *size_out)
{
  (void) comparator;
  COMPARE_RETURN_PM1(*a, *b);
  *size_out = 1;
  return 0;
}
static int compare__int8 (BulpComparator *comparator,
                          const uint8_t  *a,
                          const uint8_t  *b)
{
  (void) comparator;
  COMPARE_RETURN_PM1(*a ^ 0x80, *b ^ 0x80);
  return 0;
}
static int compare2__int8 (BulpComparator *comparator,
                           const uint8_t  *a,
                           const uint8_t  *b,
                           size_t         *size_out)
{
  (void) comparator;
  COMPARE_RETURN_PM1(*a ^ 0x80, *b ^ 0x80);
  *size_out = 1;
  return 0;
}


static int compare__uint16 (BulpComparator *comparator,
                            const uint8_t  *a,
                            const uint8_t  *b)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[1], b[1]);
  COMPARE_RETURN_PM1(a[0], b[0]);
  return 0;
}

static int compare2__uint16 (BulpComparator *comparator,
                             const uint8_t  *a,
                             const uint8_t  *b,
                             size_t         *size_out)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[1], b[1]);
  COMPARE_RETURN_PM1(a[0], b[0]);
  *size_out = 2;
  return 0;
}

static int compare__int16 (BulpComparator *comparator,
                           const uint8_t  *a,
                           const uint8_t  *b)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[1] ^ 0x80, b[1] ^ 0x80);
  COMPARE_RETURN_PM1(a[0], b[0]);
  return 0;
}

static int compare2__int16 (BulpComparator *comparator,
                            const uint8_t  *a,
                            const uint8_t  *b,
                            size_t         *size_out)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[1] ^ 0x80, b[1] ^ 0x80);
  COMPARE_RETURN_PM1(a[0], b[0]);
  *size_out = 2;
  return 0;
}
static int compare__uint32 (BulpComparator *comparator,
                            const uint8_t  *a,
                            const uint8_t  *b)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[3], b[3]);
  COMPARE_RETURN_PM1(a[2], b[2]);
  COMPARE_RETURN_PM1(a[1], b[1]);
  COMPARE_RETURN_PM1(a[0], b[0]);
  return 0;
}

static int compare2__uint32 (BulpComparator *comparator,
                             const uint8_t  *a,
                             const uint8_t  *b,
                             size_t         *size_out)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[3], b[3]);
  COMPARE_RETURN_PM1(a[2], b[2]);
  COMPARE_RETURN_PM1(a[1], b[1]);
  COMPARE_RETURN_PM1(a[0], b[0]);
  *size_out = 4;
  return 0;
}

static int compare__int32 (BulpComparator *comparator,
                           const uint8_t  *a,
                           const uint8_t  *b)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[3] ^ 0x80, b[3] ^ 0x80);
  COMPARE_RETURN_PM1(a[2], b[2]);
  COMPARE_RETURN_PM1(a[1], b[1]);
  COMPARE_RETURN_PM1(a[0], b[0]);
  return 0;
}

static int compare2__int32 (BulpComparator *comparator,
                            const uint8_t  *a,
                            const uint8_t  *b,
                            size_t         *size_out)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[3] ^ 0x80, b[3] ^ 0x80);
  COMPARE_RETURN_PM1(a[2], b[2]);
  COMPARE_RETURN_PM1(a[1], b[1]);
  COMPARE_RETURN_PM1(a[0], b[0]);
  *size_out = 4;
  return 0;
}


static int compare__uint64 (BulpComparator *comparator,
                            const uint8_t  *a,
                            const uint8_t  *b)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[7], b[7]);
  COMPARE_RETURN_PM1(a[6], b[6]);
  COMPARE_RETURN_PM1(a[5], b[5]);
  COMPARE_RETURN_PM1(a[4], b[4]);
  COMPARE_RETURN_PM1(a[3], b[3]);
  COMPARE_RETURN_PM1(a[2], b[2]);
  COMPARE_RETURN_PM1(a[1], b[1]);
  COMPARE_RETURN_PM1(a[0], b[0]);
  return 0;
}

static int compare2__uint64 (BulpComparator *comparator,
                             const uint8_t  *a,
                             const uint8_t  *b,
                             size_t         *size_out)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[7], b[7]);
  COMPARE_RETURN_PM1(a[6], b[6]);
  COMPARE_RETURN_PM1(a[5], b[5]);
  COMPARE_RETURN_PM1(a[4], b[4]);
  COMPARE_RETURN_PM1(a[3], b[3]);
  COMPARE_RETURN_PM1(a[2], b[2]);
  COMPARE_RETURN_PM1(a[1], b[1]);
  COMPARE_RETURN_PM1(a[0], b[0]);
  *size_out = 8;
  return 0;
}

static int compare__int64 (BulpComparator *comparator,
                           const uint8_t  *a,
                           const uint8_t  *b)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[7] ^ 0x80, b[7] ^ 0x80);
  COMPARE_RETURN_PM1(a[6], b[6]);
  COMPARE_RETURN_PM1(a[5], b[5]);
  COMPARE_RETURN_PM1(a[4], b[4]);
  COMPARE_RETURN_PM1(a[3], b[3]);
  COMPARE_RETURN_PM1(a[2], b[2]);
  COMPARE_RETURN_PM1(a[1], b[1]);
  COMPARE_RETURN_PM1(a[0], b[0]);
  return 0;
}

static int compare2__int64 (BulpComparator *comparator,
                            const uint8_t  *a,
                            const uint8_t  *b,
                            size_t         *size_out)
{
  (void) comparator;
  COMPARE_RETURN_PM1(a[7] ^ 0x80, b[7] ^ 0x80);
  COMPARE_RETURN_PM1(a[6], b[6]);
  COMPARE_RETURN_PM1(a[5], b[5]);
  COMPARE_RETURN_PM1(a[4], b[4]);
  COMPARE_RETURN_PM1(a[3], b[3]);
  COMPARE_RETURN_PM1(a[2], b[2]);
  COMPARE_RETURN_PM1(a[1], b[1]);
  COMPARE_RETURN_PM1(a[0], b[0]);
  *size_out = 8;
  return 0;
}


static BulpComparatorCompare
compare__fixedint__funcs[2][4] = {
  {
    compare__uint8,
    compare__uint16,
    compare__uint32,
    compare__uint64
  },
  {
    compare__int8,
    compare__int16,
    compare__int32,
    compare__int64
  },
};
static BulpComparatorCompare2
compare2__fixedint__funcs[2][4] = {
  {
    compare2__uint8,
    compare2__uint16,
    compare2__uint32,
    compare2__uint64
  },
  {
    compare2__int8,
    compare2__int16,
    compare2__int32,
    compare2__int64
  },
};
void
bulp_comparator_destroy_protected (BulpComparator *comparator)
{
  bulp_format_unref (comparator->format);
  free (comparator);
}

static inline BulpComparator *
simple_comparator (BulpFormat *format,
                   BulpComparatorCompare func,
                   BulpComparatorCompare2 func2)
{
  return bulp_comparator_new_protected (sizeof (BulpComparator),
                                        format,
                                        func,
                                        func2,
                                        bulp_comparator_destroy_protected);
}

static int
compare__packed (BulpComparator *comparator,
                 const uint8_t *a_data,
                 const uint8_t *b_data)
{
  unsigned n_elements = comparator->format->v_packed.n_elements;
  BulpFormatPackedElement *elements = comparator->format->v_packed.elements;
  unsigned shift = 0;
  unsigned at = 0;
  for (unsigned i = 0; i < n_elements; i++)
    {
      unsigned rem = elements[i].n_bits;
      uint32_t a_value, b_value;
      if ((8 - shift) >= rem)
        {
          // put remainder of byte into a_value/b_value
          a_value = a_data[at] >> shift;
          b_value = b_data[at] >> shift;

          rem -= 8 - shift;
          unsigned value_shift = 8 - shift;
          shift = 0;
          at++;
          while (rem >= 8)
            {
              a_value |= (uint32_t)a_data[at] << value_shift;
              b_value |= (uint32_t)b_data[at] << value_shift;
              value_shift += 8;
              at++;
              rem -= 8;
            }
          if (rem > 0)
            {
              a_value = ((uint32_t)a_data[at] & ((1 << rem) - 1)) << value_shift;
              b_value = ((uint32_t)b_data[at] & ((1 << rem) - 1)) << value_shift;
              shift += rem;
            }
        }
      else
        {
          a_value = (a_data[at] >> shift) & ((1<<rem) - 1);
          b_value = (b_data[at] >> shift) & ((1<<rem) - 1);
          shift += rem;
          if (shift == 8)
            {
              shift = 0;
              at++;
            }
        }
      if (a_value < b_value) return -1;
      if (a_value > b_value) return +1;
    }
  return 0;
}
static int
compare2__packed (BulpComparator *comparator,
                  const uint8_t *a_data,
                  const uint8_t *b_data,
                  size_t *len_out)
{
  int rv = compare__packed (comparator, a_data, b_data);
  if (rv == 0)
    *len_out = (comparator->format->v_packed.total_bit_size + 7) / 8;
  return rv;
}


static int
compare__float32 (BulpComparator *comparator,
                  const uint8_t *a_data,
                  const uint8_t *b_data)
{
  (void) comparator;
  float a,b;
  bulp_float32_unpack_unsafe (a_data, &a);
  bulp_float32_unpack_unsafe (b_data, &b);
  COMPARE_RETURN_PM1(a, b);
  return 0;
}
static int
compare2__float32 (BulpComparator *comparator,
                  const uint8_t *a_data,
                  const uint8_t *b_data,
                  size_t *len_out)
{
  (void) comparator;
  float a,b;
  bulp_float32_unpack_unsafe (a_data, &a);
  bulp_float32_unpack_unsafe (b_data, &b);
  COMPARE_RETURN_PM1(a, b);
  *len_out = 4;
  return 0;
}

static int
compare__float64 (BulpComparator *comparator,
                  const uint8_t *a_data,
                  const uint8_t *b_data)
{
  (void) comparator;
  double a,b;
  bulp_float64_unpack_unsafe (a_data, &a);
  bulp_float64_unpack_unsafe (b_data, &b);
  COMPARE_RETURN_PM1(a, b);
  return 0;
}
static int
compare2__float64 (BulpComparator *comparator,
                  const uint8_t *a_data,
                  const uint8_t *b_data,
                  size_t *len_out)
{
  (void) comparator;
  double a,b;
  bulp_float64_unpack_unsafe (a_data, &a);
  bulp_float64_unpack_unsafe (b_data, &b);
  COMPARE_RETURN_PM1(a, b);
  *len_out = 8;
  return 0;
}


static BulpComparatorCompare
compare__float__funcs[2] = {
  compare__float32,
  compare__float64,
};

static BulpComparatorCompare2
compare2__float__funcs[2] = {
  compare2__float32,
  compare2__float64,
};
  
BulpComparator *bulp_comparator_new_auto         (BulpFormat      *format)
{
  switch (format->type)
    {
    case BULP_FORMAT_TYPE_BINARY_DATA:
      return simple_comparator (format, compare__binary_data, compare2__binary_data);
    case BULP_FORMAT_TYPE_PACKED:
      return simple_comparator (format, compare__packed, compare2__packed);
    case BULP_FORMAT_TYPE_STRING:
      {
        BulpStringLengthType lt = format->v_string.length_type;
        return simple_comparator (format,
                                  compare__string__funcs[lt],
                                  compare2__string__funcs[lt]);
      }
      
    case BULP_FORMAT_TYPE_ENUM:
      return simple_comparator (format, compare__unsigned_varint, compare2__unsigned_varint);
    case BULP_FORMAT_TYPE_INT:
      {
        int s = format->v_int.is_signed ? 1 : 0;
        if (format->v_int.is_b128)
          return simple_comparator (format,
                                    compare__varint__funcs[s],
                                    compare2__varint__funcs[s]);
        else
          {
            int sz = format->v_int.log2_byte_size;
            return simple_comparator (format,
                                      compare__fixedint__funcs[s][sz],
                                      compare2__fixedint__funcs[s][sz]);
          }
      }
  
  switch (format->type)
    {
    case BULP_FORMAT_TYPE_BINARY_DATA:
      return simple_comparator (format, compare__binary_data, compare2__binary_data);
    case BULP_FORMAT_TYPE_PACKED:
      return simple_comparator (format, compare__packed, compare2__packed);
    case BULP_FORMAT_TYPE_STRING:
      {
        BulpStringLengthType lt = format->v_string.length_type;
        return simple_comparator (format,
                                  compare__string__funcs[lt],
                                  compare2__string__funcs[lt]);
      }
      
    case BULP_FORMAT_TYPE_ENUM:
      return simple_comparator (format, compare__unsigned_varint, compare2__unsigned_varint);
    case BULP_FORMAT_TYPE_INT:
      {
        int s = format->v_int.is_signed ? 1 : 0;
        if (format->v_int.is_b128)
          return simple_comparator (format,
                                    compare__varint__funcs[s],
                                    compare2__varint__funcs[s]);
        else
          {
            int b = format->v_int.log2_byte_size;
            return simple_comparator (format,
                                      compare__fixedint__funcs[s][b],
                                      compare2__fixedint__funcs[s][b]);
          }
      }
    case BULP_FORMAT_TYPE_FLOAT:
      {
        BulpFloatType ft = format->v_float.float_type;
        return simple_comparator (format,
                                  compare__float__funcs[ft],
                                  compare2__float__funcs[ft]);
      }

    case BULP_FORMAT_TYPE_STRUCT:
      if (format->v_struct.is_message)
        {
          // NOTE: messages NOT recommended for keys
          ...
        }
      else
        {
          // lexical order
          ...
        }

    case BULP_FORMAT_TYPE_UNION:
      {
        uint32_t a_case_value, b_case_value;
        unsigned a_cv_lenlen = bulp_uint32_unpack_unsafe
        //...
      }
    case BULP_FORMAT_TYPE_OPTIONAL:
      {
        //...
      }
    case BULP_FORMAT_TYPE_ARRAY:
      {
        //...
      }
    }
}
