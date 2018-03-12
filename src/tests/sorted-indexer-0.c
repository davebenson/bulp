#include "../bulp.h"
#include "bulp-test.h"
#include <stdio.h>
#include <string.h>

static char *tmpdir;

static void
test__simple_indexer (void)
{
  BulpNamespace *global = bulp_namespace_new_global ();
  BulpSlab slab = BULP_SLAB_INIT;
  BulpError *error = NULL;
  BulpCompressionOptions comp_options = BULP_COMPRESSION_OPTIONS_INIT;
  BulpSortedIndexer *indexer = bulp_sorted_indexer_new (tmpdir, &comp_options, &slab,
                                bulp_namespace_get_string (global),
                                bulp_namespace_get_ulong (global),
                                &error);
  TEST_ASSERT (indexer != NULL);

}

static struct {
  const char *name;
  void (*test)(void);
} tests[] = {
  { "simple indexer", test__simple_indexer }
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
