//
// Created by TD on 2021/12/6.
//

#include "qcloth_shape.h"

#include <doodle_lib/metadata/project.h>
#include <maya/MDagPath.h>
#include <maya/MFileIO.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnMesh.h>
#include <maya/MPlug.h>
#include "reference_file.h"
#include <maya_plug/data/maya_file_io.h>
#include <maya_plug/maya_plug_fwd.h>
#include <maya/MNamespace.h>

namespace doodle::maya_plug {
qcloth_shape::qcloth_shape() = default;

qcloth_shape::qcloth_shape(const entt::handle& in_ref_file, const MObject& in_object)
    : qcloth_shape() {
  p_ref_file = in_ref_file;
  obj        = in_object;
  chick_component<reference_file>(p_ref_file);
}
bool qcloth_shape::set_cache_folder() const {
  MStatus k_s{};
  MFnDependencyNode k_node{obj, &k_s};
  DOODLE_CHICK(k_s);
  string k_name{d_str{k_node.name(&k_s)}};
  DOODLE_CHICK(k_s);
  string k_namespace = p_ref_file.get<reference_file>().get_namespace();
  auto& k_cfg        = p_ref_file.get<reference_file>().get_prj().get<project::cloth_config>();
  boost::replace_all(k_name, k_cfg.cloth_proxy, k_cfg.cloth_shape);
  DOODLE_LOG_INFO("推测布料节点 {}", k_name);

  /// 选择解算节点
  MSelectionList l_selection_list{};
  k_s = l_selection_list.add(d_str{k_name}, true);
  DOODLE_CHICK(k_s);
  if (l_selection_list.length(&k_s) > 1) {
    DOODLE_CHICK(k_s);
    throw doodle_error{"出现重名物体"};
  }
  if (l_selection_list.isEmpty(&k_s)) {
    DOODLE_CHICK(k_s);
    throw doodle_error{"没有找到解算布料节点"};
  }

  /// \brief 获得解算节点fn
  MObject k_shape{};
  k_s = l_selection_list.getDependNode(0, k_shape);
  DOODLE_CHICK(k_s);
  k_s = k_node.setObject(k_shape);
  DOODLE_CHICK(k_s);
  string k_node_name = d_str{MNamespace::stripNamespaceFromName(k_node.name(), &k_s)};
  DOODLE_CHICK(k_s);
  {
    auto k_cache = k_node.findPlug(d_str{"cacheFolder"}, false, &k_s);
    DOODLE_CHICK(k_s);
    auto k_file_name       = maya_file_io::get_current_path();
    /// \brief 使用各种信息确认缓存相对路径
    const string& l_string = fmt::format("cache/{}/{}/{}",
                                         k_file_name.stem().generic_string(),
                                         k_namespace,
                                         k_node_name);
    DOODLE_LOG_INFO("设置缓存路径 {}", l_string);
    /// \brief 删除已经缓存的目录
    auto k_path = maya_file_io::work_path(l_string);
    if (FSys::exists(k_path)) {
      DOODLE_LOG_INFO("发现缓存目录, 主动删除 {}", k_path);
      FSys::remove_all(k_path);
    }
    FSys::create_directories(k_path);
    k_s = k_cache.setString(d_str{l_string});
    DOODLE_CHICK(k_s);
  }
  {
    auto k_cache = k_node.findPlug(d_str{"cacheName"}, true, &k_s);
    DOODLE_CHICK(k_s);
    k_cache.setString(d_str{k_node_name});
  }
  return true;
}
bool qcloth_shape::create_cache() const {
  if (obj.isNull())
    throw doodle_error{"空组件"};
  MStatus k_s{};
  MFnDagNode k_fn_done{obj, &k_s};
  DOODLE_CHICK(k_s);

  MDagPath k_dag_path{};
  k_s = k_fn_done.getPath(k_dag_path);
  DOODLE_CHICK(k_s);
  k_s = k_dag_path.extendToShape();
  DOODLE_CHICK(k_s);

  MFnMesh k_shape{k_dag_path, &k_s};
  DOODLE_CHICK(k_s);
  k_s = k_shape.updateSurface();
  DOODLE_CHICK(k_s);
  k_s = k_shape.syncObject();
  DOODLE_CHICK(k_s);
  return true;
}

void qcloth_shape::create_sim_cloth(const entt::handle& in_low_spahe) {

}

}  // namespace doodle::maya_plug
