﻿
#include <corelib/core/coresql.h>

#include <loggerlib/Logger.h>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>


#include <thread>
#include <stdexcept>
DOODLE_NAMESPACE_S
coreSql::coreSql()
    : p_path(),
      config(std::make_shared<sqlpp::sqlite3::connection_config>()) {}


void coreSql::initDB() {
  config->flags            = SQLITE_OPEN_READWRITE;
  config->path_to_database = p_path;
#ifdef NDEBUG
  config->debug = false;
#else
  config->debug = true;
#endif  //NDEBUG
}

ConnPtr coreSql::getConnection() {
  return std::make_unique<sqlpp::sqlite3::connection>(*config);
}
void coreSql::initDB(sqlOpenMode flags) {
  switch (flags) {
    case sqlOpenMode::readOnly:
      break;
    case sqlOpenMode::write:
      break;
    case sqlOpenMode::create:
      break;
  }
}
ConnPtr coreSql::getConnection(sqlOpenMode flags) {
  return doodle::ConnPtr();
}

DOODLE_NAMESPACE_E