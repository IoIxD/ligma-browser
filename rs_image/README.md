# rs_image

Bindings for Rust's image crate to C/C++. Rust's image crate, for coincidental reasons that I'm unsure of, supports way more image formats (including the one I needed for a project) then what I could find in C/C++. 

| Format | Decoding | Encoding |
| -------- | -------- | -------- | 
| AVIF | Yes (8-bit only) * | Yes (lossy only) |
| BMP | Yes | Yes |
| DDS | Yes | --- |
| Farbfeld | Yes | Yes |
| GIF | Yes | Yes |
| HDR | Yes | Yes |
| ICO | Yes | Yes |
| JPEG | Yes | Yes |
| EXR | Yes | Yes |
| PNG | Yes | Yes |
| PNM | Yes | Yes |
| QOI | Yes | Yes |
| TGA | Yes | Yes
| TIFF | Yes | Yes |
| WebP | Yes | Yes (lossless only) |

* If CMake is able to find `libdav1d` on your system.

Currently, the most important parts - `DynamicImage`, and Rust's own `std::iter::Iterator`, are bound. 