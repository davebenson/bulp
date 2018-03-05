#include "bulp.h"
#include "bulp-internals.h"
#include <string.h>


unsigned
bulp_packed_element_get_native   (BulpFormatPackedElement *elt,
                                  const void        *native_instance)
{
  void *at = (void *) ((char*)native_instance + elt->native_byte_offset);
  switch (elt->native_word_size)
    {
    case 1: return * (uint8_t *) at;
    case 2: return * (uint16_t *) at;
    case 4: return * (uint32_t *) at;
    default: assert(0); return 0;
    }
}

void
bulp_packed_element_set_native   (BulpFormatPackedElement *elt,
                                  void              *native_instance,
                                  unsigned           value)
{
  void *at = (void *) ((char*)native_instance + elt->native_byte_offset);
  switch (elt->native_word_size)
    {
    case 1: * (uint8_t *) at = value; return;
    case 2: * (uint16_t *) at = value; return;
    case 4: * (uint32_t *) at = value; return;
    default: assert(0); 
    }
}
#define validate_native__packed NULL
static size_t
get_packed_size__packed (BulpFormat *format,
                         void *native_data)
{
  (void) native_data;
  BulpFormatPacked *p = (BulpFormatPacked *) format;
  return (p->total_bit_size + 7) / 8;
}

static size_t
pack__packed            (BulpFormat *format,
                         void *native_data,
                         uint8_t *packed_data_out)
{
  BulpFormatPacked *p = (BulpFormatPacked *) format;
  uint8_t *at = packed_data_out;
  uint8_t shift = 0;
  for (unsigned i = 0; i < p->n_elements; i++)
    {
      unsigned v = bulp_packed_element_get_native (p->elements+i, native_data);
      unsigned rem = p->elements[i].n_bits;
      if (shift + rem <= 8)
        {
          uint8_t inv_mask = ((1<<rem)-1) << shift;
          *at &= ~inv_mask;
          *at |= v << shift;
          shift += rem;
          rem = 0;
        }
      else if (shift > 0)
        {
          unsigned w = 8 - shift;
          uint8_t inv_mask = 0xff << shift;
          *at &= ~inv_mask;
          *at |= (v << shift);
          v >>= w;
          rem -= w;
          at++;
          shift = 0;
        }
      while (rem >= 8)
        {
          /// whole bytes
          assert(shift == 0);
          *at++ = v;
          v >>= 8;
          rem -= 8;
        }
      if (rem != 0)
        {
          // last partial byte
          uint8_t invmask = (1 << rem) - 1;
          *at &= ~invmask;
          *at |= v;
          shift = rem;
        }
    }
  if (shift != 0)
    at++;
  return at - packed_data_out;
}

static void
pack_to__packed         (BulpFormat *format,
                         void *native_data,
                         BulpDataBuilder *out)
{
  size_t size = (format->v_packed.total_bit_size + 7) / 8;
  void *packed_data = alloca (size);
  pack__packed (format, native_data, packed_data);
  bulp_data_builder_append (out, size, packed_data);
}

static size_t
unpack__packed          (BulpFormat *format,
                         size_t packed_len,
                         const uint8_t *packed_data,
                         void *native_data_out,
                         BulpMemPool *pool,
                         BulpError **error)
{
  BulpFormatPacked *p = (BulpFormatPacked *) format;
  (void) pool;
  if ((p->total_bit_size + 7) / 8 > packed_len)
    {
      *error = bulp_error_new_too_short ("Packed format data too short");
      return 0;
    }
  const uint8_t *at = packed_data;
  unsigned shift = 0;
  for (unsigned i = 0; i < p->n_elements; i++)
    {
      BulpFormatPackedElement *e = p->elements + i;
      unsigned value;
      if (e->n_bits + shift < 8)
        {
          value = (*at >> shift) & ((1 << e->n_bits) - 1);
          shift += e->n_bits;
        }
      else
        {
          // straddles some bytes (or at least finishes the current byte)
          value = (*at >> shift);
          unsigned value_bits = 8 - shift;
          at++;
          shift = 0;
          while (value_bits + 8 <= e->n_bits)
            {
              value += ((unsigned)*at) << value_bits;
              value_bits += 8;
              at++;
            }
          if (value_bits < e->n_bits)
            {
              unsigned rem = e->n_bits - value_bits;
              value += (((unsigned)*at) & ((1<<rem) - 1)) << value_bits;
              shift = rem;
            }
        }
      bulp_packed_element_set_native (e, native_data_out, value);
    }
  if (shift > 0)
    at++;
  return at - packed_data;
}

