/* various options */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <assert.h>
//#include "../src/bulp-defs.h"
//#include "../src/bulp-object.h"
//#include "../src/bulp-error.h"
//#include "../src/bulp-util.h"
#define bulp_bool int
#define BULP_FALSE 0
#define BULP_TRUE 1

#ifndef offsetof
#define offsetof(st, m) ((size_t)&(((st *)0)->m))
#endif


#if 0
static void
print_binary (const uint8_t *v, unsigned nbytes)
{
  for (unsigned i = 0; i < nbytes; i++)
    {
      for (int bit = 7; bit >= 0; bit--)
        printf("%u", (v[i] >> bit) & 1);
      printf(" ");
    }
  printf("\n");
}
static void
print_hex (const uint8_t *v, unsigned nbytes)
{
  for (unsigned i = 0; i < nbytes; i++)
    {
      printf("%02x ", v[i]);
    }
  printf("\n");
}
#endif

static void
config_endian()
{
  union {
    uint32_t i;
    uint8_t d[4];
  } u;
  u.i = 0x01020304;
  if (memcmp (u.d, "\001\002\003\004", 4) == 0)
    printf ("#define BULP_IS_LITTLE_ENDIAN 0\n");
  else if (memcmp (u.d, "\004\003\002\001", 4) == 0)
    printf ("#define BULP_IS_LITTLE_ENDIAN 1\n");
  else
    assert(0);
}

/* 8-bit.  */
struct Packed8_1bit_1 { uint8_t a:1; };
struct Packed8_1bit_2 { uint8_t a:1, b:1; };
struct Packed8_1bit_3 { uint8_t a:1, b:1, c:1; };
struct Packed8_1bit_4 { uint8_t a:1, b:1, c:1, d:1; };
struct Packed8_1bit_5 { uint8_t a:1, b:1, c:1, d:1, e:1; };
struct Packed8_1bit_6 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1; };
struct Packed8_1bit_7 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1, g:1; };
struct Packed8_1bit_8 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1; };
struct Packed8_1bit_9 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1, i:1; };

struct Packed8_2bit_1 { uint8_t a:2; };
struct Packed8_2bit_2 { uint8_t a:2, b:2; };
struct Packed8_2bit_3 { uint8_t a:2, b:2, c:2; };
struct Packed8_2bit_4 { uint8_t a:2, b:2, c:2, d:2; };
struct Packed8_2bit_5 { uint8_t a:2, b:2, c:2, d:2, e:2; };

/* last 2-bit int might straddle byte boundary */
struct Packed8_11111112 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:2; };
struct Packed8_11111113 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:3; };
struct Packed8_11111114 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:4; };
struct Packed8_11111115 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:5; };
struct Packed8_11111116 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:6; };
struct Packed8_11111117 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:7; };
struct Packed8_11111118 { uint8_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:8; };

struct Packed8_TestStruct {
  struct Packed8_1bit_1    packed8_1bit_1;
  struct Packed8_1bit_2    packed8_1bit_2;
  struct Packed8_1bit_3    packed8_1bit_3;
  struct Packed8_1bit_4    packed8_1bit_4;
  struct Packed8_1bit_5    packed8_1bit_5;
  struct Packed8_1bit_6    packed8_1bit_6;
  struct Packed8_1bit_7    packed8_1bit_7;
  struct Packed8_1bit_8    packed8_1bit_8;
  struct Packed8_1bit_9    packed8_1bit_9;
  struct Packed8_2bit_1    packed8_2bit_1;
  struct Packed8_2bit_2    packed8_2bit_2;
  struct Packed8_2bit_3    packed8_2bit_3;
  struct Packed8_2bit_4    packed8_2bit_4;
  struct Packed8_2bit_5    packed8_2bit_5;
  struct Packed8_11111112  packed8_11111112;
  struct Packed8_11111113  packed8_11111113;
  struct Packed8_11111114  packed8_11111114;
  struct Packed8_11111115  packed8_11111115;
  struct Packed8_11111116  packed8_11111116;
  struct Packed8_11111117  packed8_11111117;
  struct Packed8_11111118  packed8_11111118;
};

