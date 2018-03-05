#include "bulp-test.h"
#include <stdio.h>
#include <string.h>


static void
dump_binary_as_hex (const char *prefix, size_t len, const uint8_t *data)
{
  printf("%s", prefix);
  for (size_t i = 0; i < len; i++)
      printf(" %02x", data[i]);
  printf("\n");
}
static void
test__int8_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "int8", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(f->v_int.is_signed);
  assert(f->v_int.byte_size == 1);
  assert(!f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "int8") == 0);

  int8_t min_value = -128, max_value = 127, zero = 0;
  TEST_ASSERT (bulp_int8_get_packed_size (min_value) == 1);
  TEST_ASSERT (bulp_int8_get_packed_size (zero) == 1);
  TEST_ASSERT (bulp_int8_get_packed_size (max_value) == 1);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &min_value) == 1);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &zero) == 1);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &max_value) == 1);

  // test pack
  uint8_t out[16];
  TEST_ASSERT (bulp_int8_pack (min_value, out) == 1);
  TEST_ASSERT (out[0] == (uint8_t) min_value);
  TEST_ASSERT (bulp_int8_pack (zero, out) == 1);
  TEST_ASSERT (out[0] == (uint8_t) zero);
  TEST_ASSERT (bulp_int8_pack (max_value, out) == 1);
  TEST_ASSERT (out[0] == (uint8_t) max_value);

  TEST_ASSERT (f->base.vfuncs.pack (f, &min_value, out) == 1);
  TEST_ASSERT (out[0] == (uint8_t) min_value);
  TEST_ASSERT (f->base.vfuncs.pack (f, &zero, out) == 1);
  TEST_ASSERT (out[0] == (uint8_t) zero);
  TEST_ASSERT (f->base.vfuncs.pack (f, &max_value, out) == 1);
  TEST_ASSERT (out[0] == (uint8_t) max_value);

  // test pack_to
  uint8_t packed_data[1];
  BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 2, 2);
  bulp_int8_pack_to (min_value, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 0x80);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  bulp_int8_pack_to (zero, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 0);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  bulp_int8_pack_to (max_value, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 0x7f);

  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &min_value, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 0x80);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &zero, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 0);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &max_value, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 0x7f);
  bulp_data_builder_clear (&builder);           // does nothing, since all allocations are on stack

  // test unpack
  packed_data[0] = 0x80;
  int8_t v;
  BulpError *error = NULL;
  TEST_ASSERT (bulp_int8_unpack (1, packed_data, &v, &error) == 1);
  assert(v == min_value);
  packed_data[0] = 0;
  TEST_ASSERT (bulp_int8_unpack (1, packed_data, &v, &error) == 1);
  assert(v == 0);
  packed_data[0] = 0x7f;
  TEST_ASSERT (bulp_int8_unpack (1, packed_data, &v, &error) == 1);
  assert(v == max_value);

  packed_data[0] = 0x80;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 1, packed_data, &v, NULL, &error) == 1);
  assert(v == min_value);
  packed_data[0] = 0;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 1, packed_data, &v, NULL, &error) == 1);
  assert(v == 0);
  packed_data[0] = 0x7f;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 1, packed_data, &v, NULL, &error) == 1);
  assert(v == max_value);
}

static void
test__int16_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "int16", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(f->v_int.is_signed);
  assert(f->v_int.byte_size == 2);
  assert(!f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "int16") == 0);

  int16_t min_value = -32768, max_value = 32767, zero = 0, x1234 = 0x1234;
  TEST_ASSERT (bulp_int16_get_packed_size (min_value) == 2);
  TEST_ASSERT (bulp_int16_get_packed_size (zero) == 2);
  TEST_ASSERT (bulp_int16_get_packed_size (max_value) == 2);
  TEST_ASSERT (bulp_int16_get_packed_size (x1234) == 2);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &min_value) == 2);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &zero) == 2);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &max_value) == 2);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &x1234) == 2);

  // test pack
  uint8_t out[16];
  TEST_ASSERT (bulp_int16_pack (min_value, out) == 2);
  TEST_ASSERT (out[0] == 0x00);
  TEST_ASSERT (out[1] == 0x80);
  TEST_ASSERT (bulp_int16_pack (zero, out) == 2);
  TEST_ASSERT (out[0] == 0);
  TEST_ASSERT (out[1] == 0);
  TEST_ASSERT (bulp_int16_pack (max_value, out) == 2);
  TEST_ASSERT (out[0] == 0xff);
  TEST_ASSERT (out[1] == 0x7f);
  TEST_ASSERT (bulp_int16_pack (x1234, out) == 2);
  TEST_ASSERT (out[0] == 0x34);
  TEST_ASSERT (out[1] == 0x12);

  TEST_ASSERT (f->base.vfuncs.pack (f, &min_value, out) == 2);
  TEST_ASSERT (out[0] == 0x00);
  TEST_ASSERT (out[1] == 0x80);
  TEST_ASSERT (f->base.vfuncs.pack (f, &zero, out) == 2);
  TEST_ASSERT (out[0] == 0);
  TEST_ASSERT (out[1] == 0);
  TEST_ASSERT (f->base.vfuncs.pack (f, &max_value, out) == 2);
  TEST_ASSERT (out[0] == 0xff);
  TEST_ASSERT (out[1] == 0x7f);
  TEST_ASSERT (f->base.vfuncs.pack (f, &x1234, out) == 2);
  TEST_ASSERT (out[0] == 0x34);
  TEST_ASSERT (out[1] == 0x12);

  // test pack_to
  uint8_t packed_data[2];
  BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 2, 2);
  bulp_int16_pack_to (min_value, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0x00);
  TEST_ASSERT (packed_data[1] == 0x80);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  bulp_int16_pack_to (zero, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0x00);
  TEST_ASSERT (packed_data[1] == 0x00);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  bulp_int16_pack_to (max_value, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0xff);
  TEST_ASSERT (packed_data[1] == 0x7f);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  bulp_int16_pack_to (x1234, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0x34);
  TEST_ASSERT (packed_data[1] == 0x12);

  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &min_value, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0x00);
  TEST_ASSERT (packed_data[1] == 0x80);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &zero, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0x00);
  TEST_ASSERT (packed_data[1] == 0x00);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &max_value, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0xff);
  TEST_ASSERT (packed_data[1] == 0x7f);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &x1234, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0x34);
  TEST_ASSERT (packed_data[1] == 0x12);
  bulp_data_builder_clear (&builder);           // does nothing, since all allocations are on stack

  // test unpack
  int16_t v;
  BulpError *error = NULL;
  packed_data[0] = 0x00;
  packed_data[1] = 0x80;
  TEST_ASSERT (bulp_int16_unpack (2, packed_data, &v, &error) == 2);
  assert(v == min_value);
  packed_data[0] = 0;
  packed_data[1] = 0;
  TEST_ASSERT (bulp_int16_unpack (2, packed_data, &v, &error) == 2);
  assert(v == 0);
  packed_data[0] = 0xff;
  packed_data[1] = 0x7f;
  TEST_ASSERT (bulp_int16_unpack (2, packed_data, &v, &error) == 2);
  assert(v == max_value);
  packed_data[0] = 0x34;
  packed_data[1] = 0x12;
  TEST_ASSERT (bulp_int16_unpack (2, packed_data, &v, &error) == 2);
  assert(v == x1234);


  packed_data[0] = 0x00;
  packed_data[1] = 0x80;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 2, packed_data, &v, NULL, &error) == 2);
  assert(v == min_value);
  packed_data[0] = 0;
  packed_data[1] = 0;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 2, packed_data, &v, NULL, &error) == 2);
  assert(v == 0);
  packed_data[0] = 0xff;
  packed_data[1] = 0x7f;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 2, packed_data, &v, NULL, &error) == 2);
  assert(v == max_value);
  packed_data[0] = 0x34;
  packed_data[1] = 0x12;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 2, packed_data, &v, NULL, &error) == 2);
  assert(v == x1234);
}

