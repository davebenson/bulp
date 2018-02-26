
typedef union BulpFormat BulpFormat;
typedef struct BulpNamespace BulpNamespace;

typedef enum {
  BULP_FORMAT_TYPE_BINARY_DATA,
  BULP_FORMAT_TYPE_PACKED,
  BULP_FORMAT_TYPE_STRING,
  BULP_FORMAT_TYPE_ENUM,
  BULP_FORMAT_TYPE_INT,
  BULP_FORMAT_TYPE_FLOAT,
  BULP_FORMAT_TYPE_STRUCT,
  BULP_FORMAT_TYPE_UNION,
  BULP_FORMAT_TYPE_MESSAGE,
  BULP_FORMAT_TYPE_OPTIONAL
} BulpFormatType;

typedef struct {
  size_t    (*get_packed_size) (BulpFormat *format,
                                void *native_data);
  size_t    (*pack)            (BulpFormat *format,
                                void *native_data,
                                uint8_t *packed_data_out);
  size_t    (*pack_to)         (BulpFormat *format,
                                void *native_data,
                                BulpBuffer *out);
  size_t    (*unpack)          (BulpFormat *format,
                                size_t packed_len,
                                const uint8_t *packed_data,
                                void *native_data_out,
                                BulpMemPool *pool,
                                BulpError **error);

  void      (*packed_to_json)  (BulpFormat  *format,
                                ...);
  void      (*json_to_packed)  (BulpFormat  *format,
                                ...);

  void      (*destruct_format) (BulpFormat  *format);
} BulpFormatVFuncs;

typedef struct {
  BulpFormatType type;
  uint32_t ref_count;
  BulpFormatVFuncs vfuncs;

  BulpNamespace *canonical_ns;
  const char *canonical_name;

  /* native (ie equivalent to generated C code) representation */
  size_t c_alignof;
  size_t c_sizeof;
  bulp_bool copy_with_memcpy;
  bulp_bool is_zeroable;
  const char *c_typename;               // mixed case - first letter capital
  const char *c_func_prefix;            // ie all lowercase with underscores
  const char *c_macro_prefix;           // ie all uppercase with underscores
} BulpFormatBase;

/* short int long
 * ushort uint ulong
 * int8 int16 int32 int64
 * uint8 uint16 uint32 uint64
 *
 * byte==uint8
 */
typedef struct {
  BulpFormatBase base;
  unsigned byte_size;       /* 1,2,4,8 */
  bulp_bool is_signed;
  bulp_bool is_b128;               /* variable-length encoded; otherwise, fixed-length little-endian */
} BulpFormatInt;

typedef enum
{
  BULP_FLOAT_TYPE_FLOAT32,
  BULP_FLOAT_TYPE_FLOAT64,
} BulpFloatType;

typedef struct {
  BulpFormatBase base;
  BulpFloatType float_type;
} BulpFormatFloat;

typedef struct {
  BulpFormatBase base;
} BulpFormatBinaryData;

typedef enum {
  BULP_STRING_TYPE_ASCII,
  BULP_STRING_TYPE_UTF8
} BulpStringType;

/* ascii string
   ascii0 string0 */
typedef enum {
  BULP_STRING_LENGTH_TYPE_NUL_TERMINATION,
  BULP_STRING_LENGTH_TYPE_LENGTH_PREFIXED,
} BulpStringLengthType;

typedef struct BulpFormatString {
  BulpFormatBase base;
  BulpStringType string_type;
  BulpStringLengthType length_type;
} BulpFormatString;

typedef struct BulpFormatArray {
  BulpFormatBase base;
  BulpFormat *element_format;
} BulpFormatArray;

typedef struct {
  ssize_t name_len;
  const char *name;
  unsigned n_bits;

} BulpPackedElement;

typedef struct {
  const char *name;
  unsigned n_bits;

  /* the remaining fields are computed by bulp_format_packed_new() or by
   * the machine-generated code. */
  unsigned packed_bit_offset;                   // XXX: version dependent, do not use

  /* machine-dependent */
  unsigned native_byte_offset;
  uint8_t native_word_size;                     // 1,2,4
} BulpFormatPackedElement;

unsigned bulp_packed_element_get_native   (BulpFormatPackedElement *elt,
                                           const void        *native_instance);
unsigned bulp_packed_element_set_native   (BulpFormatPackedElement *elt,
                                           void              *native_instance,
                                           unsigned           value);
#if 0
/* versioning?  i think we need to skip this */
unsigned bulp_packed_element_get_packed (const void        *packed_data,
                                                    BulpFormatPackedElement *elt);
unsigned bulp_packed_element_set_packed       (void              *packed_data,
                                        BulpFormatPackedElement *elt,
                                        unsigned           value);
#endif



typedef struct BulpFormatPacked {
  BulpFormatBase base;
  unsigned n_elements;
  BulpFormatPackedElement *elements;
} BulpFormatPacked;

