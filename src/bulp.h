#ifndef __BULP_H_INCLUDED__
#define __BULP_H_INCLUDED__

typedef struct BulpError BulpError;

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>                     // for ssize_t
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>                     // used by some inline functions

#include "bulp.generated.h"
#include "bulp-defs.h"
#include "bulp-object.h"
#include "bulp-error.h"
#include "bulp-util.h"
#include "bulp-buffer.h"
#include "bulp-slab.h"
#include "bulp-mem-pool.h"
#include "bulp-data-builder.h"
#include "bulp-compression.h"

#include "bulp-format.h"      /* also includes namespace support */

#include "bulp-filter.h"
#include "bulp-mapper.h"
#include "bulp-reader.h"
#include "bulp-writer.h"
#include "bulp-file-format-burp.h"
#include "bulp-stream-api.h"

// utf8 and utf16 handling
#include "bulp-utf.h"

// mostly for machine-generated code, but occasionally useful directly for
// unpacking/packing fundamental formats.
#include "bulp-builtins-int.h"
#include "bulp-builtins-float.h"
#include "bulp-builtins-string.h"
#include "bulp-builtins-binary-data.h"

#include "bulp-indexer-defs.h"
#include "bulp-readonly-index.h"
#include "bulp-sorted-indexer.h"

// internals
#include "bulp-json-helpers.h"
#include "bulp-codegen.h"

#endif
