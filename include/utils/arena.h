#ifndef ARENA_H_
#define ARENA_H_

#include <stdlib.h>

struct arena;

struct arena *make_arena();

void *arena_alloc(struct arena *arena, size_t alloc_size);

void arena_reset(struct arena *arena);

void arena_free(struct arena *arena);

#endif // ARENA_H_
