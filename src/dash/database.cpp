#include <chrono>
#include <cmath>
#include <dash/database.hpp>
#include <sstream>
#include <thread>


#if 0
namespace Flask {

template <typename T> std::string getType() {
  static_assert("not defined type!");
  return "";
}
template <> std::string getType<int64_t>() {
  return "int64";
}
template <> std::string getType<uint64_t>() {
  return "uint64";
}
template <> std::string getType<float>() {
  return "float";
}
template <> std::string getType<double>() {
  return "double";
}

template <typename T> Rectangle<T>::Rectangle(const std::string& id_, const T x_, const T y_, const T w_, const T h_, const int duration_) {
  id = id_;
  x = x_;
  y = y_;
  width = w_;
  height = h_;
  duration = duration_;
}

template <typename T> std::string Rectangle<T>::__get_format() const {
  std::stringstream ss;
  ss << "{type : \"" << getType<T>() << "\",";
  ss << " length: 4, ";
  ss << " class : \"Rectangle\"}";
  return ss.str();
}

template <typename T> void Rectangle<T>::_set_page_func(const HTTPRequest*, HTTPresponse* res) {
  if(duration == DASH_SEND_DATA_MANUAL) {
    stream_obj.abort();
    stream_obj.pipe(res);
  } else if(duration == DASH_SEND_DATA_ONCE) {
    T data[] = {x, y, width, height};
    res->send_raw(data, 4 * sizeof(T));
  } else {
    std::thread t([&] {
      this->send();
      std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    });
    t.detach();
  }
}

template <typename T> void Rectangle<T>::send() {
  if(duration != DASH_SEND_DATA_MANUAL) {
    std::cout << "Dataabse " + id + "is NOT SET MANUAL!" << std::endl;
  } else {
    T data[] = {x, y, width, height};
    stream_obj.send(data, 4 * sizeof(T));
  }
}
template class Rectangle<float>;
template class Rectangle<double>;
template class Rectangle<uint64_t>;
template class Rectangle<int64_t>;
template class Rectangle<int>;

template <typename T> CustomArray<T>::CustomArray(const std::string& id_, const std::vector<T>& arr_, const int duration_) {
  id = id_;
  duration = duration_;
  arr = arr_;
}

template <typename T> std::string CustomArray<T>::__get_format() const {
  std::stringstream ss;
  ss << "{type : \"" << getType<T>() << "\",";
  ss << " class : \"CustomArray\"}";
  return ss.str();
}

template <typename T> void CustomArray<T>::_set_page_func(const HTTPRequest*, HTTPresponse* res) {
  if(duration == DASH_SEND_DATA_MANUAL) {
    stream_obj.abort();
    stream_obj.pipe(res);
  } else if(duration == DASH_SEND_DATA_ONCE) {
    res->send_raw(arr.data(), arr.size() * sizeof(T));
  } else {
    std::thread t([&] {
      this->send();
      std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    });
    t.detach();
  }
}

template <typename T> void CustomArray<T>::send() {
  if(duration != DASH_SEND_DATA_MANUAL) {
    std::cout << "Dataabse " + id + "is NOT SET MANUAL!" << std::endl;
  } else {
    stream_obj.send((void*)arr.data(), arr.size() * sizeof(T));
  }
}

template class CustomArray<float>;
template class CustomArray<double>;
template class CustomArray<uint64_t>;
template class CustomArray<int64_t>;
template class CustomArray<int>;


std::string Database::get_format() const {
  std::string db_format = "{format : [";
  for(auto& p : plots) db_format += p->__get_format() + ", ";
  db_format += "]}";
  return db_format;
}
} // namespace Flask
  //
#endif 
