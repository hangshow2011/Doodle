//
// Created by TD on 2022/8/4.
//

#include "time_rules_render.h"

#include <doodle_core/metadata/rules.h>
#include <doodle_core/metadata/detail/time_point_info.h>

#include <doodle_core/metadata/time_point_wrap.h>
#include <doodle_lib/gui/gui_ref/ref_base.h>

#include <doodle_lib/lib_warp/imgui_warp.h>
#include <boost/asio.hpp>
#include <boost/optional.hpp>

namespace doodle {
namespace gui {
namespace time_sequencer_widget_ns {

namespace {
class work_gui_data {
 public:
  using base_type   = gui_cache<std::array<gui_cache<bool>, 7>>;

  using friend_type = business::rules::work_day_type;

  base_type gui_attr;
  work_gui_data()
      : work_gui_data(business::rules::work_Monday_to_Friday){};
  explicit work_gui_data(const friend_type& in_work_day_type)
      : gui_attr(
            "工作周"s,
            std::array<gui::gui_cache<bool>, 7>{gui::gui_cache<bool>{"星期日"s, in_work_day_type[0]},
                                                gui::gui_cache<bool>{"星期一"s, in_work_day_type[1]},
                                                gui::gui_cache<bool>{"星期二"s, in_work_day_type[2]},
                                                gui::gui_cache<bool>{"星期三"s, in_work_day_type[3]},
                                                gui::gui_cache<bool>{"星期四"s, in_work_day_type[4]},
                                                gui::gui_cache<bool>{"星期五"s, in_work_day_type[5]},
                                                gui::gui_cache<bool>{"星期六"s, in_work_day_type[6]}}){};

  explicit operator friend_type() {
    friend_type l_r{};
    l_r[0] = gui_attr.data[0]();
    l_r[1] = gui_attr.data[1]();
    l_r[2] = gui_attr.data[2]();
    l_r[3] = gui_attr.data[3]();
    l_r[4] = gui_attr.data[4]();
    l_r[5] = gui_attr.data[5]();
    l_r[6] = gui_attr.data[6]();
    return l_r;
  }
};

class time_hh_mm_ss_gui_data : public gui_cache<std::array<std::int32_t, 3>> {
 public:
  using base_type   = gui_cache<std::array<std::int32_t, 3>>;
  //  explicit time_hh_mm_ss_gui_data(std::int32_t in_h,
  //                                  std::int32_t in_m,
  //                                  std::int32_t in_s)
  //      : base_type("时分秒",
  //                  std::array<std::int32_t, 3>{in_h, in_m, in_s}){};

  using friend_type = chrono::seconds;
  time_hh_mm_ss_gui_data()
      : base_type("时分秒",
                  std::array<std::int32_t, 3>{}){

        };
  explicit time_hh_mm_ss_gui_data(const friend_type& in_seconds)
      : time_hh_mm_ss_gui_data() {
    chrono::hh_mm_ss l_hh_mm_ss{in_seconds};
    data[0] = boost::numeric_cast<std::int32_t>(l_hh_mm_ss.hours().count());
    data[1] = boost::numeric_cast<std::int32_t>(l_hh_mm_ss.minutes().count());
    data[2] = boost::numeric_cast<std::int32_t>(l_hh_mm_ss.seconds().count());
  };

  explicit operator friend_type() {
    return chrono::hours{data[0]} + chrono::minutes{data[1]} + chrono::seconds{data[2]};
  }
};
class time_yy_mm_dd_gui_data : public gui_cache<std::array<std::int32_t, 3>> {
 public:
  using base_type = gui_cache<std::array<std::int32_t, 3>>;
  time_yy_mm_dd_gui_data()
      : base_type("年月日"s,
                  std::array<std::int32_t, 3>{}){};
  explicit time_yy_mm_dd_gui_data(const chrono::local_days& in_days)
      : time_yy_mm_dd_gui_data() {
    chrono::year_month_day l_year_month_day{in_days};
    data[0] = boost::numeric_cast<std::int32_t>((std::int32_t)l_year_month_day.year());
    data[1] = boost::numeric_cast<std::int32_t>((std::uint32_t)l_year_month_day.month());
    data[2] = boost::numeric_cast<std::int32_t>((std::uint32_t)l_year_month_day.day());
  };

