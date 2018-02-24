#include "bulp.h"
#include <stdlib.h>
#include <string.h>

/* --- import stanzas --- */
typedef struct BulpImportEntry BulpImportEntry;
typedef struct BulpImports BulpImports;

#if 0
typedef enum {
  BULP_IMPORT_ENTRY_TYPE_EXACT,         // if "as" == NULL
  BULP_IMPORT_ENTRY_TYPE_ALIAS          // if "as" != NULL
} BulpImportEntryType;
#endif

#define MAX_INTEGER_LENGTH          20
#define MAX_FLOAT_LENGTH            128

struct BulpImportEntry {
  //BulpImportEntryType type;
  BulpNamespace *ns;
  char *as;             /* may be NULL */
};

struct BulpImports {
  /// private: use API below so that we can optimize later
  unsigned n_entries;
  BulpImportEntry *entries;
  unsigned entries_alloced;
  BulpNamespace *global_ns;
  BulpNamespace *this_ns;               // the namespace being defined
  unsigned this_ns_version;
};

static BulpImports *
bulp_imports_new (BulpNamespace *global_ns)
{
  BulpImports *imports = calloc(sizeof(BulpImports), 1);
  imports->global_ns = bulp_namespace_ref (global_ns);
  return imports;
}

static BulpFormat *
bulp_imports_lookup (BulpImports *imports, unsigned n_names, char **names)
{
  BulpNamespaceEntry ns_entry;

  // Parse base format (which is always a bareword).
  if (n_names == 1)
    {
      // search namespaces in order
      for (unsigned i = 0; i < imports->n_entries; i++)
        if (imports->entries[i].as == NULL
         && bulp_namespace_query_1 (imports->entries[i].ns, names[0], &ns_entry)
         && ns_entry.type == BULP_NAMESPACE_ENTRY_FORMAT)
          return ns_entry.info.v_format;
      return NULL;
    }

  if (n_names == 2)
    {
      // handle formats under an alias, e.g. the format referred to by F.x after
      //    import foo.bar as F;
      for (unsigned i = 0; i < imports->n_entries; i++)
        if (imports->entries[i].as != NULL
         && strcmp (imports->entries[i].as, names[0]) == 0)
          {
            if (bulp_namespace_query_1 (imports->entries[i].ns, names[1], &ns_entry)
             && ns_entry.type == BULP_NAMESPACE_ENTRY_FORMAT)
              return ns_entry.info.v_format;
              
          }
      // fall-through to exact namespace lookup
    }

  // exact global namespace (for now)
  BulpNamespace *ns = imports->global_ns;
  for (unsigned i = 0; i < n_names - 1; i++)
    {
      if (!bulp_namespace_query_1 (ns, names[i], &ns_entry))
        return NULL;
      if (ns_entry.type == BULP_NAMESPACE_ENTRY_FORMAT)
        {
          // got format but expected namespace, but no current way to give error details
          return NULL;
        }
      assert(ns_entry.type == BULP_NAMESPACE_ENTRY_SUBNAMESPACE);
      ns = ns_entry.info.v_namespace;
    }

  if (!bulp_namespace_query_1 (ns, names[n_names - 1], &ns_entry))
    return NULL;
  if (ns_entry.type == BULP_NAMESPACE_ENTRY_SUBNAMESPACE)
    {
      // got namespace but expected format, but no current way to give error details
      return NULL;
    }
  assert(ns_entry.type == BULP_NAMESPACE_ENTRY_FORMAT);
  return ns_entry.info.v_format;
}


#if 0
namespace foo.bar;

import foo.baz.Wombat;
import foo.qoo as QOO;

struct X {
  int32 a { version: 1...2, 4...; void };
  float b { version < 4, default 2.0 };
  double c;
  Wombat w;
  QOO.y qy;
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
  TOKEN_TYPE_MESSAGE,
  TOKEN_TYPE_VERSION,
  TOKEN_TYPE_DEFAULT,
  TOKEN_TYPE_VOID,

  TOKEN_TYPE_BAREWORD,

  TOKEN_TYPE_DOT,
  TOKEN_TYPE_DOTDOTDOT,
  TOKEN_TYPE_COLON,
  TOKEN_TYPE_SEMICOLON,
  TOKEN_TYPE_COMMA,
  TOKEN_TYPE_LBRACE,
  TOKEN_TYPE_RBRACE,
  TOKEN_TYPE_LBRACKET,
  TOKEN_TYPE_RBRACKET,
  TOKEN_TYPE_LPAREN,
  TOKEN_TYPE_RPAREN,
  TOKEN_TYPE_EQ,
  TOKEN_TYPE_LT,
  TOKEN_TYPE_LTEQ,
  TOKEN_TYPE_EQEQ,
  TOKEN_TYPE_GT,
  TOKEN_TYPE_GTEQ,
  TOKEN_TYPE_QUESTION_MARK,

  TOKEN_TYPE_NUMBER,                    // js conpatible
  TOKEN_TYPE_STRING,                    // js compatible
} TokenType;

static inline bulp_bool token_type_is_bareword (TokenType ttype)
{
  return ttype <= TOKEN_TYPE_BAREWORD;
}

static const char *token_type_to_string (TokenType type)
{
#define WRITE_CASE(shortname) case TOKEN_TYPE_##shortname: return #shortname
  switch (type)
  {
  WRITE_CASE(NAMESPACE);
  WRITE_CASE(STRUCT);
  WRITE_CASE(UNION);
  WRITE_CASE(PACKED);
  WRITE_CASE(MESSAGE);
  WRITE_CASE(VERSION);
  WRITE_CASE(DEFAULT);
  WRITE_CASE(VOID);
  WRITE_CASE(BAREWORD);
  WRITE_CASE(DOT);
  WRITE_CASE(COLON);
  WRITE_CASE(SEMICOLON);
  WRITE_CASE(LBRACE);
  WRITE_CASE(RBRACE);
  WRITE_CASE(LBRACKET);
  WRITE_CASE(RBRACKET);
  WRITE_CASE(LPAREN);
  WRITE_CASE(RPAREN);
  WRITE_CASE(LT);
  WRITE_CASE(LTEQ);
  WRITE_CASE(EQEQ);
  WRITE_CASE(GT);
  WRITE_CASE(GTEQ);
  WRITE_CASE(NUMBER);
  WRITE_CASE(STRING);
  default: return "*bad token-type*";
  }
#undef WRITE_CASE
}

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

static inline bulp_bool
is_bareword_char (uint8_t c)
{
  return ('a' <= c && c <= 'z')
      || ('0' <= c && c <= '9')
      || c == '_'
      || ('A' <= c && c <= 'Z');
}

static unsigned
find_bareword_length (size_t len, const uint8_t *data, size_t offset)
{
  unsigned rv = 1;
  while (offset + rv < len && is_bareword_char (data[offset+rv]))
    rv++;
  return rv;
}

/* --- json-compatible tokens --- */

