#include "../bulp.h"

// Note:  TEST_ASSERT is unaffected by NDEBUG, and its assertion will be evaluated
// exactly once.
#define TEST_ASSERT(assertion) \
  if (!(assertion)) { bulp_die("assertion failed: %s (%s, line %u)", #assertion, __FILE__, __LINE__); }
    

