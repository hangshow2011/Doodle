set(
        DOODLELIB_HEADER
        app/app.h
        client/client.h
        core/ContainerDevice.h
        core/core_set.h
        core/core_sql.h
        core/doodle_lib.h
        core/filesystem_extend.h
        core/observable_container.h
        gui/open_file_dialog.h
        core/program_options.h
        core/static_value.h
        core/tools_setting.h
        core/tree_container.h
        core/ue4_setting.h
        core/util.h
        core/image_loader.h
        core/core_sig.h

        exception/exception.h
        file_warp/image_sequence.h
        file_warp/maya_file.h
        file_warp/opencv_read_player.h
        file_warp/ue4_project.h
        file_warp/video_sequence.h
        gui/base_windwos.h
        gui/setting_windows.h
        gui/widget_register.h
        gui/main_menu_bar.h
        gui/main_status_bar.h
        gui/get_input_dialog.h
        gui/gui_ref/path.h
        gui/gui_ref/project.h
        gui/gui_ref/ref_base.h
        gui/gui_ref/ref_base.cpp

        gui/action/command.h
        gui/action/command_down_file.h
        gui/action/command_files.h 
        gui/action/command_tool.h
        gui/action/command_ue4.h
        gui/action/command_video.h
        gui/widgets/assets_file_widgets.h
        gui/widgets/assets_widget.h
        gui/widgets/edit_widgets.h
        gui/widgets/file_browser.h
        gui/widgets/long_time_tasks_widget.h
        gui/widgets/opencv_player_widget.h
        gui/widgets/project_widget.h
        gui/widgets/time_widget.h
        gui/widgets/tool_box_widget.h
        gui/widgets/screenshot_widget.h
        gui/widgets/drag_widget.h
        gui/widgets/project_edit.h

        lib_warp/WinReg.hpp
        lib_warp/boost_locale_warp.h
        lib_warp/boost_serialization_warp.h
        lib_warp/boost_uuid_warp.h
        lib_warp/cache.hpp
        lib_warp/cache_policy.hpp
        lib_warp/cmrcWarp.h
        lib_warp/entt_warp.h
        lib_warp/fifo_cache_policy.hpp
        lib_warp/imgui_warp.h
        lib_warp/json_warp.h
        lib_warp/lfu_cache_policy.hpp
        lib_warp/lru_cache_policy.hpp
        lib_warp/sqlppWarp.h
        lib_warp/std_warp.h
        logger/LoggerTemplate.h
        logger/logger.h
        metadata/assets.h
        metadata/assets_file.h
        metadata/assets_path.h
        metadata/comment.h
        metadata/episodes.h
        metadata/leaf_meta.h
        metadata/metadata.h
        metadata/metadata_cpp.h
        metadata/metadata_factory.h
        metadata/metadata_state.h
        metadata/project.h
        metadata/season.h
        metadata/shot.h
        metadata/time_point_wrap.h
        metadata/tree_adapter.h
        metadata/user.h
        metadata/image_icon.h
        pin_yin/convert.h

        thread_pool/long_term.h
        thread_pool/thread_pool.h
        toolkit/toolkit.h
        exe_warp/maya_exe.h
        exe_warp/ue4_exe.h
        long_task/image_to_move.h
        long_task/join_move.h
        long_task/process_pool.h
        long_task/restricted_task.h
        long_task/database_task.h
        long_task/drop_file_data.h
        long_task/short_cut.h

        platform/win/drop_manager.h
        platform/win/list_drive.h
        platform/win/windows_alias.h
        platform/win/wnd_proc.h
        core/app_base.h
        doodle_lib_all.h
        doodle_lib_fwd.h
        doodle_lib_pch.h
        doodle_macro.h)

set(
        DOODLELIB_SOURCE
        app/app.cpp
        client/client.cpp
        core/core_set.cpp
        core/core_sql.cpp
        core/doodle_lib.cpp
        core/filesystem_extend.cpp
        gui/open_file_dialog.cpp
        core/program_options.cpp
        core/static_value.cpp
        core/tree_container.cpp
        core/ue4_setting.cpp
        core/util.cpp
        core/image_loader.cpp
        core/core_sig.cpp

        file_warp/image_sequence.cpp
        file_warp/maya_file.cpp
        file_warp/opencv_read_player.cpp
        file_warp/ue4_project.cpp
        file_warp/video_sequence.cpp
        gui/base_windwos.cpp 
        gui/setting_windows.cpp
        gui/widget_register.cpp
        gui/main_menu_bar.cpp
        gui/main_status_bar.cpp
        gui/get_input_dialog.cpp
        gui/gui_ref/path.cpp
        gui/gui_ref/project.cpp
        gui/action/command.cpp
        gui/action/command_down_file.cpp
        gui/action/command_files.cpp
        gui/action/command_tool.cpp
        gui/action/command_ue4.cpp
        gui/action/command_video.cpp
        gui/widgets/assets_file_widgets.cpp
        gui/widgets/assets_widget.cpp
        gui/widgets/edit_widgets.cpp
        gui/widgets/file_browser.cpp
        gui/widgets/long_time_tasks_widget.cpp
        gui/widgets/opencv_player_widget.cpp
        gui/widgets/project_widget.cpp
        gui/widgets/time_widget.cpp
        gui/widgets/tool_box_widget.cpp
        gui/widgets/screenshot_widget.cpp
        gui/widgets/drag_widget.cpp
        gui/widgets/project_edit.cpp

        lib_warp/entt_warp.cpp
        lib_warp/imgui_warp.cpp
        logger/logger.cpp
        metadata/assets.cpp
        metadata/assets_file.cpp
        metadata/assets_path.cpp
        metadata/comment.cpp
        metadata/episodes.cpp
        metadata/leaf_meta.cpp
        metadata/metadata.cpp
        metadata/metadata_factory.cpp
        metadata/project.cpp
        metadata/season.cpp
        metadata/shot.cpp
        metadata/time_point_wrap.cpp
        metadata/user.cpp
        metadata/image_icon.cpp
        pin_yin/convert.cpp

        thread_pool/long_term.cpp
        toolkit/toolkit.cpp
        exe_warp/maya_exe.cpp
        exe_warp/ue4_exe.cpp
        long_task/image_to_move.cpp
        long_task/join_move.cpp
        long_task/process_pool.cpp
        long_task/restricted_task.cpp
        long_task/database_task.cpp
        long_task/drop_file_data.cpp
        long_task/short_cut.cpp

        platform/win/drop_manager.cpp
        platform/win/list_drive.cpp
        platform/win/wnd_proc.cpp
        core/app_base.cpp 
        doodle_lib_all.cpp)
