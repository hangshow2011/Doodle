//
// Created by TD on 2023/12/21.
//

#include "connect_video.h"

#include <doodle_core/core/core_set.h>
#include <doodle_core/metadata/episodes.h>
#include <doodle_core/metadata/shot.h>
#include <doodle_core/metadata/user.h>

#include "opencv2/core.hpp"
#include <opencv2/freetype.hpp>
#include <opencv2/opencv.hpp>
namespace doodle::detail {

FSys::path connect_video_t::create_out_path(const entt::handle &in_handle) {
  boost::ignore_unused(this);

  FSys::path l_out{};
  l_out = in_handle.get<out_file_path>().path;

  /// \brief 这里我们检查 shot，episode 进行路径的组合
  if (!l_out.has_extension() && in_handle.any_of<episodes>())
    l_out /=
        fmt::format("{}.mp4", in_handle.any_of<episodes>() ? fmt::to_string(in_handle.get<episodes>()) : "eps_none"s);
  else if (!l_out.has_extension()) {
    l_out /= fmt::format("{}.mp4", core_set::get_set().get_uuid());
  } else
    l_out.extension().replace_extension(".mp4");

  if (exists(l_out.parent_path())) create_directories(l_out.parent_path());
  in_handle.replace<out_file_path>(l_out);
  return l_out;
}

void connect_video_t::connect_video(
    const FSys::path &in_out_path, doodle::logger_ptr in_logger, const std::vector<FSys::path> &in_vector
) {
  boost::ignore_unused(this);

  in_logger->log(log_loc(), level::info, "开始创建视频 {}", in_out_path);
  in_logger->log(log_loc(), level::info, "获得视屏路径 {}", in_vector);
  std::atomic_bool l_stop{};

  const static cv::Size k_size{1920, 1080};
  auto video = cv::VideoWriter{in_out_path.generic_string(), cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 25, k_size};
  auto l_video_cap       = cv::VideoCapture{};
  const auto &k_size_len = in_vector.size();
  cv::Mat l_image{};
  for (auto &l_video : in_vector) {
    if (l_stop) {
      in_logger->log(log_loc(), level::off, fmt::to_string(process_message::fail));

      auto k_str = fmt::format("连接视频被主动结束 合成视频文件将被主动删除");
      in_logger->log(log_loc(), level::warn, k_str);

      try {
        remove(in_out_path);
      } catch (const FSys::filesystem_error &err) {
        in_logger->log(log_loc(), level::warn, "合成视频主动删除失败 {} ", boost::diagnostic_information(err));
      }
      return;
    }

    in_logger->log(log_loc(), level::info, "开始读取视屏 {}", l_video);

    l_video_cap.open(l_video.generic_string());
    if (!l_video_cap.isOpened()) {
      in_logger->log(log_loc(), level::warn, "视屏读取失败 跳过 {}", l_video);
      continue;
    }

    while (l_video_cap.read(l_image)) {
      if (l_image.empty()) {
        DOODLE_LOG_ERROR("{} 视屏读取失败 跳过", l_video);
        continue;
      }
      if (l_image.cols != k_size.width || l_image.rows != k_size.height) cv::resize(l_image, l_image, k_size);

      in_logger->log(log_loc(), level::info, "开始写入图片 {}", l_video);
      video << l_image;
    }
  }

  in_logger->log(log_loc(), level::info, "成功完成任务");
  in_logger->log(log_loc(), level::off, fmt::to_string(process_message::success));
}

}  // namespace doodle::detail