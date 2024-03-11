#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

const unsigned int bits_per_byte_ = 8;

const char format_[] = "BM";
const unsigned int size_offset_ = 2;
const unsigned int image_offset_ = 10;
const unsigned int width_offset_ = 18;
const unsigned int height_offset_ = 22;
const unsigned int bits_per_pixel_offset_ = 28;
const unsigned int compression_offset_ = 30;

const unsigned int bits_per_pixel_ = 24;  // for checking
const unsigned int compression_ = 0;

const unsigned int blue_ = 0;
const unsigned int green_ = 1;
const unsigned int red_ = 2;  // in that order

BMP bmp_read(const char* file_path) {
  FILE* fp = fopen(file_path, "rb");
  if (fp == NULL) {
    perror("Error opening file");
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  unsigned int byte_file_size = ftell(fp);
  rewind(fp);

  char* buffer = (char*)malloc(byte_file_size * sizeof(char));
  if (buffer == NULL) {
    perror("Error allocating ");
    exit(1);
  }

  unsigned int check_size_1 = fread(buffer, sizeof(char), byte_file_size, fp);
  if (check_size_1 != byte_file_size) {
    free(buffer);
    perror("Error reading file");
    exit(1);
  }
  fclose(fp);

  unsigned int check_size_2 = *(unsigned int*)(buffer + size_offset_);
  if (check_size_2 != byte_file_size || buffer[0] != format_[0] || buffer[1] != format_[1]) {
    free(buffer);
    perror("It is not proper BMP format");
    exit(1);
  }

  BMP bmp;
  bmp.size_ = byte_file_size;
  bmp.raw_data_ = buffer;
  bmp.offset_ = *(unsigned int*)(buffer + image_offset_);
  bmp.width_ = *(unsigned int*)(buffer + width_offset_);
  bmp.height_ = *(unsigned int*)(buffer + height_offset_);
  bmp.bits_per_pixel_ = *(unsigned int*)(buffer + bits_per_pixel_offset_);
  bmp.channels_ = bmp.bits_per_pixel_ / (sizeof(char) * bits_per_byte_);
  bmp.compression_ = *(unsigned int*)(buffer + compression_offset_);
  bmp.row_size_ = (bmp.bits_per_pixel_ * bmp.width_ + 31) / 32 * 4;
  bmp.padding_ = bmp.row_size_ - bmp.width_ * bmp.channels_;

  if (bmp.bits_per_pixel_ != bits_per_pixel_ || bmp.compression_ != compression_) {
    free(buffer);
    perror("Can work only with 24-bits uncompressed images");
    exit(1);
  }
  return bmp;
}

void bmp_write(BMP* bmp, const char* file_path) {
  FILE* fp = fopen(file_path, "wb");
  if (fp == NULL) {
    perror("Error opening file for writing");
    return;
  }
  unsigned int check = fwrite(bmp->raw_data_, sizeof(char), bmp->size_, fp);
  fclose(fp);
  if (check != bmp->size_) {
    perror("Error writing file");
    return;
  }
}

static unsigned int get_pixel_(BMP* bmp, unsigned int x, unsigned int y) {
  return bmp->offset_ + y * (bmp->width_ * bmp->channels_ + bmp->padding_) + x * bmp->channels_;
}

unsigned char get_red_pixel_value(BMP* bmp, unsigned int x, unsigned int y) {
  return bmp->raw_data_[get_pixel_(bmp, x, y) + red_];
}

unsigned char get_green_pixel_value(BMP* bmp, unsigned int x, unsigned int y) {
  return bmp->raw_data_[get_pixel_(bmp, x, y) + green_];
}

unsigned char get_blue_pixel_value(BMP* bmp, unsigned int x, unsigned int y) {
  return bmp->raw_data_[get_pixel_(bmp, x, y) + blue_];
}

RGB get_pixel_value(BMP* bmp, unsigned int x, unsigned int y) {
  RGB rgb = {.red_ = get_red_pixel_value(bmp, x, y), .green_ = get_green_pixel_value(bmp, x, y), .blue_ = get_blue_pixel_value(bmp, x, y)};
  return rgb;
}

void set_red_pixel_value(BMP* bmp, unsigned int x, unsigned int y, unsigned char value) {
  bmp->raw_data_[get_pixel_(bmp, x, y) + red_] = value;
}

void set_green_pixel_value(BMP* bmp, unsigned int x, unsigned int y, unsigned char value) {
  bmp->raw_data_[get_pixel_(bmp, x, y) + green_] = value;
}

void set_blue_pixel_value(BMP* bmp, unsigned int x, unsigned int y, unsigned char value) {
  bmp->raw_data_[get_pixel_(bmp, x, y) + blue_] = value;
}

void set_pixel_value(BMP* bmp, unsigned int x, unsigned int y, RGB rgb) {
  bmp->raw_data_[get_pixel_(bmp, x, y) + red_] = rgb.red_;
  bmp->raw_data_[get_pixel_(bmp, x, y) + green_] = rgb.green_;
  bmp->raw_data_[get_pixel_(bmp, x, y) + blue_] = rgb.blue_;
}

void bmp_close(BMP* bmp) {
  free(bmp->raw_data_);
}

void print_info(BMP* bmp) {
  printf("Size: %d\n", bmp->size_);
  printf("Offset: %d\n", bmp->offset_);
  printf("Width: %d\n", bmp->width_);
  printf("Height: %d\n", bmp->height_);
  printf("Bits Per Pixel: %d\n", bmp->bits_per_pixel_);
  printf("Channels: %d\n", bmp->channels_);
  printf("Row size: %d\n", bmp->row_size_);
  printf("Padding: %d\n", bmp->padding_);
}