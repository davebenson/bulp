
typedef enum {
  BULP_COMPRESSION_ALGORITHM_NONE,
  BULP_COMPRESSION_ALGORITHM_ZLIB,
  BULP_COMPRESSION_ALGORITHM_SNAPPY,
} BulpCompressionAlgorithm;

typedef struct {
  BulpCompressionAlgorithm algorithm;
  int level;            // 1 .. 9
  unsigned records_per_chunk;
} BulpCompressionOptions;

#define BULP_COMPRESSION_OPTIONS_INIT { BULP_COMPRESSION_ALGORITHM_SNAPPY, 5, 32 }


bulp_bool bulp_decompress (BulpCompressionAlgorithm algo,
                           size_t                   compressed_length,
                           const uint8_t           *compressed_data,
                           BulpSlab                *uncompressed_data_out,
                           BulpError              **error);
void      bulp_compress   (BulpCompressionOptions *options,
                           size_t                  uncompressed_length,
                           const uint8_t          *uncompressed_data,
                           BulpSlab               *compressed_data_out);
