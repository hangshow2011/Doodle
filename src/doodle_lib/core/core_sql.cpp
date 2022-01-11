
#include <doodle_lib/exception/exception.h>
#include <doodle_lib/core/core_set.h>
#include <doodle_lib/core/core_sql.h>
#include <doodle_lib/metadata/project.h>
#include <sqlpp11/sqlpp11.h>

#ifdef DOODLE_GRPC
#include <sqlpp11/mysql/mysql.h>
#endif

#include <sqlpp11/sqlite3/sqlite3.h>

namespace doodle {

class core_sql::impl {
 public:
  std::shared_ptr<sqlpp::sqlite3::connection_config> config;
};

core_sql::core_sql()
    : p_i(std::unique_ptr<impl>()) {
  Init();
}

void core_sql::Init() {
  auto& set = core_set::getSet();

#ifdef NDEBUG
  p_i->config->debug = false;
#else
  p_i->config->debug = true;
#endif
}

conn_ptr core_sql::get_connection(const FSys::path& in_path) const {
  p_i->config->path_to_database = in_path.generic_string();
  return std::make_unique<sqlpp::sqlite3::connection>(*(p_i->config));
}
core_sql& core_sql::Get() {
  static core_sql install;
  return install;
}
core_sql::~core_sql() = default;

}  // namespace doodle
