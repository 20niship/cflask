#pragma once
#include <functional>
#include <initializer_list>

#include <flask/components.hpp>
#include <dash/database.hpp>
#include <flask/flask.hpp>

namespace Flask {

class Dash : public Flask {
public:
  Dash() = default;
  // callback functions
  void add_callback(const std::string& id, PageFunc f);
  // Data Synchronization
  template <typename DataType> void sync_data(const std::string& id, DataType* data, int n = 1);
  template <typename DataType> void sync_data(const std::string& id, std::vector<DataType>& data, int n = 1);
  template <typename DataType> void sync_data_csv(const std::string& id, std::vector<DataType>& data);

  /* void register_db(const DB::Database &db); */
  /* [[noreturn]]void runserver(); */
};

} // namespace Flask
