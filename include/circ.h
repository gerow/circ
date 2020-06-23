#pragma once

#include <stddef.h>

struct circ {
  size_t size;
  void *buf;
  int head;
  int tail;
};

// Creation/destruction.
int circ_open(struct circ *c, size_t size);
int circ_close(struct circ *c);

// Reading funcs.
inline size_t circ_cnt(struct circ *c) {
  return (c->head - c->tail) & (c->size - 1);
}

inline void *circ_read_ptr(struct circ *c) { return c->buf + c->tail; }

inline void circ_read(struct circ *c, size_t n) {
  c->tail = (c->tail + n) & (c->size - 1);
}

// Writing funcs.
inline size_t circ_space(struct circ *c) {
  return (c->tail - c->head + 1) & (c->size - 1);
}

inline void *circ_write_ptr(struct circ *c) { return c->buf + c->head; }

inline void circ_write(struct circ *c, size_t n) {
  c->head = (c->head + n) & (c->size - 1);
}

inline void circ_reset(struct circ *c) { c->head = c->tail = 0; }
