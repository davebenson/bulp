#include "../bulp.h"
#include <stdio.h>
#include <string.h>

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
    "};\n"
    "\n"
    "union Annotation {\n"
    "  string color;\n"
    "  float32 fur_length;\n"
    "}\n"
    "\n"
    "message Car {\n"
    "  string name;\n"
    "  float32 horsepower;\n"
    "  Annotation[] annotations;\n"
    "};\n"
    "\n"
    ;

  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpError *error = NULL;
  if (!bulp_namespace_parse_data (ns, "*test-data*", sizeof (test_input)-1, (uint8_t *) test_input, &error))
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
  assert (!animal_format->v_struct.is_message);
  assert (animal_format->v_struct.n_members == 2);
  assert (strcmp (animal_format->v_struct.members[0].name, "name") == 0);
  assert (animal_format->v_struct.members[0].format->type == BULP_FORMAT_TYPE_STRING);
  assert (animal_format->v_struct.members[0].format->v_string.length_type == BULP_STRING_LENGTH_TYPE_LENGTH_PREFIXED);
  assert (animal_format->v_struct.members[0].format->v_string.string_type == BULP_STRING_TYPE_UTF8);
  assert (strcmp (animal_format->v_struct.members[1].name, "traits") == 0);
  assert (animal_format->v_struct.members[1].format->type == BULP_FORMAT_TYPE_PACKED);
  assert (animal_format->v_struct.members[1].format->v_packed.n_elements == 4);
  assert (strcmp (animal_format->v_struct.members[1].format->v_packed.elements[0].name, "n_legs") == 0);
  assert (animal_format->v_struct.members[1].format->v_packed.elements[0].n_bits == 8);

  if (!bulp_namespace_query_1 (foo_bar_ns, "Annotation", &query_res))
    bulp_die("query for Annotation failed");
  assert(query_res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *ann_format = query_res.info.v_format;
  assert (ann_format->type == BULP_FORMAT_TYPE_UNION);
  assert (ann_format->v_union.n_cases == 2);
  assert (strcmp (ann_format->v_union.cases[0].name, "color") == 0);
  assert (ann_format->v_union.cases[0].format->type == BULP_FORMAT_TYPE_STRING);
  assert (ann_format->v_union.cases[0].format->v_string.length_type == BULP_STRING_LENGTH_TYPE_LENGTH_PREFIXED);
  assert (ann_format->v_union.cases[0].format->v_string.string_type == BULP_STRING_TYPE_UTF8);
  assert (strcmp (ann_format->v_union.cases[1].name, "fur_length") == 0);
  assert (ann_format->v_union.cases[1].format->type == BULP_FORMAT_TYPE_FLOAT);
  assert (ann_format->v_union.cases[1].format->v_float.float_type == BULP_FLOAT_TYPE_FLOAT32);

  if (!bulp_namespace_query_1 (foo_bar_ns, "Car", &query_res))
    bulp_die("query for Car failed");
  assert(query_res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *car_format = query_res.info.v_format;
  assert (car_format->type == BULP_FORMAT_TYPE_STRUCT);
  assert (car_format->v_struct.is_message);
  assert (car_format->v_struct.n_members == 3);
  assert (strcmp (car_format->v_struct.members[0].name, "name") == 0);
  assert (car_format->v_struct.members[0].format->type == BULP_FORMAT_TYPE_STRING);
  assert (car_format->v_struct.members[0].format->v_string.length_type == BULP_STRING_LENGTH_TYPE_LENGTH_PREFIXED);
  assert (car_format->v_struct.members[0].format->v_string.string_type == BULP_STRING_TYPE_UTF8);
  assert (strcmp (car_format->v_struct.members[1].name, "horsepower") == 0);
  assert (car_format->v_struct.members[1].format->type == BULP_FORMAT_TYPE_FLOAT);
  assert (car_format->v_struct.members[1].format->v_float.float_type == BULP_FLOAT_TYPE_FLOAT32);
  assert (strcmp (car_format->v_struct.members[2].name, "annotations") == 0);
  assert (car_format->v_struct.members[2].format->type == BULP_FORMAT_TYPE_ARRAY);
  assert (car_format->v_struct.members[2].format->v_array.element_format == ann_format);

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

  //fprintf(stderr, "Running tests of format parser:\n");
  for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      fprintf(stderr, "Running test %s... ", tests[i].name);
      (*tests[i].test) ();
      fprintf(stderr, " ok.\n");
    }
  //fprintf(stderr, "Done.\n");
  return 0;
}
