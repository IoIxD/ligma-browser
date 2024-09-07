#include "imageload.hpp"

DynamicImage::DynamicImage(std::vector<char> data) {
  this->img = internal__Load__DynamicImage__FromMemory((uint8_t*)data.data(),
                                                       data.size());
};
Dimensions DynamicImage::GetDimensions() {
  return internal__DynamicImage__Dimensions(&this->img);
};
RGBA DynamicImage::GetPixel(uint32_t x, uint32_t y) {
  return internal__DynamicImage__GetPixel(&this->img, x, y);
};
uint32_t DynamicImage::width() {
  return internal__DynamicImage__GetWidth(&this->img);
};
uint32_t DynamicImage::height() {
  return internal__DynamicImage__GetHeight(&this->img);
};
bool DynamicImage::InBounds(uint32_t x, uint32_t y) {
  return internal__DynamicImage__InBounds(&this->img, x, y);
};
DynamicImage::~DynamicImage() {
  internal__DynamicImage__Free(&this->img);
}