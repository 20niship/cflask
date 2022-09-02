# cflask

- C++ Simple and fast HTTP server with no dependencies.
- Works in Linux environment 
  - (Not work in Windows/Mac. This library uses poll)
 

# Features
- HTTP communication (GET, POST etc...)
- URL parsing
- streaming data
- Canvas (see `examples/graph.cpp`)
- Web camera streaming

# Examples
```cpp
#include <flask/flask.hpp>

int main() {
  Flask::Flask app;
  app.start();
  app.Get("/test", [](Flask::PageIO *io) {
    io->header("Content-Type", "text/html; charset=utf-8");
    io->send("<html><body><h1>Test!</h1><p>This is Top Page!!</p></body></html>\r\n");
  });
  while(true) app.update();
  return 0;
}
```

More examples? see `examples` directory! 
 

