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

bulp_bool bulp_util_writen (int fd,
                            size_t len,
                            const void *data,
                            BulpError**error)
{
  size_t rem = len;
  const uint8_t *at = data;
  while (rem > 0)
    {
      ssize_t writerv = write (fd, at, rem);
      if (writerv < 0)
        {
          if (errno == EINTR)
            continue;
          *error = bulp_error_new_file_write (errno);
          return BULP_FALSE;
        }
      rem -= writerv;
      at += writerv;
    }
  return BULP_TRUE;
}
BulpReadResult bulp_util_readn  (int fd,
                                 size_t len,
                                 void *data,
                                 BulpError**error)
{
  size_t rem = len;
  uint8_t *at = data;
  while (rem > 0)
    {
      ssize_t readrv = read (fd, at, rem);
      if (readrv < 0)
        {
          if (errno == EINTR)
            continue;
          *error = bulp_error_new_file_read (errno);
          return BULP_READ_RESULT_ERROR;
        }
      else if (readrv == 0)
        {
          if (rem != len)
            {
              *error = bulp_error_new_too_short ("reading from file descriptor %d", fd);
              return BULP_READ_RESULT_ERROR;
            }
          return BULP_READ_RESULT_EOF;
        }

      rem -= readrv;
      at += readrv;
    }
  return BULP_READ_RESULT_OK;
}

bulp_bool
bulp_util_pread (int fd, void *out, size_t amt, uint64_t offset, BulpError **error)
{
  size_t rem = amt;
  size_t off = offset;
  uint8_t *at = out;
  while (rem > 0)
    {
      ssize_t pread_rv = pread (fd, at, rem, off);
      if (pread_rv < 0)
        {
          if (errno == EINTR)
            continue;
          *error = bulp_error_new_file_read (errno);
          return BULP_FALSE;
        }
      else if (pread_rv == 0)
        {
          *error = bulp_error_new_too_short ("reading from file descriptor %d", fd);
          return BULP_FALSE;
        }
      else
        {
          rem -= pread_rv;
          at += pread_rv;
        }
    }
  return BULP_TRUE;
}

