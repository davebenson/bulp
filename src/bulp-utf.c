#include "bulp.h"

bulp_bool
bulp_utf8_validate      (size_t max_bytes,
                         const uint8_t *bytes,
                         BulpError **error)
{
  for (size_t at = 0; at < max_bytes; )
    {
      if ((bytes[at] & 0x80) == 0)
        {
          do {
            at++;
          } while ((bytes[at] & 0x80) == 0 && at < max_bytes);
        }
      else
        {
          unsigned n = bulp_utf8_validate_char (max_bytes - at, bytes + at, error);
          if (n == 0)
            return BULP_FALSE;
          at += n;
        }
    }
  return BULP_TRUE;
}


bulp_bool
bulp_utf8_validate_nonnul  (size_t max_bytes,
                            const uint8_t *bytes,
                            BulpError **error)
{
  for (size_t at = 0; at < max_bytes; )
    {
      if ((bytes[at] & 0x80) == 0)
        {
          do {
            if (bytes[at] == 0)
              {
                *error = bulp_error_new_unexpected_nul ();
                return BULP_FALSE;
              }
            at++;
          } while ((bytes[at] & 0x80) == 0 && at < max_bytes);
        }
      else
        {
          unsigned n = bulp_utf8_validate_char (max_bytes - at, bytes + at, error);
          if (n == 0)
            return BULP_FALSE;
          at += n;
        }
    }
  return BULP_TRUE;
}

