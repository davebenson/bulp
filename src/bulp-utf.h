
/* UTF-8 and UTF-16 handling, just that which is needed for JSON parsing */


/* UTF-8 parsing */
typedef enum {
  BULP_UTF8_CHAR_VALID,
  BULP_UTF8_CHAR_INVALID,
  BULP_UTF8_CHAR_PREMATURE_EOF
} BulpUTF8CharValidationResult;


BULP_INLINE BulpUTF8CharValidationResult
bulp_utf8_char_validate (size_t max_bytes,
                         const uint8_t *bytes,
                         unsigned *unicode_out_opt,
                         unsigned *bytes_used_out);


BULP_INLINE unsigned
bulp_utf8_char_encode (unsigned unicode,
                       uint8_t *bytes_out);

BULP_INLINE unsigned
bulp_utf8_parse_char (size_t len, const uint8_t *data, uint32_t *codepoint_out, BulpError **error);
BULP_INLINE unsigned
bulp_utf8_validate_char (size_t len, const uint8_t *data, BulpError **error);

/* UTF-16 surrogate pair handling.
 * This is required so that we can transform surrogate pairs like:
 *     \ud801\udc37
 * is a UTF-16 encoded character 0x10437.  (Taken from wikipedia's UTF-16 article)
 */
typedef enum
{
  BULP_UTF16_SURROGATE_NONE,
  BULP_UTF16_SURROGATE_HI,
  BULP_UTF16_SURROGATE_LO
} BulpUTF16SurrogateType;

#define BULP_UTF16_IS_BIG(unicode)          ((unicode) >= 0x10000)
#define BULP_UTF16_IS_ENCODABLE(unicode)    ((unicode) < 0x110000)


#if BULP_CAN_INLINE || defined(BULP_INTERNAL_IMPLEMENT_INLINE_FUNCTIONS)
BULP_INLINE unsigned
bulp_utf8_char_encode (unsigned unicode,
                       uint8_t *bytes_out)
{
  if (unicode < (1<<7))
    {
      bytes_out[0] = unicode;
      return 1;
    }
  else if (unicode < (1<<11))
    {
      bytes_out[0] = 0xc0 | (unicode >> 6);
      bytes_out[1] = 0x80 | (unicode & 31);
      return 2;
    }
  else if (unicode < (1<<16))
    {
      bytes_out[0] = 0xe0 | (unicode >> 12);
      bytes_out[1] = 0x80 | ((unicode >> 6) & 0x3f);
      bytes_out[2] = 0x80 | ((unicode >> 0) & 0x3f);
      return 3;
    }
  else if (unicode < (1<<21))
    {
      bytes_out[0] = 0xf0 | (unicode >> 18);
      bytes_out[1] = 0x80 | ((unicode >> 12) & 0x3f);
      bytes_out[2] = 0x80 | ((unicode >> 6) & 0x3f);
      bytes_out[3] = 0x80 | ((unicode >> 0) & 0x3f);
      return 4;
    }
  else
    {
      assert(0);
      return 0;
    }
}

BULP_INLINE unsigned
bulp_utf8_parse_char (size_t len,
                      const uint8_t *data,
                      uint32_t *codepoint_out,
                      BulpError **error)
{
  if ((data[0] & 0x80) == 0)
    {
      *codepoint_out = data[0];
      return 1;
    }
  if ((data[0] & 0xe0) == 0xc0)
    {
      if (len < 2)
        goto too_short;
      if ((data[1] & 0xc0) != 0x80)
        goto bad_format;
      *codepoint_out = (((unsigned)data[0] & 0x1f) << 6)
                     | (((unsigned)data[1] & 0x3f) << 0);
      return 2;
    }
  if ((data[0] & 0xf0) == 0xe0)
    {
      if (len < 3)
        goto too_short;
      if ((data[1] & 0xc0) != 0x80 || (data[2] & 0xc0) != 0x80)
        goto bad_format;
      *codepoint_out = (((unsigned)data[0] & 0x0f) << 12)
                     | (((unsigned)data[1] & 0x3f) << 6)
                     | (((unsigned)data[2] & 0x3f) << 0);
      return 3;
    }
  if ((data[0] & 0xf8) == 0xf0)
    {
      if (len < 4)
        goto too_short;
      if ((data[1] & 0xc0) != 0x80 || (data[2] & 0xc0) != 0x80 || (data[3] & 0xc0) != 0x80)
        goto bad_format;
      *codepoint_out = (((unsigned)data[0] & 0x07) << 18)
                     | (((unsigned)data[1] & 0x3f) << 12)
                     | (((unsigned)data[2] & 0x3f) << 6)
                     | (((unsigned)data[3] & 0x3f) << 0);
      return 4;
    }
bad_format:
  *error = bulp_error_new_bad_utf8 ();
  return 0;
too_short:
  *error = bulp_error_new_short_utf8 ();
  return 0;
}

