

typedef struct BulpReaderBurpOptions BulpReaderBurpOptions;

struct BulpReaderBurpOptions {
  /* the directory from which to locate 'filename'.
   * Usually -1 to signal default path resolution.
   */
  int dir_fd_atfile;

  /* filename to open for reading */
  const char *filename;

  /* may be NULL to indicate that any format is ok */
  BulpFormat *format;

  /* uncompressed, zlib, ??? */
  BulpCompressionOptions compression_options;
};

#define BULP_READER_BURP_OPTIONS                    \
{                                                   \
  -1,   /* dir_fd_atfile */                         \
  NULL, /* fileanme */                              \
  NULL, /* format */                                \
  0,    /* compression algo (0=uncompressed) */     \
  BULP_COMPRESSION_DEFAULTS                         \
}


BulpReader *bulp_reader_new_burp (const BulpReaderBurpOptions *options,
                                  BulpError **error);

typedef struct BulpWriterBurpOptions BulpWriterBurpOptions;

struct BulpWriterBurpOptions {
  /* the directory from which to locate 'filename'.
   * Usually -1 to signal default path resolution.
   */
  int dir_fd_atfile;

  /* filename to open for reading */
  const char *filename;

  /* may be NULL to indicate that any format is ok */
  BulpFormat *format;
};

#define BULP_WRITER_BURP_OPTIONS             \
{                                            \
  -1,   /* dir_fd_atfile */                  \
  NULL, /* fileanme */                       \
  NULL, /* format */                         \
}


BulpWriter *bulp_writer_new_burp (const BulpWriterBurpOptions *options,
                                  BulpError **error);


