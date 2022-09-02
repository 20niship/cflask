#include <chrono>
#include <cstdint>
#include <dash/dash.hpp>
#include <filesystem>
#include <iostream>
#include <random>
#include <thread>

struct JoypadData {
  uint8_t _rectangle;
  uint8_t _cross;
  uint8_t _circle;
  uint8_t _triangle;

  uint8_t _l1;
  uint8_t _r1;
  uint8_t _l2;
  uint8_t _r2;

  uint8_t _select;
  uint8_t _start;

  uint8_t _lstick_push;
  uint8_t _rstick_push;

  uint8_t _left;
  uint8_t _right;
  uint8_t _top;
  uint8_t _bottom;

  uint8_t _axes[8];

  bool is_connected{false};
  JoypadData() = default;
  bool left() const { return _left > 0; }
  bool right() const { return _right > 0; }
  bool top() const { return _top > 0; }
  bool bottom() const { return _bottom > 0; }
  bool trignale() const { return _triangle > 0; }
  bool rectangle() const { return _rectangle > 0; }
  bool circle() const { return _circle > 0; }
  bool cross() const { return _cross > 0; }
  bool l1() const { return _l1 > 0; }
  bool r1() const { return _r1 > 0; }
  bool l2() const { return _l2 > 0; }
  bool r2() const { return _r2 > 0; }
  bool lstick_push() const { return _lstick_push > 0; }
  bool rstick_push() const { return _rstick_push > 0; }
};

JoypadData joypad1, joypad2;
Flask::Dash app;
Flask::Stream stream_obj;
Flask::Stream lrf_stream;
Flask::Stream js_stream;

void init_task() {
  app.start(8080);
  app.set_timeout(0);
  app.Get(std::regex("\\/public\\/(.*)"), new Flask::StaticDir("../../template/public"));
  std::filesystem::path p = __FILE__;
  const auto PARENT_PATH = std::string(p.parent_path().parent_path()) + "/template";
  app.Get("/", [=](auto res) { res->render(PARENT_PATH + "/rc22/index.html"); });
  app.Get("/viz", [=](auto res) { res->render(PARENT_PATH + "/rc22/visualizer.html"); });
  app.Get("/joypad", [=](auto res) { res->render(PARENT_PATH + "/rc22/joypad.html"); });
  app.Get("/power", [=](auto res) { res->render(PARENT_PATH + "/rc22/power.html"); });
  app.Get("/config", [=](auto res) { res->render(PARENT_PATH + "/rc22/config.html"); });

  // function callback
  // /cb/[name]にPOSTリクエストが送られる
  app.add_callback("start_r2", [&](auto s) {
    std::cout << "Start R2!!" << std::endl;
    s->send();
  });

  // data synchronization
  // /sync/[name]にPOSTリクエストでデータが送られる
#define DISP(A) std::cout << #A << " = " << (int)(A) << std::endl;
  const int n = 20;
  uint8_t* ptr = (uint8_t*)(&joypad1._rectangle);
  app.Post("/sync/joypad1", [&](Flask::PageIO* res) {
    const auto uint8_arr = res->get_req_body();
    const int size = uint8_arr.size();
    for(int i = 0; i < std::min(n, size); i++) {
      ptr[i] = uint8_arr[i];
    }
    res->send();
    DISP(joypad1.circle());
    DISP(joypad1.rectangle());
    DISP(joypad1.trignale());
    DISP(joypad1.cross());
    DISP(joypad1.l1());
    DISP(joypad1.l2());
    DISP(joypad1.r1());
    DISP(joypad1.r2());
    DISP(joypad1.lstick_push());
    DISP(joypad1.rstick_push());
    DISP(joypad1.top());
    DISP(joypad1.bottom());
    DISP(joypad1.left());
    DISP(joypad1.right());
    DISP(joypad1._axes[0]);
    DISP(joypad1._axes[1]);
    DISP(joypad1._axes[2]);
    DISP(joypad1._axes[3]);
    res->send();
  });

  uint8_t* ptr2 = (uint8_t*)(&joypad2._rectangle);
  app.Post("/sync/joypad2", [&](Flask::PageIO* res) {
    const auto uint8_arr = res->get_req_body();
    const int size = uint8_arr.size();
    for(int i = 0; i < std::min(n, size); i++) {
      ptr2[i] = uint8_arr[i];
    }
    std::cout << "DAAAAAAAAAAAAAA" << std::endl;
    res->send();
  });

  app.Get("/ping", [&](auto res) { res->send("pong!"); });

  stream_obj.add_header("Transfer-Encoding", "identity");
  stream_obj.add_header("Content-type", "application/octet-stream");
  lrf_stream.add_header("Transfer-Encoding", "identity");
  lrf_stream.add_header("Content-type", "application/octet-stream");
  js_stream.add_header("Transfer-Encoding", "identity");
  js_stream.add_header("Content-type", "application/octet-stream");
  app.Post("/stream", &stream_obj);
  app.Post("/jsstream", &js_stream);
  app.Post("/lrf_stream", &lrf_stream);

  std::thread t([=] {
    while(1) app.update();
  });
  t.detach();
}

int main() {
  init_task();
  int counter = 0;
  while(1) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    const std::string text = "log_info(" + std::to_string(counter) + ");";
    js_stream.send(text.data(), text.size());
    counter++;
  }
}
