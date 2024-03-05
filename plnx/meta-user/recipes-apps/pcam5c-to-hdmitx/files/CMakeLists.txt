cmake_minimum_required(VERSION 3.12)

project(pcam5c-to-hdmitx)

find_package(PkgConfig)
pkg_check_modules(LIBDRM REQUIRED libdrm)

add_executable(${PROJECT_NAME} main.c v4l2.c drm.c)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${LIBDRM_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE ${LIBDRM_LIBRARIES}
	PRIVATE mediactl;v4l2subdev
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
