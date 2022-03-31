//
// Created by TD on 2021/12/25.
//

#include "maya_exe.h"
#include "thread_pool/process_message.h"
#include <doodle_lib/core/filesystem_extend.h>
#include <doodle_lib/core/core_set.h>

#include <doodle_lib/thread_pool/thread_pool.h>
//#include <type_traits>

#include <boost/process.hpp>
#ifdef _WIN32
#include <boost/process/windows.hpp>
#elif defined __linux__
#include <boost/process/posix.hpp>
#endif
namespace doodle::details {
namespace {
//致命错误。尝试在 C:/Users/ADMINI~1/AppData/Local/Temp/Administrator.20210906.2300.ma 中保存
const std::wregex fatal_error_znch{
    LR"(致命错误.尝试在 C:/Users/[a-zA-Z~\d]+/AppData/Local/Temp/[a-zA-Z~\d]+\.\d+\.\d+\.m[ab] 中保存)"};

// Fatal Error. Attempting to save in C:/Users/Knownexus/AppData/Local/Temp/Knownexus.20160720.1239.ma
const std::wregex fatal_error_en_us{
    LR"(Fatal Error\. Attempting to save in C:/Users/[a-zA-Z~\d]+/AppData/Local/Temp/[a-zA-Z~\d]+\.\d+\.\d+\.m[ab])"};

}  // namespace

class maya_exe::impl {
 public:
  std::string in_comm;
  //  boost::process::async_pipe p_out;
  //    boost::process::async_pipe p_erra;
  boost::process::ipstream p_out;
  boost::process::ipstream p_err;
  std::future<std::string> p_out_str;
  std::future<std::string> p_err_str;

  boost::process::child p_process;
  entt::handle p_mess;
  chrono::sys_time_pos p_time;
};
maya_exe::maya_exe(const entt::handle &in_handle, const string &in_comm)
    : p_i(std::make_unique<impl>()) {
  chick_true<doodle_error>(in_handle.any_of<process_message>(), DOODLE_LOC, "缺失进度指示结构");
  chick_true<doodle_error>(core_set::getSet().has_maya(), DOODLE_LOC, "没有找到maya路径 (例如 C:/Program Files/Autodesk/Maya2019/bin)");
  p_i->p_mess  = in_handle;

  //生成命令
  p_i->in_comm = fmt::format(
      R"("{}/mayapy.exe" {})",
      core_set::getSet().maya_path().generic_string(),
      in_comm);
}
maya_exe::maya_exe(const entt::handle &in_handle, const qcloth_arg &in_arg)
    : p_i(std::make_unique<impl>()) {
  chick_true<doodle_error>(in_handle.any_of<process_message>(), DOODLE_LOC, "缺失进度指示结构");
  chick_true<doodle_error>(core_set::getSet().has_maya(), DOODLE_LOC, "没有找到maya路径 (例如 C:/Program Files/Autodesk/Maya2019/bin)");
  p_i->p_mess     = in_handle;

  //生成导出文件
  auto str_script = fmt::format(
      R"(# -*- coding: utf-8 -*-\n
import maya_fun_tool
k_f =  maya_fun_tool.open_file()
k_f.config_ = """{}"""
k_f()
quit()
)",
      nlohmann::json{in_arg}.dump());

  auto run_path = FSys::write_tmp_file("maya", str_script, ".py");

  p_i->p_mess.patch<process_message>([&](process_message &in) {
    in.message(fmt::format("开始写入配置文件 {} \n", run_path), in.warning);
  });

  //生成命令
  p_i->in_comm = fmt::format(
      R"("{}/mayapy.exe" {})",
      core_set::getSet().maya_path().generic_string(),
      run_path.generic_string());
}
maya_exe::maya_exe(const entt::handle &in_handle, const export_fbx_arg &in_arg)
    : p_i(std::make_unique<impl>()) {
  chick_true<doodle_error>(in_handle.any_of<process_message>(), DOODLE_LOC, "缺失进度指示结构");
  chick_true<doodle_error>(core_set::getSet().has_maya(), DOODLE_LOC, "没有找到maya路径 (例如 C:/Program Files/Autodesk/Maya2019/bin)");
  p_i->p_mess     = in_handle;

  //生成导出文件
  auto str_script = fmt::format(
      R"(# -*- coding: utf-8 -*-\n
import maya_fun_tool
k_f =  maya_fun_tool.open_file()
k_f.config_ = """{}"""
k_f()
quit()
)",
      nlohmann::json{in_arg}.dump());

  auto run_path = FSys::write_tmp_file("maya", str_script, ".py");
  p_i->p_mess.patch<process_message>([&](process_message &in) {
    in.message(fmt::format("开始写入配置文件 {} \n", run_path), in.warning);
  });

  //生成命令
  p_i->in_comm = fmt::format(
      R"("{}/mayapy.exe" {})",
      core_set::getSet().maya_path().generic_string(),
      run_path.generic_string());
}
maya_exe::~maya_exe() = default;

