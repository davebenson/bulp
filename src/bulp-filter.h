
struct _BulpFilterClass {
  BulpClass base_class;
  bulp_bool (*filter)(BulpFilter *filter, size_t len, const uint8_t *data);
};

struct _BulpFilter {
  BulpObject base_instance;
  BulpFormat *format;
};

/* for implementors of subclasses */
BulpFilter *bulp_filter_new_protected (BulpFilterClass *filter_class,
                                       BulpFormat      *format);


