//
// Created by TD on 2021/10/18.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>

#include <boost/program_options.hpp>
namespace doodle {

class DOODLELIB_API program_options {
 public:
  FSys::path p_config_file;
  std::pair<bool, std::int32_t> p_max_thread;
  std::pair<bool, FSys::path> p_root;
  std::string p_project_path;
  std::string p_ue4outpath;
  std::string p_ue4Project;

  bool p_help;
  bool p_version;

  std::vector<string> p_arg;

 private:
  static constexpr char input_project[] = "input_project";
  static constexpr char help[]          = "help";
  static constexpr char help_[]         = "help,h";
  static constexpr char version[]       = "version";
  static constexpr char version_[]      = "version,v";
  static constexpr char config_file[]   = "config_file";
  static constexpr char config_file_[]  = "config_file";
  static constexpr char root[]          = "root";
  static constexpr char root_[]         = "root";
  static constexpr char thread_max[]    = "thread_max";
  static constexpr char thread_max_[]   = "thread_max";
  static constexpr char ue4outpath[]    = "ue4outpath";
  static constexpr char ue4Project[]    = "ue4Project";
  /**
   * @brief 所有选项， 命令行选项
   *
   */
  boost::program_options::options_description p_opt_all;
  /**
   * @brief 解析配置文件时的选项
   *
   */
  boost::program_options::options_description p_opt_file;

  /**
   * @brief gui选项
   *
   */
  boost::program_options::options_description p_opt_gui;
  /**
   * @brief 一般选项
   *
   */
  boost::program_options::options_description p_opt_general;
  /**
   * @brief 高级设置
   *
   */
  boost::program_options::options_description p_opt_advanced;
  /**
   * @brief 位置选项
   *
   */
  boost::program_options::positional_options_description p_opt_positional;

  boost::program_options::variables_map p_vm;

 public:
  program_options();

  /**
   * @brief 解析命令行
   *
   * @param argc 传入的命令行参数
   * @param argv 传入的命令行参数
   * @return true 解析成功
   * @return false 解析失败
   */
  inline bool command_line_parser(int argc, const char* argv[]) {
    string_list k_str{argv, argv + argc};
    return command_line_parser(k_str);
  };
  bool command_line_parser(const std::vector<string>& in_arg);
  inline bool command_line_parser(const LPSTR& in_arg) {
    auto k_str = boost::program_options::split_winmain(in_arg);
    return command_line_parser(k_str);
  };
};

}  // namespace doodle
