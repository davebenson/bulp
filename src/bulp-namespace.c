#include "bulp.h"
#include "bulp-internals.h"
#include "dsk-rbtree-macros.h"
#include <string.h>
#include <stdlib.h>

#define CMP_TREE_NODES(a,b,rv) rv = strcmp ((a)->name, (b)->name)

#define GET_TREE(ns) \
        (ns)->by_name,              \
        BulpNamespaceTreeNode *,                \
        DSK_STD_GET_IS_RED,             \
        DSK_STD_SET_IS_RED,             \
        parent, left, right,            \
        CMP_TREE_NODES

static BulpNamespace *global = NULL;

BulpNamespace *bulp_namespace_global (void)
{
  if (global == NULL)
    global = bulp_namespace_new_global ();
  return global;
}

BulpNamespace *
bulp_namespace_new_global (void)
{
  BulpNamespace *rv = malloc (sizeof (BulpNamespaceToplevel));
  rv->ref_count = 1;
  rv->version = 0;
  rv->by_name = NULL;
  rv->is_toplevel = BULP_TRUE;

  _bulp_namespace_add_int_types (rv);
  _bulp_namespace_add_float_types (rv);
  _bulp_namespace_add_string_types (rv);
  _bulp_namespace_add_binary_data_types (rv);
  
  return rv;
}

/* Returns false if the name is already in use */
bulp_bool      bulp_namespace_add_subnamespace (BulpNamespace *ns,
                                                const char    *name,
                                                BulpNamespace *subns)
{
  BulpNamespaceTreeNode *tree_node = malloc (sizeof (BulpNamespaceTreeNode));
  tree_node->entry.type = BULP_NAMESPACE_ENTRY_SUBNAMESPACE;
  tree_node->entry.info.v_namespace = subns;
  tree_node->name = strdup (name);
  BulpNamespaceTreeNode *conflict;
  DSK_RBTREE_INSERT (GET_TREE (ns), tree_node, conflict);
  if (conflict != NULL)
    {
      free (tree_node->name);
      free (tree_node);
      return BULP_FALSE;
    }
  bulp_namespace_ref (subns);
  return BULP_TRUE;
}

static char *strdup_with_optional_len (ssize_t len, const char *str)
{
  if (len < 0)
    return strdup (str);
  else
    {
      char *rv = malloc (len + 1);
      memcpy (rv, str, len);
      rv[len] = 0;
      return rv;
    }
}

/* Returns false if the name is already in use */
bulp_bool      bulp_namespace_add_format       (BulpNamespace *ns,
                                                ssize_t        name_len,
                                                const char    *name,
                                                BulpFormat    *format,
                                                bulp_bool      is_canonical_ns)
{
  BulpNamespaceTreeNode *tree_node = malloc (sizeof (BulpNamespaceTreeNode));
  tree_node->entry.type = BULP_NAMESPACE_ENTRY_FORMAT;
  tree_node->entry.info.v_format = format;
  tree_node->name = strdup_with_optional_len (name_len, name);
  BulpNamespaceTreeNode *conflict;
  DSK_RBTREE_INSERT (GET_TREE (ns), tree_node, conflict);
  if (conflict != NULL)
    {
      free (tree_node->name);
      free (tree_node);
      return BULP_FALSE;
    }
  bulp_format_ref (format);
  if (is_canonical_ns)
    {
      assert(format->base.canonical_name == NULL);
      assert(format->base.canonical_ns == NULL);
      format->base.canonical_name = tree_node->name;
      format->base.canonical_ns = ns;
    }
  return BULP_TRUE;
}
unsigned       bulp_namespace_get_version      (BulpNamespace *ns)
{
  return ns->version;
}
void           bulp_namespace_set_version      (BulpNamespace *ns,
                                                unsigned       version)
{
  assert(ns->version == 0);
  ns->version = version;
}

bulp_bool      bulp_namespace_query_1          (BulpNamespace *ns,
                                                const char    *name,
                                                BulpNamespaceEntry *out)
{
#define COMPARE_NAME_WITH_NODE(unused, b_node, rv) \
  rv = strcmp (name, b_node->name)
  BulpNamespaceTreeNode *tree_node;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_TREE (ns), unused, COMPARE_NAME_WITH_NODE, tree_node);
#undef COMPARE_NAME_WITH_NODE
  if (tree_node == NULL)
    return BULP_FALSE;
  *out = tree_node->entry;
  return BULP_TRUE;
}
bulp_bool      bulp_namespace_query_1_len      (BulpNamespace *ns,
                                                size_t         name_len,
                                                const char    *name,
                                                BulpNamespaceEntry *out)
{
#define COMPARE_NAME_WITH_NODE(unused, b_node, rv) \
  rv = strncmp (name, b_node->name, name_len); \
  if (rv == 0 && b_node->name[name_len] != 0) \
    rv = 1;
  BulpNamespaceTreeNode *tree_node;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_TREE (ns), unused, COMPARE_NAME_WITH_NODE, tree_node);
#undef COMPARE_NAME_WITH_NODE
  if (tree_node == NULL)
    return BULP_FALSE;
  *out = tree_node->entry;
  return BULP_TRUE;
  
}

bulp_bool      bulp_namespace_query            (BulpNamespace *ns,
                                                const char    *dotted_name,
                                                BulpNamespaceEntry *out)
{
  ... iterate over dotted components
}

BulpNamespace *
bulp_namespace_force_1      (BulpNamespace *ns,
                             const char    *name)
{
  BulpNamespaceTreeNode tn;
  ...
  if (conflict == NULL)
    return conflict;
  else
    {
      // replace node with one on heap
      ...
    }
}

BulpNamespace *
bulp_namespace_force_1_len  (BulpNamespace *ns,
                             size_t         name_len,
                             const char    *name)
{
...
}

BulpNamespace *
bulp_namespace_force    (BulpNamespace *ns,
                             const char    *name)
{
  return bulp_namespace_force_len (ns, strlen (name), name);
}

BulpNamespace *
bulp_namespace_force_len    (BulpNamespace *ns,
                             size_t         name_len,
                             const char    *name)
{
}

static bulp_bool
namespace_foreach_recursive (BulpNamespace *ns,
                             BulpNamespaceTreeNode *node,
                             BulpNamespaceForeachFunc func,
                             void *func_data)
{
  if (node->left != NULL)
    if (!namespace_foreach_recursive (ns, node->left, func, func_data))
      return BULP_FALSE;
  if (!func (ns, node->name, &node->entry, func_data))
    return BULP_FALSE;
  if (node->right != NULL)
    if (!namespace_foreach_recursive (ns, node->right, func, func_data))
      return BULP_FALSE;
  return BULP_TRUE;
}

bulp_bool      bulp_namespace_foreach          (BulpNamespace *ns,
                                                BulpNamespaceForeachFunc func,
                                                void *func_data)
{
  assert(ns->is_toplevel);
  if (!ns->by_name)
    return BULP_TRUE;           /* vacuously true */
  return namespace_foreach_recursive (ns, ns->by_name, func, func_data);
}

BulpNamespace *bulp_namespace_ref (BulpNamespace *ns)
{
  assert(ns->ref_count > 0);
  ++(ns->ref_count);
  return ns;
}

void           bulp_namespace_unref (BulpNamespace *ns)
{
  assert(ns->ref_count > 0);
  if (--(ns->ref_count) == 0)
    {
    ... finalize
    }
}

