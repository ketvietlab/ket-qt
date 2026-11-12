# SPDX-License-Identifier: MPL-2.0
cmake_minimum_required(VERSION 3.21)
project(KetQt VERSION 0.1.0 DESCRIPTION "Két Design System components for Qt" LANGUAGES CXX)

option(KETQT_BUILD_WIDGETS "Build KetQt::Widgets" ON)
option(KETQT_BUILD_QUICK "Build the Ket QML module (KetQt::Quick)" ON)
option(KETQT_BUILD_TESTS "Build KetQt tests" ${PROJECT_IS_TOP_LEVEL})
option(KETQT_BUILD_EXAMPLES "Build the KetQt kitchensink" ${PROJECT_IS_TOP_LEVEL})
# An app that builds the kit with FetchContent or add_subdirectory links it statically and must
# not install its headers, archives and CMake package into the app's install tree.
option(KETQT_INSTALL "Install KetQt as a CMake package" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 6.8 is the oldest Qt a consumer ships (KétChat); KetPlus uses 6.11.
set(ketqt_qt_components Core Gui Svg)
if(KETQT_BUILD_WIDGETS)
    list(APPEND ketqt_qt_components Widgets PrintSupport)
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
    src/tokens/KetNavigationRecipes.cpp
    src/tokens/KetConversationRecipes.h src/tokens/KetConversationRecipes.cpp
    src/tokens/KetSidebarTreeRecipes.h src/tokens/KetSidebarTreeRecipes.cpp
    src/tokens/KetRichRecipes.h src/tokens/KetRichRecipes.cpp
    src/tokens/KetMarkdown.h src/tokens/KetMarkdown.cpp
    src/tokens/KetSyntax.h src/tokens/KetSyntax.cpp
    src/tokens/KetChatRecipes.h src/tokens/KetChatRecipes.cpp
    src/tokens/KetControlRecipes.h src/tokens/KetControlRecipes.cpp
    src/tokens/KetSurfaceRecipes.h src/tokens/KetSurfaceRecipes.cpp
    src/tokens/KetLightboxRecipes.h src/tokens/KetLightboxRecipes.cpp
    src/tokens/KetProfile.h src/tokens/KetProfile.cpp
    src/tokens/KetProfileDesignSystem.cpp src/tokens/KetProfileKetPlus.cpp
    src/tokens/KetSidebarTreeProfile.h src/tokens/KetSidebarTreeProfile.cpp
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
        src/widgets/KetNativeOverlay.h
        src/widgets/KetNavItem.h src/widgets/KetNavItem.cpp
        src/widgets/KetSegmentedControl.h src/widgets/KetSegmentedControl.cpp
        src/widgets/KetAnimationClock.h src/widgets/KetAnimationClock.cpp
        src/widgets/KetActivityIndicator.h src/widgets/KetActivityIndicator.cpp
        src/widgets/KetEmptyState.h src/widgets/KetEmptyState.cpp
        src/widgets/KetBarPainter.h src/widgets/KetBarPainter.cpp
        src/widgets/KetParagraph.h
        src/widgets/KetConversationRow.h src/widgets/KetConversationRow.cpp
        src/widgets/KetComposer.h src/widgets/KetComposer.cpp
        src/widgets/KetTranscript.h src/widgets/KetTranscript.cpp
        src/widgets/KetActivityRow.h src/widgets/KetActivityRow.cpp
        src/widgets/KetMarkdownView.h src/widgets/KetMarkdownView.cpp
        src/widgets/KetMentionPicker.h src/widgets/KetMentionPicker.cpp
        src/widgets/KetThemeCard.h src/widgets/KetThemeCard.cpp
        src/widgets/KetUpdateCard.h src/widgets/KetUpdateCard.cpp
        src/widgets/KetTaskBar.h src/widgets/KetTaskBar.cpp
        src/widgets/KetDiffView.h src/widgets/KetDiffView.cpp
        src/widgets/KetChangedFiles.h src/widgets/KetChangedFiles.cpp
        src/widgets/KetFileRow.h src/widgets/KetFileRow.cpp
        src/widgets/KetCommandPalette.h src/widgets/KetCommandPalette.cpp
        src/widgets/KetChrome.h src/widgets/KetChrome.cpp
        src/widgets/KetRouteTabs.h src/widgets/KetRouteTabs.cpp
        src/widgets/KetTreeItem.h src/widgets/KetTreeItem.cpp
        src/widgets/KetSidebarTree.h src/widgets/KetSidebarTreeDetail.h
        src/widgets/KetSidebarTreeHeaders.cpp src/widgets/KetSidebarTeamBranch.cpp
        src/widgets/KetSidebarRow.cpp
        src/widgets/KetAvatar.h src/widgets/KetAvatar.cpp
        src/widgets/KetStepSlider.h src/widgets/KetStepSlider.cpp
        src/widgets/KetSliderPainter.h src/widgets/KetSliderPainter.cpp
        src/widgets/KetGallery.h src/widgets/KetGallery.cpp
        src/widgets/KetActivityGroup.h src/widgets/KetActivityGroup.cpp
        src/widgets/KetQuestionCard.h src/widgets/KetQuestionCard.cpp
        src/widgets/KetSuggestion.h src/widgets/KetSuggestion.cpp
        src/widgets/KetAttachmentChip.h src/widgets/KetAttachmentChip.cpp
        src/widgets/KetMeter.h src/widgets/KetMeter.cpp
        src/widgets/KetPopover.h src/widgets/KetPopover.cpp
        src/widgets/KetLightboxButton.h src/widgets/KetLightboxButton.cpp
        src/widgets/KetLightboxThumb.h src/widgets/KetLightboxThumb.cpp
        src/widgets/KetLightbox.h src/widgets/KetLightbox.cpp src/widgets/KetLightboxInput.cpp
        src/widgets/KetPrint.h src/widgets/KetPrint.cpp
    )
    add_library(KetQt::Widgets ALIAS ketqt_widgets)
    target_include_directories(ketqt_widgets PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/widgets>"
        "$<INSTALL_INTERFACE:include/KetQt>"
    )
    target_link_libraries(ketqt_widgets PUBLIC KetQt::Tokens KetQt::Icons Qt6::Widgets
                                            Qt6::PrintSupport)
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
        SOURCES src/quick/KetTheme.h src/quick/KetTheme.cpp src/quick/KetThemeDetail.h src/quick/KetThemeSurfaces.cpp src/quick/KetThemeControls.cpp src/quick/KetThemeChat.cpp src/quick/KetThemeLightbox.cpp src/quick/KetThemeSidebar.cpp
        QML_FILES src/quick/qml/KetButton.qml src/quick/qml/KetIconButton.qml
                  src/quick/qml/KetBadge.qml src/quick/qml/KetTag.qml src/quick/qml/KetIcon.qml
                  src/quick/qml/KetTextField.qml src/quick/qml/KetSelect.qml
                  src/quick/qml/KetToggleIndicator.qml src/quick/qml/KetCheckbox.qml
                  src/quick/qml/KetRadio.qml src/quick/qml/KetChoice.qml
                  src/quick/qml/KetFieldRow.qml src/quick/qml/KetShadow.qml
                  src/quick/qml/KetBanner.qml src/quick/qml/KetToast.qml
                  src/quick/qml/KetDialog.qml src/quick/qml/KetMenu.qml
                  src/quick/qml/KetMenuItem.qml src/quick/qml/KetMenuSeparator.qml
                  src/quick/qml/KetMenuSection.qml
                  src/quick/qml/KetToolTip.qml src/quick/qml/KetNavItem.qml
                  src/quick/qml/KetKbd.qml src/quick/qml/KetSegmentedControl.qml
                  src/quick/qml/KetTabBar.qml src/quick/qml/KetTabButton.qml
                  src/quick/qml/KetProgressBar.qml src/quick/qml/KetActivityIndicator.qml
                  src/quick/qml/KetEmptyState.qml src/quick/qml/KetConversationRow.qml
                  src/quick/qml/KetComposer.qml src/quick/qml/KetReadingText.qml
                  src/quick/qml/KetUserMessage.qml src/quick/qml/KetAssistantMessage.qml
                  src/quick/qml/KetDayDivider.qml src/quick/qml/KetActivityRow.qml
                  src/quick/qml/KetMarkdownText.qml src/quick/qml/KetCodeBlock.qml
                  src/quick/qml/KetMarkdown.qml src/quick/qml/KetMentionPicker.qml
                  src/quick/qml/KetUsageRing.qml
                  src/quick/qml/KetCommandPalette.qml
                  src/quick/qml/KetCommandResult.qml
                  src/quick/qml/KetFileRow.qml
                  src/quick/qml/KetChangedFiles.qml
                  src/quick/qml/KetDiffView.qml
                  src/quick/qml/KetTaskRow.qml
                  src/quick/qml/KetTaskBar.qml
                  src/quick/qml/KetUpdateCard.qml
                  src/quick/qml/KetUpdatePill.qml
                  src/quick/qml/KetThemeCard.qml
                  src/quick/qml/KetAccentSwatch.qml
                  src/quick/qml/KetPaneHeader.qml
                  src/quick/qml/KetBreadcrumb.qml
                  src/quick/qml/KetStatusBar.qml
                  src/quick/qml/KetStatusItem.qml
                  src/quick/qml/KetRouteTabs.qml
                  src/quick/qml/KetGroupHeader.qml
                  src/quick/qml/KetTreeItem.qml
                  src/quick/qml/KetSidebarRepositoryHeader.qml
                  src/quick/qml/KetSidebarTeamBranch.qml
                  src/quick/qml/KetSidebarSectionLabel.qml
                  src/quick/qml/KetSidebarRow.qml
                  src/quick/qml/KetSidebarTreeGuide.qml
                  src/quick/qml/KetAvatar.qml
                  src/quick/qml/KetStepSlider.qml
                  src/quick/qml/KetSlider.qml
                  src/quick/qml/KetGallery.qml
                  src/quick/qml/KetActivityGroup.qml
                  src/quick/qml/KetQuestionCard.qml
                  src/quick/qml/KetSuggestion.qml
                  src/quick/qml/KetAttachmentChip.qml
                  src/quick/qml/KetMeter.qml
                  src/quick/qml/KetPopoverDivider.qml
                  src/quick/qml/KetPopoverText.qml
                  src/quick/qml/KetPopover.qml
                  src/quick/qml/KetLightboxButton.qml
                  src/quick/qml/KetLightboxThumb.qml
                  src/quick/qml/KetLightbox.qml
        RESOURCE_PREFIX /qt/qml
    )
    foreach(file KetButton KetIconButton KetBadge KetTag KetIcon KetTextField KetSelect
                 KetToggleIndicator KetCheckbox KetRadio KetChoice KetFieldRow KetShadow
                 KetBanner KetToast KetDialog KetMenu KetMenuItem KetMenuSeparator KetMenuSection KetToolTip
                 KetNavItem KetKbd KetSegmentedControl KetTabBar KetTabButton KetProgressBar
                 KetActivityIndicator KetEmptyState KetConversationRow KetComposer
                 KetReadingText KetUserMessage KetAssistantMessage KetDayDivider KetActivityRow
                 KetMarkdownText KetCodeBlock KetMarkdown KetMentionPicker KetUsageRing KetCommandPalette KetCommandResult KetFileRow KetChangedFiles KetDiffView KetTaskRow KetTaskBar KetUpdateCard KetUpdatePill KetThemeCard KetAccentSwatch KetPaneHeader KetBreadcrumb KetStatusBar KetStatusItem KetRouteTabs KetGroupHeader KetTreeItem KetSidebarRepositoryHeader KetSidebarTeamBranch KetSidebarSectionLabel KetSidebarRow KetSidebarTreeGuide KetAvatar KetStepSlider KetSlider KetGallery KetActivityGroup KetQuestionCard KetSuggestion KetAttachmentChip KetMeter KetPopoverDivider KetPopoverText KetPopover KetLightboxButton KetLightboxThumb KetLightbox)
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

