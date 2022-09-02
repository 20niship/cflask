#pragma once
#include <cstddef>
#include <cstring>
#include <flask/components.hpp>
#include <flask/flask.hpp>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

namespace Flask {

class BasicDashScripts : public Component {
private:
  const std::string fname = FLASK_DASH_SCRIPT_PATH;
  std::string read_file() const {
    std::ifstream ifs(fname);
    if(!ifs.is_open()) {
      ERROR("File Not found in BasicDashScripts   : " + fname);
      std::cout << "file " + fname + "is not found!!";
      return "<p>file not found! : " + fname + "</p>";
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    return "<script>" + ss.str() + "</script>";
  }

public:
  BasicDashScripts() {
    reset();
    inner_text = read_file();
  }
  virtual void __set_flask_app(Flask*) override{};
  virtual std::string render(PageIO*) override { return inner_text; }
};


class ButtonCallback : public Component {
private:
  PageFunc func;
  std::string cb_id;

public:
  ButtonCallback() = delete;
  ButtonCallback(const std::string& name, PageFunc f) {
    reset();
    inner_text = name;
    std::random_device rnd;
    cb_id = name + std::to_string(rnd() % 1000);
    func = f;
  }
  void __set_flask_app(Flask* app) override;
  virtual std::string render(PageIO*) override { return "<button onclick=\"post_fetch('/cb/" + cb_id + "')\">" + inner_text + "</button>"; }
};

template <typename T> class SliderCallback : public Component {
private:
  PageFunc func;
  std::function<void(T)> callback;
  std::string cb_id;
  std::pair<T, T> range;
  T v;

public:
  SliderCallback() = delete;
  SliderCallback(const std::string& name, T value, std::pair<T, T> range_ = {0, 100}) {
    reset();
    inner_text = name;
    std::random_device rnd;
    range = range_;
    v = value;
    cb_id = name + std::to_string(rnd() % 1000);
  }
  T get() { return v; }
  T* get_ptr() { return &v; }
  virtual void __set_flask_app(Flask* app) override {
    app->Post("/cb/" + cb_id, [&](PageIO* io) {
      const std::string s = io->get_req_body_str();
      try {
        v = std::stof(s);
      } catch(...) {
        ERROR("invalid argument (parseInt) in SliderCallback --> " + s);
      }
      io->send("ok");
    });
  };
  virtual std::string render(PageIO*) override {
    const std::string min_ = std::to_string(range.first);
    const std::string max_ = std::to_string(range.second);
    return "<div> \
        <input type=\"range\" id=\"" +
           cb_id + "\" name=\"" + inner_text + "\" min=\"" + min_ + "\" max=\"" + max_ + "\" > \
        <label for=\"" +
           cb_id + "\" id=\"label-" + cb_id + "\">" + inner_text + "</label> \
        <script> \
          document.getElementById(\"" +
           cb_id + "\").addEventListener('input', (e) => { \n\
            let el = document.getElementById(\"" +
           cb_id + "\");\n \
            let value = el.value; \n\
            fetch_data_to_string(\"/cb/" +
           cb_id + "\", value); \
            let label = document.getElementById(\"label-" +
           cb_id + "\"); label.innerHTML = \"" + inner_text + " = \" + value; \
          }); \
      </script> \
      </div> ";
  }
};


using Pos2D = std::array<float, 2>;
using Size2D = std::array<float, 2>;
using Color3 = std::array<int, 3>;

class CustomDrawer : public Component {
private:
  enum class MsgTypes {
    PointCloudXY = 1,
    PointCloudXXYY,
    Rectangle,
    WiredRectangle,
    Circle,
    WiredCircle,
    Line,
    LineList,
    Triangle,
  };

  std::string canvas_id;
  Size2D canvas_size;
  Size2D field_size;
  Stream stream;
  std::vector<float> arr;
  void _arr_clear();
  void _arr_send();
  void _arr_set_col(const Color3&);
  void _arr_set_id(const int);
  void _arr_set_pos(const Pos2D&);
  void _arr_set_type(const MsgTypes);

public:
  CustomDrawer(const std::string& canvas_id_);
  CustomDrawer();
  virtual void __set_flask_app(Flask*) override;
  std::string render(PageIO* io) override;

  void set_id(const std::string&);
  void set_field_size(const int x, const int y);
  void set_canvas_size(const int x, const int y);
  void draw_point_cloud(const int id, std::vector<float> pc, const Color3& col, const int size = 2);
  void draw_rectangle(const int id, const Pos2D& p1, const Pos2D& p2, const Color3& col);
  void draw_wired_circle(const int id, const Pos2D& center, const int radius, const Color3 col, const int width);
  void draw_circle(const int id, const Pos2D& center, const int radius, const Color3 col);
  void draw_line(const int id, const Pos2D& p1, const Pos2D& p2, const Color3 col, const float width);
  void draw_lines(const int id, const std::vector<float>& x_list, const std::vector<float>& y_list, const Color3 col, const float width);
  void end_plot();
};


} // namespace Flask
