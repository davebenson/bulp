
#if 0
namespace foo.bar;

struct X {
  int32 a { version < 5, void };
  float b { version < 4, default 2.0 };
  double c;
}

union Y {
  a;
  int[] b;
}

packed Z {
  a : 1;
  b : 4;
  c : 5;
}
#endif

typedef enum {
  TOKEN_TYPE_NAMESPACE,
  TOKEN_TYPE_STRUCT,
  TOKEN_TYPE_UNION,
  TOKEN_TYPE_PACKED,
  TOKEN_TYPE_VERSION,
  TOKEN_TYPE_DEFAULT,
  TOKEN_TYPE_VOID,

  TOKEN_TYPE_BAREWORD

  TOKEN_TYPE_DOT,
  TOKEN_TYPE_COLON,
  TOKEN_TYPE_SEMICOLON,
  TOKEN_TYPE_LBRACE,
  TOKEN_TYPE_RBRACE,
  TOKEN_TYPE_LBRACKET,
  TOKEN_TYPE_RBRACKET,
  TOKEN_TYPE_LPAREN,
  TOKEN_TYPE_RPAREN,
  TOKEN_TYPE_LT,
  TOKEN_TYPE_LTEQ,
  TOKEN_TYPE_EQEQ,
  TOKEN_TYPE_GT,
  TOKEN_TYPE_GTEQ,

  TOKEN_TYPE_NUMBER,                    // js conpatible
  TOKEN_TYPE_STRING,                    // js compatible
} TokenType;

typedef struct {
  TokenType type;
  unsigned byte_offset, byte_length;
  unsigned line_no;
} Token;

typedef struct {
  BulpError *failed;
  size_t n;
  Token *tokens;
  //size_t n_lines;
  //unsigned *line_offsets;
} TokenizeResult;


/* --- json-compatible tokens --- */

/* ------------------------- Tokenization ------------------------- */
static TokenizeResult
tokenize (const char    *filename,
          size_t         length,
          const uint8_t *data,
          unsigned      *lineno_inout)
{
  TokenizeResult rv = {0,};
  
  unsigned tokens_alloced = 128;
  Token *tokens = malloc (sizeof (Token) * tokens_alloced);
  unsigned n_tokens = 0;
  Token tmp;

  #define APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(shortname, len)      \
    do{                                                                \
      tmp.type = TOKEN_TYPE_ ## shortname;                             \
      tmp.byte_length = (len);                                         \
      tmp.byte_offset = offset;                                        \
      tmp.line_no = *line_no;                                          \
      APPEND_TMP_TO_TOKENS();                                          \
      offset += tmp.byte_length;                                       \
    }while(0)
  #define APPEND_TMP_TO_TOKENS()                                       \
    do{                                                                \
      if (tokens_alloced == n_tokens)                                  \
        {                                                              \
          tokens_alloced *= 2;                                         \
          tokens = realloc (tokens,sizeof (Token) * tokens_alloced);   \
          if (tokens == NULL)                                          \
            {                                                          \
              rv.failed = bulp_error_out_of_memory ();                 \
              goto error_cleanup;                                      \
            }                                                          \
        }                                                              \
      tokens[n_tokens++] = tmp;                                        \
    }while(0)

#if 0
  unsigned line_offsets_alloced = 64;
  unsigned *line_offsets = malloc (sizeof (Token) * tokens_alloced);
  unsigned n_line_offsets = 1;
  line_offsets[0] = 0;
#endif

  unsigned offset = 0;
  while (offset < length)
    {
      switch (data[offset]) {
        case ' ': case '\t':
          offset++;
          break;
        case '\n':
          //line_offsets[n_lines++] = offset + 1;
          offset += 1;
          break;
        case '.': APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(DOT, 1); break;
        case ':': APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(COLON, 1); break;
        case '{': APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(LBRACE, 1); break;
        case '}': APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(RBRACE, 1); break;
        case '[': APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(LBRACKET, 1); break;
        case ']': APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(RBRACKET, 1); break;
        case '(': APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(LPAREN, 1); break;
        case ')': APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(RPAREN, 1); break;
        case '<':
          if (offset + 1 < length && data[offset + 1] == '=')
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(LTEQ, 2);
          else
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(LT, 1);
          break;
        case '>':
          if (offset + 1 < length && data[offset + 1] == '=')
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(GTEQ, 2);
          else
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(GT, 1);
          break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          {
            unsigned nlen = find_number_length (length, data, offset, &rv.failed);
            if (nlen == 0)
              goto error_cleanup;
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(NUMBER, nlen);
            break;
          }
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
        case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
        case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
        case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
        case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z': case '_':
          {
            tmp.type = TOKEN_TYPE_BAREWORD;
            tmp.byte_offset = offset;
            tmp.byte_length = find_bareword_length (length, data, offset);
            switch (bw_len) {
              case 4:
                if (memcmp (data + offset, "void", 4) == 0) tmp.type = TOKEN_TYPE_VOID;
                break;
              case 5:
                if (memcmp (data + offset, "union", 5) == 0) tmp.type = TOKEN_TYPE_UNION;
                break;
              case 6:
                if (memcmp (data + offset, "struct", 6) == 0) tmp.type = TOKEN_TYPE_STRUCT;
                if (memcmp (data + offset, "packed", 6) == 0) tmp.type = TOKEN_TYPE_PACKED;
              case 7:
                if (memcmp (data + offset, "version", 6) == 0) tmp.type = TOKEN_TYPE_VERSION;
                if (memcmp (data + offset, "default", 6) == 0) tmp.type = TOKEN_TYPE_DEFAULT;
              case 9:
                if (memcmp (data + offset, "namespace", 9) == 0) tmp.type = TOKEN_TYPE_NAMESPACE;
                break;
             }
            tmp.line_no = *lineno_inout;
            APPEND_TMP_TO_TOKENS();
            offset += tmp.byte_length;
            break;
          }

        case '"':
          {
            unsigned nlen = find_quoted_string_length (length, data, offset, &rv.failed);
            if (nlen == 0)
              goto error_cleanup;
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(STRING, nlen);
            break;
          }

        default:
          ...
      }
    }

  /* success! */
  rv.failed = NULL;
  rv.n = n_tokens;
  rv.tokens = tokens;
  return rv;

error_cleanup:
  assert(rv.failed != NULL);
  free (tokens);
  free (line_offsets);
  return rv;
}

