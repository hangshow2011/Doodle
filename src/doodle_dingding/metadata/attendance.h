//
// Created by TD on 2022/9/16.
//

#pragma once

#include <doodle_dingding/doodle_dingding_fwd.h>
#include <doodle_core/core/chrono_.h>
#include <doodle_core/metadata/time_point_wrap.h>
#include <nlohmann/json.hpp>

namespace doodle::dingding {

class DOODLE_DINGDING_API approve_for_open {
 public:
  std::string duration_unit;
  std::string duration;
  std::string sub_type;
  std::string tag_name;
  std::string procInst_id;
  time_point_wrap begin_time;
  std::int32_t biz_type;
  time_point_wrap end_time;
  time_point_wrap gmt_finished;
};

class DOODLE_DINGDING_API attendance_result {
 public:
  std::int32_t record_id;
  std::string source_type;
  time_point_wrap plan_check_time;
  std::int32_t class_id;
  std::string location_method;
  std::string location_result;
  std::string outside_remark;
  std::int32_t plan_id;
  std::string user_address;
  std::int32_t group_id;
  time_point_wrap user_check_time;
  std::string procInst_id;
  std::string check_type;
  std::string time_result;
};

class DOODLE_DINGDING_API attendance_record {
 public:
  std::int64_t record_id;
  std::string user_check_time;
  bool valid_matched;
  std::string user_accuracy;
  std::string source_type;
  std::string invalid_record_msg;
  std::string invalid_record_type;
  std::string user_longitude;
  std::string user_latitude;
};

class DOODLE_DINGDING_API attendance {
 public:

  time_point_wrap date;
  std::vector<attendance_result> attendance_result_list;
  std::int32_t userid;
  std::vector<approve_for_open> approve_list;
  std::string corp_id;
  std::vector<attendance_record> check_record_list;
};

}  // namespace doodle::dingding
