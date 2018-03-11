
#include "bulp.h"

typedef struct BulpSortedStrGen BulpSortedStrGen;
struct BulpSortedStrGen
{
  char str[32];

  uint64_t at, n;
  BulpRand rand;
};

void bulp_sorted_str_gen_init (BulpSortedStrGen *gen, uint64_t seed, uint64_t n);
void bulp_sorted_str_gen_next (BulpSortedStrGen *gen);



