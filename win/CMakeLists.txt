if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /MP /W4 /wd4201)
  target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
  set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  file(GLOB CPP "*.cpp")
  file(GLOB H "*.h")
  file(GLOB WIN_CPP "win/*.cpp")
  file(GLOB WIN_H "win/*.h")
  source_group("sources" FILES ${CPP} ${H})
  source_group("sources\\win" FILES ${WIN_CPP} ${WIN_H} win/CMakeLists.txt win/resources.rc)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ./win)

target_sources(${PROJECT_NAME} PRIVATE
  win/capture.cpp
  win/gl_context.cpp
  win/native_stuff.cpp
  win/native_window.cpp
  win/CMakeLists.txt
  win/resources.rc
  ${CPP} ${H} ${WIN_CPP} ${WIN_H})

target_link_libraries(${PROJECT_NAME} PRIVATE
  opengl32
  mfuuid
  mfplat
  mf
  mfreadwrite)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION . RENAME mrain.sCr)
install(FILES ${PROJECT_SOURCE_DIR}/win/icon.ico DESTINATION .)

set(CPACK_GENERATOR "WIX")
set(CPACK_WIX_UPGRADE_GUID "E291E6FB-4650-4DB0-BA00-97448A65D9AA")
set(CPACK_WIX_PRODUCT_ICON "${PROJECT_SOURCE_DIR}/win/icon.ico")
set(CPACK_WIX_LICENSE_RTF "${PROJECT_SOURCE_DIR}/win/license.rtf")
set(CPACK_WIX_EXTENSIONS "WixUIExtension;WiXUtilExtension")
set(CPACK_WIX_PATCH_FILE "${PROJECT_SOURCE_DIR}/win/wix_patch.xml")