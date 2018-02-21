/* invariant:  if a buffer.size==0, then first_frag/last_frag == NULL.
   corollary:  if a buffer.size==0, then the buffer is using no memory. */

typedef struct _BulpBuffer BulpBuffer;
typedef struct _BulpBufferFragment BulpBufferFragment;

struct _BulpBufferFragment
{
  BulpBufferFragment    *next;
  uint8_t              *buf;
  unsigned              buf_max_size;	/* allocation size of buf */
  unsigned              buf_start;	/* offset in buf of valid data */
  unsigned              buf_length;	/* length of valid data in buf; != 0 */
  
  bulp_bool              is_foreign;
  BulpDestroyNotify      destroy;
  void                 *destroy_data;
};

struct _BulpBuffer
{
  unsigned              size;

  BulpBufferFragment    *first_frag;
  BulpBufferFragment    *last_frag;
};

#define BULP_BUFFER_INIT		{ 0, NULL, NULL }


void     bulp_buffer_init                (BulpBuffer       *buffer);

unsigned bulp_buffer_read                (BulpBuffer    *buffer,
                                         unsigned      max_length,
                                         void         *data);
unsigned bulp_buffer_peek                (const BulpBuffer* buffer,
                                         unsigned      max_length,
                                         void         *data);
int      bulp_buffer_discard             (BulpBuffer    *buffer,
                                         unsigned      max_discard);
char    *bulp_buffer_read_line           (BulpBuffer    *buffer);

char    *bulp_buffer_parse_string0       (BulpBuffer    *buffer);
                        /* Returns first char of buffer, or -1. */
int      bulp_buffer_peek_byte           (const BulpBuffer *buffer);
int      bulp_buffer_read_byte           (BulpBuffer    *buffer);

uint8_t  bulp_buffer_byte_at             (BulpBuffer    *buffer,
                                         unsigned      index);
uint8_t  bulp_buffer_last_byte           (BulpBuffer    *buffer);
/* 
 * Appending to the buffer.
 */
void     bulp_buffer_append              (BulpBuffer    *buffer, 
                                         unsigned      length,
                                         const void   *data);

BULP_INLINE_FUNC void bulp_buffer_append_small(BulpBuffer    *buffer, 
                                         unsigned      length,
                                         const void   *data);
void     bulp_buffer_append_string       (BulpBuffer    *buffer, 
                                         const char   *string);
BULP_INLINE_FUNC void bulp_buffer_append_byte(BulpBuffer    *buffer, 
                                         uint8_t       byte);
void      bulp_buffer_append_byte_f      (BulpBuffer    *buffer, 
                                         uint8_t       byte);
void     bulp_buffer_append_repeated_byte(BulpBuffer    *buffer, 
                                         size_t        count,
                                         uint8_t       byte);
#define bulp_buffer_append_zeros(buffer, count) \
  bulp_buffer_append_repeated_byte ((buffer), 0, (count))


void     bulp_buffer_append_string0      (BulpBuffer    *buffer,
                                         const char   *string);

void     bulp_buffer_append_foreign      (BulpBuffer    *buffer,
					 unsigned      length,
                                         const void   *data,
					 BulpDestroyNotify destroy,
					 void         *destroy_data);

void     bulp_buffer_printf              (BulpBuffer    *buffer,
					 const char   *format,
					 ...) BULP_GNUC_PRINTF(2,3);
void     bulp_buffer_vprintf             (BulpBuffer    *buffer,
					 const char   *format,
					 va_list       args);

uint8_t  bulp_buffer_get_last_byte       (BulpBuffer    *buffer);
uint8_t  bulp_buffer_get_byte_at         (BulpBuffer    *buffer,
                                         size_t        idx);


/* --- appending data that will be filled in later --- */
typedef struct {
  BulpBuffer *buffer;
  BulpBufferFragment *fragment;
  unsigned offset;
  unsigned length;
} BulpBufferPlaceholder;

void     bulp_buffer_append_placeholder  (BulpBuffer    *buffer,
                                         unsigned      length,
                                         BulpBufferPlaceholder *out);
void     bulp_buffer_placeholder_set     (BulpBufferPlaceholder *placeholder,
                                         const void       *data);

/* --- buffer-to-buffer transfers --- */
/* Take all the contents from src and append
 * them to dst, leaving src empty.
 */
unsigned bulp_buffer_drain               (BulpBuffer    *dst,
                                         BulpBuffer    *src);