/* ------------------------- Tokenization ------------------------- */
static TokenizeResult
tokenize (const char    *filename,
          size_t         length,
          const uint8_t *data,
          unsigned      *lineno_inout)
{
  TokenizeResult rv = {NULL, 0, NULL};
  
  unsigned tokens_alloced = 128;
  Token *tokens = malloc (sizeof (Token) * tokens_alloced);
  unsigned n_tokens = 0;
  Token tmp;

  #define APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(shortname, len)      \
    do{                                                                \
      tmp.type = TOKEN_TYPE_ ## shortname;                             \
      tmp.byte_length = (len);                                         \
      tmp.byte_offset = offset;                                        \
      tmp.line_no = *lineno_inout;                                     \
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
        case '.':
          if (offset + 2 < length && data[offset+1] == '.' && data[offset+2] == '.')
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(DOTDOTDOT, 3);
          else
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(DOT, 1);
          break;
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
        case '=':
          if (offset + 1 < length && data[offset + 1] == '=')
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(EQEQ, 2);
          else
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(EQ, 1);
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          {
            unsigned nlen = bulp_json_find_number_length (length, data, offset, filename, lineno_inout, &rv.failed);
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
            switch (tmp.byte_length) {
              case 4:
                if (memcmp (data + offset, "void", 4) == 0) tmp.type = TOKEN_TYPE_VOID;
                break;
              case 5:
                if (memcmp (data + offset, "union", 5) == 0) tmp.type = TOKEN_TYPE_UNION;
                break;
              case 6:
                if (memcmp (data + offset, "struct", 6) == 0) tmp.type = TOKEN_TYPE_STRUCT;
                if (memcmp (data + offset, "packed", 6) == 0) tmp.type = TOKEN_TYPE_PACKED;
                break;
              case 7:
                if (memcmp (data + offset, "version", 7) == 0) tmp.type = TOKEN_TYPE_VERSION;
                if (memcmp (data + offset, "default", 7) == 0) tmp.type = TOKEN_TYPE_DEFAULT;
                if (memcmp (data + offset, "message", 7) == 0) tmp.type = TOKEN_TYPE_MESSAGE;
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
            unsigned nlen = bulp_json_find_quoted_string_length (length, data, offset, filename, lineno_inout, &rv.failed);
            if (nlen == 0)
              goto error_cleanup;
            APPEND_TMP_TO_TOKENS_AND_ADVANCE_OFFSET(STRING, nlen);
            break;
          }

        default:
          rv.failed = bulp_error_new_unexpected_character (data[offset], filename, *lineno_inout);
          goto error_cleanup;
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
  return rv;
}

/* ------------------------- Parsing ------------------------- */
static unsigned
parse_dotted_name_from_tokens (const char * filename,
                               unsigned     n_tokens,
                               Token       *tokens,
                               BulpError **error)
{
  unsigned rv = 1;
  while (rv + 2 < n_tokens
       && tokens[rv].type == TOKEN_TYPE_DOT)
    {
      if (tokens[rv+1].type != TOKEN_TYPE_BAREWORD)
        {
          *error = bulp_error_new_parse (filename, tokens[rv+1].line_no, "expected bareword, got %s", token_type_to_string(tokens[rv+1].type));
          return 0;
        }
     rv += 2;
   }
  return rv;
}

static unsigned
parse_format (BulpImports *imports,
              const char *filename,
              const uint8_t *data,
              unsigned n_tokens,
              Token *tokens,
              BulpFormat **format_out,
              BulpError **error)
{
  unsigned tokens_used = 0;

  // parse dotted name into components.
  unsigned n_dotted_name_tokens = parse_dotted_name_from_tokens (filename, n_tokens, tokens, error);
  if (n_dotted_name_tokens == 0)
    return 0;
  assert(n_dotted_name_tokens % 2 == 1);
  unsigned n_names = (n_dotted_name_tokens + 1) / 2;

  BulpFormat *cur_format = NULL;

  char **strs = alloca (sizeof (char *) * n_names);
  unsigned str_heap_len = 0;
  for (unsigned i = 0; i < n_names; i++)
    str_heap_len += tokens[2*i].byte_length + 1;
  char *str_heap = alloca (str_heap_len);
  char *at = str_heap;
  for (unsigned i = 0; i < n_names; i++)
    {
      const char *name_start = (char *) data + tokens[2*i].byte_offset;
      strs[i] = at;
      memcpy (at, name_start, tokens[2*i].byte_length);
      at += tokens[2*i].byte_length;
      *at++ = 0;
    }

  cur_format = bulp_imports_lookup (imports, n_names, strs);
  if (cur_format == NULL)
    {
      // convert to dotted name
      at = str_heap;
      for (unsigned i = 0; i < n_names - 1; i++)
        {
          at += tokens[2*i].byte_length;
          *at++ = '.';
        }
      
      // return unknown-format error
      *error = bulp_error_new_unknown_format (filename, tokens[0].line_no, str_heap);
      return 0;
    }
  // handle [] or ? suffixes
  while (tokens_used < n_tokens)
    {
      switch (tokens[tokens_used].type)
        {
          case TOKEN_TYPE_LBRACKET:
            if (tokens_used + 1 == n_tokens)
              {
                *error = bulp_error_new_parse (filename, tokens[tokens_used].line_no,
                                                  "expected '%c' got EOF",
                                                  BULP_RBRACKET_CHAR);
                return 0;
              }
            else
              {
                *error = bulp_error_new_parse (filename, tokens[tokens_used+1].line_no,
                                                  "expected '%c' got %s",
                                                  BULP_RBRACKET_CHAR,
                                                  token_type_to_string (tokens[tokens_used+1].type));
                return 0;
              }
            cur_format = bulp_format_array_of (cur_format);
            tokens_used += 2;
            break;

          case TOKEN_TYPE_QUESTION_MARK:
            if (cur_format->type == BULP_FORMAT_TYPE_OPTIONAL)
              {
                at = str_heap;
                for (unsigned i = 0; i < n_names - 1; i++)
                  {
                    at += tokens[2*i].byte_length;
                    *at++ = '.';
                  }
                *error = bulp_error_new_optional_optional (filename,
                                                           tokens[tokens_used].line_no,
                                                           str_heap);
                return 0;
              }
            cur_format = bulp_format_optional_of (cur_format);
            tokens_used += 1;
            break;

          default:
            goto done_scanning_suffixes;
        }
    }
done_scanning_suffixes:
  *format_out = cur_format;
  return tokens_used;
}

static bulp_bool
is_uint (unsigned len, const uint8_t *str, unsigned *version_out)
{
  char buf[32];
  if (len > 20)
    return BULP_FALSE;
  memcpy (buf, str, len);
  buf[len] = 0;
  char *end;
  unsigned v = strtoul (buf, &end, 10);
  if (*end != 0)
    return BULP_FALSE;
  *version_out = v;
  return BULP_TRUE;
}

static inline uint32_t
get_uint_with_byte_size (const uint8_t *at, unsigned size)
{

  switch (size) {
    case 1: return *at;
    case 2: return * (uint16_t *) at;
    case 4: return * (uint32_t *) at;
  }
  assert(0);
  return 0;
}
static inline void
set_uint_with_byte_size (uint8_t *at, unsigned size, uint32_t v)
{

  switch (size) {
    case 1: *at = v; return;
    case 2: * (uint16_t *) at = v; return;
    case 4: * (uint32_t *) at = v; return;
  }
  assert(0);
  return;
}


static void copy_value (BulpFormat *format,
                        void       *dst,
                        const void *src);

static void *
memdup (unsigned length, const void *data)
{
  void *rv = malloc (length);
  memcpy (rv, data, length);
  return rv;
}
static void
copy_value_post_memcpy (BulpFormat *format,
                        void       *dst)
{
  switch (format->type)
    {
    case BULP_FORMAT_TYPE_BINARY_DATA:
      {
        BulpBinaryData *bd = dst;
        bd->data = memdup (bd->length, bd->data);
        break;
      }
    case BULP_FORMAT_TYPE_STRING:
      {
        BulpString *s = dst;
        s->str = memdup (s->length + 1, s->str);             // +1 for NUL
        break;
      }
    case BULP_FORMAT_TYPE_STRUCT:
      {
        BulpFormatStructMember *m = format->v_struct.members;
        BulpFormatStructMember *end_m = m + format->v_struct.n_members;
        while (m < end_m)
          {
            if (!m->format->base.copy_with_memcpy)
              copy_value_post_memcpy (m->format, (char*)dst + m->native_offset);
            m++;
          }
        return;
      }
    case BULP_FORMAT_TYPE_UNION:
      {
        BulpFormatUnionCase *c;
        uint32_t case_value = get_uint_with_byte_size (dst, format->v_union.native_type_size);
        c = bulp_format_union_lookup_by_value (format, case_value);
        assert (c != NULL);
        if (c->case_format != NULL && !c->case_format->base.copy_with_memcpy)
          {
            void *union_info = (char *)dst + format->v_union.native_info_offset;
            copy_value_post_memcpy (c->case_format, union_info);
          }
        break;
      }
    case BULP_FORMAT_TYPE_OPTIONAL:
      if (* (void **) dst == NULL)
        return;
      void *cpy = malloc (sizeof (format->v_optional.subformat));
      copy_value (format->v_optional.subformat, cpy, dst);
      * (void **) dst = cpy;
      return;

    default:
      assert(0);
    }
}


static void
copy_value (BulpFormat *format,
            void       *dst,
            const void *src)
{
  memcpy (dst, src, format->base.c_sizeof);
  if (format->base.copy_with_memcpy)
    return;
  copy_value_post_memcpy (format, dst);
}

static unsigned
skip_json_value (const char *filename,
                 unsigned    n_tokens,
                 Token      *tokens,
                 BulpError **error)
{
  if (n_tokens == 0)
    {
      *error = bulp_error_new_premature_eof (filename, "premature eof expected JSON value of unknown format");
      return 0;
    }
  if (token_type_is_bareword (tokens[0].type)
   || tokens[0].type == TOKEN_TYPE_NUMBER
   || tokens[0].type == TOKEN_TYPE_STRING)
    return 1;
  if (tokens[0].type == TOKEN_TYPE_LBRACE)
    {
      unsigned at = 1;
      while (at < n_tokens && tokens[at].type != TOKEN_TYPE_RBRACE)
        {
          if (tokens[at].type == TOKEN_TYPE_STRING
           || token_type_is_bareword (tokens[at].type))
            {
              at++;
            }
          else
            {
              *error = bulp_error_new_parse (filename, tokens[at].line_no,
                             "expected key in JSON object, got %s",
                             token_type_to_string (tokens[at].type));
              return 0;
            }
          if (at < n_tokens && tokens[at].type == TOKEN_TYPE_COLON)
            {
              at++;
              unsigned jtok = skip_json_value(filename, n_tokens-at, tokens+at, error);
              if (jtok == 0)
                return 0;
              jtok += at;
            }
          if (at < n_tokens && tokens[at].type == TOKEN_TYPE_COMMA)
            at++;
        }
      if (at >= n_tokens)
        {
          *error = bulp_error_new_premature_eof (filename, "in object of unknnown format");
          return 0;
        }
      assert(tokens[at].type == TOKEN_TYPE_RBRACE);
      at++;
      return at;
    }
  else if (tokens[0].type == TOKEN_TYPE_LBRACKET)
    {
      unsigned at = 1;
      while (at < n_tokens && tokens[at].type != TOKEN_TYPE_RBRACKET)
        {
          unsigned jtok = skip_json_value (filename, n_tokens-at, tokens+at, error);
          if (jtok == 0)
            return 0;
          at += jtok;
          // optional ,
          if (at < n_tokens && tokens[at].type == TOKEN_TYPE_COMMA)
            at++;
        }
      if (at >= n_tokens)
        {
          *error = bulp_error_new_premature_eof (filename, "in array of unknnown format");
          return 0;
        }
      assert(tokens[at].type == TOKEN_TYPE_RBRACKET);
      at++;
      return at;
    }
  else
    {
      *error = bulp_error_new_parse (filename, tokens[0].line_no,
                  "expected JSON value of unknown format, got %s",
                  token_type_to_string (tokens[0].type));
      return 0;
    }
}

static unsigned
parse_json_value (const char *filename,
                  const uint8_t *file_data,
                  unsigned    n_tokens,
                  Token      *tokens,
                  BulpFormat *format,
                  void       *value_out,
                  BulpError **error)
{
  switch (format->type)
    {
    case BULP_FORMAT_TYPE_BINARY_DATA:
      {
        if (tokens[0].type != TOKEN_TYPE_STRING)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                                           "binary-data expected (which should be a quoted string of hex), got %s",
                                           token_type_to_string (tokens[0].type));
            return 0;
          }
        const char *at = (const char *) file_data + tokens[0].byte_offset + 1;
        const char *end = at + tokens[0].byte_length - 2;
        unsigned line_no = tokens[0].line_no;
        BulpBinaryData bin_data;
        unsigned bin_data_alloced = 16;
        bin_data.length = 0;
        bin_data.data = malloc (bin_data_alloced);

        while (at < end)
          {
            uint8_t byte;
            if (*at == ' ') { at++; continue; }
            else if (*at == '\n') { at++; line_no++; continue; }
            else if ('0' <= *at && *at <= '9')
              byte = 16 * (*at - '0');
            else if ('a' <= *at && *at <= 'f')
              byte = 16 * (10 + *at - 'a');
            else if ('A' <= *at && *at <= 'F')
              byte = 16 * (10 + *at - 'A');
            else
              {
                *error = bulp_error_new_unexpected_character (*at, filename, line_no);
                free (bin_data.data);
                return 0;
              }
            at++;
            if (at == end)
              {
                *error = bulp_error_new_parse (filename, line_no,
                                               "half-byte found in binary-data");
                free (bin_data.data);
                return 0;
              }
            if ('0' <= *at && *at <= '9')
              byte += (*at - '0');
            else if ('a' <= *at && *at <= 'f')
              byte += (10 + *at - 'a');
            else if ('A' <= *at && *at <= 'F')
              byte += (10 + *at - 'A');
            else
              {
                *error = bulp_error_new_unexpected_character (*at, filename, line_no);
                free (bin_data.data);
                return 0;
              }
            at++;
            if (bin_data.length == bin_data_alloced)
              {
                // double buffer in size
                bin_data_alloced *= 2;
                bin_data.data = realloc (bin_data.data, bin_data_alloced);
              }
            bin_data.data[bin_data.length++] = byte;
          }
        * (BulpBinaryData *) value_out = bin_data;
        return 1;
      }
    case BULP_FORMAT_TYPE_PACKED:
      {
        memset (value_out, 0, format->base.c_sizeof);
        if (tokens[0].type != TOKEN_TYPE_LBRACE)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                                           "expected '%c', got %s",
                                           BULP_LBRACE_CHAR,
                                           token_type_to_string (tokens[0].type));
            return 0;
          }
 
        unsigned at = 1;
        bulp_bool comma_allowed = BULP_FALSE;
        while (at < n_tokens && tokens[at].type != TOKEN_TYPE_RBRACE)
          {
            if (comma_allowed && tokens[at].type == TOKEN_TYPE_COMMA)
              {
                comma_allowed = BULP_FALSE;
                at++;
                continue;
              }

            const char *name_start = (const char *) file_data + tokens[at].byte_offset;
            const char *name_end = name_start + tokens[at].byte_length;
            if (tokens[at].type == TOKEN_TYPE_STRING)
              {
                name_start++;
                name_end--;
              }
            else if (token_type_is_bareword (tokens[at].type))
              {
                /* ok */
              }
            else
              {
                *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                               "packed field-name expected, got %s",
                                               token_type_to_string (tokens[at].type));
                return 0;
              }

            BulpFormatPackedElement *elt = bulp_format_packed_lookup_element (format, name_end-name_start, name_start);
            if (elt == NULL)
              {
                *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                               "packed field-name '%.*s' not found",
                                               (int) (name_end - name_start),
                                               name_start);
                return 0;
              }

            if (at + 2 >= n_tokens)
              {
                *error = bulp_error_new_premature_eof (filename, "premature eof in constant packed value (after name)");
                return 0;
              }

            unsigned value;
            if (tokens[at+1].type != TOKEN_TYPE_COLON)
              {
                value = 1;
                at++;
              }
            else
              {
                if (tokens[at+2].type != TOKEN_TYPE_NUMBER)
                  {
                    *error = bulp_error_new_parse (filename, tokens[at+2].line_no,
                                      "expected number for packed value for %s",
                                      elt->name);

                    return 0;
                  }
                if (!is_uint (tokens[at+2].byte_length,
                              file_data + tokens[at+2].byte_offset, 
                              &value))
                  {
                    *error = bulp_error_new_parse (filename, tokens[at+2].line_no,
                                      "expected unsigned integer for packed value for %s",
                                      elt->name);
                    return 0;
                  }
              }
            bulp_packed_element_set_native (elt, value_out, value);
            at += 3;
            comma_allowed = BULP_TRUE;
          }
        if (at >= n_tokens)
          {
            *error = bulp_error_new_premature_eof (filename, "premature eof in constant packed value");
            return 0;
          }
        assert(tokens[at].type == TOKEN_TYPE_RBRACE);
        return at + 1;
      }

    case BULP_FORMAT_TYPE_STRING:
      {
        if (tokens[0].type != BULP_FORMAT_TYPE_STRING)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                              "expected string for string value");
            return 0;
          }

        // string validation and de-quoting
        const char *at = (char*)file_data + tokens[0].byte_offset;
        const char *end = at + tokens[0].byte_length;

        // convert to literal string
        BulpString s = bulp_json_string_to_literal (at, end, filename, tokens[0].line_no, error);
        if (s.str == NULL)
          {
            return 0;
          }
        * ((BulpString *) value_out) = s;
        return 1;
      }

    case BULP_FORMAT_TYPE_ENUM:
      {
        const char *value_name;
        unsigned value_name_len;

        if (token_type_is_bareword (tokens[0].type))
          {
            value_name_len = tokens[0].byte_length;
            value_name = (const char *) file_data + tokens[0].byte_offset;
          }
        else if (tokens[0].type == TOKEN_TYPE_STRING)
          {
            value_name_len = tokens[0].byte_length - 2;
            value_name = (const char *) file_data + tokens[0].byte_offset + 1;
          }
        else
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                                           "value of enum must be bareword or string, got %s",
                                           token_type_to_string (tokens[0].type));
            return 0;
          }
        BulpFormatEnumValue *enum_value;
        enum_value = bulp_format_enum_lookup_by_name (format,
                                                      value_name_len,
                                                      value_name);
        if (enum_value == NULL)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                              "bad value for enum: %.*s",
                              (int) value_name_len,
                              value_name);
            return 0;
          }

        switch (format->base.c_sizeof)
          {
            case 1: * (uint8_t *) value_out = enum_value->value; break;
            case 2: * (uint16_t *) value_out = enum_value->value; break;
            case 4: * (uint32_t *) value_out = enum_value->value; break;
            default: assert(0);
          }
        return 1;
      }
              
    case BULP_FORMAT_TYPE_INT:
      {
        if (tokens[0].type != TOKEN_TYPE_NUMBER)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                                           "expected number, got %s",
                                           token_type_to_string (tokens[0].type));
            return 0;
          }
        if (tokens[0].byte_length > MAX_INTEGER_LENGTH)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                                           "integer value too long");
            return 0;
          }
        char buf[MAX_INTEGER_LENGTH + 1];
        memcpy (buf, file_data + tokens[0].byte_offset, tokens[0].byte_length);
        buf[tokens[0].byte_length] = 0;
        uint64_t u64_value;
        if (format->v_int.is_signed)
          {
            char *end;
            int64_t i = strtoll (buf, &end, 0);
            if (end == buf || *end != 0)
              {
                *error = bulp_error_new_parse (filename, tokens[0].line_no,
                                            "error parsing signed integer");
                return 0;
              }

            // TODO: check range vs byte_size

            // note: we store the signed integer in an unsigned,
            // but that's because it doesn't matter for the 
            // storage code that switches on byte_size.
            u64_value = i;
          }
        else
          {
            char *end;
            u64_value = strtoull (buf, &end, 0);
            if (end == buf || *end != 0)
              {
                *error = bulp_error_new_parse (filename, tokens[0].line_no,
                                            "error parsing unsigned integer");
                return 0;
              }

            // TODO: check range vs byte_size
          }

        switch (format->v_int.byte_size)
          {
            case 1: * (uint8_t *) value_out = u64_value; break;
            case 2: * (uint16_t *) value_out = u64_value; break;
            case 4: * (uint32_t *) value_out = u64_value; break;
            case 8: * (uint64_t *) value_out = u64_value; break;
            default: assert(0);
          }

        return 1;
      }

    case BULP_FORMAT_TYPE_FLOAT:
      {
        char buf[MAX_FLOAT_LENGTH+1];
        if (tokens[0].type != TOKEN_TYPE_NUMBER)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                                           "expected number, got %s",
                                           token_type_to_string (tokens[0].type));
            return 0;
          }
        if (tokens[0].byte_length > MAX_FLOAT_LENGTH)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                                           "floating-point value too long");
            return 0;
          }
        memcpy (buf, file_data + tokens[0].byte_offset, tokens[0].byte_length);
        buf[tokens[0].byte_length] = 0;

        char *end;
        switch (format->v_float.float_type)
          {
          case BULP_FLOAT_TYPE_FLOAT32:
            * (float *) value_out = strtod (buf, &end);
            break;
          case BULP_FLOAT_TYPE_FLOAT64:
            * (double *) value_out = strtod (buf, &end);
            break;
          default:
            assert(0);
          }
        if (*end != 0)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                               "error parsing floating point number %s", buf);
            return 0;
          }
      }
      return 1;

    case BULP_FORMAT_TYPE_STRUCT:
      {
        uint8_t *values_found = alloca (format->v_struct.n_members);
        memset (values_found, 0, format->v_struct.n_members);
        if (tokens[0].type != TOKEN_TYPE_LBRACE)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                           "expected %c at start of struct",
                           BULP_LBRACE_CHAR);
            return 0;
          }
        unsigned at = 1;
        while (at < n_tokens && tokens[at].type != TOKEN_TYPE_RBRACE)
          {
            // name (string or bareword)
            unsigned name_len;
            const char *name_start;
            if (tokens[at].type == TOKEN_TYPE_STRING)
              {
                name_len = tokens[at].byte_length - 2;
                name_start = (char*)file_data + tokens[at].byte_offset + 1;
              }
            else if (token_type_is_bareword (tokens[at].type))
              {
                name_len = tokens[at].byte_length;
                name_start = (char*)file_data + tokens[at].byte_offset;
              }
            else
              {
                *error = bulp_error_new_parse (filename, tokens[at].line_no,
                    "expected member name (either bareword or string), got %s",
                    token_type_to_string (tokens[at].type));
                return 0;
              }
            BulpFormatStructMember *m;
            m = bulp_format_struct_lookup_by_name (format, name_len, name_start);
            if (m == NULL)
              {
                *error = bulp_error_new_parse (filename, tokens[at].line_no,
                             "missing member %.*s in %s",
                             name_len, name_start, format->base.canonical_name);
                return 0;
              }

            // colon
            if (at == n_tokens)
              {
                *error = bulp_error_new_premature_eof (filename, "expected : after member name in structured value");
                return 0;
              }
            if (tokens[at].type != TOKEN_TYPE_COLON)
              {
                *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                       "expected : after member name in structured value, got %s",
                                       token_type_to_string (tokens[at].type));
                return 0;
              }
            at++;

            // value (format depends on name)
            unsigned vtok_used = parse_json_value (filename, file_data,
                                         n_tokens - at, tokens + at,
                                         m->format,
                                         (char*)value_out + m->native_offset,
                                         error);
            if (vtok_used == 0)
              {
                bulp_error_append_message (*error, " (in %s::%s)",
                     format->base.canonical_name,
                     m->name);
                return 0;
              }
            at += vtok_used;
            if (at < n_tokens)
              {
                if (tokens[at].type == TOKEN_TYPE_COMMA)
                  {
                    at++;
                  }
              }
          }
        if (at >= n_tokens)
          {
            *error = bulp_error_new_premature_eof (filename, "premature eof in struct value");
            return 0;
          }
        assert(tokens[at].type == TOKEN_TYPE_RBRACE);
        at++;

        for (unsigned i = 0; i < format->v_struct.n_members; i++)
          if (!values_found[i])
            {
              BulpFormatStructMember *m = format->v_struct.members + i;
              if (m->versioning_info != NULL && m->versioning_info->default_value != NULL)
                {
                  copy_value (m->format, (char*) value_out + m->native_offset, m->versioning_info->default_value);
                }
              else if (m->format->base.is_zeroable)
                {
                  *error = bulp_error_new_parse (filename, tokens[0].line_no, "missing required member %s", m->name);
                  return 0;
                }
            }
        return at;
      }

    case BULP_FORMAT_TYPE_UNION:
      {
        if (tokens[0].type != TOKEN_TYPE_LBRACE)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                           "expected %c at start of struct",
                           BULP_LBRACE_CHAR);
            return 0;
          }
        unsigned at = 1;
        // name (string or bareword)
        unsigned name_len;
        const char *name_start;
        if (tokens[at].type == TOKEN_TYPE_STRING)
          {
            name_len = tokens[at].byte_length - 2;
            name_start = (char*)file_data + tokens[at].byte_offset + 1;
          }
        else if (token_type_is_bareword (tokens[at].type))
          {
            name_len = tokens[at].byte_length;
            name_start = (char*)file_data + tokens[at].byte_offset;
          }
        else
          {
            *error = bulp_error_new_parse (filename, tokens[at].line_no,
                "expected member name (either bareword or string), got %s",
                token_type_to_string (tokens[at].type));
            return 0;
          }
        BulpFormatUnionCase *c;
        c = bulp_format_union_lookup_by_name (format, name_len, name_start);
        if (c == NULL)
          {
            *error = bulp_error_new_parse (filename, tokens[at].line_no,
                         "missing case %.*s in %s",
                         name_len, name_start, format->base.canonical_name);
            return 0;
          }
       set_uint_with_byte_size (value_out, format->v_union.native_type_size, c->value);

        // colon
        if (at == n_tokens)
          {
            *error = bulp_error_new_premature_eof (filename, "expected : after case name in union value");
            return 0;
          }
        if (tokens[at].type != TOKEN_TYPE_COLON)
          {
            if (c->case_format == NULL)
              {
                // { "name": null }
                at++;
                if (at == n_tokens)
                  {
                    *error = bulp_error_new_premature_eof (filename, "expected null for empty union case value, got end-of-file");
                    return 0;
                  }
                if (tokens[at].type != TOKEN_TYPE_BAREWORD
                 || tokens[at].byte_length != 4
                 || memcmp (file_data + tokens[at].byte_offset, "null", 4) != 0)
                  {
                    *error = bulp_error_new_parse (filename, tokens[at].line_no,
                      "expected null for empty union case value, got %s",
                      token_type_to_string (tokens[at].type));
                    return 0;
                  }
                at++;
              }
            else
              { 
                *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                       "expected : after case in union value, got %s",
                                       token_type_to_string (tokens[at].type));
                return 0;
              }
          }
        else
          {
            at++;               // skip colon
          }

        if (c->case_format != NULL)
          {
            // value (format depends on name)
            unsigned vtok_used = parse_json_value (filename, file_data,
                                         n_tokens - at, tokens + at,
                                         c->case_format,
                                         (char*)value_out + format->v_union.native_info_offset,
                                         error);
            if (vtok_used == 0)
              {
                bulp_error_append_message (*error, " (in %s::%s)",
                     format->base.canonical_name,
                     c->name);
                return 0;
              }
            at += vtok_used;
            #if 0
            if (at < n_tokens)
              {
                if (tokens[at].type == TOKEN_TYPE_COMMA)
                  {
                    at++;
                  }
              }
            #endif
          }
        if (at >= n_tokens)
          {
            *error = bulp_error_new_premature_eof (filename, "premature eof in struct value");
            return 0;
          }
        if (tokens[at].type != TOKEN_TYPE_RBRACE)
          {
            *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                   "expected %c after union value, got %s",
                                   BULP_RBRACE_CHAR,
                                   token_type_to_string (tokens[at].type));
            return 0;
          }
        at++;
        return at;
      }

    case BULP_FORMAT_TYPE_MESSAGE:
      {
        uint8_t *values_found = alloca (format->v_struct.n_members);
        memset (values_found, 0, format->v_struct.n_members);
        if (tokens[0].type != TOKEN_TYPE_LBRACE)
          {
            *error = bulp_error_new_parse (filename, tokens[0].line_no,
                           "expected %c at start of struct",
                           BULP_LBRACE_CHAR);
            return 0;
          }
        unsigned at = 1;
        while (at < n_tokens && tokens[at].type != TOKEN_TYPE_RBRACE)
          {
            // name (string or bareword)
            unsigned name_len;
            const char *name_start;
            if (tokens[at].type == TOKEN_TYPE_STRING)
              {
                name_len = tokens[at].byte_length - 2;
                name_start = (char*)file_data + tokens[at].byte_offset + 1;
              }
            else if (token_type_is_bareword (tokens[at].type))
              {
                name_len = tokens[at].byte_length;
                name_start = (char*)file_data + tokens[at].byte_offset;
              }
            else
              {
                *error = bulp_error_new_parse (filename, tokens[at].line_no,
                    "expected member name (either bareword or string), got %s",
                    token_type_to_string (tokens[at].type));
                return 0;
              }
            // colon
            if (at == n_tokens)
              {
                *error = bulp_error_new_premature_eof (filename, "expected : after member name in structured value");
                return 0;
              }
            if (tokens[at].type != TOKEN_TYPE_COLON)
              {
                *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                       "expected : after member name in structured value, got %s",
                                       token_type_to_string (tokens[at].type));
                return 0;
              }
            at++;

            BulpFormatMessageField *mf;
            mf = bulp_format_message_lookup_by_name (format, name_len, name_start);
            if (mf == NULL)
              {
                unsigned nskip = skip_json_value (filename, n_tokens - at, tokens + at, error);
                if (nskip == 0)
                  return 0;
                continue;
              }


            // value (format depends on name)
            unsigned vtok_used = parse_json_value (filename, file_data,
                                         n_tokens - at, tokens + at,
                                         mf->field_format,
                                         (char*)value_out + mf->native_offset,
                                         error);
            if (vtok_used == 0)
              {
                bulp_error_append_message (*error, " (in %s::%s)",
                     format->base.canonical_name,
                     mf->name);
                return 0;
              }
            at += vtok_used;
            if (at < n_tokens)
              {
                if (tokens[at].type == TOKEN_TYPE_COMMA)
                  {
                    at++;
                  }
              }
          }
        if (at >= n_tokens)
          {
            *error = bulp_error_new_premature_eof (filename, "premature eof in struct value");
            return 0;
          }
        assert(tokens[at].type == TOKEN_TYPE_RBRACE);
        at++;

        for (unsigned i = 0; i < format->v_struct.n_members; i++)
          if (!values_found[i])
            {
              BulpFormatStructMember *m = format->v_struct.members + i;
              if (m->versioning_info != NULL && m->versioning_info->default_value != NULL)
                {
                  copy_value (m->format, (char*) value_out + m->native_offset, m->versioning_info->default_value);
                }
              else if (m->format->base.is_zeroable)
                {
                  *error = bulp_error_new_parse (filename, tokens[0].line_no, "missing required member %s", m->name);
                  return 0;
                }
            }
        return at;
      }

    case BULP_FORMAT_TYPE_OPTIONAL:
      {
        if (tokens[0].type == TOKEN_TYPE_BAREWORD
         && tokens[0].byte_length == 4
         && memcmp (file_data + tokens[0].byte_offset, "null", 4) == 0)
          {
            *(void **) value_out = NULL;
            return 1;
          }

        // else, parse subvalue
        BulpFormat *subf = format->v_optional.subformat;
        void *subvalue = malloc (subf->base.c_sizeof);
        unsigned ntok = parse_json_value (filename, file_data,
                                          n_tokens, tokens,
                                          subf, subvalue,
                                          error);
        if (ntok == 0)
          {
            free (subvalue);
            return 0;
          }

        * (void **) value_out = subvalue;
        return ntok;
      }
    default:
      assert(BULP_FALSE);
      return 0;
    }
}

