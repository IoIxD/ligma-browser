#ifndef __INTERNAL_IMAGE_LOAD_H
#define __INTERNAL_IMAGE_LOAD_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct __DynamicImage__ {
  void *image;
} __DynamicImage__;

typedef struct Dimensions {
  uint32_t width;
  uint32_t height;
} Dimensions;

typedef struct RGBA {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} RGBA;

struct __DynamicImage__ internal__Load__DynamicImage__FromMemory(uint8_t *data, uintptr_t len);

struct Dimensions internal__DynamicImage__Dimensions(struct __DynamicImage__ *this_);

struct RGBA internal__DynamicImage__GetPixel(struct __DynamicImage__ *this_,
                                             uint32_t x,
                                             uint32_t y);

uint32_t internal__DynamicImage__GetWidth(struct __DynamicImage__ *this_);

uint32_t internal__DynamicImage__GetHeight(struct __DynamicImage__ *this_);

bool internal__DynamicImage__InBounds(struct __DynamicImage__ *this_, uint32_t x, uint32_t y);

void internal__DynamicImage__Free(struct __DynamicImage__ *this_);

#endif  /* __INTERNAL_IMAGE_LOAD_H */
