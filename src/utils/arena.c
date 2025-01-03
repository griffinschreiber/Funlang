#include "arena.h"

#include <stdio.h>

#define ARENA_BLOCK_CAPACITY 1024
#define ARENA_MAX_BLOCKS 4

struct block {
  char data[ARENA_BLOCK_CAPACITY];
  size_t index;
  struct block *next;
};

// go nom nom on the heap.
struct block *make_block() {
  struct block *block = (struct block *)malloc(sizeof(struct block));
  block->index = 0;
  block->next = NULL;
  return block;
}

void free_block(struct arena *arena, struct block *block) {
  block->index = 0;
  block->next = arena->free;
  arena->free = block;
}

struct arena {
  struct block *active;
  struct block *free;
};

struct arena make_arena() {
  struct arena arena;
  arena.active = NULL;
  arena.free = make_block();
  return &arena;
}

void *arena_alloc(struct arena *arena, size_t alloc_size) {
  if (alloc_size > ARENA_BLOCK_CAPACITY) {
    fprintf(stderr, "arena_alloc: called with alloc_size bigger than ARENA_BLOCK_CAPACITY.");
    exit(1);
  }
  struct block *current = arena->free;
  while (ARENA_BLOCK_CAPACITY - current->index < alloc_size) {
    if (current->next == NULL) {
      current->next = make_block();
      return current->next;
    }
    current = current->next;
  }
  return current->data + current->index;
}

void arena_reset(struct arena *arena) {
  struct block *current = active;
  struct block *next;
  while (current) {
    next = current;
    free_block(current);
    current = next;
  }
}

void arena_free(struct arena *arena) {
  struct block *current = active;
  struct block *next;
  while (current) {
    next = current;
    free(current);
    current = next;
  }
}
