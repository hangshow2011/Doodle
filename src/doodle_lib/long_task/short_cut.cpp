//
// Created by TD on 2022/2/10.
//

#include "short_cut.h"
#include <imgui.h>
#include <core/core_sig.h>
#include <core/init_register.h>
#include <long_task/process_pool.h>

namespace doodle {
short_cut::short_cut() = default;
void short_cut::init() {
}
void short_cut::succeeded() {
}
void short_cut::failed() {
}
void short_cut::aborted() {
}
void short_cut::update(const std::chrono::duration<std::chrono::system_clock::rep,
                                                   std::chrono::system_clock::period> &,
                       void *data) {
  if (ImGui::IsKeyPressed(ImGuiKey_S) && ImGui::GetIO().KeyCtrl)
    g_reg()->ctx<core_sig>().save();
}

class [[maybe_unused]] init_short_cut
    : public init_register::registrar<init_short_cut> {
 public:
  constexpr static const std::int32_t priority{1};
  init_short_cut(){};
  void operator()() const {
    g_main_loop().attach<short_cut>();
  }
};
}  // namespace doodle
