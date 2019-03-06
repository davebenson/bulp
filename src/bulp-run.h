

typedef enum
{
  BULP_PIPE_RESULT_SUCCESS,
  BULP_PIPE_RESULT_ERROR_READING,
  BULP_PIPE_RESULT_ERROR_WRITING,
} BulpPipeResult;

BulpPipeResult bulp_run_simple_pipeline (BulpReader *reader,
                                         BulpWriter *writer,
                                         BulpError **error);

