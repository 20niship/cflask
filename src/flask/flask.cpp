#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <flask/flask.hpp>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <regex>
#include <sstream>
#include <sys/fcntl.h>
#include <sys/socket.h>

#include <thread>
#include <unordered_map>
#define impl__closeSock(s) (::close(s))
#define impl__recvSock(s, buf, len, flags) ([[nodiscard]] ::recv((s), (buf), (len), (flags)))
#define impl__sendSock(s, buf, len, flags) ([[nodiscard]] ::send((s), (buf), (len), (flags)))
#define impl__setSockOpt(s, level, optname, optval, optlen) (::setsockopt((s), (level), (optname), (optval), (optlen)))

namespace Flask {

void Flask::Get(const std::string& url, PageFunc f) {
  page_configs.push_back(PageConfig(url, f, HttpMethods::GET));
}

void Flask::Get(const std::regex& url, PageFunc f) {
  page_configs.push_back(PageConfig(url, f, HttpMethods::GET));
}

void Flask::Post(const std::string& url, PageFunc f) {
  page_configs.push_back(PageConfig(url, f, HttpMethods::POST));
}

void Flask::Post(const std::regex& url, PageFunc f) {
  page_configs.push_back(PageConfig(url, f, HttpMethods::POST));
}

void Flask::Get(const std::string& url, Component* c) {
  auto pc = PageConfig(url, {c}, HttpMethods::GET);
  pc.set_flask_app(this);
  page_configs.push_back(std::move(pc));
}

void Flask::Get(const std::regex& url, Component* c) {
  auto pc = PageConfig(url, {c}, HttpMethods::GET);
  pc.set_flask_app(this);
  page_configs.push_back(std::move(pc));
}

void Flask::Post(const std::string& url, Component* c) {
  auto pc = PageConfig(url, {c}, HttpMethods::POST);
  pc.set_flask_app(this);
  page_configs.push_back(std::move(pc));
}

void Flask::Post(const std::regex& url, Component* c) {
  auto pc = PageConfig(url, {c}, HttpMethods::POST);
  pc.set_flask_app(this);
  page_configs.push_back(std::move(pc));
}

void Flask::Get(const std::string& url, std::initializer_list<Component*> c) {
  auto pc = PageConfig(url, c, HttpMethods::GET);
  pc.set_flask_app(this);
  page_configs.push_back(std::move(pc));
}

void Flask::Get(const std::regex& url, std::initializer_list<Component*> c) {
  auto pc = PageConfig(url, c, HttpMethods::GET);
  pc.set_flask_app(this);
  page_configs.push_back(std::move(pc));
}

void Flask::Post(const std::string& url, std::initializer_list<Component*> c) {
  auto pc = PageConfig(url, c, HttpMethods::POST);
  pc.set_flask_app(this);
  page_configs.push_back(std::move(pc));
}

void Flask::Post(const std::regex& url, std::initializer_list<Component*> c) {
  auto pc = PageConfig(url, c, HttpMethods::POST);
  pc.set_flask_app(this);
  page_configs.push_back(std::move(pc));
}

bool Flask::start(int port_) {
  std::cout << "* Serving Flask app" << std::endl;
  std::cout << "* Environment: production" << std::endl;
  std::cout << "\033[31m WARNING: This is a development server. Do not use "
               "it in a production deployment. \033[m"
            << std::endl;


  server_sock = socket(AF_INET6, SOCK_STREAM, 0);
  if(server_sock < 0) {
    std::cerr << "socket error";
    return false;
  }

  /*************************************************************/
  /* Allow socket descriptor to be reuseable                   */
  /*************************************************************/
  const int opt = 1;
  if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
    ERROR("Runtime Error in setsockopt");
    close(server_sock);
    return false;
  }

  /*************************************************************/
  /* Set socket to be nonblocking. All of the sockets for      */
  /* the incoming connections will also be nonblocking since   */
  /* they will inherit that state from the listening socket.   */
  /*************************************************************/
  if(ioctl(server_sock, FIONBIO, (char*)&opt) < 0) {
    ERROR("ioctl failed!");
    close(server_sock);
    return false;
  }

  struct sockaddr_in6 addr;
  addr.sin6_family = AF_INET6;
  memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
  addr.sin6_port = htons(port_);
  if(bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    close(server_sock);
    ERROR("Bind Error : " + std::to_string(port_) + "port already in use!!");
    return false;
  }

  if(listen(server_sock, 32) != 0) {
    ERROR("Bind Error : " + std::to_string(port_) + "port already in use!!");
    close(server_sock);
    return false;
  }

