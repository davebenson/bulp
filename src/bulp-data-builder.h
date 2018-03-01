
typedef struct BulpDataBuilderPiece BulpDataBuilderPiece;
typedef struct BulpDataBuilder BulpDataBuilder;

struct BulpDataBuilderPiece
{
  unsigned len;
  const uint8_t *data;
  BulpDataBuilderPiece *next;
};

struct BulpDataBuilder
{
  unsigned n_pieces;
  BulpMemPool str_buf;
  BulpMemPoolFixed piece_buf;
  size_t cur_len;
  BulpDataBuilderPiece *first_piece, *last_piece;
};

void bulp_data_builder_init_buf (BulpDataBuilder *builder,
                                 size_t str_heap_alloced,
                                 char *str_heap,
                                 size_t pieces_alloced,
                                 BulpDataBuilderPiece *piece_heap);
void bulp_data_builder_clear    (BulpDataBuilder *builder);

#define BULP_DATA_BUILDER_DECLARE_ON_STACK(name, str_heap_size, n_pieces) \
  char name##__str_heap[str_heap_alloced]; \
  struct BulpDataBuilderPiece name##__pieces[n_pieces]; \
  BulpDataBuilder name; \
  bulp_data_builder_init_buf (&name, str_heap_size, name##__str_heap, n_pieces, name##__pieces);
  

void bulp_data_builder_append_nocopy (BulpDataBuilder *builder,
                                      size_t           len,
                                      const uint8_t   *data);
void bulp_data_builder_append        (BulpDataBuilder *builder,
                                      size_t           len,
                                      const uint8_t   *data);
void bulp_data_builder_append_byte   (BulpDataBuilder *builder,
                                      uint8_t          byte);

BulpDataBuilderPiece *bulp_data_builder_append_placeholder (BulpDataBuilder *builder);
void bulp_data_builder_update_placeholder (BulpDataBuilder *builder,
                                           BulpDataBuilderPiece *piece,
                                           size_t           len,
                                           const uint8_t   *data);

void     bulp_data_builder_to_buffer (BulpDataBuilder *builder,
                                      BulpBuffer      *out);
size_t   bulp_data_builder_get_size  (BulpDataBuilder *builder);
void     bulp_data_builder_build     (BulpDataBuilder *builder,
                                      uint8_t         *data_out);

