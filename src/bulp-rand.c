#include "bulp.h"
#include "bulp-rand.h"

// splitmix64 generator to seed prng
void     bulp_rand_init1    (BulpRand *rand,
                             uint64_t  seed)
{
  uint64_t x = seed;
  unsigned i = 0;
  for (i = 0; i < 16; i++)
    {
      uint64_t z = (x += 0x9e3779b97f4a7c15);
      z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
      z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
      rand->s[i] = z ^ (z >> 31);
    }
  rand->p = 0;
}

// xorshift1024
uint64_t bulp_rand_generate (BulpRand *rand)
{
  uint64_t s0 = rand->s[rand->p];
  rand->p += 1;
  rand->p &= 15;
  uint64_t s1 = rand->s[rand->p];
  s1 ^= s1 << 31; // a
  s1 ^= s1 >> 11; // b
  s0 ^= s0 >> 30; // c
  uint64_t prv = s0 ^ s1;
  rand->s[rand->p] = prv;
  return prv * 1181783497276652981LL; 
}