static void
test__int32_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "int32", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(f->v_int.is_signed);
  assert(f->v_int.byte_size == 4);
  assert(!f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "int32") == 0);

  int32_t min_value = INT32_MIN, max_value = INT32_MAX, zero = 0, x12345678 = 0x12345678;
  uint8_t min_value_bytes[4] = {0,0,0,0x80};
  uint8_t max_value_bytes[4] = {0xff,0xff,0xff,0x7f};
  uint8_t zero_bytes[4] = {0,0,0,0};
  uint8_t x12345678_bytes[4] = {0x78, 0x56, 0x34, 0x12};
  TEST_ASSERT (bulp_int32_get_packed_size (min_value) == 4);
  TEST_ASSERT (bulp_int32_get_packed_size (zero) == 4);
  TEST_ASSERT (bulp_int32_get_packed_size (max_value) == 4);
  TEST_ASSERT (bulp_int32_get_packed_size (x12345678) == 4);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &min_value) == 4);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &zero) == 4);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &max_value) == 4);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &x12345678) == 4);

  // test pack
  uint8_t out[16];
#define TEST_INT32_PACK(vname) \
  TEST_ASSERT (bulp_int32_pack (vname, out) == 4); \
  TEST_ASSERT (memcmp (vname##_bytes, out, 4) == 0)
  TEST_INT32_PACK(min_value);
  TEST_INT32_PACK(zero);
  TEST_INT32_PACK(max_value);
  TEST_INT32_PACK(x12345678);
#undef TEST_INT32_PACK

#define TEST_INT32_PACK_VIA_FORMAT(vname) \
  TEST_ASSERT (f->base.vfuncs.pack (f, &vname, out) == 4); \
  TEST_ASSERT (memcmp (vname##_bytes, out, 4) == 0)
  TEST_INT32_PACK_VIA_FORMAT(min_value);
  TEST_INT32_PACK_VIA_FORMAT(zero);
  TEST_INT32_PACK_VIA_FORMAT(max_value);
  TEST_INT32_PACK_VIA_FORMAT(x12345678);
#undef TEST_INT32_PACK_VIA_FORMAT

  // test pack_to
  uint8_t packed_data[4];
  BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 20, 20);

#define TEST_INT32_PACK_TO(vname) \
  BULP_DATA_BUILDER_RESET_ON_STACK (builder); \
  bulp_int32_pack_to (vname, &builder); \
  assert(builder.cur_len == 4); \
  bulp_data_builder_build (&builder, packed_data); \
  TEST_ASSERT (memcmp (vname##_bytes, packed_data, 4) == 0)

  TEST_INT32_PACK_TO(min_value);
  TEST_INT32_PACK_TO(zero);
  TEST_INT32_PACK_TO(max_value);
  TEST_INT32_PACK_TO(x12345678);

#undef TEST_INT32_PACK_TO
#define TEST_INT32_PACK_TO_VIA_FORMAT(vname) \
  BULP_DATA_BUILDER_RESET_ON_STACK (builder); \
  f->base.vfuncs.pack_to (f, &vname, &builder); \
  assert(builder.cur_len == 4); \
  bulp_data_builder_build (&builder, packed_data); \
  TEST_ASSERT (memcmp (vname##_bytes, packed_data, 4) == 0)
  TEST_INT32_PACK_TO_VIA_FORMAT(min_value);
  TEST_INT32_PACK_TO_VIA_FORMAT(zero);
  TEST_INT32_PACK_TO_VIA_FORMAT(max_value);
  TEST_INT32_PACK_TO_VIA_FORMAT(x12345678);
#undef TEST_INT32_PACK_TO_VIA_FORMAT


  // test unpack
  int32_t v;
  BulpError *error = NULL;

#define TEST_INT32_UNPACK(vname) \
  memcpy (packed_data, vname ## _bytes, 4); \
  TEST_ASSERT (bulp_int32_unpack (4, packed_data, &v, &error) == 4); \
  TEST_ASSERT(v == vname)
  TEST_INT32_UNPACK(min_value);
  TEST_INT32_UNPACK(zero);
  TEST_INT32_UNPACK(max_value);
  TEST_INT32_UNPACK(x12345678);
#undef TEST_INT32_UNPACK

#define TEST_INT32_UNPACK_VIA_FORMAT(vname) \
  memcpy (packed_data, vname ## _bytes, 4); \
  TEST_ASSERT (f->base.vfuncs.unpack (f, 4, packed_data, &v, NULL, &error) == 4); \
  TEST_ASSERT(v == vname)
  TEST_INT32_UNPACK_VIA_FORMAT(min_value);
  TEST_INT32_UNPACK_VIA_FORMAT(zero);
  TEST_INT32_UNPACK_VIA_FORMAT(max_value);
  TEST_INT32_UNPACK_VIA_FORMAT(x12345678);
#undef TEST_INT32_UNPACK_VIA_FORMAT

}

static void
test__int64_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "int64", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(f->v_int.is_signed);
  assert(f->v_int.byte_size == 8);
  assert(!f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "int64") == 0);

  int64_t min_value = INT64_MIN, max_value = INT64_MAX, zero = 0, x1234567890abcdef = 0x1234567890abcdef;
  uint8_t min_value_bytes[8] = {0,0,0,0,0,0,0,0x80};
  uint8_t max_value_bytes[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f};
  uint8_t zero_bytes[8] = {0,0,0,0,0,0,0,0};
  uint8_t x1234567890abcdef_bytes[8] = {0xef,0xcd,0xab,0x90,0x78, 0x56, 0x34, 0x12};
  TEST_ASSERT (bulp_int64_get_packed_size (min_value) == 8);
  TEST_ASSERT (bulp_int64_get_packed_size (zero) == 8);
  TEST_ASSERT (bulp_int64_get_packed_size (max_value) == 8);
  TEST_ASSERT (bulp_int64_get_packed_size (x1234567890abcdef) == 8);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &min_value) == 8);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &zero) == 8);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &max_value) == 8);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &x1234567890abcdef) == 8);

  // test pack
  uint8_t out[16];