static void
destruct_parsed_json_value (BulpFormat *format, void *native_value)
{
  if (format->base.copy_with_memcpy)
    return;
  switch (format->type)
    {
    case BULP_FORMAT_TYPE_BINARY_DATA:
      {
        uint8_t *bd = ((BulpBinaryData *) native_value)->data;
        if (bd != NULL)
          free (bd);
        break;
      }

    case BULP_FORMAT_TYPE_STRING:
      {
        char *str = ((BulpString *) native_value)->str;
        if (str != NULL)
          free (str);
        break;
      }

    case BULP_FORMAT_TYPE_STRUCT:
      for (unsigned i = 0; i < format->v_struct.n_members; i++)
        destruct_parsed_json_value (format->v_struct.members[i].format,
                        (char *) native_value + format->v_struct.members[i].native_offset);
      break;

    case BULP_FORMAT_TYPE_UNION:
      {
        uint32_t uv = get_uint_with_byte_size (native_value, format->v_union.native_type_size);
        BulpFormatUnionCase *uc = bulp_format_union_lookup_by_value (format, uv);
        if (uc != NULL && uc->case_format != NULL && !uc->case_format->base.copy_with_memcpy)
          destruct_parsed_json_value (uc->case_format,
                 (char *) native_value + format->v_union.native_info_offset);
        break;
      }

    case BULP_FORMAT_TYPE_MESSAGE:
      for (unsigned i = 0; i < format->v_message.n_fields; i++)
        destruct_parsed_json_value (format->v_message.fields[i].field_format,
                        (char *) native_value + format->v_message.fields[i].native_offset);
      break;

    case BULP_FORMAT_TYPE_OPTIONAL:
      {
        void *v = * (void **) native_value;
        if (v != NULL)
          {
            destruct_parsed_json_value (format->v_optional.subformat, v);
            free (v);
          }
        break;
      }

    default: break;
    }
}

