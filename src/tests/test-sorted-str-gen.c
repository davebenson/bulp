#include "../bulp.h"
#include "bulp-test.h"
#include <stdio.h>

void test_N (uint64_t N, uint64_t seed)
{

  BulpSortedStrGen gen;
  bulp_sorted_str_gen_init (&gen, seed, N);
  char last[20];
  last[0] = 0;
  for (uint64_t i = 0; i < N; i++)
    {
      TEST_ASSERT(strcmp (last, gen.str) < 0);
      strcpy (last, gen.str);
      //printf("%8llu: %s\n", i, gen.str);
      bulp_sorted_str_gen_next (&gen);
    }
}
  
int main()
{
  test_N(16, 50);
  test_N(1600000, 50);
  for (uint64_t s = 0; s < 50; s++)
    test_N(160000, s);
  return 0;
}