/* Like `drain', but only transfers some of the data. */
unsigned bulp_buffer_transfer            (BulpBuffer    *dst,
                                         BulpBuffer    *src,
					 unsigned      max_transfer);

/* --- file-descriptor mucking --- */
int      bulp_buffer_writev              (BulpBuffer       *read_from,
                                         int              fd);
int      bulp_buffer_writev_len          (BulpBuffer *read_from,
		                         int              fd,
		                         unsigned         max_bytes);
/* returns TRUE iff all the data was written.  'read_from' is blank. */
bulp_boolean bulp_buffer_write_all_to_fd  (BulpBuffer       *read_from,
                                         int              fd,
                                         BulpError       **error);
int      bulp_buffer_readv               (BulpBuffer       *write_to,
                                         int              fd);

/* --- deallocating memory used by buffer --- */

/* This deallocates memory used by the buffer-- you are responsible
 * for the allocation and deallocation of the BulpBuffer itself. */
void     bulp_buffer_clear               (BulpBuffer    *to_destroy);

/* Same as calling clear/init */
void     bulp_buffer_reset               (BulpBuffer    *to_reset);

/* Return a string and clear the buffer. */
char *bulp_buffer_empty_to_string (BulpBuffer *buffer);

/* --- iterating through the buffer --- */
/* 'frag_offset_out' is the offset of the returned fragment in the whole
   buffer. */
BulpBufferFragment *bulp_buffer_find_fragment (BulpBuffer   *buffer,
                                             unsigned     offset,
                                             unsigned    *frag_offset_out);

/* Free all unused buffer fragments. */
void     _bulp_buffer_cleanup_recycling_bin ();

typedef enum {
  BULP_BUFFER_DUMP_DRAIN = (1<<0),
  BULP_BUFFER_DUMP_NO_DRAIN = (1<<1),
  BULP_BUFFER_DUMP_FATAL_ERRORS = (1<<2),
  BULP_BUFFER_DUMP_LEAVE_PARTIAL = (1<<3),
  BULP_BUFFER_DUMP_NO_CREATE_DIRS = (1<<4),
  BULP_BUFFER_DUMP_EXECUTABLE = (1<<5),
} BulpBufferDumpFlags;

bulp_boolean bulp_buffer_dump (BulpBuffer          *buffer,
                             const char         *filename,
                             BulpBufferDumpFlags  flags,
                             BulpError          **error);
                          

/* misc */
int bulp_buffer_index_of(BulpBuffer *buffer, char char_to_find);

unsigned bulp_buffer_fragment_peek (BulpBufferFragment *fragment,
                                   unsigned           offset,
                                   unsigned           length,
                                   void              *buf);
bulp_boolean bulp_buffer_fragment_advance (BulpBufferFragment **frag_inout,
                                         unsigned           *offset_inout,
                                         unsigned            skip);

/* HACKS */
/* NOTE: the buffer is INVALID after this call, since no empty
   fragments are allowed.  You MUST deal with this if you do 
   not actually add data to the buffer */
void bulp_buffer_append_empty_fragment (BulpBuffer *buffer);

void bulp_buffer_maybe_remove_empty_fragment (BulpBuffer *buffer);

/* a way to delete the fragment from bulp_buffer_append_empty_fragment() */
void bulp_buffer_fragment_free (BulpBufferFragment *fragment);


#if BULP_CAN_INLINE || defined(BULP_IMPLEMENT_INLINES)
BULP_INLINE_FUNC void bulp_buffer_append_small(BulpBuffer    *buffer, 
                                         unsigned      length,
                                         const void   *data)
{
  BulpBufferFragment *f = buffer->last_frag;
  if (f != NULL
   && !f->is_foreign
   && f->buf_start + f->buf_length + length <= f->buf_max_size)
    {
      uint8_t *dst = f->buf + (f->buf_start + f->buf_length);
      const uint8_t *src = data;
      f->buf_length += length;
      buffer->size += length;
      while (length--)
        *dst++ = *src++;
    }
  else
    bulp_buffer_append (buffer, length, data);
}
BULP_INLINE_FUNC void bulp_buffer_append_byte(BulpBuffer    *buffer, 
                                            uint8_t       byte)
{
  BulpBufferFragment *f = buffer->last_frag;
  if (f != NULL
   && !f->is_foreign
   && f->buf_start + f->buf_length < f->buf_max_size)
    {
      f->buf[f->buf_start + f->buf_length] = byte;
      ++(f->buf_length);
      buffer->size += 1;
    }
  else
    bulp_buffer_append (buffer, 1, &byte);
}

#endif
