cmake_minimum_required(VERSION 3.12)

project(drm-howto)

find_package(PkgConfig)
pkg_check_modules(LIBDRM REQUIRED libdrm)

add_executable(modeset-simple modeset-simple.c)
target_include_directories(modeset-simple
	PRIVATE ${LIBDRM_INCLUDE_DIRS}
)
target_link_libraries(modeset-simple
	PRIVATE ${LIBDRM_LIBRARIES}
)

add_executable(modeset modeset.c)
target_include_directories(modeset
	PRIVATE ${LIBDRM_INCLUDE_DIRS}
)
target_link_libraries(modeset
	PRIVATE ${LIBDRM_LIBRARIES}
)

add_executable(modeset-double-buffered modeset-double-buffered.c)
target_include_directories(modeset-double-buffered
	PRIVATE ${LIBDRM_INCLUDE_DIRS}
)
target_link_libraries(modeset-double-buffered
	PRIVATE ${LIBDRM_LIBRARIES}
)

add_executable(modeset-vsync modeset-vsync.c)
target_include_directories(modeset-vsync
	PRIVATE ${LIBDRM_INCLUDE_DIRS}
)
target_link_libraries(modeset-vsync
	PRIVATE ${LIBDRM_LIBRARIES}
)

add_executable(modeset-atomic modeset-atomic.c)
target_include_directories(modeset-atomic
	PRIVATE ${LIBDRM_INCLUDE_DIRS}
)
target_link_libraries(modeset-atomic
	PRIVATE ${LIBDRM_LIBRARIES}
)

install(TARGETS 
	modeset-simple 
	modeset 
	modeset-double-buffered 
	modeset-vsync
	modeset-atomic
RUNTIME DESTINATION bin)
