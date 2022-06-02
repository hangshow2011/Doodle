//
// Created by TD on 2022/5/30.
//

#include "delete_data.h"

#include <doodle_core/thread_pool/process_message.h>
#include <doodle_core/core/doodle_lib.h>
#include <doodle_core/thread_pool/thread_pool.h>
#include <doodle_core/logger/logger.h>
#include <doodle_core/core/core_sql.h>

#include <doodle_core/metadata/metadata_cpp.h>
#include <doodle_core/metadata/image_icon.h>
#include <doodle_core/metadata/importance.h>
#include <doodle_core/metadata/organization.h>
#include <doodle_core/metadata/redirection_path_info.h>

#include <doodle_core/generate/core/sql_sql.h>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>

#include <range/v3/all.hpp>

#include <database_task/details/com_data.h>

namespace doodle::database_n {
namespace sql = doodle_database;

class delete_data::impl {
 public:
  /// \brief 实体列表
  std::vector<entt::entity> entt_list{};
  /// \brief 数据库id列表
  std::vector<std::uint64_t> delete_id_list{};

  std::future<void> future_;

  std::atomic_bool stop{false};

  std::size_t size;

  void create_id() {
    delete_id_list =
        entt_list |
        ranges::view::transform([this](const entt::entity &in) {
          if (stop)
            return std::uint64_t{};
          g_reg()->ctx().emplace<process_message>().progress_step({1, size * 2});
          return g_reg()->get<database>(in).get_id();
        }) |
        ranges::to_vector;
  }

  void delete_db(sqlpp::sqlite3::connection &in_db) {
    sql::Entity l_tabl{};
    if (stop)
      return;
    auto l_pre = in_db.prepare(
        sqlpp::remove_from(l_tabl)
            .where(l_tabl.id == sqlpp::parameter(l_tabl.id)));
    for (auto &&i : delete_id_list) {
      if (stop)
        return;
      l_pre.params.id = i;
      in_db(l_pre);
      g_reg()->ctx().emplace<process_message>().progress_step({1, size * 2});
    }
  }

  void th_delete() {
    g_reg()->ctx().emplace<process_message>().message("创建实体id");
    create_id();
    {
      g_reg()->ctx().emplace<process_message>().message("删除数据库数据");
      auto l_comm = core_sql::Get().get_connection(g_reg()->ctx().at<database_info>().path_);
      delete_db(*l_comm);
    }
    g_reg()->ctx().emplace<process_message>().message("清除程序内部注册表");
    g_reg()->destroy(entt_list.begin(), entt_list.end());
    g_reg()->ctx().emplace<process_message>().message("完成");
  }
};
delete_data::delete_data(const std::vector<entt::entity> &in_data)
    : p_i(std::make_unique<impl>()) {
  p_i->entt_list = in_data;
  p_i->size = p_i->entt_list.size();
}
delete_data::~delete_data() = default;
void delete_data::init() {
  auto &k_msg = g_reg()->ctx().emplace<process_message>();
  k_msg.set_name("插入数据");
  k_msg.set_state(k_msg.run);
  p_i->future_ = g_thread_pool().enqueue([this]() {
    p_i->th_delete();
  });
}
void delete_data::succeeded() {
  g_reg()->ctx().erase<process_message>();
}
void delete_data::failed() {
  g_reg()->ctx().erase<process_message>();
}
void delete_data::aborted() {
  g_reg()->ctx().erase<process_message>();
  p_i->stop = true;
}
void delete_data::update(
    chrono::duration<chrono::system_clock::rep,
                     chrono::system_clock::period>,
    void *data) {
  switch (p_i->future_.wait_for(0ns)) {
    case std::future_status::ready: {
      try {
        p_i->future_.get();
        this->succeed();
      } catch (const doodle_error &error) {
        DOODLE_LOG_ERROR(error.what());
        this->fail();
        throw;
      }
    } break;
    default:
      break;
  }
}
}  // namespace doodle
