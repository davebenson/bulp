
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

#define BULP_UTF16_IS_BIG(unicode)          ((unicode) >= 0x10000)
#define BULP_UTF16_IS_ENCODABLE(unicode)    ((unicode) < 0x110000)

BULP_INLINE void
bulp_utf16_big_encode (unsigned unicode, uint16_t *hi_out, uint16_t *lo_out)
{
  BULP_INLINE_ASSERT (BULP_UTF16_IS_BIG (unicode));
  BULP_INLINE_ASSERT (BULP_UTF16_IS_ENCODABLE (unicode));
  unsigned x = unicode - 0x10000;
  *hi_out = 0xd800 + (x >> 10);
  *lo_out = 0xd800 + (x & 1023);
}
