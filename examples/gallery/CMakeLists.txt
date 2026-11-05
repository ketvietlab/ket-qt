# SPDX-License-Identifier: MPL-2.0
qt_add_executable(ketqt_gallery main.cpp)
qt_add_qml_module(ketqt_gallery
    URI KetGallery
    VERSION 1.0
    QML_FILES Gallery.qml
    RESOURCE_PREFIX /qt/qml
)
target_link_libraries(ketqt_gallery PRIVATE KetQt::Widgets KetQt::Quick KetQt::QuickPlugin Qt6::QuickWidgets)
ketqt_warnings(ketqt_gallery)
