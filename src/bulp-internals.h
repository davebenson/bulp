typedef struct BulpNamespaceTreeNode BulpNamespaceTreeNode;
typedef struct BulpNamespaceToplevel BulpNamespaceToplevel;

void _bulp_namespace_add_int_types (BulpNamespaceToplevel *ns);
void _bulp_namespace_add_float_types (BulpNamespaceToplevel *ns);
void _bulp_namespace_add_string_types (BulpNamespaceToplevel *ns);
void _bulp_namespace_add_binary_data_types (BulpNamespaceToplevel *ns);

struct BulpNamespaceTreeNode
{
  char *name;
  BulpNamespaceEntry entry;
  BulpNamespaceTreeNode *left, *right, *parent;
  bulp_bool is_red;
};

struct BulpNamespace
{
  unsigned ref_count;
  unsigned version;
  bulp_bool is_toplevel;
  BulpNamespaceTreeNode *by_name;
};

struct BulpNamespaceToplevel
{
  BulpNamespace base;

  BulpFormat * format_short;
  BulpFormat * format_int;
  BulpFormat * format_long;
  BulpFormat * format_ushort;
  BulpFormat * format_uint;
  BulpFormat * format_ulong;
  BulpFormat * format_int8;
  BulpFormat * format_int16;
  BulpFormat * format_int32;
  BulpFormat * format_int64;
  BulpFormat * format_uint8;
  BulpFormat * format_uint16;
  BulpFormat * format_uint32;
  BulpFormat * format_uint64;
  BulpFormat * format_float32;
  BulpFormat * format_float64;
  BulpFormat * format_ascii;
  BulpFormat * format_string;
  BulpFormat * format_ascii0;
  BulpFormat * format_string0;
  BulpFormat * format_binary_data;
};

#define BULP_FORMAT_VFUNCS_DEFINE(shortname) \
  {                                          \
    get_packed_size__##shortname,            \
    pack__##shortname,                       \
    pack_to__##shortname,                    \
    unpack__##shortname,                     \
    destruct_format__##shortname,            \
  }
