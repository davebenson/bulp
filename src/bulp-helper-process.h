

typedef struct BulpHelperProcess BulpHelperProcess;

BulpHelperProcess * bulp_helper_process_launch (void);


typedef enum
{
  BULP_HELPER_PROCESS_SUCCESS,
  BULP_HELPER_PROCESS_ERROR,
  BULP_HELPER_PROCESS_CANCELLED,
} BulpHelperProcessResult;
typedef void (*BulpHelperProcessCallback) 
                      (BulpHelperProcessResult result,
                       BulpError              *error,       // usually NULL
                       void                   *callback_data);

#if 0
/* higher levels are concatenated */
void
bulp_helper_process_concat_sorted_levels(BulpHelperProcess *hp,
                                         const char        *filename_prefix,
                                         unsigned           n_levels,
                                         BulpHelperProcessCallback callback,
                                         void              *callback_data);
#endif

void
bulp_helper_process_delete_sorted_index (BulpHelperProcess *hp,
                                         const char        *filename_prefix,
                                         unsigned           n_levels,
                                         BulpHelperProcessCallback callback,
                                         void              *callback_data);

void
bulp_helper_process_destroy             (BulpHelperProcess *hp);

#if 0
bulp_bool
bulp_helper_foreground_concat_sorted_levels(BulpHelperProcess *hp,
                                            const char        *filename_prefix,
                                            unsigned           n_levels,
                                            BulpError        **error);
#endif

bulp_bool
bulp_helper_foreground_delete_sorted_index (BulpHelperProcess *hp,
                                            const char        *filename_prefix,
                                            unsigned           n_levels,
                                            BulpError        **error);