static void
destruct_format__packed (BulpFormat  *format)
{
  free (format->v_packed.elements);
  free (format->v_packed.elements_by_name);
}

static BulpFormatVFuncs packed_vfuncs = BULP_FORMAT_VFUNCS_DEFINE(packed);

static int
compare_ptr_packed_element_by_name (const void *a, const void *b)
{
  const BulpFormatPackedElement *const *pa = a;
  const BulpFormatPackedElement *const *pb = b;
  return strcmp ((*pa)->name, (*pb)->name);
}

BulpFormat *
bulp_format_packed_new (size_t n_elts,
                        BulpPackedElement *elts)
{
  BulpFormatPackedElement *elements = malloc (sizeof (BulpFormatPackedElement) * n_elts);
  BulpFormatPackedElement **by_name = malloc (sizeof (BulpFormatPackedElement) * n_elts);
  size_t total_bit_size = 0;
  size_t cur_offset = 0;
  size_t max_align = 0;
  for (unsigned i = 0; i < n_elts; i++)
    {
      if (elts[i].name_len > 0)
        {
          char *n = malloc (elts[i].name_len + 1);
          memcpy (n, elts[i].name, elts[i].name_len);
          n[elts[i].name_len] = 0;
          elements[i].name = n;
        }
      else
        elements[i].name = strdup (elts[i].name);
      elements[i].n_bits = elts[i].n_bits;
      total_bit_size += elements[i].n_bits;
      if (elts[i].n_bits <= 8)
        {
          elements[i].native_word_size = 1;
          elements[i].native_byte_offset = cur_offset++;
          max_align = BULP_MAX (max_align, 1);
        }
      else if (elts[i].n_bits <= 16)
        {
          elements[i].native_word_size = 2;
          cur_offset = bulp_align (cur_offset, BULP_INT16_ALIGNOF);
          elements[i].native_byte_offset = cur_offset;
          cur_offset += 2;
          max_align = BULP_MAX (max_align, BULP_INT16_ALIGNOF);
        }
      else if (elts[i].n_bits <= 32)
        {
          elements[i].native_word_size = 4;
          cur_offset = bulp_align (cur_offset, BULP_INT32_ALIGNOF);
          elements[i].native_byte_offset = cur_offset;
          cur_offset += 4;
          max_align = BULP_MAX (max_align, BULP_INT32_ALIGNOF);
        }
      else
        assert(0);

      by_name[i] = elements + i;
    }
  qsort (by_name, n_elts, sizeof (BulpFormatPackedElement *),
         compare_ptr_packed_element_by_name);
  for (unsigned i = 1; i < n_elts; i++)
    if (strcmp (by_name[i-1]->name, by_name[i]->name) == 0)
      {
        bulp_warn ("duplicate element name");
        free (elements);
        free (by_name);
        return NULL;
      }

  cur_offset = (cur_offset + max_align - 1) & ~(max_align - 1);

  BulpFormatPacked *rv = malloc (sizeof (BulpFormatPacked));
  rv->base.type = BULP_FORMAT_TYPE_PACKED;
  rv->base.ref_count = 1;
  rv->base.vfuncs = packed_vfuncs;
  rv->base.canonical_ns = NULL;
  rv->base.canonical_name = NULL;
  rv->base.c_alignof = max_align;
  rv->base.c_sizeof = cur_offset;
  rv->base.copy_with_memcpy = BULP_TRUE;
  rv->base.is_zeroable = BULP_TRUE;
  rv->base.c_typename = NULL;
  rv->base.c_func_prefix = NULL;
  rv->base.c_macro_prefix = NULL;
  rv->n_elements = n_elts;
  rv->elements = elements;
  rv->elements_by_name = by_name;
  rv->total_bit_size = total_bit_size;
  return (BulpFormat *) rv;
}

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

BulpFormatPackedElement *
bulp_format_packed_lookup_element  (BulpFormat *format,
                                    ssize_t name_len,
                                    const char *name)
{
  unsigned start = 0, n = format->v_packed.n_elements;
  while (n > 1)
    {
      unsigned mid = start + n / 2;
      int rv = part_str_compare (name_len, name, format->v_packed.elements_by_name[mid]->name);
      if (rv == 0)
        return format->v_packed.elements_by_name[mid];
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
  else if (part_str_compare (name_len, name, format->v_packed.elements_by_name[start]->name) == 0)
    return format->v_packed.elements_by_name[start];
  else
    return NULL;
}
