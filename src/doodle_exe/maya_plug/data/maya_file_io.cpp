//
// Created by TD on 2021/12/6.
//

#include "maya_file_io.h"

#include <maya/MFileIO.h>
#include <maya/MFileObject.h>
#include <maya_plug/maya_plug_fwd.h>

#include <maya/adskDataAssociations.h>
#include <maya/adskDataStream.h>
#include <maya/adskDebugPrint.h>
namespace doodle::maya_plug {

FSys::path maya_file_io::get_current_path() {
  auto k_s = MFileIO::currentFile();
  return {k_s.asUTF8()};
}
FSys::path maya_file_io::work_path(const FSys::path& in_path) {
  MFileObject k_obj{};

  k_obj.setRawName(d_str{in_path.generic_string()});
  k_obj.setResolveMethod(MFileObject::MFileResolveMethod::kNone);
  return k_obj.resolvedFullName().asUTF8();
}
std::string maya_file_io::get_channel_date() {
  MStatus k_status{};
  adsk::Data::Associations k_meta{MFileIO::metadata(&k_status)};
  DOODLE_CHICK(k_status);
  auto k_channel = k_meta.channel("doodle_sim_json");
  if (!k_channel.empty()) {
    auto k_stream = k_channel.dataStream("json_stream");
    adsk::Data::Handle k_h{k_stream->element(0)};
    if (!k_h.hasData())
      return {};
    return k_h.str(0);
  }
  return {};
}
bool maya_file_io::chick_channel() {
  MStatus k_status{};
  adsk::Data::Associations k_meta{MFileIO::metadata(&k_status)};
  DOODLE_CHICK(k_status);
  auto k_channel = k_meta.channel("doodle_sim_json");
  if (!k_channel.dataStream("json_stream")) {
    auto k_s = adsk::Data::Structure::create();              // 添加结构
    k_s->setName("json_structure");                          // 设置结构名称
    k_s->addMember(adsk::Data::Member::kString, 1, "json");  // 添加结构成员
    adsk::Data::Structure::registerStructure(*k_s);          // 注册结构

    adsk::Data::Stream k_stream{*k_s, "json_stream"};
    k_channel.setDataStream(k_stream);  // 设置流
    MFileIO::setMetadata(k_meta);
  }
  return true;
}
bool maya_file_io::replace_channel_date(const std::string& in_string) {
  chick_channel();
  MStatus k_status{};
  adsk::Data::Associations k_meta{MFileIO::metadata(&k_status)};
  DOODLE_CHICK(k_status);
  auto k_json   = k_meta.channel("doodle_sim_json");  /// 获得元数据通道
  auto k_stream = k_json.dataStream("json_stream");   /// 获得元数据流

  adsk::Data::Handle k_h{k_stream->structure()};

  std::string str_err{};
  DOODLE_LOG_INFO(in_string);
  if (k_h.fromStr(in_string, 0, str_err) != 0)
    DOODLE_LOG_ERROR(str_err);
  k_stream->setElement(0, k_h);
  MFileIO::setMetadata(k_meta);

  adsk::Debug::Print k_p{std::cout};
  decltype(k_meta)::Debug(&k_meta, k_p);
  k_p.endSection();
  return true;
}
bool maya_file_io::save_file(const FSys::path& in_file_path) {
  MStatus k_s{};
  if (!exists(in_file_path.parent_path())) {
    create_directories(in_file_path.parent_path());
  }
  k_s = MFileIO::saveAs(d_str{in_file_path.generic_string()}, nullptr, true);
  DOODLE_CHICK(k_s);
  return false;
}
bool maya_file_io::upload_file(const FSys::path& in_source_path, const FSys::path& in_prefix) {
  chick_true<doodle_error>(FSys::is_regular_file(in_source_path), DOODLE_LOC, "{} 路径不存在或者不是文件");
  auto l_upload_path = g_reg()->ctx().at<project_config::base_config>().get_upload_path();
  l_upload_path /= in_prefix;
  l_upload_path /= maya_file_io::get_current_path().stem();
  if (!FSys::exists(l_upload_path.parent_path()))
    FSys::create_directories(l_upload_path.parent_path());
  FSys::copy_file(in_source_path,
                  l_upload_path / in_source_path.filename(),
                  FSys::copy_option::overwrite_if_exists);
}
}  // namespace doodle::maya_plug
