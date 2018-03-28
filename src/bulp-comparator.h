typedef struct BulpComparator BulpComparator;

typedef int (*BulpComparatorCompare) (BulpComparator *comparator,
                                      const uint8_t  *a_data,
                                      const uint8_t  *b_data);
typedef int (*BulpComparatorCompare2)(BulpComparator *comparator,
                                      const uint8_t  *a_data,
                                      const uint8_t  *b_data,
                                      size_t         *len_out);
typedef void (*BulpComparatorDestroy)(BulpComparator *comparator);

struct BulpComparator
{
  BulpComparatorCompare  compare;
  BulpComparatorCompare2 compare2;
  BulpComparatorDestroy destroy;
  BulpFormat *format;
  unsigned ref_count;
};
BulpComparator *bulp_comparator_ref   (BulpComparator *);
void            bulp_comparator_unref (BulpComparator *);




BulpComparator *bulp_comparator_new (BulpFormat *format);




/* --- for implementing your own comparators --- */
BulpComparator *bulp_comparator_new_protected (size_t sizeof_comparator,
                                               BulpFormat *format,
                                               BulpComparatorCompare compare,
                                               BulpComparatorCompare2 compare2,
                                               BulpComparatorDestroy destroy);

// Call this at the end of your "destroy" method,
// or pass it in as "destroy" if you have nothing else to clean up.
void            bulp_comparator_destroy_protected (BulpComparator *comparator);
