#pragma once

#include <DoodleLib/DoodleLib_fwd.h>
#include <DoodleLib/Metadata/Metadata.h>
#include <magic_enum.hpp>
namespace doodle {
class DOODLELIB_API Shot : public Metadata {
 public:
  enum class ShotAbEnum;

 private:
  int64_t p_shot;
  std::string p_shot_ab;

  std::weak_ptr<Episodes> p_episodes;

 public:
  Shot();
  Shot(std::weak_ptr<Metadata> in_metadata,
       decltype(p_shot) in_shot,
       decltype(p_shot_ab) in_shot_ab      = {},
       std::weak_ptr<Episodes> in_episodes = {});

  // clang-format off
  enum class ShotAbEnum { A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z };
  // clang-format on

  [[nodiscard]] const decltype(p_shot) &getShot() const noexcept;
  void setShot(const decltype(p_shot) &in_shot);

  [[nodiscard]] const decltype(p_shot_ab) &getShotAb() const noexcept;
  void setShotAb(const decltype(p_shot_ab) &ShotAb) noexcept;
  inline void setShotAb(const ShotAbEnum &ShotAb) {
    setShotAb(std::string{magic_enum::enum_name(ShotAb)});
  };

  [[nodiscard]] EpisodesPtr getEpisodesPtr() const noexcept;
  void setEpisodesPtr(const EpisodesPtr &Episodes_) noexcept;

  [[nodiscard]] std::string str() const override;
  virtual void createMenu(ContextMenu *in_contextMenu) override;

  void load(const MetadataFactoryPtr &in_factory) override;
  void save(const MetadataFactoryPtr &in_factory) override;
  virtual void deleteData(const MetadataFactoryPtr &in_factory) override;
  bool operator<(const Shot &rhs) const;
  bool operator>(const Shot &rhs) const;
  bool operator<=(const Shot &rhs) const;
  bool operator>=(const Shot &rhs) const;

 protected:
  bool sort(const Metadata &in_rhs) const override;
  void modifyParent(const std::shared_ptr<Metadata>& in_old_parent) override;
 private:
  friend class cereal::access;
  template <class Archive>
  void serialize(Archive &ar, std::uint32_t const version);
};
template <class Archive>
void Shot::serialize(Archive &ar, const std::uint32_t version) {
  if (version == 1)
    ar(
        cereal::make_nvp("Metadata", cereal::base_class<Metadata>(this)),
        p_shot,
        p_shot_ab);
}
}  // namespace doodle

namespace cereal {
template <class Archive>
std::string save_minimal(Archive const &, doodle::Shot::ShotAbEnum const &shotab) {
  return std::string{magic_enum::enum_name(shotab)};
}
template <class Archive>
void load_minimal(Archive const &, doodle::Shot::ShotAbEnum &shotab, std::string const &value) {
  shotab = magic_enum::enum_cast<doodle::Shot::ShotAbEnum>(value).value_or(doodle::Shot::ShotAbEnum::A);
};
}  // namespace cereal

CEREAL_REGISTER_TYPE(doodle::Shot)
CEREAL_CLASS_VERSION(doodle::Shot, 1)
