//
// Created by TD on 2022/1/14.
//

#include "main_status_bar.h"
#include <doodle_lib/lib_warp/imgui_warp.h>
#include <doodle_lib/thread_pool/long_term.h>
#include <doodle_lib/lib_warp/imgui_warp.h>

/// \brief to https://github.com/ocornut/imgui/issues/1901
namespace ImGui {
bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
  ImGuiWindow* window = GetCurrentWindow();
  if (window->SkipItems)
    return false;

  ImGuiContext& g         = *GImGui;
  const ImGuiStyle& style = g.Style;
  const ImGuiID id        = window->GetID(label);

  ImVec2 pos              = window->DC.CursorPos;
  ImVec2 size             = size_arg;
  size.x -= style.FramePadding.x * 2;

  const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
  ItemSize(bb, style.FramePadding.y);
  if (!ItemAdd(bb, id))
    return false;

  // Render
  const float circleStart = size.x * 0.7f;
  const float circleEnd   = size.x;
  const float circleWidth = circleEnd - circleStart;

  window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
  window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

  const float t     = g.Time;
  const float r     = size.y / 2;
  const float speed = 1.5f;

  const float a     = speed * 0;
  const float b     = speed * 0.333f;
  const float c     = speed * 0.666f;

  const float o1    = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
  const float o2    = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
  const float o3    = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

  window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
  window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
  window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
  return false;
}

bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
  ImGuiWindow* window = GetCurrentWindow();
  if (window->SkipItems)
    return false;

  ImGuiContext& g         = *GImGui;
  const ImGuiStyle& style = g.Style;
  const ImGuiID id        = window->GetID(label);

  ImVec2 pos              = window->DC.CursorPos;
  ImVec2 size((radius)*2, (radius + style.FramePadding.y) * 2);

  const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
  ItemSize(bb, style.FramePadding.y);
  if (!ItemAdd(bb, id))
    return false;

  // Render
  window->DrawList->PathClear();

  int num_segments    = 30;
  int start           = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

  const float a_min   = IM_PI * 2.0f * ((float)start) / (float)num_segments;
  const float a_max   = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

  const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

  for (int i = 0; i < num_segments; i++) {
    const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
    window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
                                        centre.y + ImSin(a + g.Time * 8) * radius));
  }

  window->DrawList->PathStroke(color, false, thickness);
  return false;
}
}  // namespace ImGui

namespace doodle {
class main_status_bar::impl {
 public:
};

main_status_bar::main_status_bar()
    : p_i(std::make_unique<impl>()) {}

void main_status_bar::init() {
  g_reg()->set<main_status_bar&>(*this);
}
void main_status_bar::succeeded() {
}
void main_status_bar::failed() {
}
void main_status_bar::aborted() {
}
void main_status_bar::update(
    chrono::duration<chrono::system_clock::rep,
                     chrono::system_clock::period>,
    void* data) {
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
  float height                  = ImGui::GetFrameHeight();
  dear::ViewportSideBar{"状态栏_main", nullptr, ImGuiDir_Down, height, window_flags} && [&]() {
    dear::MenuBar{} && [&]() {
      if (auto l_msg = g_reg()->try_ctx<process_message>(); l_msg) {
        dear::ProgressBar(boost::rational_cast<std::float_t>(l_msg->get_progress()),
                          ImVec2{-FLT_MIN, 0.0f},
                          l_msg->get_name().c_str());
        //        const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
        //        const ImU32 bg  = ImGui::GetColorU32(ImGuiCol_Button);
        //        imgui::BufferingBar("long_time",
        //                            boost::rational_cast<std::float_t>(l_msg->get_progress()),
        //                            ImVec2(600, 5),
        //                            bg,
        //                            col);
        if (l_msg->is_success())
          g_reg()->unset<process_message>();
      }
    };
  };
}
main_status_bar::~main_status_bar() = default;

}  // namespace doodle
