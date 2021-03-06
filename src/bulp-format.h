
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
  BULP_FORMAT_TYPE_OPTIONAL,
  BULP_FORMAT_TYPE_ARRAY
} BulpFormatType;

typedef struct {
  bulp_bool (*validate_native) (BulpFormat *format,
                                void *native_data,
                                BulpError **error);        // optional
  size_t    (*get_packed_size) (BulpFormat *format,
                                void *native_data);
  size_t    (*pack)            (BulpFormat *format,
                                void *native_data,
                                uint8_t *out);
  void      (*pack_to)         (BulpFormat *format,
                                void *native_data,
                                BulpDataBuilder *builder);
  size_t    (*unpack)          (BulpFormat *format,
                                size_t packed_len,
                                const uint8_t *packed_data,
                                void *native_data_out,
                                BulpMemPool *pool,
                                BulpError **error);

  void      (*destruct_format) (BulpFormat  *format);
} BulpFormatVFuncs;

typedef struct {
  BulpFormatType type;
  uint32_t ref_count;
  BulpFormatVFuncs vfuncs;

  BulpNamespace *canonical_ns;
  const char *canonical_name;

  /* native (ie equivalent to generated C code) representation */
  size_t      c_alignof;
  size_t      c_sizeof;
  bulp_bool   c_copy_with_memcpy;
  bulp_bool   c_is_zeroable;
  const char *c_typename;               // mixed case - first letter capital
  const char *c_func_prefix;            // ie all lowercase with underscores
  const char *c_macro_prefix;           // ie all uppercase with underscores

  BulpFormat *optional_of;      // created on demand
  BulpFormat *array_of;            // created on demand
} BulpFormatBase;

/* short int long                  // b128(zigzag_encode(N))
 * ushort uint ulong               // b128(N)
 * int8 int16 int32 int64          // le(N)
 * uint8 uint16 uint32 uint64      // le(N)
 *
 * byte==uint8
 */
typedef struct {
  BulpFormatBase base;
  unsigned byte_size;       /* 1,2,4,8 */
  unsigned log2_byte_size;  /* 0,1,2,3 */
  bulp_bool is_signed;

  /* If TRUE,  variable-length encoded little-endian;
   * If FALSE, fixed-length little-endian */
  bulp_bool is_b128;
} BulpFormatInt;

typedef enum
{
  BULP_FLOAT_TYPE_FLOAT32,
  BULP_FLOAT_TYPE_FLOAT64,
  BULP_FLOAT_TYPE_SPECIAL
} BulpFloatType;

typedef struct {
  BulpFormatBase base;
  BulpFloatType float_type;
  ... revive mrad's parameterization
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

  /* machine-dependent */
  unsigned native_byte_offset;
  uint8_t native_word_size;                     // 1,2,4
} BulpFormatPackedElement;

unsigned bulp_packed_element_get_native   (BulpFormatPackedElement *elt,
                                           const void        *native_instance);
void     bulp_packed_element_set_native   (BulpFormatPackedElement *elt,
                                           void              *native_instance,
                                           unsigned           value);


typedef struct BulpFormatPacked {
  BulpFormatBase base;
  unsigned n_elements;
  BulpFormatPackedElement *elements;
  BulpFormatPackedElement **elements_by_name;
  unsigned total_bit_size;
} BulpFormatPacked;

BulpFormat *bulp_format_packed_new (size_t n_elts,
                                    BulpPackedElement *elts);
BulpFormatPackedElement *bulp_format_packed_lookup_element(BulpFormat *format,
                                                           ssize_t name_len,
                                                           const char *name);

typedef struct {
  ssize_t name_len;
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
  BulpFormatEnumValue **values_by_name;
} BulpFormatEnum;
BulpFormatEnumValue *bulp_format_enum_lookup_by_name (BulpFormat*format,
                                                      ssize_t    name_len,
                                                      const char *name);
BulpFormatEnumValue *bulp_format_enum_lookup_by_value(BulpFormat*format,
                                                      unsigned   value);

typedef struct {
  const char *name;
  BulpFormat *format;
  bulp_bool set_value;
  uint32_t value_if_set;
} BulpUnionCase;

typedef struct {
  const char *name;
  BulpFormat *format;
  uint32_t value;
} BulpFormatUnionCase;

typedef struct BulpFormatUnion {
  BulpFormatBase base;
  size_t n_cases;
  BulpFormatUnionCase *cases;
  BulpFormatUnionCase **cases_by_name;
  unsigned native_type_size;
  unsigned native_info_offset;
} BulpFormatUnion;

BulpFormatUnionCase *bulp_format_union_lookup_by_value (BulpFormat *format,
                                                        uint32_t value);
BulpFormatUnionCase *bulp_format_union_lookup_by_name  (BulpFormat *format,
                                                        ssize_t     name_len,
                                                        const char *name);

typedef struct {
  const char *name;
  BulpFormat *format;

  // ignored for non-messages
  bulp_bool set_value;
  unsigned value_if_set;
} BulpStructMember;

typedef struct {
  char *name;
  BulpFormat *format;
  unsigned value;                               // if is message
  unsigned native_offset;
  void *native_default_value;
} BulpFormatStructMember;

typedef struct BulpFormatStruct {
  BulpFormatBase base;
  size_t n_members;
  BulpFormatStructMember *members;
  BulpFormatStructMember **members_by_name;
  bulp_bool is_extensible;
} BulpFormatStruct;

BulpFormatStructMember *
bulp_format_struct_lookup_by_name (BulpFormat *format,
                                   ssize_t     name_len,
                                   const char *name_start);
BulpFormatStructMember *
bulp_format_message_lookup_by_value (BulpFormat*format,
                                     uint32_t value);

#if 0
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
  BulpFormatMessageField **fields_by_name;
} BulpFormatMessage;
BulpFormatMessageField *bulp_format_message_lookup_by_name (BulpFormat*format,
                                                      ssize_t    name_len,
                                                      const char *name);
#endif
typedef struct {
  BulpFormatBase base;
  BulpFormat *subformat;
} BulpFormatOptional;

union BulpFormat {
  BulpFormatType type;
  BulpFormatBase base;
  BulpFormatBinaryData v_binary_data;
  BulpFormatArray v_array;
  BulpFormatPacked v_packed;
  BulpFormatString v_string;
  BulpFormatEnum v_enum;
  BulpFormatInt v_int;
  BulpFormatFloat v_float;
  BulpFormatStruct v_struct;
  BulpFormatUnion v_union;
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
BulpFormat *bulp_format_new_structure    (unsigned n_members,
                                          BulpStructMember *members,
                                          bulp_bool         is_message);
BulpFormat *bulp_format_new_struct       (unsigned n_members,
                                          BulpStructMember *members);
BulpFormat *bulp_format_new_message      (unsigned n_members,
                                          BulpStructMember *members);
#define bulp_format_new_struct(n,m)      bulp_format_new_structure(n,m,0)
#define bulp_format_new_message(n,m)     bulp_format_new_structure(n,m,1)
BulpFormat *bulp_format_new_union        (unsigned n_cases,
                                          BulpUnionCase    *cases);

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

BulpFormat * bulp_namespace_get_ascii       (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_string      (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_ascii0      (BulpNamespace *toplevel_ns);
BulpFormat * bulp_namespace_get_string0     (BulpNamespace *toplevel_ns);

BulpFormat * bulp_namespace_get_binary_data (BulpNamespace *toplevel_ns);

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
                                                const char    *dotted_name);
BulpNamespace *bulp_namespace_force_subnamespace_1(BulpNamespace *ns,
                                                ssize_t        name_len,
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

