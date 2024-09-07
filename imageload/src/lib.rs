#![allow(non_snake_case)]
use std::os::raw::c_void;

use image::{self, load_from_memory, GenericImageView};

#[repr(C)]
pub struct __DynamicImage__ {
    image: *mut c_void,
}

impl __DynamicImage__ {
    fn to_real(&self) -> &mut image::DynamicImage {
        return unsafe { (self.image as *mut image::DynamicImage).as_mut().unwrap() };
    }
}

#[no_mangle]
pub extern "C" fn internal__Load__DynamicImage__FromMemory(
    data: *mut u8,
    len: usize,
) -> __DynamicImage__ {
    let slice = unsafe { std::slice::from_raw_parts(data, len) };
    let image = load_from_memory(slice).unwrap();

    __DynamicImage__ {
        image: Box::leak(Box::new(image)) as *mut image::DynamicImage as *mut c_void,
    }
}

#[repr(C)]
pub struct Dimensions {
    width: u32,
    height: u32,
}

#[repr(C)]
pub struct RGBA {
    pub r: u8,
    pub g: u8,
    pub b: u8,
    pub a: u8,
}

#[no_mangle]
pub extern "C" fn internal__DynamicImage__Dimensions(this: *mut __DynamicImage__) -> Dimensions {
    let dimensions = unsafe { this.as_mut().unwrap() }.to_real().dimensions();
    return Dimensions {
        width: dimensions.0,
        height: dimensions.1,
    };
}
#[no_mangle]
pub extern "C" fn internal__DynamicImage__GetPixel(
    this: *mut __DynamicImage__,
    x: u32,
    y: u32,
) -> RGBA {
    let rgba = unsafe { this.as_mut().unwrap() }.to_real().get_pixel(x, y);
    return RGBA {
        r: rgba.0[0],
        g: rgba.0[1],
        b: rgba.0[2],
        a: rgba.0[3],
    };
}
#[no_mangle]
pub extern "C" fn internal__DynamicImage__GetWidth(this: *mut __DynamicImage__) -> u32 {
    unsafe { this.as_mut().unwrap() }.to_real().width()
}
#[no_mangle]
pub extern "C" fn internal__DynamicImage__GetHeight(this: *mut __DynamicImage__) -> u32 {
    unsafe { this.as_mut().unwrap() }.to_real().height()
}
#[no_mangle]
pub extern "C" fn internal__DynamicImage__InBounds(
    this: *mut __DynamicImage__,
    x: u32,
    y: u32,
) -> bool {
    unsafe { this.as_mut().unwrap() }.to_real().in_bounds(x, y)
}

#[no_mangle]
pub extern "C" fn internal__DynamicImage__Free(this: *mut __DynamicImage__) {
    let og = unsafe { Box::from_raw(this.as_mut().unwrap().to_real()) };
    std::mem::forget(og);
}

#[no_mangle]
pub extern "C" fn main() -> i32 {
    return 0;
}