void maya_exe::add_maya_fun_tool() {
  const auto tmp_path = core_set::getSet().get_cache_root(
      fmt::format("maya\\v{}{}{}",
                  version::version_major,
                  version::version_minor,
                  version::version_patch));
  auto k_tmp_path = tmp_path / "maya_fun_tool.py";
  if (!exists(k_tmp_path)) {
    auto k_file_py = cmrc::DoodleLibResource::get_filesystem().open("resource/maya_fun_tool.py");
    {  //写入文件后直接关闭
      FSys::fstream file{k_tmp_path, std::ios::out | std::ios::binary};
      file.write(k_file_py.begin(), boost::numeric_cast<int64_t>(k_file_py.size()));
    }
  }
}
[[maybe_unused]] void maya_exe::init() {
  auto &l_mag = p_i->p_mess.patch<process_message>();
  l_mag.set_state(l_mag.run);
  l_mag.aborted_function = [self = this]() {if(self) self->abort(); };

  add_maya_fun_tool();
  DOODLE_LOG_INFO("命令 {}", p_i->in_comm);
  p_i->p_process = boost::process::child{
      boost::process::cmd = conv::utf_to_utf<wchar_t>(p_i->in_comm),
      boost::process::std_out > p_i->p_out,
      boost::process::std_err > p_i->p_err,
      boost::process::on_exit([&](auto...) {
        p_i->p_process.terminate();
      })
#ifdef _WIN32
          ,
      boost::process::windows::hide
#endif  //_WIN32};
  };
  p_i->p_mess.patch<process_message>([](process_message &in) {
    in.progress_step({1, 40});
  });
  p_i->p_time = chrono::system_clock::now();
}
void maya_exe::update(chrono::duration<chrono::system_clock::rep, chrono::system_clock::period>, void *data) {
  using namespace chrono::literals;

  string k_out{};
  if (p_i->p_out_str.valid()) {  /// 异步有效, 是否可以读取
    switch (p_i->p_out_str.wait_for(0ns)) {
      case std::future_status::ready: {
        k_out = p_i->p_out_str.get();
        if (!k_out.empty()) {
          auto k_str = conv::to_utf<char>(k_out, "GBK");
          k_str.pop_back();
          k_str.push_back('\n');

          p_i->p_time = chrono::system_clock::now();
          p_i->p_mess.patch<process_message>([&](process_message &in) {
            in.progress_step({1, 300});
            in.message(k_str, in.warning);
          });
        }
        goto sub_out;
        break;
      }
      default:
        break;
    }
  } else {  /// 提交新的读取函数
  sub_out:
    if (p_i->p_out)
      p_i->p_out_str = std::move(g_thread_pool().enqueue(
          [self = p_i.get()]() -> string {
            string k_str{};
            if (self && self->p_out)
              getline(self->p_out, k_str);
            return k_str;
          }));
  }

  if (p_i->p_err_str.valid()) {  /// 异步有效, 是否可以读取
    switch (p_i->p_err_str.wait_for(0ns)) {
      case std::future_status::ready: {
        k_out = p_i->p_err_str.get();
        if (!k_out.empty()) {
          auto k_str = conv::to_utf<char>(k_out, "GBK");
          k_str.pop_back();
          k_str.push_back('\n');
          auto k_w_str = conv::to_utf<wchar_t>(k_out, "GBK");
          if (std::regex_search(k_w_str, fatal_error_znch) ||
              std::regex_search(k_w_str, fatal_error_en_us)) {
            DOODLE_LOG_WARN("检测到maya结束崩溃,结束进程: 解算命令是 {}\n", p_i->in_comm);
            p_i->p_mess.patch<process_message>([&](process_message &in) {
              auto k_str = fmt::format("检测到maya结束崩溃,结束进程: 解算命令是 {}\n", p_i->in_comm);
              in.message(k_str, in.warning);
            });
            fail();
            return;
          }
          p_i->p_time = chrono::system_clock::now();
          p_i->p_mess.patch<process_message>([&](process_message &in) {
            in.progress_step({1, 20000});
            in.message(k_str, in.info);
          });
        }
        goto sub_err;
        break;
      }
      default:
        break;
    }
  } else {  /// 提交新的读取函数
  sub_err:
    if (p_i->p_err)
      p_i->p_err_str = std::move(g_thread_pool().enqueue(
          [self = p_i.get()]() -> string {
            string k_str{};
            if (self && self->p_err)
              getline(self->p_err, k_str);
            return k_str;
          }));
  }

  auto k_time = chrono::system_clock::now() - p_i->p_time;
  if (core_set::getSet().timeout < chrono::floor<chrono::seconds>(k_time).count()) {
    p_i->p_process.terminate();
    p_i->p_mess.patch<process_message>([&](process_message &in) {
      in.message("进程超时, 主动结束任务\n", in.warning);
    });
    this->fail();
  }

  if (!p_i->p_process.running() && p_i->p_out.eof() && p_i->p_err.eof()) {
    if (p_i->p_process.exit_code() == 0) {
      this->succeed();
    } else {
      this->fail();
    }
  }
}

void maya_exe::succeeded() {
  p_i->p_mess.patch<process_message>([&](process_message &in) {
    in.set_state(in.success);
    auto k_str = fmt::format("成功完成任务\n");
    in.message(k_str, in.warning);
  });
}
void maya_exe::failed() {
  p_i->p_mess.patch<process_message>([&](process_message &in) {
    in.set_state(in.fail);
    auto k_str = fmt::format("进程失败,退出代码是 {} \n", p_i->p_process.exit_code());
    in.message(k_str, in.warning);
  });
}
void maya_exe::aborted() {
  p_i->p_process.terminate();
  p_i->p_mess.patch<process_message>([&](process_message &in) {
    in.set_state(in.fail);
    auto k_str = fmt::format("进程被主动结束 \n", p_i->p_process.exit_code());
    in.message(k_str, in.warning);
  });
}

}  // namespace doodle::details
