//
// Created by TD on 2022/6/1.
//

#include <doodle_core/doodle_core_fwd.h>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
namespace doodle::database_n::details {

class update_ctx {
 public:
  static void ctx(const entt::registry& in_registry,
                  sqlpp::sqlite3::connection& in_connection);
};

}  // namespace doodle