if(KETQT_INSTALL)
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
                  src/tokens/KetProfile.h src/tokens/KetConversationRecipes.h
                  src/tokens/KetSidebarTreeProfile.h src/tokens/KetSidebarTreeRecipes.h
                  src/tokens/KetRichRecipes.h src/tokens/KetMarkdown.h src/tokens/KetSyntax.h
                  src/tokens/KetSurfaceRecipes.h
                  src/tokens/KetControlRecipes.h
                  src/tokens/KetChatRecipes.h
                  src/tokens/KetLightboxRecipes.h
                  src/icons/KetIcons.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/KetQt)
    if(KETQT_BUILD_WIDGETS)
        install(FILES src/widgets/KetStyle.h src/widgets/KetButton.h src/widgets/KetBadge.h
                      src/widgets/KetTag.h src/widgets/KetIcon.h src/widgets/KetTextField.h
                      src/widgets/KetSelect.h src/widgets/KetToggle.h src/widgets/KetChoice.h
                      src/widgets/KetFieldRow.h src/widgets/KetBanner.h src/widgets/KetToast.h
                      src/widgets/KetDialog.h src/widgets/KetNavItem.h
                      src/widgets/KetSegmentedControl.h src/widgets/KetActivityIndicator.h
                      src/widgets/KetAnimationClock.h
                      src/widgets/KetEmptyState.h src/widgets/KetConversationRow.h
                      src/widgets/KetComposer.h src/widgets/KetTranscript.h
                      src/widgets/KetActivityRow.h src/widgets/KetMarkdownView.h
                      src/widgets/KetMentionPicker.h
                      src/widgets/KetThemeCard.h
                      src/widgets/KetUpdateCard.h
                      src/widgets/KetTaskBar.h
                      src/widgets/KetDiffView.h
                      src/widgets/KetChangedFiles.h
                      src/widgets/KetFileRow.h
                      src/widgets/KetCommandPalette.h
                      src/widgets/KetChrome.h
                      src/widgets/KetRouteTabs.h
                      src/widgets/KetTreeItem.h src/widgets/KetSidebarTree.h
                      src/widgets/KetAvatar.h
                      src/widgets/KetStepSlider.h
                      src/widgets/KetGallery.h
                      src/widgets/KetActivityGroup.h
                      src/widgets/KetQuestionCard.h
                      src/widgets/KetSuggestion.h
                      src/widgets/KetAttachmentChip.h
                      src/widgets/KetMeter.h
                      src/widgets/KetPopover.h
                      src/widgets/KetLightboxButton.h
                      src/widgets/KetLightboxThumb.h
                      src/widgets/KetLightbox.h
                      src/widgets/KetPrint.h
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
endif()
