//
// Created by TD on 2022/9/30.
//

#include "gui_facet.h"

#include "doodle_core/exception/exception.h"
#include <doodle_core/core/app_base.h>
#include <doodle_core/core/core_sig.h>
#include <doodle_core/core/file_sys.h>
#include <doodle_core/core/program_info.h>
#include <doodle_core/core/util.h>
#include <doodle_core/database_task/sqlite_client.h>
#include <doodle_core/gui_template/show_windows.h>

#include <doodle_app/gui/base/base_window.h>
#include <doodle_app/gui/get_input_dialog.h>
#include <doodle_app/lib_warp/imgui_warp.h>
#include <doodle_app/platform/win/windows_proc.h>

#include <doodle_lib/long_task/image_to_move.h>

#include "boost/filesystem/path.hpp"
#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/locale.hpp>

#include "platform/win/drop_manager.h"
#include <gui/get_input_dialog.h>
#include <gui/main_menu_bar.h>
#include <gui/main_proc_handle.h>
#include <gui/main_status_bar.h>
#include <imgui_freetype.h>
#include <implot.h>
#include <implot_internal.h>

// Helper functions
#include <d3d11.h>
#include <memory>
#include <optional>
#include <tchar.h>
// 启用窗口拖拽导入头文件
#include <imgui.h>
#include <shellapi.h>
#include <vector>
#include <wil/com.h>

namespace doodle::facet {
class gui_facet::impl {
 public:
  using timer_t   = boost::asio::high_resolution_timer;
  using timer_ptr = std::shared_ptr<timer_t>;
  /// \brief 初始化 com
  [[maybe_unused]] win::ole_guard _guard{};
  win::d3d_device_ptr d3d_attr;
  std::string name_attr{"gui_windows"};
  timer_ptr timer_{};
  //  using drop_ptr_type = wil::com_ptr_t<win::drop_manager>;
  using drop_ptr_type = decltype(winrt::make_self<win::drop_manager>());

  std::shared_ptr<win::drop_manager_guard> drop_manager_guard_ptr{};
  drop_ptr_type dorp_manager{};
};

const std::string& gui_facet::name() const noexcept { return p_i->name_attr; }

bool gui_facet::post() {
  if (!g_ctx().get<program_info>().use_gui_attr()) return false;

  p_i->timer_ = std::make_shared<impl::timer_t>(g_io_context());

  init_windows();
  windows_manage_ = &g_ctx().emplace<gui::windows_manage>(this);
  static std::function<void(const boost::system::error_code& in_code)> s_fun{};
  s_fun = [&](const boost::system::error_code& in_code) {
    if (in_code == boost::asio::error::operation_aborted) {
      DOODLE_LOG_INFO(in_code.message());
      return;
    }
    if (g_ctx().get<program_info>().stop_attr()) return;
    if (!this->translate_message()) return;
    this->tick();  /// 渲染
    if (!g_ctx().get<program_info>().stop_attr()) {
      p_i->timer_->expires_after(doodle::chrono::seconds{1} / 60);
      p_i->timer_->async_wait(boost::asio::bind_cancellation_slot(app_base::Get().on_cancel.slot(), s_fun));
    }
  };

  p_i->timer_->expires_after(doodle::chrono::seconds{1} / 60);
  p_i->timer_->async_wait(boost::asio::bind_cancellation_slot(app_base::Get().on_cancel.slot(), s_fun));
  return true;
}
void gui_facet::deconstruction() {
  if (p_hwnd) {
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    g_reg()->ctx().get<std::shared_ptr<win::d3d_device>>().reset();

    p_i->drop_manager_guard_ptr.reset();
    ::DestroyWindow(p_hwnd);
    ::UnregisterClassW(p_win_class.lpszClassName, p_win_class.hInstance);
  }
}
gui_facet::gui_facet() : p_i(std::make_unique<impl>()) {
  g_ctx().emplace<gui::main_proc_handle>();
  g_ctx().emplace<identifier>();

  g_reg()->ctx().emplace<image_to_move>();
}

void gui_facet::tick() {
  // Start the Dear ImGui frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  windows_manage_->tick();
  // Rendering
  ImGui::Render();
  static ImVec4 clear_color             = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
  const float clear_color_with_alpha[4] = {
      clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w};

  p_i->d3d_attr->g_pd3dDeviceContext->OMSetRenderTargets(1, &p_i->d3d_attr->g_mainRenderTargetView, nullptr);
  p_i->d3d_attr->g_pd3dDeviceContext->ClearRenderTargetView(
      p_i->d3d_attr->g_mainRenderTargetView, clear_color_with_alpha
  );
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  p_i->d3d_attr->g_pSwapChain->Present(1, 0);  // Present with vsync
                                               // g_pSwapChain->Present(0, 0); // Present without vsync
}
bool gui_facet::translate_message() {
  MSG msg;
  while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
    /// 如果时退出消息, 直接设置停止
    if (msg.message == WM_QUIT) {
      DOODLE_LOG_INFO("开始退出");
      return false;
    }
  }
  return true;
}

