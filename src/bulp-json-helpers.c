#include "bulp-json-helpers.h"

unsigned 
bulp_json_find_backslash_sequence_length (size_t data_length, const uint8_t *data,
                                unsigned start_offset,
                                BulpError **error)
{
  assert(data[start_offset] == '\\');
  if (start_offset + 1 >= data_length)
    {
      *error = bulp_error_new_premature_eof ();
      BULP_ERROR_SET_C_LOCATION (*error);
      return 0;
    }
  switch (data[1]) {
    case 'b': case 'f': case 'n': case 'r': case 't':
    case '\'': case '"': case '/':
    case '\n':
      return 2;
    case 'u':
      if (start_offset + 5 >= data_length)
        {
          *error = bulp_error_new_premature_eof ();
          BULP_ERROR_SET_C_LOCATION (*error);
          return 0;
        }
      return 6;
    default:
      *error = bulp_error_new_invalid_backslash_sequence ();
      BULP_ERROR_SET_C_LOCATION (*error);
      return 0;
  }
}

unsigned 
bulp_json_find_quoted_string_length (size_t data_length,
                                     const uint8_t *data,
                                     unsigned start_offset,
                                     BulpError **error)
{
  char quote_char = data[start_offset];
  assert (quote_char == '"' || quote_char == '\'');

  unsigned offset = start_offset + 1;
  while (offset < data_length)
    {

      switch (data[offset])
        {
          case '\n':
            *error = ...;
            ... raw, literal newlines not allowed
            break;
          case '\\':
            {
              unsigned seqlen = find_backslash_sequence_length (data_length, data, offset, error);
              ...
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
            offset++;
            break;
        }
    }
}

