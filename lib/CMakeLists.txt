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
    "include/ast.h"
    "include/ast-print.h"
    "include/interpreter.h"
    "include/interpreter_funcs.h"
    "include/interpreter_gc.h"
    "include/interpreter_stackframe.h"
    "include/object.h.h"
    "include/parser.h.h"
    "include/stdlib/list.h"
    "include/stdlib/math.h"
    "include/stdlib/print.h"
   )
set(SOURCES
    "src/interpreter.c"
    "src/interpreter_funcs.c"
    "src/interpreter_gc.c"
    "src/interpreter_stackframe.c"
    "src/object.c"
    "src/ast.c"
    "src/ast-print.c"
    "src/stdlib/math.c"
    "src/stdlib/list.c"
    "src/stdlib/print.c"
    )
add_library(traylang-lib SHARED
    ${SOURCES}
    ${BISON_TRLParser_OUTPUTS}
    ${FLEX_TRLLexer_OUTPUTS}
)

find_package(BCLib REQUIRED)
include_directories("${BCLIB_INCLUDE_DIRS}")
target_link_libraries(traylang-lib bclib)
