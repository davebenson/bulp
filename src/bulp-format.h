
typedef union BulpFormat BulpFormat;
typedef struct BulpNamespace BulpNamespace;

typedef struct {
  BulpFormatType type;
  BulpFormatVTable *vtable;
  unsigned ref_count;
  BulpNamespace *canonical_ns;
  const char *canonical_name;
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
  unsigned max_byte_size;       /* 1,2,4,8 */
  bulp_bool is_signed;
  bulp_bool is_b128;               /* variable-length encoded; otherwise, fixed-length little-endian */
} BulpFormatInt;

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
  const char *name;
  unsigned n_bits;
} BulpPackedElement;

struct BulpFormatPacked {
  BulpFormatBase base;
  unsigned n_elements;
  BulpFormatPackedElement *elements;
};

typedef struct {
  const char *name;
  BulpFormat *case_format;
} BulpUnionCase;

struct BulpFormatUnion {
  BulpFormatBase base;
  size_t n_cases;
  BulpUnionCase *cases;
};

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

typedef struct {
  const char *name;
  BulpFormat *format;
  BulpVersioningInfo *versioning_info;
} BulpStructMember;
#define BULP_STRUCT_MEMBER_IS_EXTANT(member) \
  ((member)->versioning_info == NULL || !(member)->versioning_info->ignored)

struct BulpFormatStruct {
  BulpFormatBase base;
  size_t n_members;
  BulpStructMember *members;
};

union BulpFormat {
  BulpStringType type;
  BulpFormatBase base;
  BulpFormatBinaryData v_binary_data;
  BulpFormatPacked v_packed;
  BulpFormatString v_string;
  BulpFormatInt v_int;
  BulpFormatBits v_bits;
  BulpFormatStruct v_struct;
  BulpFormatUnion v_union;
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

BulpFormat *bulp_format_new_struct       (unsigned n_members,
                                          BulpStructMember *members);
BulpFormat *bulp_format_new_union        (unsigned n_cases,
                                          BulpUnionCase    *cases);


#define bulp_format_short       ((BulpFormat *) (&bulp_format_ints_global[0]))
#define bulp_format_int         ((BulpFormat *) (&bulp_format_ints_global[1]))
#define bulp_format_long        ((BulpFormat *) (&bulp_format_ints_global[2]))
#define bulp_format_ushort      ((BulpFormat *) (&bulp_format_ints_global[3]))
#define bulp_format_uint        ((BulpFormat *) (&bulp_format_ints_global[4]))
#define bulp_format_ulong       ((BulpFormat *) (&bulp_format_ints_global[5]))
#define bulp_format_int8        ((BulpFormat *) (&bulp_format_ints_global[6]))
#define bulp_format_int16       ((BulpFormat *) (&bulp_format_ints_global[7]))
#define bulp_format_int32       ((BulpFormat *) (&bulp_format_ints_global[8]))
#define bulp_format_int64       ((BulpFormat *) (&bulp_format_ints_global[9]))
#define bulp_format_uint8       ((BulpFormat *) (&bulp_format_ints_global[10]))
#define bulp_format_uint16      ((BulpFormat *) (&bulp_format_ints_global[11]))
#define bulp_format_uint32      ((BulpFormat *) (&bulp_format_ints_global[12]))
#define bulp_format_uint64      ((BulpFormat *) (&bulp_format_ints_global[13]))

#define bulp_format_byte        bulp_format_uint8

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
                                                const char    *name,
                                                BulpFormat    *format,
                                                bulp_bool      is_canonical_ns);

bulp_bool      bulp_namespace_query_1          (BulpNamespace *ns,
                                                const char    *name,
                                                BulpNamespaceQueryResult *out);
bulp_bool      bulp_namespace_query            (BulpNamespace *ns,
                                                const char    *dotted_name,
                                                BulpNamespaceQueryResult *out);

typedef bulp_bool (*BulpNamespaceForeachFunc) (BulpNamespace *ns,
                                               const char *name,
                                               BulpNamespaceQueryResult *value,
                                               void       *data);
bulp_bool      bulp_namespace_foreach          (BulpNamespace *ns,
                                                BulpNamespaceForeachFunc func,
                                                void func_data);

bulp_bool      bulp_namespace_parse_file       (BulpNamespace *ns,
                                                const char    *filename,
                                                BulpError    **error);


// private
extern BulpFormatInt bulp_format_ints_global[14];
extern BulpFormatString bulp_format_strings_global[4];
extern BulpFormatBinaryData bulp_format_binary_data_global;
