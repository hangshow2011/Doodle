﻿//
// Created by TD on 2021/6/17.
//

#include "doodle_lib.h"

#include <date/tz.h>
#include <logger/logger.h>
#include <doodle_core/core/core_set.h>
#include <exception/exception.h>
#include <doodle_core/metadata/metadata_cpp.h>
#include <thread_pool/thread_pool.h>

#include <doodle_core/core/core_sig.h>
#include <core/status_info.h>

#include <boost/locale.hpp>
namespace doodle {

doodle_lib* doodle_lib::p_install = nullptr;

doodle_lib::doodle_lib()
    : p_thread_pool(std::make_shared<thread_pool>(std::thread::hardware_concurrency() * 2)),
      p_log(std::make_shared<logger_ctrl>()),
      reg(std::make_shared<entt::registry>()),
      loop(),
      loop_bounded_pool(),
      asio_pool_(),
      io_context_(std::make_shared<boost::asio::io_context>()) {
  boost::locale::generator k_gen{};
  k_gen.categories(boost::locale::all_categories ^
                   boost::locale::formatting_facet ^
                   boost::locale::parsing_facet);
  FSys::path::imbue(k_gen("zh_CN.UTF-8"));
  loop_bounded_pool.timiter_ = core_set::getSet().p_max_thread;
  /// 创建依赖性
  reg->on_construct<database>().connect<&database::set_enum>();
  reg->on_construct<assets_file>().connect<&entt::registry::get_or_emplace<time_point_wrap>>();

  reg->ctx().emplace<database_info>();
  reg->ctx().emplace<project>("C:/", "tmp_project");
  reg->ctx().emplace<project_config::base_config>();

  auto& k_sig = reg->ctx().emplace<core_sig>();
  reg->ctx().emplace<status_info>();
  k_sig.project_begin_open.connect([=](const FSys::path& in_path) {
    auto k_reg                             = g_reg();
    /// @brief 设置数据库路径
    k_reg->ctx().at<database_info>().path_ = in_path;
    /// @brief 清除所有数据库实体
    k_reg->clear();
  });

  k_sig.project_end_open.connect([](const entt::handle& in_handle, const doodle::project& in_project) {
    auto& l_ctx = g_reg()->ctx();
    l_ctx.erase<project>();
    l_ctx.erase<database::ref_data>();
    l_ctx.erase<project_config::base_config>();

    l_ctx.emplace<project>(in_project);
    l_ctx.emplace<database::ref_data>(in_handle.get<database>());
    if (in_handle.any_of<project_config::base_config>()) {
      l_ctx.emplace<project_config::base_config>(in_handle.get<project_config::base_config>());
    } else
      l_ctx.emplace<project_config::base_config>();

    core_set::getSet().add_recent_project(g_reg()->ctx().at<database_info>().path_);
  });
  k_sig.save_end.connect([](const std::vector<entt::handle>&) {
    g_reg()->ctx().at<status_info>().need_save = false;
  });
  p_install = this;
}

FSys::path doodle_lib::create_time_database() {
  auto k_local_path = core_set::getSet().get_cache_root("tzdata");
  if (FSys::is_empty(k_local_path)) {
    auto k_path = cmrc::DoodleLibResource::get_filesystem().iterate_directory("resource/tzdata");
    for (const auto& i : k_path) {
      FSys::ofstream k_ofstream{k_local_path / i.filename(), std::ios::out | std::ios::binary};
      DOODLE_LOG_INFO("开始创建数据库 {}", k_local_path / i.filename());

      chick_true<doodle_error>(k_ofstream, DOODLE_LOC, "无法创建数据库 {}", k_local_path / i.filename());
      auto k_file = cmrc::DoodleLibResource::get_filesystem().open("resource/tzdata/" + i.filename());
      k_ofstream.write(k_file.begin(), boost::numeric_cast<std::int64_t>(k_file.size()));
    }
  }
  date::set_install(k_local_path.generic_string());
  DOODLE_LOG_INFO("初始化时区数据库: {}", k_local_path.generic_string());
  return k_local_path;
}
doodle_lib& doodle_lib::Get() {
  return *p_install;
}

thread_pool_ptr doodle_lib::get_thread_pool() {
  return p_thread_pool;
}

doodle_lib::~doodle_lib() = default;

thread_pool& g_thread_pool() {
  return *(doodle_lib::Get().get_thread_pool());
};
}  // namespace doodle
