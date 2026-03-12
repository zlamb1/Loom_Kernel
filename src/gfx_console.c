#include "loom/console.h"
#include "loom/font.h"

struct gfx_console
{
  struct console super;
  u8            *address;
  u32            x, y, w, h, stride, bpp, tw, th, lpad, tpad;
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
      auto x = (console->x + 4) / 4 * 4;
      if (x < console->tw)
        console->x = x;
      break;
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

  auto framebuffer = console->address;
  auto bpp = (console->bpp + 7) / 8;

  auto font = console->font;
  auto bpr = (font.glyph_width + 7) / 8;

  if (!console->tw || !console->th)
    return 0;

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
              auto color = 255 * ((pixels & 0x80) > 0);
              framebuffer[index] = color;
              framebuffer[index + 1] = color;
              framebuffer[index + 2] = color;
            }
        }

    next:
      advance (console);
    }

  __asm__ volatile ("sfence" ::: "memory");

  return count;
}

struct console *
early_gfx_console_create (u8 *address, u32 width, u32 height, u32 stride,
                          u32 bpp)
{
  if (address == null || !width || !height || !stride
      || (bpp != 24 && bpp != 32) || !console_font_get (&console.font))
    return null;

  console.super.write = gfx_console_write;
  console.super.data = &console;

  console.address = address;
  console.x = 0;
  console.y = 0;
  console.w = width;
  console.h = height;
  console.stride = stride;
  console.bpp = bpp;
  console.tw = width / console.font.glyph_width;
  console.th = height / console.font.glyph_height;
  console.lpad = (width % console.font.glyph_width) >> 1;
  console.tpad = (height % console.font.glyph_height) >> 1;

  return &console.super;
}