  explicit operator chrono::local_days() {
    return chrono::local_days{
        chrono::year_month_day{
            chrono::year{data[0]},
            chrono::month{boost::numeric_cast<std::uint32_t>(data[1])},
            chrono::day{boost::numeric_cast<std::uint32_t>(data[2])}}};
  }
};

class time_warp_gui_data
    : public gui_cache<std::pair<time_yy_mm_dd_gui_data, time_hh_mm_ss_gui_data>> {
 public:
  using base_type   = gui_cache<std::pair<time_yy_mm_dd_gui_data, time_yy_mm_dd_gui_data>>;

  using friend_type = time_point_wrap;

  time_warp_gui_data() : time_warp_gui_data(friend_type{}){};
  explicit time_warp_gui_data(const friend_type& in_point_wrap)
      : time_warp_gui_data("时间"s, in_point_wrap){};
  explicit time_warp_gui_data(const std::string& in_string, const friend_type& in_point_wrap) {
    this->gui_name    = gui_cache_name_id{in_string};
    auto&& [l_y, l_s] = in_point_wrap.compose_1();
    data.first        = time_yy_mm_dd_gui_data{l_y};
    data.second       = time_hh_mm_ss_gui_data{l_s};
  };
  explicit operator friend_type() {
    return friend_type{chrono::local_days{data.first} + chrono::seconds{data.second}};
  }
};

class time_info_gui_data
    : public gui_cache<std::pair<time_warp_gui_data, time_warp_gui_data>> {
 public:
  gui_cache<std::string> info;
  gui_cache<bool> delete_node{"删除"s, false};

  using friend_type = ::doodle::business::rules::point_type;

  time_info_gui_data() : time_info_gui_data(friend_type{}) {}
  explicit time_info_gui_data(const friend_type& in_point_type)
      : gui_cache<std::pair<time_warp_gui_data, time_warp_gui_data>>(
            "时间"s),
        info("备注"s, in_point_type.info) {
    data.first  = time_warp_gui_data{"开始时间"s, in_point_type.first};
    data.second = time_warp_gui_data{"结束时间"s, in_point_type.second};
  };

  explicit operator friend_type() {
    return friend_type{
        time_point_wrap{data.first},
        time_point_wrap{data.second},
        info()};
  }
};

class time_work_gui_data : boost::equality_comparable<time_work_gui_data> {
 public:
  time_hh_mm_ss_gui_data begin;
  time_hh_mm_ss_gui_data end;
  gui_cache_name_id name_id_delete{"删除"s};
  time_work_gui_data() = default;

  using friend_type    = std::pair<chrono::seconds, chrono::seconds>;

  explicit time_work_gui_data(const friend_type& in_pair)
      : begin(in_pair.first),
        end(in_pair.second) {}

  explicit operator friend_type() {
    return std::make_pair(friend_type::first_type{begin},
                          friend_type::second_type{end});
  }
  bool operator==(const time_work_gui_data& in_rhs) const {
    return begin == in_rhs.begin &&
           end == in_rhs.end &&
           name_id_delete == in_rhs.name_id_delete;
  }
};

class work_gui_data_render {
 public:
  bool modify{false};
  using gui_data_type = work_gui_data;
  gui_data_type gui_data{};
  bool render() {
    modify = false;
    dear::CollapsingHeader{*gui_data.gui_attr} && [this]() {
      dear::HelpMarker{"按星期去计算工作时间"};
      ranges::for_each(gui_data.gui_attr(), [this](decltype(gui_data.gui_attr().front()) in_value) {
        modify |= ImGui::Checkbox(*in_value, &in_value);
      });
    };
    return modify;
  }
  gui_data_type::friend_type get() {
    return gui_data_type::friend_type{gui_data};
  }

  void set(const gui_data_type::friend_type& in_type) {
    gui_data = gui_data_type{in_type};
  }
};
class time_work_gui_data_render {
 public:
  using gui_data_type = time_work_gui_data;
  std::vector<gui_data_type> gui_data{};
  gui_cache_name_id name_id{};

  gui_cache_name_id name_id_add{"添加"s};
  gui_cache_name_id name_id_delete{"删除"s};
  bool modify{false};

  bool render() {
    modify = false;

    dear::CollapsingHeader{*name_id} && [this]() {
      dear::HelpMarker{"每天的开始和结束时间段"};
      if (imgui::Button(*name_id_add)) {
        boost::asio::post(g_io_context(), [this]() { this->add(); });
      }
      ranges::for_each(gui_data, [this](gui_data_type& in_type) {
        modify |= ImGui::SliderInt3(*in_type.begin, in_type.begin.data.data(), 0, 59);
        modify |= ImGui::SliderInt3(*in_type.end, in_type.end.data.data(), 0, 59);
        if (imgui::Button(*in_type.name_id_delete)) {
          boost::asio::post(g_io_context(), [this, in_type]() {
            this->delete_node(in_type);
          });
        }
      });
    };
    return modify;
  };

  void add() {
    gui_data.emplace_back(gui_data_type{std::make_pair(9h, 12h)});
    this->modify = true;
  }
  void delete_node(const gui_data_type& in_data_type) {
    ranges::remove(gui_data, in_data_type);
    this->modify = true;
  }

