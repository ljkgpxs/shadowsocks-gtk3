cmake_minimum_required(VERSION 2.6)
project(shadowosocks-gtk3 C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(SS REQUIRED shadowsocks-libev)
include_directories(${GTK3_INCLUDE_DIRS} ${SS_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${SS_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER} ${SS_CFLAGS_OTHER})

add_executable(ss-gtk3 main.c)

target_link_libraries(ss-gtk3 ${GTK3_LIBRARIES} ${SS_LIBRARIES} -pthread)

install(FILES ${CMAKE_BINARY_DIR}/ss-gtk3 DESTINATION /usr/bin/ PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(FILES ss-gtk3.glade DESTINATION /usr/share/shadowsocks-gtk3/)
install(FILES shadowsocks-gtk3.desktop DESTINATION /usr/share/applications/)
install(FILES shadowsocks-gtk3.png DESTINATION /usr/share/icons/hicolor/512x512/apps/)