static unsigned
parse_member_versioning_info (const char *filename,
                              unsigned cur_version,
                              const uint8_t *file_data,
                              unsigned n_tokens,
                              Token *tokens,
                              size_t member_name_len,
                              const char *member_name,
                              BulpFormat *member_format,
                              BulpMemberVersioningInfo *v_out,
                              BulpError **error)
{
  assert(tokens[0].type == TOKEN_TYPE_LBRACE);
  unsigned at = 1;
  while (at < n_tokens)
    {
      if (tokens[at].type == TOKEN_TYPE_RBRACE)
        {
          return at + 1;
        }
      if (tokens[at].type == TOKEN_TYPE_VERSION)
        {
          if (at + 1 == n_tokens)
            {
              *error = bulp_error_new_premature_eof (filename, "EOF after 'version'");
              return 0;
            }
          if (tokens[at+1].type != TOKEN_TYPE_COLON)
            {
              *error = bulp_error_new_parse (filename, tokens[at+1].line_no,
                               "expected ':' after version, got %s",
                               token_type_to_string (tokens[at+1].type));
              return 0;
            }
          at += 2;
          unsigned n_ranges = 0;
          unsigned ranges_alloced = 0;
          BulpVersionRange *ranges = NULL;
          while (at < n_tokens)
            {
              if (tokens[at].type == TOKEN_TYPE_NUMBER)
                {
                  BulpVersionRange range;

                  // parse version or version range
                  unsigned start_version, end_version;
                  if (!is_uint (tokens[at].byte_length,
                                file_data + tokens[at].byte_offset,
                                &start_version))
                    {
                      *error = bulp_error_new_parse (filename, tokens[at].line_no, "expected unsigned integer for version number");
                      goto error_cleanup;
                    }
                  if (at + 1 < n_tokens
                   && tokens[at+1].type == TOKEN_TYPE_DOTDOTDOT)
                    {
                      if (at + 2 < n_tokens
                       && tokens[at+2].type == TOKEN_TYPE_NUMBER)
                        {
                          if (!is_uint (tokens[at+2].byte_length,
                                        file_data + tokens[at+2].byte_offset,
                                        &end_version))
                            {
                              *error = bulp_error_new_parse (filename, tokens[at].line_no, "expected unsigned integer for version number");
                              goto error_cleanup;
                            }
                          at += 3;
                        }
                      else
                        {
                          end_version = cur_version;
                          at += 2;
                        }
                    }
                  else
                    end_version = start_version;
                  range.start_version = start_version;
                  range.end_version = end_version;
                  if (n_ranges == ranges_alloced)
                    {
                      if (ranges_alloced == 0)
                        {
                          ranges_alloced = 4;
                          ranges = malloc (sizeof (BulpVersionRange) * ranges_alloced);
                        }
                      else
                        {
                          ranges_alloced *= 2;
                          ranges = realloc (ranges, sizeof (BulpVersionRange) * ranges_alloced);
                        }
                    }
                  ranges[n_ranges++] = range;
                }
              else if (tokens[at].type == TOKEN_TYPE_COMMA)
                {
                  // comma: parse new range
                  at++;
                  continue;
                }
              else if (tokens[at].type == TOKEN_TYPE_SEMICOLON
                    || tokens[at].type == TOKEN_TYPE_RBRACE)
                {
                  if (v_out->ranges != NULL)
                    free (v_out->ranges);
                  v_out->n_ranges = n_ranges;
                  v_out->ranges = ranges;
                  if (tokens[at].type == TOKEN_TYPE_RBRACE)
                    {
                      at++;
                      return at;
                    }
                  at++;
                  break;
                }
            }
        }
      else if (tokens[at].type == TOKEN_TYPE_DEFAULT)
        {
          if (at + 1 == n_tokens)
            {
              *error = bulp_error_new_premature_eof (filename, "EOF after 'default'");
              return 0;
            }
          if (tokens[at+1].type != TOKEN_TYPE_COLON)
            {
              *error = bulp_error_new_parse (filename, tokens[at+1].line_no,
                               "expected ':' after default, got %s",
                               token_type_to_string (tokens[at+1].type));
              return 0;
            }
          at += 2;
          void *value = malloc (member_format->base.c_sizeof);
          memset (value, 0, member_format->base.c_sizeof);
          unsigned vtoks = parse_json_value (filename, file_data, n_tokens - at, tokens + at,
                                             member_format, value, error);
          if (vtoks == 0)
            {
              bulp_error_append_message (*error, " (parsing member %.*s)",(int) member_name_len, member_name);
              goto error_cleanup;
            }
          if (v_out->default_value != NULL)
            {
              destruct_parsed_json_value (member_format, v_out->default_value);
              free (v_out->default_value);
            }
          v_out->default_value = value;
          at += vtoks;
        }
      else
        {
          *error = bulp_error_new_parse (filename, tokens[at].line_no,
                           "expected 'version', 'default', got %s",
                           token_type_to_string (tokens[at].type));
          return 0;
        }
    }

  *error = bulp_error_new_premature_eof (filename, "EOF in versioning information");
error_cleanup:
  if (v_out->ranges != NULL)
    free (v_out->ranges);
  if (v_out->default_value != NULL)
    {
      destruct_parsed_json_value (member_format, v_out->default_value);
      free (v_out->default_value);
    }
  return 0;
}

