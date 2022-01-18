//
// Created by TD on 2022/1/18.
//

#pragma once

#include <Windows.h>
//#include <windef.h>
//#include <boost/winapi/handles.hpp>
//#include <boost/winapi/show_window.hpp>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
namespace doodle::win {
using wnd_handle   = ::HWND;
using wnd_class    = ::WNDCLASSEX;
using wnd_instance = ::HINSTANCE;

}  // namespace doodle::win
