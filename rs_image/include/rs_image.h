#ifndef __INTERNAL_IMAGE_LOAD_H
#define __INTERNAL_IMAGE_LOAD_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum ErrorType {
  IMAGE_ERROR_NONE,
  IMAGE_ERROR_DECODING,
  IMAGE_ERROR_ENCODING,
  IMAGE_ERROR_PARAMETER_DIMENSION_MISMATCH,
  IMAGE_ERROR_PARAMETER_FAILED_ALREADY,
  IMAGE_ERROR_PARAMETER_MALFORMED,
  IMAGE_ERROR_PARAMETER_NO_MORE_DATA,
  IMAGE_ERROR_INSUFFICIENT_MEMORY,
  IMAGE_ERROR_LIMITS_UNSUPPORTED,
  IMAGE_ERROR_DIMENSION_ERROR,
  IMAGE_ERROR_UNSUPPORTED_COLOR,
  IMAGE_ERROR_UNSUPPORTED_FORMAT,
  IMAGE_ERROR_UNSUPPORTED_OTHER,
  IMAGE_ERROR_IO_NOT_FOUND,
  IMAGE_ERROR_IO_PERMISSION_DENIED,
  IMAGE_ERROR_IO_CONNECTION_REFUSED,
  IMAGE_ERROR_IO_CONNECTION_RESET,
  IMAGE_ERROR_IO_CONNECTION_ABORTED,
  IMAGE_ERROR_IO_NOT_CONNECTED,
  IMAGE_ERROR_IO_ADDR_IN_USE,
  IMAGE_ERROR_IO_ADDR_NOT_AVALIABLE,
  IMAGE_ERROR_IO_BROKEN_PIPE,
  IMAGE_ERROR_IO_ALREADY_EXISTS,
  IMAGE_ERROR_IO_WOULD_BLOCK,
  IMAGE_ERROR_IO_INVALID_INPUT,
  IMAGE_ERROR_IO_INVALID_DATA,
  IMAGE_ERROR_IO_TIMED_OUT,
  IMAGE_ERROR_IO_WRITE_ZERO,
  IMAGE_ERROR_IO_INTERRUPTED,
  IMAGE_ERROR_IO_UNSUPPORED,
  IMAGE_ERROR_IO_UNEXPECTED_EOF,
  IMAGE_ERROR_IO_OUT_OF_MEMORY,
  IMAGE_ERROR_IO_OTHER,
  IMAGE_ERROR_UNKNOWN,
} ErrorType;

typedef enum ExtendedColorType {
  EXTENDED_COLOR_TYPE_A8,
  EXTENDED_COLOR_TYPE_L1,
  EXTENDED_COLOR_TYPE_LA1,
  EXTENDED_COLOR_TYPE_RGB1,
  EXTENDED_COLOR_TYPE_RGBA1,
  EXTENDED_COLOR_TYPE_L2,
  EXTENDED_COLOR_TYPE_LA2,
  EXTENDED_COLOR_TYPE_RGB2,
  EXTENDED_COLOR_TYPE_RGBA2,
  EXTENDED_COLOR_TYPE_L4,
  EXTENDED_COLOR_TYPE_LA4,
  EXTENDED_COLOR_TYPE_RGB4,
  EXTENDED_COLOR_TYPE_RGBA4,
  EXTENDED_COLOR_TYPE_L8,
  EXTENDED_COLOR_TYPE_LA8,
  EXTENDED_COLOR_TYPE_RGB8,
  EXTENDED_COLOR_TYPE_RGBA8,
  EXTENDED_COLOR_TYPE_L16,
  EXTENDED_COLOR_TYPE_LA16,
  EXTENDED_COLOR_TYPE_RGB16,
  EXTENDED_COLOR_TYPE_RGBA16,
  EXTENDED_COLOR_TYPE_BGR8,
  EXTENDED_COLOR_TYPE_BGRA8,
  EXTENDED_COLOR_TYPE_RGB32F,
  EXTENDED_COLOR_TYPE_RGBA32F,
  EXTENDED_COLOR_TYPE_CMYK8,
} ExtendedColorType;

/**
 * Type used for the SeekFrom struct
 */
typedef enum SeekType {
  SEEK_FROM_START,
  SEEK_FROM_END,
  SEEK_FROM_CURRENT,
} SeekType;

typedef struct ColorType ColorType;

typedef struct FilterType FilterType;

typedef struct ImageFormat ImageFormat;

typedef struct DynamicImage {
  void *inner;
} DynamicImage;

typedef struct LoadFromMemoryResult {
  struct DynamicImage *res;
  enum ErrorType err;
} LoadFromMemoryResult;

/**
 * Union used for the SeekFrom struct
 */
typedef union SeekUnion {
  /**
   * Sets the offset to the provided number of bytes.
   */
  uint64_t start;
  /**
   * Sets the offset to the size of this object plus the specified number of bytes. It is possible to seek beyond the end of an object, but it’s an error to seek before byte 0.
   */
  int64_t end;
  /**
   * Sets the offset to the current position plus the specified number of bytes. It is possible to seek beyond the end of an object, but it’s an error to seek before byte 0.
   */
  int64_t current;
} SeekUnion;

/**
 * Enumeration of possible methods to seek within an I/O object.
 * ty represents whether you need to seek from the start, end, or current position
 */
