#ifndef LOOM_LIMINE_H
#define LOOM_LIMINE_H 1

#include "loom/types.h"

#define LIMINE_REQUESTS_START_MARKER                                          \
  { 0xf6b8f4b39de7d1ae, 0xfab91a6940fcb9cf, 0x785c6ed015d3e316,               \
    0x181e920a7852b9d9 }

#define LIMINE_COMMON_MAGIC 0xc7b1dd30df4c8b88, 0x0a82e883a194f07b

#define LIMINE_HHDM_REQUEST_ID                                                \
  { LIMINE_COMMON_MAGIC, 0x48dcf1cb8ad2b852, 0x63984e959a98244b }

#define LIMINE_FRAMEBUFFER_REQUEST_ID                                         \
  { LIMINE_COMMON_MAGIC, 0x9d5827dcd881dd75, 0xa3148604f6fab11b }

#define LIMINE_REQUESTS_END_MARKER { 0xadc0e0531bb10d03, 0x9572709f31764c62 }

#define LIMINE_REQUEST                                                        \
  u64 id[4];                                                                  \
  u64 revision;

struct limine_hhdm_request
{
  LIMINE_REQUEST
  struct limine_hhdm_response *response;
};

struct limine_hhdm_response
{
  u64 revision;
  u64 offset;
};

struct limine_framebuffer_request
{
  LIMINE_REQUEST
  struct limine_framebuffer_response *response;
};

struct limine_framebuffer_response
{
  u64                         revision;
  u64                         framebuffer_count;
  struct limine_framebuffer **framebuffers;
};

struct limine_framebuffer
{
  void *address;
  u64   width;
  u64   height;
  u64   pitch;
  u16   bpp;
#define LIMINE_FRAMEBUFFER_RGB 1
  u8    memory_model;
  u8    red_mask_size;
  u8    red_mask_shift;
  u8    green_mask_size;
  u8    green_mask_shift;
  u8    blue_mask_size;
  u8    blue_mask_shift;
  u8    unused[7];
  u64   edid_size;
  void *edid;

  u64                        mode_count;
  struct limine_video_mode **modes;
};

struct limine_video_mode
{
  u64 pitch;
  u64 width;
  u64 height;
  u16 bpp;
  u8  memory_model;
  u8  red_mask_size;
  u8  red_mask_shift;
  u8  green_mask_size;
  u8  green_mask_shift;
  u8  blue_mask_size;
  u8  blue_mask_shift;
};

bool limine_get_framebuffers (u64                         *framebuffer_count,
                              struct limine_framebuffer ***framebuffers);

#endif