static void
config_8bit (void)
{
  struct Packed8_TestStruct t;
  bulp_bool is_le;
  bulp_bool straddle;

  if (sizeof (struct Packed8_1bit_1) != 1)
    {
      fprintf(stderr, "min struct size != 1 not supported.\n");
      exit(1);
    }


#define RESET_T()      memset(&t, 0, sizeof (t));
  RESET_T();
  t.packed8_1bit_1.a = 1;
  if (memcmp (&t.packed8_1bit_1, "\1", 1) == 0)
    {
      // little endian packing:  assert assumptions
      t.packed8_1bit_2.a = 1;
      t.packed8_1bit_2.b = 0;
      if (memcmp (&t.packed8_1bit_2, "\1", 1) == 0)
      t.packed8_1bit_2.a = 0;
      t.packed8_1bit_2.b = 1;
      if (memcmp (&t.packed8_1bit_2, "\2", 1) == 0)

      // TODO more assertions

      // test the full 8-member, each 1 bit case
      assert(sizeof (struct Packed8_1bit_8) == 1);
      t.packed8_1bit_8.a = 1;
      assert(memcmp (&t.packed8_1bit_8, "\1", 1) == 0);
      t.packed8_1bit_8.a = 0;
      t.packed8_1bit_8.b = 1;
      assert(memcmp (&t.packed8_1bit_8, "\2", 1) == 0);
      t.packed8_1bit_8.b = 0;
      t.packed8_1bit_8.c = 1;
      assert(memcmp (&t.packed8_1bit_8, "\4", 1) == 0);
      t.packed8_1bit_8.c = 0;
      t.packed8_1bit_8.d = 1;
      assert(memcmp (&t.packed8_1bit_8, "\10", 1) == 0);
      t.packed8_1bit_8.d = 0;
      t.packed8_1bit_8.e = 1;
      assert(memcmp (&t.packed8_1bit_8, "\20", 1) == 0);
      t.packed8_1bit_8.e = 0;
      t.packed8_1bit_8.f = 1;
      assert(memcmp (&t.packed8_1bit_8, "\40", 1) == 0);
      t.packed8_1bit_8.f = 0;
      t.packed8_1bit_8.g = 1;
      assert(memcmp (&t.packed8_1bit_8, "\100", 1) == 0);
      t.packed8_1bit_8.g = 0;
      t.packed8_1bit_8.h = 1;
      assert(memcmp (&t.packed8_1bit_8, "\200", 1) == 0);

      // test the 9-bit case, to make sure it is handled in the obvious way
      assert(sizeof (struct Packed8_1bit_9) == 2);
      t.packed8_1bit_9.a = 1;
      assert(memcmp (&t.packed8_1bit_9, "\1\0", 2) == 0);
      t.packed8_1bit_9.a = 0;
      t.packed8_1bit_9.b = 1;
      assert(memcmp (&t.packed8_1bit_9, "\2\0", 1) == 0);
      t.packed8_1bit_9.b = 0;
      t.packed8_1bit_9.c = 1;
      assert(memcmp (&t.packed8_1bit_9, "\4\0", 1) == 0);
      t.packed8_1bit_9.c = 0;
      t.packed8_1bit_9.d = 1;
      assert(memcmp (&t.packed8_1bit_9, "\10\0", 1) == 0);
      t.packed8_1bit_9.d = 0;
      t.packed8_1bit_9.e = 1;
      assert(memcmp (&t.packed8_1bit_9, "\20\0", 1) == 0);
      t.packed8_1bit_9.e = 0;
      t.packed8_1bit_9.f = 1;
      assert(memcmp (&t.packed8_1bit_9, "\40\0", 1) == 0);
      t.packed8_1bit_9.f = 0;
      t.packed8_1bit_9.g = 1;
      assert(memcmp (&t.packed8_1bit_9, "\100\0", 1) == 0);
      t.packed8_1bit_9.g = 0;
      t.packed8_1bit_9.h = 1;
      assert(memcmp (&t.packed8_1bit_9, "\200\0", 1) == 0);
      t.packed8_1bit_9.h = 0;
      t.packed8_1bit_9.i = 1;
      assert(memcmp (&t.packed8_1bit_9, "\0\1", 1) == 0);

      assert(sizeof(t.packed8_2bit_5) == 2);
      t.packed8_2bit_5.a = 0;
      t.packed8_2bit_5.b = 1;
      t.packed8_2bit_5.c = 2;
      t.packed8_2bit_5.d = 3;
      t.packed8_2bit_5.e = 2;
      /*expected: 11100100 00000010    == e402*/
      assert(memcmp(&t.packed8_2bit_5, "\xe4\x02", 2) == 0);

      is_le = BULP_TRUE;
    }
  else if (memcmp (&t.packed8_1bit_1, "\200", 1) == 0)
    {
      // big endian packing:  assert assumptions
      assert(0);                // not done
    }
  else
    {
      fprintf(stderr, "non-understood packing order: %02x (%s:%u)\n",
         ((uint8_t *)(&t.packed8_1bit_1))[0], __FILE__, __LINE__);
      exit(1);
    }

#define LE_OR_BE(le, be) ((is_le) ? (le) : (be))

  /* Compute whether multibit fields in uint8_t contexts
     can straddle the byte boundary */
  assert(sizeof (struct Packed8_11111112) == 2);
  t.packed8_11111112.a = 1;
  assert (memcmp (&t.packed8_11111112, LE_OR_BE ("\1\0", "\200\0"), 2) == 0);

  t.packed8_11111112.a = 0;
  t.packed8_11111112.h = 1;
  if (memcmp (&t.packed8_11111112, LE_OR_BE ("\0\1", "\0\100"), 2) == 0)
    { 
      // straddle == false
      straddle = BULP_FALSE;
      t.packed8_11111112.a = 0;
      t.packed8_11111112.h = 2;
      assert (memcmp (&t.packed8_11111112, LE_OR_BE ("\0\2", "\0\200"), 2) == 0);
      t.packed8_11111112.a = 0;
      t.packed8_11111112.h = 3;
      assert (memcmp (&t.packed8_11111112, LE_OR_BE ("\0\3", "\0\300"), 2) == 0);
      t.packed8_11111113.a = 0;
      t.packed8_11111113.h = 4;
      assert (memcmp (&t.packed8_11111113, LE_OR_BE ("\0\4", "\0\200"), 2) == 0);
      t.packed8_11111113.a = 0;
      t.packed8_11111113.h = 5;
      assert (memcmp (&t.packed8_11111113, LE_OR_BE ("\0\5", "\0\240"), 2) == 0);
      t.packed8_11111114.a = 0;
      t.packed8_11111114.h = 1;
      assert (memcmp (&t.packed8_11111114, LE_OR_BE ("\0\1", "\020\000"), 2) == 0);
      t.packed8_11111114.a = 0;
      t.packed8_11111114.h = 2;
      assert (memcmp (&t.packed8_11111114, LE_OR_BE ("\0\2", "\040\000"), 2) == 0);
      t.packed8_11111114.a = 0;
      t.packed8_11111114.h = 4;
      assert (memcmp (&t.packed8_11111114, LE_OR_BE ("\0\4", "\000\004"), 2) == 0);
      t.packed8_11111114.a = 0;
      t.packed8_11111114.h = 8;
      assert (memcmp (&t.packed8_11111114, LE_OR_BE ("\0\10", "\000\010"), 2) == 0);
      ///...
    }
  else if (memcmp (&t.packed8_11111112, LE_OR_BE ("\200\0", "\1\0"), 2) == 0)
    { 
      // straddle == true
      straddle = BULP_TRUE;
      ///...
    }
  else
    {
      // error: straddle == wtf
      fprintf(stderr, "non-understood multibit-straddle strategy for bytes %02x%02x (%s:%u)\n",
         ((uint8_t *)(&t.packed8_11111112))[0],
         ((uint8_t *)(&t.packed8_11111112))[1],
         __FILE__, __LINE__);
      exit(1);
    }

  printf ("#define BULP_CONFIG_BITPACK_FORMAT8_IS_LITTLE_ENDIAN    %u\n", is_le);
  printf ("#define BULP_CONFIG_BITPACK_FORMAT8_CAN_FIELDS_STRADDLE_BYTE_BOUNDARY    %u\n", straddle);
}

