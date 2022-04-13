//
// Created by TD on 2022/1/14.
//

#pragma once
#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/gui/gui_ref/base_window.h>
namespace doodle {
namespace main_menu_bar_ns {
class layout_data {
 private:
  friend void to_json(nlohmann::json& j, const layout_data& p);
  friend void from_json(const nlohmann::json& j, layout_data& p);

 public:
  std::map<std::string, bool> windows_show;
  std::string imgui_data;
  std::string name;
  bool operator==(const layout_data& in_rhs) const;
  bool operator!=(const layout_data& in_rhs) const;
  bool operator==(const std::string& in_rhs) const;
  bool operator!=(const std::string& in_rhs) const;
};

}  // namespace main_menu_bar_ns
class DOODLELIB_API main_menu_bar
    : public gui::base_window,
      public process_t<main_menu_bar> {
 private:
  class impl;
  std::unique_ptr<impl> p_i;
  friend void to_json(nlohmann::json& j, const main_menu_bar& p);
  friend void from_json(const nlohmann::json& j, main_menu_bar& p);

 protected:
  void open_by_name_widget(const std::string_view& in_view, bool force_show = true);
  void widget_menu_item(const std::string_view& in_view);
  virtual void menu_file();
  virtual void menu_windows();
  virtual void menu_edit();
  virtual void menu_tool();
  void menu_layout();
  void layout_save() const;
  void layout_load(const main_menu_bar_ns::layout_data& in_data);
  void layout_delete(const std::string& in_name);

 public:
  main_menu_bar();
  ~main_menu_bar() override;

  const string& title() const override;
  void init() override;
  void succeeded() override;
  virtual void aborted() override;
  void update(
      const chrono::system_clock::duration& in_duration,
      void* in_data) override;
};
}  // namespace doodle
