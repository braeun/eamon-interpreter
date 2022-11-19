
BISON_TARGET(eamon_parser runtime/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
            DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h
#             COMPILE_FLAGS "-v"
             )

FLEX_TARGET(eamon_lexer runtime/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
            COMPILE_FLAGS "-i -I -L -d"
            )

set(HEADERS
  aboutdialog.h
  defines.h
  diskselectdialog.h
  eamons.h
  errormessagesdialog.h
  mainwindow.h
  optionsdialog.h
  palettefactory.h
  qtcolorbutton.h
  screen.h
  disk/abstractdisk.h
  disk/dosdisk.h
  disk/extractionutils.h
  editor/codeeditor.h
  editor/editorwidget.h
  editor/editorwindow.h
  editor/syntaxhighlighter.h
  editor/linenumberarea.h
  nlohmann/json.h
  nlohmann/json_fwd.h
  runtime/assembler.h
  runtime/compilerdata.h
  runtime/disassembler.h
  runtime/errors.h
  runtime/function.h
  runtime/op.h
  runtime/variable.h
  runtime/address.h
  runtime/constant.h
  runtime/executable.h
  runtime/inputstream.h
  runtime/library.h
  runtime/memory.h
  runtime/outputstream.h
  runtime/stack.h
  runtime/symbol.h
  runtime/type.h
  runtime/value.h
  runtime/vm.h
  runtime/compiler.h
  runtime/diskfile.h
  runtime/scanner.h
  runtime/vmthread.h
  )

add_executable(eamon
  main.cpp
  aboutdialog.cpp
  aboutdialog.ui
  diskselectdialog.cpp
  diskselectdialog.ui
  eamons.cpp
  errormessagesdialog.cpp
  errormessagesdialog.ui
  eamon.qrc
  mainwindow.cpp
  mainwindow.ui
  optionsdialog.cpp
  optionsdialog.ui
  palettefactory.cpp
  qtcolorbutton.cpp
  screen.cpp
  disk/abstractdisk.cpp
  disk/dosdisk.cpp
  disk/extractionutils.cpp
  editor/codeeditor.cpp
  editor/editorwidget.cpp
  editor/editorwidget.ui
  editor/editorwindow.cpp
  editor/editorwindow.ui
  editor/syntaxhighlighter.cpp
  editor/linenumberarea.cpp
  runtime/assembler.cpp
  runtime/compilerdata.cpp
  runtime/disassembler.cpp
  runtime/errors.cpp
  runtime/function.cpp
  runtime/op.cpp
  runtime/variable.cpp
  runtime/address.cpp
  runtime/constant.cpp
  runtime/executable.cpp
  runtime/inputstream.cpp
  runtime/library.cpp
  runtime/memory.cpp
  runtime/outputstream.cpp
  runtime/stack.cpp
  runtime/symbol.cpp
  runtime/type.cpp
  runtime/value.cpp
  runtime/vm.cpp
  runtime/compiler.cpp
  runtime/diskfile.cpp
  runtime/scanner.cpp
  runtime/vmthread.cpp
  ${BISON_eamon_parser_OUTPUTS}
  ${FLEX_eamon_lexer_OUTPUTS}

  ${HEADERS}
  )

target_compile_definitions(eamon PRIVATE ${EAMON_DEF})

target_include_directories(eamon
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FLEX_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
  )

target_link_libraries(eamon
  Qt5::Xml
  Qt5::Widgets
  )
