//
// Created by TD on 2022/4/1.
//

#pragma once
#include <doodle_core/doodle_core_fwd.h>
#include <bitset>
#include <utility>
#include <doodle_core/metadata/time_point_wrap.h>
#include <doodle_core/metadata/rules.h>

#include <doodle_core/lib_warp/boost_icl_warp.h>

#include <boost/icl/split_interval_set.hpp>
#include <boost/icl/discrete_interval.hpp>
#include <boost/icl/interval_map.hpp>
#include <boost/icl/interval_set.hpp>
#include <doodle_core/metadata/detail/boost_lcl_time_point_adaptation.h>
namespace doodle {

namespace business {

class DOODLE_CORE_EXPORT work_clock {
  rules rules_;
  using time_type              = doodle::time_point_wrap;
  using duration_type          = doodle::time_point_wrap::duration;
  using info_type              = std::set<std::string>;
  using discrete_interval_time = boost::icl::discrete_interval<time_type>;
  using interval_set_time      = boost::icl::interval_set<time_type>;
  using interval_map_time      = boost::icl::interval_map<time_type, info_type>;

  void gen_rules_(const discrete_interval_time& in_time);
  void generate_interval_map_time_(const discrete_interval_time& in_time);
  interval_set_time interval_set_time_;
  interval_map_time interval_map_time_;

 public:
  work_clock();

  void set_rules(const rules& in_rules);
  /**
   * @brief 设置工作时间时钟的开始和结束(缓存)
   * @param in_min
   * @param in_max
   */
  void set_interval(const time_type& in_min,
                    const time_type& in_max);

  /**
   * @brief 获取两个时间点点工作时间(按照规则获取)
   * @param in_min 开始时间
   * @param in_max 结束时间
   * @return 工作时间
   */
  duration_type operator()(const time_type& in_min,
                                  const time_type& in_max) const;

  /**
   * @brief 根据传入的开始时间和工作时间段获取下一个时间点
   * @param in_begin 开始时间
   * @param in_du 时间段
   * @return 下一个时间点
   */
  time_type next_time(const time_type& in_begin,
                      const time_type::duration& in_du) const;

  /**
   * @brief 获取两个时间点之间点时间分段( 休息时间段 -> 工作时间段)
   * @param in_min 开始时间
   * @param in_max 结束时间
   * @return 时间段
   */
  [[nodiscard("")]] std::vector<std::pair<time_type, time_type>> get_work_du(
      const time_type& in_min,
      const time_type& in_max) const;

  /**
   * @brief 获取当前点所在时间段段的备注
   * @param in_time 时间点
   * @return 可选段备注
   */
  std::optional<std::string> get_time_info(
      const time_type& in_min,
      const time_type& in_max);

  std::string debug_print();
};
}  // namespace business
}  // namespace doodle
