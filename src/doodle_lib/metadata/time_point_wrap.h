﻿//
// Created by TD on 2021/5/17.
//

#pragma once
#include <date/date.h>
#include <date/tz.h>
#include <doodle_lib/Exception/exception.h>
#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/metadata/leaf_meta.h>

namespace doodle {

/**
 * @brief 这是一个小的时间类
 * @warning 这个类中的设置时间的函数和都是设置本地日期的，并不是utc时间， 他会自动在内部转换为utc
 */
class DOODLELIB_API time_point_wrap {
 public:
  using time_point       = chrono::sys_time_pos;
  using time_duration    = time_point::duration;
  using time_local_point = chrono::local_time<time_duration>;
  using time_zoned       = chrono::zoned_time<time_duration>;

 private:
 public:
  class DOODLELIB_API gui_data {
   public:
    std::uint16_t year_;
    std::uint16_t month_;
    std::uint16_t day_;
    std::uint16_t hours_;
    std::uint16_t minutes_;
    std::uint16_t seconds_;

    gui_data() : gui_data(time_point_wrap{}) {}
    gui_data(const time_point_wrap& in_wrap) {
      std::tie(year_,
               month_,
               day_,
               hours_,
               minutes_,
               seconds_) = in_wrap.compose();
    }
  };

  chrono::zoned_time<time_duration> zoned_time_;
  time_point_wrap();
  explicit time_point_wrap(const time_zoned& in_time_zoned);
  explicit time_point_wrap(time_point in_utc_timePoint);
  explicit time_point_wrap(time_local_point in_utc_timePoint);
  explicit time_point_wrap(
      std::uint16_t in_year,
      std::uint16_t in_month,
      std::uint16_t in_day,
      std::uint16_t in_hours,
      std::uint16_t in_minutes,
      std::uint16_t in_seconds);
  explicit time_point_wrap(const gui_data& in_data);

  [[nodiscard]] std::tuple<std::uint16_t,  // year
                           std::uint16_t,  // month
                           std::uint16_t,  // day
                           std::uint16_t,  // hours
                           std::uint16_t,  // minutes
                           std::uint16_t>  // seconds
  compose() const;

  [[nodiscard]] std::string get_week_s() const;
  [[nodiscard]] std::int32_t get_week_int() const;

  [[nodiscard]] std::string show_str() const;

  /**
   *
   * @param in 结束的时间
   * @return
   *
   * @todo 时间选项中去除节假日和个人调休
   *
   */
  [[nodiscard]] chrono::hours_double work_duration(const time_point_wrap& in) const;

 private:
  /**
   * 这个是计算开始时到一天结束时的工作时长
   * 通常是安装8小时计算, 同时计算前段时间可以使用 8-return
   * @param in_point  开始的时间点
   * @return 工作时间长度
   *
   * @todo: 这里我们要添加设置， 而不是静态变量
   */
  chrono::hours_double one_day_works_hours(const chrono::local_time<chrono::seconds>& in_point) const;
  chrono::days work_days(const time_point& in_begin, const time_point& in_end) const;

  //这里是序列化的代码

  friend void to_json(nlohmann::json& j, const time_point_wrap& p) {
    j["time"] = p.zoned_time_.get_sys_time();
  }
  friend void from_json(const nlohmann::json& j, time_point_wrap& p) {
    p.zoned_time_ = j.at("time").get<time_point>();
  }
};

// class time_point : public std::chrono::time_point<std::chrono::system_clock> {
//  public:
//   time_point(){
//   };
// }

namespace gui {
template <>
class adl_traits<time_point_wrap> {
 public:
  using gui_data = time_point_wrap::gui_data;
};
}  // namespace gui
}  // namespace doodle
