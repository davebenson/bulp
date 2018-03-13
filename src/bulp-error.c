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
BulpError *bulp_error_new_file_seek (int errno_value)
{
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_FILE_SEEK_FAILURE,
                                            0, NULL,
                                            "error seeking file: %s",
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

BulpError *bulp_error_new_file_write (int errno_value)
{
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_FILE_WRITE_FAILURE,
                                            0, NULL,
                                            "error writing file: %s",
                                            strerror (errno_value));
  return rv;
}

BulpError *bulp_error_new_infinity_not_allowed (void)
{
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_INFINITY,
                                            0, NULL,
                                            "infinity not allowed");
  return rv;
}

BulpError *bulp_error_new_not_a_number (void)
{
  BulpError *rv = bulp_error_new_protected (BULP_ERROR_NOT_A_NUMBER,
                                            0, NULL,
                                            "not a number (NaN or denormal)");
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

BulpError *bulp_error_new_too_short (const char *format,
                                     ...)
{
  va_list args;
  va_start (args, format);
  BulpError *rv = bulp_error_new_protected_valist (BULP_ERROR_TOO_SHORT, 0, NULL,
              "too short: ", format, args);
  va_end (args);
  return rv;
}
BulpError *bulp_error_new_bad_data (const char *format,
                                    ...)
{
  va_list args;
  va_start (args, format);
  BulpError *rv = bulp_error_new_protected_valist (BULP_ERROR_BAD_DATA, 0, NULL,
              "too short: ", format, args);
  va_end (args);
  return rv;
}

BulpError *
bulp_error_new_missing_terminator (const char *format, ...)
{
  va_list args;
  va_start (args, format);
  BulpError *rv = bulp_error_new_protected_valist (BULP_ERROR_MISSING_TERMINATOR, 0, NULL,
              "missing terminator: ", format, args);
  va_end (args);
  return rv;
}

BulpError *
bulp_error_new_nonascii (void)
{
  return bulp_error_new_protected (BULP_ERROR_NONASCII,
                                   0, NULL,
                                   "nonascii character encountered");
}
BulpError * bulp_error_new_unexpected_nul (void)
{
  return bulp_error_new_protected (BULP_ERROR_NONASCII,
                                   0, NULL,
                                   "unexpected NUL encountered");
}


BulpError *bulp_error_new_bad_case_value (uint32_t value, const char *union_name)
{
  return bulp_error_new_protected (BULP_ERROR_BAD_CASE_VALUE,
                                   0, NULL,
                                   "value %u not defined for union %s",
                                   value, union_name);
}

BulpError *
bulp_error_new_unknown_format (const char *filename,
                               unsigned    line_no,
                               const char *dotted_name)
{
  return bulp_error_new_protected (BULP_ERROR_UNKNOWN_FORMAT,
                                   0, NULL,
                                   "unknown format %s at %s:%u",
                                   dotted_name, filename, line_no);
}

BulpError *
bulp_error_new_optional_optional (const char *filename,
                                  unsigned    line_no,
                                  const char *base_format_name)
{
  return bulp_error_new_protected (BULP_ERROR_OPTIONAL_OPTIONAL,
                                   0, NULL,
                                   "taking optional of optional not allowed (base format %s) (at %s:%u)",
                                   base_format_name, filename, line_no);
}
BulpError *bulp_error_new_bad_utf8 (void)
{
  return bulp_error_new_protected (BULP_ERROR_UTF8_BAD,
                                   0, NULL,
                                   "bad UTF-8 encoded data");
}

BulpError *bulp_error_new_short_utf8 (void)
{
  return bulp_error_new_protected (BULP_ERROR_UTF8_SHORT,
                                   0, NULL,
                                   "end-of-data mid-UTF-8 encoded character");
}

BulpError *bulp_error_ref (BulpError *error)
{
  assert(error->ref_count != 0);
  error->ref_count++;
  return error;
}

void       bulp_error_unref (BulpError *error)
{
  assert(error->ref_count != 0);
  if (--error->ref_count == 0)
    {
      error->destroy (error);
    }
}


void       bulp_error_append_message (BulpError *error,
                                      const char *format,
                                      ...)
{
  va_list args;
  va_start (args, format);
  char *msg;
  vasprintf (&msg, format, args);
  va_end (args);
  char *total_msg;
  asprintf (&total_msg, "%s%s", error->message, msg);
  free (msg);
  free (error->message);
  error->message = total_msg;
}


void bulp_error_base_destroy_protected (BulpError *error)
{
  free (error->message);
  free (error);
}

BulpError *bulp_error_new_protected (BulpErrorCode code,
                                     size_t        sizeof_error,                // or 0 for sizeof(BulpError)
                                     void        (*destroy)(BulpError*),
                                     const char   *format,
                                     ...)
{
  if (sizeof_error == 0)
    sizeof_error = sizeof (BulpError);
  else
    assert (sizeof_error >= sizeof (BulpError));
  if (destroy == NULL)
    destroy = bulp_error_base_destroy_protected;
  char *msg;
  va_list args;
  va_start (args, format);
  vasprintf (&msg, format, args);
  va_end (args);
  BulpError *rv = malloc (sizeof_error);
  rv->code = code;
  rv->ref_count = 1;
  rv->message = msg;
  rv->destroy = destroy;
  return rv;
}

BulpError *bulp_error_new_protected_valist (BulpErrorCode code,
                                     size_t        sizeof_error,                // or 0 for sizeof(BulpError)
                                     void        (*destroy)(BulpError*),        // or NULL for base destroy
                                     const char *premessage,    /* or NULL */
                                     const char   *format,
                                     va_list args)
{
  if (sizeof_error == 0)
    sizeof_error = sizeof (BulpError);
  else
    assert (sizeof_error >= sizeof (BulpError));
  if (destroy == NULL)
    destroy = bulp_error_base_destroy_protected;
  char *msg;
  vasprintf (&msg, format, args);
  if (premessage != NULL)
    {
      char *newmsg;
      asprintf(&newmsg, "%s%s", premessage, msg);
      free (msg);
      msg = newmsg;
    }
  BulpError *rv = malloc (sizeof_error);
  rv->code = code;
  rv->ref_count = 1;
  rv->message = msg;
  rv->destroy = destroy;
  return rv;
}
