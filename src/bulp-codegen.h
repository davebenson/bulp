

typedef struct {
  int unused;
} BulpCodegenOptions;

void bulp_codegen (BulpNamespace *ns,
                   BulpCodegenOptions *options,
                   BulpBuffer *h_out,
                   BulpBuffer *c_out);