static char *
cut_token (const uint8_t *file_data, const Token *token)
{
  char *rv = malloc (token->byte_length + 1);
  memcpy (rv, file_data + token->byte_offset, token->byte_length);
  rv[token->byte_length] = 0;
  return rv;
}

static unsigned
parse_struct_member (BulpImports *imports,
                     const char *filename,
                     const uint8_t *file_data,
                     unsigned n_tokens,
                     Token *tokens,
                     BulpStructMember *member_out,
                     BulpError **error)
{
  BulpFormat *format;
  unsigned fmt_tokens_used = parse_format (imports, filename, file_data, n_tokens, tokens, &format, error);
  if (fmt_tokens_used == 0)
    return 0;
  if (fmt_tokens_used == n_tokens)
    {
      *error = bulp_error_new_premature_eof (filename, "premature eof in structure member");
      return 0;
    }
  unsigned name_token_index = fmt_tokens_used;
  Token *name_tok = tokens + fmt_tokens_used;
  if (name_tok->type != TOKEN_TYPE_BAREWORD)
    {
      *error = bulp_error_new_parse (filename, tokens[fmt_tokens_used].line_no,
                                     "structure name expected, got %s",
                                     token_type_to_string (tokens[fmt_tokens_used].type));
      return 0;
    }
  unsigned tokens_used = fmt_tokens_used + 1;
  if (tokens_used == n_tokens)
    {
      *error = bulp_error_new_premature_eof (filename, "premature eof after structure member");
      return 0;
    }

  /* The semicolon is optional if there's a versioning stanza */
  bulp_bool require_semicolon = BULP_TRUE;

  BulpMemberVersioningInfo versioning = BULP_MEMBER_VERSIONING_INFO_INIT;
  if (tokens[tokens_used].type == TOKEN_TYPE_LBRACE)
    {
      unsigned ntok = parse_member_versioning_info (filename,
                                                    imports->this_ns_version,
                                                    file_data,
                                                    n_tokens - tokens_used,
                                                    tokens + tokens_used,
                                                    name_tok->byte_length,
                                                    (char*)file_data + name_tok->byte_offset,
                                                    format,
                                                    &versioning,
                                                    error);
      
      if (ntok == 0)
        return 0;
      tokens_used += ntok;
      require_semicolon = BULP_FALSE;
    }

  if (tokens_used < n_tokens && tokens[tokens_used].type == TOKEN_TYPE_SEMICOLON)
    {
      tokens_used++;
    }
  else if (require_semicolon)
    {
      if (tokens_used == n_tokens)
        {
          *error = bulp_error_new_premature_eof (filename, "premature eof after structure member; expected semicolon or versioning info");
          return 0;
        }
      else
        {
          *error = bulp_error_new_parse (filename, tokens[tokens_used].line_no, "expected ';' after structure member, got %s", token_type_to_string (tokens[tokens_used].type));
          return 0;
        }
      return 0;
    }

  member_out->format = format;
  member_out->name = cut_token (file_data, tokens + name_token_index);
  return tokens_used;
}

