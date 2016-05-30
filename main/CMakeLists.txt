set(SOURCES
    "src/main.c"
    )

add_executable(tray ${SOURCES})

target_link_libraries(tray traylang-lib)

set_property(TARGET tray PROPERTY C_STANDARD 99)
