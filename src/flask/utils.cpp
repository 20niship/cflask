
#include <flask/flask.hpp>
#include <iostream>
#include <mutex>
#define impl__closeSock(s) (::close(s))
#define impl__recvSock(s, buf, len, flags) ([[nodiscard]] ::recv((s), (buf), (len), (flags)))
#define impl__sendSock(s, buf, len, flags) ([[nodiscard]] ::send((s), (buf), (len), (flags)))
#define impl__setSockOpt(s, level, optname, optval, optlen) (::setsockopt((s), (level), (optname), (optval), (optlen)))

namespace Flask {
const std::unordered_map<int, std::string> status_codes_desc_map = {
  {100, "Continue"},
  {101, "Switching Protocols"},
  {102, "Processing"},
  {200, "OK"},
  {201, "Created"},
  {202, "Accepted"},
  {203, "Non-Authoritative Information"},
  {204, "No Content"},
  {205, "Reset Content"},
  {206, "Partial Content"},
  {207, "Multi-Status"},
  {208, "Already Reported"},
  {300, "Multiple Choices"},
  {301, "Moved Permanently"},
  {302, "Found"},
  {303, "See Other"},
  {304, "Not Modified"},
  {305, "Use Proxy"},
  {307, "Temporary Redirect"},
  {400, "Bad Request"},
  {401, "Unauthorized"},
  {402, "Payment Required"},
  {403, "Forbidden"},
  {404, "Not Found"},
  {405, "Method Not Allowed"},
  {406, "Not Acceptable"},
  {407, "Proxy Authentication Required"},
  {408, "Request Timeout"},
  {409, "Conflict"},
  {410, "Gone"},
  {411, "Length Required"},
  {412, "Precondition Failed"},
  {413, "Request Entity Too Large"},
  {414, "Request-URI Too Large"},
  {415, "Unsupported Media Type"},
  {416, "Request Range Not Satisfiable"},
  {417, "Expectation Failed"},
  {418, "I'm a teapot"},
  {422, "Unprocessable Entity"},
  {423, "Locked"},
  {424, "Failed Dependency"},
  {425, "No code"},
  {426, "Upgrade Required"},
  {428, "Precondition Required"},
  {429, "Too Many Requests"},
  {431, "Request Header Fields Too Large"},
  {449, "Retry with"},
  {500, "Internal Server Error"},
  {501, "Not Implemented"},
  {502, "Bad Gateway"},
  {503, "Service Unavailable"},
  {504, "Gateway Timeout"},
  {505, "HTTP Version Not Supported"},
  {506, "Variant Also Negotiates"},
  {507, "Insufficient Storage"},
  {509, "Bandwidth Limit Exceeded"},
  {510, "Not Extended"},
  {511, "Network Authentication Required"},
};
std::string get_statuscode_desc(const int code) {
  if(status_codes_desc_map.find(code) == status_codes_desc_map.end()) return "";
  return status_codes_desc_map.at(code);
}

inline std::string render_template_file(const std::string& fname, const RenderValues&) {
  std::ifstream ifs("template/" + fname);
  if(!ifs.is_open()) { // file not found
    return FLASK_404_ERROR_HTML_TEXT;
  }
  std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  return str;
}

std::string render_template_text(const std::string& text, RenderValues) {
  return text;
}

void read_file(const std::string& path, std::string& out) {
  std::ifstream fs(path, std::ios_base::binary);
  fs.seekg(0, std::ios_base::end);
  auto size = fs.tellg();
  fs.seekg(0);
  out.resize(static_cast<size_t>(size));
  fs.read(&out[0], static_cast<std::streamsize>(size));
}


std::ofstream log_file;
std::mutex file_log_mtx;

void set_log_file(const std::string& f) {
  if(log_file.is_open()) log_file.close();
  log_file.open(f, std::ios::out | std::ios::trunc);
  if(!log_file.is_open()) {
    ERROR("ファイルを開けませんでした：" + f);
  }
}
void showLog(const std::string& pre, const std::string& suf, const std::string msg) {
  std::stringstream ss;
  ss << pre << msg << suf << std::endl;
  std::lock_guard<std::mutex> lock{file_log_mtx};
  if(log_file.is_open()) {
    log_file << ss.str();
    log_file << std::flush;
  } else {
    std::cout << ss.str();
  }
}
void INFO(const std::string& message) {
  showLog("", "", message);
}
void DEBUG(const std::string& message) {
  showLog("\x1b[32m", "\x1b[0m", message);
}
void WARNING(const std::string& message) {
  showLog("\x1b[33m", "\x1b[0m", message);
}
void ERROR(const std::string& message) {
  showLog("\x1b[31m", "\x1b[0m", message);
}

std::string html_encode(const std::string& value) {
  std::string buf;
  buf.reserve(value.size());
  for(size_t i = 0; i != value.size(); ++i) {
    switch(value[i]) {
      case '&': buf.append("&amp;"); break;
      case '\"': buf.append("&quot;"); break;
      case '\'': buf.append("&apos;"); break;
      case '<': buf.append("&lt;"); break;
      case '>': buf.append("&gt;"); break;
      default: buf.append(&value[i], 1); break;
    }
  }
  return buf;
}


std::string toLower(const std::string& t) {
  std::string s = t;
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
  return s;
}

HttpMethods cvt_str_to_http_method(const std::string& m) {
  const auto lower = toLower(m);
  if(lower == "get") return HttpMethods::GET;
  if(lower == "post") return HttpMethods::POST;
  if(lower == "head") return HttpMethods::HEAD;
  if(lower == "connect") return HttpMethods::CONNECT;
  if(lower == "put") return HttpMethods::PUT;
  if(lower == "delete") return HttpMethods::DELETE;
  return HttpMethods::UNKNOWN;
}

std::string cvt_method_to_str(const HttpMethods& m) {
  switch(m) {
    case HttpMethods::GET: return "GET";
    case HttpMethods::POST: return "POST";
    case HttpMethods::DELETE: return "DELETE";
    case HttpMethods::PUT: return "PUT";
    case HttpMethods::HEAD: return "HEAD";
    case HttpMethods::CONNECT: return "CONNECT";
    default: return "";
  }
}
} // namespace Flask
