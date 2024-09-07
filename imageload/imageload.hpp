#include <cstdint>
#include <vector>
#include "__internal__imageload.hpp"

#ifndef __IMAGE_LOAD_HPP
#define __IMAGE_LOAD_HPP

class DynamicImage {
 private:
  __DynamicImage__ img;

 public:
  DynamicImage(std::vector<char>);
  ~DynamicImage();
  Dimensions GetDimensions();
  RGBA GetPixel(uint32_t x, uint32_t y);
  uint32_t width();
  uint32_t height();
  bool InBounds(uint32_t x, uint32_t y);
};
#endif