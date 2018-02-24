unsigned
bulp_json_find_number_length (size_t data_length, const uint8_t *data,
                              unsigned start_offset,
                              const char *filename,
                              unsigned *lineno_inout,
                              BulpError **error);

unsigned 
bulp_json_find_backslash_sequence_length (size_t data_length,
                                          const uint8_t *data,
                                          unsigned start_offset,
                                          const char *filename,
                                          unsigned *lineno_inout,
                                          BulpError **error);

unsigned 
bulp_json_find_quoted_string_length (size_t data_length,
                                     const uint8_t *data,
                                     unsigned start_offset,
                                     const char *filename,
                                     unsigned *lineno_inout,
                                     BulpError **error);

// return_value.str == NULL implies that an error occurred.
BulpString
bulp_json_string_to_literal (const char *at,
                             const char *end,
                             const char *filename,
                             unsigned    start_line_no,
                             BulpError **error);
