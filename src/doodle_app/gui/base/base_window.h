//
// Created by TD on 2022/4/8.
//
#pragma once
#include <doodle_core/core/init_register.h>
#include <doodle_core/gui_template/show_windows.h>

#include <doodle_app/doodle_app_fwd.h>
#include <doodle_app/lib_warp/imgui_warp.h>

#include <boost/signals2.hpp>

#include <utility>
namespace doodle::facet {
class gui_facet;
}
namespace doodle::gui {

class windows_manage {
  std::weak_ptr<facet::gui_facet> gui_facet;

 public:
  explicit windows_manage(const std::weak_ptr<facet::gui_facet>& in_gui_facet) : gui_facet(in_gui_facet){};
  void create_windows_(windows&& in_windows);
  template <typename T, typename... Arg>
  T* create_windows(Arg&&... in_arg) {
    windows l_install{std::in_place_type<T>, std::forward<Arg&&>(in_arg)...};
    auto l_t = static_cast<T*>(l_install.data());
    create_windows_(std::move(l_install));
    return l_t;
  };
};
windows_manage& g_windows_manage();

}  // namespace doodle::gui
