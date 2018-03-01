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

BulpFormatStructMember *
bulp_format_struct_lookup_by_name  (BulpFormat *format,
                                    ssize_t     name_len,
                                    const char *name)
{
  assert (format->type == BULP_FORMAT_TYPE_STRUCT);
  unsigned start = 0, n = format->v_struct.n_members;
  while (n > 1)
    {
      unsigned mid = start + n / 2;
      int rv = part_str_compare (name_len, name, format->v_struct.members_by_name[mid]->name);
      if (rv == 0)
        return format->v_struct.members_by_name[mid];
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
  else if (part_str_compare (name_len, name, format->v_struct.members_by_name[start]->name) == 0)
    return format->v_struct.members_by_name[start];
  else
    return NULL;
}

BulpFormatStructMember *
bulp_format_message_lookup_by_value (BulpFormat *format,
                                     unsigned    value)
{
  assert (format->type == BULP_FORMAT_TYPE_STRUCT);
  assert (format->v_struct.is_message);
  unsigned start = 0, n = format->v_struct.n_members;
  while (n > 1)
    {
      unsigned mid = start + n / 2;
      unsigned mid_value = format->v_struct.members[mid].value;
      if (mid_value == value)
        return format->v_struct.members + mid;
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
  else if (format->v_struct.members[start].value == value)
    return format->v_struct.members + start;
  else
    return NULL;
}

/* --- extensive structure (message) vfuncs --- */
static bulp_bool
validate_native__message (BulpFormat *format,
                          void *native_data,
                          BulpError **error)
{
  for (unsigned i = 0; i < format->v_struct.n_members; i++)
    {
      BulpFormatStructMember *member = format->v_struct.members + i;
      void *member_data = (char*)native_data + member->native_offset;
      BulpFormat *member_format = member->format;
      if (member_format->base.vfuncs.validate_native != NULL
       && !member_format->base.vfuncs.validate_native (member_format, member_data, error))
        {
          bulp_error_append_message (*error, " (in member %s of %s)",
                                     member->name, format->base.canonical_name);
          return BULP_FALSE;
        }
    }
  return BULP_TRUE;
}

static size_t
get_packed_size__message (BulpFormat *format,
                          void *native_data)
{
  size_t rv = 0;
  for (unsigned i = 0; i < format->v_struct.n_members; i++)
    {
      // encode member value
      rv += bulp_uint32_get_packed_size (format->v_struct.members[i].value + 1);

      BulpFormatStructMember *member = &format->v_struct.members[i];
      BulpFormat *subformat = member->format;
      char *native_member = (char*) native_data + member->native_offset;
      // find packed length
      size_t sublen = subformat->base.vfuncs.get_packed_size (subformat, native_member);
      // add encoded packed length
      size_t sublenlen = bulp_uint32_get_packed_size (sublen);
      rv += sublen + sublenlen;
    }
  // add terminal NUL
  return rv + 1;
}

static size_t
pack__message            (BulpFormat *format,
                          void *native_data,
                          uint8_t *packed_data_out)
{
  size_t rv = 0;
  for (unsigned i = 0; i < format->v_struct.n_members; i++)
    {
      BulpFormatStructMember *member = format->v_struct.members + i;
      void *member_data = (char*) native_data + member->native_offset;

      // encode member value
      rv += bulp_uint32_pack (format->v_struct.members[i].value + 1, packed_data_out + rv);
      
      BulpFormat *subformat = member->format;
      unsigned subsize = subformat->base.vfuncs.pack (subformat, member_data, packed_data_out + rv + 1);
      uint8_t lenbuf[10];
      uint8_t lenlen = bulp_uint32_pack (subsize, lenbuf);
      if (lenlen != 1)
        memmove (packed_data_out + rv + lenlen,
                 packed_data_out + rv + 1,
                 subsize);
      memcpy (packed_data_out + rv, lenbuf, lenlen);
      rv += lenlen + subsize;
    }
  packed_data_out[rv++] = 0;
  return rv;
}

static void
pack_to__message         (BulpFormat *format,
                          void *native_data,
                          BulpDataBuilder *builder)
{
  for (unsigned i = 0; i < format->v_struct.n_members; i++)
    {
      BulpFormatStructMember *member = format->v_struct.members + i;
      void *member_data = (char*) native_data + member->native_offset;

      // encode member value
      bulp_uint32_pack_to (format->v_struct.members[i].value + 1, builder);

      BulpDataBuilderPiece *lenbufpiece = bulp_data_builder_append_placeholder (builder);

      size_t start_size = builder->cur_len;
      BulpFormat *subformat = member->format;
      subformat->base.vfuncs.pack_to (subformat, member_data, builder);
      size_t sub_size = builder->cur_len - start_size;

      uint8_t lenbuf[10];
      uint8_t lenlen = bulp_uint32_pack (sub_size, lenbuf);
      bulp_data_builder_update_placeholder (builder, lenbufpiece, lenlen, lenbuf);
    }
  bulp_data_builder_append_byte (builder, 0);
}

static size_t
unpack__message          (BulpFormat *format,
                          size_t packed_len,
                          const uint8_t *packed_data,
                          void *native_data_out,
                          BulpMemPool *pool,
                          BulpError **error)
{
  size_t at = 0;
  while (at < packed_len && packed_data[at] != 0)
    {
      uint32_t tag, member_size;
      unsigned tsize = bulp_uint32_unpack (packed_len - at, packed_data - at, &tag, error);
      if (tsize == 0)
        return 0;
      at += tsize;
      if (at >= packed_len)
        {
          *error = bulp_error_new_too_short("error parsing message member len");
          return 0;
        }
      unsigned ssize = bulp_uint32_unpack (packed_len - at, packed_data - at, &member_size, error);
      if (ssize == 0)
        return 0;
      at += ssize;
      if (at + member_size > packed_len)
        {
          *error = bulp_error_new_too_short("message member data as given by prefixed-length");
          return 0;
        }
      BulpFormatStructMember *member = bulp_format_message_lookup_by_value (format, tag);
      if (member != NULL)
        {
          /* valid members should be unpacked; invalid are skipped */
          BulpFormat *fformat = member->format;
          void *member_data = (char*) native_data_out + member->native_offset;
          size_t subsize = fformat->base.vfuncs.unpack (fformat,
                                                        packed_len - at,
                                                        packed_data + at,
                                                        member_data,
                                                        pool, error);
          if (subsize == 0)
            {
              if (format->base.canonical_name != NULL)
                bulp_error_append_message (*error, " (in member %u of %s)",
                                           tag, format->base.canonical_name);
              return 0;
            }
        }
      at += member_size;
    }
  if (at >= packed_len)
    {
      *error = bulp_error_new_too_short("error parsing message in between members");
      return 0;
    }
  return at + 1;        // skip nul
}

static void
destruct_format__message (BulpFormat  *format)
{
  for (unsigned i = 0; i < format->v_struct.n_members; i++)
    {
      BulpFormatStructMember *member = format->v_struct.members + i;
      bulp_format_unref (member->format);
      free ((char*) member->name);
    }
  free (format->v_struct.members);
  free (format->v_struct.members_by_name);
}

static BulpFormatVFuncs vfuncs__message = BULP_FORMAT_VFUNCS_DEFINE(message);

/* --- non-extensible structure (message) vfuncs --- */
#define validate_native__struct validate_native__message
static size_t
get_packed_size__struct (BulpFormat *format,
                          void *native_data)
{
  size_t rv = 0;
  for (unsigned i = 0; i < format->v_struct.n_members; i++)
    {
      BulpFormatStructMember *member = &format->v_struct.members[i];
      BulpFormat *subformat = member->format;
      char *native_member = (char*) native_data + member->native_offset;
      rv += subformat->base.vfuncs.get_packed_size (subformat, native_member);
    }
  return rv;
}

static size_t
pack__struct             (BulpFormat *format,
                          void *native_data,
                          uint8_t *packed_data_out)
{
  size_t rv = 0;
  for (unsigned i = 0; i < format->v_struct.n_members; i++)
    {
      BulpFormatStructMember *member = format->v_struct.members + i;
      char *native_member = (char*) native_data + member->native_offset;
      BulpFormat *subformat = member->format;
      rv += subformat->base.vfuncs.pack (subformat, native_member, packed_data_out + rv);
    }
  return rv;
}

static void
pack_to__struct          (BulpFormat *format,
                          void *native_data,
                          BulpDataBuilder *builder)
{
  for (unsigned i = 0; i < format->v_struct.n_members; i++)
    {
      BulpFormatStructMember *member = format->v_struct.members + i;
      void *member_data = (char*) native_data + member->native_offset;
      BulpFormat *subformat = member->format;
      subformat->base.vfuncs.pack_to (subformat, member_data, builder);
    }
}

static size_t
unpack__struct           (BulpFormat *format,
                          size_t packed_len,
                          const uint8_t *packed_data,
                          void *native_data_out,
                          BulpMemPool *pool,
                          BulpError **error)
{
  size_t at = 0;
  for (unsigned i = 0; i < format->v_struct.n_members; i++)
    {
      /* valid members should be unpacked; invalid are skipped */
      BulpFormatStructMember *member = format->v_struct.members + i;
      BulpFormat *member_format = member->format;
      void *member_data = (char*) native_data_out + member->native_offset;
      size_t subsize = member_format->base.vfuncs.unpack (member_format,
                                                          packed_len - at,
                                                          packed_data + at,
                                                          member_data,
                                                          pool, error);
      if (subsize == 0)
        {
          if (format->base.canonical_name != NULL)
            bulp_error_append_message (*error, " (in member %s of struct %s)",
                                       member->name, format->base.canonical_name);
          return 0;
        }
      at += subsize;
    }
  if (at >= packed_len)
    {
      *error = bulp_error_new_too_short("error parsing message in between members");
      return 0;
    }
  return at + 1;        // skip nul
}

static void
destruct_format__struct (BulpFormat  *format)
{
  for (unsigned i = 0; i < format->v_struct.n_members; i++)
    {
      BulpFormatStructMember *member = format->v_struct.members + i;
      bulp_format_unref (member->format);
      free ((char*) member->name);
    }
  free (format->v_struct.members);
  free (format->v_struct.members_by_name);
}

static BulpFormatVFuncs vfuncs__struct = BULP_FORMAT_VFUNCS_DEFINE(struct);


static int
compare_format_message_members_by_value (const void *a, const void *b)
{
  const BulpFormatStructMember *A = a;
  const BulpFormatStructMember *B = b;
  return (A->value < B->value) ? -1 : (A->value > B->value) ? 1 : 0;
}

static int
compare_ptr_format_message_members_by_name (const void *a, const void *b)
{
  const BulpFormatStructMember *const*pA = a;
  const BulpFormatStructMember *const*pB = b;
  const BulpFormatStructMember *A = *pA;
  const BulpFormatStructMember *B = *pB;
  return strcmp(A->name, B->name);
}

BulpFormat *
bulp_format_new_struct          (unsigned n_members,
                                 BulpStructMember *members,
                                 bulp_bool is_message)
{
  BulpFormatStruct *rv = malloc (sizeof (BulpFormatStruct));

  //  compute values
  rv->n_members = n_members;
  rv->members = malloc (sizeof (BulpFormatStructMember) * n_members);
  rv->is_message = is_message;
  unsigned next_value = 0;
  bulp_bool copy_with_memcpy = BULP_TRUE;
  bulp_bool is_zeroable = BULP_TRUE;
  size_t size = 0;
  size_t max_align = 1;
  for (unsigned i = 0; i < n_members; i++)
    {
      unsigned v = (members[i].set_value) ?  members[i].value_if_set : next_value;
      rv->members[i].value = v;
      rv->members[i].name = strdup (members[i].name);

      next_value = v + 1;
      BulpFormat *member_format = members[i].format;
      if (!member_format->base.copy_with_memcpy)
        copy_with_memcpy = BULP_FALSE;
      if (!member_format->base.is_zeroable)
        is_zeroable = BULP_FALSE;
      size = bulp_align(size, member_format->base.c_alignof);
      max_align = BULP_MAX (max_align, member_format->base.c_alignof);
    }

  // sort values by value
  qsort (rv->members, rv->n_members, sizeof (BulpFormatStructMember), compare_format_message_members_by_value);

  // search for dup values
  for (unsigned i = 1; i < n_members; i++)
    if (rv->members[i-1].value == rv->members[i].value)
      {
        bulp_die ("duplicate value in message (%s v %s) (value=%u)",
                  rv->members[i-1].name, rv->members[i].name, rv->members[i].value);
      }

  // compute values_by_name
  rv->members_by_name = malloc (sizeof (BulpFormatStructMember *) * n_members);
  for (unsigned i = 0; i < n_members; i++)
    rv->members_by_name[i] = rv->members + i;
  qsort (rv->members_by_name, rv->n_members, sizeof (BulpFormatStructMember*), compare_ptr_format_message_members_by_name);

  // search for dup names
  for (unsigned i = 1; i < n_members; i++)
    if (strcmp (rv->members_by_name[i-1]->name, rv->members_by_name[i]->name) == 0)
      {
        bulp_die ("duplicate name in enum (name %s)",
                  rv->members_by_name[i]->name);
      }

  // initialize rv->base
  rv->base.type = BULP_FORMAT_TYPE_STRUCT;
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
  rv->base.vfuncs = is_message ? vfuncs__message : vfuncs__struct;

  return (BulpFormat *) rv;
}
