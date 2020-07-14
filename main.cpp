// raytracer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <amp.h>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "camera.h"
#include "hitable_list.h"
#include "material.h"
#include "sphere.h"

using namespace std;
using namespace concurrency;

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

vec3 ray_color(const ray &r, const hitable_list &world, int depth) {
  hit_record rec;

  if (depth <= 0)
    return vec3(0, 0, 0);

  if (world.hit(r, 0.001, infinity, rec)) {
    // TODO ?
    ray scattered;
    vec3 attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, depth - 1);
    return vec3(0, 0, 0);
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

hitable_list random_scene() {
  hitable_list world;

  world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000,
                                make_shared<lambertian>(vec3(0.5, 0.5, 0.5))));

  int i = 1;
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = vec3::random() * vec3::random();
          world.add(make_shared<sphere>(center, 0.2,
                                        make_shared<lambertian>(albedo)));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = vec3::random(.5, 1);
          auto fuzz = random_double(0, .5);
          world.add(make_shared<sphere>(center, 0.2,
                                        make_shared<metal>(albedo, fuzz)));
        } else {
          // glass
          world.add(
              make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
        }
      }
    }
  }

  world.add(
      make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));

  world.add(make_shared<sphere>(vec3(-4, 1, 0), 1.0,
                                make_shared<lambertian>(vec3(0.4, 0.2, 0.1))));

  world.add(make_shared<sphere>(vec3(4, 1, 0), 1.0,
                                make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

  return world;
}

// 逐行从上到下从左到右扫描
void get_image() {

  cout << "width=" << width << ", height=" << height << endl;

  // objects
  hitable_list world = random_scene();

  auto material1 = make_shared<lambertian>(vec3(0.7, 0.3, 0.3));
  auto material2 = make_shared<lambertian>(vec3(0.8, 0.8, 0.0));
  auto material3 = make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.2);
  auto material4 = make_shared<metal>(vec3(0.8, 0.8, 0.8), 1.0);

  world.add(make_shared<sphere>(vec3(0, 0, -1), 0.5, material1));
  world.add(make_shared<sphere>(vec3(0, -100.5, -1), 100, material2));

  world.add(make_shared<sphere>(vec3(1, 0, -1), 0.5, material3));
  world.add(
      make_shared<sphere>(vec3(-1, 0, -1), 0.5, make_shared<dielectric>(1.5)));

  world.add(make_shared<sphere>(vec3(-1, 0, -1), -0.45,
                                make_shared<dielectric>(1.5)));

  // for write image file
  vector<vec3> color_list;

  const int samples_per_pixel = 100; // 100;
  const int max_depth = 50;
  vec3 lookfrom(13, 2, 3);
  vec3 lookat(0, 0, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  double fov = 30;

  // init camera
  const auto aspect_ratio = double(width) / height;
  camera cam(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus);

  for (int j = height - 1; j >= 0; j--) {
    std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
    for (int i = 0; i < width; i++) {
      vec3 color(0, 0, 0);
      // deal samples
      for (int s = 0; s < samples_per_pixel; s++) {
        auto u = (i + random_double()) / width;
        auto v = (j + random_double()) / height;
        auto r = cam.get_ray(u, v);
        color += ray_color(r, world, max_depth);
      }
      color.gamma(samples_per_pixel);
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
