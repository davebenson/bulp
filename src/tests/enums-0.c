#include "../bulp.h"
#include "bulp-test.h"
#include <stdio.h>

typedef enum {
  BAZ__TINY__A,
  BAZ__TINY__B,
  BAZ__TINY__C = 10,
  BAZ__TINY__D,
} Foo__Bar__BazTiny;

typedef enum {
  BAZ__TINY2__A,
  BAZ__TINY2__B,
  BAZ__TINY2__C = 127,
  BAZ__TINY2__D,
} Foo__Bar__BazTiny2;

typedef enum {
  BAZ__SMALL__A,
  BAZ__SMALL__B,
  BAZ__SMALL__C = (1<<14) - 1,
  BAZ__SMALL__D,
} Foo__Bar__BazSmall;

typedef enum {
  BAZ__INT__D,
  BAZ__INT__C,
  BAZ__INT__B = (1<<21) - 1,
  BAZ__INT__A,
} Foo__Bar__BazInt;

static void
test__enum_format_parser (void)
{
  const char spec[] = 
     "namespace foo.bar;\n"
     "enum BazTiny {\n"
     "  a,\n"
     "  b,\n"
     "  c = 10,\n"
     "  d,\n"
     "}\n"
     "enum BazTiny2 {\n"
     "  a,\n"
     "  b,\n"
     "  c = 127,\n"
     "  d,\n"
     "}\n"
     "enum BazSmall {\n"
     "  a,\n"
     "  b,\n"
     "  c = 16383,\n"
     "  d,\n"
     "}\n"
     "enum BazInt {\n"
     "  d,\n"
     "  c,\n"
     "  b = 2097151,\n"
     "  a,\n"
     "}\n"
     ;

  BulpError *error = NULL;
  BulpNamespace *g = bulp_namespace_new_global ();
  if (!bulp_namespace_parse_data(g, "*inline*", sizeof(spec)-1, (uint8_t *) spec, &error))
    bulp_die ("error parsing namespace: %s", error->message);
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query (g, "foo.bar", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_SUBNAMESPACE);
  BulpNamespace *bar = res.info.v_namespace;

  /* Test tiny enum */
  TEST_ASSERT (bulp_namespace_query_1 (bar, "BazTiny", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *baz_tiny = res.info.v_format;
  TEST_ASSERT (baz_tiny->type == BULP_FORMAT_TYPE_ENUM);
  TEST_ASSERT (baz_tiny->v_enum.n_values == 4);
  TEST_ASSERT (baz_tiny->v_enum.values[0].value == 0);
  TEST_ASSERT (strcmp (baz_tiny->v_enum.values[0].name, "a") == 0);
  TEST_ASSERT (baz_tiny->v_enum.values[1].value == 1);
  TEST_ASSERT (strcmp (baz_tiny->v_enum.values[1].name, "b") == 0);
  TEST_ASSERT (baz_tiny->v_enum.values[2].value == 10);
  TEST_ASSERT (strcmp (baz_tiny->v_enum.values[2].name, "c") == 0);
  TEST_ASSERT (baz_tiny->v_enum.values[3].value == 11);
  TEST_ASSERT (strcmp (baz_tiny->v_enum.values[3].name, "d") == 0);
  TEST_ASSERT (baz_tiny->base.c_sizeof == sizeof (Foo__Bar__BazTiny));
  Foo__Bar__BazTiny baz_tiny_value = BAZ__TINY__A;
  TEST_ASSERT (baz_tiny->base.vfuncs.validate_native (baz_tiny, &baz_tiny_value, &error));
  TEST_ASSERT (baz_tiny->base.vfuncs.get_packed_size (baz_tiny, &baz_tiny_value) == 1);
  baz_tiny_value = BAZ__TINY__B;
  TEST_ASSERT (baz_tiny->base.vfuncs.validate_native (baz_tiny, &baz_tiny_value, &error));
  TEST_ASSERT (baz_tiny->base.vfuncs.get_packed_size (baz_tiny, &baz_tiny_value) == 1);
  baz_tiny_value = BAZ__TINY__C;
  TEST_ASSERT (baz_tiny->base.vfuncs.validate_native (baz_tiny, &baz_tiny_value, &error));
  TEST_ASSERT (baz_tiny->base.vfuncs.get_packed_size (baz_tiny, &baz_tiny_value) == 1);
  baz_tiny_value = BAZ__TINY__D;
  TEST_ASSERT (baz_tiny->base.vfuncs.validate_native (baz_tiny, &baz_tiny_value, &error));
  TEST_ASSERT (baz_tiny->base.vfuncs.get_packed_size (baz_tiny, &baz_tiny_value) == 1);
  baz_tiny_value = BAZ__TINY__D + 1;                    //INVALID
  TEST_ASSERT (!baz_tiny->base.vfuncs.validate_native (baz_tiny, &baz_tiny_value, &error));
  TEST_ASSERT (error != NULL);
  bulp_error_unref (error);
  error = NULL;
  baz_tiny_value = 2;                    //INVALID
  TEST_ASSERT (!baz_tiny->base.vfuncs.validate_native (baz_tiny, &baz_tiny_value, &error));
  TEST_ASSERT (error != NULL);
  bulp_error_unref (error);
  error = NULL;

  /* Test tiny2 enum (fits in a byte, but may require 2 bytes packed due to b128 */
  TEST_ASSERT (bulp_namespace_query_1 (bar, "BazTiny2", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *baz_tiny2 = res.info.v_format;
  TEST_ASSERT (baz_tiny2->type == BULP_FORMAT_TYPE_ENUM);
  TEST_ASSERT (baz_tiny2->v_enum.n_values == 4);
  TEST_ASSERT (baz_tiny2->v_enum.values[0].value == 0);
  TEST_ASSERT (strcmp (baz_tiny2->v_enum.values[0].name, "a") == 0);
  TEST_ASSERT (baz_tiny2->v_enum.values[1].value == 1);
  TEST_ASSERT (strcmp (baz_tiny2->v_enum.values[1].name, "b") == 0);
  TEST_ASSERT (baz_tiny2->v_enum.values[2].value == 127);
  TEST_ASSERT (strcmp (baz_tiny2->v_enum.values[2].name, "c") == 0);
  TEST_ASSERT (baz_tiny2->v_enum.values[3].value == 128);
  TEST_ASSERT (strcmp (baz_tiny2->v_enum.values[3].name, "d") == 0);
  TEST_ASSERT (baz_tiny2->base.c_sizeof == sizeof (Foo__Bar__BazTiny2));
  Foo__Bar__BazTiny2 baz_tiny2_value = BAZ__TINY2__A;
  TEST_ASSERT (baz_tiny2->base.vfuncs.validate_native (baz_tiny2, &baz_tiny2_value, &error));
  TEST_ASSERT (baz_tiny2->base.vfuncs.get_packed_size (baz_tiny2, &baz_tiny2_value) == 1);
  baz_tiny2_value = BAZ__TINY2__B;
  TEST_ASSERT (baz_tiny2->base.vfuncs.validate_native (baz_tiny2, &baz_tiny2_value, &error));
  TEST_ASSERT (baz_tiny2->base.vfuncs.get_packed_size (baz_tiny2, &baz_tiny2_value) == 1);
  baz_tiny2_value = BAZ__TINY2__C;
  TEST_ASSERT (baz_tiny2->base.vfuncs.validate_native (baz_tiny2, &baz_tiny2_value, &error));
  TEST_ASSERT (baz_tiny2->base.vfuncs.get_packed_size (baz_tiny2, &baz_tiny2_value) == 1);
  baz_tiny2_value = BAZ__TINY2__D;
  TEST_ASSERT (baz_tiny2->base.vfuncs.validate_native (baz_tiny2, &baz_tiny2_value, &error));
  TEST_ASSERT (baz_tiny2->base.vfuncs.get_packed_size (baz_tiny2, &baz_tiny2_value) == 2);

  baz_tiny2_value = BAZ__TINY2__D + 1;                    //INVALID
  TEST_ASSERT (!baz_tiny2->base.vfuncs.validate_native (baz_tiny2, &baz_tiny2_value, &error));
  TEST_ASSERT (error != NULL);
  bulp_error_unref (error);
  error = NULL;
  baz_tiny2_value = 2;                    //INVALID
  TEST_ASSERT (!baz_tiny2->base.vfuncs.validate_native (baz_tiny2, &baz_tiny2_value, &error));
  TEST_ASSERT (error != NULL);
  bulp_error_unref (error);
  error = NULL;
  //TODO: test unpack()


  // baz_small (a enum that can be represented by a uint16_t)
  TEST_ASSERT (bulp_namespace_query_1 (bar, "BazSmall", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *baz_small = res.info.v_format;
  TEST_ASSERT (baz_small->type == BULP_FORMAT_TYPE_ENUM);
  TEST_ASSERT (baz_small->v_enum.n_values == 4);
  TEST_ASSERT (baz_small->v_enum.values[0].value == 0);
  TEST_ASSERT (strcmp (baz_small->v_enum.values[0].name, "a") == 0);
  TEST_ASSERT (baz_small->v_enum.values[1].value == 1);
  TEST_ASSERT (strcmp (baz_small->v_enum.values[1].name, "b") == 0);
  TEST_ASSERT (baz_small->v_enum.values[2].value == (1<<14)-1);
  TEST_ASSERT (strcmp (baz_small->v_enum.values[2].name, "c") == 0);
  TEST_ASSERT (baz_small->v_enum.values[3].value == (1<<14));
  TEST_ASSERT (strcmp (baz_small->v_enum.values[3].name, "d") == 0);
  TEST_ASSERT (baz_small->base.c_sizeof == sizeof (Foo__Bar__BazSmall));
  Foo__Bar__BazSmall baz_small_value = BAZ__SMALL__A;
  TEST_ASSERT (baz_small->base.vfuncs.validate_native (baz_small, &baz_small_value, &error));
  TEST_ASSERT (baz_small->base.vfuncs.get_packed_size (baz_small, &baz_small_value) == 1);
  baz_small_value = BAZ__SMALL__B;
  TEST_ASSERT (baz_small->base.vfuncs.validate_native (baz_small, &baz_small_value, &error));
  TEST_ASSERT (baz_small->base.vfuncs.get_packed_size (baz_small, &baz_small_value) == 1);
  baz_small_value = BAZ__SMALL__C;
  TEST_ASSERT (baz_small->base.vfuncs.validate_native (baz_small, &baz_small_value, &error));
  TEST_ASSERT (baz_small->base.vfuncs.get_packed_size (baz_small, &baz_small_value) == 2);
  baz_small_value = BAZ__SMALL__D;
  TEST_ASSERT (baz_small->base.vfuncs.validate_native (baz_small, &baz_small_value, &error));
  TEST_ASSERT (baz_small->base.vfuncs.get_packed_size (baz_small, &baz_small_value) == 3);

  baz_small_value = BAZ__SMALL__D + 1;                    //INVALID
  TEST_ASSERT (!baz_small->base.vfuncs.validate_native (baz_small, &baz_small_value, &error));
  TEST_ASSERT (error != NULL);
  bulp_error_unref (error);
  error = NULL;
  baz_small_value = 2;                    //INVALID
  TEST_ASSERT (!baz_small->base.vfuncs.validate_native (baz_small, &baz_small_value, &error));
  TEST_ASSERT (error != NULL);
  bulp_error_unref (error);
  error = NULL;
  //TODO: test unpack()

  
  // baz_small (a enum that can be represented by a uint16_t)
  //TODO...

  bulp_namespace_unref (g);
}

static struct {
  const char *name;
  void (*test)(void);
} tests[] = {
  { "enum format parser test",     test__enum_format_parser },
};
                                 
int main()
{
  unsigned i;
  //bulp_init ();

  for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      fprintf(stderr, "Running test %s... ", tests[i].name);
      (*tests[i].test) ();
      fprintf(stderr, " ok.\n");
    }
  return 0;
}
