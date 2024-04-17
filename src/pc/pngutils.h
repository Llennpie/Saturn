#ifndef PngUtils_H
#define PngUtils_H

// this file exists because yucky stb_write_image.h
// throws link errors if you include it multiple times

extern unsigned char* pngutils_write_png_to_mem(unsigned char* pixels, int stride_bytes, int x, int y, int n, int* out_len);
extern int pngutils_write_png(const char* filename, int x, int y, int comp, const void* data, int stride_bytes);
extern unsigned char* pngutils_read_png_from_memory(const unsigned char* data, int len, int* x, int* y, int* depth, int desired_channels);
extern unsigned char* pngutils_read_png(const char* filename, int* x, int* y, int* comp, int req_comp);
extern void pngutils_free(void* data);

#endif