
#define BULP_BURP_HEADER_MAGIC "..."
#define BULP_BURP_HEADER_MAGIC_LEN 8
#define BULP_BURP_RECORD_START "..."
#define BULP_BURP_RECORD_START_LEN 8

struct BulpReaderBurpOptions {
  /* the directory from which to locate 'filename'.
   * Usually -1 to signal default path resolution.
   */
  int dir_fd_atfile;

  /* filename to open for reading */
  const char *filename;

  /* may be NULL to indicate that any format is ok */
  BulpFormat *format;
};

#define BULP_READER_BURP_OPTIONS             \
{                                            \
  -1,   /* dir_fd_atfile */                  \
  NULL, /* fileanme */                       \
  NULL, /* format */                         \
}


BulpReader *bulp_reader_new_burp (const BulpReaderBurpFileOptions *options,
                                  BulpError **error);


BulpWriter *bulp_writer_new_burp (const BulpWriterBurpFileOptions *options,
                                  BulpError **error);


