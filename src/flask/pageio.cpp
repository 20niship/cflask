#include <filesystem>
#include <flask/flask.hpp>
#include <flask/utils.hpp>
#include <flask/webenum.hpp>
#include <fstream>
#include <tuple>
#include <unordered_map>

#define impl__closeSock(s) (::close(s))
#define impl__recvSock(s, buf, len, flags) (::recv((s), (buf), (len), (flags)))
#define impl__sendSock(s, buf, len, flags) (::send((s), (buf), (len), (flags)))
#define impl__setSockOpt(s, level, optname, optval, optlen) (::setsockopt((s), (level), (optname), (optval), (optlen)))

namespace Flask {
void PageIO::init() {
  _closed = sent_body = header_send_done = false;
  _status = StatusCodes::OK;
  domain = "";
  host = "";
  path = "";
  scheme = "";
  uri = "";
  extention = "";
  http_ver = "";

  // setCurrentTime();
  // _status = status_;
  // switch (_status){
  // case StatusCodes::Moved_Parmanently: //Moved Permanently
  // 恒久的に移動した。 case StatusCodes::Found: //Found case
  // StatusCodes::See_Other: //See Other
  //   header("Location", content);
  //   break;
  // default:
  //   context = content;
  // }
}

void PageIO::setCurrentTime() {
  return; // TODO ここでセグフォするのなんで！？
  const std::string dayofweek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  const std::string months[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec"};
  const time_t t = time(NULL);
  struct tm tm;
  localtime_r(&t, &tm);
  // https://www.mm2d.net/main/prog/c/time-01.html
  std::stringstream ss;
  ss << dayofweek[tm.tm_mday] << "," << tm.tm_mday << " " << months[tm.tm_mon] << " " << tm.tm_year << " " << tm.tm_hour << ":" << tm.tm_min << ":" << tm.tm_sec << " GMT";
  res_headers["Date"] = ss.str();
}

std::string PageIO::get_header_text() const {
  // if(isSendHeader) return "";
  const auto desc = get_statuscode_desc((int)_status);
  std::string h_out = "HTTP/1.1 " + std::to_string((int)_status) + " " + desc + "\r\n";
  // setCurrentTime();
  for(auto itr = res_headers.begin(); itr != res_headers.end(); ++itr) h_out += itr->first + ": " + itr->second + "\r\n";
  h_out += "\r\n";
  return h_out;
}

bool PageIO::send_header() {
  const auto h = get_header_text();
  const bool connected = send_raw_keep_alive(h.data(), h.size());
  header_send_done = true;
  return connected;
}

// PageIO(StatusCodes status, std::string type, std::string content){
// createResponse(status, type, content); } PageIO(std::string content){
// createResponse(StatusCodes::OK, FLASK_RESPONSE_CONTENT_TYPE_HTML, content);
// } PageIO(StatusCodes status_code){
//   createResponse(status_code, FLASK_RESPONSE_CONTENT_TYPE_HTML, "");
//   switch((int)status_code){
//     case (int)StatusCodes::Not_Found:
//       context = FLASK_404_ERROR_HTML_TEXT;
//       break;
//   }
// }

void PageIO::header(const std::string& key, const std::string& value) {
  res_headers.insert({key, value});
}
void PageIO::status(const StatusCodes s) {
  _status = s;
}
void PageIO::status(const int s) {
  status(static_cast<StatusCodes>(s));
}
void PageIO::close() {
  if(_closed) return;
  impl__closeSock(_sock);
  _closed = true;
}
int PageIO::get_socket() {
  return _sock;
}

std::vector<char> PageIO::get_req_body() const {
  return std::vector<char>(req_data.begin() + body_start_idx, req_data.end());
}
std::string PageIO::get_req_body_str() const {
  const auto body = get_req_body();
  return std::string(body.begin(), body.end());
}

int PageIO::get_req_body_size() const {
  return req_body_size;
}

void PageIO::send(const std::string& msg) {
  if(_closed) return;
  const std::string allstr = get_header_text() + msg;
  impl__sendSock(_sock, allstr.data(), allstr.size(), 0);
  header_send_done = true;
  header("Content-Length", std::to_string(msg.size()));
  this->close();
}

void PageIO::send(const void*, const size_t) {
  std::cout << "NOT IMPLEMENTED!!" << std::endl;
  std::cout << "PageIO::send(ptr, size);";
  exit(1);
}

void PageIO::send_raw(const void* ptr, const size_t size) {
  if(_closed) return;
  impl__sendSock(_sock, ptr, size, 0);
  this->close();
}

bool PageIO::send_raw_keep_alive(const void* ptr, const size_t size) {
  if(_closed) return false;
  const int rc = impl__sendSock(_sock, ptr, size, MSG_NOSIGNAL);
  if(rc < 0) {
    close();
    return false;
  }
  return true;
}

/* void PageIO::send_raw(cv::Mat img){ */
/*   if(_closed) return; */
/*   this->close(); */
/* } */

void PageIO::serve_file(const std::string filename) {
  if(_closed) {
    std::cout << "socket closed" << std::endl;
    return;
  }

  // templateフォルダ中に該当するファイルが存在する場合
  std::ifstream ifs(filename, std::ios::in | std::ios::binary);
  if(!ifs.is_open()) { // file not found
    status(StatusCodes::Not_Found);
    send("File not found " + filename);
    ERROR("File Not Found : " + filename);
    return;
  }

  const std::unordered_map<std::string, std::string> content_types = {
    {"txt", "text/plain; charset=utf-8"},
    {"html", "text/html; charset=utf-8"},
    {"js", "text/javascript"},
    {"json", "text/json"},
    {"png", "image/png"},
    {"jpg", "image/jpg"},
    {"jpeg", "image/jpeg"},
    {"gif", "image/gif"},
    {"css", "text/css"},
  };

  auto extention = filename.substr(filename.rfind(".") + 1);

  std::string content_type = "application/octet-stream"; // binary file
  // Content-Type: application/force-download 共生ダウンロードさせたい時
  // TODO:
  // ファイルの非同期ダウンロードhttps://qiita.com/kawasima/items/2231f607677af31234e7
  if(content_types.find(extention) != content_types.end()) {
    content_type = content_types.at(extention);
    // auto res = PageIO(StatusCodes::Unsopported_Media_Type);
    // auto str = res.to_text_all();
    // impl__sendSock(_sock, str.data(), str.size(), 0);

    // ifs.close();
    // return false;
  }

  auto req_body_size = std::filesystem::file_size(filename);
  header("Content-Type", content_type); // content_types.at(extention));
  header("Content-Length", std::to_string(req_body_size));
  header("Server", "Werkzeug/1.0.1 Python/3.7.3");
  const std::string hstr = get_header_text();
  impl__sendSock(_sock, hstr.data(), hstr.size(), 0);

  char buf[1024];
  while(!ifs.eof() && ifs && ifs.peek() != EOF) {
    size_t data_size = ifs.read(buf, sizeof(buf)).gcount();
    impl__sendSock(_sock, buf, data_size, 0);
  }
  ifs.close();
  close();
}

// TODO: render ejs
void PageIO::render(const std::string filename, const RenderValues&) {
  serve_file(filename);
}

void PageIO::redirect(const std::string new_url) {
  status(StatusCodes::Moved_Parmanently);
  header("Content-Type", "text/plain; charset=utf-8");
  header("Location", new_url);
  send();
}


PageIO::PageIO(const int sock_) : _sock(sock_) {
  init();
}

void PageIO::receive_new_data() {
  const int new_size_max = 1024;
  const int s = req_data.size();
  req_data.resize(s + new_size_max);
  std::fill(req_data.begin() + s, req_data.end(), 0);
  const int rret = impl__recvSock(_sock, req_data.data() + s, new_size_max, 0);
  if(rret < 0) {
    _valid = false;
    close();
    return;
  }
  if(rret == 0) this->close();
  req_data.resize(s + rret);
  parse_request();
  const auto desired_len_str = get_req_header("content-length");
  if(desired_len_str == "") {
    return;
  } else {
    const int desired_len = std::stoi(desired_len_str);
    if(desired_len <= (int)req_data.size()) {
      /* _valid = true; */
      received_all = true;
    }
  }
}


std::tuple<std::unordered_map<std::string, std::string>, int> parse_split2(const std::string_view& str, const std::string& split1, const std::string& split2) {
  std::string::size_type cursor = 0;
  constexpr auto npos = std::string::npos;
  std::unordered_map<std::string, std::string> res;
  const auto to_lower = [](const std::string_view& upper) {
    std::string lower;
    std::transform(upper.begin(), upper.end(), std::back_inserter(lower), [](unsigned char c) { return std::tolower(c); });
    return lower;
  };
  while(true) {
    const auto b = str.find(split1, cursor);
    if(b == npos) break;
    const auto a = str.find(split2, b);
    if(a == npos) break;
    if(a <= b) break;
    const auto&& key = str.substr(cursor, b - cursor);
    const auto&& value = str.substr(b + 1, a - b - 1);
    res.emplace(to_lower(key), to_lower(value));
    cursor = a + 1;
  }
  return {res, cursor};
}

void PageIO::parse_request() {
  char meth_name_[16];
  char uri_addr_[256];
  char http_ver_[64];

  sscanf((char*)req_data.data(), "%s %s %s", meth_name_, uri_addr_, http_ver_);
  method = cvt_str_to_http_method(meth_name_);
  uri = uri_addr_;
  http_ver = http_ver_;

  const auto args_pos = uri.find("?");
  const auto frag_pos = uri.find("#");

  // pathの設定
  auto path_end = uri.size();
  if(frag_pos != std::string::npos) path_end = frag_pos;
  if(args_pos != std::string::npos) path_end = args_pos;
  path = uri.substr(0, path_end);

  //拡張子の設定
  auto extention_pos = path.rfind(".");
  if(extention_pos == std::string::npos)
    extention = "";
  else
    extention = path.substr(extention_pos + 1);

  // req_argsの設定
  if(args_pos != std::string::npos) {
    const auto args_end_pos = (frag_pos != std::string::npos) ? (frag_pos - args_pos - 1) : std::string::npos;
    std::string ss = uri.substr(args_pos + 1, args_end_pos);
    /* const auto [res, _] = parse_split2(ss, "=", "&"); */
    while(true) {
      auto a = ss.find("&");
      auto b = ss.find("=");
      if(b != std::string::npos) req_args.insert(std::pair<std::string, std::string>(ss.substr(0, b), ss.substr(b + 1, a - b - 1)));
      if(a == std::string::npos) break;
      ss = ss.substr(a + 1);
    }
  }

  // fragmentの設定
  // if(frag_pos != std::string::npos) fragment = uri.substr(frag_pos+1);
  // else fragment = "";

  // header解析
  const std::string req_str(req_data.begin(), req_data.end());
  const auto header_start = req_str.find("\n");
  constexpr auto npos = std::string::npos;

  if(header_start == npos) {
    std::cerr << "[ERROR] Bad Request [header_start == NULL]\n";
    _status = StatusCodes::Bad_Request;
    this->close();
    return;
  }

  const auto [res, dend] = parse_split2(req_str.substr(header_start + 1), ":", "\n");
  req_headers = res;
  body_start_idx = header_start + dend + 1;
  std::cout << body_start_idx << std::endl;

  // ここで 127.0.0.1/rにアクセスすると止まるバグがある？
  req_body_size = req_data.size() - body_start_idx + 1;
  /* context = const_cast<char *>(header_end) + 1; */
  auto desired_len_str = get_req_header("content-length");
  if(desired_len_str != "") {
    req_body_size = std::stoi(desired_len_str);
  }

  // formの設定
  if(method == HttpMethods::POST && get_req_header("content-type").find("application/x-www-form-urlencoded") != std::string::npos) {
    const std::string body_str = get_req_body_str();
    auto [res, i] = parse_split2(body_str, ":", "&");
    for(auto &&r : res) req_args.emplace(r.first, r.second);
  }
  _valid = true;
}

bool PageIO::valid() const {
  return _valid;
}
bool PageIO::closed() const {
  return _closed;
}

std::string PageIO::get_req_arg(const std::string& key) const {
  auto itr = req_args.find(key);
  if(itr == req_args.end()) return "";
  return itr->second;
}

std::string PageIO::get_req_header(const std::string& key) const {
  auto itr = req_headers.find(key);
  if(itr == req_headers.end()) return "";
  return itr->second;
}

std::string PageIO::get_path() const {
  return path;
}
std::string PageIO::get_method_str() const {
  return cvt_method_to_str(method);
}
HttpMethods PageIO::get_method() const {
  return method;
}
std::string PageIO::get_http_ver() const {
  return http_ver;
}
bool PageIO::has_path() const {
  return _valid && path != "";
}

void PageIO::report_request_info() const {
  std::stringstream ss;
  ss << "\n\n ----------------------- URL INFORMATION ----------------------- \n";
  ss << "method = " << get_method_str() << std::endl;
  ss << "httpver = " << http_ver << std::endl;
  ss << "path = " << path << std::endl;
  ss << "uri = " << uri << std::endl;
  ss << "scheme = " << scheme << std::endl;
  ss << "domain = " << domain << std::endl;
  ss << "header_len = " << req_headers.size() << std::endl;
  ss << "content_len = " << req_body_size << std::endl;
  ss << "extentinon = " << extention << std::endl;
  for(auto itr = req_args.begin(); itr != req_args.end(); ++itr) ss << "argument[" << itr->first << "] = " << itr->second << "\n"; // 値を表示
  ss << std::endl;
  for(auto itr = req_headers.begin(); itr != req_headers.end(); ++itr) ss << "headers[" << itr->first << "] = " << itr->second << "\n"; // 値を表示
  ss << " ----------------------- URL INFORMATION -----------------------\n\n";
  std::cout << ss.str() << ::std::flush;
}


void PageIO::report() const {
  report_request_info();
}

bool PageIO::shouldClose() {
  if(!req_headers.contains("connection")) return false;
  return req_headers.at("connection").find("keep-alive") == std::string::npos;
}

} // namespace Flask
