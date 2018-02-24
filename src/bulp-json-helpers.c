#include "bulp.h"
#include <stdlib.h>
#include <string.h>

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
      *error = bulp_error_new_premature_eof (filename, "in backslash sequence");
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
          *error = bulp_error_new_premature_eof (filename, "\\u expected 4 hex digits");
          BULP_ERROR_SET_C_LOCATION (*error);
          return 0;
        }
      return 6;
    default:
      *error = bulp_error_new_parse (filename, *lineno_inout, "invalid backslash sequence");
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
            *error = bulp_error_new_parse (filename, *lineno_inout, "newlines not allowed in strings");
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
                unsigned nbytes = bulp_utf8_validate_char (data_length - offset, data + offset, error);
                if (nbytes == 0)
                  {
                    bulp_error_append_message (*error, ", at %s:%u", filename, *lineno_inout);
                    return 0;
                  }
                offset += nbytes;
              }
            break;
        }
    }
  *error = bulp_error_new_premature_eof  (filename, "unterminated quoted string starting at line %u", (unsigned) start_line_no);
  return 0;
}

// return_value.str == NULL implies that an error occurred.
// this function doesn't validate
BulpString
bulp_json_string_to_literal (const char *start, const char *end)
{
  const char *at = start;

  // verify and remove start+end quotes
  assert(at + 2 < end);
  assert(at[0] == '"' || at[0] == '\'');
  assert(*(end-1) == at[0]);
  at++;
  end--;

  unsigned rv_alloced = 8;
  BulpString rv = { 0, malloc (rv_alloced) };
  while (at < end)
    {
      char c = *at;
      if (c == '\\')
        {
          if (at + 1 == end)
            break; // premature eof - ignore
          switch (at[1])
            {
            case 'b': c = '\b'; break;
            case 'f': c = '\f'; break;
            case 't': c = '\t'; break;
            case 'n': c = '\n'; break;
            case 'r': c = '\r'; break;
            case '"': c = '"'; break;
            case '\'': c = '\''; break;
            case '/': c = '/'; break;
            case '\\': c = '\\'; break;
            case 'u':
              {
                char hexbuf[5];
                memcpy (hexbuf, at + 2, 4);
                hexbuf[4] = 0;
                unsigned unicode = strtol (hexbuf, NULL, 16);
                if (bulp_utf16_surrogate_type (unicode) == BULP_UTF16_SURROGATE_HI)
                  {
                    assert(at[6] == '\\');
                    assert(at[7] == 'u');
                    memcpy (hexbuf, at + 8, 4);
                    hexbuf[4] = 0;
                    unsigned lo = strtol (hexbuf, NULL, 16);
                    unicode = bulp_utf16_surrogates_combine (unicode, lo);
                    at += 12;
                  }
                else
                  {
                    at += 6;
                  }
                  
                unsigned utf8len;
                uint8_t utf8buf[10];
                utf8len = bulp_utf8_char_encode (unicode, utf8buf);

                while (rv_alloced < rv.length + utf8len + 1)
                  {
                    rv_alloced *= 2;
                    rv.str = realloc (rv.str, rv_alloced);
                  }
                memcpy (rv.str + rv.length, utf8buf, utf8len);
                rv.length += utf8len;
                break;
              }
            case '\n':
              //line_no++;
              at += 2;
              continue;
            }
        }
      else
        at++;

      // append character 'c' to rv
      if (rv_alloced <= rv.length + 1)
        {
          // double size
          rv_alloced *= 2;
          rv.str = realloc (rv.str, rv_alloced);
        }
      rv.str[rv.length++] = c;

    }
  rv.str[rv.length] = '\0';
  return rv;
}
