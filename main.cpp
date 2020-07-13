// raytracer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "camera.h"
#include "hitable_list.h"
#include "rtweekend.h"
#include "sphere.h"

using namespace std;

const int width = 300;
const int height = 150;

// main.cc 球体表面法相
double hit_sphere(const vec3 &center, double radius, const ray &r) {
  vec3 oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = dot(oc, r.direction());
  auto c = oc.length_squared() - radius * radius;
  auto discriminant = half_b * half_b - a * c;

  if (discriminant < 0) {
    return -1.0;
  } else {
    return (-half_b - sqrt(discriminant)) / a;
  }
}

vec3 ray_color(const ray &r, const hitable_list &world) {
  hit_record rec;
  if (world.hit(r, 0, infinity, rec)) {
    return 0.5 * (rec.normal + vec3(1, 1, 1));
  }
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

void write_image_file(vector<vec3> &color_list) {
  int bpp = 4;
  size_t data_size = width * height * bpp;
  char *img_data = (char *)malloc(data_size);
  int count = 0;
  for (auto &color : color_list) {
    img_data[count++] = (char)(color.x() * 255);
    img_data[count++] = (char)(color.y() * 255);
    img_data[count++] = (char)(color.z() * 255);
    img_data[count++] = (char)255;
  }
  stbi_write_png("image.png", width, height, bpp, img_data, 0);
  stbi_image_free(img_data);
}

// 逐行从上到下从左到右扫描
void get_image() {

  cout << "width=" << width << ", height=" << height << endl;

  // objects
  hitable_list world;
  world.add(make_shared<sphere>(vec3(0, 0, -1), 0.5));
  world.add(make_shared<sphere>(vec3(0, -100.5, -1), 100));

  // for write image file
  vector<vec3> color_list;

  int samples_per_pixel = 100;

  // int bpp = 4;
  // size_t data_size = width * height * bpp;
  // char *img_data = (char *)malloc(data_size);
  // int count = 0;

  camera cam;

  for (int j = height - 1; j >= 0; j--) {
    std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
    for (int i = 0; i < width; i++) {
      vec3 color(0, 0, 0);
      // deal samples
      for (int s = 0; s < samples_per_pixel; s++) {
        auto u = (i + random_double()) / width;
        auto v = (j + random_double()) / height;
        auto r = cam.get_ray(u, v);
        color += ray_color(r, world);
      }
      color.normalized();
      color_list.push_back(color);
    }
  }
  write_image_file(color_list);
  std::cerr << "\nDone.\n";
}

int main() {
  std::cout << "start ... \n";
  get_image();
  std::cout << "end ... \n";
}
