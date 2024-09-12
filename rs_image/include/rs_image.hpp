#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <functional>
#include <vector>

namespace rs_image {
namespace internal {
using std::abs;
extern "C" {
#include "rs_image.h"
}
}  // namespace internal

using internal::ColorType;
using internal::Dimensions;
using internal::Ordering;
using internal::PixelResult;
using internal::Rgba;
using internal::SizeHint;

class image_error : public std::exception {
  internal::ErrorType er;

 public:
  image_error(internal::ErrorType er) : er(er) {};
  const char* what() const noexcept override;
};

template <typename T>
class Iterator {
  internal::RawIterator* inner;

 public:
  Iterator(internal::RawIterator* inner) : inner() {};

  SizeHint size_hint();
  uintptr_t count();
  Iterator step_by(uintptr_t step);
  Iterator chain(Iterator other);
  Iterator zip(Iterator other);
  Iterator map(std::function<void(T)>);
  void for_each(std::function<void(T)>);
  Iterator filter(std::function<bool(T)>);
  Iterator filter_map(std::function<Iterator*(T)>);
  Iterator enumerate();
  Iterator peekable();
  Iterator skip_while(std::function<bool(T)>);
  Iterator take_while(std::function<bool(T)>);
  Iterator map_while(std::function<Iterator*(T)>);
  Iterator skip(uintptr_t n);
  Iterator take(uintptr_t n);
  Iterator scan(void* initial_state, std::function<void*(void*, T)>);
  Iterator flat_map(std::function<Iterator(T)>);
  Iterator fuse();
  Iterator inspect(std::function<void(T)>);
  Iterator by_ref();
  bool all(std::function<bool(T)>);
  bool any(std::function<bool(T)>);
  Ordering cmp(Iterator other);
  bool eq(Iterator other);
  bool ne(Iterator other);
  bool lt(Iterator other);
  bool le(Iterator other);
  bool gt(Iterator other);
  bool ge(Iterator other);
  T next();
  T last();
  T nth(uintptr_t n);
  T fold(T init, std::function<T(T, T)>);
  T reduce(std::function<T(T, T)>);
  T find(std::function<bool(T*)>);
  T find_map(std::function<T(T)>);
  size_t* position(std::function<bool(T*)>);
  T max();
  T min();
  Ordering* partial_cmp(Iterator* other);
  std::vector<T> collect();

  /*T* max_by_key(std::function<void*(void*)>);
  T* max_by(std::function<Ordering(void*, void*)>);
  T* min_by_key(std::function<void*(void*)>);
  T* min_by(std::function<Ordering(void*, void*)>);*/
};

class DynamicImage {
 private:
  internal::DynamicImage* img;
  DynamicImage(internal::DynamicImage* img) : img(img) {};

 public:
  uint32_t width();

  DynamicImage(std::vector<char> data);
  ~DynamicImage();

  // DynamicImage* from_decoder();
  uint8_t* as_bytes(size_t* count);
  DynamicImage* blur(float sigma);
  DynamicImage* brighten(float value);
  void invert();
  ColorType color();
  DynamicImage* fliph();
  DynamicImage* flipv();
  DynamicImage* grayscale();
  uint32_t height();
  DynamicImage* rotate180();
  DynamicImage* rotate270();
  DynamicImage* rotate90();
  DynamicImage* into_luma16();
  DynamicImage* into_luma8();
  DynamicImage* into_luma_alpha16();
  DynamicImage* into_luma_alpha8();
  DynamicImage* into_rgb16();
  DynamicImage* into_rgb32f();
  DynamicImage* into_rgb8();
  DynamicImage* into_rgba16();
  DynamicImage* into_rgba32f();
  DynamicImage* into_rgba8();
  Dimensions get_dimensions();
  Rgba get_pixel(uint32_t x, uint32_t y);
  bool in_bounds(uint32_t x, uint32_t y);
  Iterator<PixelResult> pixels();
};

}  // namespace rs_image
