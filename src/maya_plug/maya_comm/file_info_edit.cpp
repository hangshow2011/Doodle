//
// Created by TD on 2024/1/11.
//

#include "file_info_edit.h"

#include <maya_plug/data/maya_tool.h>
#include <maya_plug/data/qcloth_shape.h>
#include <maya_plug/node/files_info.h>

#include <maya/MArgDatabase.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnReference.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItSelectionList.h>

namespace doodle::maya_plug {
file_info_edit::file_info_edit()  = default;
file_info_edit::~file_info_edit() = default;

MSyntax file_info_edit_syntax() {
  MSyntax l_syntax{};
  //  l_syntax.addFlag("-rf", "-reference_file", MSyntax::kString);
  //  l_syntax.addFlag("-rnp", "-reference_file_namespace", MSyntax::kString);
  //  l_syntax.addFlag("-rp", "-reference_file_path", MSyntax::kString);
  //  l_syntax.addFlag("-is", "-is_solve", MSyntax::kBoolean);
  //  l_syntax.addFlag("-co", "-collision_objects", MSyntax::kString);
  //  l_syntax.addFlag("-wf", "-wind_fields", MSyntax::kString);

  // 强制
  l_syntax.addFlag("-f", "-force", MSyntax::kNoArg);
  // 节点
  l_syntax.addFlag("-n", "-node", MSyntax::kString);
  // 添加碰撞
  l_syntax.addFlag("-ac", "-add_collision", MSyntax::kSelectionItem);
  // 添加风场
  l_syntax.addFlag("-aw", "-add_wind_field", MSyntax::kSelectionItem);
  // frame_samples
  l_syntax.addFlag("-fs", "-frame_samples", MSyntax::kLong);
  // time_scale

  l_syntax.addFlag("-ts", "-time_scale", MSyntax::kDouble);
  // length_scale
  l_syntax.addFlag("-ls", "-length_scale", MSyntax::kDouble);
  // max_cg_iteration
  l_syntax.addFlag("-mci", "-max_cg_iteration", MSyntax::kLong);
  // cg_accuracy
  l_syntax.addFlag("-ca", "-cg_accuracy", MSyntax::kLong);
  // gravity
  l_syntax.addFlag("-g", "-gravity", MSyntax::kString);
  // 开始复盖配置
  l_syntax.addFlag("-c", "-override", MSyntax::kNoArg);

  l_syntax.setObjectType(MSyntax::kSelectionList);
  l_syntax.useSelectionAsDefault(true);

  return l_syntax;
}

MStatus file_info_edit::doIt(const MArgList &in_list) {
  MStatus l_status{};
  MArgDatabase l_arg_data{syntax(), in_list, &l_status};
  maya_chick(l_status);

  if (l_arg_data.isFlagSet("-f")) {
    is_force   = true;
    p_run_func = &file_info_edit::create_node;
  }
  if (l_arg_data.isFlagSet("-n")) {
    MString l_node_name{};
    l_arg_data.getFlagArgument("-n", 0, l_node_name);
    MSelectionList l_selection_list{};
    l_status = l_selection_list.add(l_node_name);
    maya_chick(l_status);
    l_status = l_selection_list.getDependNode(0, p_current_node);
    maya_chick(l_status);
  }
  if (l_arg_data.isFlagSet("-ac")) {
    maya_chick(l_arg_data.getObjects(p_selection_list));
    p_run_func = &file_info_edit::add_collision;
  }
  if (l_arg_data.isFlagSet("-aw")) {
    maya_chick(l_arg_data.getObjects(p_selection_list));
    p_run_func = &file_info_edit::add_wind_field;
  }
  if (l_arg_data.isFlagSet("-fs")) {
    std::int32_t l_frame_samples{};
    l_arg_data.getFlagArgument("-fs", 0, l_frame_samples);
    frame_samples = l_frame_samples;
    p_run_func    = &file_info_edit::set_node_attr;
  }
  if (l_arg_data.isFlagSet("-ts")) {
    std::double_t l_time_scale{};
    l_arg_data.getFlagArgument("-ts", 0, l_time_scale);
    time_scale = l_time_scale;
    p_run_func = &file_info_edit::set_node_attr;
  }
  if (l_arg_data.isFlagSet("-ls")) {
    std::double_t l_length_scale{};
    l_arg_data.getFlagArgument("-ls", 0, l_length_scale);
    length_scale = l_length_scale;
    p_run_func   = &file_info_edit::set_node_attr;
  }
  if (l_arg_data.isFlagSet("-mci")) {
    std::int32_t l_max_cg_iteration{};
    l_arg_data.getFlagArgument("-mci", 0, l_max_cg_iteration);
    max_cg_iteration = l_max_cg_iteration;
    p_run_func       = &file_info_edit::set_node_attr;
  }
  if (l_arg_data.isFlagSet("-ca")) {
    std::int32_t l_cg_accuracy{};
    l_arg_data.getFlagArgument("-ca", 0, l_cg_accuracy);
    cg_accuracy = l_cg_accuracy;
    p_run_func  = &file_info_edit::set_node_attr;
  }
  if (l_arg_data.isFlagSet("-g")) {
    MString l_gravity{};
    l_arg_data.getFlagArgument("-g", 0, l_gravity);
    std::array<std::double_t, 3> l_gravity_array{};
    MStringArray l_str_array{};
    l_gravity.split(' ', l_str_array);
    if (l_str_array.length() != 3) return MStatus::kInvalidParameter;
    for (auto i = 0; i < l_str_array.length(); ++i) {
      l_gravity_array[i] = l_str_array[i].asDouble();
    }
    gravity    = l_gravity_array;
    p_run_func = &file_info_edit::set_node_attr;
  }
  if (l_arg_data.isFlagSet("-c")) {
    p_run_func = &file_info_edit::override_node_attr;
  }

  return redoIt();
}

MStatus file_info_edit::redoIt() {
  MStatus l_status{};
  if (p_run_func == nullptr) return MStatus::kInvalidParameter;
  l_status = (this->*p_run_func)();
  return l_status;
}

MStatus file_info_edit::override_node_attr() {
  MStatus l_status{};
  for (MItDependencyNodes l_it{MFn::kPluginDependNode, &l_status}; !l_it.isDone(); l_it.next()) {
    MFnDependencyNode l_fn_node{l_it.thisNode(), &l_status};
    maya_chick(l_status);
    if (l_fn_node.typeId() == doodle_file_info::doodle_id) {
      reference_file l_ref_file{l_it.thisNode()};
      auto l_node    = l_it.thisNode();
      auto l_ql_core = qcloth_shape::get_ql_solver(l_ref_file.get_all_object());
      set_attribute(l_ql_core, "simpleSubsampling", get_attribute<std::int32_t>(l_node, "simple_subsampling"));
      set_attribute(l_ql_core, "frameSamples", get_attribute<std::int32_t>(l_node, "frame_samples"));
      set_attribute(l_ql_core, "timeScale", get_attribute<std::double_t>(l_node, "time_scale"));
      set_attribute(l_ql_core, "lengthScale", get_attribute<std::double_t>(l_node, "length_scale"));
      set_attribute(l_ql_core, "maxCGIteration", get_attribute<std::int32_t>(l_node, "max_cg_iteration"));
      set_attribute(l_ql_core, "cgAccuracy", get_attribute<std::int32_t>(l_node, "cg_accuracy"));
      set_attribute(l_ql_core, "gravity0", get_attribute<std::double_t>(l_node, "gravityx"));
      set_attribute(l_ql_core, "gravity1", get_attribute<std::double_t>(l_node, "gravityy"));
      set_attribute(l_ql_core, "gravity2", get_attribute<std::double_t>(l_node, "gravityz"));
    }
  }
  return l_status;
}

MStatus file_info_edit::add_wind_field() {
  MStatus l_status{};
  MFnDependencyNode l_fn_node{p_current_node, &l_status};
  maya_chick(l_status);

  // 清除所有的连接
  {
    auto l_plug = l_fn_node.findPlug(doodle_file_info::wind_field, true);
    if (l_plug.isConnected()) {
      auto l_connected_plug = l_plug.source(&l_status);
      maya_chick(l_status);
      maya_chick(dg_modifier_.disconnect(l_connected_plug, l_plug));
    }
  }
  // 重新连接
  for (MItSelectionList l_it{p_selection_list}; !l_it.isDone(); l_it.next()) {
    MObject l_obj{};
    l_it.getDependNode(l_obj);
    MFnDependencyNode l_fn_node_2{l_obj, &l_status};
    maya_chick(l_status);
    if (l_fn_node_2.typeId() == MFn::kMesh) {
      maya_chick(dg_modifier_.connect(get_plug(l_obj, "message"), get_plug(p_current_node, "wind_field")));
    }
    break;
  }
  maya_chick(dg_modifier_.doIt());
  return l_status;
}

MStatus file_info_edit::add_collision() {
  MStatus l_status{};
  MFnDependencyNode l_fn_node{p_current_node, &l_status};
  maya_chick(l_status);

  // 清除所有的连接
  {
    auto l_plug = l_fn_node.findPlug(doodle_file_info::collision_objects, true);
    for (auto i = 0; i < l_plug.numConnectedElements(); ++i) {
      auto l_connected_plug = l_plug.connectionByPhysicalIndex(i, &l_status);
      maya_chick(l_status);
      maya_chick(dg_modifier_.disconnect(l_connected_plug, l_plug));
    }
  }
  // 重新连接
  for (MItSelectionList l_it{p_selection_list}; !l_it.isDone(); l_it.next()) {
    MObject l_obj{};
    l_it.getDependNode(l_obj);
    MFnDependencyNode l_fn_node_2{l_obj, &l_status};
    maya_chick(l_status);
    if (l_fn_node_2.typeId() == MFn::kMesh) {
      maya_chick(dg_modifier_.connect(get_plug(l_obj, "message"), get_plug(p_current_node, "collision_objects")));
    }
  }
  maya_chick(dg_modifier_.doIt());
  return l_status;
}

MStatus file_info_edit::create_node() {
  MStatus l_status{};
  {  // 创建节点
    if (has_node() && !is_force) {
      displayError("has node, use -f to force");
      return MS::kSuccess;
    }
    if (is_force) {
      l_status = delete_node();
      maya_chick(l_status);
    }
    for (MItDependencyNodes l_it{MFn::kReference, &l_status}; !l_it.isDone(); l_it.next()) {
      MFnReference l_fn_ref{l_it.thisNode(), &l_status};
      if (!l_fn_ref.isLoaded()) continue;

      MObject l_node = dg_modifier_.createNode(doodle_file_info::doodle_id, &l_status);
      maya_chick(l_status);
      MFnDependencyNode l_fn_node{l_node, &l_status};
      maya_chick(l_status);

      maya_chick(dg_modifier_.connect(get_plug(l_it.thisNode(), "message"), get_plug(l_node, "reference_file")));
      dg_modifier_.newPlugValueString(
          get_plug(l_node, "reference_file_path"), l_fn_ref.fileName(false, false, false, &l_status)
      );
      maya_chick(l_status);
      dg_modifier_.newPlugValueString(
          get_plug(l_node, "reference_file_namespace"), l_fn_ref.associatedNamespace(false, &l_status)
      );
      maya_chick(l_status);
      maya_chick(dg_modifier_.doIt());
    }
  }
  return l_status;
}

MStatus file_info_edit::set_node_attr() {
  MStatus l_status{};
  MFnDependencyNode l_fn_node{p_current_node, &l_status};
  maya_chick(l_status);

  if (frame_samples.has_value()) {
    maya_chick(dg_modifier_.newPlugValueInt(
        l_fn_node.findPlug(doodle_file_info::frame_samples, false, &l_status), frame_samples.value()
    ));
  }
  if (time_scale.has_value()) {
    maya_chick(dg_modifier_.newPlugValueDouble(
        l_fn_node.findPlug(doodle_file_info::time_scale, false, &l_status), time_scale.value()
    ));
  }
  if (length_scale.has_value()) {
    maya_chick(dg_modifier_.newPlugValueDouble(
        l_fn_node.findPlug(doodle_file_info::length_scale, false, &l_status), length_scale.value()
    ));
  }
  if (max_cg_iteration.has_value()) {
    maya_chick(dg_modifier_.newPlugValueInt(
        l_fn_node.findPlug(doodle_file_info::max_cg_iteration, false, &l_status), max_cg_iteration.value()
    ));
  }
  if (cg_accuracy.has_value()) {
    maya_chick(dg_modifier_.newPlugValueInt(
        l_fn_node.findPlug(doodle_file_info::cg_accuracy, false, &l_status), cg_accuracy.value()
    ));
  }
  if (gravity.has_value()) {
    maya_chick(dg_modifier_.newPlugValueDouble(get_plug(p_current_node, "gravityx"), gravity.value()[0]));
    maya_chick(dg_modifier_.newPlugValueDouble(get_plug(p_current_node, "gravityy"), gravity.value()[1]));
    maya_chick(dg_modifier_.newPlugValueDouble(get_plug(p_current_node, "gravityz"), gravity.value()[2]));
  }
  maya_chick(dg_modifier_.doIt());
  return l_status;
}

MStatus file_info_edit::undoIt() { return dg_modifier_.undoIt(); }

bool file_info_edit::isUndoable() const { return true; }

MStatus file_info_edit::delete_node() {
  MStatus l_status{};
  for (MItDependencyNodes l_it{MFn::kPluginDependNode, &l_status}; !l_it.isDone(); l_it.next()) {
    MFnDependencyNode l_fn_node{l_it.thisNode(), &l_status};
    maya_chick(l_status);
    if (l_fn_node.typeId() == doodle_file_info::doodle_id) {
      maya_chick(dg_modifier_.deleteNode(l_it.thisNode(&l_status)));
      maya_chick(l_status);
    }
  }
  return l_status;
}

bool file_info_edit::has_node() const {
  MStatus l_status{};
  for (MItDependencyNodes l_it{MFn::kPluginDependNode, &l_status}; !l_it.isDone(); l_it.next()) {
    MFnDependencyNode l_fn_node{l_it.thisNode(), &l_status};
    maya_chick(l_status);
    if (l_fn_node.typeId() == doodle_file_info::doodle_id) return true;
  }
  return false;
}

}  // namespace doodle::maya_plug
