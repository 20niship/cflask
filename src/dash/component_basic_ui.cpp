#include <dash/ex_component.hpp>
#include <iostream>
namespace Flask {

void ButtonCallback::__set_flask_app(Flask* app) { app->Post("/cb/" + cb_id, func); };

}
