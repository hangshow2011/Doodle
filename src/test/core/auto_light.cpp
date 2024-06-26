//
// Created by TD on 2023/11/23.
//

#include <doodle_core/database_task/sqlite_client.h>
#include <doodle_core/lib_warp/boost_fmt_error.h>
#include <doodle_core/metadata/assets_file.h>
#include <doodle_core/metadata/episodes.h>
#include <doodle_core/metadata/file_association.h>
#include <doodle_core/metadata/main_map.h>
#include <doodle_core/metadata/shot.h>

#include <doodle_app/app/app_command.h>

#include <doodle_lib/core/down_auto_light_anim_file.h>
#include <doodle_lib/core/up_auto_light_file.h>
#include <doodle_lib/doodle_lib_all.h>
#include <doodle_lib/exe_warp/import_and_render_ue.h>
#include <doodle_lib/exe_warp/maya_exe.h>
#include <doodle_lib/exe_warp/ue_exe.h>

#include <boost/asio.hpp>

constexpr auto l_data = R"([
{
    "out_file": "D:/test_files/test_anim_11_20/fbx/LQ_ep092_sc089/LQ_ep092_sc089_Ch426A_rig_wxc_1001-1013.fbx",
    "ref_file": "C:/sy/LianQiShiWanNian_8/6-moxing/Ch/JDCh_05/Ch426A/Rig/Ch426A_rig_wxc.ma"
},
{
    "out_file": "",
    "ref_file": "C:/sy/LianQiShiWanNian_8/6-moxing/BG/JD_05/BG027C/Mod/YuDaoZong_TingYuan_Low.ma"
},
{
    "out_file": "D:/test_files/test_anim_11_20/fbx/LQ_ep092_sc089/LQ_ep092_sc089_camera_1001-1013.fbx",
    "ref_file": ""
}
])";

class maya_exe_test : public doodle::maya_exe {
 public:
  maya_exe_test()           = default;
  ~maya_exe_test() override = default;

 private:
  void queue_up(
      const entt::handle& in_msg, const std::string_view& in_key,
      const std::shared_ptr<doodle::maya_exe_ns::arg>& in_arg, call_fun_type in_call_fun,
      const std::function<void(doodle::maya_exe_ns::maya_out_arg)>& in_set_arg_fun
  ) override {
    if (!doodle::FSys::exists("D:/test_files/test_anim_11_20/fbx/LQ_ep092_sc089/")) {
      doodle::FSys::create_directories("D:/test_files/test_anim_11_20/fbx/LQ_ep092_sc089/");
    }
    doodle::FSys::copy(
        "D:/test_files/test_ue_auto_main/LQ_ep092_sc089", "D:/test_files/test_anim_11_20/fbx/LQ_ep092_sc089/",
        doodle::FSys::copy_options::overwrite_existing
    );

    in_call_fun->ec_ = boost::system::error_code{};
    doodle::maya_exe_ns::maya_out_arg l_arg{};
    l_arg.begin_time = 1001;
    l_arg.end_time   = 1013;
    l_arg.out_file_list.emplace_back(
        "D:/test_files/test_anim_11_20/fbx/LQ_ep092_sc089/LQ_ep092_sc089_Ch426A_rig_wxc_1001-1013.fbx",
        "C:/sy/LianQiShiWanNian_8/6-moxing/Ch/JDCh_05/Ch426A/Rig/Ch426A_rig_wxc.ma"
    );
    l_arg.out_file_list.emplace_back(
        "D:/test_files/test_anim_11_20/fbx/LQ_ep092_sc089/LQ_ep092_sc089_camera_1001-1013.fbx", ""
    );
    l_arg.out_file_list.emplace_back(
        "", "C:/sy/LianQiShiWanNian_8/6-moxing/BG/JD_05/BG027C/Mod/YuDaoZong_TingYuan_Low.ma"
    );

    in_set_arg_fun(l_arg);

    in_call_fun->complete();
  }
};

class ue_exe_test : public doodle::ue_exe {
 public:
  ue_exe_test()           = default;
  ~ue_exe_test() override = default;

