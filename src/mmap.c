#include "loom/mmap.h"
#include "loom/math.h"
#include "loom/print.h"

#define EVTS_SIZE (MMAP_SIZE * 2)

struct mem_event
{
  u64 start;
  u32 memory_type;
#define MEM_EVENT_START 0
#define MEM_EVENT_END   1
  u32 type;
};

struct evt_hook_ctx
{
  uint              count;
  struct mem_event *evts;
};

extern char __phys_base;
extern char __phys_end;

const char *memory_type_names[MEMORY_TYPE_MAX_VALUE + 1] = {
  [MEMORY_TYPE_FREE] = "Free",
  [MEMORY_TYPE_RESERVED] = "Reserved",
  [MEMORY_TYPE_BAD_RAM] = "Bad Ram",
};

struct mmap mmap = { 0 };

static int
_build_mmap (uint evt_count, struct mem_event *evts)
{
  u64  pos = 0;
  int  primary_type = -1;
  uint active_types[MEMORY_TYPE_MAX_VALUE + 1] = { 0 };

  for (uint i = 0; i < evt_count; ++i)
    for (uint j = i + 1; j < evt_count; ++j)
      {
        auto evt1 = evts[i];
        auto evt2 = evts[j];
        if (evt1.start > evt2.start
            || (evt1.start == evt2.start && evt1.type > evt2.type))
          {
            // Sort by address then by type (start events come first).
            evts[i] = evt2;
            evts[j] = evt1;
          }
      }

  mmap.count = 0;

  for (uint i = 0; i < evt_count; ++i)
    {
      auto evt = evts[i];
      int  strictest = -1;

      if (evt.memory_type > MEMORY_TYPE_MAX_VALUE)
        return -1;

      if (evt.type == MEM_EVENT_START)
        {
          if (primary_type < 0 || primary_type < evt.memory_type)
            {
              auto length = evt.start - pos;

              if (primary_type > -1 && length)
                {
                  if (mmap.count >= MMAP_SIZE)
                    return -1;

                  mmap.regions[mmap.count++] = (struct memory_region) {
                    .start = pos,
                    .length = length,
                    .type = primary_type,
                  };
                }

              pos = evt.start;
              primary_type = evt.memory_type;
            }

          active_types[evt.memory_type] += 1;

          continue;
        }

      if (!active_types[evt.memory_type])
        return -1;

      active_types[evt.memory_type] -= 1;

      if (primary_type == evt.memory_type && !active_types[primary_type])
        {
          for (int j = MEMORY_TYPE_MAX_VALUE; j >= 0; --j)
            if (active_types[j])
              {
                strictest = j;
                break;
              }
        }
      else
        strictest = primary_type;

      if (strictest < primary_type)
        {
          auto length = evt.start - pos;

          if (length)
            {
              if (mmap.count >= MMAP_SIZE)
                return -1;

              mmap.regions[mmap.count++] = (struct memory_region) {
                .start = pos,
                .length = length,
                .type = primary_type,
              };
            }

          pos = evt.start;
          primary_type = strictest;
        }
    }

  return 0;
}

int
mmap_evt_hook (u64 start, u64 length, uint type, void *p)
{
  auto ctx = (struct evt_hook_ctx *) p;

  if (ctx->count >= EVTS_SIZE - 1)
    return -1;

  if (!length)
    return 0;

  switch (type)
    {
    case MEMORY_TYPE_FREE:
    case MEMORY_TYPE_BAD_RAM:
      break;
    default:
      type = MEMORY_TYPE_RESERVED;
      break;
    }

  ctx->evts[ctx->count++] = (struct mem_event) {
    .start = start,
    .memory_type = type,
    .type = MEM_EVENT_START,
  };

  u64 end;
  if (add (start, length, &end))
    end = UINT64_MAX;

  ctx->evts[ctx->count++] = (struct mem_event) {
    .start = end,
    .memory_type = type,
    .type = MEM_EVENT_END,
  };

  return 0;
}

void
mmap_init (mmap_iterator iterator)
{
  struct mem_event    evts[EVTS_SIZE];
  struct evt_hook_ctx ctx = { .evts = evts };

  auto phys_base = (uintptr) &__phys_base;
  auto phys_end = (uintptr) &__phys_end;

  if (mmap_evt_hook (phys_base, phys_end - phys_base, MEMORY_TYPE_RESERVED,
                     &ctx))
    goto fail;

  if (iterator (mmap_evt_hook, &ctx) || _build_mmap (ctx.count, evts))
    goto fail;

  for (uint i = 0; i < mmap.count; ++i)
    {
      auto region = mmap.regions[i];
      kprintfln ("Memory Region: %#.16x64 -> %#-.16x64 [%s]", region.start,
                 region.start + region.length, memory_type_names[region.type]);
    }

  return;

fail:
  kprintfln ("Failed to build memory map. Too many entries.");
  return;
}