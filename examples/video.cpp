#include <chrono>
#include <flask/flask.hpp>
#include <functional>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <sstream>
#include <flask/components.hpp>
#include <thread>

cv::Mat image;

Flask::Stream stream_obj;
void stream(auto*) {}

#include <regex>
#include <string>

int main() {
  cv::VideoCapture cap(0);
  Flask::Flask app;
  app.start();
  app.Get("/video", [](Flask::PageIO* io) {
    const std::string header = "HTTP/1.1 200 OK\r\nContent-type: multipart/x-mixed-replace; boundary=frame\r\n";
    io->send_raw_keep_alive(header.data(), header.size());
    stream_obj.abort();
    stream_obj.pipe(io);
  });
  app.Get("/", [](Flask::PageIO* io) {
    const std::string html = "<html><head><title>Streaming Test</title></head><body><img src=\"/video\"></body></html>";
    io->header("Content-Type", "text/html; charset=utf-8");
    io->send(html);
  });
  if(!cap.isOpened()) {
    std::cout << "camera not opened!";
    return -1;
  }
  stream_obj.clear_header();
  std::thread t([&] {
    while(true) app.update();
  });
  t.detach();

  while(true) {
    cap >> image;
    const std::vector<int> param = {cv::IMWRITE_JPEG_LUMA_QUALITY, 30};
    std::vector<uint8_t> buf;
    cv::imencode(".jpg", image, buf, param); //エンコード
    std::cout << buf.size() << std::endl;
    cv::imshow("frame", image);
    cv::waitKey(100);
    const std::string data = "\r\n--frame\r\nContent-Type:image/jpeg\n\r\n" + std::string(buf.begin(), buf.end()) + "\r\n\r\n";
    stream_obj.send((void*)data.data(), data.size());
  }
  return 0;
}
