//
// Created by TD on 2022/5/30.
//

#include "app_base.h"
#include <doodle_core/core/core_set.h>
#include <doodle_core/thread_pool/process_pool.h>
#include <doodle_core/thread_pool/asio_pool.h>
#include <doodle_core/core/doodle_lib.h>
#include <doodle_core/thread_pool/thread_pool.h>
#include <doodle_core/core/init_register.h>
#include <doodle_core/logger/logger.h>
#include <doodle_core/database_task/sqlite_client.h>

#include <boost/locale.hpp>
#include <boost/asio.hpp>
namespace doodle {

class app_base::impl {
 public:
  boost::asio::high_resolution_timer timer_{g_io_context()};
};

app_base* app_base::self = nullptr;

app_base::app_base()
    : p_title(boost::locale::conv::utf_to_utf<wchar_t>(fmt::format(
          "doodle {}", version::version_str))),
      stop_(false),
      instance(::GetModuleHandleW(nullptr)),
      p_lib(std::make_shared<doodle_lib>()),
      p_i(std::make_unique<impl>()) {
  self = this;

  DOODLE_LOG_INFO("开始初始化基本配置");

  core_set_init k_init{};

  p_lib->create_time_database();
  DOODLE_LOG_INFO("寻找用户配置文件目录");
  k_init.config_to_user();
  k_init.find_maya();
  DOODLE_LOG_INFO("读取配置文件");
  k_init.read_file();
  g_bounded_pool().timiter_ = core_set::getSet().p_max_thread;
  g_pool().post<one_process_t>([this]() {
    init_register::instance().reg_class();
    this->load_back_end();
  });
}
app_base::~app_base() = default;

app_base::app_base(win::wnd_instance const& in_instance)
    : app_base() {
  instance = in_instance;
  self     = this;
}
std::atomic_bool& app_base::stop() {
  stop_ = true;
  return stop_;
}
bool app_base::valid() const {
  return false;
}
app_base& app_base::Get() {
  return *self;
}
std::int32_t app_base::run() {
  begin_loop();
  g_io_context().run();
  clear_loop();
  return 0;
}
void app_base::begin_loop() {
  p_i->timer_.cancel();
  static std::function<void(const boost::system::error_code& in_code)> s_fun{};
  s_fun = [&](const boost::system::error_code& in_code) {
    if (in_code == boost::asio::error::operation_aborted)
      return;
    this->loop_one();
    if (!stop_) {
      p_i->timer_.expires_after(doodle::chrono::seconds{1} / 60);
      p_i->timer_.async_wait(s_fun);
    }
  };

  p_i->timer_.expires_after(doodle::chrono::seconds{1} / 60);
  p_i->timer_.async_wait(s_fun);
}

std::int32_t app_base::poll_one() {
  g_io_context().poll_one();
  return 0;
}
void app_base::stop_app(bool in_stop) {
  g_main_loop().abort(in_stop);
  g_bounded_pool().abort(in_stop);
  g_pool().abort(in_stop);
  g_pool().post<one_process_t>([this]() {
    core_set_init{}.write_file();
  });
  this->stop_ = true;
}

bool app_base::is_stop() const {
  return stop_;
}
void app_base::load_project(const FSys::path& in_path) const {
  auto l_path = in_path;
  if (!l_path.empty() &&
      FSys::exists(l_path) &&
      FSys::is_regular_file(l_path) &&
      l_path.extension() == doodle_config::doodle_db_name.data()) {
    database_n::sqlite_client{}.open_sqlite(l_path);
  }
}
void app_base::clear_loop() {
  while (!is_loop_empty()) {
    static decltype(chrono::system_clock::now()) s_now{chrono::system_clock::now()};
    decltype(chrono::system_clock::now()) l_now{chrono::system_clock::now()};
    g_main_loop().update(l_now - s_now, nullptr);
    g_bounded_pool().update(l_now - s_now, nullptr);
    s_now = l_now;
  }
  g_io_context().run();
}
bool app_base::is_loop_empty() {
  return g_main_loop().empty() && g_bounded_pool().empty();
}
void app_base::loop_one() {
  static decltype(chrono::system_clock::now()) s_now{chrono::system_clock::now()};
  decltype(chrono::system_clock::now()) l_now{chrono::system_clock::now()};
  g_main_loop().update(l_now - s_now, nullptr);
  g_bounded_pool().update(l_now - s_now, nullptr);
  s_now = l_now;
}

}  // namespace doodle
