#ifndef LOOM_FONT_H
#define LOOM_FONT_H 1

#include "loom/types.h"

struct font
{
  u8  *glyph_data;
  uint glyph_count;
  uint glyph_size;
  uint glyph_width;
  uint glyph_height;
};

bool consoleFontGet (struct font *f);

static inline u8 *
fontGlyphData (struct font *f, uint glyph)
{
  if (glyph >= f->glyph_count)
    return null;
  return f->glyph_data + glyph * f->glyph_size;
}

#endif