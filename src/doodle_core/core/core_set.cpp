#include "core_set.h"

#include <doodle_core/pin_yin/convert.h>
#include <doodle_core/logger/logger.h>

#include <doodle_core/lib_warp/boost_uuid_warp.h>
#include <boost/algorithm/string.hpp>
#include <boost/dll.hpp>

#ifdef _WIN32
#include <ShlObj.h>
#else
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif  // _WIN32

namespace doodle {

FSys::path win::get_pwd()
#ifdef _WIN32
{
  /// 这里我们手动做一些工作
  /// 获取环境变量 FOLDERID_Documents
  PWSTR pManager;
  SHGetKnownFolderPath(FOLDERID_Documents, NULL, nullptr, &pManager);
  chick_true<doodle_error>(pManager, DOODLE_LOC, "unable to find a save path");

  auto k_path = FSys::path{pManager};
  CoTaskMemFree(pManager);
  return k_path;
}
#else
{
  auto pw = getpwuid(getuid())->pw_dir;
  return FSys::path{pw};
};
#endif  // _WIN32

FSys::path win::get_font() {
  /// 这里我们手动做一些工作
  /// 获取环境变量 FOLDERID_Documents
  PWSTR pManager;
  SHGetKnownFolderPath(FOLDERID_Fonts, NULL, nullptr, &pManager);
  chick_true<doodle_error>(pManager, DOODLE_LOC, "unable to find a save path");

  auto k_path = FSys::path{pManager};
  CoTaskMemFree(pManager);
  return k_path;
}

core_set &core_set::getSet() {
  static core_set install;
  return install;
}

bool core_set::has_maya() const noexcept {
  return !p_mayaPath.empty();
}

const FSys::path &core_set::maya_path() const noexcept {
  return p_mayaPath;
}

void core_set::set_maya_path(const FSys::path &in_MayaPath) noexcept {
  p_mayaPath = in_MayaPath;
}

core_set::core_set()
    : p_user_("user"),
      organization_name(),
      p_doc(FSys::current_path()),
      p_uuid_gen(),
      p_mayaPath(),
      p_max_thread(std::thread::hardware_concurrency() - 2),
      p_root(FSys::temp_directory_path() / "Doodle"),
      _root_cache(p_root / "cache"),
      _root_data(p_root / "data"),
      timeout(3600),
      max_install_reg_entt(8),
      json_data(std::make_shared<nlohmann::json>()) {
}

boost::uuids::uuid core_set::get_uuid() {
  return p_uuid_gen();
}

std::string core_set::get_user() const { return p_user_; }

std::string core_set::get_user_en() const {
  return boost::algorithm::to_lower_copy(
      convert::Get().toEn(p_user_));
}

void core_set::set_user(const std::string &value) {
  p_user_ = value;
}

FSys::path core_set::get_doc() const {
  return p_doc;
}

void core_set::set_root(const FSys::path &in_path) {
  p_root      = in_path;
  _root_cache = p_root / "cache";
  _root_data  = p_root / "data";
  DOODLE_LOG_INFO("设置缓存目录", p_root);
}

FSys::path core_set::get_cache_root() const {
  return _root_cache;
}

FSys::path core_set::get_cache_root(const FSys::path &in_path) const {
  auto path = get_cache_root() / in_path;
  if (!FSys::exists(path))
    FSys::create_directories(path);
  return path;
}

FSys::path core_set::get_data_root() const {
  return _root_data;
}

FSys::path core_set::program_location() {
  return boost::dll::program_location().parent_path();
}
std::string core_set::config_file_name() {
  static std::string str{"doodle_config"};
  return str;
}
std::string core_set::get_uuid_str() {
  return boost::uuids::to_string(get_uuid());
}

void core_set::set_max_tread(const std::uint16_t in) {
  p_max_thread = in;
}

core_set_init::core_set_init()
    : p_set(core_set::getSet()) {
  if (!FSys::exists(p_set.p_doc))
    FSys::create_directories(p_set.p_doc);
  if (!FSys::exists(p_set.get_cache_root())) {
    FSys::create_directories(p_set.get_cache_root());
  }
  if (!FSys::exists(p_set.get_data_root())) {
    FSys::create_directories(p_set.get_data_root());
  }
}
bool core_set_init::find_maya() {
  DOODLE_LOG_INFO("寻找maya");

  if (FSys::exists(R"(C:\Program Files\Autodesk\Maya2020\bin)")) {
    p_set.p_mayaPath = R"(C:\Program Files\Autodesk\Maya2020\bin\)";
    return true;
  } else if (FSys::exists(R"(C:\Program Files\Autodesk\Maya2019\bin)")) {
    p_set.p_mayaPath = R"(C:\Program Files\Autodesk\Maya2019\bin\)";
    return true;
  } else if (FSys::exists(R"(C:\Program Files\Autodesk\Maya2018\bin)")) {
    p_set.p_mayaPath = R"(C:\Program Files\Autodesk\Maya2018\bin\)";
    return true;
  }
  return false;
}

bool core_set_init::read_file() {
  FSys::path l_k_setting_file_name = p_set.get_doc() / p_set.config_file_name();
  DOODLE_LOG_INFO("读取配置文件 {}", l_k_setting_file_name);
  if (FSys::exists(l_k_setting_file_name)) {
    FSys::ifstream l_in_josn{l_k_setting_file_name, std::ifstream::binary};
    try {
      *p_set.json_data = nlohmann::json::parse(l_in_josn);
      p_set.json_data->at("setting").get_to(p_set);
    } catch (const nlohmann::json::parse_error &err) {
      DOODLE_LOG_DEBUG(err.what());
      return false;
    }
    return true;
  }
  return false;
}
bool core_set_init::write_file() {
  DOODLE_LOG_INFO("写入配置文件 {}", p_set.p_doc / p_set.config_file_name());

  FSys::ofstream l_ofstream{p_set.p_doc / p_set.config_file_name(), std::ios::out | std::ios::binary};
  (*p_set.json_data)["setting"] = p_set;
  l_ofstream << p_set.json_data->dump();
  return true;
}

bool core_set_init::config_to_user() {
  p_set.p_doc = win::get_pwd() / "doodle";
  if (!FSys::exists(p_set.p_doc)) {
    FSys::create_directories(p_set.p_doc);
  }
  return true;
}

nlohmann::json &core_set_init::json_value() {
  return *p_set.json_data;
}

void to_json(nlohmann::json &j, const core_set &p) {
  j["user_"]                = p.p_user_;
  j["organization_name"]    = p.organization_name;
  j["mayaPath"]             = p.p_mayaPath;
  j["max_thread"]           = p.p_max_thread;
  j["timeout"]              = p.timeout;
  j["project_root"]         = p.project_root;
  j["max_install_reg_entt"] = p.max_install_reg_entt;
  j["ue4_path"]             = p.ue4_path;
  j["ue4_version"]          = p.ue4_version;
}
void from_json(const nlohmann::json &j, core_set &p) {
  j.at("user_").get_to(p.p_user_);
  if (j.count("organization_name"))
    j.at("organization_name").get_to(p.organization_name);
  if (j.count("ue4_setting")) {
    j["ue4_setting"].at("ue4_path").get_to(p.ue4_path);
    j["ue4_setting"].at("ue4_version").get_to(p.ue4_version);
  }
  if (j.count("ue4_path"))
    j["ue4_path"].get_to(p.ue4_path);
  if (j.count("ue4_version"))
    j["ue4_version"].get_to(p.ue4_version);
  j.at("mayaPath").get_to(p.p_mayaPath);
  j.at("max_thread").get_to(p.p_max_thread);
  j.at("timeout").get_to(p.timeout);
  if (j.contains("project_root"))
    j.at("project_root").get_to(p.project_root);
  if (j.contains("max_install_reg_entt"))
    j.at("max_install_reg_entt").get_to(p.max_install_reg_entt);
}
void core_set::add_recent_project(const FSys::path &in) {
  auto k_find_root = std::find(project_root.begin(), project_root.end(), in);
  if (k_find_root != project_root.end())
    std::swap(project_root[0], *k_find_root);
  else {
    std::rotate(project_root.rbegin(), project_root.rbegin() + 1, project_root.rend());
    project_root[0] = in;
  }
}
std::string core_set::get_uuid_str(const std::string &in_add) {
  return get_uuid_str() + in_add;
}
}  // namespace doodle
