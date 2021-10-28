//
// Created by teXiao on 2021/4/27.
//

#pragma once
#include <doodle_lib/core/observable_container.h>
#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/lib_warp/protobuf_warp.h>
#include <doodle_lib/metadata/metadata_factory.h>
#include <doodle_lib/metadata/tree_adapter.h>

#include <any>
#include <boost/intrusive/intrusive_fwd.hpp>
#include <boost/intrusive/link_mode.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/pack_options.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/intrusive/trivial_value_traits.hpp>
#include <boost/serialization/export.hpp>
#include <boost/signals2.hpp>
#include <optional>
namespace doodle {

enum class metadata_type {
  unknown_file       = 0,
  project_root       = 1,
  file               = 2,
  folder             = 3,
  derive_file        = 4,
  animation_lib_root = 5
};

class DOODLELIB_API tree_relationship
/* : public boost::intrusive::set_base_hook<> */ {
 private:
  entt::entity p_parent;

 public:
  std::vector<entt::entity> p_child;
  // boost::intrusive::set<> p_child;

  DOODLE_MOVE(tree_relationship)

  tree_relationship::tree_relationship()
      : p_parent(entt::null),
        p_child() {
  }
  tree_relationship(entt::entity in_parent)
      : tree_relationship() {
    set_parent(std::move(in_parent));
  }
  bool has_parent() const;

  template <class parent_class>
  parent_class *find_parent_class() {
    auto k_p = p_parent;
    auto k_r = g_reg();
    while (k_p != entt::null) {
      auto k     = make_handle(k_p);
      auto k_eps = k.try_get<episodes>();
      if (k_eps)
        return k_eps;
      k_p = k.get<tree_relationship>().get_parent();
    }
    return nullptr;
  }

  [[nodiscard]] const entt::entity &get_parent() const noexcept;
  [[nodiscard]] entt::handle get_parent_h() const noexcept;
  void set_parent(const entt::entity &in_parent) noexcept;

  [[nodiscard]] const std::vector<entt::entity> &get_child() const noexcept;
  [[nodiscard]] std::vector<entt::entity> &get_child() noexcept;
  void set_child(const std::vector<entt::entity> &in_child) noexcept;

  entt::entity get_root() const;
};

class DOODLELIB_API database {
  friend rpc_metadata_client;

 private:
  mutable std::uint64_t p_id;
  mutable string p_id_str;
  std::optional<uint64_t> p_parent_id;
  metadata_type p_type;
  std::string p_uuid;
  std::uint32_t p_boost_serialize_vesion;

  void set_id(std::uint64_t in_id) const;

 public:
  database();
  ~database() = default;

  DOODLE_MOVE(database);

  std::size_t p_has_child;
  std::size_t p_has_file;
  bool p_updata_parent_id;
  bool p_updata_type;
  /**
   * @brief 需要加载
   */
  bool p_need_load;
  /**
   * @brief 需要保存
   */
  bool p_need_save;

  FSys::path get_url_uuid() const;
  bool has_parent() const;
  std::int32_t get_meta_type_int() const;
  bool is_install() const;
  const string &get_id_str() const;

  [[nodiscard]] bool has_child() const;
  [[nodiscard]] bool has_file() const;

  const std::string &get_uuid() const;

  /**
   * @brief 获得数据库id
   *
   * @return std::int32_t
   */
  std::uint64_t get_id() const;

  /**
   * @brief 设置数据库中的类型
   *
   * @param in_meta 类型
   */
  void set_meta_typp(const metadata_type &in_meta);
  void set_meta_typp(const std::string &in_meta);
  void set_meta_type(std::int32_t in_);

  database &operator=(const metadata_database &in);
  explicit operator metadata_database() const;
  bool operator==(const database &in_rhs) const;
  bool operator!=(const database &in_rhs) const;

  friend class boost::serialization::access;
  template <class Archive>
  void save(Archive &ar, const std::uint32_t version) const {
    ar &BOOST_SERIALIZATION_NVP(p_id);
    ar &BOOST_SERIALIZATION_NVP(p_parent_id);
    ar &BOOST_SERIALIZATION_NVP(p_type);
    ar &BOOST_SERIALIZATION_NVP(p_uuid);
    ar &BOOST_SERIALIZATION_NVP(p_has_child);
    ar &BOOST_SERIALIZATION_NVP(p_has_file);
  };

  template <class Archive>
  void load(Archive &ar, const std::uint32_t version) {
    p_boost_serialize_vesion = version;
    if (version == 1) {
      ar &BOOST_SERIALIZATION_NVP(p_id);
      ar &BOOST_SERIALIZATION_NVP(p_parent_id);
      ar &BOOST_SERIALIZATION_NVP(p_type);
      ar &BOOST_SERIALIZATION_NVP(p_uuid);
      ar &BOOST_SERIALIZATION_NVP(p_has_child);
      ar &BOOST_SERIALIZATION_NVP(p_has_file);
    }
  };

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

// using to_str = entt::tag<"to_str"_hs>;

class DOODLELIB_API to_str {
 private:
  mutable std::string p_str;

 public:
  to_str() = default;
  DOODLE_MOVE(to_str);

  const string &get() const;
  operator string() const;
};

}  // namespace doodle

// CEREAL_REGISTER_TYPE(doodle::metadata)
// CEREAL_REGISTER_POLYMORPHIC_RELATION(std::enable_shared_from_this<doodle::metadata>, doodle::metadata)

BOOST_CLASS_VERSION(doodle::database, 1)
BOOST_SERIALIZATION_ASSUME_ABSTRACT(doodle::database)
BOOST_CLASS_EXPORT_KEY(doodle::database)
