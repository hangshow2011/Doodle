//
// Created by TD on 2021/12/6.
//

#include "qcloth_shape.h"
#include "doodle_core/metadata/metadata.h"

#include <maya/MDagPath.h>
#include <maya/MFileIO.h>
#include <maya/MFnMesh.h>
#include <maya/MPlug.h>
#include <maya/MNamespace.h>
#include <maya/MItDag.h>
#include <maya/MDagModifier.h>
#include <maya/MFnSet.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MAnimControl.h>

#include <maya_plug/data/reference_file.h>
#include <maya_plug/data/maya_file_io.h>
#include <maya_plug/maya_plug_fwd.h>

#include <magic_enum.hpp>

namespace doodle::maya_plug {

namespace qcloth_shape_n {
maya_obj::maya_obj() = default;
maya_obj::maya_obj(const MObject& in_object) {
  obj = in_object;
  MStatus k_s;
  MFnDependencyNode k_node{in_object, &k_s};
  DOODLE_CHICK(k_s);
  p_name = d_str{k_node.name(&k_s)};
  DOODLE_CHICK(k_s);
}
}  // namespace qcloth_shape_n

namespace {
/**
 * @brief 复制并制作低模
 * @param in_object 传入的低模物体
 * @param in_parent 传入低模的父物体
 * @return 复制的低模
 */
MObject make_low_node(const MObject& in_object, const MObject& in_parent, const std::string& in_suffix) {
  MStatus l_s{};
  MFnDagNode l_node{};
  /// \brief 返回的结果, 复制出来的obj
  MObject l_r{};

  // 复制传入节点
  l_s = l_node.setObject(in_object);
  DOODLE_CHICK(l_s);
  l_r = l_node.duplicate(false, false, &l_s);
  DOODLE_LOG_INFO("复制节点 {}", l_node.name(&l_s));
  DOODLE_CHICK(l_s);

  // 设置复制节点的名称
  l_s = l_node.setObject(l_r);
  DOODLE_CHICK(l_s);
  std::string k_anim_mesh_name = d_str{l_node.name(&l_s)};
  DOODLE_CHICK(l_s);
  l_node.setName(d_str{fmt::format("{}_{}", k_anim_mesh_name, in_suffix)}, false, &l_s);
  DOODLE_CHICK(l_s);
  DOODLE_LOG_INFO("设置复制节点名称 {}", l_node.name(&l_s));
  DOODLE_CHICK(l_s);

  // 设置材质属性
  MFnSet l_mat{get_shading_engine(in_object), &l_s};
  DOODLE_CHICK(l_s);
  l_s = l_mat.addMember(l_r);
  DOODLE_CHICK(l_s);

  /// 将复制节点添加为父节点的子物体
  l_s = l_node.setObject(in_parent);
  DOODLE_CHICK(l_s);
  l_s = l_node.addChild(l_r);
  DOODLE_CHICK(l_s);
  DOODLE_LOG_INFO("设置复制节点父物体 {}", l_node.name(&l_s));
  DOODLE_CHICK(l_s);
  return l_r;
}

/**
 * @brief 复制一次高模并将高模重新设置为父物体
 * @param in_high_node 传入的高模
 * @param in_parent 传入的父物体
 * @return 复制出来的高模列表
 */
std::vector<MObject> make_high_node(const qcloth_shape_n::shape_list& in_high_node, const MObject& in_parent) {
  MStatus l_s{};
  std::vector<MObject> l_r{};
  MFnDagNode l_node{};
  /// 复制高模节点作为输出
  std::transform(in_high_node.begin(), in_high_node.end(),
                 std::back_inserter(l_r),
                 [&](const qcloth_shape_n::maya_obj& in_object) -> MObject {
                   /// 复制模型
                   l_s = l_node.setObject(in_object.obj);
                   DOODLE_CHICK(l_s);
                   auto l_r = l_node.duplicate(false, false, &l_s);
                   DOODLE_CHICK(l_s);
                   DOODLE_LOG_INFO("复制高模节点 {}", l_node.name(&l_s));
                   DOODLE_CHICK(l_s);

                   // 设置复制节点的名称
                   l_node.setObject(l_r);
                   std::string k_anim_mesh_name = d_str{l_node.name(&l_s)};
                   DOODLE_CHICK(l_s);
                   l_node.setName(d_str{fmt::format("{}_out_mesh", k_anim_mesh_name)}, false, &l_s);
                   DOODLE_CHICK(l_s);
                   DOODLE_LOG_INFO("设置复制高模节点名称 {}", l_node.name(&l_s));
                   DOODLE_CHICK(l_s);

                   /// 设置材质
                   MFnSet l_mat{get_shading_engine(in_object.obj), &l_s};
                   DOODLE_CHICK(l_s);
                   l_s = l_mat.addMember(l_r);
                   DOODLE_CHICK(l_s);

                   /// 将复制节点添加为父节点的子物体
                   l_s = l_node.setObject(in_parent);
                   DOODLE_CHICK(l_s);
                   l_s = l_node.addChild(l_r);
                   DOODLE_CHICK(l_s);
                   DOODLE_LOG_INFO("设置复制高模节点父物体 {}", l_node.name(&l_s));
                   DOODLE_CHICK(l_s);

                   return l_r;
                 });
  return l_r;
}
/**
 * @brief 将低模使用包裹变形包裹高模
 * @param in_low 传入的低模节点
 * @param in_high_node 传入的高模节点
 */
void warp_model(const MObject& in_low, const std::vector<MObject>& in_high_node) {
  MStatus l_s{};

  /// 创建包裹变形(maya的包裹变形需要先选择高模, 可以多个, 然后选中低模) 包裹时需要添加独占式绑定参数
  MSelectionList k_select{};
  MFnDependencyNode l_node{};
  std::string l_string{"添加 "};
  /// 添加高模
  std::for_each(in_high_node.begin(), in_high_node.end(),
                [&](const MObject& in_obj) -> void {
                  l_s = k_select.add(in_obj, false);
                  DOODLE_CHICK(l_s);
                  l_s = l_node.setObject(in_obj);
                  DOODLE_CHICK(l_s);
                  l_string += fmt::format("高模节点 {}", l_node.name(&l_s));
                  DOODLE_CHICK(l_s);
                });

  l_s = k_select.add(in_low, false);
  DOODLE_CHICK(l_s);
  l_s = l_node.setObject(in_low);
  DOODLE_CHICK(l_s);
  l_string += fmt::format(" 低模节点 {} 进行包裹变形", l_node.name(&l_s));
  DOODLE_CHICK(l_s);
  DOODLE_LOG_INFO(l_string);
  /// 设置选择
  l_s = MGlobal::setActiveSelectionList(k_select);
  DOODLE_CHICK(l_s);
  l_s = MGlobal::executeCommand(d_str{R"(doWrapArgList "7" {"1","0","1", "2","1","1","0","0"};)"});
  DOODLE_CHICK(l_s);
}

/**
 * @brief 使用混合变形将解算节点的动态传递给动画节点
 * 并且会将动画节点的皮肤簇设置为无效,并将解算的动态设置为1
 *
 * @param in_sim_node 传入的解算节点
 * @param in_anim_node 传入的动画节点
 *
 */
void transfer_dynamic(const MObject& in_sim_node, const MObject& in_anim_node) {
  MStatus l_s{};
  MObject l_skin_cluster{};
  /// \brief 获得组件点上下文
  auto l_shape = get_shape(in_anim_node);

  /// 寻找高模的皮肤簇
  for (MItDependencyGraph i{l_shape,
                            MFn::kSkinClusterFilter,
                            MItDependencyGraph::Direction::kUpstream};
       !i.isDone();
       i.next()) {
    l_skin_cluster = i.currentItem(&l_s);
    DOODLE_CHICK(l_s);
  }

  chick_true<maya_error>(!l_skin_cluster.isNull(), DOODLE_SOURCE_LOC, "没有找到混合变形节点");
  /// 先将高模的皮肤簇权重重置为0;
  MFnSkinCluster l_fn_skin_cluster{l_skin_cluster, &l_s};
  l_s = l_fn_skin_cluster.setEnvelope(0);
  DOODLE_CHICK(l_s);

  DOODLE_LOG_INFO("找到高模皮肤簇 {}， 并将包裹设置为0 ", l_fn_skin_cluster.name(&l_s));
  DOODLE_CHICK(l_s);

  ///  获得名称进行格式化命令
  MFnDagNode l_node{in_anim_node, &l_s};
  DOODLE_CHICK(l_s);
  std::string l_aim_name = d_str{l_node.name(&l_s)};
  DOODLE_CHICK(l_s);

  l_s = l_node.setObject(in_sim_node);
  DOODLE_CHICK(l_s);

  std::string l_sim_name = d_str{l_node.name(&l_s)};
  DOODLE_CHICK(l_s);
  DOODLE_LOG_INFO("生成包裹命令 blendShape -automatic {} {};", l_sim_name, l_aim_name)
  /// 这个设置包裹
  MStringArray l_blend{};
  l_s = MGlobal::executeCommand(d_str{
                                    fmt::format(R"(blendShape -automatic {} {};)",
                                                l_sim_name, l_aim_name)},
                                l_blend, true);
  DOODLE_CHICK(l_s);
  //      /// 开始设置权重
  MGlobal::executeCommand(d_str{fmt::format(R"(setAttr "{}.{}" 1;)",
                                            l_blend[0], l_sim_name)});
};

/**
 * @brief
 * @param in_object 要创建解算网格的网格体
 * @return 解算网格创建的 qlClothShape 和他的下一个标准输出端 kMesh 类型
 */
std::tuple<MObject, MObject> qlCreateCloth(const MObject& in_object) {
  MObject l_mesh{};
  MStatus l_s{};
  /// 创建解算网络
  MSelectionList l_selection_list{};
  l_s = l_selection_list.add(in_object);
  DOODLE_CHICK(l_s);
  l_s = MGlobal::setActiveSelectionList(l_selection_list);
  DOODLE_CHICK(l_s);
  MString l_cloth_shape_name{};
  l_s = MGlobal::executeCommand(d_str{"qlCreateCloth;"}, l_cloth_shape_name);
  DOODLE_CHICK(l_s);
  l_s = l_selection_list.clear();
  DOODLE_CHICK(l_s);

  l_s = l_selection_list.add(l_cloth_shape_name, true);
  DOODLE_CHICK(l_s);

  MObject l_cloth_shape{};
  l_s = l_selection_list.getDependNode(0, l_cloth_shape);
  DOODLE_CHICK(l_s);
  auto l_plug = get_plug(l_cloth_shape, "outputMesh");
  for (MItDependencyGraph i{l_plug, MFn::kMesh};
       !i.isDone();
       i.next()) {
    l_mesh = i.currentItem(&l_s);
    DOODLE_CHICK(l_s);
  }
  chick_true<maya_error>(!l_mesh.isNull(), DOODLE_SOURCE_LOC, "找不到解算网格的输出端");

  return std::make_tuple(l_cloth_shape, l_mesh);
}

/**
 * @brief 在整个文件中全局搜素 解算核心节点
 * @return 解算核心
 */
MObject get_ql_solver() {
  MStatus l_status{};
  MObject l_object{};
  for (MItDependencyNodes i{
           MFn::kPluginLocatorNode, &l_status};
       !i.isDone(); i.next()) {
    auto k_obj = i.thisNode(&l_status);
    MFnDependencyNode k_dep{k_obj};
    if (k_dep.typeName(&l_status) == "qlSolverShape") {
      l_object = k_obj;
    }
  }
  chick_true<maya_error>(!l_object.isNull(), DOODLE_LOC, "没有找到qlSolver解算核心");
  return l_object;
}
/**
 * @brief 创建碰撞体
 * @param in_collider 传入要创建碰撞体的 mobj
 * @return 碰撞体和碰撞偏移物体
 */
std::tuple<MObject, MObject> _add_collider_(const MObject& in_collider) {
  MStatus l_status{};
  /// 创建碰撞体
  auto l_ql_solver = get_ql_solver();
  MSelectionList l_list{};
  l_status = l_list.add(l_ql_solver);
  DOODLE_CHICK(l_status);
  l_status = l_list.add(in_collider);
  DOODLE_CHICK(l_status);
  l_status = MGlobal::setActiveSelectionList(l_list);
  DOODLE_CHICK(l_status);
  MString l_collider_name{};
  l_status = MGlobal::executeCommand("qlCreateCollider;", l_collider_name);
  DOODLE_CHICK(l_status);

  /// 获取创建出来的碰撞体和解算体
  l_status = l_list.clear();
  l_status = l_list.add(l_collider_name);
  MObject l_collider{};
  l_status = l_list.getDependNode(0, l_collider);

  MObject l_collider_offset{};
  set_attribute(l_collider, "offset", 0.03);

  auto l_out_plug = get_plug(l_collider, "output");
  MPlugArray l_plug_array{};
  l_out_plug.destinations(l_plug_array, &l_status);
  DOODLE_CHICK(l_status);
  for (int l_i = 0; l_i < l_plug_array.length(); ++l_i) {
    auto l_node = l_plug_array[l_i].node(&l_status);
    DOODLE_CHICK(l_status);
    if (l_node.hasFn(MFn::kMesh))
      l_collider_offset = l_node;
  }

  chick_true<maya_error>(!l_collider.isNull() && !l_collider_offset.isNull(), DOODLE_LOC, "寻找的的解算网格体和偏移网格体不一致");
  return std::make_tuple(l_collider, l_collider_offset);
}

/**
 * @brief 检查组节点名称
 * @param in_node 传入的节点
 * @param in_name 节点名称
 * @return 不符合名称的话返回空节点, 否则返回传入节点
 */
MObject chick_group(const MFnDagNode& in_node,
                    const std::string& in_name) {
  MStatus l_s{};
  auto l_name = in_node.name(&l_s);
  DOODLE_CHICK(l_s);
  if (l_name.asUTF8() == in_name.c_str()) {
    auto l_obj = in_node.object(&l_s);
    DOODLE_CHICK(l_s);
    return l_obj;
  }
  return {};
}

/**
 * @brief 创建组节点,并根据传入的名称和父对象进行设置
 * @param in_modifier 传入的管理器
 * @param in_name 传入的名称
 * @param in_parent 传入的父物体
 * @return 创建的maya组节点
 */
MObject make_group(MDagModifier& in_modifier,
                   const std::string& in_name,
                   const MObject& in_parent) {
  MStatus l_s{};
  auto l_r = in_modifier.createNode(d_str{"transform"}, in_parent, &l_s);
  DOODLE_CHICK(l_s);
  l_s = in_modifier.renameNode(l_r, d_str{in_name});
  DOODLE_CHICK(l_s);
  l_s = in_modifier.doIt();
  DOODLE_CHICK(l_s);
  return l_r;
}
}  // namespace

qcloth_shape::qcloth_shape() = default;

qcloth_shape::qcloth_shape(const entt::handle& in_ref_file, const MObject& in_object)
    : qcloth_shape() {
  p_ref_file = in_ref_file;
  obj        = in_object;
  chick_true<doodle_error>(p_ref_file.any_of<reference_file>(), DOODLE_LOC, "缺失组件");
}
bool qcloth_shape::set_cache_folder() const {
  return set_cache_folder(FSys::path{});
}

bool qcloth_shape::create_cache() const {
  chick_true<doodle_error>(!obj.isNull(), DOODLE_SOURCE_LOC, "空组件");
  MStatus k_s{};
  MFnDependencyNode l_node{obj, &k_s};
  auto k_plug = get_plug(obj, "outputMesh");
  /// \brief 使用这种方式评估网格
  return !k_plug.asMObject(&k_s).isNull();
}

std::vector<entt::handle> qcloth_shape::create_sim_cloth(const entt::handle& in_handle) {
  chick_true<doodle_error>(
      in_handle.any_of<qcloth_shape_n::maya_obj, qcloth_shape_n::shape_list>(),
      DOODLE_LOC, "缺失组件");

  auto& k_ref = project::get_current().get<project_config::base_config>();
  MAnimControl::setMinTime(MTime{950, MTime::uiUnit()});
  auto l_group = get_cloth_group();

  MStatus k_s{};
  MFnDagNode l_node{};
  MDagModifier l_modifier{};

  /// \brief 主要的动画输出节点(需要输入到解算输入端)
  auto& k_anim_mesh        = in_handle.get<qcloth_shape_n::maya_obj>();
  {
    set_node_name(k_anim_mesh.obj, fmt::format("{}_proxy", k_anim_mesh.p_name));
  }
  /// \brief 主要的输入节点
  auto k_proxy_node_input  = make_low_node(k_anim_mesh.obj, l_group.anim_grp, "input");
  /// \brief 主要的输出节点
  auto k_proxy_node_output = make_low_node(k_anim_mesh.obj, l_group.deform_grp, "output");
  /// \brief 动画高模
  auto& k_maya_high_mesh   = in_handle.get<qcloth_shape_n::shape_list>();
  auto l_high_mesh         = make_high_node(k_maya_high_mesh, l_group.export_grp);

  MDagPath l_path{};

  auto [l_ql, l_mesh_out] = qlCreateCloth(k_proxy_node_input);

  {  /// @brief 设置名称
    set_node_name(get_transform(l_ql), fmt::format("{}_cloth", k_anim_mesh.p_name));
    /// \brief 获取ql 创建布料时的附带创建出现的网格
    set_node_name(get_transform(l_mesh_out), fmt::format("{}_cloth_proxy", k_anim_mesh.p_name));
  }

  auto l_ql_core = get_ql_solver();
  set_attribute(l_ql_core, "frameSamples", 6);
  set_attribute(l_ql_core, "cgAccuracy", 9);
  set_attribute(l_ql_core, "selfCollision", true);
  set_attribute(l_ql_core, "sharpFeature", true);
  {  /// 整理层级关系
    auto l_ql_tran       = get_transform(l_ql);
    auto l_mesh_out_tran = get_transform(l_mesh_out);
    add_child(l_group.solver_grp, l_ql_tran);
    add_child(l_ql_tran, l_mesh_out_tran);
  }
  {  /// 将解算的输出网格连接到代理输出中去
    k_s = l_modifier.connect(get_plug(l_mesh_out, "outMesh"),
                             get_plug(k_proxy_node_output, "inMesh"));
    DOODLE_CHICK(k_s);
    k_s = l_modifier.doIt();
    DOODLE_CHICK(k_s);
  }
  warp_model(k_proxy_node_output, l_high_mesh);
  {
    /// 创建解算网络的输出 这个可以用融合变形(其中先选择主动变形物体, 再选择被变形物体)
    chick_true<maya_error>(l_high_mesh.size() == k_maya_high_mesh.size(), DOODLE_SOURCE_LOC, "节点数量不一致");
    for (int l_i = 0; l_i < l_high_mesh.size(); ++l_i) {
      transfer_dynamic(l_high_mesh[l_i], k_maya_high_mesh[l_i].obj);
    }
  }
  return {};
}

qcloth_shape::cloth_group qcloth_shape::get_cloth_group() {
  MStatus k_s{};
  qcloth_shape::cloth_group k_r{};
  MFnDagNode k_node{};

  auto k_reg = g_reg();
  if (k_reg->ctx().contains<qcloth_shape::cloth_group>()) {
    return k_reg->ctx().at<qcloth_shape::cloth_group>();
  }

  for (MItDag i{MItDag::kDepthFirst, MFn::Type::kTransform, &k_s}; !i.isDone(); i.next()) {
    DOODLE_CHICK(k_s);
    k_s = k_node.setObject(i.currentItem());
    DOODLE_CHICK(k_s);
    k_r.cfx_grp        = chick_group(k_node, "cfx_grp");
    k_r.solver_grp     = chick_group(k_node, "solver_grp");
    k_r.anim_grp       = chick_group(k_node, "anim_grp");
    k_r.constraint_grp = chick_group(k_node, "constraint_grp");
    k_r.collider_grp   = chick_group(k_node, "collider_grp");
    k_r.deform_grp     = chick_group(k_node, "deform_grp");
    k_r.export_grp     = chick_group(k_node, "export_grp");
  }
  MDagModifier k_m{};
  if (k_r.cfx_grp.isNull())
    k_r.cfx_grp = make_group(k_m, "cfx_grp", MObject::kNullObj);

  if (k_r.solver_grp.isNull())
    k_r.solver_grp = make_group(k_m, "solver_grp", k_r.cfx_grp);

  if (k_r.anim_grp.isNull())
    k_r.anim_grp = make_group(k_m, "anim_grp", k_r.cfx_grp);

  if (k_r.constraint_grp.isNull())
    k_r.constraint_grp = make_group(k_m, "constraint_grp", k_r.cfx_grp);

  if (k_r.collider_grp.isNull())
    k_r.collider_grp = make_group(k_m, "collider_grp", k_r.cfx_grp);

  if (k_r.deform_grp.isNull())
    k_r.deform_grp = make_group(k_m, "deform_grp", k_r.cfx_grp);

  if (k_r.export_grp.isNull())
    k_r.export_grp = make_group(k_m, "export_grp", k_r.cfx_grp);

  k_reg->ctx().emplace<qcloth_shape::cloth_group>(k_r);
  return k_r;
}

void qcloth_shape::add_collider(const entt::handle& in_handle) {
  chick_true<component_error>(
      in_handle.any_of<qcloth_shape_n::shape_list>(), DOODLE_LOC, "缺失组件");
  auto l_group   = get_cloth_group();
  auto l_ql      = get_ql_solver();
  auto l_ql_tran = get_transform(l_ql);

  /// \brief 鉴于有些文件会锁定默认着色组, 我们需要进行解锁
  auto k_s       = MGlobal::executeCommand(d_str{R"(lockNode -l false -lu false ":initialShadingGroup";)"});
  DOODLE_CHICK(k_s);
  add_child(l_group.cfx_grp, l_ql_tran);

  for (auto& l_item : in_handle.get<qcloth_shape_n::shape_list>()) {
    auto [l_col, l_col_off] = _add_collider_(l_item.obj);
    auto l_col_tran         = get_transform(l_col);
    auto l_col_off_tran     = get_transform(l_col_off);
    add_child(l_group.collider_grp, l_col_tran);
    add_child(l_group.collider_grp, l_col_off_tran);
  }
}
std::vector<entt::handle> qcloth_shape::create(const entt::handle& in_ref_file) {
  /// 这里我们使用节点类名称寻找 qlClothShape ;
  MStatus k_s{};
  std::vector<entt::handle> result{};
  auto&& l_ref = in_ref_file.get<reference_file>();
  for (MItDependencyNodes i{MFn::Type::kPluginLocatorNode}; !i.isDone(); i.next()) {
    auto k_obj = i.thisNode(&k_s);
    DOODLE_CHICK(k_s);
    MFnDependencyNode k_dep{k_obj};
    if (k_dep.typeName(&k_s) == "qlClothShape" && l_ref.has_node(k_obj)) {
      DOODLE_CHICK(k_s);
      auto k_h = make_handle();
      k_h.emplace<qcloth_shape>(in_ref_file, k_obj);
      result.emplace_back(k_h);
    }
  }
  return result;
}
bool qcloth_shape::set_cache_folder(const FSys::path& in_path) const {
  MStatus k_s{};
  /// \brief 获得解算节点fn
  MFnDependencyNode k_node{obj, &k_s};
  DOODLE_CHICK(k_s);
  std::string k_namespace = p_ref_file.get<reference_file>().get_namespace();

  DOODLE_CHICK(k_s);
  std::string k_node_name = d_str{MNamespace::stripNamespaceFromName(k_node.name(), &k_s)};
  DOODLE_CHICK(k_s);
  {
    auto k_cache = k_node.findPlug(d_str{"cacheFolder"}, false, &k_s);
    DOODLE_CHICK(k_s);
    auto k_file_name    = maya_file_io::get_current_path();
    /// \brief 使用各种信息确认缓存相对路径
    FSys::path l_string = fmt::format("cache/{}/{}/{}",
                                      k_file_name.stem().generic_string(),
                                      k_namespace,
                                      k_node_name);
    l_string /= in_path;
    DOODLE_LOG_INFO("设置缓存路径 {}", l_string);
    /// \brief 删除已经缓存的目录
    auto k_path = maya_file_io::work_path(l_string);
    if (FSys::exists(k_path)) {
      DOODLE_LOG_INFO("发现缓存目录, 主动删除 {}", k_path);
      FSys::remove_all(k_path);
    }
    FSys::create_directories(k_path);
    k_s = k_cache.setString(d_str{l_string.generic_string()});
    DOODLE_CHICK(k_s);
  }
  {
    auto k_cache = k_node.findPlug(d_str{"cacheName"}, true, &k_s);
    DOODLE_CHICK(k_s);
    k_cache.setString(d_str{k_node_name});
  }
  return true;
}

}  // namespace doodle::maya_plug