  std::vector<gui_data_type::friend_type> get() {
    return gui_data |
           ranges::view::transform([](auto&& in_item) {
             return gui_data_type::friend_type{in_item};
           }) |
           ranges::to_vector;
  }

  void set(const std::vector<gui_data_type::friend_type>& in_type) {
    gui_data = in_type |
               ranges::view::transform([](auto&& in_item) {
                 return gui_data_type{in_item};
               }) |
               ranges::to_vector;
  }
};

class time_info_gui_data_render {
 public:
  using gui_data_type = time_info_gui_data;

  std::vector<time_info_gui_data> gui_data{};

  gui_cache_name_id gui_name{};
  gui_cache_name_id gui_name_add{"添加"s};

  bool modify{false};

  bool render() {
    modify = false;

    dear::CollapsingHeader{*gui_name} && [this]() {
      if (ImGui::Button(*gui_name_add)) {
        boost::asio::post(g_io_context(), [this]() { this->add(); });
      }
      ranges::for_each(gui_data, [this](gui_data_type& in_data) {
        dear::Text(in_data.data.first.gui_name.name);
      });
    };
    return modify;
  }

  void add() {
    gui_data.emplace_back(gui_data_type::friend_type{});
  }

  void delete_node(const gui_data_type& in_data) {
  }
  void set(const std::vector<gui_data_type::friend_type>& in_type) {
    gui_data = in_type |
               ranges::view::transform([](auto&& in_item) {
                 return gui_data_type{in_item};
               }) |
               ranges::to_vector;
  }
  std::vector<gui_data_type::friend_type> get() {
    return gui_data |
           ranges::view::transform([](auto&& in_item) {
             return gui_data_type::friend_type{in_item};
           }) |
           ranges::to_vector;
  }
};

}  // namespace

class time_rules_render::impl {
 public:
  class render_time_rules {
   public:
    work_gui_data_render work_gui_data_attr{};
    time_work_gui_data_render time_work_gui_data_attr{};
    time_info_gui_data_render extra_holidays_attr{};
    time_info_gui_data_render extra_work_attr{};
    time_info_gui_data_render extra_rest_attr{};
  };
  rules_type rules_attr;

  render_time_rules render_time{};

  void set_rules(std::size_t in_size) {
    switch (in_size) {
      case 0ull:
        set_work_gui_data_attr();
        break;
      case 1ull:
        set_time_work_gui_data_attr();
        break;
      case 2ull:
        set_extra_holidays_attr();
        break;
      case 3ull:
        set_extra_work_attr();
        break;
      case 4ull:
        set_extra_rest_attr();
        break;
      default:
        break;
    }
  }

  bool mod{false};

 private:
  void set_work_gui_data_attr() {
    rules_attr.work_weekdays() = render_time.work_gui_data_attr.get();
  }
  void set_time_work_gui_data_attr() {
    rules_attr.work_time() =
        render_time.time_work_gui_data_attr.get();
  }
  void set_extra_holidays_attr() {
    rules_attr.extra_holidays() =
        render_time.extra_holidays_attr.get();
  }
  void set_extra_work_attr() {
    rules_attr.extra_work() =
        render_time.extra_work_attr.get();
  }
  void set_extra_rest_attr() {
    rules_attr.extra_rest() =
        render_time.extra_rest_attr.get();
  }
};

time_rules_render::time_rules_render()
    : p_i(std::make_unique<impl>()) {
  p_i->rules_attr = g_reg()->ctx().emplace<rules_type>();
}
const time_rules_render::rules_type& time_rules_render::rules_attr() const {
  return p_i->rules_attr;
}
void time_rules_render::rules_attr(const time_rules_render::rules_type& in_rules_type) {
  p_i->rules_attr                              = in_rules_type;

  p_i->render_time.work_gui_data_attr.gui_data = work_gui_data{in_rules_type.work_weekdays()};
  p_i->render_time.time_work_gui_data_attr.set(in_rules_type.work_time());
  p_i->render_time.extra_holidays_attr.set(in_rules_type.extra_holidays());
  p_i->render_time.extra_work_attr.set(in_rules_type.extra_work());
  p_i->render_time.extra_rest_attr.set(in_rules_type.extra_rest());
}
bool time_rules_render::render() {
  p_i->mod = false;
  boost::pfr::for_each_field(p_i->render_time, [this](auto&& in, std::size_t in_size) {
    if (in.render()) {
      this->p_i->mod |= true;
      this->p_i->set_rules(in_size);
    }
  });

  return p_i->mod;
}
time_rules_render::~time_rules_render() = default;
}  // namespace time_sequencer_widget_ns
}  // namespace gui
}  // namespace doodle
