

typedef struct BulpMerger BulpMerger;
struct BulpMerger
{
  void (*merge) (BulpMerger *merger,
                 size_t a_length,
                 const uint8_t *a_data,
                 size_t b_length,
                 const uint8_t *b_data,
                 BulpSlab *out);
  void (*destroy) (BulpMerger *merger);
  BulpFormat *format;
  unsigned ref_count;
};

typedef struct BulpComparator BulpComparator;
struct BulpComparator
{
  int  (*compare) (BulpComparator *comparator,
                   size_t a_length,
                   const uint8_t *a_data,
                   size_t b_length,
                   const uint8_t *b_data);
  void (*destroy) (BulpMerger *merger);
  BulpFormat *format;
  unsigned ref_count;
};

