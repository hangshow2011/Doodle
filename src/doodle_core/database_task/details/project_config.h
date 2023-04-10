#pragma once

#include <doodle_core/doodle_core_fwd.h>
#include <doodle_core/metadata/project.h>

#include "entt/entity/fwd.hpp"

namespace doodle::database_n {
template <>
struct sql_com<project_config::base_config> {
  registry_ptr reg_;

  void install_sub(
      conn_ptr& in_ptr, const std::vector<entt::handle>& in_handles, const std::map<entt::handle, std::int64_t>& in_map
  );

  void create_table(conn_ptr& in_ptr);
  void insert(conn_ptr& in_ptr, const std::vector<entt::entity>& in_id);
  void update(conn_ptr& in_ptr, const std::vector<entt::entity>& in_id);
  /**
   *
   * @param in_ptr
   * @param in_handle id与之相对的实体
   */
  void select(conn_ptr& in_ptr, const std::map<std::int64_t, entt::entity>& in_handle);
  void destroy(conn_ptr& in_ptr, const std::vector<std::int64_t>& in_handle);
};
}  // namespace doodle::database_n