BULP_INLINE unsigned
bulp_utf8_validate_char (size_t len,
                         const uint8_t *data,
                         BulpError **error)
{
  if ((data[0] & 0x80) == 0)
    {
      return 1;
    }
  if ((data[0] & 0xe0) == 0xc0)
    {
      if (len < 2)
        goto too_short;
      if ((data[1] & 0xc0) != 0x80)
        goto bad_format;
      return 2;
    }
  if ((data[0] & 0xf0) == 0xe0)
    {
      if (len < 3)
        goto too_short;
      if ((data[1] & 0xc0) != 0x80 || (data[2] & 0xc0) != 0x80)
        goto bad_format;
      return 3;
    }
  if ((data[0] & 0xf8) == 0xf0)
    {
      if (len < 3)
        goto too_short;
      if ((data[1] & 0xc0) != 0x80 || (data[2] & 0xc0) != 0x80 || (data[3] & 0xc0) != 0x80)
        goto bad_format;
      return 4;
    }
bad_format:
  *error = bulp_error_new_bad_utf8 ();
  return 0;
too_short:
  *error = bulp_error_new_short_utf8 ();
  return 0;
}

#if 0
BULP_INLINE unsigned
bulp_utf8_char_encode (unsigned unicode,
                       uint8_t *bytes_out)
{
  if (unicode < (1<<7))
    {
      bytes_out[0] = unicode;
      return 1;
    }
  else if (unicode < (1<<10))
    {
      bytes_out[0] = 0xc0 | (unicode >> 6)
      bytes_out[1] = 0x80 | (unicode & 0x3f);
      return 2;
    }
  else if (unicode < (1<<16))
    {
      bytes_out[0] = 0xe0 | (unicode >> 12)
      bytes_out[1] = 0x80 | ((unicode >> 6) & 0x3f);
      bytes_out[2] = 0x80 | (unicode & 0x3f);
      return 3;
    }
  else if (unicode < (1<<21))
    {
      bytes_out[0] = 0xf0 | (unicode >> 18)
      bytes_out[1] = 0x80 | ((unicode >> 12) & 0x3f);
      bytes_out[2] = 0x80 | ((unicode >> 6) & 0x3f);
      bytes_out[3] = 0x80 | (unicode & 0x3f);
      return 3;
    }
  else
    {
      assert(0);
      return 0;
    }
}
#endif

BULP_INLINE BulpUTF16SurrogateType
bulp_utf16_surrogate_type (uint16_t utf16)
{
  switch (utf16 >> 10)
    {
      //0xd8>>2 == 0xd8/4 = (12*16+8)/4 = 12*4+2 == 50 == 0x32
      case 0x32: return BULP_UTF16_SURROGATE_HI;

      //0xdc>>2 == 0xdc/4 = (12*16+12)/4 = 12*4+3 == 51 == 0x33
      case 0x33: return BULP_UTF16_SURROGATE_LO;

      default: return BULP_UTF16_SURROGATE_NONE;
    }
}

BULP_INLINE unsigned
bulp_utf16_surrogates_combine (uint16_t hi, uint16_t lo)
{
  BULP_INLINE_ASSERT (bulp_utf16_surrogate_type (hi) == BULP_UTF16_SURROGATE_HI);
  BULP_INLINE_ASSERT (bulp_utf16_surrogate_type (lo) == BULP_UTF16_SURROGATE_LO);
  return ((hi - 0xd800) << 10) + (lo - 0xdc00) + 0x10000;
}

BULP_INLINE void
bulp_utf16_big_encode (unsigned unicode, uint16_t *hi_out, uint16_t *lo_out)
{
  BULP_INLINE_ASSERT (BULP_UTF16_IS_BIG (unicode));
  BULP_INLINE_ASSERT (BULP_UTF16_IS_ENCODABLE (unicode));
  unsigned x = unicode - 0x10000;
  *hi_out = 0xd800 + (x >> 10);
  *lo_out = 0xd800 + (x & 1023);
}
#endif

