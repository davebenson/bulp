
void     bulp_die (const char *format, ...) BULP_PRINTF_LIKE(1,2);
void     bulp_warn (const char *format, ...) BULP_PRINTF_LIKE(1,2);

uint8_t *bulp_util_file_load (const char *filename, 
                              size_t     *length_out,
                              BulpError **error);
