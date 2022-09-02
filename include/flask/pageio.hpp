#pragma once

#include <flask/webenum.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace Flask {

using HeaderList = std::unordered_map<std::string, std::string>;
using RenderValues = std::unordered_map<std::string, std::string>;

class PageIO {
private:
  std::unordered_map<std::string, std::string> req_args;
  int _sock;

  // ----------------  for request ----------------------
  /* StatusCodes _status; */
  std::string domain, host, path, scheme, uri, extention, http_ver;
  HttpMethods method;
  int port;
  std::string ip;
  int body_start_idx;
  int req_body_size;
  std::vector<uint8_t> req_data; // Requestの中身へのポインタ
  std::string get_header_text()const;
  void parse_request();
  bool received_all = false;
  bool _valid = false;
  bool _closed = false;

  // -------------  for response --------------------
  std::string context;
  StatusCodes _status; // 404とか200みたいなやつ
  bool sent_body, header_send_done;
  HeaderList req_headers, res_headers;
public:
  PageIO() = delete;
  PageIO(const int s);
  void close();
  bool valid() const;
  bool closed() const;
  std::string get_req_arg(const std::string &key)const;
  auto get_req_args()const;
  std::string get_req_header(const std::string &key) const;
  void report_request_info() const;
  std::vector<char> get_req_body()const;
  int get_req_body_size()const;
  std::string get_req_body_str()const;
  bool shouldClose();
  void receive_new_data();
  void set_port(const int p){port = p;}
  void set_ip_addr(const char* ip_){ip =std::string(ip_); }
  /* void send_impl(){ */
  /*     const auto rc = recv(fd.fd, buffer, sizeof(buffer), 0); */
  /*     if(rc < 0) { */
  /*       if(errno != EWOULDBLOCK) perror("  recv() failed"); */
  /*       break; */
  /*     } */
  /*     if(rc == 0) { */
  /*       printf("  Connection closed\n"); */
  /*       break; */
  /*     } */
  /* } */
  HttpMethods get_method() const;
  std::string get_path() const;
  std::string get_http_ver() const;
  std::string get_method_str() const;
  bool has_path() const;

  // -------------  for response --------------------
  void init();
  void setCurrentTime();
  void header(const std::string& key, const std::string& value);
  void status(const StatusCodes s);
  void status(const int s);
  int get_socket();
  bool is_header_send() { return header_send_done; }
  void set_header_send(const bool v) { header_send_done = v; }
  void send(const std::string& msg = "");
  [[deprecated]] void send(const void* ptr, const size_t size);
  void send_raw(const void* ptr, const size_t size); // TODO: じっそうする
  bool send_raw_keep_alive(const void* ptr, const size_t size);
  bool send_header();
  /* void send_raw(cv::Mat img); */
  void serve_file(const std::string filename);
  // TODO: render ejs
  void render(const std::string filename, const RenderValues& v = {});
  void redirect(const std::string new_url);

  void report()const;
};

} // namespace Flask
