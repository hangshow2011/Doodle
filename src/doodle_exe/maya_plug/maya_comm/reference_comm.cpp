//
// Created by TD on 2021/12/13.
//

#include "reference_comm.h"

#include <doodle_lib/lib_warp/entt_warp.h>
#include <doodle_lib/metadata/metadata.h>
#include <doodle_lib/metadata/project.h>
#include <doodle_lib/client/client.h>
#include <doodle_lib/long_task/process_pool.h>
#include <doodle_lib/core/app_base.h>
#include <doodle_lib/core/core_set.h>

#include <maya/MDagPath.h>
#include <maya/MFileIO.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MUuid.h>
#include <maya/adskDataAssociations.h>
#include <maya/adskDataStream.h>
#include <maya/adskDebugPrint.h>
#include <maya/MArgParser.h>
#include <maya/MAnimControl.h>
#include <maya/MNamespace.h>
#include <maya/MArgDatabase.h>
#include <maya/MItSelectionList.h>

#include <maya_plug/data/reference_file.h>
#include <maya_plug/data/maya_file_io.h>
#include <maya_plug/data/qcloth_shape.h>

#include <magic_enum.hpp>

namespace doodle::maya_plug {
namespace {
constexpr const char doodle_startTime[]              = "-st";
constexpr const char doodle_endTime[]                = "-et";
constexpr const char doodle_project_path[]           = "-pr";
constexpr const char doodle_export_type[]            = "-ef";
constexpr const char doodle_export_use_select[]      = "-s";

constexpr const char doodle_export_type_long[]       = "-exportType";
constexpr const char doodle_export_use_select_long[] = "-select";
constexpr const char doodle_project_path_long[]      = "-project";
constexpr const char doodle_startTime_long[]         = "-startTime";
constexpr const char doodle_endTime_long[]           = "-endTime";

};  // namespace

MSyntax ref_file_sim_syntax() {
  MSyntax syntax{};
  syntax.addFlag(doodle_startTime, doodle_startTime_long, MSyntax::kTime);
  syntax.addFlag(doodle_endTime, doodle_endTime_long, MSyntax::kTime);
  return syntax;
}
MSyntax ref_file_export_syntax() {
  MSyntax syntax{};
  syntax.addFlag(doodle_startTime, doodle_startTime_long, MSyntax::kTime);
  syntax.addFlag(doodle_endTime, doodle_endTime_long, MSyntax::kTime);
  syntax.addFlag(doodle_export_type, doodle_export_type_long, MSyntax::kString);
  syntax.addFlag(doodle_export_use_select, doodle_export_use_select_long, MSyntax::kBoolean);
  return syntax;
}

MSyntax load_project_syntax() {
  MSyntax syntax{};
  syntax.addFlag(doodle_project_path, doodle_project_path_long, MSyntax::kString);
  return syntax;
}
MSyntax set_cloth_cache_path_syntax() {
  MSyntax l_syntax{};
  l_syntax.addArg(MSyntax::MArgType::kSelectionItem);
  l_syntax.useSelectionAsDefault(true);
  l_syntax.setObjectType(MSyntax::MObjectFormat::kSelectionList);
  return l_syntax;
}

MStatus create_ref_file_command::doIt(const MArgList& in_arg) {
  MStatus k_s;
  MArgParser k_prase{syntax(), in_arg, &k_s};
  entt::handle k_def_prj;

  chick_true<doodle_error>(g_reg()->ctx().contains<root_ref>(), DOODLE_LOC, "没有获取到项目");
  k_def_prj = g_reg()->ctx().at<root_ref>().root_handle();

  DOODLE_LOG_INFO(
      "获得默认项目 {}", bool(k_def_prj));
  DOODLE_LOG_INFO("开始清除引用实体")
  auto k_ref_view   = g_reg()->view<reference_file>();
  auto k_cloth_view = g_reg()->view<qcloth_shape>();
  g_reg()->destroy(k_ref_view.begin(), k_ref_view.end());
  g_reg()->destroy(k_cloth_view.begin(), k_cloth_view.end());

  auto k_names = MNamespace::getNamespaces(MNamespace::rootNamespace(), false, &k_s);

  for (int l_i = 0; l_i < k_names.length(); ++l_i) {
    auto&& k_name = k_names[l_i];
    reference_file k_ref{};

    if (k_ref.set_namespace(d_str{k_name})) {
      if (k_ref.is_loaded()) {
        DOODLE_LOG_INFO("获得引用文件 {}", k_ref.path);
        auto k_h = make_handle();
        k_h.emplace<reference_file>(k_ref);
      } else {
        DOODLE_LOG_INFO("引用文件 {} 未加载", k_ref.path);
      }
    } else {
      DOODLE_LOG_WARN("命名空间 {} 中无有效引用", k_name);
    }
  }

  return k_s;
}
MStatus ref_file_load_command::doIt(const MArgList& in_arg_list) {
  MStatus k_s{};
  auto k_j_str = maya_file_io::get_channel_date();
  std::vector<entt::entity> k_delete{};

  nlohmann::json k_j{};
  try {
    k_j = nlohmann::json::parse(k_j_str);
  } catch (const nlohmann::json::exception& error) {
    DOODLE_LOG_WARN("解析元数据错误 {}, 取消解析元数据，使用默认元数据", error.what())
  }
  for (auto&& [k_e, k_ref] : g_reg()->view<reference_file>().each()) {
    auto l_i = make_handle(k_e);
    auto l_p = k_ref.path;
    if (k_j.contains(l_p)) {
      DOODLE_LOG_INFO("加载元数据 {}", l_p);
      entt_tool::load_comm<reference_file>(l_i, k_j.at(l_p));
    } else {
      l_i.get<reference_file>().use_sim = true;
    }

    if (!l_i.get<reference_file>().use_sim) {
      DOODLE_LOG_INFO("引用文件{}不解算", l_i.get<reference_file>().path);
      k_delete.push_back(l_i);
    }
  }
  g_reg()->destroy(k_delete.begin(), k_delete.end());

  for (auto&& [k_e, k_ref] : g_reg()->view<reference_file>().each()) {
    if (k_ref.replace_sim_assets_file()) {
    } else {
      k_delete.push_back(k_e);
    }
  }
  g_reg()->destroy(k_delete.begin(), k_delete.end());
  return k_s;
}
MStatus ref_file_sim_command::doIt(const MArgList& in_arg) {
  MStatus k_s{};
  MArgParser k_prase{syntax(), in_arg, &k_s};
  auto k_start = MAnimControl::minTime();
  auto k_end   = MAnimControl::maxTime();

  if (k_prase.isFlagSet(doodle_startTime, &k_s)) {
    DOODLE_CHICK(k_s);
    k_s = k_prase.getFlagArgument(doodle_startTime, 0, k_start);
    DOODLE_CHICK(k_s);
  }
  if (k_prase.isFlagSet(doodle_endTime, &k_s)) {
    DOODLE_CHICK(k_s);
    k_s = k_prase.getFlagArgument(doodle_endTime, 0, k_end);
    DOODLE_CHICK(k_s);
  }
  DOODLE_LOG_INFO(
      "解算开始时间 {}  结束时间 {}  ",
      k_start.value(), k_end.value());

  for (auto&& [k_e, k_ref] : g_reg()->view<reference_file>().each()) {
    DOODLE_LOG_INFO("引用文件{}发现需要设置解算碰撞体", k_ref.path)
    /// \brief 生成需要的 布料实体
    qcloth_shape::create(make_handle(k_e));
    /// \brief 添加碰撞体
    k_ref.add_collision();
    /// \brief 更新ql初始化状态
    k_ref.qlUpdateInitialPose();
  }
  for (auto&& [k_e, k_qs] : g_reg()->view<qcloth_shape>().each()) {
    DOODLE_LOG_INFO("开始设置解算布料的缓存文件夹")
    k_qs.set_cache_folder();
  }

  /// \brief 在这里我们保存引用
  try {
    auto k_save_file = maya_file_io::work_path("ma");
    if (!FSys::exists(k_save_file)) {
      FSys::create_directories(k_save_file);
    }
    k_save_file /= maya_file_io::get_current_path().filename();
    k_s = MFileIO::saveAs(d_str{k_save_file.generic_string()}, nullptr, true);
    DOODLE_LOG_INFO("保存文件到 {}", k_save_file);
    DOODLE_CHICK(k_s);
  } catch (maya_error& error) {
    DOODLE_LOG_WARN("无法保存文件: {}", error);
  }

  for (MTime k_t = k_start; k_t <= k_end; ++k_t) {
    MAnimControl::setCurrentTime(k_t);
    for (auto&& [k_e, k_ref] : g_reg()->view<qcloth_shape>().each()) {
      k_ref.create_cache();
    }
  }
  return k_s;
}
MStatus ref_file_export_command::doIt(const MArgList& in_arg) {
  MStatus k_s{};
  MArgParser k_prase{syntax(), in_arg, &k_s};
  MTime k_start{MAnimControl::minTime()};
  MTime k_end = MAnimControl::maxTime();
  bool use_select{false};
  reference_file::export_type k_export_type{};

  if (k_prase.isFlagSet(doodle_startTime, &k_s)) {
    DOODLE_CHICK(k_s);
    k_s = k_prase.getFlagArgument(doodle_startTime, 0, k_start);
    DOODLE_CHICK(k_s);
  }
  if (k_prase.isFlagSet(doodle_endTime, &k_s)) {
    DOODLE_CHICK(k_s);
    k_s = k_prase.getFlagArgument(doodle_endTime, 0, k_end);
    DOODLE_CHICK(k_s);
  }
  if (k_prase.isFlagSet(doodle_export_type, &k_s)) {
    DOODLE_CHICK(k_s);
    MString k_k_export_type_s{};
    k_s = k_prase.getFlagArgument(doodle_export_type, 0, k_k_export_type_s);
    DOODLE_CHICK(k_s);
    k_export_type = magic_enum::enum_cast<reference_file::export_type>(
                        d_str{k_k_export_type_s}.str())
                        .value();
  }
  if (k_prase.isFlagSet(doodle_export_use_select, &k_s)) {
    DOODLE_CHICK(k_s);
    k_s = k_prase.getFlagArgument(doodle_export_use_select, 0, use_select);
    DOODLE_CHICK(k_s);
  }

  DOODLE_LOG_INFO(
      "导出开始时间 {}  结束时间 {} 导出类型 {} ",
      k_start.value(), k_end.value(), magic_enum::enum_name(k_export_type));

  if (use_select) {
    DOODLE_LOG_INFO("开始使用交互式导出");
    MSelectionList k_select{};
    k_s = MGlobal::getActiveSelectionList(k_select);
    for (auto&& [k_e, k_r] : g_reg()->view<reference_file>().each()) {
      if (k_r.has_node(k_select)) {
        reference_file::export_arg l_export_arg{k_export_type, k_start, k_end};
        k_r.export_file(l_export_arg);
      }
    }
  } else {
    DOODLE_LOG_INFO("全部的引用文件导出")
    for (auto&& [k_e, k_r] : g_reg()->view<reference_file>().each()) {
      reference_file::export_arg l_export_arg{k_export_type, k_start, k_end};
      k_r.export_file(l_export_arg);
    }
  }

  return k_s;
}

MStatus load_project::doIt(const MArgList& in_arg) {
  MStatus k_s{};
  MArgParser k_prase{syntax(), in_arg, &k_s};
  FSys::path k_path{};

  if (k_prase.isFlagSet(doodle_project_path, &k_s)) {
    DOODLE_CHICK(k_s);
    MString k_path_M{};
    k_s    = k_prase.getFlagArgument(doodle_project_path, 0, k_path_M);
    k_path = k_path_M.asUTF8();
    core_set_init{}.init_project(k_path);
  } else {
    core_set_init{}.init_project();
  }
  DOODLE_LOG_INFO("开始打开项目 {}", k_path);
  if (MGlobal::mayaState(&k_s) != MGlobal::kInteractive) {
    while (!g_main_loop().empty()) {
      app_base::Get().loop_one();
    }
  }
  return k_s;
}

MStatus set_cloth_cache_path::doIt(const MArgList& in_list) {
  MStatus l_status{};
  MArgDatabase k_prase{syntax(), in_list, &l_status};
  MSelectionList l_list{};
  DOODLE_CHICK(k_prase.getObjects(l_list));

  MObject l_object{};
  for (auto&& [k_e, k_ref] : g_reg()->view<reference_file>().each()) {
    DOODLE_LOG_INFO("引用文件{}被发现需要设置解算碰撞体", k_ref.path)
    /// \brief 生成需要的 布料实体
    if (!l_list.isEmpty())
      for (auto l_i = MItSelectionList{l_list}; !l_i.isDone(); l_i.next()) {
        DOODLE_CHICK(l_i.getDependNode(l_object));
        if (k_ref.has_node(l_object))
          qcloth_shape::create(make_handle(k_e));
      }
    else
      qcloth_shape::create(make_handle(k_e));
  }
  for (auto&& [k_e, k_qs] : g_reg()->view<qcloth_shape>().each()) {
    DOODLE_LOG_INFO("开始设置解算布料的缓存文件夹")
    k_qs.set_cache_folder(core_set::getSet().get_user_en());
  }
  return l_status;
}
}  // namespace doodle::maya_plug