struct Packed16_1bit_1  { uint16_t a:1; };
struct Packed16_1bit_2  { uint16_t a:1, b:1; };
struct Packed16_1bit_3  { uint16_t a:1, b:1, c:1; };
struct Packed16_1bit_4  { uint16_t a:1, b:1, c:1, d:1; };
struct Packed16_1bit_5  { uint16_t a:1, b:1, c:1, d:1, e:1; };
struct Packed16_1bit_6  { uint16_t a:1, b:1, c:1, d:1, e:1, f:1; };
struct Packed16_1bit_7  { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1; };
struct Packed16_1bit_8  { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1; };
struct Packed16_1bit_9  { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1,
                                   i:1, j:1; };
struct Packed16_1bit_10 { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1,
                                   i:1, j:1; };
struct Packed16_1bit_11 { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1,
                                   i:1, j:1, k:1; };
struct Packed16_1bit_12 { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1,
                                   i:1, j:1, k:1, l:1; };
struct Packed16_1bit_13 { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1,
                                   i:1, j:1, k:1, l:1, m:1; };
struct Packed16_1bit_14 { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1,
                                   i:1, j:1, k:1, l:1, m:1, n:1; };
struct Packed16_1bit_15 { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1,
                                   i:1, j:1, k:1, l:1, m:1, n:1, o:1; };
