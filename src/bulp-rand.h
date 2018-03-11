
typedef struct BulpRand BulpRand;
struct BulpRand
{
  uint64_t s[16];
  unsigned p;
};


void     bulp_rand_init1    (BulpRand *rand,
                             uint64_t  seed);
uint64_t bulp_rand_generate (BulpRand *rand);
