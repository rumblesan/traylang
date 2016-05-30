include_directories(include)

set(TRAYLANG_TEST_SOURCES
    "src/traylang/test_interpreter.c"
    )

foreach(TEST_FILE ${TRAYLANG_TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable("traylang_${TEST_NAME}" ${TEST_FILE})
    target_link_libraries("traylang_${TEST_NAME}" traylang-lib)
    add_test("traylang_${TEST_NAME}" "traylang_${TEST_NAME}")
endforeach(TEST_FILE)

