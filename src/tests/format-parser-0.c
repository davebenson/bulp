#include "../bulp.h"
#include <stdio.h>

static void
test__simple_overview (void)
{
  static const char test_input[] =
    "namespace foo.bar;\n"
    "\n"
    "enum Type {\n"
    "  animal,\n"
    "  vegetable,\n"
    "  mineral\n"
    "};\n"
    "\n"
    "packed AnimalTraits {\n"
    "  n_legs : 8;\n"
    "  has_hair : 1;\n"
    "  warmblooded : 1;\n"
    "  poisonous : 1;\n"
    "};\n"
    "\n"
    "struct Animal {\n"
    "  string name;\n"
    "  AnimalTraits traits;\n"
    "};\n" ;  
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpError *error = NULL;
  if (!bulp_namespace_parse_data (ns, "*test-data*", sizeof (test_input), (uint8_t *) test_input, &error))
    bulp_die ("parsing ns data: %s", error->message);

  BulpNamespaceEntry query_res;
  if (!bulp_namespace_query_1 (ns, "foo", &query_res))
    bulp_die("query for foo failed");
  assert(query_res.type == BULP_NAMESPACE_ENTRY_SUBNAMESPACE);
  BulpNamespace *foo_ns = query_res.info.v_namespace;

  if (!bulp_namespace_query_1 (foo_ns, "bar", &query_res))
    bulp_die("query for bar failed");
  assert(query_res.type == BULP_NAMESPACE_ENTRY_SUBNAMESPACE);
  BulpNamespace *foo_bar_ns = query_res.info.v_namespace;

  if (!bulp_namespace_query_1 (foo_bar_ns, "Animal", &query_res))
    bulp_die("query for Animal failed");
  assert(query_res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *animal_format = query_res.info.v_format;
  assert (animal_format->type == BULP_FORMAT_TYPE_STRUCT);
  assert (animal_format->v_struct.n_members == 2);
  assert (animal_format->v_struct.members[0].format->type == BULP_FORMAT_TYPE_STRING);
  assert (animal_format->v_struct.members[0].format->v_string.length_type == BULP_STRING_LENGTH_TYPE_LENGTH_PREFIXED);
  assert (animal_format->v_struct.members[1].format->v_string.string_type == BULP_STRING_TYPE_UTF8);

  bulp_namespace_unref (ns);
}

static struct {
  const char *name;
  void (*test)(void);
} tests[] = {
  { "simple overview", test__simple_overview }
};
                                 
int main()
{
  unsigned i;
  //bulp_init ();

  fprintf(stderr, "Running tests of format parser:\n");
  for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      fprintf(stderr, "Running test %s... ", tests[i].name);
      (*tests[i].test) ();
      fprintf(stderr, " ok.\n");
    }
  fprintf(stderr, "Done.\n");
  return 0;
}
