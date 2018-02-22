typedef struct BulpMapper BulpMapper;
typedef struct BulpMapperClass BulpMapperClass;

struct BulpMapperClass {
  BulpClass base_class;
  bulp_bool (*mapper)(BulpMapper *mapper,
                      size_t len, const uint8_t *data,
                      size_t *len_out, const uint8_t **data_out);
};

struct BulpMapper {
  BulpObject base_instance;
  BulpFormat *in_format;
  BulpFormat *out_format;
};

/* for implementors of subclasses */
BulpMapper *bulp_mapper_new_protected (BulpMapperClass *mapper_class,
                                       BulpFormat      *in_format,
                                       BulpFormat      *out_format);

