#include <dash/ex_component.hpp>
#include <iostream>
#include <thread>

namespace Flask {
void CustomDrawer::_arr_send() {
  stream.send(arr.data(), arr.size() * sizeof(float));
}

CustomDrawer::CustomDrawer(const std::string& id) {
  canvas_id = id;
  _arr_clear();
  stream.add_header("Transfer-Encoding", "identity");
  stream.add_header("Content-type", "application/octet-stream");
}

CustomDrawer::CustomDrawer() {
  _arr_clear();
  stream.add_header("Transfer-Encoding", "identity");
  stream.add_header("Content-type", "application/octet-stream");
  /* stream.abort(); */
}

void CustomDrawer::_arr_set_col(const Color3& col) {
  arr.push_back(col[0]);
  arr.push_back(col[1]);
  arr.push_back(col[2]);
}

void CustomDrawer::_arr_set_type(const MsgTypes t) {
  arr.push_back((float)t);
}

void CustomDrawer::_arr_set_id(const int id) {
  arr.push_back((float)id);
}

void CustomDrawer::_arr_set_pos(const Pos2D& pos) {
  arr.push_back(pos[0]);
  arr.push_back(pos[1]);
}

void CustomDrawer::__set_flask_app(Flask* app) {
  DEBUG("callled a/staream post");
  app->Post("/stream", &stream);
}

std::string CustomDrawer::render(PageIO *) {
  return "<canvas id=\"visualiser\"></canvas>";
}

void CustomDrawer::set_id(const std::string&) {}
void CustomDrawer::_arr_clear() {
  arr.clear();
}
void CustomDrawer::set_field_size(const int x, const int y) {
  field_size = {(float)x, (float)y};
}

void CustomDrawer::set_canvas_size(const int x, const int y) {
  canvas_size = {(float)x, (float)y};
}

void CustomDrawer::draw_point_cloud(const int id, std::vector<float> pc, const Color3& col, const int size) {
  _arr_clear();
  _arr_set_id(id);
  _arr_set_type(MsgTypes::PointCloudXY);
  _arr_set_col(col);
  arr.push_back(size);
  for(auto&& p : pc) arr.push_back(p);
  _arr_send();
}

void CustomDrawer::draw_rectangle(const int id, const Pos2D& p1, const Pos2D& p2, const Color3& col) {
  _arr_clear();
  _arr_set_id(id);
  _arr_set_type(MsgTypes::Rectangle);
  _arr_set_col(col);
  _arr_set_pos(p1);
  _arr_set_pos(p2);
  _arr_send();
}
void CustomDrawer::draw_wired_circle(const int id, const Pos2D& center, const int radius, const Color3 col, const int width) {
  _arr_clear();
  _arr_set_id(id);
  _arr_set_type(MsgTypes::WiredCircle);
  _arr_set_col(col);
  _arr_set_pos(center);
  arr.push_back(radius);
  arr.push_back(width);
  _arr_send();
}
void CustomDrawer::draw_circle(const int id, const Pos2D& center, const int radius, const Color3 col) {
  _arr_clear();
  _arr_set_id(id);
  _arr_set_type(MsgTypes::Circle);
  _arr_set_col(col);
  _arr_set_pos(center);
  arr.push_back(radius);
  _arr_send();
}


void CustomDrawer::draw_lines(const int id, const std::vector<float>& x_list, const std::vector<float>& y_list, const Color3 col, const float width) {
  _arr_clear();
  _arr_set_id(id);
  _arr_set_type(MsgTypes::LineList);
  _arr_set_col(col);
  arr.push_back(width);
  for(size_t i = 0; i < x_list.size(); i++) {
    arr.push_back(x_list[i]);
    arr.push_back(y_list[i]);
  }
  _arr_send();
}

void CustomDrawer::draw_line(const int id, const Pos2D& p1, const Pos2D& p2, const Color3 col, const float width) {
  _arr_clear();
  _arr_set_id(id);
  _arr_set_type(MsgTypes::Line);
  _arr_set_col(col);
  _arr_set_pos(p1);
  _arr_set_pos(p2);
  arr.push_back(width);
  _arr_send();
}

} // namespace Flask