static unsigned
parse_struct_format (BulpImports *imports,
                     const char *filename,
                     const uint8_t *file_data,
                     unsigned n_tokens, Token *tokens,
                     BulpError **error)
{
  unsigned token_at = 0;
  assert(tokens[0].type == TOKEN_TYPE_STRUCT);
  unsigned n_members = 0;
  if (token_at + 1 >= n_tokens || tokens[token_at+1].type == TOKEN_TYPE_BAREWORD)
    {
      *error = bulp_error_new_parse (filename, tokens[token_at].line_no, "expected structure name");
      goto error_cleanup;
    }
  Token *name_tok = tokens + token_at + 1;
  if (token_at + 2 >= n_tokens || tokens[token_at+2].type == TOKEN_TYPE_LBRACE)
    {
      *error = bulp_error_new_parse (filename, tokens[token_at+1].line_no, "expected '%c'", BULP_LBRACE_CHAR);
      goto error_cleanup;
    }
  unsigned members_alloced = 4;
  BulpStructMember *members = malloc(sizeof(BulpStructMember) * members_alloced);
  BulpStructMember member;
  token_at += 3;
  while (token_at < n_tokens && tokens[token_at].type != TOKEN_TYPE_RBRACE)
    {
      if (tokens[token_at].type == TOKEN_TYPE_SEMICOLON)
        {
          token_at++;
          continue;
        }
      unsigned mn = parse_struct_member(imports, filename, file_data, n_tokens - token_at, tokens + token_at, &member, error);
      if (mn == 0)
        goto error_cleanup;
      if (n_members == members_alloced)
        {
          members_alloced *= 2;
          members = realloc (members, sizeof(BulpStructMember) * members_alloced);
        }
      members[n_members++] = member;
    }
  if (token_at == n_tokens)
    {
      *error = bulp_error_new_premature_eof(filename, "premature eof, expected %c after struct members", BULP_RBRACE_CHAR);
      goto error_cleanup;
    }
  token_at++;

  BulpFormat *fmt = bulp_format_new_struct (n_members, members);
  for (unsigned k = 0; k < n_members; k++)
    free ((char*) members[k].name);
  bulp_namespace_add_format (imports->this_ns, name_tok->byte_length,
                             (char*) file_data + name_tok->byte_offset, fmt,
                             BULP_TRUE);
  bulp_format_unref (fmt);
  return token_at;

error_cleanup:
  for (unsigned k = 0; k < n_members; k++)
    {
      free ((char*) members[k].name);
    }
  free (members);
  return 0;
}

