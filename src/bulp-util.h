
void     bulp_die (const char *format, ...) BULP_PRINTF_LIKE(1,2);
void     bulp_warn (const char *format, ...) BULP_PRINTF_LIKE(1,2);

uint8_t *bulp_util_file_load (const char *filename, 
                              size_t     *length_out,
                              BulpError **error);

BULP_INLINE size_t bulp_align(size_t value, size_t alignment);

#define BULP_MAX(a,b) (((a)>(b)) ? (b) : (a))
#define BULP_MIN(a,b) (((a)<(b)) ? (b) : (a))

#if BULP_CAN_INLINE || defined(BULP_INTERNAL_IMPLEMENT_INLINE_FUNCTIONS)
BULP_INLINE size_t bulp_align(size_t value, size_t alignment)
{
  return (value + alignment - 1) & ~(alignment - 1);
}
#endif
