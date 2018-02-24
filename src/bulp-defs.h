
typedef int bulp_bool;
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

#define BULP_INLINE inline


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