static unsigned
parse_packed_format (BulpNamespace *ns,
                     const char    *filename,
                     const uint8_t *file_data,
                     unsigned       n_tokens,
                     Token         *tokens,
                     BulpError    **error)
{
  assert(tokens[0].type == TOKEN_TYPE_PACKED);
  if (n_tokens == 1)
    {
      *error = bulp_error_new_premature_eof (filename,
                                             "expected packed format name");
      return 0;
    }
  if (!token_type_is_bareword (tokens[1].type))
    {
      *error = bulp_error_new_parse (filename, tokens[1].line_no,
                                     "expected packed format name, got %s",
                                     token_type_to_string (tokens[1].type));
      return 0;
    }
  if (n_tokens == 2)
    {
      *error = bulp_error_new_premature_eof (filename,
                                             "expected %c after format name",
                                             BULP_LBRACE_CHAR);
      return 0;
    }
  unsigned at = 2;
  unsigned n_elts = 0;
  unsigned elts_alloced = 4;
  BulpPackedElement *packed_elts = malloc (sizeof (BulpPackedElement) * elts_alloced);
  while (at < n_tokens && tokens[at].type != TOKEN_TYPE_RBRACE)
    {
      // parse name
      const char *name = (char *) file_data + tokens[at].byte_offset;
      unsigned name_len = tokens[at].byte_length;
      if (tokens[at].type == TOKEN_TYPE_BAREWORD)
        {
          //name, name_len are valid
        }
      else if (tokens[at].type == TOKEN_TYPE_STRING)
        {
          name++;
          name_len -= 2;
        }
      else
        {
          *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                         "expected field-name for packed, got %s",
                                         token_type_to_string (tokens[at].type));
          return 0;
        }
      at++;
      if (at >= n_tokens)
        {
          *error = bulp_error_new_premature_eof (filename, "expected token after packed-field name");
          return 0;
        }

      uint32_t n_bits;
      if (tokens[at].type == TOKEN_TYPE_COLON)
        {
          // skip colon
          at++;

          // parse size
          if (at >= n_tokens)
            {
              *error = bulp_error_new_premature_eof (filename, "expected bit-size after : in packed-field");
              return 0;
            }
          if (tokens[at].type != TOKEN_TYPE_NUMBER)
            {
              *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                       "expected number for packed-field size, got %s",
                                       token_type_to_string (tokens[at].type));
              return 0;
            }
          if (!is_uint (tokens[at].byte_length,
                        file_data + tokens[at].byte_offset,
                        &n_bits))
            {
              *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                       "number representing bit-size was not unsigned integer");
              return 0;
            }
          if (n_bits == 0)
            {
              *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                       "bit size of 0 not allowed");
              return 0;
            }
          if (n_bits > 32)
            {
              *error = bulp_error_new_parse (filename, tokens[at].line_no,
                                       "bit size too long, max 32");
              return 0;
            }
          at++;
        }
      else
        {
          n_bits = 1;
        }

      BulpPackedElement packed_elt;
      packed_elt.name_len = name_len;
      packed_elt.name = name;
      packed_elt.n_bits = n_bits;
      if (n_elts == elts_alloced)
        {
          elts_alloced *= 2;
          packed_elts = realloc (packed_elts, sizeof (BulpPackedElement) * elts_alloced);
        }
      packed_elts[n_elts++] = packed_elt;
    }
  if (at >= n_tokens)
    {
      *error = bulp_error_new_premature_eof (filename, "expected %c after packed", BULP_RBRACE_CHAR);
      return 0;
    }
  assert(tokens[at].type == TOKEN_TYPE_RBRACE);
  at++;


  BulpFormat *format = bulp_format_packed_new (n_elts, packed_elts);
  Token *t = tokens + 1;
  bulp_namespace_add_format (ns,
                             t->byte_length,
                             (char*)file_data + t->byte_offset,
                             format, BULP_TRUE);
  bulp_format_unref (format);
  return at;
}
static unsigned
parse_union_case    (BulpImports *imports,
                     const char *filename,
                     const uint8_t *file_data,
                     unsigned n_tokens,
                     Token *tokens,
                     BulpUnionCase *case_out,
                     BulpError **error)
{
  BulpFormat *format;
  unsigned fmt_tokens_used = parse_format (imports, filename, file_data, n_tokens, tokens, &format, error);
  if (fmt_tokens_used == 0)
    return 0;
  if (fmt_tokens_used == n_tokens)
    {
      *error = bulp_error_new_premature_eof (filename, "premature eof in structure member");
      return 0;
    }
  Token *name_tok = tokens + fmt_tokens_used;
  if (name_tok->type != TOKEN_TYPE_BAREWORD)
    {
      *error = bulp_error_new_parse (filename, tokens[fmt_tokens_used].line_no,
                                     "structure name expected, got %s",
                                     token_type_to_string (tokens[fmt_tokens_used].type));
      return 0;
    }
  unsigned tokens_used = fmt_tokens_used + 1;
  if (tokens_used == n_tokens)
    {
      *error = bulp_error_new_premature_eof (filename, "premature eof after structure member");
      return 0;
    }

  bulp_bool has_value = BULP_FALSE;
  uint32_t value = 0;
  if (tokens_used < n_tokens && tokens[tokens_used].type == TOKEN_TYPE_EQ)
    {
      has_value = BULP_TRUE;
      if (tokens_used + 1 >= n_tokens)
        {
          *error = bulp_error_new_premature_eof (filename, "expected number after union-case '='");
          return 0;
        }
      if (tokens[tokens_used+1].type != TOKEN_TYPE_NUMBER)
        {
          *error = bulp_error_new_parse (filename, tokens[tokens_used+1].line_no,
                                         "expected number after union-case '=', got %s",
                                         token_type_to_string (tokens[tokens_used+1].type));
          return 0;
        }
      if (!is_uint (tokens[tokens_used+1].byte_length,
                    file_data + tokens[tokens_used+1].byte_offset,
                    &value))
        {
          *error = bulp_error_new_parse (filename, tokens[tokens_used + 1].line_no,
                                   "number representing union-case id was not unsigned integer");
          return 0;
        }
      tokens_used += 2;
    }

  if (tokens_used >= n_tokens)
    {
      *error = bulp_error_new_premature_eof (filename, "premature eof after union case, expected ;");
      return 0;
    }
  if (tokens[tokens_used].type != TOKEN_TYPE_SEMICOLON)
    {
      *error = bulp_error_new_parse (filename, tokens[tokens_used].line_no, "expected ';' after union case, got %s", token_type_to_string (tokens[tokens_used].type));
      return 0;
    }
  tokens_used++;

  case_out->case_format = format;
  case_out->name = cut_token (file_data, name_tok);
  case_out->has_value = has_value;
  case_out->value = value;
  return tokens_used;
}
static unsigned
parse_union_format  (BulpImports *imports,
                     const char *filename,
                     const uint8_t *file_data,
                     unsigned n_tokens, Token *tokens,
                     BulpError **error)
{
  unsigned token_at = 0;
  assert(tokens[0].type == TOKEN_TYPE_UNION);
  unsigned n_cases = 0;
  if (token_at + 1 >= n_tokens || tokens[token_at+1].type == TOKEN_TYPE_BAREWORD)
    {
      *error = bulp_error_new_parse (filename, tokens[token_at].line_no, "expected structure name");
      goto error_cleanup;
    }
  Token *name_tok = tokens + token_at + 1;
  if (token_at + 2 >= n_tokens || tokens[token_at+2].type == TOKEN_TYPE_LBRACE)
    {
      *error = bulp_error_new_parse (filename, tokens[token_at+1].line_no, "expected '%c'", BULP_LBRACE_CHAR);
      goto error_cleanup;
    }
  unsigned cases_alloced = 4;
  BulpUnionCase *cases = malloc(sizeof(BulpUnionCase) * cases_alloced);
  BulpUnionCase c;
  token_at += 3;
  while (token_at < n_tokens && tokens[token_at].type != TOKEN_TYPE_RBRACE)
    {
      if (tokens[token_at].type == TOKEN_TYPE_SEMICOLON)
        {
          token_at++;
          continue;
        }
      unsigned mn = parse_union_case(imports, filename, file_data, n_tokens - token_at, tokens + token_at, &c, error);
      if (mn == 0)
        goto error_cleanup;
      if (n_cases == cases_alloced)
        {
          cases_alloced *= 2;
          cases = realloc (cases, sizeof(BulpUnionCase) * cases_alloced);
        }
      cases[n_cases++] = c;
    }
  if (token_at == n_tokens)
    {
      *error = bulp_error_new_premature_eof(filename, "premature eof, expected %c after struct members", BULP_RBRACE_CHAR);
      goto error_cleanup;
    }
  token_at++;

  BulpFormat *fmt = bulp_format_new_union (n_cases, cases);
  for (unsigned k = 0; k < n_cases; k++)
    free ((char*) cases[k].name);
  bulp_namespace_add_format (imports->this_ns, name_tok->byte_length,
                             (char*) file_data + name_tok->byte_offset, fmt,
                             BULP_TRUE);
  bulp_format_unref (fmt);
  return token_at;

error_cleanup:
  for (unsigned k = 0; k < n_cases; k++)
    {
      free ((char*) cases[k].name);
    }
  free (cases);
  return 0;
}

