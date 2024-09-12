#include <cstdint>
#include <vector>
#include "rs_image.hpp"

#ifdef __NEVER_DEFINED
#include "rs_image.h"
#endif

namespace rs_image {

template <typename T>
SizeHint Iterator<T>::size_hint() {
  return internal::iter_size_hint(this->inner);
}
template <typename T>
uintptr_t Iterator<T>::count() {
  return internal::iter_count(this->inner);
}
template <typename T>
Iterator<T> Iterator<T>::step_by(uintptr_t step) {
  return internal::iter_step_by(this->inner, step);
}
template <typename T>
Iterator<T> Iterator<T>::chain(Iterator other) {
  return internal::iter_chain(this->inner, other);
}
template <typename T>
Iterator<T> Iterator<T>::zip(Iterator other) {
  return internal::iter_zip(this->inner, other);
}
template <typename T>
Iterator<T> Iterator<T>::map(std::function<void(T)> f) {
  return internal::iter_map(this->inner, f);
}

template <typename T>
void Iterator<T>::for_each(std::function<void(T)> f) {
  return internal::iter_for_each(this->inner, f);
}

template <typename T>
Iterator<T> Iterator<T>::filter(std::function<bool(T)> f) {
  return internal::iter_filter(this->inner, f);
}
template <typename T>
Iterator<T> Iterator<T>::filter_map(std::function<Iterator*(T)> f) {
  return internal::iter_filter_map(this->inner, f);
}
template <typename T>
Iterator<T> Iterator<T>::enumerate() {
  return internal::iter_enumerate(this->inner);
}
template <typename T>
Iterator<T> Iterator<T>::peekable() {
  return internal::iter_peekable(this->inner);
}
template <typename T>
Iterator<T> Iterator<T>::skip_while(std::function<bool(T)> f) {
  return internal::iter_skip_while(this->inner, f);
}
template <typename T>
Iterator<T> Iterator<T>::take_while(std::function<bool(T)> f) {
  return internal::iter_take_while(this->inner, f);
}
template <typename T>
Iterator<T> Iterator<T>::map_while(std::function<Iterator*(T)> f) {
  return internal::iter_map_while(this->inner, f);
}
template <typename T>
Iterator<T> Iterator<T>::skip(uintptr_t n) {
  return internal::iter_skip(this->inner, n);
}
template <typename T>
Iterator<T> Iterator<T>::take(uintptr_t n) {
  return internal::iter_take(this->inner, n);
}
template <typename T>
Iterator<T> Iterator<T>::scan(void* initial_state,
                              std::function<void*(void*, T)> f) {
  return internal::iter_scan(this->inner, initial_state, f);
}
template <typename T>
Iterator<T> Iterator<T>::flat_map(std::function<Iterator(T)> f) {
  return internal::iter_flat_map(this->inner, f);
}
template <typename T>
Iterator<T> Iterator<T>::fuse() {
  return internal::iter_fuse(this->inner);
}
template <typename T>
Iterator<T> Iterator<T>::inspect(std::function<void(T)> f) {
  return internal::iter_inspect(this->inner, f);
}
template <typename T>
Iterator<T> Iterator<T>::by_ref() {
  return internal::iter_by_ref(this->inner);
}
template <typename T>
bool Iterator<T>::all(std::function<bool(T)> f) {
  return internal::iter_all(this->inner, f);
}
template <typename T>
bool Iterator<T>::any(std::function<bool(T)> f) {
  return internal::iter_any(this->inner, f);
}
template <typename T>
Ordering Iterator<T>::cmp(Iterator other) {
  return internal::iter_cmp(this->inner, other);
}
template <typename T>
bool Iterator<T>::eq(Iterator other) {
  return internal::iter_eq(this->inner, other);
}
template <typename T>
bool Iterator<T>::ne(Iterator other) {
  return internal::iter_ne(this->inner, other);
}
template <typename T>
bool Iterator<T>::lt(Iterator other) {
  return internal::iter_lt(this->inner, other);
}
template <typename T>
bool Iterator<T>::le(Iterator other) {
  return internal::iter_le(this->inner, other);
}
template <typename T>
bool Iterator<T>::gt(Iterator other) {
  return internal::iter_gt(this->inner, other);
}
template <typename T>
bool Iterator<T>::ge(Iterator other) {
  return internal::iter_ge(this->inner, other);
}
template <typename T>
T Iterator<T>::next() {
  return internal::iter_next(this->inner);
}
template <typename T>
T Iterator<T>::last() {
  return internal::iter_last(this->inner);
}
template <typename T>
T Iterator<T>::nth(uintptr_t n) {
  return internal::iter_nth(this->inner, n);
}
template <typename T>
T Iterator<T>::fold(T init, std::function<T(T, T)> f) {
  return internal::iter_fold(this->inner, init, f);
}
template <typename T>
T Iterator<T>::reduce(std::function<T(T, T)> f) {
  return internal::iter_reduce(this->inner, f);
}
template <typename T>
T Iterator<T>::find(std::function<bool(T*)> f) {
  return internal::iter_find(this->inner, f);
}
template <typename T>
T Iterator<T>::find_map(std::function<T(T)> f) {
  return internal::iter_find_map(this->inner, f);
}
template <typename T>
size_t* Iterator<T>::position(std::function<bool(T*)> f) {
  return internal::iter_position(this->inner, f);
}
template <typename T>
T Iterator<T>::max() {
  return internal::iter_max(this->inner);
}
template <typename T>
T Iterator<T>::min() {
  return internal::iter_min(this->inner);
}
template <typename T>
Ordering* Iterator<T>::partial_cmp(Iterator* other) {
  return internal::iter_partial_cmp(this->inner, other);
}
template <typename T>
std::vector<T> Iterator<T>::collect() {
  uintptr_t size;
  auto bytes = internal::iter_collect(this->inner, &size);
  return std::vector(bytes, bytes + size);
}
}  // namespace rs_image