struct Packed16_1bit_16 { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1,
                                   i:1, j:1, k:1, l:1, m:1, n:1, o:1, p:1; };
struct Packed16_1bit_17 { uint16_t a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1,
                                   i:1, j:1, k:1, l:1, m:1, n:1, o:1, p:1,
                                   q:1; };
struct Packed16_2bit_1 { uint16_t a:2; };
struct Packed16_2bit_2 { uint16_t a:2, b:2; };
struct Packed16_2bit_3 { uint16_t a:2, b:2, c:2; };
struct Packed16_2bit_4 { uint16_t a:2, b:2, c:2, d:2; };
struct Packed16_2bit_5 { uint16_t a:2, b:2, c:2, d:2, e:2; };
struct Packed16_2bit_6 { uint16_t a:2, b:2, c:2, d:2, e:2, f:2; };
struct Packed16_2bit_7 { uint16_t a:2, b:2, c:2, d:2, e:2, f:2, g:2; };
struct Packed16_2bit_8 { uint16_t a:2, b:2, c:2, d:2, e:2, f:2, g:2, h:2; };
struct Packed16_2bit_9 { uint16_t a:2, b:2, c:2, d:2, e:2, f:2, g:2, h:2, i:2; };

struct Packed16_44454 {uint16_t a:4, b:4, c:4, d:5, e:4; };
struct Packed16_22222224 {uint16_t a:2, b:2, c:2, d:2, e:2, f:2, g:2, h:4; };

struct Packed16_TestStruct {
  struct Packed16_1bit_1      packed16_1bit_1;
  struct Packed16_1bit_2      packed16_1bit_2;
  struct Packed16_1bit_3      packed16_1bit_3;
  struct Packed16_1bit_4      packed16_1bit_4;
  struct Packed16_1bit_5      packed16_1bit_5;
  struct Packed16_1bit_6      packed16_1bit_6;
  struct Packed16_1bit_7      packed16_1bit_7;
  struct Packed16_1bit_8      packed16_1bit_8;
  struct Packed16_1bit_9      packed16_1bit_9;
  struct Packed16_1bit_10     packed16_1bit_10;
  struct Packed16_1bit_11     packed16_1bit_11;
  struct Packed16_1bit_12     packed16_1bit_12;
  struct Packed16_1bit_13     packed16_1bit_13;
  struct Packed16_1bit_14     packed16_1bit_14;
  struct Packed16_1bit_15     packed16_1bit_15;
  struct Packed16_1bit_16     packed16_1bit_16;
  struct Packed16_1bit_17     packed16_1bit_17;
  struct Packed16_2bit_1      packed16_2bit_1;
  struct Packed16_2bit_2      packed16_2bit_2;
  struct Packed16_2bit_3      packed16_2bit_3;
  struct Packed16_2bit_4      packed16_2bit_4;
  struct Packed16_2bit_5      packed16_2bit_5;
  struct Packed16_2bit_6      packed16_2bit_6;
  struct Packed16_2bit_7      packed16_2bit_7;
  struct Packed16_2bit_8      packed16_2bit_8;
  struct Packed16_2bit_9      packed16_2bit_9;
  struct Packed16_44454       packed16_44454;
  struct Packed16_22222224    packed16_22222224;
};

