// raytracer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

const int width = 300;
const int height = 200;

void write_image() {

  cout << "width=" << width << ", height=" << height << endl;

  //逐行从左到右扫描
  int bpp = 4;
  size_t data_size = width * height * bpp;
  char *img_data = (char *)malloc(data_size);
  int count = 0;
  for (int j = height - 1; j >= 0; j--) {
    for (int i = 0; i < width; i++) {
      auto r = float(i) / width;
      auto g = float(j) / height;
      auto b = 0.5f;
      char ir = (char)(255 * r);
      char ig = (char)(255 * g);
      char ib = (char)(255 * b);
      char alpha = (char)255;
      img_data[count++] = ir;
      img_data[count++] = ig;
      img_data[count++] = ib;
      img_data[count++] = alpha;
    }
  }
  stbi_write_png("image.png", width, height, bpp, img_data, 0);
  stbi_image_free(img_data);
}

int main() {
  std::cout << "start ... \n";
  write_image();
}
