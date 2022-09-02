#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <flask/components.hpp>
#include <dash/dash.hpp>

#define disp(A) (std::cout << #A << " = " << (A) << "\n")

namespace Flask {

/* void Dash::main_page_func(const HTTPRequest *, */
/*                           HTTPresponse *res) { */
/*   std::string st = ""; */
/*   for (const auto c : components) */
/*     st += c->render(); */
/*   res->header("Content-Type", "text/html; charset=utf-8"); */
/*   res->send(st); */
/* } */

void Dash::add_callback(const std::string& id, PageFunc f) {
  Get("/cb/" + id, f);
}

template <typename DataType> void Dash::sync_data(const std::string& id, DataType* data, int n) {
  Get("/sync/" + id, [&](PageIO *req) {
    const auto uint8_arr = req->get_req_body();
    const DataType* array = static_cast<DataType*>((void*)uint8_arr.data());
    const auto size_bytes = req->get_req_body_size();
    const int size = size_bytes / sizeof(DataType);
    for(int i = 0; i < std::min(n, size); i++) {
      disp((int)array[i]);
      /* data[i] = array[i]; */
    }
    req->send();
  });
  disp(data);
}

template void Dash::sync_data(const std::string&, uint64_t*, int);
template void Dash::sync_data(const std::string&, uint32_t*, int);
template void Dash::sync_data(const std::string&, uint8_t*, int);
template void Dash::sync_data(const std::string&, uint16_t*, int);
template void Dash::sync_data(const std::string&, int64_t*, int);
template void Dash::sync_data(const std::string&, int32_t*, int);
template void Dash::sync_data(const std::string&, int8_t*, int);
template void Dash::sync_data(const std::string&, int16_t*, int);
template void Dash::sync_data(const std::string&, float*, int);
template void Dash::sync_data(const std::string&, double*, int);

//文字列のsplit機能
std::vector<std::string> split(const std::string str, const char del) {
  int first = 0;
  int last = str.find_first_of(del);
  std::vector<std::string> result;
  while(first < (int)str.size()) {
    std::string subStr(str, first, last - first);
    result.push_back(subStr);
    first = last + 1;
    last = str.find_first_of(del, first);
    if(last == (int)std::string::npos) {
      last = str.size();
    }
  }
  return result;
}

template <typename DataType> void Dash::sync_data_csv(const std::string& id, std::vector<DataType>& data) {
  Get("/sync/" + id, [&](PageIO *req) {
    std::vector<DataType> tmp;
    tmp.clear();
    const auto s = req->get_req_body_str();
    const auto strs = split(s, ',');
    for(auto&& t : strs) {
      try {
        tmp.push_back(std::stoi(t));
      } catch(const std::invalid_argument& e) {
        std::cout << "invalid argument" << std::endl;
      } catch(const std::out_of_range& e) {
        std::cout << "out of range" << std::endl;
      }
    }
    data = tmp;
    req->send();
  });
}

template void Dash::sync_data_csv(const std::string&, std::vector<uint32_t>&);

/* template<typename DataType> */
/* void Dash::sync_data(const std::string &id, std::vector<DataType> &data, int
 * n){ */
/* } */

/* [[noreturn]] void Dash::runserver() { */
/*   page_list.push_back(FlaskPagePack( */
/*       "/", [&](const auto req, const auto res) { main_page_func(req, res); })); */
/*   for (const auto &p : pages) */
/*     Get(p.uri, p.func); */
/*   while (true) */
/*     update(); */
/* } */

/* void Dash::register_db(const DB::Database &db) { */
/* const auto format_str = db.get_format(); */
/* Get("/format", [&]([[maybe_unused]] const auto req, */
/*                              const auto res) { res->send(format_str); }); */

/* for (auto &p : db.plots) { */
/*   const auto uri = "/data/" + p->get_id(); */
/*   Get( */
/*       uri, [&](const auto req, auto res) { p->_set_page_func(req, res); }); */
/* } */
/* } */

} // namespace Flask
