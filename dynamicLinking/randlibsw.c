#include <stdio.h>
#include <stdlib.h>
#include "randlib.h"

// Input stream containing random bytes.
static FILE *urandstream;

// Initialize the software rand64 implementation.
__attribute__ ((constructor))
static void software_rand64_init (void) {
  urandstream = fopen("/dev/urandom", "r");
  if (! urandstream)
    abort();
}

// Finalize the software rand64 implementation.
__attribute__ ((destructor))
static void software_rand64_fini (void) {
  fclose(urandstream);
}

// Return a random value, using software operations.
extern unsigned long long rand64(void) {
  unsigned long long int x;
  if (fread (&x, sizeof x, 1, urandstream) != 1)
    abort();
  return x;
}
