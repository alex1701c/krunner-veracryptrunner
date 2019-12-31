find_package(Qt5 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets Core Quick QuickWidgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Service Runner TextWidgets ConfigWidgets PlasmaQuick)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(kcm_krunner_veracryptrunner_SRCS veracryptrunner_config.cpp ../ui/VeracryptConfigItem.cpp)

ki18n_wrap_ui(kcm_krunner_veracryptrunner_SRCS ../ui/veracryptrunner_config_item.ui)
add_library(kcm_krunner_veracryptrunner2 MODULE ${kcm_krunner_veracryptrunner_SRCS})
target_link_libraries(kcm_krunner_veracryptrunner2
        Qt5::Core
        Qt5::Gui
        KF5::CoreAddons
        KF5::ConfigCore
        KF5::I18n
        KF5::ConfigWidgets
        KF5::Runner
        )

install(TARGETS kcm_krunner_veracryptrunner2 DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-runner-veracryptrunner2_config.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