void gui_facet::init_windows() {
  auto l_instance     = g_ctx().get<program_info>().handle_attr();

  p_win_class         = {sizeof(WNDCLASSEX), CS_CLASSDC, win::WndProc, 0L,      sizeof(std::nullptr_t),
                         l_instance,         nullptr,    nullptr,      nullptr, nullptr,
                         _T("doodle"),       nullptr};
  p_win_class.hIconSm = (HICON)LoadImageW(l_instance, MAKEINTRESOURCEW(1), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
  p_win_class.hIcon   = p_win_class.hIconSm;

  // Create application window
  // ImGui_ImplWin32_EnableDpiAwareness();
  ::RegisterClassExW(&p_win_class);
  auto l_str = boost::locale::conv::utf_to_utf<wchar_t>(g_ctx().get<program_info>().title_attr());
  p_hwnd     = ::CreateWindowExW(
      0L, p_win_class.lpszClassName, l_str.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, g_ctx().get<program_info>().parent_windows_attr(), nullptr, p_win_class.hInstance, this
  );

  // Initialize Direct3D
  p_i->d3d_attr = g_reg()->ctx().emplace<std::shared_ptr<win::d3d_device>>(std::make_shared<win::d3d_device>(p_hwnd));

  // Show the window
  ::ShowWindow(p_hwnd, SW_SHOWDEFAULT);
  ::UpdateWindow(p_hwnd);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows
  // io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;
  // io.ConfigViewportsNoDefaultParent = true;
  // io.ConfigDockingAlwaysTabBar = true;
  // io.ConfigDockingTransparentPayload = true;
  // Experimental.

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding              = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplWin32_Init(p_hwnd);
  ImGui_ImplDX11_Init(p_i->d3d_attr->g_pd3dDevice, p_i->d3d_attr->g_pd3dDeviceContext);

  /// 启用文件拖拽
  DragAcceptFiles(p_hwnd, 1);
  /// \brief 注册拖放对象
  p_i->dorp_manager           = winrt::make_self<win::drop_manager>();
  p_i->drop_manager_guard_ptr = std::make_shared<win::drop_manager_guard>(p_i->dorp_manager, p_hwnd);

  //  HMONITOR hmon  = MonitorFromWindow(p_impl->p_hwnd,
  //                                     MONITOR_DEFAULTTONEAREST);
  //  MONITORINFO mi = {sizeof(mi)};
  //  auto k_r       = GetMonitorInfo(hmon, &mi);
  //  DOODLE_CHICK(k_r != 0,doodle_error{ "无法设置全屏"});
  //  SetWindowPos(p_impl->p_hwnd, nullptr, mi.rcMonitor.left,
  //               mi.rcMonitor.top,
  //               mi.rcMonitor.right - mi.rcMonitor.left,
  //               mi.rcMonitor.bottom - mi.rcMonitor.top,
  //               SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

  auto imgui_file_path        = core_set::get_set().get_cache_root("imgui") / "imgui.ini";
  static std::string _l_p{imgui_file_path.generic_string()};
  io.IniFilename = _l_p.c_str();

  {
    // io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF(
        doodle_config::font_default.data(), 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull()
    );
    // 加载emj字体
    static ImWchar ranges[] = {0x1, 0x1FFFF, 0};
    static ImFontConfig cfg;
    cfg.OversampleH = cfg.OversampleV = 1;
    cfg.MergeMode                     = true;
    cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguiemj.ttf", 16.0f, &cfg, ranges);
    //    auto l_font                         =
    //    cmrc::DoodleLibResourceFont::get_filesystem().open(FONT_ICON_FILE_NAME_FAS); static const ImWchar
    //    icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0}; ImFontConfig icons_config; icons_config.MergeMode            =
    //    true; icons_config.PixelSnapH           = true; icons_config.FontDataOwnedByAtlas = false;
    //    // io.Fonts->AddFontFromFileTTF(FONT_ICON_F
    //    // ILE_NAME_FAS, 16.0f, &icons_config, icons_ranges);
    //    io.Fonts->AddFontFromMemoryTTF(
    //        (void*)l_font.begin(), boost::numeric_cast<std::int32_t>(l_font.size()), 16.0f, &icons_config,
    //        icons_ranges
    //    );
  }

  DOODLE_CHICK(::IsWindowUnicode(p_hwnd), doodle_error{"错误的窗口"});

  static std::function<void()> s_set_title_fun{};
  s_set_title_fun = [this]() {
    auto& l_prj  = g_reg()->ctx().get<project>();
    auto l_title = boost::locale::conv::utf_to_utf<char>(g_ctx().get<program_info>().title_attr());
    auto l_str   = fmt::format(
        "{0} 文件 {1} 项目路径 {2} 名称: {3}({4})({5})", l_title,
        g_ctx().get<database_n::file_translator_ptr>()->get_project_path(), l_prj.p_path, l_prj.show_str(), l_prj.str(),
        l_prj.short_str()
    );
    set_title(l_str);
  };
  g_ctx().get<core_sig>().project_end_open.connect(s_set_title_fun);

  s_set_title_fun();
  boost::asio::post(g_io_context(), [this]() { this->load_windows(); });
}
void gui_facet::close_windows() {
  auto g_quit{[l_hwnd = p_hwnd, this]() {
    ::ShowWindow(l_hwnd, SW_HIDE);
    ::DestroyWindow(l_hwnd);
    this->translate_message();
    doodle::app_base::Get().stop_app();
  }};

  boost::asio::post(g_io_context(), g_quit);
}
void gui_facet::show_windows() const { ::ShowWindow(p_hwnd, SW_SHOW); }
void gui_facet::set_title(const std::string& in_title) const {
  boost::asio::post(g_io_context(), [&, in_title]() {
    auto l_str = boost::locale::conv::utf_to_utf<wchar_t>(in_title);
    SetWindowTextW(p_hwnd, l_str.c_str());
  });
}
gui_facet::~gui_facet() { deconstruction(); };
void gui_facet::destroy_windows() { ::PostQuitMessage(0); }

win::drop_manager* gui_facet::drop_manager() { return p_i->dorp_manager.get(); }
}  // namespace doodle::facet