static bulp_bool equals_uint16_array(const void *data, unsigned N, ...)
{
  uint16_t *arr = alloca(sizeof(uint16_t) * N);
  va_list args;
  va_start(args, N);
  for (unsigned i = 0; i < N; i++)
    arr[i] = va_arg(args, int);
  va_end(args);
  return memcmp (arr, data, N * 2) == 0;
}

static void
config_16bit (void)
{
  struct Packed16_TestStruct t;
  RESET_T();
  bulp_bool is_le, straddle;

  assert(sizeof(t.packed16_1bit_1) == 2);
  assert(memcmp (&t.packed16_1bit_1, "\0\0", 2) == 0);
  t.packed16_1bit_1.a = 1;
  if (equals_uint16_array (&t.packed16_1bit_1, 1, 1))
    {
      is_le = BULP_TRUE;

      assert(sizeof(t.packed16_1bit_3) == 2);
      t.packed16_1bit_3.c = 1;
      assert (memcmp (&t.packed16_1bit_3, "\4\0", 2) == 0);
    }
  else if (equals_uint16_array (&t.packed16_1bit_1, 1, 0x8000))
    {
      is_le = BULP_FALSE;

      assert(0);                // TODO big endian support
    }
  else
    {
      fprintf(stderr, "non-understood packing order: %04x (%s:%u)\n",
         ((uint16_t *)(&t.packed16_1bit_1))[0],
         __FILE__, __LINE__);
      exit(1);
    }
  printf ("#define BULP_CONFIG_BITPACK_FORMAT16_IS_LITTLE_ENDIAN    %u\n", is_le);
  assert(sizeof(t.packed16_44454) == 4);
  t.packed16_44454.d = 31;
  if (equals_uint16_array (&t.packed16_44454, 2, 0, 31))
    {
      straddle = BULP_FALSE;
      t.packed16_44454.d = 1;
      assert(equals_uint16_array(&t.packed16_44454, 2, 0x0000, 0x0001));
      t.packed16_44454.d = 2;
      assert(equals_uint16_array(&t.packed16_44454, 2, 0x0000, 0x0002));
      t.packed16_44454.d = 4;
      assert(equals_uint16_array(&t.packed16_44454, 2, 0x0000, 0x0004));
      t.packed16_44454.d = 16;
      assert(equals_uint16_array(&t.packed16_44454, 2, 0x0000, 0x0010));
      t.packed16_44454.d = 0;
      t.packed16_44454.e = 1;
      assert(equals_uint16_array(&t.packed16_44454, 2, 0x0000, 0x0020));
      t.packed16_44454.e = 2;
      assert(equals_uint16_array(&t.packed16_44454, 2, 0x0000, 0x0040));

      assert(sizeof (t.packed16_22222224) == 4);
      t.packed16_22222224.h = 15;
      assert(equals_uint16_array(&t.packed16_22222224, 2, 0, 15));
      t.packed16_22222224.h = 1;
      assert(equals_uint16_array(&t.packed16_22222224, 2, 0, 1));
      t.packed16_22222224.h = 8;
      assert(equals_uint16_array(&t.packed16_22222224, 2, 0, 8));
    }
  else if (equals_uint16_array (&t.packed16_44454, 2, 0xd000, 0x0001))
    {
      straddle = BULP_TRUE;
      assert(0);   // MORE ASSERTS
    }
  else
    {
      // error: straddle == wtf
      fprintf(stderr, "non-understood multibit-straddle strategy for uint16 %04x %04x (%s:%u)\n",
         ((uint16_t *)(&t.packed16_44454))[0],
         ((uint16_t *)(&t.packed16_44454))[1],
         __FILE__, __LINE__);
      exit(1);
    }
  printf ("#define BULP_CONFIG_BITPACK_FORMAT16_CAN_FIELDS_STRADDLE_BYTE_BOUNDARY    %u\n", straddle);
}

//for ($i=1;$i<=33;$i++) { print("a$i:1, ") }