BulpFormat *bulp_format_packed_new (size_t n_elts,
                                    BulpPackedElement *elts);
BulpFormatPackedElement *bulp_format_packed_lookup_element(BulpFormat *format,
                                                           ssize_t name_len,
                                                           const char *name);

typedef struct {
  char *name;
  bulp_bool set_value;
  unsigned value_if_set;
} BulpEnumValue;

typedef struct {
  char *name;
  uint32_t value;
} BulpFormatEnumValue;

typedef struct
{
  BulpFormatBase base;
  unsigned n_values;
  BulpFormatEnumValue *values;
} BulpFormatEnum;
BulpFormatEnumValue *bulp_format_enum_lookup_by_name (BulpFormat*format,
                                                      ssize_t    name_len,
                                                      const char *name);

typedef struct {
  const char *name;
  BulpFormat *case_format;
  bulp_bool has_value;
  uint32_t value;
} BulpUnionCase;

typedef struct {
  const char *name;
  BulpFormat *case_format;
  uint32_t value;
} BulpFormatUnionCase;

typedef struct BulpFormatUnion {
  BulpFormatBase base;
  size_t n_cases;
  BulpFormatUnionCase *cases;
  unsigned native_type_size;
  unsigned native_info_offset;
} BulpFormatUnion;

BulpFormatUnionCase *bulp_format_union_lookup_by_value (BulpFormat *format,
                                                        uint32_t value);
BulpFormatUnionCase *bulp_format_union_lookup_by_name  (BulpFormat *format,
                                                        ssize_t     name_len,
                                                        const char *name);

typedef struct {
  unsigned start_version;
  unsigned end_version;
} BulpVersionRange;


typedef struct {
  int n_ranges;
  BulpVersionRange *ranges;
  void *default_value;
  bulp_bool ignored;
} BulpMemberVersioningInfo;

#define BULP_MEMBER_VERSIONING_INFO_INIT {0,NULL,NULL,BULP_FALSE}

typedef struct {
  const char *name;
  BulpFormat *format;
  BulpMemberVersioningInfo *versioning_info;
} BulpStructMember;

typedef struct {
  char *name;
  BulpFormat *format;
  BulpMemberVersioningInfo *versioning_info;
  unsigned native_offset;
} BulpFormatStructMember;
#define BULP_STRUCT_MEMBER_IS_EXTANT(member) \
  ((member)->versioning_info == NULL || !(member)->versioning_info->ignored)

typedef struct BulpFormatStruct {
  BulpFormatBase base;
  size_t n_members;
  BulpFormatStructMember *members;
} BulpFormatStruct;

BulpFormatStructMember *
bulp_format_struct_lookup_by_name (BulpFormat *format,
                                   ssize_t     name_len,
                                   const char *name_start);

typedef struct {
  char *name;
  bulp_bool set_value;
  unsigned value_if_set;
  BulpFormat *field_format;
} BulpMessageField;

typedef struct {
  char *name;
  uint32_t value;
  BulpFormat *field_format;
  unsigned native_offset;
} BulpFormatMessageField;

typedef struct
{
  BulpFormatBase base;
  unsigned n_fields;
  BulpFormatMessageField *fields;
} BulpFormatMessage;
BulpFormatMessageField *bulp_format_message_lookup_by_name (BulpFormat*format,
                                                      ssize_t    name_len,
                                                      const char *name);
BulpFormatMessageField *bulp_format_message_lookup_by_value (BulpFormat*format,
                                                             uint32_t value);
typedef struct {
  BulpFormatBase base;
  BulpFormat *subformat;
} BulpFormatOptional;

union BulpFormat {
  BulpFormatType type;
  BulpFormatBase base;
  BulpFormatBinaryData v_binary_data;
  BulpFormatPacked v_packed;
  BulpFormatString v_string;
  BulpFormatEnum v_enum;
  BulpFormatInt v_int;
  BulpFormatFloat v_float;
  BulpFormatStruct v_struct;
  BulpFormatUnion v_union;
  BulpFormatMessage v_message;
  BulpFormatOptional v_optional;
};

BulpFormat *bulp_format_from_binary_data (size_t length,
                                          const uint8_t data,
                                          BulpError **error);
BulpFormat *bulp_format_to_binary_data   (size_t length,
                                          const uint8_t data,
                                          BulpError **error);
BulpFormat *bulp_format_array_of         (BulpFormat *format);
BulpFormat *bulp_format_int_generic      (unsigned n_bits, /* 8,16,32,64 */
                                          bulp_bool is_signed,
                                          bulp_bool is_b128);
BulpFormat *bulp_format_try_int_generic  (unsigned n_bits, /* 8,16,32,64 */
                                          bulp_bool is_signed,
                                          bulp_bool is_b128,
                                          BulpError **error);
