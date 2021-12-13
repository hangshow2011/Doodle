//
// Created by TD on 2021/11/22.
//

#include "create_hud_node.h"
#include "maya_plug_fwd.h"

#include <maya/MFnAttribute.h>
#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include "maya_render/hud_render_node.h"
namespace doodle::maya_plug {
create_hud_node::create_hud_node() = default;

bool create_hud_node::hide(bool hide) const {
  MStatus k_s;
  MItDag k_it{MItDag::kBreadthFirst, MFn::kLocator, &k_s};
  CHECK_MSTATUS_AND_RETURN(k_s, false);

  for (; !k_it.isDone(); k_it.next()) {
    MFnDagNode k_node{k_it.currentItem(&k_s)};
    CHECK_MSTATUS_AND_RETURN(k_s, false);
    if (k_node.typeId() != doodle_info_node::doodle_id) {
      MDagPath k_path{};
      k_s = k_node.getPath(k_path);
      CHECK_MSTATUS_AND_RETURN(k_s, false);
      MFnDagNode k_tran{k_path.transform(&k_s)};
      CHECK_MSTATUS_AND_RETURN(k_s, false);
      // MFnAttribute k_vis{k_tran.attribute("visibility")};
      MPlug k_vis{k_tran.object(), k_tran.attribute("visibility")};
      k_s = k_vis.setBool(hide);
      CHECK_MSTATUS_AND_RETURN(k_s, false);
    }
  }
  return true;
}

bool create_hud_node::operator()() const {
  MStatus k_s;
  MItDag k_it{MItDag::kBreadthFirst, MFn::kLocator, &k_s};
  CHECK_MSTATUS_AND_RETURN(k_s, false);

  bool has_node{true};
  for (; !k_it.isDone(); k_it.next()) {
    MFnDagNode k_node{k_it.currentItem(&k_s)};
    CHECK_MSTATUS_AND_RETURN(k_s, false);
    has_node &= (k_node.typeId() != doodle_info_node::doodle_id);
  }

  for (size_t i = 0; i < M3dView::numberOf3dViews(); ++i) {
    M3dView k_v{};
    k_s = M3dView::get3dView(i, k_v);
    CHECK_MSTATUS_AND_RETURN(k_s, false);
    k_v.setObjectDisplay(k_v.objectDisplay(&k_s) | M3dView::kDisplayLocators);
  }
  M3dView::scheduleRefreshAllViews();

  if (has_node) {
    MFnDagNode k_node{};

    auto k_obj = k_node.create(doodle_info_node::doodle_id, d_str{"doodle_hud"});

    DOODLE_LOG_INFO("完成创建节点 doodle_hud");
  } else {
    DOODLE_LOG_WARN("节点(doodle_hud)已经存在， 不需要重复创建")
  }
  return true;
}


MStatus create_hud_node_maya::doIt(const MArgList& in_arg) {
  create_hud_node k_c{};
  return k_c() ? MStatus::kSuccess : MStatus::kFailure;
}
}  // namespace doodle::maya_plug
