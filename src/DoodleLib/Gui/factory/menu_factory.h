﻿//
// Created by TD on 2021/6/28.
//

#pragma once
#include <DoodleLib/DoodleLib_fwd.h>

#include <nana/gui.hpp>
#include <nana/gui/widgets/menu.hpp>
namespace doodle {
/**
 * @brief 创建各种菜单的辅助类
 * 
 */
class DOODLELIB_API menu_factory_base {
 protected:
  CoreSet& p_set;
  std::vector<action_ptr> p_action;
  nana::window p_window;
  MetadataPtr p_metadata;
  MetadataPtr p_parent;

  friend Project;
  friend Assets;
  friend Episodes;
  friend Shot;
  friend AssetsFile;

  virtual void create_menu(const ProjectPtr& in_ptr)    = 0;
  virtual void create_menu(const AssetsPtr& in_ptr)     = 0;
  virtual void create_menu(const EpisodesPtr& in_ptr)   = 0;
  virtual void create_menu(const ShotPtr& in_ptr)       = 0;
  virtual void create_menu(const AssetsFilePtr& in_ptr) = 0;

 public:
  explicit menu_factory_base(nana::window in_window);
  /**
   * @brief 设置创建各种内容所需要的数据
   * @warning 这里要注意先添加数据， 然后再开始创建菜单
   * @param in_ptr 选中的内容
   * @param in_parent 添加数据的父指针（只有在创建项目时这个可以是空）
   */
  void set_metadate(const MetadataPtr& in_ptr, const MetadataPtr& in_parent = {});
  /**
   * @brief 将创建的动作转换为菜单的便捷函数
   * 
   * @param in_menu 
   */
  void operator()(nana::menu& in_menu);
};
/**
 * @brief 创建右键菜单
 * 
 */
class DOODLELIB_API menu_factory : public menu_factory_base {
 public:
  explicit menu_factory(nana::window in_window);

  virtual void create_prj();

  virtual void create_menu(const ProjectPtr& in_ptr) override;
  virtual void create_menu(const AssetsPtr& in_ptr) override;
  virtual void create_menu(const EpisodesPtr& in_ptr) override;
  virtual void create_menu(const ShotPtr& in_ptr) override;
  virtual void create_menu(const AssetsFilePtr& in_ptr) override;

 protected:
  void create_delete_assets();
  void create_assets();
  void creare_episodes();
  void create_shot();
  void create_assets_file();
};

/**
 * @brief 拖拽文件时所产生的菜单
 * 
 */
class DOODLELIB_API dragdrop_menu_factory : public menu_factory_base {
  std::vector<FSys::path> p_paths;

 public:
  void create_menu(const ProjectPtr& in_ptr) override;
  void create_menu(const AssetsPtr& in_ptr) override;
  void create_menu(const EpisodesPtr& in_ptr) override;
  void create_menu(const ShotPtr& in_ptr) override;
  void create_menu(const AssetsFilePtr& in_ptr) override;
 
  explicit dragdrop_menu_factory(nana::window in_window)
      : menu_factory_base(in_window),
        p_paths(){};

  /**
   * @brief 传入的拖拽文件的列表
   * 
   * @param in_path 
   */
  void set_drop_file(const std::vector<FSys::path>& in_path);
  void drop_menu();
};
}  // namespace doodle
