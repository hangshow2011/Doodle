//
// Created by TD on 2022/8/4.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>

namespace doodle {
namespace business {
class rules;
}

namespace gui {
namespace time_sequencer_widget_ns {

class DOODLELIB_API time_rules_render {
 public:
  using rules_type = ::doodle::business::rules;

 private:
  class impl;
  std::unique_ptr<impl> p_i;

 public:
  time_rules_render();
  virtual ~time_rules_render();

  const rules_type& rules_attr() const;
  void rules_attr(const rules_type& in_rules_type);
  bool render();
};

}  // namespace time_sequencer_widget_ns
}  // namespace gui
}  // namespace doodle
