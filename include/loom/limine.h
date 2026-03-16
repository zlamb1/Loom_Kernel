#include "loom/console.h"
#ifndef LOOM_LIMINE_H
#define LOOM_LIMINE_H 1

#include "loom/boot_info.h"

#define LIMINE_REQUESTS_START_MARKER                                          \
  { 0xf6b8f4b39de7d1ae, 0xfab91a6940fcb9cf, 0x785c6ed015d3e316,               \
    0x181e920a7852b9d9 }

#define LIMINE_COMMON_MAGIC 0xc7b1dd30df4c8b88, 0x0a82e883a194f07b

#define LIMINE_HHDM_REQUEST_ID                                                \
  { LIMINE_COMMON_MAGIC, 0x48dcf1cb8ad2b852, 0x63984e959a98244b }

#define LIMINE_FRAMEBUFFER_REQUEST_ID                                         \
  { LIMINE_COMMON_MAGIC, 0x9d5827dcd881dd75, 0xa3148604f6fab11b }

#define LIMINE_MEMMAP_REQUEST_ID                                              \
  { LIMINE_COMMON_MAGIC, 0x67cf3d9d378a806f, 0xe304acdfc50c3c62 }

#define LIMINE_DATE_AT_BOOT_REQUEST_ID                                        \
  { LIMINE_COMMON_MAGIC, 0x502746e184c088aa, 0xfbc5ec83e6327893 }

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

struct limine_memmap_request
{
  u64                            id[4];
  u64                            revision;
  struct limine_memmap_response *response;
};

struct limine_memmap_response
{
  u64                          revision;
  u64                          entry_count;
  struct limine_memmap_entry **entries;
};

struct limine_memmap_entry
{
  u64 base;
  u64 length;
#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_EXECUTABLE_AND_MODULES 6
#define LIMINE_MEMMAP_FRAMEBUFFER            7
#define LIMINE_MEMMAP_RESERVED_MAPPED        8
  u64 type;
};

struct limine_date_at_boot_request
{
  u64                                  id[4];
  u64                                  revision;
  struct limine_date_at_boot_response *response;
};

struct limine_date_at_boot_response
{
  u64 revision;
  i64 timestamp;
};

bool limine_get_framebuffers (u64                         *framebuffer_count,
                              struct limine_framebuffer ***framebuffers);

static inline struct console *
early_limine_gfx_console_create (struct limine_framebuffer *fb)
{
  struct fb_desc desc = {
    .address = fb->address,
    .width = fb->width,
    .height = fb->height,
    .stride = fb->pitch,
    .bpp = fb->bpp,
    .red_mask_size = fb->red_mask_size,
    .red_mask_shift = fb->red_mask_shift,
    .green_mask_size = fb->green_mask_size,
    .green_mask_shift = fb->green_mask_shift,
    .blue_mask_size = fb->blue_mask_size,
    .blue_mask_shift = fb->blue_mask_shift,
  };

  return early_gfx_console_create (desc);
}

bool limine_get_boot_time (timestamp *ts);

struct boot_info limine_early_boot (void);

#endif