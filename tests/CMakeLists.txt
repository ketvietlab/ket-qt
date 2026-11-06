# SPDX-License-Identifier: MPL-2.0
function(ketqt_test name)
    cmake_parse_arguments(ARG "" "" "LIBS" ${ARGN})
    qt_add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE Qt6::Test ${ARG_LIBS})
    ketqt_warnings(${name})
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endfunction()

ketqt_test(tst_tokens LIBS KetQt::Tokens)
ketqt_test(tst_icons LIBS KetQt::Icons)
if(KETQT_BUILD_WIDGETS)
    ketqt_test(tst_widgets LIBS KetQt::Widgets)
    ketqt_test(tst_rich_widgets LIBS KetQt::Widgets)
    ketqt_test(tst_chat LIBS KetQt::Widgets)
    ketqt_test(tst_controls LIBS KetQt::Widgets)
    ketqt_test(tst_surfaces LIBS KetQt::Widgets)
endif()
if(KETQT_BUILD_QUICK)
    ketqt_test(tst_quick LIBS KetQt::Quick KetQt::QuickPlugin)
    ketqt_test(tst_quick_chrome LIBS KetQt::Quick KetQt::QuickPlugin)
endif()
if(KETQT_BUILD_WIDGETS AND KETQT_BUILD_QUICK)
    ketqt_test(tst_lightbox LIBS KetQt::Widgets KetQt::Quick KetQt::QuickPlugin)
endif()

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    # The committed token table must match a fresh run of the generator.
    add_test(NAME generated_tokens_current
        COMMAND Python3::Interpreter "${PROJECT_SOURCE_DIR}/tools/generate_tokens.py" --check)
    # The committed Két Symbol qrc and name table must match the vendored icons.
    add_test(NAME generated_icons_current
        COMMAND Python3::Interpreter "${PROJECT_SOURCE_DIR}/tools/generate_icons.py" --check)
endif()
