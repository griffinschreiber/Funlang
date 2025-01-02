#ifndef ARENA_H_
#define ARENA_H_

#include <stdlib.h>

struct arena;

struct arena *make_arena();

void *arena_alloc(size_t alloc_size);

void reset_arena(struct arena *arena);

void free_arena(struct arena *arena);

#endif // ARENA_H_
