set(SOURCES
    "src/main.c"
    )

add_executable(tray ${SOURCES})

target_link_libraries(tray traylang)

set_property(TARGET tray PROPERTY C_STANDARD 99)

install(TARGETS tray
    EXPORT TrayLangTargets
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
