target_sources(
        doodle_lib
        PUBLIC
        main_gui.h
        auto_light_service.h
        http_distributed_tasks.h
        http_working_service.h
        auto_light_process.h
        file_exists_launch.h

        PRIVATE
        main_gui.cpp
        auto_light_service.cpp
        http_distributed_tasks.cpp
        http_working_service.cpp
        auto_light_process.cpp
        file_exists_launch.cpp
)