#define TEST_INT64_PACK(vname) \
  TEST_ASSERT (bulp_int64_pack (vname, out) == 8); \
  TEST_ASSERT (memcmp (vname##_bytes, out, 8) == 0)
  TEST_INT64_PACK(min_value);
  TEST_INT64_PACK(zero);
  TEST_INT64_PACK(max_value);
  TEST_INT64_PACK(x1234567890abcdef);
#undef TEST_INT64_PACK

#define TEST_INT64_PACK_VIA_FORMAT(vname) \
  TEST_ASSERT (f->base.vfuncs.pack (f, &vname, out) == 8); \
  TEST_ASSERT (memcmp (vname##_bytes, out, 8) == 0)
  TEST_INT64_PACK_VIA_FORMAT(min_value);
  TEST_INT64_PACK_VIA_FORMAT(zero);
  TEST_INT64_PACK_VIA_FORMAT(max_value);
  TEST_INT64_PACK_VIA_FORMAT(x1234567890abcdef);
#undef TEST_INT64_PACK_VIA_FORMAT

  // test pack_to
  uint8_t packed_data[8];
  BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 20, 20);

#define TEST_INT64_PACK_TO(vname) \
  BULP_DATA_BUILDER_RESET_ON_STACK (builder); \
  bulp_int64_pack_to (vname, &builder); \
  assert(builder.cur_len == 8); \
  bulp_data_builder_build (&builder, packed_data); \
  TEST_ASSERT (memcmp (vname##_bytes, packed_data, 8) == 0)

  TEST_INT64_PACK_TO(min_value);
  TEST_INT64_PACK_TO(zero);
  TEST_INT64_PACK_TO(max_value);
  TEST_INT64_PACK_TO(x1234567890abcdef);

#undef TEST_INT64_PACK_TO
#define TEST_INT64_PACK_TO_VIA_FORMAT(vname) \
  BULP_DATA_BUILDER_RESET_ON_STACK (builder); \
  f->base.vfuncs.pack_to (f, &vname, &builder); \
  assert(builder.cur_len == 8); \
  bulp_data_builder_build (&builder, packed_data); \
  TEST_ASSERT (memcmp (vname##_bytes, packed_data, 8) == 0)
  TEST_INT64_PACK_TO_VIA_FORMAT(min_value);
  TEST_INT64_PACK_TO_VIA_FORMAT(zero);
  TEST_INT64_PACK_TO_VIA_FORMAT(max_value);
  TEST_INT64_PACK_TO_VIA_FORMAT(x1234567890abcdef);
#undef TEST_INT64_PACK_TO_VIA_FORMAT


  // test unpack
  int64_t v;
  BulpError *error = NULL;

#define TEST_INT64_UNPACK(vname) \
  memcpy (packed_data, vname ## _bytes, 8); \
  TEST_ASSERT (bulp_int64_unpack (8, packed_data, &v, &error) == 8); \
  TEST_ASSERT(v == vname)
  TEST_INT64_UNPACK(min_value);
  TEST_INT64_UNPACK(zero);
  TEST_INT64_UNPACK(max_value);
  TEST_INT64_UNPACK(x1234567890abcdef);
#undef TEST_INT64_UNPACK

#define TEST_INT64_UNPACK_VIA_FORMAT(vname) \
  memcpy (packed_data, vname ## _bytes, 8); \
  TEST_ASSERT (f->base.vfuncs.unpack (f, 8, packed_data, &v, NULL, &error) == 8); \
  TEST_ASSERT(v == vname)
  TEST_INT64_UNPACK_VIA_FORMAT(min_value);
  TEST_INT64_UNPACK_VIA_FORMAT(zero);
  TEST_INT64_UNPACK_VIA_FORMAT(max_value);
  TEST_INT64_UNPACK_VIA_FORMAT(x1234567890abcdef);
#undef TEST_INT64_UNPACK_VIA_FORMAT

}

static void
test__uint8_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "uint8", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(!f->v_int.is_signed);
  assert(f->v_int.byte_size == 1);
  assert(!f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "uint8") == 0);

  uint8_t hundred = 100, max_value = 255, zero = 0;
  TEST_ASSERT (bulp_uint8_get_packed_size (hundred) == 1);
  TEST_ASSERT (bulp_uint8_get_packed_size (zero) == 1);
  TEST_ASSERT (bulp_uint8_get_packed_size (max_value) == 1);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &hundred) == 1);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &zero) == 1);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &max_value) == 1);

  // test pack
  uint8_t out[16];
  TEST_ASSERT (bulp_uint8_pack (hundred, out) == 1);
  TEST_ASSERT (out[0] == 100);
  TEST_ASSERT (bulp_uint8_pack (zero, out) == 1);
  TEST_ASSERT (out[0] == 0);
  TEST_ASSERT (bulp_uint8_pack (max_value, out) == 1);
  TEST_ASSERT (out[0] == 0xff);

  TEST_ASSERT (f->base.vfuncs.pack (f, &hundred, out) == 1);
  TEST_ASSERT (out[0] == 100);
  TEST_ASSERT (f->base.vfuncs.pack (f, &zero, out) == 1);
  TEST_ASSERT (out[0] == 0);
  TEST_ASSERT (f->base.vfuncs.pack (f, &max_value, out) == 1);
  TEST_ASSERT (out[0] == 0xff);

  // test pack_to
  uint8_t packed_data[1];
  BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 2, 2);
  bulp_uint8_pack_to (hundred, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 100);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  bulp_uint8_pack_to (zero, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 0);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  bulp_uint8_pack_to (max_value, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 0xff);

  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &hundred, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 100);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &zero, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 0);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &max_value, &builder);
  assert(builder.cur_len == 1);
  bulp_data_builder_build (&builder, packed_data);
  assert(packed_data[0] == 0xff);
  bulp_data_builder_clear (&builder);           // does nothing, since all allocations are on stack

  // test unpack
  packed_data[0] = 100;
  uint8_t v;
  BulpError *error = NULL;
  TEST_ASSERT (bulp_uint8_unpack (1, packed_data, &v, &error) == 1);
  assert(v == hundred);
  packed_data[0] = 0;
  TEST_ASSERT (bulp_uint8_unpack (1, packed_data, &v, &error) == 1);
  assert(v == 0);
  packed_data[0] = 0xff;
  TEST_ASSERT (bulp_uint8_unpack (1, packed_data, &v, &error) == 1);
  assert(v == max_value);

  packed_data[0] = 100;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 1, packed_data, &v, NULL, &error) == 1);
  assert(v == hundred);
  packed_data[0] = 0;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 1, packed_data, &v, NULL, &error) == 1);
  assert(v == 0);
  packed_data[0] = 0xff;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 1, packed_data, &v, NULL, &error) == 1);
  assert(v == max_value);
}

