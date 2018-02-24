
typedef enum {
  BULP_ERROR_OUT_OF_MEMORY,
  BULP_ERROR_UNEXPECTED_CHARACTER,
  BULP_ERROR_PREMATURE_EOF,
  BULP_ERROR_JSON_BAD_NUMBER,
  BULP_ERROR_JSON_BAD_STRING,
  BULP_ERROR_PARSE,
} BulpErrorCode;

typedef struct BulpError BulpError;
struct BulpError {
  BulpErrorCode code;
  char *message;
  const char *c_filename;
  unsigned c_lineno;
  BulpError *cause;
  unsigned ref_count;

  unsigned source_offset;
  unsigned source_lineno;
};


BulpError * bulp_error_new_file_not_found (const char *filename);
BulpError * bulp_error_new_file_open_error (const char *filename, int errno_value);
BulpError * bulp_error_new_stat_failed (const char *filename, int errno_value);
BulpError * bulp_error_new_file_read (int errno_value);
BulpError *bulp_error_out_of_memory (void);
BulpError *bulp_error_new_unexpected_character (uint8_t c, const char *filename, unsigned lineno);
BulpError *bulp_error_new_premature_eof (const char *filename,
                                         const char *format,
                                         ...) BULP_PRINTF_LIKE(2,3);
BulpError *bulp_error_new_parse (const char *filename,
                                 unsigned    line_no,
                                 const char *format,
                                 ...) BULP_PRINTF_LIKE(3,4);
BulpError *bulp_error_new_unknown_format (const char *filename,
                                          unsigned    line_no,
                                          const char *dotted_name);
BulpError *bulp_error_new_optional_optional (const char *filename,
                                             unsigned    line_no,
                                             const char *base_format_name);
BulpError *bulp_error_ref (BulpError *error);
void       bulp_error_unref (BulpError *error);

void       bulp_error_append_message (BulpError *error,
                                      const char *format,
                                      ...) BULP_PRINTF_LIKE(2,3);

#define BULP_ERROR_SET_C_LOCATION(e) do{    \
    (e)->c_filename = __FILE__;             \
    (e)->c_lineno = __LINE__;               \
  }while(0)
