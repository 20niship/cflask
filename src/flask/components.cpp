#include <flask/components.hpp>
#include <flask/flask.hpp>

namespace Flask {
std::string Component::surround_tag(const std::string& str_) const {
  std::string s = "<" + tag_text + " ";
  if(class_names.size() > 0) {
    s += "class=\"";
    for(const auto& c : class_names) s += c + " ";
    s += "\"";
  }

  if(id != "") s += " id= \"" + id + "\"";
  s += ">";
  s += str_;
  s += "</" + tag_text + ">";
  return s;
}


void Stream::abort() {
  reset();
}

void Stream::reset() {
  for(auto&& r : res_list) close(r);
  _locked = false;
  res_list.clear();
}

void Stream::close(PageIO *io) {
  const auto ptr = std::find(res_list.begin(), res_list.end(), io);
  if(ptr == res_list.end()) return;
  res_list.erase(ptr);
  io->close();
}

void Stream::send_raw(const void* data, int size) {
  for(auto&& res : res_list) {
    if(res == nullptr || res->closed()) {
      close(res);
      continue;
    }
    const bool keep_alive = res->send_raw_keep_alive(data, size);
    if(!keep_alive) close(res);
  }
}

const char *header_text = "HTTP/1.1 200 OK\r\nTransfer-Encoding: identity\r\nContent-type: aplication/octet-stream\r\n\r\n";
void Stream::send_header() {
  if(header_list.size() == 0) return;
  for(auto&& res : res_list) {
    if(res == nullptr || res->closed()) {
      close(res);
      continue;
    }
    if(res->is_header_send()) continue;
    const auto keep_alive = res->send_raw_keep_alive(header_text, strlen(header_text));
    if(!keep_alive) close(res);
    res->set_header_send(true);
    /* for(const auto & h : header_list) res->header(h.first, h.second); */
    /* const bool keep_alive = res->send_header(); */
    /* if(!keep_alive) close(res); */
  }
}

void Stream::send_impl(const void* data, const int size) {
  send_header();
  send_raw(data, size);
}



/* Canvas::Canvas(const std::string &, const ::Flask::DB::Database &){ */
/*  inner_text = ""; */
/*     /1* "<Canvas width=" *1/ */
/* } */

/* std::string Multer::render(const HTTPRequest *req, HTTPresponse *res) const{ */
/*   std::cout << "UPLOAD FILE ...... " << std::endl; */
/*   auto content_type = req->getHeader("content-type"); */
/*   if (content_type.find("multipart/form-data") != std::string::npos) { */
/*     auto bpos = content_type.find("boundary="); */
/*     auto boundary = content_type.substr(bpos + 7); */
/*     if (bpos == std::string::npos || boundary == "") { */
/*       res->status(::Flask::StatusCodes::Bad_Request); */
/*       res->send("bad request!!"); */
/*       return ""; */
/*     } */
/*     char *cursor = req->context; */
/*     while (cursor < req->context + req->content_size) { */
/*       char *boundary_pos = strstr(cursor, boundary.c_str()); */
/*       if (boundary_pos == NULL) */
/*         break; */
/*       std::unordered_map<std::string, std::string> fileinfo; */
/*       char str1[2048]; */
/*       char str2[2048]; */
/*       char *cursor_fileinfo = const_cast<char *>(strchr(raw_request, '\n')) + 1; */
/*       while (true) { */
/*         auto a = strchr(cursor_fileinfo, '\n'); */
/*         auto b = strchr(cursor_fileinfo, ':'); */
/*         if (b != NULL && a != NULL && a > b) { */
/*           strncpy2(str1, cursor_fileinfo, b - cursor_fileinfo); */
/*           strncpy2(str2, b + 1, a - b - 1); */
/*           std::string ss1 = str1; */
/*           std::string ss2 = str2; */
/*           std::transform(ss1.begin(), ss1.end(), ss1.begin(), */
/*                          [](unsigned char c) { return std::tolower(c); }); */
/*           std::transform(ss2.begin(), ss2.end(), ss2.begin(), */
/*                          [](unsigned char c) { return std::tolower(c); }); */
/*           unordered_map.insert(std::pair(ss1, ss2)); */
/*         } else { */
/*           header_end = cursor_fileinfo; */
/*           break; */
/*         } */
/*         cursor_fileinfo = a + 1; */
/*       } */
/*       // Content-Disposition: form-data; name="value2"; filename="temp.cpp" */
/*       // Content-Type: text/plain */
/*       std::cout << "write file " << (boundary_pos - cursor) << " strs" << std::endl; */
/*       std::ofstream f; */
/*       f.open("test.txt"); */
/*       f.write((char *)cursor, boundary_pos - cursor); */
/*       f.close(); */
/*       cursor = boundary_pos + boundary.size(); */
/*     } */
/*   } */
/* } */
} // namespace Flask