static void
test__uint16_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "uint16", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(!f->v_int.is_signed);
  assert(f->v_int.byte_size == 2);
  assert(!f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "uint16") == 0);

  uint16_t tenthou = 10000, max_value = 65535, zero = 0, x1234 = 0x1234;
  TEST_ASSERT (bulp_uint16_get_packed_size (tenthou) == 2);
  TEST_ASSERT (bulp_uint16_get_packed_size (zero) == 2);
  TEST_ASSERT (bulp_uint16_get_packed_size (max_value) == 2);
  TEST_ASSERT (bulp_uint16_get_packed_size (x1234) == 2);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &tenthou) == 2);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &zero) == 2);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &max_value) == 2);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &x1234) == 2);

  // test pack
  uint8_t out[16];
  TEST_ASSERT (bulp_uint16_pack (tenthou, out) == 2);
  TEST_ASSERT (out[0] == 10000 % 256);
  TEST_ASSERT (out[1] == 10000 / 256);
  TEST_ASSERT (bulp_uint16_pack (zero, out) == 2);
  TEST_ASSERT (out[0] == 0);
  TEST_ASSERT (out[1] == 0);
  TEST_ASSERT (bulp_uint16_pack (max_value, out) == 2);
  TEST_ASSERT (out[0] == 0xff);
  TEST_ASSERT (out[1] == 0xff);
  TEST_ASSERT (bulp_uint16_pack (x1234, out) == 2);
  TEST_ASSERT (out[0] == 0x34);
  TEST_ASSERT (out[1] == 0x12);

  TEST_ASSERT (f->base.vfuncs.pack (f, &tenthou, out) == 2);
  TEST_ASSERT (out[0] == 10000 % 256);
  TEST_ASSERT (out[1] == 10000 / 256);
  TEST_ASSERT (f->base.vfuncs.pack (f, &zero, out) == 2);
  TEST_ASSERT (out[0] == 0);
  TEST_ASSERT (out[1] == 0);
  TEST_ASSERT (f->base.vfuncs.pack (f, &max_value, out) == 2);
  TEST_ASSERT (out[0] == 0xff);
  TEST_ASSERT (out[1] == 0xff);
  TEST_ASSERT (f->base.vfuncs.pack (f, &x1234, out) == 2);
  TEST_ASSERT (out[0] == 0x34);
  TEST_ASSERT (out[1] == 0x12);

  // test pack_to
  uint8_t packed_data[2];
  BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 2, 2);
  bulp_uint16_pack_to (tenthou, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 10000 % 256);
  TEST_ASSERT (packed_data[1] == 10000 / 256);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  bulp_uint16_pack_to (zero, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0x00);
  TEST_ASSERT (packed_data[1] == 0x00);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  bulp_uint16_pack_to (max_value, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0xff);
  TEST_ASSERT (packed_data[1] == 0xff);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  bulp_uint16_pack_to (x1234, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0x34);
  TEST_ASSERT (packed_data[1] == 0x12);

  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &tenthou, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 10000 % 256);
  TEST_ASSERT (packed_data[1] == 10000 / 256);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &zero, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0x00);
  TEST_ASSERT (packed_data[1] == 0x00);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &max_value, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0xff);
  TEST_ASSERT (packed_data[1] == 0xff);
  BULP_DATA_BUILDER_RESET_ON_STACK (builder);
  f->base.vfuncs.pack_to (f, &x1234, &builder);
  assert(builder.cur_len == 2);
  bulp_data_builder_build (&builder, packed_data);
  TEST_ASSERT (packed_data[0] == 0x34);
  TEST_ASSERT (packed_data[1] == 0x12);
  bulp_data_builder_clear (&builder);           // does nothing, since all allocations are on stack

  // test unpack
  uint16_t v;
  BulpError *error = NULL;
  packed_data[0] = 10000 % 256;
  packed_data[1] = 10000 / 256;
  TEST_ASSERT (bulp_uint16_unpack (2, packed_data, &v, &error) == 2);
  assert(v == tenthou);
  packed_data[0] = 0;
  packed_data[1] = 0;
  TEST_ASSERT (bulp_uint16_unpack (2, packed_data, &v, &error) == 2);
  assert(v == 0);
  packed_data[0] = 0xff;
  packed_data[1] = 0xff;
  TEST_ASSERT (bulp_uint16_unpack (2, packed_data, &v, &error) == 2);
  assert(v == max_value);
  packed_data[0] = 0x34;
  packed_data[1] = 0x12;
  TEST_ASSERT (bulp_uint16_unpack (2, packed_data, &v, &error) == 2);
  assert(v == x1234);

  packed_data[0] = 10000 % 256;
  packed_data[1] = 10000 / 256;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 2, packed_data, &v, NULL, &error) == 2);
  assert(v == tenthou);
  packed_data[0] = 0;
  packed_data[1] = 0;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 2, packed_data, &v, NULL, &error) == 2);
  assert(v == 0);
  packed_data[0] = 0xff;
  packed_data[1] = 0xff;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 2, packed_data, &v, NULL, &error) == 2);
  assert(v == max_value);
  packed_data[0] = 0x34;
  packed_data[1] = 0x12;
  TEST_ASSERT (f->base.vfuncs.unpack (f, 2, packed_data, &v, NULL, &error) == 2);
  assert(v == x1234);
}

static void
test__uint32_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "uint32", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(!f->v_int.is_signed);
  assert(f->v_int.byte_size == 4);
  assert(!f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "uint32") == 0);

  uint32_t max_value = UINT32_MAX, zero = 0, x12345678 = 0x12345678;
  uint8_t max_value_bytes[4] = {0xff,0xff,0xff,0xff};
  uint8_t zero_bytes[4] = {0,0,0,0};
  uint8_t x12345678_bytes[4] = {0x78, 0x56, 0x34, 0x12};
  TEST_ASSERT (bulp_uint32_get_packed_size (zero) == 4);
  TEST_ASSERT (bulp_uint32_get_packed_size (max_value) == 4);
  TEST_ASSERT (bulp_uint32_get_packed_size (x12345678) == 4);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &zero) == 4);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &max_value) == 4);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &x12345678) == 4);

  // test pack
  uint8_t out[16];
