set(CMAKE_BINARY_DIR "../bin")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(veracryptrunner_SRCS main.cpp runner/VeracryptRunner.cpp config/VeracryptConfigItem.cpp runner/editdialog.cpp)

ki18n_wrap_ui(veracryptrunner_SRCS runner/editdialog.ui)
qt5_add_dbus_adaptor(veracryptrunner_SRCS "/usr/share/dbus-1/interfaces/kf5_org.kde.krunner1.xml" runner/VeracryptRunner.h VeracryptRunner)

add_executable(veracryptrunner ${veracryptrunner_SRCS})
target_link_libraries(veracryptrunner
        Qt5::DBus
        KF5::Runner
        KF5::Notifications
        KF5::I18n
        )

install(FILES runner/veracryptrunner2.desktop DESTINATION ~/.local/share/kservices5/)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    install(TARGETS veracryptrunner DESTINATION ~/.config/autostart-scripts/)
endif()

add_subdirectory(config)