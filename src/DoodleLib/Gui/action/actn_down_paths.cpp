//
// Created by TD on 2021/7/13.
//

#include "actn_down_paths.h"

#include <Exception/Exception.h>
#include <Metadata/Metadata_cpp.h>
#include <core/CoreSet.h>
#include <rpc/RpcFileSystemClient.h>

#include <memory>
namespace doodle {
actn_down_paths::actn_down_paths()
    : p_tran() {
  p_name = "下载文件";
}
long_term_ptr actn_down_paths::run(const MetadataPtr& in_data, const MetadataPtr& in_parent) {
  auto k_data = sig_get_arg().value();
  if (k_data.is_cancel)
    return p_term;

  if (!in_data)
    throw DoodleError{"选择为空"};
  if (!details::is_class<AssetsFile>(in_data))
    throw DoodleError{"无法转换为 assets file"};

  auto k_ass = std::dynamic_pointer_cast<AssetsFile>(in_data);
  if (k_ass->getPathFile().empty())
    return p_term;

  if (!FSys::exists(k_data.date.parent_path()))
    FSys::create_directories(k_data.date.parent_path());

  auto k_client = CoreSet::getSet().getRpcFileSystemClient();

  auto k_paths = k_ass->getPathFile();

  rpc_trans_path_ptr_list k_list{};
  for (auto& k_item : k_paths) {
    k_list.emplace_back(std::make_unique<rpc_trans_path>(k_data.date / k_item->getServerPath().filename(), k_item->getServerPath()));
  }
  p_tran = k_client->Download(k_list);
  p_term = p_tran->get_term();
  return p_term;
}

}  // namespace doodle