#define TEST_UINT32_PACK(vname) \
  TEST_ASSERT (bulp_uint32_pack (vname, out) == 4); \
  TEST_ASSERT (memcmp (vname##_bytes, out, 4) == 0)
  TEST_UINT32_PACK(zero);
  TEST_UINT32_PACK(max_value);
  TEST_UINT32_PACK(x12345678);
#undef TEST_UINT32_PACK

#define TEST_UINT32_PACK_VIA_FORMAT(vname) \
  TEST_ASSERT (f->base.vfuncs.pack (f, &vname, out) == 4); \
  TEST_ASSERT (memcmp (vname##_bytes, out, 4) == 0)
  TEST_UINT32_PACK_VIA_FORMAT(zero);
  TEST_UINT32_PACK_VIA_FORMAT(max_value);
  TEST_UINT32_PACK_VIA_FORMAT(x12345678);
#undef TEST_UINT32_PACK_VIA_FORMAT

  // test pack_to
  uint8_t packed_data[4];
  BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 20, 20);

#define TEST_UINT32_PACK_TO(vname) \
  BULP_DATA_BUILDER_RESET_ON_STACK (builder); \
  bulp_uint32_pack_to (vname, &builder); \
  assert(builder.cur_len == 4); \
  bulp_data_builder_build (&builder, packed_data); \
  TEST_ASSERT (memcmp (vname##_bytes, packed_data, 4) == 0)

  TEST_UINT32_PACK_TO(zero);
  TEST_UINT32_PACK_TO(max_value);
  TEST_UINT32_PACK_TO(x12345678);

#undef TEST_UINT32_PACK_TO
#define TEST_UINT32_PACK_TO_VIA_FORMAT(vname) \
  BULP_DATA_BUILDER_RESET_ON_STACK (builder); \
  f->base.vfuncs.pack_to (f, &vname, &builder); \
  assert(builder.cur_len == 4); \
  bulp_data_builder_build (&builder, packed_data); \
  TEST_ASSERT (memcmp (vname##_bytes, packed_data, 4) == 0)
  TEST_UINT32_PACK_TO_VIA_FORMAT(zero);
  TEST_UINT32_PACK_TO_VIA_FORMAT(max_value);
  TEST_UINT32_PACK_TO_VIA_FORMAT(x12345678);
#undef TEST_UINT32_PACK_TO_VIA_FORMAT


  // test unpack
  uint32_t v;
  BulpError *error = NULL;

#define TEST_UINT32_UNPACK(vname) \
  memcpy (packed_data, vname ## _bytes, 4); \
  TEST_ASSERT (bulp_uint32_unpack (4, packed_data, &v, &error) == 4); \
  TEST_ASSERT(v == vname)
  TEST_UINT32_UNPACK(zero);
  TEST_UINT32_UNPACK(max_value);
  TEST_UINT32_UNPACK(x12345678);
#undef TEST_UINT32_UNPACK

#define TEST_UINT32_UNPACK_VIA_FORMAT(vname) \
  memcpy (packed_data, vname ## _bytes, 4); \
  TEST_ASSERT (f->base.vfuncs.unpack (f, 4, packed_data, &v, NULL, &error) == 4); \
  TEST_ASSERT(v == vname)
  TEST_UINT32_UNPACK_VIA_FORMAT(zero);
  TEST_UINT32_UNPACK_VIA_FORMAT(max_value);
  TEST_UINT32_UNPACK_VIA_FORMAT(x12345678);
#undef TEST_UINT32_UNPACK_VIA_FORMAT

}

static void
test__uint64_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "uint64", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(!f->v_int.is_signed);
  assert(f->v_int.byte_size == 8);
  assert(!f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "uint64") == 0);

  uint64_t max_value = UINT64_MAX, zero = 0, x1234567890abcdef = 0x1234567890abcdef;
  uint8_t max_value_bytes[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
  uint8_t zero_bytes[8] = {0,0,0,0,0,0,0,0};
  uint8_t x1234567890abcdef_bytes[8] = {0xef,0xcd,0xab,0x90,0x78, 0x56, 0x34, 0x12};
  TEST_ASSERT (bulp_uint64_get_packed_size (zero) == 8);
  TEST_ASSERT (bulp_uint64_get_packed_size (max_value) == 8);
  TEST_ASSERT (bulp_uint64_get_packed_size (x1234567890abcdef) == 8);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &zero) == 8);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &max_value) == 8);
  TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &x1234567890abcdef) == 8);

  // test pack
  uint8_t out[16];
#define TEST_UINT64_PACK(vname) \
  TEST_ASSERT (bulp_uint64_pack (vname, out) == 8); \
  TEST_ASSERT (memcmp (vname##_bytes, out, 8) == 0)
  TEST_UINT64_PACK(zero);
  TEST_UINT64_PACK(max_value);
  TEST_UINT64_PACK(x1234567890abcdef);
#undef TEST_UINT64_PACK

#define TEST_UINT64_PACK_VIA_FORMAT(vname) \
  TEST_ASSERT (f->base.vfuncs.pack (f, &vname, out) == 8); \
  TEST_ASSERT (memcmp (vname##_bytes, out, 8) == 0)
  TEST_UINT64_PACK_VIA_FORMAT(zero);
  TEST_UINT64_PACK_VIA_FORMAT(max_value);
  TEST_UINT64_PACK_VIA_FORMAT(x1234567890abcdef);
#undef TEST_UINT64_PACK_VIA_FORMAT

  // test pack_to
  uint8_t packed_data[8];
  BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 20, 20);

#define TEST_UINT64_PACK_TO(vname) \
  BULP_DATA_BUILDER_RESET_ON_STACK (builder); \
  bulp_uint64_pack_to (vname, &builder); \
  assert(builder.cur_len == 8); \
  bulp_data_builder_build (&builder, packed_data); \
  TEST_ASSERT (memcmp (vname##_bytes, packed_data, 8) == 0)

  TEST_UINT64_PACK_TO(zero);
  TEST_UINT64_PACK_TO(max_value);
  TEST_UINT64_PACK_TO(x1234567890abcdef);

#undef TEST_UINT64_PACK_TO
#define TEST_UINT64_PACK_TO_VIA_FORMAT(vname) \
  BULP_DATA_BUILDER_RESET_ON_STACK (builder); \
  f->base.vfuncs.pack_to (f, &vname, &builder); \
  assert(builder.cur_len == 8); \
  bulp_data_builder_build (&builder, packed_data); \
  TEST_ASSERT (memcmp (vname##_bytes, packed_data, 8) == 0)
  TEST_UINT64_PACK_TO_VIA_FORMAT(zero);
  TEST_UINT64_PACK_TO_VIA_FORMAT(max_value);
  TEST_UINT64_PACK_TO_VIA_FORMAT(x1234567890abcdef);
#undef TEST_UINT64_PACK_TO_VIA_FORMAT


  // test unpack
  uint64_t v;
  BulpError *error = NULL;

#define TEST_UINT64_UNPACK(vname) \
  memcpy (packed_data, vname ## _bytes, 8); \
  TEST_ASSERT (bulp_uint64_unpack (8, packed_data, &v, &error) == 8); \
  TEST_ASSERT(v == vname)
  TEST_UINT64_UNPACK(zero);
  TEST_UINT64_UNPACK(max_value);
  TEST_UINT64_UNPACK(x1234567890abcdef);
#undef TEST_UINT64_UNPACK

#define TEST_UINT64_UNPACK_VIA_FORMAT(vname) \
  memcpy (packed_data, vname ## _bytes, 8); \
  TEST_ASSERT (f->base.vfuncs.unpack (f, 8, packed_data, &v, NULL, &error) == 8); \
  TEST_ASSERT(v == vname)
  TEST_UINT64_UNPACK_VIA_FORMAT(zero);
  TEST_UINT64_UNPACK_VIA_FORMAT(max_value);
  TEST_UINT64_UNPACK_VIA_FORMAT(x1234567890abcdef);
#undef TEST_UINT64_UNPACK_VIA_FORMAT

}

static void
test__short_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "short", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpError *error = NULL;
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(f->v_int.is_signed);
  assert(f->v_int.byte_size == 2);
  assert(f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "short") == 0);

  // test zigzagging
  TEST_ASSERT(bulp_zigzag16(0) == 0);
  TEST_ASSERT(bulp_zigzag16(-1) == 1);
  TEST_ASSERT(bulp_zigzag16(1) == 2);
  TEST_ASSERT(bulp_zigzag16(-32768) == 65535);
  TEST_ASSERT(bulp_unzigzag16(0) == 0);
  TEST_ASSERT(bulp_unzigzag16(1) == -1);
  TEST_ASSERT(bulp_unzigzag16(2) == 1);
  for (int i = -32768; i < 32768; i++)
    TEST_ASSERT(bulp_unzigzag16(bulp_zigzag16(i)) == i);

  static uint8_t bytes_0[] = {0 };
  static uint8_t bytes_m1[] = {0x1};
  static uint8_t bytes_1[] = {0x2};
  static uint8_t bytes_16384[] = {0x80, 0x80, 2 };
  static uint8_t bytes_m32768[] = {0xff, 0xff, 3 };
  static uint8_t bytes_32767[] = {0xfe, 0xff, 3 };
  static struct {
    int16_t value;
    size_t expected_len;
    const uint8_t *expected;
  } test_vectors[] = {
#define EMIT_TEST_VECTOR(n)    { n, sizeof(bytes_##n), bytes_##n }
#define EMIT_TEST_VECTOR_NEGATIVE(n)    { -n, sizeof(bytes_m##n), bytes_m##n }
    EMIT_TEST_VECTOR(0),
    EMIT_TEST_VECTOR_NEGATIVE(1),
    EMIT_TEST_VECTOR(1),
    EMIT_TEST_VECTOR(16384),
    EMIT_TEST_VECTOR_NEGATIVE(32768),
    EMIT_TEST_VECTOR(32767),
#undef EMIT_TEST_VECTOR
#undef EMIT_TEST_VECTOR_NEGATIVE
  };

  uint8_t out[30];
  for (unsigned i = 0; i < sizeof(test_vectors)/sizeof(test_vectors[0]); i++)
    {
      //printf("test %u: value=%d, %04x\n", i, test_vectors[i].value, (uint16_t) test_vectors[i].value);
      TEST_ASSERT (bulp_short_get_packed_size (test_vectors[i].value) == test_vectors[i].expected_len);
      TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &test_vectors[i].value) == test_vectors[i].expected_len);

      TEST_ASSERT (bulp_short_pack (test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      memset (out, 0xff, sizeof(out));
      TEST_ASSERT (f->base.vfuncs.pack (f, &test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 20, 20);
      bulp_short_pack_to (test_vectors[i].value, &builder);
      TEST_ASSERT(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_RESET_ON_STACK (builder);
      f->base.vfuncs.pack_to (f, &test_vectors[i].value, &builder);
      assert(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      bulp_data_builder_clear (&builder);           // does nothing, since all allocations are on stack

      int16_t v;
      memcpy (out, test_vectors[i].expected, test_vectors[i].expected_len);
      TEST_ASSERT (bulp_short_unpack (30, out, &v, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);

      v = 13453;   // set it to garbage to ensure it is set
      TEST_ASSERT (f->base.vfuncs.unpack (f, 8, out, &v, NULL, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);
    }
}

static void
test__int_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "int", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpError *error = NULL;
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(f->v_int.is_signed);
  assert(f->v_int.byte_size == 4);
  assert(f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "int") == 0);

  TEST_ASSERT(bulp_zigzag32(0) == 0);
  TEST_ASSERT(bulp_zigzag32(-1) == 1);
  TEST_ASSERT(bulp_zigzag32(1) == 2);
  TEST_ASSERT(bulp_zigzag32(-32768) == 65535);
  TEST_ASSERT(bulp_unzigzag32(0) == 0);
  TEST_ASSERT(bulp_unzigzag32(1) == -1);
  TEST_ASSERT(bulp_unzigzag32(2) == 1);
  TEST_ASSERT(bulp_unzigzag32(bulp_zigzag32(123456789)) == 123456789);
  TEST_ASSERT(bulp_unzigzag32(bulp_zigzag32(-123456789)) == -123456789);

  static uint8_t bytes_0[] = {0 };
  static uint8_t bytes_m1[] = {0x1};
  static uint8_t bytes_1[] = {0x2};
  static uint8_t bytes_16384[] = {0x80, 0x80, 2 };
  static uint8_t bytes_m32768[] = {0xff, 0xff, 3 };
  static uint8_t bytes_32767[] = {0xfe, 0xff, 3 };
  static uint8_t bytes_1048576[] = {0x80, 0x80, 0x80, 0x01};            // 1<<20, zz=1<<21
  static uint8_t bytes_m1048576[] = {0xff, 0xff, 0x7f };            // 1<<20, zz=1<<21-1
  static uint8_t bytes_1073741824[] = {0x80, 0x80, 0x80, 0x80, 8};     // 1<<30, zz=1<<31
  static uint8_t bytes_m1073741824[] = {0xff, 0xff, 0xff, 0xff, 7};

  static struct {
    int32_t value;
    size_t expected_len;
    const uint8_t *expected;
  } test_vectors[] = {
#define EMIT_TEST_VECTOR(n)    { INT32_C(n), sizeof(bytes_##n), bytes_##n }
#define EMIT_TEST_VECTOR_NEGATIVE(n)    { INT32_C(-n), sizeof(bytes_m##n), bytes_m##n }
    EMIT_TEST_VECTOR(0),
    EMIT_TEST_VECTOR_NEGATIVE(1),
    EMIT_TEST_VECTOR(1),
    EMIT_TEST_VECTOR(16384),
    EMIT_TEST_VECTOR_NEGATIVE(32768),
    EMIT_TEST_VECTOR(32767),
    EMIT_TEST_VECTOR(1048576),
    EMIT_TEST_VECTOR_NEGATIVE(1048576),
    EMIT_TEST_VECTOR(1073741824),
    EMIT_TEST_VECTOR_NEGATIVE(1073741824),
#undef EMIT_TEST_VECTOR
  };

  uint8_t out[30];
  for (unsigned i = 0; i < sizeof(test_vectors)/sizeof(test_vectors[0]); i++)
    {
      //fprintf(stderr, "test case %u: value=%d [0x%x]\n", i, test_vectors[i].value, test_vectors[i].value);
      TEST_ASSERT (bulp_int_get_packed_size (test_vectors[i].value) == test_vectors[i].expected_len);
      TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &test_vectors[i].value) == test_vectors[i].expected_len);

      TEST_ASSERT (bulp_int_pack (test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      memset (out, 0xff, sizeof(out));
      TEST_ASSERT (f->base.vfuncs.pack (f, &test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 20, 20);
      bulp_int_pack_to (test_vectors[i].value, &builder);
      TEST_ASSERT(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_RESET_ON_STACK (builder);
      f->base.vfuncs.pack_to (f, &test_vectors[i].value, &builder);
      assert(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      bulp_data_builder_clear (&builder);           // does nothing, since all allocations are on stack

      int32_t v;
      memcpy (out, test_vectors[i].expected, test_vectors[i].expected_len);
      TEST_ASSERT (bulp_int_unpack (30, out, &v, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);

      v = 43453;   // set it to garbage to ensure it is set
      TEST_ASSERT (f->base.vfuncs.unpack (f, 30, out, &v, NULL, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);
    }
}

static void
test__long_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "long", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpError *error = NULL;
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(f->v_int.is_signed);
  assert(f->v_int.byte_size == 8);
  assert(f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "long") == 0);

  static uint8_t bytes_0[] = {0 };
  static uint8_t bytes_m1[] = {0x1};
  static uint8_t bytes_1[] = {0x2};
  static uint8_t bytes_16384[] = {0x80, 0x80, 2 };
  static uint8_t bytes_m32768[] = {0xff, 0xff, 3 };
  static uint8_t bytes_32767[] = {0xfe, 0xff, 3 };
  static uint8_t bytes_1048576[] = {0x80, 0x80, 0x80, 0x01};            // 1<<20, zz=1<<21
  static uint8_t bytes_m1048576[] = {0xff, 0xff, 0x7f };            // 1<<20, zz=1<<21-1
  static uint8_t bytes_1073741824[] = {0x80, 0x80, 0x80, 0x80, 8};     // 1<<30, zz=1<<31
  static uint8_t bytes_m1073741824[] = {0xff, 0xff, 0xff, 0xff, 7};
  static uint8_t bytes_1099511627776[] = {0x80,0x80,0x80,0x80,0x80,0x40};   // 2^40, zz=1<<41
  static uint8_t bytes_m1099511627776[] = {0xff,0xff,0xff,0xff,0xff,0x3f};  // -2^40, zz=1<<41-1
  static uint8_t bytes_1125899906842624[] =  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x04};   // 2^50, zz=1<<51
  static uint8_t bytes_m1125899906842624[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x03};  // -2^50, zz=1<<51-1
  static uint8_t bytes_1152921504606846976[] =  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x20};  //2^60; zz=2^61
  static uint8_t bytes_m1152921504606846976[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1f};  //-2^60; zz=2^61-1



  static struct {
    int64_t value;
    size_t expected_len;
    const uint8_t *expected;
  } test_vectors[] = {
#define EMIT_TEST_VECTOR(n)    { INT32_C(n), sizeof(bytes_##n), bytes_##n }
#define EMIT_TEST_VECTOR_NEGATIVE(n)    { INT32_C(-n), sizeof(bytes_m##n), bytes_m##n }
    EMIT_TEST_VECTOR(0),
    EMIT_TEST_VECTOR_NEGATIVE(1),
    EMIT_TEST_VECTOR(1),
    EMIT_TEST_VECTOR(16384),
    EMIT_TEST_VECTOR_NEGATIVE(32768),
    EMIT_TEST_VECTOR(32767),
    EMIT_TEST_VECTOR(1048576),
    EMIT_TEST_VECTOR_NEGATIVE(1048576),
    EMIT_TEST_VECTOR(1073741824),
    EMIT_TEST_VECTOR_NEGATIVE(1073741824),
    EMIT_TEST_VECTOR(1099511627776),
    EMIT_TEST_VECTOR_NEGATIVE(1099511627776),
    EMIT_TEST_VECTOR(1125899906842624),
    EMIT_TEST_VECTOR_NEGATIVE(1125899906842624),
    EMIT_TEST_VECTOR(1152921504606846976),
    EMIT_TEST_VECTOR_NEGATIVE(1152921504606846976),
#undef EMIT_TEST_VECTOR
#undef EMIT_TEST_VECTOR_NEGATIVE
  };

  uint8_t out[30];
  for (unsigned i = 0; i < sizeof(test_vectors)/sizeof(test_vectors[0]); i++)
    {
      //fprintf(stderr, "test case %u: value=%lld [0x%llx]\n", i, test_vectors[i].value, test_vectors[i].value);
      TEST_ASSERT (bulp_long_get_packed_size (test_vectors[i].value) == test_vectors[i].expected_len);
      TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &test_vectors[i].value) == test_vectors[i].expected_len);

      TEST_ASSERT (bulp_long_pack (test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      memset (out, 0xff, sizeof(out));
      TEST_ASSERT (f->base.vfuncs.pack (f, &test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 20, 20);
      bulp_long_pack_to (test_vectors[i].value, &builder);
      TEST_ASSERT(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_RESET_ON_STACK (builder);
      f->base.vfuncs.pack_to (f, &test_vectors[i].value, &builder);
      assert(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      bulp_data_builder_clear (&builder);           // does nothing, since all allocations are on stack

      int64_t v;
      memcpy (out, test_vectors[i].expected, test_vectors[i].expected_len);
      TEST_ASSERT (bulp_long_unpack (30, out, &v, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);

      v = 43453;   // set it to garbage to ensure it is set
      TEST_ASSERT (f->base.vfuncs.unpack (f, 30, out, &v, NULL, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);
    }
}

static void
test__ushort_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "ushort", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpError *error = NULL;
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(!f->v_int.is_signed);
  assert(f->v_int.byte_size == 2);
  assert(f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "ushort") == 0);

  static uint8_t bytes_0[] = {0 };
  static uint8_t bytes_127[] = {127 };
  static uint8_t bytes_128[] = {0x80, 1 };
  static uint8_t bytes_16383[] = {0xff, 0x7f };
  static uint8_t bytes_16384[] = {0x80, 0x80, 1 };
  static uint8_t bytes_65535[] = {0xff, 0xff, 3 };

  static struct {
    uint16_t value;
    size_t expected_len;
    const uint8_t *expected;
  } test_vectors[] = {
#define EMIT_TEST_VECTOR(n)    { n, sizeof(bytes_##n), bytes_##n }
    EMIT_TEST_VECTOR(0),
    EMIT_TEST_VECTOR(127),
    EMIT_TEST_VECTOR(128),
    EMIT_TEST_VECTOR(16383),
    EMIT_TEST_VECTOR(16384),
    EMIT_TEST_VECTOR(65535),
#undef EMIT_TEST_VECTOR
  };

  uint8_t out[30];
  for (unsigned i = 0; i < sizeof(test_vectors)/sizeof(test_vectors[0]); i++)
    {
      TEST_ASSERT (bulp_ushort_get_packed_size (test_vectors[i].value) == test_vectors[i].expected_len);
      TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &test_vectors[i].value) == test_vectors[i].expected_len);

      TEST_ASSERT (bulp_ushort_pack (test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      memset (out, 0xff, sizeof(out));
      TEST_ASSERT (f->base.vfuncs.pack (f, &test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 20, 20);
      bulp_ushort_pack_to (test_vectors[i].value, &builder);
      TEST_ASSERT(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_RESET_ON_STACK (builder);
      f->base.vfuncs.pack_to (f, &test_vectors[i].value, &builder);
      assert(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      bulp_data_builder_clear (&builder);           // does nothing, since all allocations are on stack

      uint16_t v;
      memcpy (out, test_vectors[i].expected, test_vectors[i].expected_len);
      TEST_ASSERT (bulp_ushort_unpack (30, out, &v, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);

      v = 43453;   // set it to garbage to ensure it is set
      TEST_ASSERT (f->base.vfuncs.unpack (f, 8, out, &v, NULL, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);
    }
}

static void
test__uint_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "uint", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpError *error = NULL;
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(!f->v_int.is_signed);
  assert(f->v_int.byte_size == 4);
  assert(f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "uint") == 0);

  static uint8_t bytes_0[] = {0 };
  static uint8_t bytes_127[] = {127 };
  static uint8_t bytes_128[] = {0x80, 1 };
  static uint8_t bytes_1234[] = {0x80 | 82, 9 };
  static uint8_t bytes_16383[] = {0xff, 0x7f };
  static uint8_t bytes_16384[] = {0x80, 0x80, 1 };
  static uint8_t bytes_65535[] = {0xff, 0xff, 3 };
  static uint8_t bytes_123456[] = {64|0x80, 68|0x80, 7};
  static uint8_t bytes_12345679[] = {79|0x80, 66|0x80, 113|0x80, 5};
  static uint8_t bytes_268435455[] = {0xff, 0xff, 0xff, 0x7f};
  static uint8_t bytes_268435456[] = {0x80, 0x80, 0x80, 0x80, 0x01};
  static uint8_t bytes_123456791[] = { 23|0x80, 26|0x80, 111|0x80, 58 };
  static uint8_t bytes_4294967295[] = {0xff, 0xff, 0xff, 0xff, 0x0f};

  static struct {
    uint32_t value;
    size_t expected_len;
    const uint8_t *expected;
  } test_vectors[] = {
#define EMIT_TEST_VECTOR(n)    { UINT32_C(n), sizeof(bytes_##n), bytes_##n }
    EMIT_TEST_VECTOR(0),
    EMIT_TEST_VECTOR(127),
    EMIT_TEST_VECTOR(128),
    EMIT_TEST_VECTOR(1234),
    EMIT_TEST_VECTOR(16383),
    EMIT_TEST_VECTOR(16384),
    EMIT_TEST_VECTOR(65535),
    EMIT_TEST_VECTOR(123456),
    EMIT_TEST_VECTOR(12345679),
    EMIT_TEST_VECTOR(268435455),
    EMIT_TEST_VECTOR(268435456),
    EMIT_TEST_VECTOR(123456791),
    EMIT_TEST_VECTOR(4294967295),
#undef EMIT_TEST_VECTOR
  };

  uint8_t out[30];
  for (unsigned i = 0; i < sizeof(test_vectors)/sizeof(test_vectors[0]); i++)
    {
      //fprintf(stderr, "test case %u: value=%u [0x%x]\n", i, test_vectors[i].value, test_vectors[i].value);
      TEST_ASSERT (bulp_uint_get_packed_size (test_vectors[i].value) == test_vectors[i].expected_len);
      TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &test_vectors[i].value) == test_vectors[i].expected_len);

      TEST_ASSERT (bulp_uint_pack (test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      memset (out, 0xff, sizeof(out));
      TEST_ASSERT (f->base.vfuncs.pack (f, &test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 20, 20);
      bulp_uint_pack_to (test_vectors[i].value, &builder);
      TEST_ASSERT(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_RESET_ON_STACK (builder);
      f->base.vfuncs.pack_to (f, &test_vectors[i].value, &builder);
      assert(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      bulp_data_builder_clear (&builder);           // does nothing, since all allocations are on stack

      uint32_t v;
      memcpy (out, test_vectors[i].expected, test_vectors[i].expected_len);
      TEST_ASSERT (bulp_uint_unpack (30, out, &v, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);

      v = 43453;   // set it to garbage to ensure it is set
      TEST_ASSERT (f->base.vfuncs.unpack (f, 8, out, &v, NULL, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);
    }
}


static void
test__ulong_format (void)
{
  BulpNamespace *ns = bulp_namespace_new_global ();
  BulpNamespaceEntry res;
  TEST_ASSERT (bulp_namespace_query_1 (ns, "ulong", &res));
  TEST_ASSERT (res.type == BULP_NAMESPACE_ENTRY_FORMAT);
  BulpError *error = NULL;
  BulpFormat *f = res.info.v_format;
  assert(f->type == BULP_FORMAT_TYPE_INT);
  assert(!f->v_int.is_signed);
  assert(f->v_int.byte_size == 8);
  assert(f->v_int.is_b128);
  assert(strcmp (f->base.canonical_name, "ulong") == 0);

  static uint8_t bytes_0[] = {0 };
  static uint8_t bytes_127[] = {127 };
  static uint8_t bytes_128[] = {0x80, 1 };
  static uint8_t bytes_1234[] = {0x80 | 82, 9 };
  static uint8_t bytes_16383[] = {0xff, 0x7f };
  static uint8_t bytes_16384[] = {0x80, 0x80, 1 };
  static uint8_t bytes_65535[] = {0xff, 0xff, 3 };
  static uint8_t bytes_123456[] = {64|0x80, 68|0x80, 7};
  static uint8_t bytes_12345679[] = {79|0x80, 66|0x80, 113|0x80, 5};
  static uint8_t bytes_268435455[] = {0xff, 0xff, 0xff, 0x7f};
  static uint8_t bytes_268435456[] = {0x80, 0x80, 0x80, 0x80, 0x01};
  static uint8_t bytes_123456791[] = { 23|0x80, 26|0x80, 111|0x80, 58 };
  static uint8_t bytes_4294967295[] = {0xff, 0xff, 0xff, 0xff, 0x0f};
  static uint8_t bytes_4294967296[] = {0x80, 0x80, 0x80, 0x80, 0x10};
  static uint8_t bytes_34359738367[] = {0xff, 0xff, 0xff, 0xff, 0x7f};
  static uint8_t bytes_34359738368[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x01};
  static uint8_t bytes_4398046511103[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};
  static uint8_t bytes_4398046511104[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01};
  static uint8_t bytes_562949953421311[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};
  static uint8_t bytes_562949953421312[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01};
  static uint8_t bytes_72057594037927935[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};
  static uint8_t bytes_72057594037927936[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01};
  static uint8_t bytes_9223372036854775807[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};
  static uint8_t bytes_9223372036854775808[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};

  static struct {
    uint64_t value;
    size_t expected_len;
    const uint8_t *expected;
  } test_vectors[] = {
#define EMIT_TEST_VECTOR(n)    { UINT64_C(n), sizeof(bytes_##n), bytes_##n }
    EMIT_TEST_VECTOR(0),
    EMIT_TEST_VECTOR(127),
    EMIT_TEST_VECTOR(128),
    EMIT_TEST_VECTOR(1234),
    EMIT_TEST_VECTOR(16383),
    EMIT_TEST_VECTOR(16384),
    EMIT_TEST_VECTOR(65535),
    EMIT_TEST_VECTOR(123456),
    EMIT_TEST_VECTOR(12345679),
    EMIT_TEST_VECTOR(268435455),
    EMIT_TEST_VECTOR(268435456),
    EMIT_TEST_VECTOR(123456791),
    EMIT_TEST_VECTOR(4294967295),
    EMIT_TEST_VECTOR(4294967296),
    EMIT_TEST_VECTOR(34359738367),
    EMIT_TEST_VECTOR(34359738368),
    EMIT_TEST_VECTOR(4398046511103),
    EMIT_TEST_VECTOR(4398046511104),
    EMIT_TEST_VECTOR(562949953421311),
    EMIT_TEST_VECTOR(562949953421312),
    EMIT_TEST_VECTOR(72057594037927935),
    EMIT_TEST_VECTOR(72057594037927936),
    EMIT_TEST_VECTOR(9223372036854775807),  // 1<<63 - 1
    EMIT_TEST_VECTOR(9223372036854775808),  // 1<<63
#undef EMIT_TEST_VECTOR
  };

  uint8_t out[30];
  for (unsigned i = 0; i < sizeof(test_vectors)/sizeof(test_vectors[0]); i++)
    {
      //fprintf(stderr, "test case %u: value=%llu [0x%llx]\n", i, test_vectors[i].value, test_vectors[i].value);
      TEST_ASSERT (bulp_ulong_get_packed_size (test_vectors[i].value) == test_vectors[i].expected_len);
      TEST_ASSERT (f->base.vfuncs.get_packed_size (f, &test_vectors[i].value) == test_vectors[i].expected_len);

      TEST_ASSERT (bulp_ulong_pack (test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      memset (out, 0xff, sizeof(out));
      TEST_ASSERT (f->base.vfuncs.pack (f, &test_vectors[i].value, out) == test_vectors[i].expected_len);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_DECLARE_ON_STACK(builder, 20, 20);
      bulp_ulong_pack_to (test_vectors[i].value, &builder);
      TEST_ASSERT(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      BULP_DATA_BUILDER_RESET_ON_STACK (builder);
      f->base.vfuncs.pack_to (f, &test_vectors[i].value, &builder);
      assert(builder.cur_len == test_vectors[i].expected_len);
      memset (out, 0xff, sizeof(out));
      bulp_data_builder_build (&builder, out);
      TEST_ASSERT (memcmp (test_vectors[i].expected, out, test_vectors[i].expected_len) == 0);

      bulp_data_builder_clear (&builder);           // does nothing, since all allocations are on stack

      uint64_t v;
      memcpy (out, test_vectors[i].expected, test_vectors[i].expected_len);
      TEST_ASSERT (bulp_ulong_unpack (30, out, &v, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);

      v = 43453;   // set it to garbage to ensure it is set
      TEST_ASSERT (f->base.vfuncs.unpack (f, 30, out, &v, NULL, &error) == test_vectors[i].expected_len);
      TEST_ASSERT (v == test_vectors[i].value);
    }
}


static struct {
  const char *name;
  void (*test)(void);
} tests[] = {
  { "int8 test",   test__int8_format },
  { "int16 test",  test__int16_format },
  { "int32 test",  test__int32_format },
  { "int64 test",  test__int64_format },
  { "uint8 test",  test__uint8_format },
  { "uint16 test", test__uint16_format },
  { "uint32 test", test__uint32_format },
  { "uint64 test", test__uint64_format },
  { "short test",  test__short_format },
  { "int test",    test__int_format },
  { "long test",   test__long_format },
  { "ushort test", test__ushort_format },
  { "uint test",   test__uint_format },
  { "ulong test",  test__ulong_format },
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
