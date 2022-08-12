//
// Created by TD on 2022/7/29.
//
#pragma once
#include <main/maya_plug_fwd.h>
namespace doodle {
namespace maya_plug {

namespace sequence_to_blend_shape_ns {
constexpr char name[] = "doodle_sequence_to_blend_shape";
MSyntax syntax();
}  // namespace sequence_to_blend_shape_ns

class sequence_to_blend_shape : public doodle::TemplateAction<
                                    sequence_to_blend_shape,
                                    sequence_to_blend_shape_ns::name,
                                    sequence_to_blend_shape_ns::syntax> {
  class impl;
  std::unique_ptr<impl> p_i;
  void get_arg(const MArgList& in_arg);

  void create_mesh();
  void create_mesh(std::size_t in_index);
  void create_anim();
  void run_blend_shape_comm();

  /**
   * @brief 将网格居中在mesh中心
   * @param in_path
   */
  static void center_pivot(MDagPath& in_path);
  /**
   * @brief 使用传入的矩阵将座标轴重置在传入的点中
   *    @li 会对做标进行归零处理
   *    @li 在试图中体现为归零座标轴, 并将座标轴移动至传入点
   * @param in_path 变换的路径
   * @param in_matrix 变换网格的矩阵
   * @param in_point 传入的点
   */
  static void center_pivot(MDagPath& in_path, const MMatrix& in_matrix, const MPoint& in_point);

  static void to_work_zero(const MDagPath& in_path);

 public:
  sequence_to_blend_shape();
  ~sequence_to_blend_shape() override;

  MStatus doIt(const MArgList& in_arg) override;
  [[maybe_unused]] MStatus undoIt() override;
  [[maybe_unused]] MStatus redoIt() override;
  [[maybe_unused]] [[nodiscard]] bool isUndoable() const override;
};

}  // namespace maya_plug
}  // namespace doodle
