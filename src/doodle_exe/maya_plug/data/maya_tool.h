//
// Created by TD on 2021/12/16.
//

#pragma once
#include <maya/MApiNamespace.h>
#include <maya/MPlug.h>
#include <maya_plug/maya_plug_fwd.h>
namespace doodle::maya_plug {
/**
 * @brief 这个插件会寻找节点的属性, 并在找到时返回
 * @param in_node 节点obj
 * @param in_name 属性名称
 * @return 找到的属性类
 *
 * 这个函数会先寻找节点本身的属性, 如果没有找到, 并且节点下方有 形状 节点, 还会扩展到形状节点中检查属性
 */
MPlug get_plug(const MObject& in_node, const std::string& in_name);

template <typename T>
void set_attribute(const MObject& in_node,
                   const std::string& in_name,
                   const T& in_t) {
  auto l_s = get_plug(in_node, in_name).setValue(in_t);
  DOODLE_CHICK(l_s);
}
template <typename T>
T get_attribute(const MObject& in_node,
                const std::string& in_name) {
  T result;
  auto l_s = get_plug(in_node, in_name).getValue(result);
  DOODLE_CHICK(l_s);
  return result;
}

MObject get_shading_engine(const MObject& in_node);

MObject get_first_mesh(const MObject& in_node);

MObject get_shape(const MObject& in_object);
MObject get_transform(const MObject& in_object);

void add_child(const MObject& in_praent, MObject& in_child);

void add_mat(const MObject& in_obj, MObject& in_ref_obj);
std::string node_name(const MObject& in_obj);
}  // namespace doodle::maya_plug