typedef struct SeekFrom {
  enum SeekType ty;
  union SeekUnion val;
} SeekFrom;

/**
 * Struct that contains function pointers that correspond to both Rust's Write trait and it's Seek trait.
 * The idea here is that you put the pointer for your custom own struct in the user_data field,
 * then pass your own functions which will then access that user data.
 */
typedef struct RustWriter {
  void *user_data;
  uintptr_t (*write_fn)(void *ud, const uint8_t *buf, uintptr_t buf_size);
  void (*flush_fn)(void *ud);
  uint64_t (*seek_fn)(void *ud, struct SeekFrom pos);
} RustWriter;

/**
 * Struct that contains function pointers that correspond to the image crate's ImageEncoder trait.
 * The idea here is that you put the pointer for your custom own struct in the user_data field,
 * then pass your own function to writeFn, which will then access that user data.
 */
typedef struct ImageEncoder {
  void *user_data;
  void (*writeFn)(void *ud,
                  const uint8_t *buf,
                  uintptr_t size,
                  uint32_t width,
                  uint32_t height,
                  enum ExtendedColorType color_type);
} ImageEncoder;

typedef struct Dimensions {
  uint32_t width;
  uint32_t height;
} Dimensions;

typedef struct Rgba {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} Rgba;

struct LoadFromMemoryResult dynamic_image_load_from_memory(uint8_t *bytes, uintptr_t size);

void dynamic_image_from_decoder(void);

struct DynamicImage *dynamic_image_adjust_contrast(struct DynamicImage *this_, float c);

uint8_t *dynamic_image_as_bytes(struct DynamicImage *this_, uintptr_t *count);

struct DynamicImage *dynamic_image_blur(struct DynamicImage *this_, float sigma);

struct DynamicImage *dynamic_image_brighten(struct DynamicImage *this_, int32_t value);

void dynamic_image_invert(struct DynamicImage *this_);

struct ColorType dynamic_image_color(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_crop(struct DynamicImage *this_,
                                        uint32_t x,
                                        uint32_t y,
                                        uint32_t width,
                                        uint32_t height);

struct DynamicImage *dynamic_image_crop_imm(struct DynamicImage *this_,
                                            uint32_t x,
                                            uint32_t y,
                                            uint32_t width,
                                            uint32_t height);

struct DynamicImage *dynamic_image_filter3x3(struct DynamicImage *this_,
                                             float *kernel,
                                             uintptr_t size);

struct DynamicImage *dynamic_image_fliph(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_flipv(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_grayscale(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_unsharpen(struct DynamicImage *this_,
                                             float sigma,
                                             int32_t threshold);

uint32_t dynamic_image_width(struct DynamicImage *this_);

uint32_t dynamic_image_height(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_huerotate(struct DynamicImage *this_, int32_t value);

const uint8_t **dynamic_image_into_bytes(const struct DynamicImage *this_, uintptr_t *size);

struct DynamicImage *dynamic_image_resize(struct DynamicImage *this_,
                                          uint32_t nwidth,
                                          uint32_t nheight,
                                          struct FilterType filter);

struct DynamicImage *dynamic_image_resize_exact(struct DynamicImage *this_,
                                                uint32_t nwidth,
                                                uint32_t nheight,
                                                struct FilterType filter);

struct DynamicImage *dynamic_image_resize_to_fill(struct DynamicImage *this_,
                                                  uint32_t nwidth,
                                                  uint32_t nheight,
                                                  struct FilterType filter);

struct DynamicImage *dynamic_image_rotate180(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_rotate270(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_rotate90(struct DynamicImage *this_);

const char *dynamic_image_save(struct DynamicImage *this_, const char *path);

enum ErrorType dynamic_image_save_with_format(struct DynamicImage *this_,
                                              const char *path,
                                              struct ImageFormat format);

struct DynamicImage *dynamic_image_thumbnail(struct DynamicImage *this_,
                                             uint32_t nwidth,
                                             uint32_t nheight);

struct DynamicImage *dynamic_image_thumbnail_exact(struct DynamicImage *this_,
                                                   uint32_t nwidth,
                                                   uint32_t nheight);

struct DynamicImage *dynamic_image_into_luma16(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_into_luma8(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_into_luma_alpha16(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_into_luma_alpha8(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_into_rgb16(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_into_rgb32f(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_into_rgb8(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_into_rgba16(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_into_rgba32f(struct DynamicImage *this_);

struct DynamicImage *dynamic_image_into_rgba8(struct DynamicImage *this_);

enum ErrorType dynamic_image_write_to(struct DynamicImage *this_,
                                      struct RustWriter *w,
                                      struct ImageFormat format);

enum ErrorType dynamic_image_write_with_encoder(struct DynamicImage *this_,
                                                struct ImageEncoder *encoder);

struct Dimensions dynamic_image_dimensions(struct DynamicImage *this_);

struct Rgba dynamic_image_get_pixel(struct DynamicImage *this_, uint32_t x, uint32_t y);

bool dynamic_image_in_bounds(struct DynamicImage *this_, uint32_t x, uint32_t y);

void dynamic_image_free(struct DynamicImage *this_);

#endif  /* __INTERNAL_IMAGE_LOAD_H */
