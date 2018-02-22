
typedef enum {
  BULP_WRITER_CLOSE_RESULT_OK,
  BULP_WRITER_CLOSE_RESULT_ERROR
} BulpWriterCloseResult;

typedef enum {
  BULP_WRITE_RESULT_OK,
  BULP_WRITE_RESULT_ERROR
} BulpWriteResult;

typedef struct BulpWriterClass BulpWriterClass;
typedef struct BulpWriter BulpWriter;

struct BulpWriterClass {
  BulpClass base_class;
 
  // optional
  BulpWriterCloseResult (*close) (BulpWriter *writer, BulpError **error);
};

struct BulpWriter {
  BulpObject base_object;
  BulpFormat *writer_format;
  BulpWriteResult (*write) (BulpWriter *writer, size_t data_length, const uint8_t *data, BulpError **error);
};

