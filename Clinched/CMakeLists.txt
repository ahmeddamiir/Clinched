cmake_minimum_required(VERSION 3.16)

project(Clinched VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appClinched
    main.cpp
)

qt_add_qml_module(appClinched
    URI Clinched
    QML_FILES
        QML_FILES/Main.qml
        QML_FILES/DietView.qml
)

set_target_properties(appClinched PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appClinched
    PRIVATE Qt6::Quick Qt6::QuickControls2
)

install(TARGETS appClinched
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)