﻿#include <corelib/core/coreset.h>

#include <corelib/core/coresql.h>

#include <corelib/Exception/Exception.h>
#include <pinyinlib/convert.h>
#include <loggerlib/Logger.h>

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/process.hpp>
#include <boost/dll.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#include <magic_enum.hpp>
#include <ShlObj.h>
DOODLE_NAMESPACE_S

const dstring coreSet::settingFileName = "doodle_conf.json";

coreSet &coreSet::getSet() {
  static coreSet install;
  return install;
}

void coreSet::init() {
  //这里我们手动做一些工作
  //获取环境变量
  PWSTR pManager;
  SHGetKnownFolderPath(FOLDERID_Documents, NULL, NULL, &pManager);
  if (!pManager) DoodleError("无法找到保存路径");

  doc = fileSys::path{pManager} / "doodle";
  getSetting();
  coreSql &sql = coreSql::getCoreSql();
  getCacheDiskPath();
  appendEnvironment();

  if (!boost::filesystem::exists(getCacheRoot())) {
    boost::filesystem::create_directories(getCacheRoot());
  }
}

void coreSet::reInit() {
}

void coreSet::appendEnvironment() const {
  auto env          = boost::this_process::environment();
  auto this_process = program_location();
  env["PATH"] += (this_process.parent_path() / "tools/ffmpeg/bin").generic_string();
  if (boost::filesystem::exists(R"(C:\Program Files\Autodesk\Maya2018\bin)")) {
    env["PATH"] += R"(C:\Program Files\Autodesk\Maya2018\bin\)";
  } else if (boost::filesystem::exists(R"(C:\Program Files\Autodesk\Maya2019\bin)")) {
    env["PATH"] += R"(C:\Program Files\Autodesk\Maya2019\bin\)";
  } else if (boost::filesystem::exists(R"(C:\Program Files\Autodesk\Maya2020\bin)")) {
    env["PATH"] += R"(C:\Program Files\Autodesk\Maya2020\bin\)";
  }
}

void coreSet::writeDoodleLocalSet() {
  if (ue4_setting.hasPath() && !FSys::exists(ue4_setting.Path() / "Engine/Binaries/Win64/UE4Editor.exe")) {
    ue4_setting.setPath({});
    throw FileError{ue4_setting.Path(), " 在路径中没有找到ue,不保存"};
  }
  nlohmann::json root;

  root["user"]       = user;
  root["department"] = getDepartment();
  root["synPath"]    = synPath.generic_string();

  boost::filesystem::ofstream outjosn;
  outjosn.open(doc / settingFileName, std::ifstream::binary);
  outjosn << root.dump();
  outjosn.close();
}

coreSet::coreSet()
    : user("user"),
      department(Department::VFX),
      synPath("D:/ue_prj"),
      cacheRoot("C:/Doodle_cache"),
      doc("C:/Doodle_cache"),
      p_uuid_gen(),
      ue4_setting(Ue4Setting::Get()) {
}

void coreSet::getSetting() {
  static fileSys::path k_settingFileName = doc / settingFileName;
  if (boost::filesystem::exists(k_settingFileName)) {
    fileSys::path strFile(k_settingFileName);
    boost::filesystem::ifstream inJosn;
    inJosn.open(k_settingFileName, std::ifstream::binary);

    nlohmann::json root = nlohmann::json::parse(inJosn);
    inJosn.close();

    user       = root.value("user", "uset_tmp");
    auto k_dep = root.value("department", "VFX");
    department = magic_enum::enum_cast<Department>(k_dep).value_or(Department::VFX);
    synPath    = root.value("synPath", "D:/ue_prj");
  }
}

dstring coreSet::toIpPath(const dstring &path) {
  static boost::regex exp("^[A-Z]:");
  if (boost::regex_search(path, exp)) {
    return path.substr(2);
  }
  return path;
}

boost::uuids::uuid coreSet::getUUID() {
  return p_uuid_gen();
}

dstring coreSet::getDepartment() const {
  return std::string{magic_enum::enum_name(department)};
}

void coreSet::setDepartment(const dstring &value) {
  department = magic_enum::enum_cast<Department>(value).value_or(Department::VFX);
}

dstring coreSet::getUser() const { return user; }

dstring coreSet::getUser_en() const {
  return boost::algorithm::to_lower_copy(
      dopinyin::convert::Get().toEn(user));
}

void coreSet::setUser(const dstring &value) { user = value; }

fileSys::path coreSet::getDoc() const { return doc; }

fileSys::path coreSet::getCacheRoot() const { return cacheRoot; }

void coreSet::getServerSetting() {
  //获得项目个数

  // *shotRoot   = map["shotRoot"];
  // *assRoot    = map["assetsRoot"];
  // *prjectRoot = map["project"];
}

void coreSet::getCacheDiskPath() {
  const static std::vector<std::string> dirs{"D:/",
                                             "E:/",
                                             "F:/",
                                             "G:/",
                                             "H:/",
                                             "I:/",
                                             "J:/",
                                             "K:/",
                                             "L:/"};
  for (auto &dir : dirs) {
    if (fileSys::exists(dir)) {
      auto info = fileSys::space(dir);
      if (((float)info.available / (float)info.available) > 0.05) {
        cacheRoot = dir + "Doodle_cache";
        break;
      }
    }
  }
}

fileSys::path coreSet::program_location() {
  return boost::dll::program_location().parent_path();
}
fileSys::path coreSet::program_location(const fileSys::path &path) {
  return program_location() / path;
}

DOODLE_NAMESPACE_E
