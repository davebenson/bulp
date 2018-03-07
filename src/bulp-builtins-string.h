
/* ascii string
   ascii0 string0 */

// length-prefixed ascii string
BULP_INLINE bulp_bool bulp_ascii_validate       (BulpString str,
                                                 BulpError **error);
BULP_INLINE size_t bulp_ascii_get_packed_size   (BulpString str);
BULP_INLINE size_t bulp_ascii_pack              (BulpString str,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_ascii_pack_to           (BulpString str,
                                                 BulpDataBuilder *out);
BULP_INLINE size_t bulp_ascii_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpError**error);

// length-prefixed utf8 string [length-prefix is in bytes]
BULP_INLINE bulp_bool bulp_string_validate      (BulpString str,
                                                 BulpError **error);
BULP_INLINE size_t bulp_string_get_packed_size  (BulpString str);
BULP_INLINE size_t bulp_string_pack             (BulpString str,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_string_pack_to          (BulpString str,
                                                 BulpDataBuilder *out);
BULP_INLINE size_t bulp_string_unpack           (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpError**error);

// NUL-terminated ascii string
BULP_INLINE bulp_bool bulp_ascii0_validate      (BulpString str,
                                                 BulpError **error);
BULP_INLINE size_t bulp_ascii0_get_packed_size  (BulpString str);
BULP_INLINE size_t bulp_ascii0_pack             (BulpString str,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_ascii0_pack_to          (BulpString str,
                                                 BulpDataBuilder *out);
BULP_INLINE size_t bulp_ascii0_unpack           (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpError**error);

// NUL-terminated utf8 string
BULP_INLINE bulp_bool bulp_string0_validate     (BulpString str,
                                                 BulpError **error);
BULP_INLINE size_t bulp_string0_get_packed_size (BulpString str);
BULP_INLINE size_t bulp_string0_pack            (BulpString str,
                                                 uint8_t *out);
BULP_INLINE size_t bulp_string0_pack_to         (BulpString str,
                                                 BulpDataBuilder *out);
BULP_INLINE size_t bulp_string0_unpack          (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpError**error);

#if BULP_CAN_INLINE || defined(BULP_INTERNAL_IMPLEMENT_INLINE_FUNCTIONS)
BULP_INLINE bulp_bool bulp_ascii_validate          (BulpString s,
                                                    BulpError **error)
{
  for (unsigned i = 0; i < s.length; i++)
    if (s.str[i] & 0x80)
      {
        *error = bulp_error_new_nonascii ();
        return BULP_FALSE;
      }
  return BULP_TRUE;
}

BULP_INLINE size_t bulp_ascii_get_packed_size   (BulpString str)
{
  return bulp_uint_get_packed_size(str.length) + str.length;
}

BULP_INLINE size_t bulp_ascii_pack              (BulpString str,
                                                 uint8_t *out)
{
  size_t lenlen = bulp_uint_pack (str.length, out);
  memcpy (out + lenlen, str.str, str.length);
  return lenlen + str.length;
}
BULP_INLINE size_t bulp_ascii_pack_to           (BulpString str,
                                                 BulpDataBuilder *out)
{
  size_t lenlen = bulp_uint_pack_to (str.length, out);
  bulp_data_builder_append_nocopy (out, str.length, (uint8_t *) str.str);
  return lenlen + str.length;
}

BULP_INLINE size_t bulp_ascii_unpack            (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpError**error)
{
  uint32_t len;
  size_t lenlen = bulp_uint_unpack (packed_len, packed_data, &len, error);
  if (lenlen == 0)
    return 0;
  if (lenlen + len > packed_len)
    {
      *error = bulp_error_new_too_short ("unpacking length-prefixed ascii string");
      return 0;
    }
  out->str = (char*) packed_data + lenlen;
  out->length = len;
  if (!bulp_ascii_validate (*out, error))
    return 0;
  return len + lenlen;
}

BULP_INLINE bulp_bool bulp_string_validate         (BulpString str,
                                                 BulpError **error)
{
  return bulp_utf8_validate (str.length, (const uint8_t *) str.str, error);
}

BULP_INLINE size_t bulp_string_get_packed_size  (BulpString str)
{
  return bulp_uint_get_packed_size(str.length) + str.length;
}

BULP_INLINE size_t bulp_string_pack             (BulpString str,
                                                 uint8_t *out)
{
  size_t lenlen = bulp_uint_pack (str.length, out);
  memcpy (out + lenlen, str.str, str.length);
  return lenlen + str.length;
}

BULP_INLINE size_t bulp_string_pack_to          (BulpString str,
                                                 BulpDataBuilder *out)
{
  size_t lenlen = bulp_uint_pack_to (str.length, out);
  bulp_data_builder_append_nocopy (out, str.length, (uint8_t *) str.str);
  return lenlen + str.length;
}

BULP_INLINE size_t bulp_string_unpack           (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpError**error)
{
  uint32_t len;
  size_t lenlen = bulp_uint_unpack (packed_len, packed_data, &len, error);
  if (lenlen == 0)
    return 0;
  if (lenlen + len > packed_len)
    {
      *error = bulp_error_new_too_short ("unpacking length-prefixed UTF-8 string");
      return 0;
    }
  out->str = (char*) packed_data + lenlen;
  out->length = len;
  if (!bulp_string_validate (*out, error))
    return 0;
  return len + lenlen;
}

BULP_INLINE bulp_bool bulp_ascii0_validate         (BulpString str,
                                                 BulpError **error)
{
  for (unsigned i = 0; i < str.length; i++)
    if ((str.str[i] & 0x80) || str.str[i] == 0)
      {
        *error = bulp_error_new_nonascii ();
        return BULP_FALSE;
      }
  return BULP_TRUE;
}

BULP_INLINE size_t bulp_ascii0_get_packed_size  (BulpString str)
{
  return str.length + 1;
}

BULP_INLINE size_t bulp_ascii0_pack             (BulpString str,
                                                 uint8_t *out)
{
  memcpy (out, str.str, str.length);
  out[str.length] = 0;
  return str.length + 1;
}

BULP_INLINE size_t bulp_ascii0_pack_to          (BulpString str,
                                                 BulpDataBuilder *out)
{
  bulp_data_builder_append_nocopy (out, str.length, (uint8_t *) str.str);
  bulp_data_builder_append_byte (out, 0);
  return 1 + str.length;
}

BULP_INLINE size_t bulp_ascii0_unpack           (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpError**error)
{
  const uint8_t *end = packed_data + packed_len;
  out->str = (char *) packed_data;
  const uint8_t *at = packed_data;
  while (at < end)
    {
      if (*at & 0x80)
        {
          *error = bulp_error_new_nonascii ();
          return 0;
        }
      else if (*at == 0)
        goto found_nul;
      at++;
    }
  *error = bulp_error_new_missing_terminator ("expected NUL after ascii0 string");
  return 0;

found_nul:
  out->length = at - packed_data;
  return out->length + 1;
}


BULP_INLINE bulp_bool bulp_string0_validate        (BulpString str,
                                                 BulpError **error)
{
  return bulp_utf8_validate_nonnul (str.length, (const uint8_t *) str.str, error);
}

BULP_INLINE size_t bulp_string0_get_packed_size (BulpString str)
{
  return str.length + 1;
}

BULP_INLINE size_t bulp_string0_pack            (BulpString str,
                                                 uint8_t *out)
{
  memcpy (out, str.str, str.length);
  out[str.length] = 0;
  return str.length + 1;
}

BULP_INLINE size_t bulp_string0_pack_to         (BulpString str,
                                                 BulpDataBuilder *out)
{
  bulp_data_builder_append_nocopy (out, str.length, (uint8_t *) str.str);
  bulp_data_builder_append_byte (out, 0);
  return str.length + 1;
}

BULP_INLINE size_t bulp_string0_unpack          (size_t packed_len,
                                                 const uint8_t *packed_data,
                                                 BulpString *out,
                                                 BulpError**error)
{
  const uint8_t *nul = memchr (packed_data, 0, packed_len);
  if (nul == NULL)
    {
      *error = bulp_error_new_missing_terminator ("expected NUL after string0 string");
      return 0;
    }
  out->str = (char*) packed_data;
  out->length = nul - packed_data;
  if (!bulp_utf8_validate (out->length, (const uint8_t *) out->str, error))
    return 0;
  return nul + 1 - packed_data;
}

#endif
