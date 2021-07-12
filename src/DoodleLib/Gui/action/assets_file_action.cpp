//
// Created by TD on 2021/6/28.
//

#include "assets_file_action.h"

#include <Metadata/Assets.h>
#include <Metadata/AssetsFile.h>
#include <Metadata/AssetsPath.h>
#include <Metadata/Comment.h>
#include <Metadata/TimeDuration.h>
namespace doodle {

actn_assfile_create::actn_assfile_create(std::any&& in_any) {
  p_name = "创建资产文件";
}

actn_assfile_create::actn_assfile_create() {
  p_name = "创建资产文件";
}

long_term_ptr actn_assfile_create::run(const MetadataPtr& in_data, const MetadataPtr& in_parent) {
  auto k_s = sig_get_arg().value().date;
  AssetsFilePtr k_item;

  k_item = std::make_shared<AssetsFile>(in_parent, k_s);
  in_parent->child_item.push_back_sig(k_item);
  k_item->setVersion(k_item->find_max_version());
  k_item->updata_db(in_parent->getMetadataFactory());
  in_parent->sortChildItems(true);
  return {};
}

actn_assfile_add_com::actn_assfile_add_com(std::any&& in_any) {
  p_name = "添加评论";
}
long_term_ptr actn_assfile_add_com::run(const MetadataPtr& in_data, const MetadataPtr& in_parent) {
  auto k_s   = sig_get_arg().value().date;
  auto k_ass = std::dynamic_pointer_cast<AssetsFile>(in_data);
  k_ass->addComment(std::make_shared<Comment>(k_s));
  k_ass->updata_db(k_ass->getMetadataFactory());
  return {};
}
actn_assfile_add_com::actn_assfile_add_com() {
  p_name = "添加评论";
}

actn_assfile_datetime::actn_assfile_datetime(std::any&& in_any) {
  p_name = "修改日期";
}
long_term_ptr actn_assfile_datetime::run(const MetadataPtr& in_data, const MetadataPtr& in_parent) {
  auto k_time = sig_get_arg().value().time;
  auto k_ass  = std::dynamic_pointer_cast<AssetsFile>(in_data);
  k_ass->setTime(k_time);
  k_ass->updata_db(k_ass->getMetadataFactory());
  return {};
}
actn_assfile_datetime::actn_assfile_datetime() {
  p_name = "修改日期";
}

actn_assfile_delete::actn_assfile_delete(std::any&& in_any) {
  p_name = "删除";
}
long_term_ptr actn_assfile_delete::run(const MetadataPtr& in_data, const MetadataPtr& in_parent) {
  auto k_ass = std::dynamic_pointer_cast<AssetsFile>(in_data);
  auto k_p   = k_ass->getParent();
  try {
    k_p->child_item.erase_sig(k_ass);
    k_ass->deleteData(k_ass->getMetadataFactory());
  } catch (const std::runtime_error&) {
    DOODLE_LOG_WARN("无法找到 id {} {} ", k_ass->getId(), k_ass->showStr())
  }
  return {};
}
actn_assfile_delete::actn_assfile_delete() {
  p_name = "删除";
}
}  // namespace doodle
