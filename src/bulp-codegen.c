#include "bulp.h"

typedef struct {
  BulpBuffer *h_buffer;
  BulpBuffer *c_buffer;
  unsigned    longest_typename;
  unsigned    longest_func_prefix;
} Info;

static bulp_bool
render_typedef_struct (BulpNamespace *ns,
                       const char *name,
                       BulpNamespaceEntry *value,
                       void       *data)
{
  if (value->type != BULP_NAMESPACE_ENTRY_FORMAT)
    return BULP_TRUE;
  BulpFormat *format = value->info.v_format;
  if (format->type == BULP_FORMAT_TYPE_PACKED
   && format->type == BULP_FORMAT_TYPE_UNION
   && format->type == BULP_FORMAT_TYPE_STRUCT)
    {
      Info *info = data;
      bulp_buffer_printf (info->h_buffer,
                          "typedef struct %s %s;\n",
                          format->base.c_typename,
                          format->base.c_typename);
    }
  return BULP_TRUE;
}

static void
render_type_decl (BulpNamespace *ns,
                  const char *name,
                  BulpNamespaceEntry *value,
                  void       *data)
{
  if (value->type != BULP_NAMESPACE_ENTRY_FORMAT)
    return BULP_TRUE;
  BulpFormat *format = value->info.v_format;
  switch (format->type)
    {
      case BULP_FORMAT_TYPE_PACKED:
        ...
        break;
      case BULP_FORMAT_TYPE_UNION:
        ...
        break;
      case BULP_FORMAT_TYPE_STRUCT:
        ...
        break;
      case BULP_FORMAT_TYPE_ENUM:
        ...
        break;
    }
  return BULP_TRUE;
}
static bulp_bool
compute_typename_stats (BulpNamespace *ns,
                        const char *name,
                        BulpNamespaceEntry *value,
                        void       *data)
{
  if (value->type != BULP_NAMESPACE_ENTRY_FORMAT)
    return BULP_TRUE;
  BulpFormat *format = value->info.v_format;
  unsigned len_type = strlen (format->base.c_typename);
  unsigned len_prefix = strlen (format->base.c_func_prefix);
  info->longest_typename = BULP_MAX (info->longest_typename, len_type);
  info->longest_prefix = BULP_MAX (info->longest_prefix, len_prefix);
  return BULP_TRUE;
}

typedef struct {
  unsigned rv_column_width;
  unsigned fct_name_column_width;
  /* note: there is an extra single space for either left-paren or space */
  unsigned fct_arg_type_column_width;
  // there's no limit on the argument name size
} FunctionPrototypeSpacing; 

typedef struct 
{
  const char *type_name;                // does not include "*" symbols or extra space
  unsigned n_stars;
  const char *arg_name;
} FunctionPrototypeArg;

typedef struct {
  const char *rv_type;                 // does not include "*" symbols or extra space
  unsigned rv_n_stars;
  const char *fct_name;
  unsigned n_args;
  FunctionPrototypeArg *args;
} FunctionPrototypeSpec;

static void
write_function_prototype (const FunctionPrototypeSpacing *spacing,
                          const FunctionPrototypeSpec *spec,
                          BulpBuffer *out)
{
  unsigned rv_type_len = strlen (spec->rv_type);
  unsigned rv_type_space_required = spec->rv_n_stars == 0 ? (rv_type_len + 1) : (rv_type_len + rv_n_stars)
  if (rv_type_space_required > spacing->rv_column_width)
    {
      bulp_buffer_append_string (out, spec->rv_type);
      if (spec->rv_n_stars > 0)
        {
          bulp_buffer_append_byte (out, ' ');
          bulp_buffer_append_repeated_char (out, '*', spec->rv_n_stars);
        }
      bulp_buffer_append_byte (out, '\n');
    }
  else
    {
      bulp_buffer_append_string (out, spec->rv_type);
      bulp_buffer_append_repeated_char (out, ' ', spacing->rv_column_width - rv_type_len - spec->rv_n_stars);
      bulp_buffer_append_repeated_char (out, '*', spec->rv_n_stars);
    }

  bulp_buffer_append_string (out, spec->fct_name);
  bulp_buffer_append_byte (out, BULP_LPAREN_CHAR);
  if (spec->n_args == 0)
    {
      bulp_buffer_append_string (out, "void" BULP_RPAREN_STR);
      return;
    }
  for (unsigned i = 0; i < spec->n_args; i++)
    {
      bulp_bool is_last = (i+1) == spec->n_args;
      int n_spaces = ...;
      if (n_spaces < 0)
        n_spaces = 0;
      bulp_buffer_append_string (out, spec->args[i].type_name);
      ...

      if (is_last)
        {
          bulp_buffer_append_byte (out, BULP_RPAREN_CHAR);
        }
      else
        {
          bulp_buffer_append_string (out, ",\n");
          bulp_buffer_append_repeated_char (out, ' ', spacing->rv_column_width + spacing->fct_name_column_width + 1);
        }
    }
}

static bulp_bool
compute_typename_stats (BulpNamespace *ns,
                        const char *name,
                        BulpNamespaceEntry *value,
                        void       *data)
{
  if (value->type != BULP_NAMESPACE_ENTRY_FORMAT)
    return BULP_TRUE;
  const char *maybe_amp = format->base.pass_by_ref ? "&" : "";
  unsigned amp_pad = format->base.pass_by_ref ? 0 : 1;
  bulp_buffer_printf ("size_t %s_get_packed_size%*s(%s%*s%svalue);\n",
                      format->base.c_func_prefix,
                      func_pad, "",
                      format->base.c_typename,
                      typename_pad, "",
                      maybe_amp);
  bulp_buffer_printf ("size_t %s_pack           %*s(%s%*s%svalue,\n",
                      "       ""                %*s uint8_t%*s*out);\n"
                      format->base.c_func_prefix,
                      func_pad, "",
                      format->base.c_typename,
                      func_pad, "",
                      info->longest_typename - 7);
  bulp_buffer_printf ("size_t %s_pack_to        %*s(%s%*s%svalue,\n",
                      "       ""                %*s BulpDataBuilder%*s*out);\n"
  bulp_buffer_printf ("size_t %s_unpack         %*s(const uint8_t%*s*data,\n"
                      "       ""                %*s BulpDataBuilder%*s*out);\n"
}

void
bulp_codegen (BulpNamespace *ns,
              BulpCodegenOptions *options,
              BulpBuffer *h_out,
              BulpBuffer *c_out)
{
  Info info = { h_out, c_out, 8, 8 };
  bulp_namespace_foreach (ns, render_typedef_struct, &info);
  bulp_namespace_foreach (ns, render_type_decl, &info);
  bulp_namespace_foreach (ns, compute_typename_stats, &info);
  bulp_namespace_foreach (ns, render_func_decls, &info);
}

