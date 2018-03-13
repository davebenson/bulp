#include "../bulp.h"
#include <stdio.h>

#define DEFINE_BINARY_DATA_STR(name, str) \
const char name##__data[] = str; \
static BulpBinaryData name = { sizeof(name##__data) - 1, (uint8_t *) name##__data }
#define DEFINE_BINARY_DATA(name, braced_hex) \
const uint8_t name##__data[] = braced_hex; \
static BulpBinaryData name = { sizeof(name##__data), (const uint8_t *) str }
#define DEFINE_BINARY_DATA_FROM_STATIC_CHAR_ARRAY(name, chararray) \
static BulpBinaryData name = { sizeof(chararray) - 1, (uint8_t *) chararray }

#define A32      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
#define A256     A32 A32 A32 A32 A32 A32 A32 A32 
#define A1024    A256 A256 A256 A256
#define A4096    A1024 A1024 A1024 A1024

DEFINE_BINARY_DATA_STR(hello_world, "Hello World!");
DEFINE_BINARY_DATA_STR(a1024, A1024);
DEFINE_BINARY_DATA_STR(a4096, A4096);

const char moby_dick_str[] =
#include "data/moby-dick.str"
;
DEFINE_BINARY_DATA_FROM_STATIC_CHAR_ARRAY(moby_dick, moby_dick_str);


static struct {
  const char *name;
  BulpBinaryData *data;
} test_vectors[] = {
#define T(name)       { #name, &name }
  T(hello_world),
  T(a1024),
  T(a4096),
  T(moby_dick)
};

static struct {
  const char *name;
  BulpCompressionOptions comp_opts;
} compression_options[] = {
  { "uncompressed", BULP_COMPRESSION_OPTIONS_INIT_UNCOMPRESSED(256) },
  { "zlib1", BULP_COMPRESSION_OPTIONS_INIT_ZLIB(1, 256) },
  { "zlib3", BULP_COMPRESSION_OPTIONS_INIT_ZLIB(3, 256) },
  { "zlib5", BULP_COMPRESSION_OPTIONS_INIT_ZLIB(5, 256) },
  { "zlib7", BULP_COMPRESSION_OPTIONS_INIT_ZLIB(7, 256) },
  { "zlib9", BULP_COMPRESSION_OPTIONS_INIT_ZLIB(9, 256) },
  { "snappy", BULP_COMPRESSION_OPTIONS_INIT_SNAPPY(256) },
};


int main()
{
  BulpSlab compressed_data = BULP_SLAB_INIT;
  BulpSlab uncompressed_data = BULP_SLAB_INIT;
  bulp_bool report = BULP_FALSE;

  for (unsigned tv = 0; tv < sizeof(test_vectors)/sizeof(test_vectors[0]); tv++)
    for (unsigned co = 0; co < sizeof(compression_options)/sizeof(compression_options[0]); co++)
      {
        BulpError *error = NULL;

        // compress
        bulp_compress (&compression_options[co].comp_opts,
                       test_vectors[tv].data->length,
                       test_vectors[tv].data->data,
                       &compressed_data);

        // uncompress
        if (!bulp_decompress (compression_options[co].comp_opts.algorithm,
                              compressed_data.length,
                              compressed_data.data,
                              &uncompressed_data, &error))
          bulp_die ("error uncompressing: %s", error->message);

        // verify
        assert (uncompressed_data.length == test_vectors[tv].data->length);
        assert (memcmp (uncompressed_data.data,
                        test_vectors[tv].data->data,
                        uncompressed_data.length) == 0);

        // report
        if (report)
          fprintf(stderr,
                  "%s\t%s\t%u\t%u\t%.01f%%\n",
                  test_vectors[tv].name,
                  compression_options[co].name,
                  (unsigned) test_vectors[tv].data->length,
                  (unsigned) compressed_data.length,
                  (double) compressed_data.length * 100.0 / test_vectors[tv].data->length);
      }
  return 0;
}
