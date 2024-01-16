//
// Created by TD on 2021/11/30.
//
#pragma once
#include <maya_plug/main/maya_plug_fwd.h>

#include "entt/entity/fwd.hpp"
#include "maya/MApiNamespace.h"
#include "maya/MString.h"
#include <maya/MSelectionList.h>
#include <maya/MTime.h>
#include <vector>
namespace doodle::maya_plug {
class reference_file;
namespace reference_file_ns {
class generate_file_path_base : boost::less_than_comparable<generate_file_path_base> {
 protected:
  std::string extract_reference_name;
  std::string format_reference_name;
  std::string extract_scene_name;
  std::string format_scene_name;
  bool use_add_range;
  template <typename T1, typename Char, typename Enable>
  friend struct fmt::formatter;

  virtual FSys::path get_path() const                               = 0;
  virtual FSys::path get_name(const std::string &in_ref_name) const = 0;

  std::string get_extract_scene_name(const std::string &in_name) const;
  std::string get_extract_reference_name(const std::string &in_name) const;

 public:
  generate_file_path_base()          = default;
  virtual ~generate_file_path_base() = default;

  std::optional<std::string> add_external_string;

  std::pair<MTime, MTime> begin_end_time;
  FSys::path operator()(const reference_file &in_ref) const;
  [[nodiscard("")]] bool operator==(const generate_file_path_base &in) const noexcept;
  [[nodiscard("")]] bool operator<(const generate_file_path_base &in) const noexcept;
};

class generate_abc_file_path : boost::less_than_comparable<generate_abc_file_path>, public generate_file_path_base {
 protected:
  [[nodiscard("")]] FSys::path get_path() const override;
  [[nodiscard("")]] FSys::path get_name(const std::string &in_ref_name) const override;
  friend struct fmt::formatter<generate_file_path_base>;
  bool export_fbx{};

 public:
  explicit generate_abc_file_path(const entt::registry &in = *g_reg());
  virtual ~generate_abc_file_path();

  inline void set_fbx_path(bool is_export_fbx = true) { export_fbx = is_export_fbx; };
};

class generate_fbx_file_path : boost::less_than_comparable<generate_fbx_file_path>, public generate_file_path_base {
  friend struct fmt::formatter<generate_fbx_file_path>;

 private:
  std::string camera_suffix{};
  bool is_camera_attr{};

 protected:
  [[nodiscard("")]] FSys::path get_path() const override;
  [[nodiscard("")]] FSys::path get_name(const std::string &in_ref_name) const override;

 public:
  explicit generate_fbx_file_path(const entt::registry &in = *g_reg());

  void is_camera(bool in_is_camera);

  virtual ~generate_fbx_file_path();
};

}  // namespace reference_file_ns

/**
 * @brief 一个类似的引用文件使用名称空间作为引用的定位,
 * 而不是ref节点,
 * 这样我们可以在文件中创建出一个类似的引用, 但不是引用,
 * 并且具有一定引用概念的类
 */
class reference_file {
 public:
 private:
  std::string get_file_namespace() const;

  void find_ref_node();

  void set_path(const MObject &in_ref_node);

  MObject file_info_node_;
  /**
   * @brief 引用 maya obj 节点(可空)
   */
  MObject p_m_object;

 public:
  reference_file();
  explicit reference_file(const MObject &in_ref_node);

  void set_file_info_node(const MObject &in_file_info_node);
  [[nodiscard]] MSelectionList get_collision_model() const;

  [[nodiscard]] std::string get_namespace() const;

  [[nodiscard]] bool has_node(const MSelectionList &in_list);
  [[nodiscard]] bool has_node(const MObject &in_node) const;
  /**
   * @brief 获取真正的路径名称
   * @return
   */
  [[nodiscard]] FSys::path get_path() const;
  /**
   * @brief 获取真正的路径名称
   * @return
   */
  [[nodiscard]] FSys::path get_abs_path() const;
  /**
   * @brief 没有加载的引用和资产不存在的文件返回false 我们认为这不是异常, 属于正常情况
   */
  bool replace_sim_assets_file();
  /**
   * @brief 替换引用 需要组件 redirection_path_info_edit
   */
  bool replace_file(const FSys::path &in_handle);

  std::optional<MDagPath> get_field_dag() const;

  enum class export_type : std::uint32_t {
    abc = 1,
    fbx = 2,
  };

  /**
   * @brief 从配置文件中查找需要导出组名称对应的 maya 组 (名称空间为引用空间)
   * @return 导出配置文件中对应的组
   */
  std::optional<MDagPath> export_group_attr() const;

  /**
   * @brief 获取这个抽象引用中所有的 obj
   * @return 选中所有的obj
   */
  MSelectionList get_all_object() const;

  std::vector<MDagPath> get_alll_cloth_obj() const;
};

class reference_file_factory {
 public:
  reference_file_factory()  = default;
  ~reference_file_factory() = default;

  [[nodiscard]] std::vector<entt::handle> create_ref(bool is_filter = true) const;
  [[nodiscard]] std::vector<entt::handle> create_ref(const MSelectionList &in_list, bool is_filter = true) const;
};

}  // namespace doodle::maya_plug

namespace fmt {
/**
 * @brief
 */
template <>
struct formatter< ::doodle::maya_plug::reference_file_ns::generate_abc_file_path> : formatter<std::string> {
  template <typename FormatContext>
  auto format(const ::doodle::maya_plug::reference_file_ns::generate_abc_file_path &in_, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(
        ctx.out(), "extract_scene_name : {} extract_reference_name : {} use_add_range : {} add_external_string : {}",
        in_.extract_scene_name, in_.extract_reference_name, in_.use_add_range,
        in_.add_external_string ? *in_.add_external_string : std::string{}
    );
  }
};
}  // namespace fmt
