﻿//
// Created by TD on 2021/5/18.
//

#pragma once
#include <DoodleLib/DoodleLib_fwd.h>

namespace doodle {
class DOODLELIB_API AssetsPath {
  /**
   * @brief 上传时的本地路径
   * 
   */
  FSys::path p_local_path;
  /**
   * @brief 上传时相对本地根的路径
   * 
   */
  FSys::path p_lexically_relative;
  /**
   * @brief 服务器路径
   * 
   */
  FSys::path p_server_path;
  /**
   * @brief 服务器中如果存在文件的备份路径
   * 
   */
  FSys::path p_backup_path;

 public:
  AssetsPath();
  /**
   * @brief 生成一个类
   * 
   * @param in_path 输入路径，这个会调用 AssetsPath::setPath(const FSys::path &in_path)
   */
  explicit AssetsPath(const FSys::path &in_path, const MetadataConstPtr &in_metadata = {});

  [[nodiscard]] const FSys::path &getLocalPath() const;
  [[nodiscard]] FSys::path get_cache_path() const;
  [[nodiscard]] const FSys::path &getServerPath() const;
  [[nodiscard]] const FSys::path &getBackupPath() const;
  /**
   * @brief 设置资产的本地文件的路径，自动产生服务器路径
   * 同时会自动产生服务器路径 
   *  
   * @param in_path 本地文件的路径 
   */
  void setPath(const FSys::path &in_path);
  /**
   * @brief 设置资产的本地文件的路径，根据元数据产生路径
   * 
   * 会产生根据元数据标签产生的路径
   * 
   * @param in_path  本地文件的路径 
   * @param in_metadata 元数据指针， 
   */
  void setPath(const FSys::path &in_path, const MetadataConstPtr &in_metadata);

  /**
   * @brief设置资产的本地文件的路径，直接设置服务器路径
   * 
   * @param in_local_path 本地路径
   * @param in_server_path 服务器路径
   */
  void setPath(const FSys::path &in_local_path, const FSys::path &in_server_path);
  //  void open();
 private:
  //这里是序列化的代码
  friend class cereal::access;
  template <class Archive>
  void serialize(Archive &ar, std::uint32_t const version);
};

template <class Archive>
void AssetsPath::serialize(Archive &ar, const std::uint32_t version) {
  if (version == 1)
    ar(cereal::make_nvp("path", p_local_path));

  if (version == 2)
    ar(cereal::make_nvp("local_path", p_local_path),
       cereal::make_nvp("lexically_relative", p_lexically_relative),
       cereal::make_nvp("server_path", p_server_path));

  if (version == 3)
    ar(cereal::make_nvp("local_path", p_local_path),
       cereal::make_nvp("lexically_relative", p_lexically_relative),
       cereal::make_nvp("server_path", p_server_path),
       cereal::make_nvp("backup_path", p_backup_path));
}

}  // namespace doodle

CEREAL_CLASS_VERSION(doodle::AssetsPath, 3)
