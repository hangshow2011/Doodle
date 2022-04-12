//
// Created by TD on 2021/9/19.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>
namespace doodle {

/**
 * @brief maya工具类
 * @image html comm_maya_tool.jpg maya工具窗口
 *
 * @warning 在导出过程中关闭软件或者强制结束任务会产生不可预置的问题
 *
 * @li @b 总项 \n
 * maya文件选择时，对于解算和导出来说是公用的路径，
 *
 * @li @b 解算子项 \n
 * 解算时有解算资源所在路径选项的要求， 这个是必须填写的项目，
 * 在使用只解算时是不会替换解算引用的
 * @warning
 *  * 需要 有 UE4 后缀的组, 使用这个组来输出abc文件
 *  * 需要 解算从950开始
 *  * 需要帧率时25fps
 *  * 最好有一台摄像机用来创建拍屏
 *  * 最好具有标记元数据, 使用这个元数据来确认解算的人物
 *
 * @li @b 导出fbx子项 \n
 * 导出fbx时是按照保存是引用保存时的状态加载的，
 * 如果勾选直接加载所有引用的话， 会不查看保存状态， 直接加载所有引用
 *
 *
 */
class DOODLELIB_API comm_maya_tool : public process_t<comm_maya_tool> {
  FSys::path p_cloth_path;
  std::string p_text;
  std::vector<FSys::path> p_sim_path;
  bool p_only_sim;
  bool p_use_all_ref;

 public:
  comm_maya_tool();
  bool show{true};
  constexpr static std::string_view name{"maya工具"};

  [[maybe_unused]] void init();
  [[maybe_unused]] void succeeded();
  [[maybe_unused]] void failed();
  [[maybe_unused]] void aborted();
  [[maybe_unused]] void update(delta_type, void* data);
  void render();
};

/**
 * @brief 创建视频工具类
 * @image html comm_create_video.jpg 创建视频工具
 *
 * @li @b 输出文件夹 \n
 * 输出文件夹指定了输出路径,这个是必填项目
 *
 * @li @b 图片序列 :\n
 * 这个可以 @b 选择图片 序列和 @b 选择文件夹， \n
 * @note 这里文件夹中的图片将全部载入合成视频 \n
 * 同时可以多次选中添加到合成队列中， 并且一起合成 \n
 * 需要清除时， 点击清除按钮 \n
 *
 * @li @b 清除:
 * 清除整个列表
 * @li @b 创建视频:
 * 直接将整个队列中的文件连接为多个视频
 *
 * @li @b 视频序列 \n
 * 选择视频, 后点击连接视频进行连接
 *
 */
class DOODLELIB_API comm_create_video : public process_t<comm_create_video> {
  class impl;
  class image_arg;
  std::unique_ptr<impl> p_i;

  entt::handle create_image_to_move_handle(const FSys::path& in_path);

 public:
  comm_create_video();
  ~comm_create_video() override;
  constexpr static std::string_view name{"创建视频"};
  bool show{false};
  [[maybe_unused]] void init();
  [[maybe_unused]] void succeeded();
  [[maybe_unused]] void failed();
  [[maybe_unused]] void aborted();
  [[maybe_unused]] void update(delta_type, void* data);
  void render();
};


}  // namespace doodle
