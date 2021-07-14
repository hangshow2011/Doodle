﻿//
// Created by TD on 2021/6/28.
//

#include "menu_factory.h"

#include <Gui/action/action_import.h>
#include <Gui/progress.h>
#include <Metadata/AssetsFile.h>
#include <Metadata/Metadata_cpp.h>
#include <threadPool/long_term.h>

#include <nana/gui/filebox.hpp>

namespace doodle {

menu_factory_base::menu_factory_base(nana::window in_window)
    : p_window(in_window),
      p_set(CoreSet::getSet()),
      p_action(),
      p_metadata(),
      p_parent() {
}

void menu_factory_base::set_metadate(const MetadataPtr& in_ptr, const MetadataPtr& in_parent) {
  p_metadata = in_ptr;
  p_parent   = in_parent;
}

void menu_factory_base::operator()(nana::menu& in_menu) {
  auto k_f = shared_from_this();
  for (auto& k_i : p_action) {
    if (k_i)
      in_menu.append(
          k_i->class_name(),
          [k_i, k_f](const nana::menu::item_proxy&) {
            try {
              (*k_i)(k_f->p_metadata, k_f->p_parent);
              if (k_i->is_async()) {
                auto k_long = k_i->get_long_term_signal();
                if (k_long)
                  progress::create_progress(k_f->p_window, k_long, "结果");
              }
            } catch (DoodleError& error) {
              DOODLE_LOG_WARN(error.what())
              nana::msgbox k_msgbox{k_f->p_window, error.what()};
              k_msgbox.show();
            }
          });
    else
      in_menu.append_splitter();
  }
}

menu_factory::menu_factory(nana::window in_window)
    : menu_factory_base(in_window) {
}

void menu_factory::create_menu(const ProjectPtr& in_ptr) {
  if (p_metadata) {  /// 首先测试是否选中prj 这种情况下显示所有
    create_prj();
    //----------------------------------------------------------------

    p_action.emplace_back(action_ptr{});

    auto k_rm = std::make_shared<actn_rename_project>();
    p_action.push_back(k_rm);
    auto k_rp = std::make_shared<actn_setpath_project>();
    p_action.emplace_back(k_rp);

    k_rm->sig_get_arg.connect([this]() {
      nana::inputbox mess{p_window, "项目名称: "};
      nana::inputbox::text name{"项目名称: "};
      mess.show_modal(name);
      auto str = name.value();

      actn_rename_project::arg k_arg{};
      k_arg.date = str;
      return k_arg;
    });

    k_rp->sig_get_arg.connect([this]() {
      nana::folderbox mes{
          p_window};
      mes.allow_multi_select(false).title("选择项目根目录: ");

      auto k_e = mes();
      actn_setpath_project::arg k_arg{};
      k_arg.date = k_e.at(0);
      return k_arg;
    });

    if (!p_metadata->hasChild()) {
      p_action.emplace_back(action_ptr{});
      p_action.emplace_back(std::make_shared<actn_delete_project>());
    }

  } else if (p_parent) {  /// 没有选中pej 如果有传入父指针, 则可以认为时ass tree 调用, 显示各种创建选项
    create_assets();
    creare_episodes();
    create_shot();
  } else {  /// 都没有  只有创建物体这一个选择了
    create_prj();
  }
}

void menu_factory::create_menu(const AssetsPtr& in_ptr) {
  if (p_metadata && p_parent) {
    create_assets();
    auto k_s_ass = std::make_shared<actn_assets_setname>();
    p_action.push_back(k_s_ass);
    k_s_ass->sig_get_arg.connect([this]() {
      nana::inputbox msg{p_window, "创建: "};
      nana::inputbox::text name{"名称: "};
      msg.show_modal(name);
      return actn_assets_setname::arg{name.value()};
    });
    create_delete_assets();
  } else if (p_parent) {
    create_assets_file();
  }
}

void menu_factory::create_menu(const EpisodesPtr& in_ptr) {
  if (p_metadata && p_parent) {
    creare_episodes();
    create_shot();
    create_assets();

    auto k_s_eps = std::make_shared<actn_episode_set>();
    p_action.push_back(k_s_eps);
    k_s_eps->sig_get_arg.connect([this]() {
      nana::inputbox msg{p_window, "修改: "};
      nana::inputbox::integer k_i{"集数: ", 0, 0, 9999, 1};
      msg.show_modal(k_i);
      return actn_episode_set::arg{k_i.value()};
    });
    create_delete_assets();
  } else if (p_parent) {
    create_assets_file();
  }
}
void menu_factory::create_menu(const ShotPtr& in_ptr) {
  if (p_metadata && p_parent) {
    create_shot();
    create_assets();

    auto k_s_sh = std::make_shared<actn_shot_set>();
    p_action.push_back(k_s_sh);
    auto k_s_shab = std::make_shared<actn_shotab_set>();
    p_action.push_back(k_s_shab);

    k_s_sh->sig_get_arg.connect([this]() {
      nana::inputbox msg{p_window, "修改: "};
      nana::inputbox::integer k_i{"镜头号: ", 0, 0, 9999, 1};
      msg.show_modal(k_i);
      return actn_shot_set::arg{k_i.value()};
    });
    k_s_shab->sig_get_arg.connect([this]() {
      nana::inputbox msg{p_window, "修改: "};
      auto k_s_v = magic_enum::enum_names<Shot::ShotAbEnum>();
      nana::inputbox::text k_text{"ab镜头号:", std::vector<std::string>{k_s_v.begin(), k_s_v.end()}};
      msg.show_modal(k_text);
      return actn_shotab_set::arg{k_text.value()};
    });
    create_delete_assets();
  } else if (p_parent) {
    create_assets_file();
  }
}

void menu_factory::create_menu(const AssetsFilePtr& in_ptr) {
  if (p_metadata && p_parent) {
    create_assets_file();

    p_action.emplace_back(action_ptr{});

    auto k_s_c = std::make_shared<actn_assfile_add_com>();
    p_action.push_back(k_s_c);
    auto k_s_t = std::make_shared<actn_assfile_datetime>();
    p_action.push_back(k_s_t);


    k_s_c->sig_get_arg.connect([this]() {
      nana::inputbox msg{p_window, "评论: "};
      nana::inputbox::text name{"评论: "};
      msg.show_modal(name);
      return actn_assfile_add_com::arg{name.value()};
    });
    k_s_t->sig_get_arg.connect([this]() {
      nana::inputbox msg{p_window, "修改: "};
      //    nana::inputbox::date k_date{"时间: "};
      auto time = std::dynamic_pointer_cast<AssetsFile>(p_metadata)->getTime();
      nana::inputbox::integer k_year{"年", time->get_year(), 1999, 2999, 1};
      nana::inputbox::integer k_month{"月", time->get_month(), 1, 12, 1};
      nana::inputbox::integer k_day{"天", time->get_day(), 1, 31, 1};
      nana::inputbox::integer k_hours{"时", time->get_hour(), 0, 23, 1};
      nana::inputbox::integer k_minutes{"分", time->get_minutes(), 0, 59, 1};
      nana::inputbox::integer k_seconds{"秒", time->get_second(), 0, 59, 1};

      msg.show_modal(k_year, k_month, k_day, k_hours, k_minutes, k_seconds);

      time->set_year(k_year.value());
      time->set_month(k_month.value());
      time->set_day(k_day.value());
      time->set_hour(k_hours.value());
      time->set_minutes(k_minutes.value());
      time->set_second(k_seconds.value());
      return actn_assfile_datetime::arg{time};
    });
    if(!in_ptr->getPathFile().empty()) {
      auto k_down = std::make_shared<actn_down_paths>();
      p_action.push_back(k_down);
      k_down->sig_get_arg.connect([this]() {
        nana::folderbox k_folderbox{p_window, FSys::current_path()};
        k_folderbox.allow_multi_select(false);
        auto k_paths = k_folderbox();
        actn_down_paths::arg_ arg{k_paths.front()};
        if (k_paths.empty())
          arg.is_cancel = true;
        return arg;
      });
    }

    p_action.emplace_back(std::make_shared<actn_assfile_delete>());

  } else if (p_parent) {
    create_assets_file();
  }
}

void menu_factory::create_assets() {
  auto k_c_ass = std::make_shared<actn_assets_create>();
  p_action.push_back(k_c_ass);

  k_c_ass->sig_get_arg.connect([this]() {
    nana::inputbox msg{p_window, "创建: "};
    nana::inputbox::text name{"名称: "};
    msg.show_modal(name);
    return actn_assets_create::arg{name.value()};
  });
}

void menu_factory::creare_episodes() {
  auto k_c_eps = std::make_shared<actn_episode_create>();
  p_action.push_back(k_c_eps);
  k_c_eps->sig_get_arg.connect([this]() {
    nana::inputbox msg{p_window, "创建: "};
    nana::inputbox::integer k_i{"集数: ", 0, 0, 9999, 1};
    msg.show_modal(k_i);
    return actn_episode_create::arg{k_i.value()};
  });
}

void menu_factory::create_shot() {
  auto k_c_shot = std::make_shared<actn_shot_create>();
  p_action.push_back(k_c_shot);

  k_c_shot->sig_get_arg.connect([this]() {
    nana::inputbox msg{p_window, "创建: "};
    nana::inputbox::integer k_i{"镜头号: ", 0, 0, 9999, 1};
    msg.show_modal(k_i);
    return actn_shot_create::arg{k_i.value()};
  });
}

void menu_factory::create_assets_file() {
  auto k_c_f = std::make_shared<actn_assfile_create>();
  p_action.push_back(k_c_f);
  k_c_f->sig_get_arg.connect([this]() {
    return actn_assfile_create::arg{this->p_parent->showStr()};
  });
}

void menu_factory::create_prj() {
  auto k_create = std::make_shared<actn_create_project>();
  p_action.push_back(k_create);

  k_create->sig_get_arg.connect([this]() {
    nana::folderbox mes{
        p_window, FSys::current_path()};
    mes.allow_multi_select(false).title("选择项目根目录: ");

    auto k_e = mes();
    if (k_e.empty())
      return actn_create_project::arg{};

    nana::inputbox mess{p_window, "项目名称: "};

    nana::inputbox::text name{"项目名称: "};
    mess.show_modal(name);
    auto str = name.value();
    if (str.empty())
      return actn_create_project::arg{};
    actn_create_project::arg k_arg{};
    k_arg.prj_path = k_e.at(0);
    k_arg.name     = str;

    return k_arg;
  });
}
void menu_factory::create_delete_assets() {
  if (!p_metadata)
    return;

  if (!p_metadata->hasChild()) {
    p_action.emplace_back(action_ptr{});
    auto k_d_ = p_action.emplace_back(std::make_shared<actn_assets_delete>());
  }
}
void dragdrop_menu_factory::create_menu(const ProjectPtr& in_ptr) {
}
void dragdrop_menu_factory::create_menu(const AssetsPtr& in_ptr) {
  drop_menu();
}
void dragdrop_menu_factory::create_menu(const EpisodesPtr& in_ptr) {
  drop_menu();
}
void dragdrop_menu_factory::create_menu(const ShotPtr& in_ptr) {
  drop_menu();
}
void dragdrop_menu_factory::create_menu(const AssetsFilePtr& in_ptr) {
  drop_menu();
}
void dragdrop_menu_factory::drop_menu() {
  if (p_paths.empty())
    return;

  //有任何一个路径不存在之际返回
  for (const auto& k_p : p_paths) {
    if (!FSys::exists(k_p))
      return;
  }

  auto k_up_folder = std::make_shared<actn_up_paths>();
  p_action.push_back(k_up_folder);
  k_up_folder->sig_get_arg.connect([this]() {
    return actn_up_paths::arg_{p_paths};
  });

  if (p_paths.size() == 1) {
    auto k_path = p_paths.front();

    if (FSys::is_directory(k_path)) {  ///这是一个目录 如果可以转换为视频的话直接转换为视频
      auto k_image = std::make_shared<actn_image_to_movie>();
      actn_image_to_movie::arg_ k_arg{};
      k_arg.image_list = {k_path};
      if (k_image->is_accept(k_arg)) {
        /// 直接转换动画但是并不上传
        k_image->sig_get_arg.connect([this]() {
          actn_image_to_movie::arg_ k_arg{};
          k_arg.image_list = p_paths;
          k_arg.out_file   = CoreSet::getSet().getCacheRoot("imaeg_to_move");
          return k_arg;
        });

        p_action.push_back(k_image);

        create_image_and_up();
      }

    } else if (FSys::is_regular_file(k_path)) {
    } else {
      return;
    }

  } else {
  }
  p_action.emplace_back(action_ptr{});
  p_action.emplace_back(std::make_shared<actn_null>());
}
void dragdrop_menu_factory::create_image_and_up() {
  auto k_i_anf_up = std::make_shared<actn_image_to_move_up>();
  k_i_anf_up->sig_get_arg.connect([this]() {
    actn_image_to_move_up::arg_ k_arg{};
    k_arg.image_list = p_paths;
    k_arg.out_file   = CoreSet::getSet().getCacheRoot("imaeg_to_move");
    return k_arg;
  });

  p_action.push_back(k_i_anf_up);
}
void dragdrop_menu_factory::set_drop_file(const std::vector<FSys::path>& in_path) {
  p_paths = in_path;
}

}  // namespace doodle