struct Packed32_1bit_1 {
  uint32_t a1:1;
};
struct Packed32_1bit_2 {
  uint32_t a1:1, a2:1;
};
struct Packed32_1bit_3 {
  uint32_t a1:1, a2:1, a3:1;
};
struct Packed32_1bit_5 {
  uint32_t a1:1, a2:1, a3:1, a4:1, a5:1;
};
struct Packed32_1bit_11 {
  uint32_t a1:1, a2:1, a3:1, a4:1, a5:1, a6:1, a7:1, a8:1, a9:1, a10:1, a11:1;
};
struct Packed32_1bit_19 {
  uint32_t a1:1, a2:1, a3:1, a4:1, a5:1, a6:1, a7:1, a8:1, a9:1, a10:1, a11:1, a12:1, a13:1, a14:1, a15:1, a16:1, a17:1, a18:1, a19:1;
};
struct Packed32_1bit_29 {
  uint32_t a1:1, a2:1, a3:1, a4:1, a5:1, a6:1, a7:1, a8:1, a9:1, a10:1, a11:1, a12:1, a13:1, a14:1, a15:1, a16:1, a17:1, a18:1, a19:1, a20:1, a21:1, a22:1, a23:1, a24:1, a25:1, a26:1, a27:1, a28:1, a29:1;
};
struct Packed32_1bit_33 {
  uint32_t a1:1, a2:1, a3:1, a4:1, a5:1, a6:1, a7:1, a8:1, a9:1, a10:1, a11:1, a12:1, a13:1, a14:1, a15:1, a16:1, a17:1, a18:1, a19:1, a20:1, a21:1, a22:1, a23:1, a24:1, a25:1, a26:1, a27:1, a28:1, a29:1, a30:1, a31:1, a32:1, a33:1; 
};

struct Packed32_31_2 {
  uint32_t a:31, b:2;
};
struct Packed32_7bit_5 {
  uint32_t a:7, b:7, c:7, d:7, e:7;
};

struct Packed32_TestStruct {
  struct Packed32_1bit_1      packed32_1bit_1;
  struct Packed32_1bit_2      packed32_1bit_2;
  struct Packed32_1bit_3      packed32_1bit_3;
  struct Packed32_1bit_5      packed32_1bit_5;
  struct Packed32_1bit_11     packed32_1bit_11;
  struct Packed32_1bit_19     packed32_1bit_19;
  struct Packed32_1bit_29     packed32_1bit_29;
  struct Packed32_1bit_33     packed32_1bit_33;
  struct Packed32_31_2        packed32_31_2;
  struct Packed32_7bit_5      packed32_7bit_5;
};
static bulp_bool equals_uint32_array(const void *data, unsigned N, ...)
{
  uint32_t *arr = alloca(sizeof(uint32_t) * N);
  va_list args;
  va_start(args, N);
  for (unsigned i = 0; i < N; i++)
    arr[i] = va_arg(args, unsigned);
  va_end(args);
  return memcmp (arr, data, N * 4) == 0;
}
static void
config_32bit (void)
{
  struct Packed32_TestStruct t;
  RESET_T();
  bulp_bool is_le, straddle;

  assert(sizeof(t.packed32_1bit_1) == 4);
  t.packed32_1bit_1.a1 = 1;
  if (equals_uint32_array (&t.packed32_1bit_1, 1, 1))
    {
      is_le = BULP_TRUE;
      t.packed32_1bit_3.a3 = 1;
      assert(equals_uint32_array (&t.packed32_1bit_3, 1, 4));
      t.packed32_1bit_5.a5 = 1;
      assert(equals_uint32_array (&t.packed32_1bit_5, 1, 16));

      // TODO: etc

      assert(sizeof(t.packed32_1bit_33) == 8);
      t.packed32_1bit_33.a1 = 1;
      assert(equals_uint32_array (&t.packed32_1bit_33, 2, 1, 0));
      t.packed32_1bit_33.a1 = 0;
      t.packed32_1bit_33.a2 = 1;
      assert(equals_uint32_array (&t.packed32_1bit_33, 2, 2, 0));
      t.packed32_1bit_33.a2 = 0;
      t.packed32_1bit_33.a33 = 1;
      assert(equals_uint32_array (&t.packed32_1bit_33, 2, 0, 1));

    }
  else if (equals_uint32_array (&t.packed32_1bit_1, 1, 0x80000000))
    {
      is_le = BULP_FALSE;
      assert(0);
    }
  else
    {
      fprintf(stderr, "non-understood packing order: %08x (%s:%u)\n",
         ((uint32_t *)(&t.packed32_1bit_1))[0],
         __FILE__, __LINE__);
      exit(1);
    }

  assert(sizeof(t.packed32_31_2) == 8);
  t.packed32_31_2.b = 1;
  if (equals_uint32_array (&t.packed32_31_2, 2, 0, LE_OR_BE(1, 0x40000000)))
    {
      straddle = BULP_FALSE;
      t.packed32_31_2.b = 2;
      assert(equals_uint32_array (&t.packed32_31_2, 2, 0, LE_OR_BE(2, 0x80000000)));
    }
  else if (equals_uint32_array (&t.packed32_31_2, 2, LE_OR_BE(1, 0), LE_OR_BE(0, 0x80000000)))
    {
      straddle = BULP_TRUE;
      t.packed32_31_2.b = 2;
      assert(equals_uint32_array (&t.packed32_31_2, 2, LE_OR_BE(0, 1), LE_OR_BE(1, 0)));
    }
  else
    {
      fprintf(stderr, "non-understood multibit-straddle strategy for uint32 %08x %08x (%s:%u)\n",
         ((uint32_t *)(&t.packed32_31_2))[0],
         ((uint32_t *)(&t.packed32_31_2))[1],
         __FILE__, __LINE__);
      exit(1);
    }

  printf ("#define BULP_CONFIG_BITPACK_FORMAT32_IS_LITTLE_ENDIAN    %u\n", is_le);
  printf ("#define BULP_CONFIG_BITPACK_FORMAT32_CAN_FIELDS_STRADDLE_BYTE_BOUNDARY    %u\n", straddle);
}

