//
// Created by TD on 2022/5/30.
//

#include "insert.h"
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
#include <doodle_core/lib_warp/entt_warp.h>
#include <doodle_core/database_task/sql_file.h>

#include <doodle_core/generate/core/sql_sql.h>
#include <doodle_core/generate/core/metadatatab_sql.h>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/ppgen.h>

#include <range/v3/range.hpp>
#include <range/v3/range_for.hpp>
#include <range/v3/all.hpp>

#include <boost/asio.hpp>

#include <database_task/details/com_data.h>
namespace doodle::database_n {
namespace sql = doodle_database;
namespace {

/**
 * @brief 实体数据
 */
class entity_data {
 public:
  entt::entity entt_{};
  std::uint64_t l_id{};
  std::string uuid_data{};
};

}  // namespace
class insert::impl {
 private:
  /**
   * @brief 线程未来数据获取
   */
  std::vector<std::future<void>> futures_;

 public:
  using com_data = details::com_data;
  /**
   * @brief 传入的实体列表
   */
  std::vector<entt::entity> entt_list{};
  /**
   * @brief 实体数据生成
   */
  std::map<entt::entity, std::shared_ptr<entity_data>> main_tabls;
  /**
   * @brief 组件数据生成
   */
  std::vector<com_data> com_tabls;

  std::vector<std::pair<std::int32_t, std::string>> ctx_tabls;

  using boost_strand = boost::asio::strand<decltype(g_thread_pool().pool_)::executor_type>;

  /**
   * @brief boost 无锁保护
   */
  boost_strand strand_{boost::asio::make_strand(g_thread_pool().pool_)};
  /**
   * @brief 原子停止指示
   */
  std::atomic_bool stop{false};

  /**
   * @brief 线程未来数据获取
   */
  std::future<void> future_;
  std::size_t size;

  /**
   * @brief 在注册表中插入实体
   * @param in_db 传入的插入数据库连接
   */
  void insert_db_entity(sqlpp::sqlite3::connection &in_db) {
    sql::Entity l_tabl{};
    auto l_pre = in_db.prepare(
        sqlpp::insert_into(l_tabl)
            .set(
                l_tabl.uuidData = sqlpp::parameter(l_tabl.uuidData)));

    for (auto &&i : main_tabls) {
      if (stop)
        return;
      l_pre.params.uuidData = i.second->uuid_data;
      i.second->l_id        = in_db(l_pre);
      g_reg()->ctx().emplace<process_message>().progress_step({1, size * 4});
    }
  }
  /**
   * @brief 在数据库中插入组件
   * @param in_db 传入的插入数据库连接
   */
  void insert_db_com(sqlpp::sqlite3::connection &in_db) {
    sql::ComEntity l_tabl{};
    auto l_pre = in_db.prepare(
        sqlpp::insert_into(
            l_tabl)
            .set(
                l_tabl.jsonData = sqlpp::parameter(l_tabl.jsonData),
                l_tabl.comHash  = sqlpp::parameter(l_tabl.comHash),
                l_tabl.entityId = sqlpp::parameter(l_tabl.entityId)));
    for (auto &&j : com_tabls) {
      if (stop)
        return;
      l_pre.params.jsonData = j.json_data;
      l_pre.params.comHash  = j.com_id;
      l_pre.params.entityId = main_tabls.at(j.entt_)->l_id;
      auto l_size           = in_db(l_pre);
      DOODLE_LOG_INFO("插入数据 id {}", l_size);
      g_reg()->ctx().emplace<process_message>().progress_step({1, size * 4});
    }
  }

