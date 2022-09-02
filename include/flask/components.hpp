#pragma once
#include <cstddef>
#include <cstring>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
#include <regex>
#include <assert.h>
#include <flask/pageio.hpp>

namespace Flask {

class Flask;
using PageFunc = std::function<void(PageIO*)>;

class Component {
protected:
  std::vector<Component*> child;
  std::string inner_text;
  std::string tag_text;
  std::vector<std::string> class_names;
  std::string id;
  std::string surround_tag(const std::string& str_) const; // タグで囲んだテキストを表示する
  std::regex base_path;

public:
  virtual void __set_flask_app(Flask* app) = 0;
  void reset() {
    child.clear();
    inner_text = "";
  }
  Component() { reset(); }
  Component(const std::string& str_) { inner_text = str_; };
  void set_innerText(const std::string& str_) { inner_text = str_; }
  std::string get_innerText() const { return inner_text; }
  void set_base_path(const std::regex &path) { base_path = path; }
  virtual std::string render(PageIO *io) = 0;
  auto get_child(){return child;}
};

// clang-format off
#define DASHCPP_REGISTER_SIMPLE_COMPONENTS(component_name, tag_text_name) \
class component_name : public Component{ \
public:\
  component_name() {reset(); tag_text = tag_text_name;} \
  component_name(const std::string& str_){ reset(); tag_text =tag_text_name; inner_text = str_; } \
  virtual std::string render(PageIO *) override { return surround_tag(inner_text);}\
  virtual void __set_flask_app(Flask*)override{}; \
};
// clang-format on

DASHCPP_REGISTER_SIMPLE_COMPONENTS(H1, "h1")
DASHCPP_REGISTER_SIMPLE_COMPONENTS(H2, "h2")
DASHCPP_REGISTER_SIMPLE_COMPONENTS(H3, "h3")
DASHCPP_REGISTER_SIMPLE_COMPONENTS(H4, "h4")
DASHCPP_REGISTER_SIMPLE_COMPONENTS(H5, "h5")
DASHCPP_REGISTER_SIMPLE_COMPONENTS(H6, "h6")
DASHCPP_REGISTER_SIMPLE_COMPONENTS(body, "body")
DASHCPP_REGISTER_SIMPLE_COMPONENTS(Hr, "hr")
DASHCPP_REGISTER_SIMPLE_COMPONENTS(Paragraph, "p")
DASHCPP_REGISTER_SIMPLE_COMPONENTS(Bold, "b")

class Div : public Component {
public:
  Div() { child.clear(); }
  Div(std::initializer_list<Component*> child_components) {
    reset();
    tag_text = "div";
    for(auto l : child_components) child.push_back(l);
  }
  virtual std::string render(PageIO *io) override {
    std::string st = "";
    for(const auto c : child) st += c->render(io);
    return st;
  }
  virtual void __set_flask_app(Flask*) override{};
};

class Link : public Component {
private:
  std::string uri;

public:
  Link() = delete;
  Link(const std::string& str, const std::string& uri_) {
    reset();
    inner_text = str;
    uri = uri_;
    tag_text = "a";
  }
  virtual std::string render(PageIO *) override { return "<a href=\"" + uri + "\">" + inner_text + "</a>"; }
  virtual void __set_flask_app(Flask*) override{};
};

/* class Canvas : public Component { */
/* public: */
/*   Canvas() { */
/*     reset(); */
/*     tag_text = "canvas"; */
/*   }; */
/*   virtual void __set_flask_app(Flask*){}; */
/*   Canvas(const std::string& id, const Database& db); */
/* }; */

class RawString : public Component {
public:
  RawString() {
    reset();
    inner_text = "";
    tag_text = "";
  }
  RawString(const std::string& str_) { inner_text = str_; }
  virtual void __set_flask_app(Flask*) override{};
  virtual std::string render(PageIO *) override { return inner_text; }
};
#define disp(A) (std::cout << #A << " = " << (A) << "\n")

class StaticFile : public Component {
private:
  std::string fname;
  std::string read_file() const {
    std::ifstream ifs(fname);
    if(!ifs.is_open()) {
      std::cout << "file " + fname + "is not found!!";
      return "<p>file not found! : " + fname + "</p>";
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
  }

public:
  StaticFile() = delete;
  StaticFile(const std::string& filename) {
    reset();
    fname = filename;
  }
  virtual void __set_flask_app(Flask*) override{};
  virtual std::string render(PageIO *) override { return read_file(); }
};

// ----------------------------------------------------------------
//   Callbacks
// ----------------------------------------------------------------
class StaticDir : public Component {
private:
  std::string dir;

public:
  StaticDir(const std::string& d) { dir = d; }
  StaticDir() { dir = "./template"; }
  virtual void __set_flask_app(Flask*) override{};
  virtual std::string render(PageIO *io) override {
    const std::string path = io->get_path();
    const int beginIdx = path.rfind("/");
    std::string file_path = dir + path.substr(beginIdx);
    std::smatch results;
    if(std::regex_match(path, results, base_path) && results.size() > 1) file_path = dir + "/" + results[1].str();
    io->serve_file(file_path);
    return "";
  };
};

class Multer : public Component {
private:
  std::string destination;

public:
  Multer(const std::string dest) { destination = dest; }
  Multer() = delete;
  virtual void __set_flask_app(Flask*) override{};
  std::string render(PageIO *) override;
};

class Stream : public Component {
private:
  std::vector<PageIO *> res_list;
  bool _locked;
  std::unordered_map<std::string, std::string> header_list;
  //  ストリームを閉じます。
  void reset();
  void close(PageIO * res);
  void send_raw(const void* data, int size);
  void send_header();
  void send_impl(const void* data, const int size);

public:
  Stream() { reset(); }
  // WritableStream がライターにロックされているかどうかを示すブール値。
  bool locked() const { return _locked; }
  bool closed() const { return res_list.size() == 0; }
  virtual void __set_flask_app(Flask*) override{};
  std::string render(PageIO *io) override {
    res_list.push_back(io);
    return "";
  }
  // ストリームを中止し、プロデューサーがストリームに正常に書き込むことができなくなり、キューに入れられた書き込みが破棄されてすぐにエラー状態に移行することを通知します。
  void abort();
  void pipe(PageIO *io) { res_list.push_back(io); }
  void add_header(const std::string& key, const std::string& value) { header_list.emplace(key, value); }
  void clear_header() { header_list.clear(); }

  void send_with_chunk_size(const void* data, int size) {
    for(auto&& res : res_list) {
      assert(res != nullptr);
      const std::string size_str = std::to_string(size) + "\r\n";
      const int bufsize = size + size_str.size();
      char* buf = (char*)malloc(bufsize);
      strcpy(buf, size_str.c_str());
      strcpy(buf + size_str.size(), (char*)data);
      res->send_raw_keep_alive(data, size);
    }
  }

  // WritableStreamDefaultWriter の新しいインスタンスを返し、そのインスタンスにストリームをロックします。
  // ストリームがロックされている間、このライターが開放されるまで他のライターを取得することはできません。 void getWriter(){}

  void send(const void* data, int size) { send_impl(data, size); }
  void send(const std::string& str) { send_impl(str.data(), str.size()); }
  void send(const char* str) { send(std::string(str)); }
  template <typename T> Stream& operator<<(const T var_) {
    send(std::to_string(var_));
    return *this;
  }
};

} // namespace Flask
