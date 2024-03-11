#ifndef BMP_H
#define BMP_H

#ifdef __cplusplus
extern "C" {
#endif

// for 24-bits non-compressed images

typedef struct BMP_file {
  unsigned char* raw_data_;
  unsigned int size_;
  unsigned int offset_;
  unsigned int width_;
  unsigned int height_;
  unsigned int bits_per_pixel_;  // = 24
  unsigned int channels_;        // = 3
  unsigned int compression_;     // = 0
  unsigned int row_size_;
  unsigned int padding_;
} BMP;

typedef struct RGB_pixel {
  unsigned char red_;
  unsigned char green_;
  unsigned char blue_;
} RGB;

BMP bmp_read(const char* file_path);
void bmp_write(BMP* bmp, const char* file_path);
void bmp_close(BMP* bmp);
void print_info(BMP* bmp);

unsigned char get_red_pixel_value(BMP* bmp, unsigned int x, unsigned int y);
unsigned char get_green_pixel_value(BMP* bmp, unsigned int x, unsigned int y);
unsigned char get_blue_pixel_value(BMP* bmp, unsigned int x, unsigned int y);
RGB get_pixel_value(BMP* bmp, unsigned int x, unsigned int y);

void set_red_pixel_value(BMP* bmp, unsigned int x, unsigned int y, unsigned char value);
void set_green_pixel_value(BMP* bmp, unsigned int x, unsigned int y, unsigned char value);
void set_blue_pixel_value(BMP* bmp, unsigned int x, unsigned int y, unsigned char value);
void set_pixel_value(BMP* bmp, unsigned int x, unsigned int y, RGB rgb);

#ifdef __cplusplus
}
#endif

#endif  // BMP_H