/*
 * Copyright (c) 2023 Ian Marco Moffett and the VegaOS team.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of VegaOS nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <dev/video/fb.h>
#include <sys/limine.h>
#include <sys/pal.h>

#define FRAMEBUFFER framebuffer_req.response->framebuffers[0]

static volatile struct limine_framebuffer_request framebuffer_req = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

/*
 *  Does not continue if
 *  a framebuffer is not available.
 *
 *  TODO: Continue but just without
 *        using the framebuffer.
 */

static void fb_assert(void)
{
  if (framebuffer_req.response == NULL)
  {
    global_irq_disable();
    halt();
  }
}

static uint32_t get_index(uint32_t x, uint32_t y)
{
  fb_assert();
  return x + y * (FRAMEBUFFER->pitch/4);
}

int fb_putpix(uint32_t x, uint32_t y, uint32_t color)
{
  if (x >= fb_get_width() || y >= fb_get_height())
  {
    return 1;
  }
  
  uint32_t *fb = (uint32_t *)FRAMEBUFFER->address;
  fb[get_index(x, y)] = color;
  return 0;
}

void fb_clear(uint32_t color)
{
  for (uint32_t y = 0; y < fb_get_height(); ++y)
  {
    for (uint32_t x = 0; x < fb_get_width(); ++x)
    {
      fb_putpix(x, y, color);
    }
  }
}

uint32_t fb_get_height(void)
{
  return FRAMEBUFFER->height;
}

uint32_t fb_get_width(void)
{
  return FRAMEBUFFER->width;
}
