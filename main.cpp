// raytracer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ray.h"
#include "stb_image_write.h"

using namespace std;

const int width = 300;
const int height = 200;

vec3 ray_color(const ray &r) {
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

void write_image() {

  cout << "width=" << width << ", height=" << height << endl;

  //逐行从左到右扫描
  vec3 lower_left_corner(-2.0, -1.0, -1.0);
  vec3 horizontal(4.0, 0.0, 0.0);
  vec3 vertical(0.0, 2.0, 0.0);
  vec3 origin(0.0, 0.0, 0.0);
  // stringstream ss;
  int bpp = 4;
  size_t data_size = width * height * bpp;
  char *img_data = (char *)malloc(data_size);
  int count = 0;
  for (int j = height - 1; j >= 0; j--) {
    std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
    for (int i = 0; i < width; i++) {
      // auto r = float(i) / width;
      // auto g = float(j) / height;
      // auto b = 0.5f;
      // img_data[count++] = (char)(255 * r);
      // img_data[count++] = (char)(255 * g);
      // img_data[count++] = (char)(255 * b);
      // img_data[count++] = (char)255;
      auto u = float(i) / width;
      auto v = float(j) / height;
      ray ra(origin, lower_left_corner + u * horizontal + v * vertical);
      auto color = ray_color(ra);
      img_data[count++] = (char)(color.x() * 255);
      img_data[count++] = (char)(color.y() * 255);
      img_data[count++] = (char)(color.z() * 255);
      img_data[count++] = (char)255;
    }
  }
  stbi_write_png("image.png", width, height, bpp, img_data, 0);
  stbi_image_free(img_data);
  std::cerr << "\nDone.\n" << sizeof(float);
}

int main() {
  std::cout << "start ... \n";
  write_image();
  std::cout << "end ... \n";
}
