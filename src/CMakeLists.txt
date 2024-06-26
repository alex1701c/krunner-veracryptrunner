set(CMAKE_BINARY_DIR "../bin")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(veracryptrunner_SRCS main.cpp runner/VeracryptRunner.cpp ui/VeracryptConfigItem.cpp ui/editdialog.cpp cli/CLIMain.cpp)

ki18n_wrap_ui(veracryptrunner_SRCS ui/veracryptrunner_config_item.ui)
qt_add_dbus_adaptor(veracryptrunner_SRCS runner/org.kde.krunner1.xml runner/VeracryptRunner.h VeracryptRunner)

add_executable(veracryptrunner ${veracryptrunner_SRCS})
target_link_libraries(veracryptrunner
        Qt::DBus
        Qt::Widgets
        KF${QT_MAJOR_VERSION}::ConfigCore
        KF${QT_MAJOR_VERSION}::Notifications
        KF${QT_MAJOR_VERSION}::I18n
        )

install(FILES runner/veracryptrunner2.desktop DESTINATION "${KDE_INSTALL_DATAROOTDIR}/krunner/dbusplugins")
install(TARGETS veracryptrunner DESTINATION ${KDE_INSTALL_LIBEXECDIR})
ecm_generate_dbus_service_file(NAME net.veracryptrunner2
                               EXECUTABLE ${KDE_INSTALL_FULL_LIBEXECDIR}/veracryptrunner
                               DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})

add_subdirectory(config)
