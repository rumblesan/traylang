include_directories(include)

set(TEST_SOURCES
    "src/test_interpreter.c"
    "src/test_interpreter_gc.c"
    "src/test_traylang.c"
    "src/test_ast.c"
    )

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable("${TEST_NAME}" ${TEST_FILE})
    target_link_libraries("${TEST_NAME}" traylang)
    add_test("${TEST_NAME}" "${TEST_NAME}")
endforeach(TEST_FILE)

