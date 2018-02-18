
typedef enum {
  BULP_ERROR_OUT_OF_MEMORY,
  BULP_ERROR_UNEXPECTED_CHARACTER,
  BULP_ERROR_PREMATURE_EOF,
  BULP_ERROR_JSON_BAD_NUMBER,
  BULP_ERROR_JSON_BAD_STRING,

} BulpErrorCode;

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


BulpError *bulp_error_out_of_memory (void);
BulpError *bulp_error_new_unexpected_character (uint8_t c, const char *filename, unsigned lineno);
BulpError *bulp_error_new_premature_eof (unsigned source_offset);
BulpError *bulp_error_ref (BulpError *error);
void       bulp_error_unref (BulpError *error);


#define BULP_ERROR_SET_C_LOCATION(e) do{    \
    (e)->c_filename = __FILE__;             \
    (e)->c_lineno = __LINE__;               \
  }while(0)
