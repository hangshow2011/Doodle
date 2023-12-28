//
// Created by TD on 2022/5/30.
//

#pragma once
#include <doodle_core/core/app_facet.h>
#include <doodle_core/doodle_core_fwd.h>
#include <doodle_core/platform/win/windows_alias.h>

#include <boost/signals2.hpp>

#include <argh.h>
#include <thread>

namespace doodle {
/**
 * @brief 基础的事件循环类,  只有事件循环可以使用
 */

/**
 * @brief app 的基类
 *
 */
class DOODLE_CORE_API app_base {
 public:
 protected:
  class cancellation_signals {
    std::list<boost::asio::cancellation_signal> sigs;
    std::mutex mtx;

   public:
    void emit(boost::asio::cancellation_type ct = boost::asio::cancellation_type::all);

    boost::asio::cancellation_slot slot();
  };
  static app_base* self;
  doodle_lib_ptr lib_ptr;

  std::wstring p_title;
  argh::parser arg_;
  std::vector<std::shared_ptr<void>> facets_;

  /// @brief 在初始化中获取的id为主id(也是渲染线程id)
  std::thread::id run_id{std::this_thread::get_id()};

  void init();

  /**
   * @brief 这个会在第一个循环中加载
   *
   */

  virtual void post_constructor() = 0;
  virtual void deconstruction()   = 0;
  std::atomic_bool stop_;

 public:
  explicit app_base(int argc, const char* const argv[]);
  app_base() : app_base(0, nullptr) {}
  virtual ~app_base();

  /**
   * @brief 直接使用默认配置运行
   * @return
   */
  virtual std::int32_t run();

  bool is_main_thread() const;

  virtual std::int32_t poll_one();

  boost::signals2::signal<void()> on_stop;
  cancellation_signals on_cancel;

  void stop_app(bool in_stop = false);

  DOODLE_DIS_COPY(app_base);
  static app_base& Get();
};

/**
 * @brief 基本的命令行类
 */
template <typename... Facet_>
class app_command : public app_base {
 public:
  app_command() : app_base() { run_facet(); };

  app_command(int argc, const char* const argv[]) : app_command(argc, argv) { run_facet(); }
  virtual ~app_command() override = default;

  void run_facet() {
    std::array<bool, sizeof...(Facet_)> l_r{
        Facet_{}(arg_, facets_)...,
    };
    stop_ = std::any_of(l_r.begin(), l_r.end(), [](bool i) { return i; });
  }

 protected:
};

template <typename... Facet_>
using app_plug = app_command<Facet_...>;

}  // namespace doodle
