add_subdirectory(get)

target_sources(
        doodle_lib
        PUBLIC
        computer.h
        task_info.h
        task_server.h
        http_snapshot.h
        file_exists.h

        PRIVATE
        computer.cpp
        task_info.cpp
        task_server.cpp
        http_snapshot.cpp
        file_exists.cpp
)