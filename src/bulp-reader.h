typedef enum
{
  BULP_READER_CLOSE_OK,
  BULP_READER_CLOSE_OK_NOT_DONE,
  BULP_READER_CLOSE_ERROR,
} BulpReaderCloseResult;

#define BULP_READER_CLOSE_RESULT_IS_OK(result) \
  ((result) == BULP_READER_CLOSE_OK || (result) == BULP_READER_CLOSE_OK_NOT_DONE)

typedef struct BulpReaderClass BulpReaderClass;
typedef struct BulpReader BulpReader;

struct BulpReaderClass {
  BulpClass base_class;

  // initial values
  BulpReadResult (*peek) (BulpReader *reader,
                          size_t *data_length_out,
                          const uint8_t **data_out,
                          BulpError **error);
  BulpReadResult (*advance) (BulpReader *reader, BulpError **error);

  // optional
  BulpReaderCloseResult (*close) (BulpReader *reader, BulpError **error);
};
extern BulpReaderClass bulp_reader_class;

struct BulpReader {
  BulpObject base_instance;
  BulpFormat *reader_format;

  BulpReadResult (*peek) (BulpReader *reader, size_t *data_length_out, const uint8_t **data_out, BulpError **error);
  BulpReadResult (*advance) (BulpReader *reader, BulpError **error);

};

BulpReader *bulp_readers_concat_new     (unsigned n_readers, BulpReader **readers);
BulpReader *bulp_reader_sync_filter_new (BulpReader *reader, BulpFilter *filter);
BulpReader *bulp_reader_flat_mapped_new (BulpReader *reader, BulpMapper *flat_map);
BulpReader *bulp_reader_mapped_new      (BulpReader *reader, BulpMapper *map);
BulpReaderCloseResult bulp_reader_close (BulpReader *reader, BulpError **error);

/* for implementors of subclasses */
BulpReader *bulp_reader_new_protected   (BulpReaderClass *reader_class,
                                         BulpFormat      *format);
