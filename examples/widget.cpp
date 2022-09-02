#include <dash/dash.hpp>
#include <dash/ex_component.hpp>
#include <flask/components.hpp>

int main() {
  Flask::Dash app;
  app.set_timeout(100);
  app.start();
  auto cb1 = Flask::SliderCallback("slider", 100, {0, 100});
  auto cb2 = Flask::SliderCallback("slider", 200, {0, 500});

  app.Get("/", { new Flask::H1("Widget Test"), new Flask::BasicDashScripts(), &cb1, &cb2, new Flask::ButtonCallback("button1", [](Flask::PageIO* io) { io->report(); io->send("ok"); }) });
  while(1) {
    app.update();
    std::cout << "Slider value = " << cb1.get() << " - " << cb2.get() << std::endl;
  }
}
