#include "bulp.h"
#include <zlib.h>
#include <snappy-c.h>


bulp_bool bulp_decompress (BulpCompressionAlgorithm algo,
                           size_t                   compressed_length,
                           const uint8_t           *compressed_data,
                           BulpSlab                *uncompressed_data_out,
                           BulpError              **error)
{
  switch (algo)
    {
      case BULP_COMPRESSION_ALGORITHM_NONE:
        memcpy (bulp_slab_set_size (uncompressed_data_out, compressed_length),
                compressed_data,
                compressed_length);
        return BULP_TRUE;

      case BULP_COMPRESSION_ALGORITHM_ZLIB:
        {
          uint32_t uncomp_len;
          unsigned lenlen = bulp_uint_unpack (compressed_length, compressed_data, &uncomp_len, error);
          if (lenlen == 0)
            return BULP_FALSE;
          uLongf dest_len = uncomp_len;
          if (uncompress (bulp_slab_set_size (uncompressed_data_out, uncomp_len), &dest_len,
                          compressed_data + lenlen,
                          compressed_length - lenlen) != Z_OK)
            {
              *error = bulp_error_new_bad_data ("bad ZLIB compressed data");
              return BULP_FALSE;
            }
          return BULP_TRUE;
        }

      case BULP_COMPRESSION_ALGORITHM_SNAPPY:
        {
          size_t output_length;
          if (snappy_uncompressed_length((const char *) compressed_data, compressed_length, &output_length) != SNAPPY_OK)
            {
              *error = bulp_error_new_bad_data ("bad SNAPPY compressed data");
              return BULP_FALSE;
            }
         char* output = (char*) bulp_slab_set_size (uncompressed_data_out, output_length);
         if (snappy_uncompress((const char *) compressed_data, compressed_length, output, &output_length) == SNAPPY_OK)
           return BULP_TRUE;
         else
           {
             *error = bulp_error_new_bad_data ("bad SNAPPY compressed data");
             return BULP_FALSE;
           }
        }

      default:
        assert(0);
    }
}


void      bulp_compress   (BulpCompressionOptions *options,
                           size_t                  uncompressed_length,
                           const uint8_t          *uncompressed_data,
                           BulpSlab               *compressed_data_out)
{
  switch (options->algorithm)
    {
      case BULP_COMPRESSION_ALGORITHM_NONE:
        bulp_slab_set_data (compressed_data_out, uncompressed_length, uncompressed_data);
        return;

      case BULP_COMPRESSION_ALGORITHM_ZLIB:
        {
          uLong bound = compressBound (uncompressed_length);
          unsigned lenlen = bulp_uint_get_packed_size (uncompressed_length);
          uLongf dest_len = bound;
          uint8_t *comp_data = bulp_slab_set_size (compressed_data_out, lenlen + bound);
          bulp_uint_pack (uncompressed_length, comp_data);
          if (compress2 (comp_data + lenlen,
                         &dest_len,
                         uncompressed_data,
                         uncompressed_length,
                         options->level) != Z_OK)
            {
              assert(0);
            }
          bulp_slab_truncate (compressed_data_out, dest_len + lenlen);
          return;
        }

      case BULP_COMPRESSION_ALGORITHM_SNAPPY:
        {
          char *comp_data = (char *) bulp_slab_set_size (compressed_data_out, snappy_max_compressed_length (uncompressed_length));
          size_t output_length = compressed_data_out->length;
          if (snappy_compress( (const char *)uncompressed_data, uncompressed_length, comp_data, &output_length) != SNAPPY_OK)
            {
              assert(0);
            }
         bulp_slab_truncate (compressed_data_out, output_length);
         return;
       }

      default:
        assert(0);
    }
}
