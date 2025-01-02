#include "arena.h"

#define ARENA_BLOCK_DATA_SIZE 1024

struct block {
  char data[ARENA_BLOCK_DATA_SIZE];
  size_t index;
  struct block *next;
};

struct arena {
  struct block *active;
  struct block *free;
};

struct arena make_arena() {
  struct arena arena;
  arena.active = NULL;
  arena.free = (struct block *)malloc(sizeof(struct block));
  return &arena;
}

void free_block(struct arena *arena, struct block *block) {
  block->index = 0;
  block->next = arena->free;
  arena->free = block;
}
