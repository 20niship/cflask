#include <flask/components.hpp>
#include <dash/dash.hpp>
#include <iostream>

int main() {
  Flask::Dash app;
  app.start();

  app.Get("/", {new Flask::H1("This is Header 1"), new Flask::Paragraph("This is Paragraph"), new Flask::StaticFile("../../template/index.html"), new Flask::H2("Header 2"), new Flask::Bold("AAAA"),
                new Flask::Div({new Flask::Hr(), new Flask::Paragraph("Inside div")})});
  while(1) app.update();
  return 0;
}
