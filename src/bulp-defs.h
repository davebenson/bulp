
typedef uint8_t bulp_bool;
#define BULP_FALSE 0
#define BULP_TRUE  1

#define BULP_INLINE_ASSERT(x) assert(x)

/* BULP_PRINTF_LIKE(format_idx,arg_idx): Advise the compiler
 * that the arguments should be like printf(3); it may
 * optionally print type warnings.  */
#ifdef __GNUC__
#if     __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define BULP_PRINTF_LIKE( format_idx, arg_idx )    \
  __attribute__((__format__ (__printf__, format_idx, arg_idx)))
#endif
#endif
#ifndef BULP_PRINTF_LIKE                /* fallback: no compiler hint */
# define BULP_PRINTF_LIKE( format_idx, arg_idx )
#endif

// TODO
#define BULP_LIKELY(cond)   (cond)
#define BULP_UNLIKELY(cond)   (cond)

#ifdef BULP_INTERNAL_IMPLEMENT_INLINE_FUNCTIONS
#  define BULP_CAN_INLINE 0
#else
#  ifndef BULP_CAN_INLINE
#    define BULP_CAN_INLINE 1
#  endif
#endif

#if BULP_CAN_INLINE
#  define BULP_INLINE inline
#else
#  define BULP_INLINE
#endif

#define BULP_LBRACE_CHAR    '{'
#define BULP_LBRACKET_CHAR  '['
#define BULP_LPAREN_CHAR    '('
#define BULP_RBRACE_CHAR    '}'
#define BULP_RBRACKET_CHAR  ']'
#define BULP_RPAREN_CHAR    ')'

#define BULP_LBRACE_STR     "{"
#define BULP_LBRACKET_STR   "["
#define BULP_LPAREN_STR     "("
#define BULP_RBRACE_STR     "}"
#define BULP_RBRACKET_STR   "]"
#define BULP_RPAREN_STR     ")"


typedef enum
{
  BULP_READ_RESULT_OK,
  BULP_READ_RESULT_EOF,
  BULP_READ_RESULT_ERROR
} BulpReadResult;

typedef struct BulpArray BulpArray;
struct BulpArray
{
  unsigned length;                 // in elements
  void *data;
};

typedef struct BulpString BulpString;
struct BulpString
{
  unsigned length;                 // in bytes
  char *str;                       // must be non-NULL, and str[len] must be '\0' (NUL).   UTF-8
};

typedef struct BulpBinaryData BulpBinaryData;
struct BulpBinaryData
{
  unsigned length;                 // in bytes
  uint8_t *data;
};

BULP_INLINE uint32_t bulp_uint32_to_little_endian (uint32_t input);
BULP_INLINE uint64_t bulp_uint64_to_little_endian (uint64_t input);
#define bulp_uint32_to_little_endian bulp_uint32_from_little_endian
#define bulp_uint64_to_little_endian bulp_uint64_from_little_endian

#if BULP_CAN_INLINE || defined(BULP_INTERNAL_IMPLEMENT_INLINE_FUNCTIONS)
BULP_INLINE uint32_t bulp_uint32_to_little_endian (uint32_t input)
{
#if BULP_IS_LITTLE_ENDIAN
  return input;
#else
  return (input >> 24)
       | (input << 24)
       | ((input << 8) & 0xff0000)
       | ((input >> 8) & 0xff00);
#endif
}

BULP_INLINE uint64_t bulp_uint64_to_little_endian (uint64_t input)
{
#if BULP_IS_LITTLE_ENDIAN
  return input;
#else
  return (input >> 56)
       | (input << 56)
       | ((input << 40) & 0xff000000000000ULL)
       | ((input >> 40) & 0xff00)
       | ((input << 24) & 0xff0000000000ULL)
       | ((input >> 24) & 0xff0000)
       | ((input << 8) & 0xff00000000ULL)
       | ((input >> 8) & 0xff000000);
#endif
}

#endif
