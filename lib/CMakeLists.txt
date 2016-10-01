include_directories(include)

find_package(BISON 3.0)
find_package(FLEX 2.5)

bison_target(TRLParser grammar/traylang.y ${CMAKE_CURRENT_BINARY_DIR}/y.tab.c)
flex_target(TRLLexer
    grammar/traylang.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
    COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/lex.yy.h
)
add_flex_bison_dependency(TRLLexer TRLParser)

set(HEADERS
    "include/traylang.h"
    "include/traylang_ffi.h"
    "include/interpreter.h"
    "include/object.h"
    "include/ast.h"
   )
set(SOURCES
    "src/traylang.c"
    "src/traylang_ffi.c"
    "src/interpreter_funcs.c"
    "src/interpreter_gc.c"
    "src/interpreter_stackframe.c"
    "src/interpreter_interpret.c"
    "src/object_funcs.c"
    "src/ast.c"
    "src/ast-print.c"
    "src/stdlib/math.c"
    "src/stdlib/list.c"
    "src/stdlib/print.c"
    )

add_library(traylang STATIC
    ${SOURCES}
    ${BISON_TRLParser_OUTPUTS}
    ${FLEX_TRLLexer_OUTPUTS}
)

find_package(BCLib REQUIRED)
include_directories("${BCLIB_INCLUDE_DIRS}")
message( STATUS "bclib include dir:    " ${BCLIB_INCLUDE_DIRS})
target_link_libraries(traylang bclib)


set_target_properties(traylang PROPERTIES
    PUBLIC_HEADER "${HEADERS}")

install(TARGETS traylang
    DESTINATION lib
    EXPORT TrayLangTargets
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/traylang"
    COMPONENT dev
    )

