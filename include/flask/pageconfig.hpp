#pragma once

#include <assert.h>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <numeric>
#include <random>
#include <regex>
#include <utility>
#include <vector>

#include <flask/pageio.hpp>
#include <flask/components.hpp>

namespace Flask {

class Flask;

struct PageConfig {
  static void NOP(PageIO*) {}
  PageFunc func = NOP;
  std::vector<Component*> comp;
  std::regex path;
  std::vector<HttpMethods> accept_methods{HttpMethods::GET};

  void set_base_path(Component* c) {
    c->set_base_path(path);
    for(auto cc : c->get_child()) set_base_path(cc);
  }
  void set_flask_app(Flask* app, Component* c) {
    c->__set_flask_app(app);
    for(auto cc : c->get_child()) set_flask_app(app, cc);
  }
  void set_base_path() {
    for(auto cc : comp) set_base_path(cc);
  }
  void set_flask_app(Flask* app) {
    for(auto cc : comp) set_flask_app(app, cc);
  }

  PageConfig() = delete;
  PageConfig(const std::string& url, const PageFunc& f, const HttpMethods met = HttpMethods::GET) {
    func = f;
    path = url;
    accept_methods = {met};
    comp.clear();
  }
  PageConfig(const std::regex& reg, const PageFunc& f, const HttpMethods met = HttpMethods::GET) {
    func = f;
    path = reg;
    comp.clear();
    accept_methods = {met};
  }
  PageConfig(const std::string& url, std::initializer_list<Component*> comps, const HttpMethods met) {
    path = std::regex(url);
    accept_methods = {met};
    comp.clear();
    for(auto&& c : comps) comp.push_back(c);
    set_base_path();
  }
  PageConfig(const std::regex& reg, std::initializer_list<Component*> comps, const HttpMethods met) {
    func = NOP;
    path = reg;
    comp.clear();
    accept_methods = {met};
    comp.clear();
    for(auto&& c : comps) comp.push_back(c);
    set_base_path();
  }

  void set_methods(const std::initializer_list<HttpMethods> m) {
    accept_methods.clear();
    for(auto&& m_ : m) accept_methods.push_back(m_);
  }

  bool method_acc(const HttpMethods& method_) const {
    for(auto&& m : accept_methods)
      if(m == method_) return true;
    return false;
  }

  bool match(const std::string& url, const HttpMethods& method_) const;

  bool run(PageIO* io) {
    std::string st = "";
    if(comp.size() > 0) {
      for(auto&& c : comp) st += c->render(io);
      if(!io->closed() && st != "") {
        io->header("Content-Type", "text/html; charset=utf-8");
        io->send(st);
      }
    } else {
      func(io);
    }
    return true;
  }
};
} // namespace Flask