/* ------------------------- Tokens to JSON ------------------------- */
...

/* ------------------------- Parsing ------------------------- */
static bulp_bool
parse_tokens (BulpNamespace *toplevel_ns,
              const char *filename,
              unsigned    n_tokens,
              Token      *tokens,
              BulpError **error)
{
  if (n_tokens == 0)
    {
      // or add use premature-eof or a "empty" error
      return BULP_TRUE;
    }

  if (tokens[0].type != TOKEN_TYPE_NAMESPACE)
    {
      SET_ERROR(bulp_error_new_parse (...));
      goto error_cleanup;
    }

  unsigned ns_n_tokens = parse_dotted_name_from_tokens (...);
  if (ns_n_tokens == 0)
    {
      goto error_cleanup;
    }

  token_at += dotted_name_n_tokens;

  if (tokens[token_at].type != TOKEN_TYPE_SEMICOLON)
    {
      SET_ERROR(bulp_error_new_parse (...));
      goto error_cleanup;
    }

  // parse stanzas 
  switch (tokens[token_at].type)
    {
      case TOKEN_TYPE_STRUCT:
        ...
      case TOKEN_TYPE_PACKED:
        ...
      case TOKEN_TYPE_UNION:
        ...
      default:
        SET_ERROR (...);
        goto error_cleanup;
    }

  return BULP_TRUE;
}

/* ------------------------- Toplevel ------------------------- */
static bulp_bool
bulp_namespace_parse_file       (BulpNamespace *ns,
                                 const char    *filename,
                                 BulpError    **error)
{
  // load into memory 
  size_t data_length;
  uint8_t *data = bulp_util_file_load (filename, &data_length, error);
  if (data == NULL)
    {
      return BULP_FALSE;
    }
  parse_rv = bulp_namespace_parse_data (ns, filename, data_length, data, error);
  free (data);
  return parse_rv;
}

static bulp_bool
bulp_namespace_parse_data       (BulpNamespace *ns,
                                 const char    *filename,
                                 size_t         data_length,
                                 const uint8_t *data,
                                 BulpError    **error)
{
  // tokenize
  unsigned lineno = 1;
  TokenizeResult tokenize_result = tokenize (filename, data_length, data, &lineno);
  if (tokenize_result.failed != NULL)
    {
      *error = tokenize_result.failed;
      return BULP_FALSE;
    }

  // parse tokens
  bulp_bool parse_token_rv = parse_tokens (filename, tokenize_result.n_tokens, tokenize_result.tokens, error);
  free (tokenize_result.tokens);
  free (file_data);
  return parse_token_rv;
}
