#include "../bulp.h"
#include "bulp-test.h"
#include <stdio.h>
#include <string.h>

static char *tmpdir;

static void
test__simple_empty_sorted_indexer (void)
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

  while (1)
    {
      switch (bulp_sorted_indexer_finish (indexer))
        {
        case BULP_SORTED_INDEXER_RESULT_GOT_INDEX:
          goto done_finishing;
        case BULP_SORTED_INDEXER_RESULT_RUNNING:
          break;
        case BULP_SORTED_INDEXER_RESULT_ERROR:
          assert(0);
        }
    }
done_finishing:
  bulp_sorted_indexer_destroy (indexer, BULP_FALSE);

  // create readonly-index
  BulpComparator *comparator = bulp_comparator_new (bulp_namespace_get_string (global));
  BulpReadonlyIndex *index = bulp_readonly_index_new (tmpdir, comparator, &comp_options, &error);
  TEST_ASSERT (index != NULL);

  // test a few strings to not exist
  BulpSlab out = BULP_SLAB_INIT;
  TEST_ASSERT (bulp_readonly_index_lookup (index, 3, (uint8_t *) "\002ab", &out, &error) == BULP_READONLY_INDEX_LOOKUP_NOT_FOUND);
  TEST_ASSERT (bulp_readonly_index_lookup (index, 1, (uint8_t *) "\0", &out, &error) == BULP_READONLY_INDEX_LOOKUP_NOT_FOUND);

  return;
}

static struct {
  const char *name;
  void (*test)(void);
} tests[] = {
  { "simple empty indexer", test__simple_empty_sorted_indexer }
};
                                 
int main()
{
  unsigned i;
  //bulp_init ();

  //fprintf(stderr, "Running tests of format parser:\n");
  tmpdir = "tmpdir";
  for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      fprintf(stderr, "Running test %s... ", tests[i].name);
      (*tests[i].test) ();
      fprintf(stderr, " ok.\n");
    }
  //fprintf(stderr, "Done.\n");
  return 0;
}