BulpFormat *bulp_format_string_generic   (unsigned n_bits, /* 8,16,32,64 */
                                          BulpStringType type,
                                          BulpStringLengthType length_type,
                                          bulp_bool is_b128);
BulpFormat *bulp_format_try_string_generic(unsigned n_bits, /* 8,16,32,64 */
                                          BulpStringType type,
                                          BulpStringLengthType length_type,
                                          BulpError **error);

// note: you cannot take the optional of an optional
BulpFormat *bulp_format_optional_of      (BulpFormat *subtype);

BulpFormat *bulp_format_new_enum         (unsigned n_values,
                                          BulpEnumValue *values);
BulpFormat *bulp_format_new_struct       (unsigned n_members,
                                          BulpStructMember *members);
BulpFormat *bulp_format_new_union        (unsigned n_cases,
                                          BulpUnionCase    *cases);
BulpFormat *bulp_format_new_message      (unsigned n_fields,
                                          BulpMessageField *fields);

BulpFormat *bulp_format_ref              (BulpFormat       *format);
void        bulp_format_unref            (BulpFormat       *format);


bulp_bool    bulp_namespace_is_toplevel     (BulpNamespace *ns);
BulpFormat * bulp_namespace_get_short       (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_int         (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_long        (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_ushort      (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_uint        (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_ulong       (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_int8        (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_int16       (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_int32       (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_int64       (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_uint8       (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_uint16      (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_uint32      (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_uint64      (BulpNamespace *toplevel_ns);
#define      bulp_namespace_get_byte        bulp_namespace_get_uint8
BulpFormat * bulp_namespace_get_float32     (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_float64     (BulpNamespace *toplevel_ns);

#define bulp_format_ascii       ((BulpFormat *) (&bulp_format_strings_global[0]))
#define bulp_format_string      ((BulpFormat *) (&bulp_format_strings_global[1]))
#define bulp_format_ascii0      ((BulpFormat *) (&bulp_format_strings_global[2]))
#define bulp_format_string0     ((BulpFormat *) (&bulp_format_strings_global[3]))

#define bulp_format_binary_data ((BulpFormat *) (&bulp_format_binary_data_global))

/* refers to a process-wide global-namespace */
BulpNamespace *bulp_namespace_global (void);

/* refers to a new global-namespace */
BulpNamespace *bulp_namespace_new_global (void);

/* Returns false if the name is already in use */
bulp_bool      bulp_namespace_add_subnamespace (BulpNamespace *ns,
                                                const char    *name,
                                                BulpNamespace *subns);
/* Returns false if the name is already in use */
bulp_bool      bulp_namespace_add_format       (BulpNamespace *ns,
                                                ssize_t        name_len,
                                                const char    *name,
                                                BulpFormat    *format,
                                                bulp_bool      is_canonical_ns);

unsigned       bulp_namespace_get_version      (BulpNamespace *ns);
void           bulp_namespace_set_version      (BulpNamespace *ns,
                                                unsigned       version);

typedef enum {
  BULP_NAMESPACE_ENTRY_SUBNAMESPACE,
  BULP_NAMESPACE_ENTRY_FORMAT
} BulpNamespaceEntryType;

typedef struct {
  BulpNamespaceEntryType type;
  union {
    BulpNamespace *v_namespace;
    BulpFormat *v_format;
  } info;
} BulpNamespaceEntry;
bulp_bool      bulp_namespace_query_1          (BulpNamespace *ns,
                                                const char    *name,
                                                BulpNamespaceEntry *out);
bulp_bool      bulp_namespace_query            (BulpNamespace *ns,
                                                const char    *dotted_name,
                                                BulpNamespaceEntry *out);
BulpNamespace *bulp_namespace_force_subnamespace(BulpNamespace *ns,
                                                const char    *name);

typedef bulp_bool (*BulpNamespaceForeachFunc) (BulpNamespace *ns,
                                               const char *name,
                                               BulpNamespaceEntry *value,
                                               void       *data);
bulp_bool      bulp_namespace_foreach          (BulpNamespace *ns,
                                                BulpNamespaceForeachFunc func,
                                                void *func_data);

bulp_bool      bulp_namespace_parse_file       (BulpNamespace *ns,
                                                const char    *filename,
                                                BulpError    **error);
bulp_bool      bulp_namespace_parse_data       (BulpNamespace *ns,
                                                const char    *pseudo_filename,
                                                size_t         file_length,
                                                const uint8_t *file_data,
                                                BulpError    **error);
BulpNamespace *bulp_namespace_ref (BulpNamespace *ns);
void           bulp_namespace_unref (BulpNamespace *ns);


// private
extern BulpFormatInt bulp_format_ints_global[14];
extern BulpFormatString bulp_format_strings_global[4];
extern BulpFormatBinaryData bulp_format_binary_data_global;
