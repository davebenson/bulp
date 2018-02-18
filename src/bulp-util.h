
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

void     bulp_die (const char *format, ...) BULP_PRINTF_LIKE(1,2);

uint8_t *bulp_util_file_load (const char *filename, 
                              size_t     *length_out,
                              BulpError **error);
