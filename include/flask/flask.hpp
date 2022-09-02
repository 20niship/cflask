#pragma once
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <flask/webenum.hpp>
#include <flask/pageio.hpp>
#include <flask/utils.hpp>
#include <flask/pageconfig.hpp>

namespace Flask {
class PageIO;

// using RequestArgs =  std::unordered_map<std::string, std::string>;

class Flask {
protected:
  int server_sock;
  int timeout = 1000;

  struct Page {
    PageIO io;
    PageConfig* config{nullptr};
    Page(const int socket) : io(socket) { config = nullptr; }
    pollfd get_pollfd() {
      pollfd fd;
      fd.fd = io.get_socket();
      fd.events = POLLIN;
      return fd;
    }
  };

  std::vector<PageConfig> page_configs;
  std::vector<Page> pages;
  // std::unordered_map<std::string, PageFunc> PageList;
  //     /** Returns true on success, or false if there was an error */
  //     bool SetSocketBlockingEnabled(int fd, bool blocking{
  //       if (fd < 0) return false;
  // #ifdef _WIN32
  //       unsigned long mode = blocking ? 0 : 1;
  //       return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
  // #else
  //       int flags = fcntl(fd, F_GETFL, 0);
  //       if (flags == -1)
  //         return false;
  //       flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
  //       return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
  // #endif
  //     }
  void add_io(const int);
  PageIO* get_io_ptr(const int sock);
  std::vector<struct pollfd> get_fds();

public:
  Flask() = default;
  bool start(const int port_ = 8080);
  [[noreturn]] void runserver();
  void update();
  void report();
  void close_all();
  void set_timeout(const int ms) { timeout = ms; }
  void Get(const std::string& url, PageFunc f);
  void Get(const std::regex& url, PageFunc f);
  void Post(const std::string& url, PageFunc f);
  void Post(const std::regex& url, PageFunc f);
  void Get(const std::string& url, std::initializer_list<Component*>);
  void Get(const std::regex& url, std::initializer_list<Component*>);
  void Post(const std::string& url, std::initializer_list<Component*>);
  void Post(const std::regex& url, std::initializer_list<Component*>);
  void Get(const std::string& url, Component*);
  void Get(const std::regex& url, Component*);
  void Post(const std::string& url, Component*);
  void Post(const std::regex& url, Component*);
  void StaticDir(const std::string local, const std::string uri);
  ~Flask();
};

} // namespace Flask
