#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

unsigned char* pngutils_write_png_to_mem(unsigned char* pixels, int stride_bytes, int x, int y, int n, int* out_len) {
    return stbi_write_png_to_mem(pixels, stride_bytes, x, y, n, out_len);
}

int pngutils_write_png(const char* filename, int x, int y, int comp, const void* data, int stride_bytes) {
    return stbi_write_png(filename, x, y, comp, data, stride_bytes);
}

unsigned char* pngutils_read_png_from_memory(const unsigned char* data, int len, int* x, int* y, int* depth, int desired_channels) {
    return stbi_load_from_memory(data, len, x, y, depth, desired_channels);
}

unsigned char* pngutils_read_png(const char* filename, int* x, int* y, int* comp, int req_comp) {
    return stbi_load(filename, x, y, comp, req_comp);
}

void pngutils_free(void* data) {
    stbi_image_free(data);
}