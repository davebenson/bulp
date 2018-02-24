#include "bulp-json-helpers.h"

static inline bulp_bool
is_octal_digit (char d)
{
  return '0' <= d && d < '8';
}

unsigned 
bulp_json_find_backslash_sequence_length (size_t data_length, const uint8_t *data,
                                          unsigned start_offset,
                                          const char *filename,
                                          unsigned *lineno_inout,
                                          BulpError **error)
{
  assert(data[start_offset] == '\\');
  if (start_offset + 1 >= data_length)
    {
      *error = bulp_error_new_premature_eof ();
      BULP_ERROR_SET_C_LOCATION (*error);
      return 0;
    }
  switch (data[start_offset + 1]) {
    case 'b': case 'f': case 'n': case 'r': case 't':
    case '\'': case '"': case '/':
      return 2;
    case '\n':
      *lineno_inout += 1;
      return 2;

    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
      if (start_offset + 2 >= data_length && is_octal_digit (data[start_offset+2]))
        {
          if (start_offset + 3 >= data_length && is_octal_digit (data[start_offset+3]))
            return 4;
          else
            return 3;
        }
      else
        return 2;
          
    case 'u':
      if (start_offset + 5 >= data_length)
        {
          *error = bulp_error_new_premature_eof (filename, *lineno_inout);
          BULP_ERROR_SET_C_LOCATION (*error);
          return 0;
        }
      return 6;
    default:
      *error = bulp_error_new_invalid_backslash_sequence (filename, *lineno_inout);
      BULP_ERROR_SET_C_LOCATION (*error);
      return 0;
  }
}

unsigned 
bulp_json_find_quoted_string_length (size_t data_length,
                                     const uint8_t *data,
                                     unsigned start_offset,
                                     const char *filename,
                                     unsigned *lineno_inout,
                                     BulpError **error)
{
  char quote_char = data[start_offset];
  unsigned start_line_no = *lineno_inout;
  assert (quote_char == '"' || quote_char == '\'');

  unsigned offset = start_offset + 1;
  while (offset < data_length)
    {
      switch (data[offset])
        {
          case '\n':
            *error = bulp_error_parse_bad_newline ("newlines not allowed in strings", filename, *lineno_inout);
            break;
          case '\\':
            {
              unsigned seq_len = bulp_json_find_backslash_sequence_length (data_length, data, offset, filename, lineno_inout, error);
              if (seq_len == 0)
                return 0;
              offset += seq_len;
            }
            break;
          case '\'': case '"':
            if (data[offset] == quote_char)
              {
                /* success! */
                return offset + 1 - start_offset;
              }
            offset++;
            break;

          default:
            if (data[offset] < 128)
              {
                offset++;
              }
            else
              {
                /* validate utf8 */
                ...
              }
            break;
        }
    }
  *error = bulp_error_new_unterminated_string (filename, start_line_no);
  return 0;
}

// return_value.str == NULL implies that an error occurred.
BulpString
bulp_json_string_to_literal (const char *filename, unsigned line_no, const char *start, const char *end, BulpError **error)
{
  const char *at = start;

  // verify and remove start+end quotes
  assert(at + 2 < end);
  assert(at[0] == '"' || at[0] == '\'');
  assert(*(end-1) == at[0]);
  at++;
  end--;

  .... dequote
}
