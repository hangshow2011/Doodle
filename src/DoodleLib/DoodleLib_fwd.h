﻿#pragma once

#include <DoodleConfig.h>

#include <boost/filesystem.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#include <DoodleLib/DoodleMacro.h>
#include <DoodleLib/Logger/Logger.h>
#include <DoodleLib/libWarp/CerealWarp.h>
#include <DoodleLib/libWarp/cmrcWarp.h>
#include <DoodleLib/libWarp/sqlppWarp.h>
#include <date/date.h>
#include <doodlelib_export.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

//#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <codecvt>
//#undef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <stdexcept>
// namespace fmt {
// namespace FSys = std::filesystem;
// template <class Char>
// struct formatter<FSys::path, Char> : formatter<basic_string_view<Char>, Char> {
//   template <typename FormatContext>
//   auto format(const FSys::path &in_path, FormatContext &ctx) {
//     if constexpr (std::is_same_v<Char, char>)
//       return formatter<basic_string_view<Char>, Char>::format(in_path.generic_string(), ctx);
//     else if constexpr (std::is_same_v<Char, wchar_t>)
//       return formatter<basic_string_view<Char>, Char>::format(in_path.generic_wstring(), ctx);
//   }
// };
// }  // namespace fmt

//开始我们的名称空间
namespace doodle {
namespace details {
class no_copy {
 public:
  no_copy()                = default;
  no_copy(const no_copy &) = delete;
  no_copy &operator=(const no_copy &) = delete;
};
template <typename T, typename Enable = void>
struct is_smart_pointer {
  static const auto value = std::false_type::value;
};

template <typename T>
struct is_smart_pointer<T, typename std::enable_if<std::is_same<typename std::remove_cv<T>::type, std::shared_ptr<typename T::element_type>>::value>::type> {
  static const auto value = std::true_type::value;
};

template <typename T>
struct is_smart_pointer<T, typename std::enable_if<std::is_same<typename std::remove_cv<T>::type, std::unique_ptr<typename T::element_type>>::value>::type> {
  static const auto value = std::true_type::value;
};

template <typename T>
struct is_smart_pointer<T, typename std::enable_if<std::is_same<typename std::remove_cv<T>::type, std::weak_ptr<typename T::element_type>>::value>::type> {
  static const auto value = std::true_type::value;
};

/**
 * 这个是判断指针或者共享指针是什么类的帮助函数
 * @tparam T 是否是这个类
 * @tparam RT 传入的类型
 * @param in_rt 输入的
 * @return 是否是可以转换的
 */
template <class T, class RT>
bool is_class(const RT &in_rt) {
  if constexpr (is_smart_pointer<RT>::value) {
    if (!in_rt)
      return false;
    const auto &k_item = *in_rt;
    return typeid(T) == typeid(k_item);
  } else {
    return typeid(T) == typeid(in_rt);
  }
}

}  // namespace details
namespace chrono {
namespace literals {
using namespace std::chrono_literals;
using namespace date::literals;

}  // namespace literals

using namespace std::chrono;
using namespace date;

using hours_double = duration<std::double_t, std::ratio<3600>>;
using days_double  = duration<std::double_t, std::ratio<28800>>;
using sys_time_pos = time_point<system_clock>;

/// TODO: 这里我们暂时使用周六和周日作为判断, 但是实际上还有各种假期和其他情况要计入
bool is_rest_day(const sys_days &in_days);
// template <class Clock>
// bool is_morning_works(const std::chrono::time_point<Clock, typename Clock::duration>& in_time) {
// }
}  // namespace chrono

namespace FSys {
// namespace details{
// using namespace std::filesystem;
//
// }
using namespace std::filesystem;
using fstream  = std::fstream;
using istream  = std::istream;
using ifstream = std::ifstream;
using ofstream = std::ofstream;
using ostream  = std::ostream;

// class path : public ::std::filesystem::path{
//  public:
//
// };

// using std::filesystem::filesystem_error;
// using std::filesystem::directory_entry;
// using std::filesystem::directory_iteratorr;
// using std::filesystem::recursive_directory_iterator;
// using std::filesystem::file_status;
// using std::filesystem::space_info;
// using std::filesystem::file_type;
// using std::filesystem::perms;
// using std::filesystem::perm_options;
// using std::filesystem::copy_options;
// using std::filesystem::directory_options;
// using std::filesystem::file_time_type;
//
// using std::filesystem::absolute;
// using std::filesystem::canonical;
// using std::filesystem::weakly_canonical;
// using std::filesystem::copy;
// using std::filesystem::copy_file;
// using std::filesystem::copy_symlink;
// using std::filesystem::create_directories;
// using std::filesystem::create_directory;
// using std::filesystem::create_hard_link;
// using std::filesystem::create_symlink;
// using std::filesystem::create_directory_symlink;
// using std::filesystem::current_path;
// using std::filesystem::exists;
// using std::filesystem::equivalent;
// using std::filesystem::file_size;
// using std::filesystem::hard_link_count;
// using std::filesystem::last_write_time;
// using std::filesystem::permissions;
// using std::filesystem::read_symlink;
// using std::filesystem::remove;
// using std::filesystem::remove_all;
// using std::filesystem::rename;
// using std::filesystem::resize_file;
// using std::filesystem::space;
// using std::filesystem::status;
// using std::filesystem::symlink_status;
// using std::filesystem::temp_directory_path;
//
// using std::filesystem::is_block_file;
// using std::filesystem::is_character_file;
// using std::filesystem::is_directory;
// using std::filesystem::is_empty;
// using std::filesystem::is_fifo;
// using std::filesystem::is_other;
// using std::filesystem::is_regular_file;
// using std::filesystem::is_socket;
// using std::filesystem::is_symlink;
// using std::filesystem::status;
DOODLELIB_API inline path make_path(const std::string &in_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  return path{convert.from_bytes(in_string)};
}
DOODLELIB_API std::time_t last_write_time_t(const path &in_path);
DOODLELIB_API inline std::chrono::time_point<std::chrono::system_clock> last_write_time_point(const path &in_path) {
  return std::chrono::system_clock::from_time_t(last_write_time_t(in_path));
}
DOODLELIB_API path add_time_stamp(const path &in_path);
DOODLELIB_API void open_explorer(const path &in_path);
DOODLELIB_API void backup_file(const path &source);
DOODLELIB_API std::string file_hash_sha224(const path &in_file);
}  // namespace FSys

using ConnPtr = std::unique_ptr<sqlpp::mysql::connection>;

class CoreSet;
class Project;
class Episodes;
class Shot;
class Metadata;
class Assets;
class CoreSql;
class LabelNode;
class AssetsFile;
class MetadataFactory;
class ContextMenu;
class TimeDuration;
class Comment;
class AssetsPath;

class RpcMetadataClient;
class RpcMetadaataServer;
class ProjectManage;
class RpcServerHandle;
class AssetsTree;
class ListAttributeModel;
class RpcFileSystemClient;
class RpcFileSystemServer;
class DragFilesFactory;
class action;
class actn_up_paths;
class DoodleLib;
class ThreadPool;
class menu_factory_base;
class long_term;
class ImageSequence;
class season;
class actn_create_ass_up_paths;
class action_base;
class tool_box_menu_factory;
class VideoSequence;
class Ue4Project;
using ue4_project_ptr = std::shared_ptr<Ue4Project>;

using video_sequence_ptr        = std::shared_ptr<VideoSequence>;
using tool_box_menu_factory_ptr = std::shared_ptr<tool_box_menu_factory>;

using action_base_ptr              = std::shared_ptr<action_base>;
using string_list                  = std::vector<std::string>;
using string_matrix2               = std::vector<string_list>;
using string_matrix2_ptr           = std::shared_ptr<string_matrix2>;
using string_list_ptr              = std::shared_ptr<string_list>;
using actn_create_ass_up_paths_ptr = std::shared_ptr<actn_create_ass_up_paths>;
using season_ptr                   = std::shared_ptr<season>;
using MetadataPtr                  = std::shared_ptr<Metadata>;
using MetadataConstPtr             = std::shared_ptr<const Metadata>;
using RpcMetadataClientPtr         = std::shared_ptr<RpcMetadataClient>;
using RpcMetadataServerPtr         = std::shared_ptr<RpcMetadaataServer>;
using RpcServerHandlePtr           = std::shared_ptr<RpcServerHandle>;
using RpcFileSystemServerPtr       = std::shared_ptr<RpcFileSystemServer>;
using RpcFileSystemClientPtr       = std::shared_ptr<RpcFileSystemClient>;
using ProjectPtr                   = std::shared_ptr<Project>;
using EpisodesPtr                  = std::shared_ptr<Episodes>;
using ShotPtr                      = std::shared_ptr<Shot>;
using AssetsPtr                    = std::shared_ptr<Assets>;
using AssetsFilePtr                = std::shared_ptr<AssetsFile>;
using coreSqlPtr                   = std::shared_ptr<CoreSql>;
using LabelNodePtr                 = std::shared_ptr<LabelNode>;
using AssetsFilePtr                = std::shared_ptr<AssetsFile>;
using MetadataFactoryPtr           = std::shared_ptr<MetadataFactory>;
using TimeDurationPtr              = std::shared_ptr<TimeDuration>;
using CommentPtr                   = std::shared_ptr<Comment>;
using AssetsPathPtr                = std::shared_ptr<AssetsPath>;
using DragFilesFactoryPtr          = std::shared_ptr<DragFilesFactory>;
using action_ptr                   = std::shared_ptr<action>;
using actn_up_paths_ptr            = std::shared_ptr<actn_up_paths>;
using DoodleLibPtr                 = std::unique_ptr<DoodleLib>;
using ThreadPoolPtr                = std::shared_ptr<ThreadPool>;
using menu_factory_ptr             = std::shared_ptr<menu_factory_base>;
using long_term_ptr                = std::shared_ptr<long_term>;
using ImageSequencePtr             = std::shared_ptr<ImageSequence>;

namespace rpc_trans {
class down_file;
class down_dir;
class up_file;
class up_dir;
class trans_file;
class trans_files;
using trans_file_ptr = std::shared_ptr<trans_file>;
}  // namespace rpc_trans
class Doodle;
[[maybe_unused]] DOODLELIB_API DoodleLibPtr make_doodle_lib();

}  // namespace doodle
