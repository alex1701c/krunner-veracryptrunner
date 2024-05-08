cmake_minimum_required(VERSION 3.16)
project(veracryptrunner)

set(QT_MIN_VERSION 5.15.0)
set(KF_MIN_VERSION 5.91.0)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_MODULE_PATH})
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMGenerateDBusServiceFile)
include(ECMDeprecationSettings)
include(FeatureSummary)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)
ecm_set_disabled_deprecation_versions(
    QT ${QT_MIN_VERSION}
    KF ${KF_MIN_VERSION}
)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS DBus Widgets)
find_package(KF5 ${KF_MIN_VERSION} REQUIRED COMPONENTS Config Notifications I18n)

add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
