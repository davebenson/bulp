//#define _GNU_SOURCE

#include "bulp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

BulpError *bulp_error_new_file_not_found (const char *filename)
{
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_FILE_NOT_FOUND,
                                            0, NULL,
                                            "file not found: %s", filename);
  return rv;
}

BulpError *
bulp_error_new_file_open_error (const char *filename,
                                int errno_value)
{
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_FILE_OPEN_FAILURE,
                                            0, NULL,
                                            "error opening file %s: %s",
                                            filename,
                                            strerror (errno_value));
  return rv;
}

BulpError *
bulp_error_new_stat_failed (const char *filename,
                            int errno_value)
{
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_FILE_STAT_FAILURE,
                                            0, NULL,
                                            "error stat'ing file %s: %s",
                                            filename,
                                            strerror (errno_value));
  return rv;
}
BulpError *bulp_error_new_file_read (int errno_value)
{
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_FILE_READ_FAILURE,
                                            0, NULL,
                                            "error reading file: %s",
                                            strerror (errno_value));
  return rv;
}

BulpError *bulp_error_out_of_memory (void)
{
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_OUT_OF_MEMORY,
                                            0, NULL,
                                            "out-of-memory");
  return rv;
}

BulpError *
bulp_error_new_unexpected_character (uint8_t c,
                                     const char *filename,
                                     unsigned lineno)
{
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_OUT_OF_MEMORY,
                                            0, NULL,
                                            "unexpected character 0x%02x (at %s:%u)", c, filename, lineno);
  return rv;
}

BulpError *bulp_error_new_premature_eof (const char *filename,
                                         const char *format,
                                         ...)
{
  va_list args;
  char *formatted_str;
  va_start (args, format);
  vasprintf (&formatted_str, format, args);
  va_end (args);
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_PREMATURE_EOF,
                                            0, NULL,
                                            "premature end-of-file in file %s: %s",
                                            filename,
                                            formatted_str);
  free (formatted_str);
  return rv;
}
BulpError *bulp_error_new_parse (const char *filename,
                                 unsigned    line_no,
                                 const char *format,
                                 ...)
{
  va_list args;
  char *formatted_str;
  va_start (args, format);
  vasprintf (&formatted_str, format, args);
  va_end (args);
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_PARSE,
                                            0, NULL,
                                            "parse error at %s:%u: %s",
                                            filename, line_no,
                                            formatted_str);
  free (formatted_str);
  return rv;
}

BulpError *
bulp_error_new_unknown_format (const char *filename,
                               unsigned    line_no,
                               const char *dotted_name)
{
...
}

BulpError *
bulp_error_new_optional_optional (const char *filename,
                                  unsigned    line_no,
                                  const char *base_format_name)
{
...
}

BulpError *bulp_error_ref (BulpError *error)
{
  assert(error->ref_count != 0);
  error->ref_count++;
}

void       bulp_error_unref (BulpError *error)
{
  assert(error->ref_count != 0);
  if (--error->ref_count == 0)
    {
      error->destroy
    }
}


void       bulp_error_append_message (BulpError *error,
                                      const char *format,
                                      ...)
{
  va_list args = va_start (args, format);
  char *msg;
  vasprintf (&msg, format, args);
  va_end (args);
  char *total_msg = asprintf ("%s%s", error->message, msg);
  free (msg);
  free (error->message);
  error->message = total_msg;
}


void bulp_error_base_destroy_protected (BulpError *error)
{
  free (error->message);
  free (error);
}

BulpError *bulp_error_new_protected (BulpErrorCode code,B
                                     size_t        sizeof_error,                // or 0 for sizeof(BulpError)
                                     void        (*destroy)(BulpError*);        // or NULL for base destroy
                                     const char   *format,
                                     ...) BULP_PRINTF_LIKE(4,5);

