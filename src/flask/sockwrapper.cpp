#include <flask/sockwrapper.hpp>

#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

namespace Flask::Core {

int listen(const int sock, const int backlog) {
  return ::listen(sock, backlog);
}
/* int accept(const int sock){return ::accept(sock); } */
int recv(const int sock, char* buf, const int len, const int flags) {
  return ::recv(sock, buf, len, flags);
}
int send(const int sock, const void* buf, const int len, const int flags) {
  return ::send(sock, buf, len, flags);
}
int close(const int sock) {
  return ::close(sock);
}

} // namespace Flask::Core
