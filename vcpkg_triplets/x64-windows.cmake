set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

if (PORT MATCHES "imgui")
    set(VCPKG_CXX_FLAGS "-DUNICODE -D_UNICODE")
endif ()