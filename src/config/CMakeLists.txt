set(kcm_krunner_veracryptrunner_SRCS veracryptrunner_config.cpp ../ui/VeracryptConfigItem.cpp)

ki18n_wrap_ui(kcm_krunner_veracryptrunner_SRCS ../ui/veracryptrunner_config_item.ui)
ki18n_wrap_ui(kcm_krunner_veracryptrunner_SRCS veracryptrunner_config.ui)
add_library(kcm_krunner_veracryptrunner2 MODULE ${kcm_krunner_veracryptrunner_SRCS})
target_link_libraries(kcm_krunner_veracryptrunner2
    Qt::Gui
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::ConfigCore
    KF${QT_MAJOR_VERSION}::KCMUtils
)

install(TARGETS kcm_krunner_veracryptrunner2 DESTINATION ${KDE_INSTALL_PLUGINDIR})
