
typedef enum {
  BULP_CLASS_TYPE_READER,
  BULP_CLASS_TYPE_WRITER,
} BulpClassType;

typedef struct BulpClass BulpClass;
typedef struct BulpObject BulpObject;

struct BulpClass {
  const char *class_name;
  BulpClass *parent_class;
  BulpClassType class_type;
  size_t sizeof_class;
  size_t sizeof_instance;

  void (*destroy) (BulpObject *object);
};

struct BulpObject {
  BulpClass *bulp_class;
};


