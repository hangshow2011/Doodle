//
// Created by TD on 2022/5/30.
//

#pragma once
#include <doodle_core/doodle_core_fwd.h>
#include <doodle_core/platform/win/windows_alias.h>

#include <boost/asio/high_resolution_timer.hpp>
namespace doodle {
/**
 * @brief 基础的事件循环类,  只有事件循环可以使用
 */
class DOODLE_CORE_EXPORT app_base {
 protected:
  static app_base* self;

  std::wstring p_title;
  win::wnd_instance instance;
  doodle_lib_ptr p_lib;
  /**
   * @brief 这个会在第一个循环中加载
   *
   */
  virtual void load_back_end() = 0;
  boost::asio::high_resolution_timer timer_;


 public:
  explicit app_base();
  explicit app_base(const win::wnd_instance& in_instance);
  virtual ~app_base();

  /**
   * @brief 直接使用默认配置运行
   * @return
   */
  virtual std::int32_t run();

  virtual void loop_one();
  virtual void clear_loop();
  virtual bool is_loop_empty();

  std::atomic_bool& stop();
  bool is_stop() const;
  void stop_app(bool in_stop = false);
  std::atomic_bool stop_;
  virtual bool valid() const;

  void load_project(const FSys::path& in_path) const;

  DOODLE_DIS_COPY(app_base);
  static app_base& Get();
};

}  // namespace doodle
