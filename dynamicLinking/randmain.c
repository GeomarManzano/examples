/*

Copyright 2015 Geomar Manzano

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#include "randcpuid.h"

int main(int argc, char **argv) {
  bool valid = false;
  long long nbytes;
  
  if (argc == 2) {
    char *endptr;
    errno = 0;
    nbytes = strtoll (argv[1], &endptr, 10);

    if (errno)
      perror(argv[1]);
    else
      valid = !*endptr && 0 <= nbytes;
  }

  if (!valid) {
    fprintf (stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
    return 1;
  }

  if (nbytes == 0)
    return 0;

  // Arrange to use the appropriate library.
  void* dl_handle;
   
  if (rdrand_supported())
    dl_handle = dlopen("randlibhw.so", RTLD_LAZY);
  else
    dl_handle = dlopen("randlibsw.so", RTLD_LAZY);

  if (!dl_handle) {
    printf("!!! %s\n", dlerror());
    return -1;
  }

  unsigned long long (*func)(void) = dlsym(dl_handle, "rand64");
  int wordsize = sizeof (*func)();
  char* error = dlerror();
   
  if (error != NULL) {
    printf("!!! %s\n", error);
    return -1;
  }

  int output_errno = 0;
  
  do {
    unsigned long long x = (*func)();
    size_t outbytes = nbytes < wordsize ? nbytes : wordsize;

    if (fwrite(&x, 1, outbytes, stdout) != outbytes) {
      output_errno = errno;
      break;
    }

    nbytes -= outbytes;
  } while (0 < nbytes);

  if (fclose(stdout) != 0)
    output_errno = errno;

  if (output_errno) {
    errno = output_errno;
    perror ("output");
    dlclose(dl_handle);
    return 1;
  }

  dlclose(dl_handle);
  return 0;
}
