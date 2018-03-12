
void     bulp_die (const char *format, ...) BULP_PRINTF_LIKE(1,2);
void     bulp_warn (const char *format, ...) BULP_PRINTF_LIKE(1,2);

bulp_bool bulp_util_read_file (const char *filename, 
                              size_t     *length_out,
                              uint8_t   **data_out,
                              BulpError **error);

BULP_INLINE size_t bulp_align(size_t value, size_t alignment);

bulp_bool bulp_util_writen (int fd,
                            size_t len,
                            const void *data,
                            BulpError**error);
BulpReadResult bulp_util_readn  (int fd,
                                 size_t len,
                                 void *data,
                                 BulpError**error);

bulp_bool bulp_util_pread (int fd, void *out, size_t amt, uint64_t offset, BulpError **error);

#define BULP_MAX(a,b) (((a)>(b)) ? (b) : (a))
#define BULP_MIN(a,b) (((a)<(b)) ? (b) : (a))

#if BULP_CAN_INLINE || defined(BULP_INTERNAL_IMPLEMENT_INLINE_FUNCTIONS)
BULP_INLINE size_t bulp_align(size_t value, size_t alignment)
{
  return (value + alignment - 1) & ~(alignment - 1);
}
#endif
