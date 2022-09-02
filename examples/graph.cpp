#include <dash/dash.hpp>
#include <dash/database.hpp>
#include <dash/ex_component.hpp>
#include <flask/components.hpp>
#include <iostream>
#include <random>
#include <thread>

Flask::CustomDrawer drawer;

int counter;

void update_fig() {
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_int_distribution<int> distr(0, 10000);
  while(1) {
    /* drawer.set_field_size(1000, 1000); */

    // draw point cloud
    std::vector<float> pc_list;
    for(int i = 0; i < 1000; i++) pc_list.push_back(distr(eng));
    drawer.draw_point_cloud(0, pc_list, {255, 255, 0}, 30);

    // draw wired circle
    drawer.draw_wired_circle(2, {500, 300}, 100 * counter, {0, 255, 255}, 30);

    // draw rectangle
    drawer.draw_rectangle(1, {100.0f * counter, 200}, {500 + 100.0f * counter, 600}, {255, 100, 30});

    // draw linw
    drawer.draw_line(3, {0.0f + counter * 100, 1000}, {6000, 7000}, {255, 10, 255}, 60);

    /* drawer.set_drawing_list(); */
    std::vector<float> x_list, y_list;
    for(int j = 0; j < 20; j++) {
      x_list.push_back(20 * counter + 80 * j);
      y_list.push_back(2000 + 10 * (j - 10) * (j - 10));
    }
    drawer.draw_lines(4, x_list, y_list, {50, 255, 50}, 30);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    counter++;
  }
}

int main() {
  Flask::Dash app;
  app.start();
  using namespace Flask;
  drawer.set_id("canvas_id");

  app.Get(std::regex("\\/public\\/(.*)"), new StaticDir("../../template/public"));
  app.Get("/", &drawer);
  std::thread t(update_fig);
  t.detach();

  while(1) app.update();
  return 0;
}
