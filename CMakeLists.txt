# SPDX-License-Identifier: MPL-2.0
cmake_minimum_required(VERSION 3.21)
project(KetQt VERSION 0.1.0 DESCRIPTION "Két Design System components for Qt" LANGUAGES CXX)

option(KETQT_BUILD_WIDGETS "Build KetQt::Widgets" ON)
option(KETQT_BUILD_QUICK "Build the Ket QML module (KetQt::Quick)" ON)
option(KETQT_BUILD_TESTS "Build KetQt tests" ${PROJECT_IS_TOP_LEVEL})
option(KETQT_BUILD_EXAMPLES "Build the KetQt kitchensink" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 6.8 is the oldest Qt a consumer ships (KétChat); KetPlus uses 6.11.
set(ketqt_qt_components Core Gui Svg)
if(KETQT_BUILD_WIDGETS)
    list(APPEND ketqt_qt_components Widgets)
endif()
if(KETQT_BUILD_QUICK)
    list(APPEND ketqt_qt_components Qml Quick QuickControls2)
endif()
if(KETQT_BUILD_TESTS)
    list(APPEND ketqt_qt_components Test)
endif()
if(KETQT_BUILD_EXAMPLES AND KETQT_BUILD_WIDGETS AND KETQT_BUILD_QUICK)
    list(APPEND ketqt_qt_components QuickWidgets)
endif()
find_package(Qt6 6.8 REQUIRED COMPONENTS ${ketqt_qt_components})
qt_standard_project_setup(REQUIRES 6.8)

function(ketqt_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Tokens: generated design-system values, ket::Appearance and the shared component recipes.
add_library(ketqt_tokens STATIC
    src/tokens/KetTokens.h src/tokens/KetTokens.cpp
    src/tokens/KetAppearance.h src/tokens/KetAppearance.cpp
    src/tokens/KetRecipes.h src/tokens/KetRecipes.cpp
    src/tokens/KetOverlayRecipes.cpp
    src/tokens/KetProfile.h src/tokens/KetProfile.cpp
)
add_library(KetQt::Tokens ALIAS ketqt_tokens)
target_include_directories(ketqt_tokens PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/tokens>"
    "$<INSTALL_INTERFACE:include/KetQt>"
)
target_link_libraries(ketqt_tokens PUBLIC Qt6::Core Qt6::Gui)
set_target_properties(ketqt_tokens PROPERTIES EXPORT_NAME Tokens)
ketqt_warnings(ketqt_tokens)

# Icons: Két Symbol SVGs (CC BY 4.0) compiled in, with the size and weight rules.
qt_add_resources(ketqt_icon_resources src/icons/generated/ket-symbol.qrc)
add_library(ketqt_icons STATIC
    src/icons/KetIcons.h src/icons/KetIcons.cpp ${ketqt_icon_resources}
)
add_library(KetQt::Icons ALIAS ketqt_icons)
target_include_directories(ketqt_icons PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/icons>"
    "$<INSTALL_INTERFACE:include/KetQt>"
)
target_link_libraries(ketqt_icons PUBLIC Qt6::Gui PRIVATE Qt6::Svg)
set_target_properties(ketqt_icons PROPERTIES EXPORT_NAME Icons)
ketqt_warnings(ketqt_icons)

if(KETQT_BUILD_WIDGETS)
    add_library(ketqt_widgets STATIC
        src/widgets/KetStyle.h src/widgets/KetStyle.cpp
        src/widgets/KetButton.h src/widgets/KetButton.cpp
        src/widgets/KetBadge.h src/widgets/KetBadge.cpp
        src/widgets/KetTag.h src/widgets/KetTag.cpp
        src/widgets/KetIcon.h src/widgets/KetIcon.cpp
        src/widgets/KetTextField.h src/widgets/KetTextField.cpp
        src/widgets/KetSelect.h src/widgets/KetSelect.cpp
        src/widgets/KetToggle.h src/widgets/KetToggle.cpp
        src/widgets/KetChoice.h src/widgets/KetChoice.cpp
        src/widgets/KetFieldRow.h src/widgets/KetFieldRow.cpp
        src/widgets/KetBanner.h src/widgets/KetBanner.cpp
        src/widgets/KetToast.h src/widgets/KetToast.cpp
        src/widgets/KetDialog.h src/widgets/KetDialog.cpp
        src/widgets/KetMenuPainter.h src/widgets/KetMenuPainter.cpp
        src/widgets/KetShadow.h
    )
    add_library(KetQt::Widgets ALIAS ketqt_widgets)
    target_include_directories(ketqt_widgets PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/widgets>"
        "$<INSTALL_INTERFACE:include/KetQt>"
    )
    target_link_libraries(ketqt_widgets PUBLIC KetQt::Tokens KetQt::Icons Qt6::Widgets)
    set_target_properties(ketqt_widgets PROPERTIES EXPORT_NAME Widgets)
    ketqt_warnings(ketqt_widgets)
endif()

if(KETQT_BUILD_QUICK)
    # Static QML module `Ket`. A consumer links KetQt::Quick and KetQt::QuickPlugin and adds
    # Q_IMPORT_QML_PLUGIN(KetPlugin) once.
    qt_add_library(ketqt_quick STATIC)
    qt_add_qml_module(ketqt_quick
        URI Ket
        VERSION 1.0
        PLUGIN_TARGET ketqt_quickplugin
        OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/qml/Ket
        CLASS_NAME KetPlugin
        SOURCES src/quick/KetTheme.h src/quick/KetTheme.cpp
        QML_FILES src/quick/qml/KetButton.qml src/quick/qml/KetIconButton.qml
                  src/quick/qml/KetBadge.qml src/quick/qml/KetTag.qml src/quick/qml/KetIcon.qml
                  src/quick/qml/KetTextField.qml src/quick/qml/KetSelect.qml
                  src/quick/qml/KetToggleIndicator.qml src/quick/qml/KetCheckbox.qml
                  src/quick/qml/KetRadio.qml src/quick/qml/KetChoice.qml
                  src/quick/qml/KetFieldRow.qml src/quick/qml/KetShadow.qml
                  src/quick/qml/KetBanner.qml src/quick/qml/KetToast.qml
                  src/quick/qml/KetDialog.qml src/quick/qml/KetMenu.qml
                  src/quick/qml/KetMenuItem.qml src/quick/qml/KetMenuSeparator.qml
                  src/quick/qml/KetToolTip.qml
        RESOURCE_PREFIX /qt/qml
    )
    foreach(file KetButton KetIconButton KetBadge KetTag KetIcon KetTextField KetSelect
                 KetToggleIndicator KetCheckbox KetRadio KetChoice KetFieldRow KetShadow
                 KetBanner KetToast KetDialog KetMenu KetMenuItem KetMenuSeparator KetToolTip)
        set_source_files_properties(src/quick/qml/${file}.qml PROPERTIES QT_RESOURCE_ALIAS ${file}.qml)
    endforeach()
    add_library(KetQt::Quick ALIAS ketqt_quick)
    add_library(KetQt::QuickPlugin ALIAS ketqt_quickplugin)
    target_include_directories(ketqt_quick PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/quick>"
    )
    target_link_libraries(ketqt_quick PUBLIC KetQt::Tokens KetQt::Icons Qt6::Qml Qt6::Quick Qt6::QuickControls2)
    ketqt_warnings(ketqt_quick)
endif()

if(KETQT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(KETQT_BUILD_EXAMPLES AND KETQT_BUILD_WIDGETS AND KETQT_BUILD_QUICK)
    add_subdirectory(examples/kitchensink)
endif()

# Install Tokens and Widgets as a CMake package. The static QML module is consumed with
# add_subdirectory for now (docs/architecture.md, "Consuming KetQt").
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(ketqt_install_targets ketqt_tokens ketqt_icons)
if(KETQT_BUILD_WIDGETS)
    list(APPEND ketqt_install_targets ketqt_widgets)
endif()
install(TARGETS ${ketqt_install_targets} EXPORT KetQtTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/KetQt
)
install(FILES src/tokens/KetTokens.h src/tokens/KetAppearance.h src/tokens/KetRecipes.h
              src/tokens/KetProfile.h
              src/icons/KetIcons.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/KetQt)
if(KETQT_BUILD_WIDGETS)
    install(FILES src/widgets/KetStyle.h src/widgets/KetButton.h src/widgets/KetBadge.h
                  src/widgets/KetTag.h src/widgets/KetIcon.h src/widgets/KetTextField.h
                  src/widgets/KetSelect.h src/widgets/KetToggle.h src/widgets/KetChoice.h
                  src/widgets/KetFieldRow.h src/widgets/KetBanner.h src/widgets/KetToast.h
                  src/widgets/KetDialog.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/KetQt)
endif()
install(EXPORT KetQtTargets NAMESPACE KetQt:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KetQt)
configure_package_config_file(cmake/KetQtConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/KetQtConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KetQt)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/KetQtConfigVersion.cmake"
    COMPATIBILITY SameMinorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/KetQtConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KetQtConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/KetQt)
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/KetQt)
install(FILES third_party/ket-design-system/LICENSE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/KetQt RENAME LICENSE.ket-design-system)
install(FILES third_party/ket-symbol/NOTICE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/KetQt RENAME NOTICE.ket-symbol)
install(FILES third_party/ket-symbol/LICENSES/CC-BY-4.0.txt
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/KetQt RENAME LICENSE.ket-symbol)
