#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <vector>
namespace internal {
using std::abs;
extern "C" {
#include "rs_image.h"
}
}  // namespace internal

using internal::Dimensions;
using internal::Rgba;

#ifndef __IMAGE_LOAD_HPP
#define __IMAGE_LOAD_HPP

class image_error : public std::exception {
  internal::ErrorType er;

 public:
  image_error(internal::ErrorType er) : er(er) {};
  const char* what() const noexcept override {
    switch (er) {
      case internal::IMAGE_ERROR_NONE:
        return "No error";
      case internal::IMAGE_ERROR_DECODING:
        return "Decoding error";
      case internal::IMAGE_ERROR_ENCODING:
        return "Encoding error";
      case internal::IMAGE_ERROR_PARAMETER_DIMENSION_MISMATCH:
        return "The Image's dimensions are either too small or too large";
      case internal::IMAGE_ERROR_PARAMETER_FAILED_ALREADY:
        return "The end the image stream has been reached due to a previous "
               "error";
      case internal::IMAGE_ERROR_PARAMETER_MALFORMED:
        return "A parameter is malformed";
      case internal::IMAGE_ERROR_PARAMETER_NO_MORE_DATA:
        return "The end of the image has been reached";
      case internal::IMAGE_ERROR_INSUFFICIENT_MEMORY:
        return "Memory limit exceeded";
      case internal::IMAGE_ERROR_LIMITS_UNSUPPORTED:
        return "Image size exceeds limit";
      case internal::IMAGE_ERROR_DIMENSION_ERROR:
        return "Some strict limits are specified but not supported by the "
               "opertation.";
      case internal::IMAGE_ERROR_UNSUPPORTED_COLOR:
        return "The encoder or decoder for this image does not support the "
               "provided color type";
      case internal::IMAGE_ERROR_UNSUPPORTED_FORMAT:
        return "The image format is not supported";
      case internal::IMAGE_ERROR_UNSUPPORTED_OTHER:
        return "IMAGE_ERROR_UNSUPPORTED_OTHER";
      case internal::IMAGE_ERROR_IO_NOT_FOUND:
        return "IMAGE_ERROR_IO_NOT_FOUND";
        break;
      case internal::IMAGE_ERROR_IO_PERMISSION_DENIED:
        return "IMAGE_ERROR_IO_PERMISSION_DENIED";
        break;
      case internal::IMAGE_ERROR_IO_CONNECTION_REFUSED:
        return "IMAGE_ERROR_IO_CONNECTION_REFUSED";
        break;
      case internal::IMAGE_ERROR_IO_CONNECTION_RESET:
        return "IMAGE_ERROR_IO_CONNECTION_RESET";
        break;
      case internal::IMAGE_ERROR_IO_CONNECTION_ABORTED:
        return "IMAGE_ERROR_IO_CONNECTION_ABORTED";
        break;
      case internal::IMAGE_ERROR_IO_NOT_CONNECTED:
        return "IMAGE_ERROR_IO_NOT_CONNECTED";
        break;
      case internal::IMAGE_ERROR_IO_ADDR_IN_USE:
        return "IMAGE_ERROR_IO_ADDR_IN_USE";
        break;
      case internal::IMAGE_ERROR_IO_ADDR_NOT_AVALIABLE:
        return "IMAGE_ERROR_IO_ADDR_NOT_AVALIABLE";
        break;
      case internal::IMAGE_ERROR_IO_BROKEN_PIPE:
        return "IMAGE_ERROR_IO_BROKEN_PIPE";
        break;
      case internal::IMAGE_ERROR_IO_ALREADY_EXISTS:
        return "IMAGE_ERROR_IO_ALREADY_EXISTS";
        break;
      case internal::IMAGE_ERROR_IO_WOULD_BLOCK:
        return "IMAGE_ERROR_IO_WOULD_BLOCK";
        break;
      case internal::IMAGE_ERROR_IO_INVALID_INPUT:
        return "IMAGE_ERROR_IO_INVALID_INPUT";
        break;
      case internal::IMAGE_ERROR_IO_INVALID_DATA:
        return "IMAGE_ERROR_IO_INVALID_DATA";
        break;
      case internal::IMAGE_ERROR_IO_TIMED_OUT:
        return "IMAGE_ERROR_IO_TIMED_OUT";
        break;
      case internal::IMAGE_ERROR_IO_WRITE_ZERO:
        return "IMAGE_ERROR_IO_WRITE_ZERO";
        break;
      case internal::IMAGE_ERROR_IO_INTERRUPTED:
        return "IMAGE_ERROR_IO_INTERRUPTED";
        break;
      case internal::IMAGE_ERROR_IO_UNSUPPORED:
        return "IMAGE_ERROR_IO_UNSUPPORED";
        break;
      case internal::IMAGE_ERROR_IO_UNEXPECTED_EOF:
        return "IMAGE_ERROR_IO_UNEXPECTED_EOF";
        break;
      case internal::IMAGE_ERROR_IO_OUT_OF_MEMORY:
        return "IMAGE_ERROR_IO_OUT_OF_MEMORY";
        break;
      case internal::IMAGE_ERROR_IO_OTHER:
        return "IMAGE_ERROR_IO_OTHER";
        break;
      case internal::IMAGE_ERROR_UNKNOWN:
        return "IMAGE_ERROR_UNKNOWN";
        break;
    }
  }
};

class DynamicImage {
 private:
  internal::DynamicImage* img;

 public:
  DynamicImage(std::vector<char> data) {
    auto er = internal::dynamic_image_load_from_memory(
        (uint8_t*)(char*)data.data(), data.size());
    if (er.err != internal::IMAGE_ERROR_NONE) {
      throw new image_error(er.err);
    }
    this->img = er.res;
    if (er.res == NULL) {
      throw new std::runtime_error("it's fucking null dude idk");
    }
  };
  ~DynamicImage() { internal::dynamic_image_free(this->img); };
  Dimensions get_dimensions() {
    return internal::dynamic_image_dimensions(this->img);
  };
  Rgba get_pixel(uint32_t x, uint32_t y) {
    return internal::dynamic_image_get_pixel(this->img, x, y);
  };
  uint32_t width() {
    if (this->img == NULL) {
      throw new std::runtime_error("it's fucking null dude idk");
    };
    return internal::dynamic_image_width(this->img);
  };
  uint32_t height() { return internal::dynamic_image_height(this->img); };
  bool InBounds(uint32_t x, uint32_t y) {
    return internal::dynamic_image_in_bounds(this->img, x, y);
  };
};
#endif