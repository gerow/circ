#include "circ.h"

#define _GNU_SOURCE

#include <sys/mman.h>
#include <unistd.h>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

int circ_open(struct circ *c, size_t size) {
  if (size < getpagesize()) {
    size = getpagesize();
  }
  if (unlikely(__builtin_popcount(size) != 1)) {
    // user provided a non-power-of-2 size, bail!
    return -1;
  }
  c->size = size;
  c->head = c->tail = 0;

  int rv = -1;
  c->buf = NULL;
  int fd = memfd_create("circ", 0);
  if (fd == -1) {
    goto out;
  }
  if (ftruncate(fd, size) == -1) {
    goto out;
  }

  // First create a PROT_NONE mapping twice the size of our buffer in order to
  // allocate the address space.
  c->buf = mmap(0, c->size * 2, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (unlikely(c->buf == MAP_FAILED)) {
    goto out;
  }
  // Now now that we have the entire address range reserved we can actually map
  // in our memfd to both the lower and upper halves.
  void *addr = mmap(c->buf, c->size, PROT_READ | PROT_WRITE,
                    MAP_FIXED | MAP_SHARED, fd, 0);
  if (unlikely(addr == MAP_FAILED)) {
    goto out;
  }
  addr = mmap(c->buf + c->size, c->size, PROT_READ | PROT_WRITE,
              MAP_FIXED | MAP_SHARED, fd, 0);
  if (unlikely(addr == MAP_FAILED)) {
    goto out;
  }

  rv = 0;
out:
  // Close the fd even on success since it's already mapped in.
  if (likely(fd != -1)) {
    close(fd);
  }
  if (unlikely(rv == -1) && c->buf != NULL && c->buf != MAP_FAILED) {
    munmap(c->buf, c->size * 2);
  }
  return rv;
}

int circ_close(struct circ *c) { return munmap(c->buf, c->size * 2); }
