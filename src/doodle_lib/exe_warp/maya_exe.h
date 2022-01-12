//
// Created by TD on 2021/12/25.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>

namespace doodle::details {
class DOODLELIB_API qcloth_arg {
 public:
  FSys::path sim_path;
  FSys::path qcloth_assets_path;
  FSys::path export_path;
  uuid uuid_p;
  bool only_sim;

  friend void to_json(nlohmann::json &nlohmann_json_j, const qcloth_arg &nlohmann_json_t) {
    nlohmann_json_j["path"]               = nlohmann_json_t.sim_path.generic_string();
    nlohmann_json_j["export_path"]        = nlohmann_json_t.export_path.generic_string();
    nlohmann_json_j["qcloth_assets_path"] = nlohmann_json_t.qcloth_assets_path.generic_string();
    nlohmann_json_j["only_sim"]           = nlohmann_json_t.only_sim;
    nlohmann_json_j["uuid"]               = boost::lexical_cast<string>(nlohmann_json_t.uuid_p);
  };
};

class DOODLELIB_API export_fbx_arg {
 public:
  /**
   * @brief maya文件源路径(文件路径)
   *
   */
  FSys::path file_path;
  /**
   * @brief 导出文件的路径(目录)
   *
   */
  FSys::path export_path;
  /**
   * @brief 是否导出所有引用
   *
   */
  bool use_all_ref;

  uuid uuid_p;
  friend void to_json(nlohmann::json &nlohmann_json_j, const export_fbx_arg &nlohmann_json_t) {
    nlohmann_json_j["path"]        = nlohmann_json_t.file_path;
    nlohmann_json_j["export_path"] = nlohmann_json_t.export_path;
    nlohmann_json_j["use_all_ref"] = nlohmann_json_t.use_all_ref;
    nlohmann_json_j["uuid"]        = boost::lexical_cast<string>(nlohmann_json_t.uuid_p);
  };
};

class DOODLELIB_API maya_exe : public process_t<maya_exe> {
  class impl;
  std::unique_ptr<impl> p_i;
  static void add_maya_fun_tool() ;

 public:
  using base_type = process_t<maya_exe>;

  //  void succeed() noexcept;
  //  void fail() noexcept;
  //  void pause() noexcept;
  /**
   * @brief 运行mayapy 任意python脚本
   * @param in_handle 具有消息组件的的句柄
   * @param in_file py文件路径
   *
   * 检查 process_message 和 core_set::getSet().has_maya()
   *
   */
  explicit maya_exe(const entt::handle &in_handle, const string &in_file);
  /**
   * @brief 使用配置进行qcloth操作
   * @param in_handle 具有消息组件的的句柄
   * @param in_arg qcloth 配置类
   *
   * 检查 process_message 和 core_set::getSet().has_maya()
   *
   */
  explicit maya_exe(const entt::handle &in_handle, const qcloth_arg &in_arg);
  /**
   * @brief 使用配置进行fbx导出
   * @param in_handle 具有消息组件的的句柄
   * @param in_arg 导出fbx 配置类
   *
   * 检查 process_message 和 core_set::getSet().has_maya()
   *
   */
  explicit maya_exe(const entt::handle &in_handle, const export_fbx_arg &in_arg);
  ~maya_exe() override;

  [[maybe_unused]] void init();
  [[maybe_unused]] void succeeded();
  [[maybe_unused]] void failed();
  [[maybe_unused]] void aborted();
  [[maybe_unused]] void update(base_type::delta_type, void *data);
};
}  // namespace doodle::details
