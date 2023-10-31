#pragma once

#include <doodle_core/core/init_register.h>

#include <doodle_app/gui/base/base_window.h>

#include <doodle_lib/doodle_lib_fwd.h>
namespace doodle::gui {

class DOODLELIB_API zh_model_upload {
  class impl;
  class image_arg;
  std::unique_ptr<impl> p_i;

  entt::handle create_image_to_move_handle(const FSys::path& in_path);

 public:
  zh_model_upload();
  ~zh_model_upload();
  constexpr static std::string_view name{gui::config::menu_w::zh_model_upload};
  const std::string& title() const;
  bool render();
};
}  // namespace doodle::gui