#include <flask/pageconfig.hpp>
#include <flask/utils.hpp>
#include <regex>

namespace Flask{
  bool PageConfig::match(const std::string& url, const HttpMethods& method_) const {
    if(!method_acc(method_)) return false;
    return std::regex_match(url, path);
  }
}
