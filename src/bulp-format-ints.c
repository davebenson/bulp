

// Example usage:
//   EMIT_BASE(&bulp_namespace_global, "short", uint16_t, "uint16_t", bulp_uint16, BULP_UINT16) \
/* --- Implementations for 'short' --- */
/* --- Implementations for 'int' --- */
/* --- Implementations for 'long' --- */
/* --- Implementations for 'ushort' --- */
/* --- Implementations for 'uint' --- */
/* --- Implementations for 'ulong' --- */

#define EMIT_BASE(canon_ns, canon_name, byte_size, ctype, ctypestr, func_prefix, macro_prefix) \
  { \
    BULP_FORMAT_INT,     \
    0, /* ref-count */   \
    EMIT_INT_FORMAT_VFUNCS(func_prefix); \
    &bulp_namespace_global, \
    #canon_name, \
    byte_size, /* c_alignof */ \
    byte_size, /* c_sizeof */ \
    ctype, /* c_typename */ \
    func_prefix, /* c_func_prefix */ \
    macro_prefix, /* c_macro_prefix */ \
    NULL,       /* optional_of */ \
    NULL        /* array_of */ \
  }
BulpFormatInt bulp_format_ints_global[14] = {

  /* ints[0] == short: variable-length zigzagged 16-bit signed integer */
  {
    EMIT_BASE("short", int16_t, "int16_t", "bulp_short", "BULP_SHORT"),
    2,  /* byte_size */
    BULP_TRUE, /* is_signed */
    BULP_TRUE, /* is_b128 */
  },
  /* ints[1] == int: variable-length zigzagged 32-bit signed integer */
  {
    EMIT_BASE("int", int32_t, "int32_t", "bulp_int", "BULP_INT"),
    4,  /* byte_size */
    BULP_TRUE, /* is_signed */
    BULP_TRUE, /* is_b128 */
  },
  /* ints[2] == long: variable-length zigzagged 64-bit signed integer */
  {
    EMIT_BASE("long", int64_t, "int64_t", "bulp_long", "BULP_long"),
    8,  /* byte_size */
    BULP_TRUE, /* is_signed */
    BULP_TRUE, /* is_b128 */
  },
  /* ints[3] == ushort: variable-length 16-bit unsigned integer */
  {
    EMIT_BASE("ushort", uint16_t, "uint16_t", "bulp_ushort", "BULP_USHORT"),
    2,  /* byte_size */
    BULP_FALSE, /* !is_signed */
    BULP_TRUE, /* is_b128 */
  },
  /* ints[4] == uint: variable-length 32-bit unsigned integer */
  {
    EMIT_BASE("int", int32_t, "int32_t", "bulp_int", "BULP_INT"),
    4,  /* byte_size */
    BULP_FALSE, /* !is_signed */
    BULP_TRUE, /* is_b128 */
  },
  /* ints[5] == ulong: variable-length 64-bit unsigned integer */
  {
    EMIT_BASE("long", int64_t, "int64_t", "bulp_int64", "BULP_INT64"),
    8,  /* byte_size */
    BULP_FALSE, /* !is_signed */
    BULP_TRUE, /* is_b128 */
  },

  /* ints[6] == int8: fixed-length 8-bit signed integer */
  {
    EMIT_BASE("int8", int8_t, "int8_t", "bulp_int8", "BULP_INT8"),
    1,  /* byte_size */
    BULP_TRUE, /* is_signed */
    BULP_FALSE, /* is_b128 */
  },
  /* ints[7] == int16: fixed-length 16-bit signed integer */
  {
    EMIT_BASE("int16", int16_t, "int16_t", "bulp_int16", "BULP_INT16"),
    2,  /* byte_size */
    BULP_TRUE, /* is_signed */
    BULP_FALSE, /* is_b128 */
  },
  /* ints[8] == int32: fixed-length 32-bit signed integer */
  {
    EMIT_BASE("int32", int32_t, "int32_t", "bulp_int32", "BULP_INT32"),
    4,  /* byte_size */
    BULP_TRUE, /* is_signed */
    BULP_FALSE, /* is_b128 */
  },
  /* ints[9] == int64: fixed-length 64-bit signed integer */
  {
    EMIT_BASE("int64", int64_t, "int64_t", "bulp_int64", "BULP_INT64"),
    8,  /* byte_size */
    BULP_TRUE, /* is_signed */
    BULP_FALSE, /* is_b128 */
  },
  /* ints[10] == uint8: fixed-length 8-bit signed integer */
  {
    EMIT_BASE("uint8", uint8_t, "uint8_t", "bulp_uint8", "BULP_UINT8"),
    1,  /* byte_size */
    BULP_FALSE, /* !is_signed */
    BULP_FALSE, /* is_b128 */
  },
  /* ints[11] == uint16: fixed-length 16-bit signed integer */
  {
    EMIT_BASE("uint16", uint16_t, "uint16_t", "bulp_uint16", "BULP_UINT16"),
    2,  /* byte_size */
    BULP_FALSE, /* !is_signed */
    BULP_FALSE, /* is_b128 */
  },
  /* ints[12] == uint32: fixed-length 32-bit signed integer */
  {
    EMIT_BASE("uint32", uint32_t, "uint32_t", "bulp_uint32", "BULP_UINT32"),
    4,  /* byte_size */
    BULP_FALSE, /* !is_signed */
    BULP_FALSE, /* is_b128 */
  },
  /* ints[13] == uint64: fixed-length 64-bit signed integer */
  {
    EMIT_BASE("uint64", uint64_t, "uint64_t", "bulp_uint64", "BULP_UINT64"),
    8,  /* byte_size */
    BULP_FALSE, /* !is_signed */
    BULP_FALSE, /* is_b128 */
  },

};
