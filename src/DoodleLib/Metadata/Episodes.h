#pragma once
#include <DoodleLib/DoodleLib_fwd.h>
#include <DoodleLib/Metadata/Metadata.h>

namespace doodle {

class DOODLELIB_API Episodes : public Metadata {
  int64_t p_episodes;

 public:
  Episodes();
  explicit Episodes(std::weak_ptr<Metadata> in_metadata, int64_t in_episodes);
  // ~Episodes();
  
  [[nodiscard]] const int64_t &getEpisodes() const noexcept;
  void setEpisodes(const int64_t &Episodes_);

  [[nodiscard]] std::string str() const override;

  bool operator<(const Episodes &in_rhs) const;
  bool operator>(const Episodes &in_rhs) const;
  bool operator<=(const Episodes &in_rhs) const;
  bool operator>=(const Episodes &in_rhs) const;

 protected:
  void _select_indb(const MetadataFactoryPtr &in_factory) override;
  void _updata_db(const MetadataFactoryPtr &in_factory) override;
  virtual void _insert_into(const MetadataFactoryPtr &in_factory) override;
  virtual void _deleteData(const MetadataFactoryPtr &in_factory) override;
  virtual bool sort(const Metadata &in_rhs) const override;

 private:
  friend class cereal::access;
  template <class Archive>
  void serialize(Archive &ar, std::uint32_t const version);
};
template <class Archive>
void Episodes::serialize(Archive &ar, const std::uint32_t version) {
  if (version == 1)
    ar(cereal::make_nvp("Metadata", cereal::base_class<Metadata>(this)),
       p_episodes);
}
}  // namespace doodle

CEREAL_REGISTER_TYPE(doodle::Episodes)
CEREAL_CLASS_VERSION(doodle::Episodes, 1)
