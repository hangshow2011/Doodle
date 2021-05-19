// Fill out your copyright notice in the Description page of Project Settings.
#include <Metadata/Project.h>
#include <Exception/Exception.h>
#include <Metadata/MetadataFactory.h>
#include <Logger/Logger.h>
#include <PinYIn/convert.h>

#include <core/coreset.h>
#include <DoodleLib/Metadata/ContextMenu.h>

#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#include <cereal/archives/portable_binary.hpp>

namespace doodle {

Project::Project()
    : p_name(),
      p_path() {
}

Project::Project(FSys::path in_path, std::string in_name)
    : p_name(std::move(in_name)),
      p_path(std::move(in_path)) {
}

const std::string& Project::getName() const noexcept {
  return p_name;
}

void Project::setName(const std::string& Name) noexcept {
  p_path = Name;
}

const FSys::path& Project::getPath() const noexcept {
  return p_path;
}

void Project::setPath(const FSys::path& Path) {
  if (Path.empty())
    throw DoodleError{"项目路径不能为空"};
  p_path = Path;
}

std::string Project::str() const {
  return boost::algorithm::to_lower_copy(
      convert::Get().toEn(this->p_name));
}

std::string Project::shortStr() const {
  auto wstr       = boost::locale::conv::utf_to_utf<wchar_t>(this->p_name);
  auto& k_pingYin = convert::Get();
  std::string str{};
  for (auto s : wstr) {
    auto k_s_front = k_pingYin.toEn(s).front();
    str.append(&k_s_front, 1);
  }
  DOODLE_LOG_INFO(str)
  return boost::algorithm::to_upper_copy(str.substr(0, 2));
}

std::string Project::showStr() const {
  return this->p_name;
}
std::string Project::getConfigFileFolder() {
  static std::string str{".doodle_config"};
  return str;
}

std::string Project::getConfigFileName() {
  static std::string str{"doodle_config.dole"};
  return str;
}
FSys::path Project::DBRoot() const {
  return p_path / "_._root";
}
void Project::save(const MetadataFactoryPtr& in_factory) {
  p_metadata_flctory_ptr_ = in_factory;
  in_factory->save(this);
}
void Project::load(const MetadataFactoryPtr& in_factory) {
  in_factory->load(this);
  p_metadata_flctory_ptr_ = in_factory;
}
bool Project::operator<(const Project& in_rhs) const {
  //  return std::tie(static_cast<const doodle::Metadata&>(*this), p_name, p_path) < std::tie(static_cast<const doodle::Metadata&>(in_rhs), in_rhs.p_name, in_rhs.p_path);
  return std::tie(p_name, p_path) < std::tie(in_rhs.p_name, in_rhs.p_path);
}
bool Project::operator>(const Project& in_rhs) const {
  return in_rhs < *this;
}
bool Project::operator<=(const Project& in_rhs) const {
  return !(in_rhs < *this);
}
bool Project::operator>=(const Project& in_rhs) const {
  return !(*this < in_rhs);
}

bool Project::sort(const Metadata& in_rhs) const {
  if (typeid(in_rhs) == typeid(*this)) {
    return *this < (dynamic_cast<const Project&>(in_rhs));
  } else {
    return str() < in_rhs.str();
  }
}
void Project::modifyParent(const std::shared_ptr<Metadata>& in_old_parent) {
  //在这里， 如果已经保存过或者已经是从磁盘中加载来时， 必然会持有工厂， 这个时候我们就要告诉工厂， 我们改变了父子关系
  if (p_metadata_flctory_ptr_)
    p_metadata_flctory_ptr_->modifyParent(this, in_old_parent.get());
}
void Project::createMenu(ContextMenu* in_contextMenu) {
  in_contextMenu->createMenu(std::dynamic_pointer_cast<Project>(shared_from_this()));
}
void Project::deleteData(const MetadataFactoryPtr& in_factory) {
  in_factory->deleteData(this);
}

}  // namespace doodle
