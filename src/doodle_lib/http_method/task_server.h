//
// Created by TD on 2024/2/28.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>
namespace doodle {
class server_task_info;
}
namespace doodle::http {

class task_server {
  using timer_t     = boost::asio::steady_timer;
  using timer_t_ptr = std::shared_ptr<timer_t>;

  timer_t_ptr timer_ptr_{};
  logger_ptr logger_ptr_{};

  // 分配任务
  bool assign_task();

  bool is_running_ = false;
  // 运行守卫
  struct run_guard_t {
    explicit run_guard_t(task_server* in_ptr) : ptr_(in_ptr) { ptr_->is_running_ = true; }
    ~run_guard_t() { ptr_->is_running_ = false; }
    task_server* ptr_;
  };

  void begin_assign_task();

 public:
  task_server();
  ~task_server() = default;

  // 此处可进行多线程调用
  void run();

  std::map<boost::uuids::uuid, std::shared_ptr<doodle::server_task_info>> task_map_{};  // 任务列表
};

}  // namespace doodle::http