#include <chrono>
#include <flask/flask.hpp>
#include <flask/components.hpp>
#include <functional>
#include <iostream>
#include <sstream>
#include <thread>
#include <regex>
#include <string>


Flask::Stream stream_obj;
void send_tmp() {
  int i = 0;
  while(1) {
    i++;
    std::chrono::milliseconds dura(50);
    std::this_thread::sleep_for(dura); // 2000 ミリ秒
#define NUMSS 700
    float nums[NUMSS];
    for(int j = 0; j < NUMSS; j++) {
      nums[j] = j;
    }
    stream_obj.send((void*)nums, NUMSS * sizeof(float));
    if(stream_obj.closed()) return;
  }
}

int main() {
  Flask::Flask app;
  // Flaskアプリをスタートします, 第一引数に使用ポートを設定できます。
  app.start();
  app.Get("/test", [](Flask::PageIO *io) {
    io->report_request_info();
    io->header("Content-Type", "text/html; charset=utf-8");
    io->send("<html><body><h1>Test!</h1><p>This is Top Page!!</p></body></html>\r\n");
  });
  app.Get("/r", [](auto io) { io->redirect("/"); });
  app.Get("/stream", [](auto io) {
    stream_obj.abort();
    stream_obj.pipe(io);
    std::thread t(&send_tmp); //, (std::string)inet_ntoa(addr.sin_addr));
    t.detach();
  });

  /* Flask::ReadableStream read_stream; */
  /* app.Get("/read_stream", [](auto io){ io->req.pipe(read_stream);}); */
  app.Get("/component", {
                          new Flask::H1("This is Header 1"),
                          new Flask::Paragraph("このようにして、タグごとに作ることもできます"),
                          new Flask::Link("ホームディレクトリに移動するお", "/"),
                        });
  app.Get(std::regex("\\/public\\/(.*)"), new Flask::StaticDir("../../template/public"));
  app.Get("/", new Flask::StaticFile("../../template/flask_test.html"));

  while(true) app.update();
  return 0;
}