struct AlignInt16Test { char c; uint16_t v; };
struct AlignInt32Test { char c; uint32_t v; };
struct AlignInt64Test { char c; uint64_t v; };
struct AlignFloat32Test { char c; float v; };
struct AlignFloat64Test { char c; double v; };
struct AlignPointerTest { char c; void * v; };

static void
config_align (void)
{
  printf ("#define BULP_INT8_ALIGNOF 1\n");
  printf ("#define BULP_INT16_ALIGNOF %u\n", (unsigned) offsetof(struct AlignInt16Test, v));
  printf ("#define BULP_INT32_ALIGNOF %u\n", (unsigned) offsetof(struct AlignInt32Test, v));
  printf ("#define BULP_INT64_ALIGNOF %u\n", (unsigned) offsetof(struct AlignInt64Test, v));
  printf ("#define BULP_FLOAT32_ALIGNOF %u\n", (unsigned) offsetof(struct AlignFloat32Test, v));
  printf ("#define BULP_FLOAT64_ALIGNOF %u\n", (unsigned) offsetof(struct AlignFloat64Test, v));
  printf ("#define BULP_POINTER_ALIGNOF %u\n", (unsigned) offsetof(struct AlignPointerTest, v));
}

enum TinyEnum { TINY_A, TINY_B };
enum ShortEnum { SHORT_A, SHORT_B, SHORT_C = 0x100 };
enum IntEnum { INT_A, INT_B, INT_C = 0x10000 };
struct AlignTinyEnumTest { char c; enum TinyEnum v; };
struct AlignShortEnumTest { char c; enum ShortEnum v; };
struct AlignIntEnumTest { char c; enum IntEnum v; };

static void
config_enum_sizes (void)
{
  printf ("#define BULP_SIZEOF_TINY_ENUM %u\n", (unsigned) sizeof (enum TinyEnum));
  printf ("#define BULP_SIZEOF_SHORT_ENUM %u\n", (unsigned) sizeof (enum ShortEnum));
  printf ("#define BULP_SIZEOF_INT_ENUM %u\n", (unsigned) sizeof (enum IntEnum));
  printf ("#define BULP_ALIGNOF_TINY_ENUM %u\n", (unsigned) offsetof(struct AlignTinyEnumTest, v));
  printf ("#define BULP_ALIGNOF_SHORT_ENUM %u\n", (unsigned) offsetof(struct AlignShortEnumTest, v));
  printf ("#define BULP_ALIGNOF_INT_ENUM %u\n", (unsigned) offsetof(struct AlignIntEnumTest, v));
}

int main()
{
  config_endian();
  config_8bit();
  config_16bit();
  config_32bit();
  config_align();
  config_enum_sizes();
  return 0;
}
