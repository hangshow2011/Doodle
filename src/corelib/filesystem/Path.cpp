#include <corelib/filesystem/Path.h>
#include <boost/filesystem.hpp>

#include <loggerlib/Logger.h>
#include <nlohmann/json.hpp>

DOODLE_NAMESPACE_S
namespace FileSystem {

const fileSys::path Path::p_prefix{R"(\\?\)"};

Path::Path()
    : p_path(std::make_shared<fileSys::path>()),
      p_exist(false),
      p_isDir(false),
      p_size((uint64_t)0),
      p_time(std::chrono::system_clock::from_time_t(0)),
      p_long_path(false) {
}

Path::Path(std::string str)
    : p_path(std::make_shared<fileSys::path>(std::move(str))),
      p_exist(false),
      p_isDir(false),
      p_size((uint64_t)0),
      p_time(std::chrono::system_clock::from_time_t(0)),
      p_long_path(p_path->size() > _MAX_PATH) {
  this->scanningInfo();
}

Path::Path(std::shared_ptr<fileSys::path> path)
    : p_path(std::move(path)),
      p_exist(false),
      p_isDir(false),
      p_size((uint64_t)0),
      p_time(std::chrono::system_clock::from_time_t(0)),
      p_long_path(p_path->size() > _MAX_PATH) {
  this->scanningInfo();
}

const std::shared_ptr<fileSys::path> &Path::path() const noexcept {
  return p_path;
}

void Path::setPath(const std::shared_ptr<fileSys::path> &Path_) noexcept {
  p_path      = Path_;
  p_long_path = p_path->size() > _MAX_PATH;
  scanningInfo();
}

void Path::scanningInfo() {
  DOODLE_LOG_INFO(p_path->generic_string());
  p_exist = fileSys::exists(*p_path);
  if (p_exist) {
    p_isDir = fileSys::is_directory(*p_path);
    if (!p_long_path)
      p_time = std::chrono::system_clock::from_time_t(
          fileSys::last_write_time(*p_path));
    else
      p_time = std::chrono::system_clock::from_time_t(
          fileSys::last_write_time(p_prefix / (*p_path)));

    if (!p_isDir) {
      p_size = fileSys::file_size(*p_path);
    }
  }
}

const bool &Path::exists() const {
  return p_exist;
}

const bool &Path::isDirectory() const {
  return p_isDir;
}

const uint64_t &Path::size() const {
  return p_size;
}

const std::chrono::time_point<std::chrono::system_clock> &Path::modifyTime() const {
  return p_time;
}

void Path::rename(const Path &path) {
  if (!fileSys::exists(path.p_path->parent_path())) {
    fileSys::create_directories(path.p_path->parent_path());
  }
  fileSys::rename(*p_path, *(path.p_path));
  p_path = path.p_path;
}

void Path::copy(const Path &target) {
  if (!p_exist) return;
  // if (fileSys::exists(*(target.p_path))) return;

  if (!fileSys::exists(target.p_path->parent_path())) {
    fileSys::create_directories(target.p_path->parent_path());
  }
  if (target.p_time == p_time && target.p_size == p_size)
    return;
  //我们要在目标时间比来源时间晚的时候才复制和
  else  // if (target.p_time < p_time)
    fileSys::copy_file(*p_path, *(target.p_path), fileSys::copy_option::overwrite_if_exists);
}

void Path::create() {
  if (p_isDir)
    fileSys::create_directories(*p_path);
}

std::vector<std::shared_ptr<Path>> Path::list() {
  DOODLE_LOG_INFO(p_path->generic_string());
  std::vector<std::shared_ptr<Path>> Paths{};
  if (p_isDir) {
    for (auto &it : fileSys::recursive_directory_iterator(*p_path)) {
      auto k_path       = std::make_shared<Path>();
      *(k_path->p_path) = it.path();
      k_path->scanningInfo();
      Paths.push_back(k_path);
    }
  }
  return Paths;
}

std::unique_ptr<std::fstream> Path::open(std::ios_base::openmode modle) {
  if (!fileSys::exists(p_path->parent_path()))
    fileSys::create_directories(p_path->parent_path());

  auto fileptr = std::make_unique<fileSys::fstream>(*p_path, modle);
  if (!fileptr->is_open())
    fileptr->open(*p_path, modle);
  return fileptr;
}
}  // namespace FileSystem
DOODLE_NAMESPACE_E
