#include "loom/compiler.h"
#include "loom/console.h"
#include "loom/font.h"
#include "loom/types.h"

struct gfx_console
{
  struct console super;
  u8            *address;
  u32            x, y, w, h, stride, bpp, tw, th, lpad, tpad;
  u8             red_mask_size, red_mask_shift;
  u8             green_mask_size, green_mask_shift;
  u8             blue_mask_size, blue_mask_shift;
  struct font    font;
};

static struct gfx_console console;

static inline void force_inline
advance_line (struct gfx_console *console)
{
  if (++console->y >= console->th)
    {
      // TODO: Scroll!
      console->y = 0;
    }
}

static inline void force_inline
advance (struct gfx_console *console)
{
  if (++console->x >= console->tw)
    {
      console->x = 0;
      advance_line (console);
    }
}

static inline bool force_inline
control_codes (struct gfx_console *console, byte ch)
{
  switch (ch)
    {
    case '\b':
      if (console->x)
        --console->x;
      else if (console->y)
        {
          console->x = console->tw - 1;
          --console->y;
        }
      break;
    case '\t':
      {
        auto x = (console->x + 4) / 4 * 4;
        if (x < console->tw)
          console->x = x;
        break;
      }
    case '\r':
      console->x = 0;
      break;
    case '\n':
      console->x = 0;
      advance_line (console);
      break;
    default:
      return false;
    }

  return true;
}

static uint
gfx_console_write (void *data, uint n, const char *s)
{
  struct gfx_console *console = data;

  uint count = 0;
  byte b;

  auto fb = console->address;
  auto bpp = (console->bpp + 7) / 8;

  auto font = console->font;
  auto bpr = (font.glyph_width + 7) / 8;

  if (!console->tw || !console->th)
    return 0;

  bool aligned = console->bpp == 32 && !(((uintptr) fb) & 3);

  byte rgb[3] = { 0, 225, 225 };

  u32 bg = 0;
  u32 fg = (rgb[0] << console->red_mask_shift)
           | (rgb[1] << console->green_mask_shift)
           | (rgb[2] << console->blue_mask_shift);

  while (count < n)
    {
      b = s[count++];

      if (control_codes (console, b))
        continue;

      auto x = console->x * font.glyph_width + console->lpad;
      auto y = console->y * font.glyph_height + console->tpad;
      byte pixels;

      u8 *glyph_data = font_glyph_data (&font, b);
      if (glyph_data == null)
        goto next;

      for (u32 gy = 0; gy < font.glyph_height; ++gy)
        {
          auto index = (y + gy) * console->stride + x * bpp;
          for (u32 gx = 0; gx < font.glyph_width;
               ++gx, pixels <<= 1, index += bpp)
            {
              if ((gx & 7) == 0)
                pixels = glyph_data[gy * bpr + (gx >> 3)];
              auto color = (pixels & 0x80) > 0 ? fg : bg;
              if (aligned)
                {
                  auto pixel = (u32 *) (fb + index);
                  *pixel = color;
                }
              else
                {
                  fb[index] = color;
                  fb[index + 1] = color >> 8;
                  fb[index + 2] = color >> 16;
                }
            }
        }

    next:
      advance (console);
    }

  __asm__ volatile ("sfence" ::: "memory");

  return count;
}

static inline bool force_inline
validate_mask (u8 size, u8 shift, u32 bpp)
{
  return size == 8 && (shift % 8) == 0 && shift < bpp;
}

struct console *
early_gfx_console_create (struct fb_desc desc)
{
  if (desc.address == null || !desc.width || !desc.height || !desc.stride
      || (desc.bpp != 24 && desc.bpp != 32)
      || !validate_mask (desc.red_mask_size, desc.red_mask_shift, desc.bpp)
      || !validate_mask (desc.green_mask_size, desc.green_mask_shift, desc.bpp)
      || !validate_mask (desc.blue_mask_size, desc.blue_mask_shift, desc.bpp)
      || !console_font_get (&console.font))
    return null;

  console.super.write = gfx_console_write;
  console.super.data = &console;

  console.address = desc.address;
  console.x = 0;
  console.y = 0;
  console.w = desc.width;
  console.h = desc.height;
  console.stride = desc.stride;
  console.bpp = desc.bpp;
  console.tw = desc.width / console.font.glyph_width;
  console.th = desc.height / console.font.glyph_height;
  console.lpad = (desc.width % console.font.glyph_width) >> 1;
  console.tpad = (desc.height % console.font.glyph_height) >> 1;

  console.red_mask_size = desc.red_mask_size;
  console.red_mask_shift = desc.red_mask_shift;
  console.green_mask_size = desc.green_mask_size;
  console.green_mask_shift = desc.green_mask_shift;
  console.blue_mask_size = desc.blue_mask_size;
  console.blue_mask_shift = desc.blue_mask_shift;

  return &console.super;
}