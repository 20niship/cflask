#pragma once


namespace Flask::Core {

class pollfd_wrap {
private:
  class Impl;

public:
  pollfd_wrap(int sock);
};

[[nodiscard]] int listen(const int sock, const int backlog);
[[nodiscard]] int accept(const int sock);
[[nodiscard]] int recv(const int sock, char* buf, const int len, const int flags);
[[nodiscard]] int send(const int sock, const char* buf, const int len, const int flags);
int close(const int sock);
} // namespace Flask::Core
