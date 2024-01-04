//
// Created by TD on 2022/6/2.
//

#pragma once

#include <doodle_core/doodle_core_fwd.h>

#include <any>
#include <filesystem>
#include <memory>
#include <utility>
namespace doodle::database_n {

class file_translator;
using file_translator_ptr = std::shared_ptr<file_translator>;
class DOODLE_CORE_API file_translator : public std::enable_shared_from_this<file_translator> {
 private:
  using timer_t = boost::asio::steady_timer;
  static constexpr std::string_view memory_data{":memory:"};
  std::any obs{};
  registry_ptr registry_attr{};
  std::atomic_bool save_all{};
  bool only_ctx{};
  bool only_open{};

  std::shared_ptr<timer_t> timer_{};  // 定时器

  void begin_save();

 protected:
  FSys::path project_path;

  virtual void async_open_impl();
  virtual void async_save_impl();
  virtual void async_import_impl(const FSys::path& in_path);

 public:
  file_translator();
  explicit file_translator(registry_ptr in_registry);
  ~file_translator() = default;

  inline file_translator& set_only_ctx(bool in_only_ctx) {
    only_ctx = in_only_ctx;
    return *this;
  }
  inline file_translator& set_only_open(bool in_only_open) {
    only_open = in_only_open;
    return *this;
  }
  /**
   * @brief 使用路径打开项目文件
   * @param in_path 传入的项目文件路径
   */

  inline auto async_open(const FSys::path& in_path) {
    project_path = in_path.empty() ? FSys::path{memory_data} : in_path;
    return boost::asio::post(g_io_context(), [this, in_path]() { async_open_impl(); });
  };
  template <typename CompletionHandler>
  auto async_open(
      const FSys::path& in_path, bool in_only_ctx, bool in_only_open, const registry_ptr& in_registry_ptr,
      CompletionHandler&& in_completion_handler
  ) {
    only_ctx      = in_only_ctx;
    only_open     = in_only_open;
    registry_attr = in_registry_ptr;
    project_path  = in_path.empty() ? FSys::path{memory_data} : in_path;
    return boost::asio::async_initiate<CompletionHandler, void(boost::system::error_code)>(
        [this](auto&& in_completion_handler, const FSys::path& in_path, bool in_only_ctx, bool in_only_open) {
          auto l_completion_handler = std::make_shared<std::decay_t<decltype(in_completion_handler)>>(
              std::forward<decltype(in_completion_handler)>(in_completion_handler)
          );
          auto l_exe = boost::asio::get_associated_executor(*l_completion_handler);
          boost::asio::post(g_io_context(), [this, l_completion_handler, l_exe]() {
            async_open_impl();
            //            boost::asio::post()
          });
        },
        std::forward<decltype(in_completion_handler)>(in_completion_handler), in_path, in_only_ctx, in_only_open
    );
  }

  /**
   * @brief 使用路径打开项目文件
   * @param in_path 传入的项目文件路径
   */

  inline auto async_import(const FSys::path& in_path) { return async_import_impl(in_path); };

  virtual void new_file_scene(const FSys::path& in_path, const project& in_project);
  inline FSys::path get_project_path() const { return project_path; }
};

}  // namespace doodle::database_n