  void set_database_id() {
    ranges::for_each(entt_list, [this](entt::entity &in_) {
      entt::handle l_h{*g_reg(), in_};
      l_h.get<database>().set_id(main_tabls[in_]->l_id);
    });
  }
  /**
   * @brief 创建实体数据(多线程)
   */
  void create_entt_data() {
    main_tabls = entt_list |
                 ranges::view::transform([](const entt::entity &in) {
                   auto l_i   = std::make_shared<entity_data>();
                   l_i->entt_ = in;
                   return std::make_pair(in, l_i);
                 }) |
                 ranges::to<std::map<entt::entity, std::shared_ptr<entity_data>>>();
    ranges::for_each(main_tabls, [this](decltype(main_tabls)::value_type &in) {
      if (stop)
        return;
      futures_.emplace_back(
          boost::asio::post(
              g_thread_pool().pool_,
              std::packaged_task<void()>{
                  [=]() {
                    if (stop)
                      return;
                    auto l_h = entt::handle{*g_reg(), in.second->entt_};
                    in.second->uuid_data =
                        boost::uuids::to_string(l_h.get<database>().uuid());
                    g_reg()->ctx().emplace<process_message>().progress_step({1, size * 4});
                  }}));
    });
  }

  //#define Type_T doodle::project
  /**
   * @brief 创建组件数据
   * @tparam Type_T 组件类型
   */
  template <typename Type_T>
  void _create_com_data_(std::size_t in_size) {
    ranges::for_each(entt_list, [this, in_size](const entt::entity &in) {
      if (stop)
        return;
      futures_.emplace_back(
          boost::asio::post(
              strand_,
              std::packaged_task<void()>{
                  [=]() {
                    if (stop)
                      return;
                    auto l_h = entt::handle{*g_reg(), in};
                    if (l_h.all_of<Type_T>()) {
                      nlohmann::json l_j{};
                      l_j = l_h.get<Type_T>();
                      com_tabls.emplace_back(in, entt::type_id<Type_T>().hash(), l_j.dump());
                    }
                    g_reg()->ctx().emplace<process_message>().progress_step({1, size * in_size});
                  }}));
    });
  }
  template <typename... Type_T>
  void create_com_data() {
    auto l_size = sizeof...(Type_T);
    (_create_com_data_<Type_T>(l_size), ...);
  }

  /**
   * @brief 从主线程开始调用的函数
   */
  void th_insert() {
    g_reg()->ctx().emplace<process_message>().message("创建实体数据");
    create_entt_data();
    g_reg()->ctx().emplace<process_message>().message("组件数据...");
    create_com_data<doodle::project,
                    doodle::episodes,
                    doodle::shot,
                    doodle::season,
                    doodle::assets,
                    doodle::assets_file,
                    doodle::time_point_wrap,
                    doodle::comment,
                    doodle::project_config::base_config,
                    doodle::image_icon,
                    doodle::importance,
                    doodle::organization_list,
                    doodle::redirection_path_info>();

    g_reg()->ctx().emplace<process_message>().message("完成数据线程准备");
    for (auto &f : futures_) {
      if (stop)
        return;
      f.get();
    }
    g_reg()->ctx().emplace<process_message>().message("完成数据数据创建");
    auto l_comm = core_sql::Get().get_connection(g_reg()->ctx().at<database_info>().path_);
    g_reg()->ctx().emplace<process_message>().message("开始插入数据库实体");
    insert_db_entity(*l_comm);
    g_reg()->ctx().emplace<process_message>().message("组件插入...");
    insert_db_com(*l_comm);
    g_reg()->ctx().emplace<process_message>().message("回调设置id");
    set_database_id();
    g_reg()->ctx().emplace<process_message>().message("完成");
  }
};
insert::insert(const std::vector<entt::entity> &in_inster)
    : p_i(std::make_unique<impl>()) {
  p_i->entt_list = in_inster;
}
insert::~insert() = default;
void insert::init() {
  auto &k_msg = g_reg()->ctx().emplace<process_message>();
  k_msg.set_name("插入数据");
  k_msg.set_state(k_msg.run);
  p_i->future_ = g_thread_pool().enqueue([this]() {
    p_i->th_insert();
  });
}
void insert::succeeded() {
  g_reg()->ctx().erase<process_message>();
}
void insert::failed() {
  g_reg()->ctx().erase<process_message>();
}
void insert::aborted() {
  g_reg()->ctx().erase<process_message>();
  p_i->stop = true;
}
void insert::update(
    chrono::duration<chrono::system_clock::rep, chrono::system_clock::period>,
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

}  // namespace doodle::database_n
