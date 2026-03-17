#ifndef LOOM_MMAP_H
#define LOOM_MMAP_H 1

#include "loom/types.h"

typedef int (*mmap_iterator_hook) (u64 start, u64 length, uint type,
                                   void *ctx);
typedef int (*mmap_iterator) (mmap_iterator_hook, void *);

struct memory_region
{
  u64 start;
  u64 length;
#define MEMORY_TYPE_FREE      0
#define MEMORY_TYPE_RESERVED  1
#define MEMORY_TYPE_BAD_RAM   2
#define MEMORY_TYPE_MAX_VALUE 2
  uint type;
};

struct mmap
{
#define MMAP_SIZE 128
  uint                 count;
  struct memory_region regions[MMAP_SIZE];
};

extern const char *memory_type_names[MEMORY_TYPE_MAX_VALUE + 1];
extern struct mmap mmap;

void mmapInit (mmap_iterator iterator);

#endif