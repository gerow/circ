#include <circ.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_full() {
  int status = 0;

  struct circ c;
  if (circ_open(&c, 1 << 10) == -1) {
    perror("circ_open");
    return 1;
  }

  // Read a bit into it to make sure we test wrapping properly.
  circ_write(&c, 16);
  circ_read(&c, 16);

  char magic = 0xa5;
  size_t space = circ_space(&c);
  memset(circ_write_ptr(&c), magic, space);
  circ_write(&c, space);

  // Make sure cnt == prev space.
  size_t got = circ_cnt(&c);
  size_t want = space;
  if (got != want) {
    status = 1;
    fprintf(stderr, "circ_cnt wrong: got %ld want %ld\n", got, want);
  }
  // Make sure space == 0.
  got = circ_space(&c);
  want = 0;
  if (got != want) {
    status = 1;
    fprintf(stderr, "circ_space wrong: got %ld want %ld\n", got, want);
  }
  // Make sure a read-back contains the right values.
  int num_read = 0;
  while (circ_cnt(&c) > 0) {
    char got = *(char *)circ_read_ptr(&c);
    if (got != magic) {
      status = 1;
      fprintf(stderr, "circ_read_ptr: bad value at index %x, got %x want %x\n",
              num_read, got, magic);
    }
    num_read++;
    circ_read(&c, 1);
  }
  if (num_read != space) {
    status = 1;
    fprintf(stderr, "bad number of reads: got %d want %ld\n", num_read, space);
  }

  if (status) {
    fprintf(stderr, "FAILED test_full\n");
  } else {
    fprintf(stderr, "PASSED test_full\n");
  }
  return status;
}

int main(int argc, char **argv) {
  int status = 0;

  status |= test_full();

  exit(status);
}
