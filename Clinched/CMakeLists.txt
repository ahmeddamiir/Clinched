cmake_minimum_required(VERSION 3.19)
project(Clinched LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(Clinched
    WIN32 MACOSX_BUNDLE
    main.cpp
    core/models.h
    core/datamanager.h
    core/datamanager.cpp
    ui/mainwindow.cpp
    ui/mainwindow.h
    ui/diettab.h
    ui/diettab.cpp
    ui/workoutstab.h
    ui/workoutstab.cpp
)

target_include_directories(Clinched PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)
qt_add_resources(Clinched "app_resources"
    PREFIX "/"
    FILES resources/presets.json
)

target_link_libraries(Clinched
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS Clinched
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Clinched
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