  const bool isDebugMode = true;

  std::cout << "* Debug mode: " << (isDebugMode ? "ON" : "OFF") << std::endl;
  std::cout << "* Restarting with stat" << std::endl;
  if(isDebugMode) std::cout << "* Debugger is active!" << std::endl;
  std::cout << "* Running on http://127.0.0.1:" << std::to_string(port_) << "/ (Press CTRL+C to quit)" << std::endl;
  return true;
}


void Flask::report() {
  std::cout << "------------  Flask Webserver Config Report ----------" << std::endl;
  std::cout << "functions  : " << page_configs.size() << std::endl;
}

std::vector<struct pollfd> Flask::get_fds() {
  std::vector<struct pollfd> fds;
  pollfd listen_fd;
  listen_fd.fd = server_sock;
  listen_fd.events = POLLIN;
  fds.push_back(listen_fd);
  for(auto&& p : pages) {
    pollfd fd;
    fd.fd = p.io.get_socket();
    fd.events = POLLIN;
    fds.push_back(std::move(fd));
  }
  return fds;
}

PageIO* Flask::get_io_ptr(const int sock) {
  for(auto&& p : pages) {
    if(p.io.get_socket() == sock) return &(p.io);
  }
  return nullptr;
}

void Flask::update() {
  for(size_t i = 0; i < pages.size(); i++) {
    auto&& p = pages[i];
    if(p.io.closed()) {
      pages.erase(pages.begin() + i);
      continue;
    }

    if(p.io.has_path() && p.config == nullptr) {
      // AddPage関数でページを保存した場合ここで実行される
      bool found_page = false;
      for(auto&& pcnf : page_configs) {
        p.config = &pcnf;
        if(!pcnf.match(p.io.get_path(), p.io.get_method())) continue;
        pcnf.run(&p.io);
        found_page = true;
      }
      if(!found_page) {
        p.io.status(404);
        const auto method = p.io.get_method_str();
        const auto ver = p.io.get_http_ver();
        const auto path = p.io.get_path();
        p.io.send("404 Not Found  : " + method + " / " + ver + " request to " + path);
        ERROR("Page Not Found : " + method + "/ " + ver + "--> " + path);
      }
      INFO("[new request] " + p.io.get_method_str() + "\t" + p.io.get_path() + "\t" + p.io.get_http_ver() + " ");
    }
  }

  auto fds = get_fds();
  const auto poll_result = poll(fds.data(), fds.size(), timeout);
  if(poll_result == 0) return; // timeout
  if(poll_result < 0) {
    ERROR("Poll Error in runserver");
    return;
  }
  for(size_t i = 0; i < fds.size(); i++) {
    auto&& fd = fds[i];
    if(fd.revents == 0) continue;
    if(fd.revents != POLLIN) continue;

    if(fd.fd == server_sock) {
      sockaddr_in client_addr;
      int len = sizeof(client_addr);
      const int new_sd = accept(server_sock, (struct sockaddr*)&client_addr, (socklen_t *)&len);
      if(new_sd < 0) {
        if(errno == EWOULDBLOCK || errno == EAGAIN) break;
        ERROR("accept new connection failed");
      } else {
        const auto ipaddr = inet_ntoa(client_addr.sin_addr);
        const auto port = (int)ntohs(client_addr.sin_port);
        if(get_io_ptr(new_sd) != nullptr) {
          close(new_sd);
          return;
        }
        auto page = Page(new_sd);
        page.io.set_port(port);
        page.io.set_ip_addr(ipaddr);
        pages.push_back(Page(new_sd));
      }
    } else {
      auto io = get_io_ptr(fd.fd);
      if(io == nullptr) continue;
      io->receive_new_data();
      /* close(fd.fd); */
      /* std::cout << "connection Closed" << fd.fd << std::endl; */
      /* fd.fd = -1; */
      /* fds.erase(fds.begin() + i); */
    }
  }
}

void Flask::StaticDir(const std::string local, const std::string uri) {
  page_configs.push_back(PageConfig(std::regex("\\" + uri + "\\/*"), [&](PageIO*) {
    std::cout << "NOT IMPLEMENTED!!!" << std::endl;
    std::cout << "StaticDIR" << local << "< " << uri << std::endl;
    /* const int beginIdx = req->path.rfind("/"); */
    /* const std::string path = local + req->path.substr(beginIdx); */
    /* res->serve_file(path); */
  }));
}

void Flask::close_all() {
  for(auto&& p : pages) p.io.close();
}

Flask::~Flask() {
  impl__closeSock(server_sock);
}

} // namespace Flask