/// messages
static unsigned
parse_message_field    (BulpImports *imports,
                        const char *filename,
                        const uint8_t *file_data,
                        unsigned n_tokens,
                        Token *tokens,
                        BulpMessageField *message_field_out,
                        BulpError **error)
{
  BulpFormat *format;
  unsigned fmt_tokens_used = parse_format (imports, filename, file_data, n_tokens, tokens, &format, error);
  if (fmt_tokens_used == 0)
    return 0;
  if (fmt_tokens_used == n_tokens)
    {
      *error = bulp_error_new_premature_eof (filename, "premature eof in structure member");
      return 0;
    }
  Token *name_tok = tokens + fmt_tokens_used;
  if (name_tok->type != TOKEN_TYPE_BAREWORD)
    {
      *error = bulp_error_new_parse (filename, tokens[fmt_tokens_used].line_no,
                                     "structure name expected, got %s",
                                     token_type_to_string (tokens[fmt_tokens_used].type));
      return 0;
    }
  unsigned tokens_used = fmt_tokens_used + 1;
  if (tokens_used == n_tokens)
    {
      *error = bulp_error_new_premature_eof (filename, "premature eof after structure member");
      return 0;
    }

  bulp_bool has_value = BULP_FALSE;
  uint32_t value = 0;
  if (tokens_used < n_tokens && tokens[tokens_used].type == TOKEN_TYPE_EQ)
    {
      has_value = BULP_TRUE;
      if (tokens_used + 1 >= n_tokens)
        {
          *error = bulp_error_new_premature_eof (filename, "expected number after union-case '='");
          return 0;
        }
      if (tokens[tokens_used+1].type != TOKEN_TYPE_NUMBER)
        {
          *error = bulp_error_new_parse (filename, tokens[tokens_used+1].line_no,
                                         "expected number after union-case '=', got %s",
                                         token_type_to_string (tokens[tokens_used+1].type));
          return 0;
        }
      if (!is_uint (tokens[tokens_used+1].byte_length,
                    file_data + tokens[tokens_used+1].byte_offset,
                    &value))
        {
          *error = bulp_error_new_parse (filename, tokens[tokens_used + 1].line_no,
                                   "number representing union-case id was not unsigned integer");
          return 0;
        }
      tokens_used += 2;
    }

  if (tokens_used >= n_tokens)
    {
      *error = bulp_error_new_premature_eof (filename, "premature eof after union case, expected ;");
      return 0;
    }
  if (tokens[tokens_used].type != TOKEN_TYPE_SEMICOLON)
    {
      *error = bulp_error_new_parse (filename, tokens[tokens_used].line_no, "expected ';' after union case, got %s", token_type_to_string (tokens[tokens_used].type));
      return 0;
    }
  tokens_used++;

  message_field_out->field_format = format;
  message_field_out->name = cut_token (file_data, name_tok);
  message_field_out->set_value = has_value;
  message_field_out->value_if_set = value;
  return tokens_used;
}
static unsigned
parse_message_format(BulpImports *imports,
                     const char *filename,
                     const uint8_t *file_data,
                     unsigned n_tokens, Token *tokens,
                     BulpError **error)
{
  unsigned token_at = 0;
  assert(tokens[0].type == TOKEN_TYPE_MESSAGE);
  unsigned n_fields = 0;
  if (token_at + 1 >= n_tokens || tokens[token_at+1].type == TOKEN_TYPE_BAREWORD)
    {
      *error = bulp_error_new_parse (filename, tokens[token_at].line_no, "expected message name");
      goto error_cleanup;
    }
  Token *name_tok = tokens + token_at + 1;
  if (token_at + 2 >= n_tokens || tokens[token_at+2].type == TOKEN_TYPE_LBRACE)
    {
      *error = bulp_error_new_parse (filename, tokens[token_at+1].line_no, "expected '%c'", BULP_LBRACE_CHAR);
      goto error_cleanup;
    }
  unsigned fields_alloced = 4;
  BulpMessageField *fields = malloc(sizeof(BulpMessageField) * fields_alloced);
  BulpMessageField mfield;
  token_at += 3;
  while (token_at < n_tokens && tokens[token_at].type != TOKEN_TYPE_RBRACE)
    {
      if (tokens[token_at].type == TOKEN_TYPE_SEMICOLON)
        {
          token_at++;
          continue;
        }
      unsigned mn = parse_message_field(imports,
                                        filename,
                                        file_data,
                                        n_tokens - token_at,
                                        tokens + token_at,
                                        &mfield,
                                        error);
      if (mn == 0)
        goto error_cleanup;
      if (n_fields == fields_alloced)
        {
          fields_alloced *= 2;
          fields = realloc (fields, sizeof(BulpMessageField) * fields_alloced);
        }
      fields[n_fields++] = mfield;
    }
  if (token_at == n_tokens)
    {
      *error = bulp_error_new_premature_eof(filename, "premature eof, expected %c after struct members", BULP_RBRACE_CHAR);
      goto error_cleanup;
    }
  token_at++;

  BulpFormat *fmt = bulp_format_new_message (n_fields, fields);
  for (unsigned k = 0; k < n_fields; k++)
    free ((char*) fields[k].name);
  bulp_namespace_add_format (imports->this_ns, name_tok->byte_length,
                             (char*) file_data + name_tok->byte_offset, fmt,
                             BULP_TRUE);
  bulp_format_unref (fmt);
  return token_at;

error_cleanup:
  for (unsigned k = 0; k < n_fields; k++)
    {
      free ((char*) fields[k].name);
    }
  free (fields);
  return 0;
}


static bulp_bool
parse_tokens (BulpImports *imports,
              const char *filename,
              const uint8_t *file_data,
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
      *error = bulp_error_new_parse (filename, tokens[0].line_no,
                              "expected 'namespace'");
      return BULP_FALSE;
    }
  unsigned token_at = 1;
  if (n_tokens < 2 || tokens[1].type != TOKEN_TYPE_BAREWORD)
    {
      *error = bulp_error_new_parse (filename, tokens[0].line_no,
                              "expected namespace name");
      return BULP_FALSE;
    }
  unsigned ns_n_tokens = parse_dotted_name_from_tokens (filename, n_tokens - 1, tokens + 1, error);
  if (ns_n_tokens == 0)
    {
      return BULP_FALSE;
    }
  token_at += ns_n_tokens;

  if (tokens[token_at].type != TOKEN_TYPE_SEMICOLON)
    {
      *error = bulp_error_new_parse (filename, tokens[token_at].line_no, "expected ';'");
      return BULP_FALSE;
    }

  // parse stanzas 
  while (token_at < n_tokens)
    {
      switch (tokens[token_at].type)
        {
          case TOKEN_TYPE_SEMICOLON:
            token_at++;
            break;
          case TOKEN_TYPE_STRUCT:
            {
              unsigned n_subtokens = parse_struct_format (imports, filename,
                                                          file_data,
                                                          n_tokens - token_at,
                                                          tokens + token_at,
                                                          error);
              if (n_subtokens == 0)
                return BULP_FALSE;
              token_at += n_subtokens;
            }
            break;
                
          case TOKEN_TYPE_PACKED:
            {
              unsigned n_subtokens = parse_packed_format (imports->this_ns,
                                                          filename, file_data,
                                                          n_tokens - token_at,
                                                          tokens + token_at,
                                                          error);
              if (n_subtokens == 0)
                return BULP_FALSE;
              token_at += n_subtokens;
            }
            break;

          case TOKEN_TYPE_UNION:
            {
              unsigned n_subtokens = parse_union_format (imports, filename, file_data,
                               n_tokens - token_at, tokens + token_at,
                               error);
              if (n_subtokens == 0)
                return BULP_FALSE;
              token_at += n_subtokens;
            }
            break;

          case TOKEN_TYPE_MESSAGE:
            {
              unsigned n_subtokens = parse_message_format (imports, filename, file_data,
                               n_tokens - token_at, tokens + token_at,
                               error);
              if (n_subtokens == 0)
                return BULP_FALSE;
              token_at += n_subtokens;
            }
            break;

          default:
            *error = bulp_error_new_parse (filename, tokens[token_at].line_no,
                   "expected 'struct', 'object', 'packed', or 'union', got %s",
                   token_type_to_string(tokens[token_at].type));
            return BULP_FALSE;
        }
    }

  return BULP_TRUE;
}

/* ------------------------- Toplevel ------------------------- */
bulp_bool
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
  bulp_bool parse_rv = bulp_namespace_parse_data (ns, filename, data_length, data, error);
  free (data);
  return parse_rv;
}

bulp_bool
bulp_namespace_parse_data       (BulpNamespace *ns,
                                 const char    *filename,
                                 size_t         data_length,
                                 const uint8_t *data,
                                 BulpError    **error)
{
  // tokenize
  unsigned lineno = 1;
  TokenizeResult tokenize_result = tokenize (filename, data_length, data, &lineno);
  BulpImports *imports = bulp_imports_new (ns);
  if (tokenize_result.failed != NULL)
    {
      *error = tokenize_result.failed;
      return BULP_FALSE;
    }

  // parse tokens
  bulp_bool parse_token_rv = parse_tokens (imports, filename, data, tokenize_result.n, tokenize_result.tokens, error);
  free (tokenize_result.tokens);
  return parse_token_rv;
}