 private:
  void queue_up(const entt::handle& in_msg, const std::string& in_command_line, call_fun_type in_call_fun) override {
    doodle::log_info(fmt::format("ue_exe_test {}", in_command_line));
    static std::filesystem::path l_path{"D:/doodle/cache/ue/TE/YuDaoZong_TingYuan/Saved/MovieRenders/Ep_0092_sc_0089"};
    if (!doodle::FSys::exists(l_path)) {
      doodle::FSys::create_directories(l_path);
    }
    doodle::FSys::copy("C:/Users/TD/Pictures/Screenshots", l_path, doodle::FSys::copy_options::overwrite_existing);
    in_call_fun->ec_ = boost::system::error_code{};
    in_call_fun->complete();
  }
};

namespace doodle {

void next_time_run(
    boost::asio::deadline_timer::duration_type in_time,
    boost::asio::any_completion_handler<void(boost::system::error_code)> in_fun
) {
  auto l_time = std::make_shared<boost::asio::deadline_timer>(g_io_context());
  l_time->expires_from_now(in_time);
  l_time->async_wait([l_time, l_fun = std::move(in_fun)](const boost::system::error_code& in_ec) mutable {
    l_fun(in_ec);
  });
}

void test_fun4() {
  auto l_ue_msg = entt::handle{*g_reg(), g_reg()->create()};
  const FSys::path l_update_path{"D:/test_files/test_ue_auto_main/result"};
  l_ue_msg.emplace<process_message>("测试");
  l_ue_msg.emplace<episodes>(92);
  l_ue_msg.emplace<shot>(89, shot::shot_ab_enum::None);
  l_ue_msg.emplace<project>("测试", l_update_path, "test", "TE", "");
  import_and_render_ue l_import_and_render_ue{l_ue_msg};
  down_auto_light_anim_file::down_info l_down_info{};
  l_down_info.render_project_ =
      "C:/sy/LianQiShiWanNian_8/6-moxing/Ch/JDCh_05/Ch426A/BuJiaBan_UE5/BuJiaBan_UE5.uproject";
  l_import_and_render_ue({}, l_down_info);
}

void test_fun3(boost::system::error_code in_code) {
  test_fun4();
  auto l_maya_exe = g_ctx().get<maya_exe_ptr>();
  auto k_arg      = maya_exe_ns::export_fbx_arg{};
  const FSys::path l_update_path{"D:/test_files/test_ue_auto_main/result"};
  const FSys::path l_file_path{"D:/test_files/test_ue_auto_main/LQ_ep092_sc089.ma"};

  k_arg.file_path        = l_file_path;
  k_arg.export_anim_time = g_reg()->ctx().get<project_config::base_config>().export_anim_time;
  k_arg.project_         = g_ctx().get<database_n::file_translator_ptr>()->get_project_path();
  auto l_ue_msg          = entt::handle{*g_reg(), g_reg()->create()};
  l_ue_msg.emplace<process_message>("测试");
  l_ue_msg.emplace<episodes>(92);
  l_ue_msg.emplace<shot>(89, shot::shot_ab_enum::None);
  l_ue_msg.emplace<project>("测试", l_update_path, "test", "TE", "");

  down_auto_light_anim_file l_down_anim_file{l_ue_msg};
  import_and_render_ue l_import_and_render_ue{l_ue_msg};
  up_auto_light_anim_file l_up_auto_light_file{l_ue_msg};
  l_up_auto_light_file.async_end(boost::asio::bind_executor(
      g_io_context(), [](boost::system::error_code, std::filesystem::path) { app_base::Get().stop_app(); }
  ));
  l_import_and_render_ue.async_end(boost::asio::bind_executor(g_io_context(), std::move(l_up_auto_light_file)));
  l_down_anim_file.async_down_end(boost::asio::bind_executor(g_io_context(), std::move(l_import_and_render_ue)));

  l_maya_exe->async_run_maya(l_ue_msg, k_arg, boost::asio::bind_executor(g_io_context(), std::move(l_down_anim_file)));
}

void test_fun2(boost::system::error_code in_code) {
  auto l_handle{entt::handle{*g_reg(), g_reg()->create()}};
  l_handle.emplace<database>();
  l_handle.emplace<assets_file>().path_attr("C:/sy/LianQiShiWanNian_8/6-moxing/Ch/JDCh_05/Ch426A/Rig/Ch426A_rig_wxc.ma"
  );
  FSys::software_flag_file(l_handle.get<assets_file>().path_attr(), l_handle.get<database>().uuid());
  log_info(fmt::format("Ch426A_rig_wxc uuid {}", l_handle.get<database>().uuid()));

  auto l_handle_ue{entt::handle{*g_reg(), g_reg()->create()}};
  l_handle_ue.emplace<database>();
  l_handle_ue.emplace<assets_file>().path_attr(
      "C:/sy/LianQiShiWanNian_8/6-moxing/Ch/JDCh_05/Ch426A/BuJiaBan_UE5/Content/Character/BuJiaBan/Mesh/BuJiaBan.uasset"
  );
  l_handle_ue.emplace<ue_main_map>(
      "C:/sy/LianQiShiWanNian_8/6-moxing/Ch/JDCh_05/Ch426A/BuJiaBan_UE5/BuJiaBan_UE5.uproject"
  );
  l_handle_ue.emplace<character_id>();
  FSys::software_flag_file(l_handle_ue.get<assets_file>().path_attr(), l_handle_ue.get<database>().uuid());
  log_info(fmt::format("BuJiaBan_UE5 uuid {}", l_handle_ue.get<database>().uuid()));

  auto l_handle2{entt::handle{*g_reg(), g_reg()->create()}};
  l_handle2.emplace<database>();
  l_handle2.emplace<assets_file>().path_attr(
      "C:/sy/LianQiShiWanNian_8/6-moxing/BG/JD_05/BG027C/Mod/YuDaoZong_TingYuan_Low.ma"
  );
  FSys::software_flag_file(l_handle2.get<assets_file>().path_attr(), l_handle2.get<database>().uuid());
  log_info(fmt::format("YuDaoZong_TingYuan_Low uuid {}", l_handle2.get<database>().uuid()));

  auto l_handle_ue2{entt::handle{*g_reg(), g_reg()->create()}};
  l_handle_ue2.emplace<database>();
  l_handle_ue2.emplace<assets_file>().path_attr(
      R"(C:/sy/LianQiShiWanNian_8/6-moxing/BG/JD_05/BG027C/YuDaoZong_TingYuan/Content/YuDaoZong_TingYuan/Map/YuDaoZong_TingYuan.umap)"
  );
  l_handle_ue2.emplace<scene_id>();

  l_handle_ue2.emplace<ue_main_map>(
      R"(C:/sy/LianQiShiWanNian_8/6-moxing/BG/JD_05/BG027C/YuDaoZong_TingYuan/YuDaoZong_TingYuan.uproject)"
  );
  FSys::software_flag_file(l_handle_ue2.get<assets_file>().path_attr(), l_handle_ue2.get<database>().uuid());
  log_info(fmt::format("YuDaoZong_TingYuan uuid {}", l_handle_ue2.get<database>().uuid()));

  auto l_handle3{entt::handle{*g_reg(), g_reg()->create()}};
  l_handle3.emplace<database>();
  l_handle3.emplace<file_association>().maya_file = l_handle;
  l_handle3.patch<file_association>().ue_file     = l_handle_ue;

  auto l_handle4{entt::handle{*g_reg(), g_reg()->create()}};
  l_handle4.emplace<database>();
  l_handle4.emplace<file_association>().maya_file = l_handle2;
  l_handle4.patch<file_association>().ue_file     = l_handle_ue2;

  l_handle.emplace<file_association_ref>(l_handle3);
  l_handle_ue.emplace<file_association_ref>(l_handle3);

  l_handle2.emplace<file_association_ref>(l_handle4);
  l_handle_ue2.emplace<file_association_ref>(l_handle4);
  doodle::test_fun3({});
}

void test_fun(boost::system::error_code in_code) {
  g_ctx().emplace<maya_exe_ptr>() = std::make_shared<maya_exe_test>();
  g_ctx().emplace<ue_exe_ptr>()   = std::make_shared<ue_exe_test>();

  g_ctx().emplace<database_n::file_translator_ptr>()->set_only_open(true).async_open(
      "E:/cache/doodle_main2.doodle_db", false, true, g_reg(), [](auto&&) {}
  );
  next_time_run({0, 0, 1}, doodle::test_fun2);
}
}  // namespace doodle

int core_auto_light(int argc, char* argv[]) {
  using main_app = doodle::app_command<>;
  main_app l_app{argc, argv};
  doodle::next_time_run({0, 0, 1}, doodle::test_fun);

  return l_app.run();
}
