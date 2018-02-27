#include "bulp.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void
bulp_die (const char *format, ...)
{
  va_list args;
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  fprintf(stderr, "\n");
  abort();
}

void
bulp_warn (const char *format, ...)
{
  fprintf(stderr, "*** WARNING *** ");
  va_list args;
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  fprintf(stderr, "\n");
  abort();
}

uint8_t *bulp_util_file_load (const char *filename, 
                              size_t     *length_out,
                              BulpError **error)
{
  int fd = open (filename, O_RDONLY);
  if (fd < 0)
    {
      int e = errno;
      if (e == ENOENT)
        *error = bulp_error_new_file_not_found (filename);
      else
        *error = bulp_error_new_file_open_error (filename, e);
      BULP_ERROR_SET_C_LOCATION (*error);
      return NULL;
    }
  struct stat stat_buf;
  if (fstat (fd, &stat_buf) < 0)
    {
      *error = bulp_error_new_stat_failed (filename, errno);
      BULP_ERROR_SET_C_LOCATION (*error);
      return NULL;
    }
  uint8_t *out = malloc (stat_buf.st_size == 0 ? 1 : stat_buf.st_size);
  if (out == NULL)
    {
      *error = bulp_error_out_of_memory ();
      BULP_ERROR_SET_C_LOCATION (*error);
      close (fd);
      return NULL;
    }
  ssize_t nread = 0;
  size_t total = stat_buf.st_size;
  size_t amt = 0;
  while (amt < total && (nread = read (fd, out + amt, total - amt)) > 0)
    {
      amt += nread;
    }
  if (nread < 0)
    {
      *error = bulp_error_new_file_read (errno);
      BULP_ERROR_SET_C_LOCATION (*error);
      free (out);
      close (fd);
      return NULL;
    }
  if (amt < total)
    {
      *error = bulp_error_new_premature_eof (filename, "reading to end of file, despite stat result");
      BULP_ERROR_SET_C_LOCATION (*error);
      free (out);
      close (fd);
      return NULL;
    }
  close (fd);
  *length_out = total;
  return out;
}
