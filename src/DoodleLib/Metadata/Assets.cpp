//
// Created by teXiao on 2021/4/27.
//

#include <DoodleLib/Metadata/Assets.h>
#include <DoodleLib/Metadata/MetadataFactory.h>
#include <DoodleLib/PinYin/convert.h>

namespace doodle {
Assets::Assets()
    : Metadata(),
      p_name(),
      p_name_enus() {
}

Assets::Assets(std::weak_ptr<Metadata> in_metadata, std::string in_name)
    : Metadata(std::move(in_metadata)),
      p_name(std::move(in_name)),
      p_name_enus(convert::Get().toEn(p_name)) {
}

// Assets::~Assets() {
//   if (p_metadata_flctory_ptr_)
//     updata_db(p_metadata_flctory_ptr_);
// }

std::string Assets::str() const {
  if(p_name_enus.empty())
    return convert::Get().toEn(p_name);
  return p_name_enus;
}
std::string Assets::showStr() const {
  return p_name;
}

bool Assets::operator<(const Assets& in_rhs) const {
  //  return std::tie(static_cast<const doodle::Metadata&>(*this), p_name) < std::tie(static_cast<const doodle::Metadata&>(in_rhs), in_rhs.p_name);
  return std::tie(p_name) < std::tie(in_rhs.p_name);
}
bool Assets::operator>(const Assets& in_rhs) const {
  return in_rhs < *this;
}
bool Assets::operator<=(const Assets& in_rhs) const {
  return !(in_rhs < *this);
}
bool Assets::operator>=(const Assets& in_rhs) const {
  return !(*this < in_rhs);
}

bool Assets::sort(const Metadata& in_rhs) const {
  if (typeid(in_rhs) == typeid(*this)) {
    return *this < (dynamic_cast<const Assets&>(in_rhs));
  } else {
    return str() < in_rhs.str();
  }
}

const std::string& Assets::getName1() const {
  return p_name;
}
void Assets::setName1(const std::string& in_name) {
  p_name = in_name;
  if(p_name_enus.empty())
    p_name_enus = convert::Get().toEn(p_name);
  saved(true);
  sig_change();
}
const std::string& Assets::getNameEnus() const {
  return p_name_enus;
}
void Assets::setNameEnus(const std::string& in_nameEnus) {
  p_name_enus = in_nameEnus;
  saved(true);
  sig_change();

}
void Assets::_select_indb(const MetadataFactoryPtr& in_factory) {
  in_factory->select_indb(this);
}

void Assets::_updata_db(const MetadataFactoryPtr& in_factory) {
  if (isInstall())
    p_metadata_flctory_ptr_->updata_db(this);
  else
    p_metadata_flctory_ptr_->insert_into(this);
}

void Assets::_insert_into(const MetadataFactoryPtr& in_factory) {
  in_factory->insert_into(this);
}
void Assets::_deleteData(const MetadataFactoryPtr& in_factory) {
  in_factory->deleteData(this);
}
}  // namespace doodle
