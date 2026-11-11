# SPDX-License-Identifier: MPL-2.0
qt_add_executable(ketqt_kitchensink
    main.cpp
    KitchenSinkWindow.h KitchenSinkWindow.cpp
    KitchenSinkCommon.h KitchenSinkCommon.cpp
    FoundationPages.h FoundationPages.cpp
    ComponentPages.h ComponentPages.cpp
    FormPages.h FormPages.cpp
    OverlayPages.h OverlayPages.cpp
    NavigationPages.h NavigationPages.cpp
    ConversationPages.h ConversationPages.cpp
    RichPages.h RichPages.cpp
    ChatPages.h ChatPages.cpp
    ControlPages.h ControlPages.cpp
    SurfacePages.h SurfacePages.cpp
    LightboxPages.h LightboxPages.cpp
    SidebarTreePages.h SidebarTreePages.cpp
)
qt_add_qml_module(ketqt_kitchensink
    URI KetKitchenSink
    VERSION 1.0
    SOURCES KitchenSinkHost.h KitchenSinkHost.cpp
    QML_FILES qml/ButtonMatrix.qml qml/ButtonLive.qml qml/IconButtonMatrix.qml qml/IconButtonLive.qml
              qml/BadgeTones.qml qml/TagSamples.qml
              qml/IconGallery.qml qml/FormSamples.qml qml/OverlaySamples.qml qml/NavigationSamples.qml qml/ConversationSamples.qml qml/RichSamples.qml qml/ChatSamples.qml qml/ControlSamples.qml qml/SurfaceSamples.qml qml/LightboxSamples.qml qml/SidebarTreeSamples.qml
    RESOURCE_PREFIX /qt/qml
)
foreach(file ButtonMatrix ButtonLive IconButtonMatrix IconButtonLive BadgeTones TagSamples IconGallery FormSamples OverlaySamples NavigationSamples
             ConversationSamples RichSamples ChatSamples ControlSamples SurfaceSamples LightboxSamples SidebarTreeSamples)
    set_source_files_properties(qml/${file}.qml PROPERTIES QT_RESOURCE_ALIAS ${file}.qml)
endforeach()
# Synthetic screenshots for the gallery samples, drawn for the kitchensink.
qt_add_resources(ketqt_kitchensink kitchensink_images
    PREFIX /kitchensink
    FILES images/screenshot-sidebar.png images/screenshot-editor.png
          images/screenshot-settings.png images/screenshot-dialog.png
          images/screenshot-terminal.png
)
set_target_properties(ketqt_kitchensink PROPERTIES
    MACOSX_BUNDLE ON
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE_BUNDLE_NAME "KetQt Kitchensink"
    MACOSX_BUNDLE_GUI_IDENTIFIER vn.ketviet.ketqt.kitchensink
)
target_link_libraries(ketqt_kitchensink PRIVATE
    KetQt::Widgets KetQt::Quick KetQt::QuickPlugin Qt6::QuickWidgets)
ketqt_warnings(ketqt_kitchensink)

if(KETQT_BUILD_TESTS)
    # Every page builds and loads its QML without errors, in light and dark.
    add_test(NAME kitchensink_smoke COMMAND ketqt_kitchensink --smoke)
    set_tests_properties(kitchensink_smoke PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software")
endif()
