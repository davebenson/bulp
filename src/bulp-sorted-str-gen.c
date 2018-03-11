#include "bulp.h"

/*
N=32 alphabet;
  12 characters; 4 wasted bits
*/

static const char charset[32] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
#define CHARSET_SIZE_BITS 5
#define STRING_LENGTH (64 / CHARSET_SIZE_BITS)

static void
gen_for_interval (BulpSortedStrGen *gen)
{
  /*      (1<<64) * at       (1<<64) * (at+1)
    from  ------------ to    ----------------
                n                    n     

    but, due to rounding ease, we actually compute
    
          (1<<64)-1                    (1<<64)-1      
   from   --------- * at     to        --------- * at 
               n                            n         

  */
  uint64_t r = bulp_rand_generate (&gen->rand) / gen->n
             + (0xFFFFFFFFFFFFFFFFULL / gen->n) * gen->at;
  for (unsigned i = 0; i < STRING_LENGTH; i++)
    gen->str[i] = charset[(r >> (59 - CHARSET_SIZE_BITS*i)) & ((1 << CHARSET_SIZE_BITS) - 1)];
  gen->str[STRING_LENGTH] = 0;
}
void bulp_sorted_str_gen_init (BulpSortedStrGen *gen, uint64_t seed, uint64_t n)
{
  bulp_rand_init1 (&gen->rand, seed);

  gen->at = 0;
  gen->n = n;
  gen_for_interval (gen);
}
void bulp_sorted_str_gen_next (BulpSortedStrGen *gen)
{
  gen->at++;
  gen_for_interval(gen);
}
