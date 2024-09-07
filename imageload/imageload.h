#ifndef __INTERNAL_IMAGE_LOAD_H
#define __INTERNAL_IMAGE_LOAD_H

namespace __INTERNAL_DONT__FUCKING__USE {

struct __DynamicImage__ {
  void* image;
};

struct Dimensions {
  uint32_t width;
  uint32_t height;
};

struct RGBA {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

extern "C" {

__DynamicImage__ internal__Load__DynamicImage__FromMemory(uint8_t* data,
                                                          uintptr_t len);

Dimensions internal__DynamicImage__Dimensions(__DynamicImage__* this_);

RGBA internal__DynamicImage__GetPixel(__DynamicImage__* this_,
                                      uint32_t x,
                                      uint32_t y);

uint32_t internal__DynamicImage__GetWidth(__DynamicImage__* this_);

uint32_t internal__DynamicImage__GetHeight(__DynamicImage__* this_);

bool internal__DynamicImage__InBounds(__DynamicImage__* this_,
                                      uint32_t x,
                                      uint32_t y);

}  // extern "C"

}  // namespace __INTERNAL_DONT__FUCKING__USE

#endif  // __INTERNAL_IMAGE_LOAD_H
