
struct BulpIndexerOptions
{
  BulpCompressionOptions compression_options;

  BulpFormat *key_format;
  BulpFormat *value_format;
};

struct BulpIndexerFileLevel
{
  int key_fd;
  int index_fd;
  uint64_t key_offset;
};



.#.index  == records:
               key offset, key length
               compression_blob (start/length)
.#.keys   == raw keys

