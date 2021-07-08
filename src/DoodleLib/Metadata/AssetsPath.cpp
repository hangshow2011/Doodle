﻿//
// Created by TD on 2021/5/18.
//

#include "AssetsPath.h"

#include <DoodleLib/Exception/Exception.h>
#include <DoodleLib/Metadata/Metadata.h>
#include <DoodleLib/core/CoreSet.h>
#include <Logger/Logger.h>
namespace doodle {
AssetsPath::AssetsPath()
    : p_local_path(),
      p_lexically_relative(),
      p_server_path() {
}
AssetsPath::AssetsPath(const FSys::path &in_path, const MetadataConstPtr &in_metadata)
    : p_local_path(),
      p_lexically_relative(),
      p_server_path() {
  if (in_metadata)
    this->setPath(in_path, in_metadata);
  else
    setPath(in_path);
}
const FSys::path &AssetsPath::getLocalPath() const {
  return p_local_path;
}

const FSys::path &AssetsPath::getServerPath() const {
  return p_server_path;
}
void AssetsPath::setPath(const FSys::path &in_path) {
  auto &k_set       = CoreSet::getSet();
  auto uuid         = k_set.getUUIDStr();
  auto k_prj        = k_set.get_project();
  FSys::path k_path = k_prj->str();

  auto k_server_path = k_path / uuid.substr(3) / uuid / in_path.filename();
  setPath(in_path, k_server_path);
}

void AssetsPath::setPath(const FSys::path &in_path, const MetadataConstPtr &in_metadata) {
  auto &k_set = CoreSet::getSet();
  auto k_prj  = k_set.get_project();

  /// 这里使用树,向上寻找,组合路径
  MetadataConstPtr k_m{};
  if (details::is_class<AssetsFile>(in_metadata))
    k_m = in_metadata->getParent();
  else
    k_m = in_metadata;

  FSys::path k_path{k_m->str()};
  while (k_m->hasParent()) {
    k_m    = k_m->getParent();
    k_path = FSys::path{k_m->str()} / k_path;
  }

  setPath(in_path, k_path);
}

void AssetsPath::setPath(const FSys::path &in_local_path, const FSys::path &in_server_path) {
  if (!FSys::exists(in_local_path))
    throw DoodleError{"不存在文件"};
  p_local_path           = in_local_path;
  const auto k_root_path = in_local_path.root_path();
  p_lexically_relative   = in_local_path.lexically_relative(k_root_path);
  p_server_path          = in_server_path;
  DOODLE_LOG_INFO(fmt::format("本地路径: {}, 设置服务路径: {}, 相对路径: {} ", p_local_path, p_server_path, p_lexically_relative));
}
}  // namespace doodle
