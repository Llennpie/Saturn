#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

unsigned char* pngutils_write_png_to_mem(unsigned char* pixels, int stride_bytes, int x, int y, int n, int* out_len) {
    return stbi_write_png_to_mem(pixels, stride_bytes, x, y, n, out_len);
}

int pngutils_write_png(const char* filename, int x, int y, int comp, const void* data, int stride_bytes) {
    return stbi_write_png(filename, x, y, comp, data, stride_bytes);
}