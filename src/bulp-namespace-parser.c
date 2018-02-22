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
};

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
  int32 a { version < 5, void };
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
  TOKEN_TYPE_VERSION,
  TOKEN_TYPE_DEFAULT,
  TOKEN_TYPE_VOID,

  TOKEN_TYPE_BAREWORD,

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
  TOKEN_TYPE_QUESTION_MARK,

  TOKEN_TYPE_NUMBER,                    // js conpatible
  TOKEN_TYPE_STRING,                    // js compatible
} TokenType;

static const char *token_type_to_string (TokenType type)
{
#define WRITE_CASE(shortname) case TOKEN_TYPE_##shortname: return #shortname
  switch (type)
  {
  WRITE_CASE(NAMESPACE);
  WRITE_CASE(STRUCT);
  WRITE_CASE(UNION);
  WRITE_CASE(PACKED);
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
            ...
          case TOKEN_TYPE_QUESTION_MARK:
            ...
          default:
            goto done_scanning_suffixes;
        }
    }
done_scanning_suffixes:

  *format_out = fmt;
  return tokens_used;
}

static unsigned
parse_struct_member (BulpImports *imports,
                     const char *filename,
                     unsigned n_tokens,
                     Token *tokens,
                     BulpStructMember *member_out,
                     BulpError **error)
{
  BulpFormat *format;
}

static unsigned
parse_struct (const char *filename,
              BulpNamespace *ns,
              const uint8_t *binary_data,
              unsigned n_tokens, Token *tokens,
              BulpError **error)
{
  unsigned token_at = 0;
  assert(tokens[0].type == TOKEN_TYPE_STRUCT);
  if (token_at + 1 >= n_tokens || tokens[token_at+1].type == TOKEN_TYPE_BAREWORD)
    {
      *error = bulp_error_new_parse (filename, tokens[token_at].line_no, "expected structure name");
      goto error_cleanup;
    }
  if (token_at + 2 >= n_tokens || tokens[token_at+2].type == TOKEN_TYPE_LBRACE)
    {
      *error = bulp_error_new_parse (filename, tokens[token_at+1].line_no, "expected '%c'", BULP_LBRACE_CHAR);
      goto error_cleanup;
    }
  unsigned members_alloced = 4;
  BulpStructMember *members = malloc(sizeof(BulpStructMember) * members_alloced);
  unsigned n_members = 0;
  BulpStructMember member;
  token_at += 3;
  while (token_at < n_tokens && tokens[token_at].type != TOKEN_TYPE_RBRACE)
    {
      if (tokens[token_at].type == TOKEN_TYPE_SEMICOLON)
        {
          token_at++;
          continue;
        }
      unsigned mn = parse_struct_member(filename, n_tokens - token_at, tokens + token_at, &member, &error);
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
      *error = bulp_error_new_premature_eof(...);
      goto structure_error_cleanup;
    }
  token_at++;

  BulpFormat *fmt = bulp_format_new_struct (n_members, members);
  for (unsigned k = 0; k < n_members; k++)
    free (members[k].name);
  bulp_namespace_add_with_length_take_format (ns, ...)
  return token_at;

error_cleanup:
  for (unsigned k = 0; k < n_members; k++)
    {
      free (members[k].name);
    }
  free (members);
  return 0;
}
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
      *error = bulp_error_new_parse (filename, tokens[0].line_no,
                              "expected 'namespace'");
      goto error_cleanup;
    }
  unsigned token_at = 1;
  if (n_tokens < 2 || tokens[1].type != TOKEN_TYPE_BAREWORD)
rt' --- */
/* --- Implementations for 'uint'    {
      *error = bulp_error_new_parse (filename, tokens[0].line_no,
                              "expected namespace name");
      goto error_cleanup;
    }
  unsigned ns_n_tokens = parse_dotted_name_from_tokens (filename, n_tokens - 1, tokens + 1, error);
  if (ns_n_tokens == 0)
    {
      goto error_cleanup;
    }
  token_at += ns_n_tokens;

  if (tokens[token_at].type != TOKEN_TYPE_SEMICOLON)
    {
      *error = bulp_error_new_parse (filename, tokens[token_at].line_no, "expected ';'");
      goto error_cleanup;
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
              unsigned n_subtokens = parse_struct (ns, filename,
                               n_tokens - token_at, tokens + token_at,
                               error);
              if (n_subtokens == 0)
                goto error_cleanup;
              token_at += n_subtokens;
            }
            break;
                
          case TOKEN_TYPE_PACKED:
            ...

          case TOKEN_TYPE_UNION:
            ...

          case TOKEN_TYPE_OBJECT:
            ...

          default:
            *error = bulp_error_new_... (...);
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
