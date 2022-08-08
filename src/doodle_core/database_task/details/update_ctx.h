//
// Created by TD on 2022/6/1.
//

#include <doodle_core/doodle_core_fwd.h>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
namespace doodle::database_n::details {

class update_ctx {
 public:
  static void select_ctx(entt::registry &in_registry,
                         sqlpp::sqlite3::connection &in_connection);

  static void ctx(const entt::registry &in_registry,
                  sqlpp::sqlite3::connection &in_connection);
  static void ctx(const entt::registry &in_registry);
};

std::tuple<std::uint32_t, std::uint32_t> get_version(
    sqlpp::sqlite3::connection &in_connection);

void add_ctx_table(sqlpp::sqlite3::connection &in_conn);
void add_entity_table(sqlpp::sqlite3::connection &in_conn);
void add_component_table(sqlpp::sqlite3::connection &in_conn);
void add_version_table(sqlpp::sqlite3::connection &in_conn);
bool has_version_table(sqlpp::sqlite3::connection &in_conn);
void set_version(sqlpp::sqlite3::connection &in_conn);

class db_compatible {
 public:
  static bool has_metadatatab_table(sqlpp::sqlite3::connection &in_conn);
  static void delete_metadatatab_table(sqlpp::sqlite3::connection &in_conn);
};

}  // namespace doodle::database_n::details
