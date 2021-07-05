﻿//
// Created by TD on 2021/5/7.
//

#pragma once
#include <DoodleLib/DoodleLib_fwd.h>
#include <DoodleLib/Metadata/Metadata.h>
#include <DoodleLib/core/CoreSet.h>

#include <cereal/types/chrono.hpp>

namespace doodle {
/**
  * @brief 这个类代表着服务端的文件条目
  * 
  */
class DOODLELIB_API AssetsFile : public Metadata {
  std::string p_name;
  std::string p_ShowName;
  AssetsPathPtr p_path_file;
  std::vector<AssetsPathPtr> p_path_files;
  TimeDurationPtr p_time;
  std::string p_user;
  Department p_department;
  std::vector<CommentPtr> p_comment;
  std::uint64_t p_version;

 public:
  /**
   * @brief 默认构造
   * 
   */
  AssetsFile();

  /**
   * @brief 构造一个条目并添加一些必要的属性
   * 
   * @param in_metadata 父条目,这个是必须的
   * @param in_path 本地路径,这个在创建时会自动生成一个服务器路径(基本上是一个uuid路径) 
   *                基本是调用 AssetsPath::setPath(const FSys::path &in_path)
   * @param name 名称
   * @param showName 显示名称 
   */
  explicit AssetsFile(std::weak_ptr<Metadata> in_metadata, std::string showName, std::string Name = {});
  // ~AssetsFile();

  [[nodiscard]] std::string str() const override;
  [[nodiscard]] std::string showStr() const override;

  [[nodiscard]] std::chrono::time_point<std::chrono::system_clock> getStdTime() const;
  void setStdTime(const std::chrono::time_point<std::chrono::system_clock>& in_time);
  [[nodiscard]] const TimeDurationPtr& getTime() const;
  void setTime(const TimeDurationPtr& in_time);

  [[nodiscard]] const std::string& getUser() const;
  void setUser(const std::string& in_user);
  const std::vector<AssetsPathPtr>& getPathFile() const;
  void setPathFile(const std::vector<AssetsPathPtr>& in_pathFile);
  Department getDepartment() const;
  void setDepartment(Department in_department);

  [[nodiscard]] const std::vector<CommentPtr>& getComment() const;
  void setComment(const std::vector<CommentPtr>& in_comment);
  void addComment(const CommentPtr& in_comment);

  const std::uint64_t& getVersion() const noexcept;
  std::string getVersionStr() const;
  void setVersion(const std::uint64_t& in_Version) noexcept;

  virtual void create_menu(const menu_factory_ptr& in_factoryPtr) override;

  bool operator<(const AssetsFile& in_rhs) const;
  bool operator>(const AssetsFile& in_rhs) const;
  bool operator<=(const AssetsFile& in_rhs) const;
  bool operator>=(const AssetsFile& in_rhs) const;

 protected:
  void _select_indb(const MetadataFactoryPtr& in_factory) override;
  void _updata_db(const MetadataFactoryPtr& in_factory) override;
  virtual void _insert_into(const MetadataFactoryPtr& in_factory) override;
  virtual void _deleteData(const MetadataFactoryPtr& in_factory) override;
  virtual bool sort(const Metadata& in_rhs) const override;

 private:
  friend class cereal::access;
  template <class Archive>
  void serialize(Archive& ar, std::uint32_t const version);
};

template <class Archive>
void AssetsFile::serialize(Archive& ar, const std::uint32_t version) {
  if (version == 1)
    ar(
        cereal::make_nvp("Metadata", cereal::base_class<Metadata>(this)),
        CEREAL_NVP(p_name),
        CEREAL_NVP(p_ShowName),
        CEREAL_NVP(p_path_file),
        CEREAL_NVP(p_time),
        CEREAL_NVP(p_user),
        CEREAL_NVP(p_department),
        CEREAL_NVP(p_comment));
  if (version == 2)
    ar(
        cereal::make_nvp("Metadata", cereal::base_class<Metadata>(this)),
        CEREAL_NVP(p_name),
        CEREAL_NVP(p_ShowName),
        CEREAL_NVP(p_path_file),
        CEREAL_NVP(p_time),
        CEREAL_NVP(p_user),
        CEREAL_NVP(p_department),
        CEREAL_NVP(p_comment),
        CEREAL_NVP(p_version));
  if (version == 3)
    ar(
        cereal::make_nvp("Metadata", cereal::base_class<Metadata>(this)),
        CEREAL_NVP(p_name),
        CEREAL_NVP(p_ShowName),
        CEREAL_NVP(p_path_files),
        CEREAL_NVP(p_time),
        CEREAL_NVP(p_user),
        CEREAL_NVP(p_department),
        CEREAL_NVP(p_comment),
        CEREAL_NVP(p_version));
}

}  // namespace doodle

CEREAL_REGISTER_TYPE(doodle::AssetsFile)
CEREAL_CLASS_VERSION(doodle::AssetsFile, 3)
