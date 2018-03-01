
typedef enum {
  BULP_ERROR_OUT_OF_MEMORY,
  BULP_ERROR_UNEXPECTED_CHARACTER,
  BULP_ERROR_PREMATURE_EOF,
  BULP_ERROR_JSON_BAD_NUMBER,
  BULP_ERROR_JSON_BAD_STRING,
  BULP_ERROR_PARSE,
  BULP_ERROR_INFINITY,
  BULP_ERROR_NOT_A_NUMBER,
  BULP_ERROR_FILE_NOT_FOUND,
  BULP_ERROR_FILE_OPEN_FAILURE,
  BULP_ERROR_FILE_STAT_FAILURE,
  BULP_ERROR_FILE_READ_FAILURE,
  BULP_ERROR_FILE_WRITE_FAILURE,
  BULP_ERROR_UNKNOWN_FORMAT,
  BULP_ERROR_OPTIONAL_OPTIONAL,
  BULP_ERROR_UTF8_BAD,
  BULP_ERROR_UTF8_SHORT,
  BULP_ERROR_TOO_SHORT,
  BULP_ERROR_BAD_DATA,
} BulpErrorCode;

typedef struct BulpError BulpError;

typedef struct BulpErrorClass BulpErrorClass;
struct BulpErrorClass {
  BulpClass base_class;
};

typedef struct BulpError BulpError;
struct BulpError {
  BulpErrorCode code;
  char *message;
  const char *c_filename;
  unsigned c_lineno;
  BulpError *cause;
  unsigned ref_count;
  void (*destroy)(BulpError*);
};


BulpError * bulp_error_new_file_not_found (const char *filename);
BulpError * bulp_error_new_file_open_error (const char *filename, int errno_value);
BulpError * bulp_error_new_stat_failed (const char *filename, int errno_value);
BulpError * bulp_error_new_file_read (int errno_value);
BulpError * bulp_error_new_file_write (int errno_value);
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
BulpError *bulp_error_new_too_short (const char *format,
                                     ...) BULP_PRINTF_LIKE(1,2);
BulpError *bulp_error_new_bad_data (const char *format,
                                    ...) BULP_PRINTF_LIKE(1,2);
BulpError *bulp_error_new_infinity_not_allowed (void);
BulpError *bulp_error_new_not_a_number (void);


void       bulp_error_append_message (BulpError *error,
                                      const char *format,
                                      ...) BULP_PRINTF_LIKE(2,3);

BulpError *bulp_error_new_bad_utf8 (void);
BulpError *bulp_error_new_short_utf8 (void);
#define BULP_ERROR_SET_C_LOCATION(e) do{    \
    (e)->c_filename = __FILE__;             \
    (e)->c_lineno = __LINE__;               \
  }while(0)


// implementation helpers - probably not necessary to be public api
void       bulp_error_base_destroy_protected (BulpError *);
BulpError *bulp_error_new_protected (BulpErrorCode code,
                                     size_t        sizeof_error,                // or 0 for sizeof(BulpError)
                                     void        (*destroy)(BulpError*),        // or NULL for base destroy
                                     const char   *format,
                                     ...) BULP_PRINTF_LIKE(4,5);

BulpError *bulp_error_new_protected_valist (BulpErrorCode code,
                                     size_t        sizeof_error,                // or 0 for sizeof(BulpError)
                                     void        (*destroy)(BulpError*),        // or NULL for base destroy
                                     const char *premessage,    /* or NULL */
                                     const char   *format,
                                     va_list args);
