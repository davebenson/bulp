#include "bulp.h"
#include "dsk-list-macros.h"
#include <string.h>

#define GET_PIECE_QUEUE(builder) \
  BulpDataBuilderPiece *, (builder)->first_piece, (builder)->last_piece, next

#define PLACEHOLDER_SIZE 0xffffffff

void bulp_data_builder_init_buf (BulpDataBuilder *builder,
                                 size_t str_heap_alloced,
                                 char *str_heap,
                                 size_t pieces_alloced,
                                 BulpDataBuilderPiece *piece_heap)
{
  builder->n_pieces = 0;
  bulp_mem_pool_init_buf (&builder->str_buf, str_heap_alloced, str_heap);
  bulp_mem_pool_fixed_init_buf (&builder->piece_buf, sizeof (BulpDataBuilderPiece), pieces_alloced, piece_heap);
  builder->cur_len = 0;
  builder->first_piece = NULL;
  builder->last_piece = NULL;
}

void bulp_data_builder_clear    (BulpDataBuilder *builder)
{
  bulp_mem_pool_clear (&builder->str_buf);
  bulp_mem_pool_fixed_clear (&builder->piece_buf);
}

void bulp_data_builder_append_nocopy (BulpDataBuilder *builder,
                                      size_t           len,
                                      const uint8_t   *data)
{
  BulpDataBuilderPiece *p = bulp_mem_pool_fixed_alloc (&builder->piece_buf);
  p->len = len;
  p->data = data;
  builder->cur_len += len;
  DSK_QUEUE_ENQUEUE (GET_PIECE_QUEUE (builder), p);
}

void
bulp_data_builder_append        (BulpDataBuilder *builder,
                                 size_t           len,
                                 const uint8_t   *data)
{
  BulpDataBuilderPiece *p = bulp_mem_pool_fixed_alloc (&builder->piece_buf);
  p->len = len;
  p->data = bulp_mem_pool_memdup (&builder->str_buf, len, data);
  builder->cur_len += len;
  DSK_QUEUE_ENQUEUE (GET_PIECE_QUEUE (builder), p);
}

void bulp_data_builder_append_byte   (BulpDataBuilder *builder,
                                      uint8_t          byte)
{
  bulp_data_builder_append (builder, 1, &byte);
}

BulpDataBuilderPiece *bulp_data_builder_append_placeholder (BulpDataBuilder *builder)
{
  BulpDataBuilderPiece *p = bulp_mem_pool_fixed_alloc (&builder->piece_buf);
  p->len = PLACEHOLDER_SIZE;
  p->data = NULL;
  DSK_QUEUE_ENQUEUE (GET_PIECE_QUEUE (builder), p);
  return p;
}

void bulp_data_builder_update_placeholder (BulpDataBuilder *builder,
                                           BulpDataBuilderPiece *piece,
                                           size_t           len,
                                           const uint8_t   *data)
{
  assert(piece->len == PLACEHOLDER_SIZE);
  
  piece->len = len;
  piece->data = bulp_mem_pool_memdup (&builder->str_buf, len, data);
  builder->cur_len += len;
}

void     bulp_data_builder_to_buffer (BulpDataBuilder *builder,
                                      BulpBuffer      *out)
{
  for (BulpDataBuilderPiece *piece = builder->first_piece;
       piece != NULL;
       piece = piece->next)
    {
      bulp_buffer_append (out, piece->len, piece->data);
    }
}
size_t   bulp_data_builder_get_size  (BulpDataBuilder *builder)
{
  return builder->cur_len;
}
void     bulp_data_builder_build     (BulpDataBuilder *builder,
                                      uint8_t         *data_out)
{
  uint8_t* at = data_out;
  for (BulpDataBuilderPiece *piece = builder->first_piece;
       piece != NULL;
       piece = piece->next)
    {
      memcpy (at, piece->data, piece->len);
      at += piece->len;
    }
}

