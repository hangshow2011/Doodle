#include "shot.h"

#include <doodle_lib/Exception/exception.h>
#include <doodle_lib/core/doodle_lib.h>
#include <doodle_lib/gui/factory/attribute_factory_interface.h>
#include <doodle_lib/metadata/episodes.h>
#include <doodle_lib/metadata/metadata_factory.h>
BOOST_CLASS_EXPORT_IMPLEMENT(doodle::shot)
namespace doodle {

shot::shot()
    : p_shot(-1),
      p_shot_ab("None") {
}

shot::shot(
    decltype(p_shot) in_shot,
    decltype(p_shot_ab) in_shot_ab)
    : p_shot(in_shot),
      p_shot_ab(std::move(in_shot_ab)) {
  if (p_shot < 0)
    throw doodle_error{"shot无法为负"};
}

const int64_t& shot::get_shot() const noexcept {
  return p_shot;
}

void shot::set_shot(const int64_t& in_shot) {
  if (in_shot <= 0)
    throw doodle_error{"shot无法为负"};

  p_shot = in_shot;
}

const std::string& shot::get_shot_ab() const noexcept {
  return p_shot_ab;
}

shot::shot_ab_enum shot::get_shot_ab_enum() const noexcept {
  return magic_enum::enum_cast<shot_ab_enum>(p_shot_ab).value_or(shot_ab_enum::None);
}

void shot::set_shot_ab(const std::string& ShotAb) noexcept {
  p_shot_ab = ShotAb;
}
episodes* shot::get_episodes_ptr() const {
  auto k_reg  = g_reg();
  auto k_ent  = make_handle(*this);
  auto &k_tree = k_ent.get<tree_relationship>();
  return k_tree.find_parent_class<episodes>();
}

std::string shot::str() const {
  return fmt::format("sc{:04d}{}", p_shot, p_shot_ab == "None" ? "" : p_shot_ab);
}
bool shot::operator<(const shot& rhs) const {
  return std::tie(p_shot, p_shot_ab) < std::tie(rhs.p_shot, rhs.p_shot_ab);
}
bool shot::operator>(const shot& rhs) const {
  return rhs < *this;
}
bool shot::operator<=(const shot& rhs) const {
  return !(rhs < *this);
}
bool shot::operator>=(const shot& rhs) const {
  return !(*this < rhs);
}

bool shot::analysis(const std::string& in_path) {
  static std::regex reg{R"(sc_?(\d+)([a-z])?)", std::regex_constants::icase};
  std::smatch k_match{};
  const auto& k_r = std::regex_search(in_path, k_match, reg);
  if (k_r) {
    p_shot = std::stoi(k_match[1].str());
    if (k_match.size() > 2)
      p_shot_ab = k_match[2].str();
  }
  return k_r;
}

shot_ptr shot::analysis_static(const std::string& in_path) {
  auto k_shot = new_object<shot>();
  if (k_shot->analysis(in_path))
    return k_shot;
  else
    return {};
}

void shot::attribute_widget(const attribute_factory_ptr& in_factoryPtr) {
  in_factoryPtr->show_attribute(this);
}

}  // namespace doodle
