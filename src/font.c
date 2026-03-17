#include "loom/font.h"
#include "loom/compiler.h"
#include "loom/math.h"
#include "loom/types.h"

extern byte _binary_consolefont_psfu_start;
extern byte _binary_consolefont_psfu_end;

struct psf2_font
{
#define PSF2_FONT_MAGIC 0x864ab572
  u32 magic;
  u32 version;
  u32 size;
  u32 flags;
  u32 glyph_count;
  u32 glyph_size;
  u32 glyph_height;
  u32 glyph_width;
} packed;

bool
consoleFontGet (struct font *f)
{
  byte *start = &_binary_consolefont_psfu_start;
  byte *end = &_binary_consolefont_psfu_end;

  if ((uintptr) start >= (uintptr) end)
    return false;

  auto blob_size = end - start;

  if (blob_size < sizeof (struct psf2_font))
    return false;

  auto header = (struct psf2_font *) start;
  u32  size;

  if (header->magic != PSF2_FONT_MAGIC
      || mul (header->glyph_count, header->glyph_size, &size)
      || add (size, header->size, &size) || blob_size < size)
    return false;

  auto expected_size = header->glyph_width;
  if (mul (expected_size, header->glyph_height, &expected_size)
      || header->glyph_size < expected_size / 8)
    return false;

  f->glyph_data = (u8 *) header + header->size;

  if (cast (header->glyph_count, &f->glyph_count)
      || cast (header->glyph_size, &f->glyph_size)
      || cast (header->glyph_width, &f->glyph_width)
      || cast (header->glyph_height, &f->glyph_height))
    return false;

  return true;
}